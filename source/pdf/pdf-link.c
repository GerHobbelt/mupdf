// Copyright (C) 2004-2023 Artifex Software, Inc.
//
// This file is part of MuPDF.
//
// MuPDF is free software: you can redistribute it and/or modify it under the
// terms of the GNU Affero General Public License as published by the Free
// Software Foundation, either version 3 of the License, or (at your option)
// any later version.
//
// MuPDF is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more
// details.
//
// You should have received a copy of the GNU Affero General Public License
// along with MuPDF. If not, see <https://www.gnu.org/licenses/agpl-3.0.en.html>
//
// Alternative licensing terms are available from the licensor.
// For commercial licensing, see <https://www.artifex.com/> or contact
// Artifex Software, Inc., 39 Mesa Street, Suite 108A, San Francisco,
// CA 94129, USA, for further information.

#include "mupdf/fitz.h"
#include "mupdf/pdf.h"
#include "pdf-annot-imp.h"

#include <string.h>
#include <math.h>

#if FZ_ENABLE_PDF

static pdf_obj *
resolve_dest_rec(fz_context *ctx, pdf_document *doc, pdf_obj *dest, int depth)
{
	if (depth > 10) /* Arbitrary to avoid infinite recursion */
		return NULL;

	if (pdf_is_name(ctx, dest) || pdf_is_string(ctx, dest))
	{
		dest = pdf_lookup_dest(ctx, doc, dest);
		dest = resolve_dest_rec(ctx, doc, dest, depth+1);
		return dest;
	}

	else if (pdf_is_array(ctx, dest))
	{
		return dest;
	}

	else if (pdf_is_dict(ctx, dest))
	{
		dest = pdf_dict_get(ctx, dest, PDF_NAME(D));
		return resolve_dest_rec(ctx, doc, dest, depth+1);
	}

	else if (pdf_is_indirect(ctx, dest))
		return dest;

	return NULL;
}

static pdf_obj *
resolve_dest(fz_context *ctx, pdf_document *doc, pdf_obj *dest)
{
	return resolve_dest_rec(ctx, doc, dest, 0);
}

char *
pdf_parse_link_dest_to_file(fz_context *ctx, pdf_document *doc, const char *file, int is_url, pdf_obj *dest)
{
	fz_link_dest destination = fz_make_link_dest_none();
	const char *name = NULL;
	char *uri;

	if (file && pdf_is_name(ctx, dest))
		name = pdf_to_name(ctx, dest);
	else if (file && pdf_is_string(ctx, dest))
		name = pdf_to_text_string(ctx, dest, NULL);
	else
	{
		pdf_obj *arg1, *arg2, *arg3, *arg4;
		float arg1v, arg2v, arg3v, arg4v;
		pdf_obj *pageobj, *typeobj;
		fz_matrix page_ctm;
		fz_rect rect;
		int pageno;
		fz_point p;

		if (!file)
			dest = resolve_dest(ctx, doc, dest);

		if (!pdf_is_array(ctx, dest) || pdf_array_len(ctx, dest) < 1)
		{
			fz_warn(ctx, "invalid link destination");
			return NULL;
		}

		if (file)
		{
			pageno = pdf_array_get_int(ctx, dest, 0);
			pageobj = NULL;
		}
		else
		{
			pageobj = pdf_array_get(ctx, dest, 0);
			if (pdf_is_int(ctx, pageobj))
			{
				pageno = pdf_to_int(ctx, pageobj);
				pageobj = pdf_lookup_page_obj(ctx, doc, pageno);
			}
			else
				pageno = pdf_lookup_page_number(ctx, doc, pageobj);
			pageno = fz_clampi(pageno, 0, pdf_count_pages(ctx, doc) - 1);
		}
		destination.loc.page = pageno;

		typeobj = pdf_array_get(ctx, dest, 1);
		if (typeobj == PDF_NAME(XYZ))
			destination.type = FZ_LINK_DEST_XYZ;
		else if (typeobj == PDF_NAME(Fit))
			destination.type = FZ_LINK_DEST_FIT;
		else if (typeobj == PDF_NAME(FitH))
			destination.type = FZ_LINK_DEST_FIT_H;
		else if (typeobj == PDF_NAME(FitV))
			destination.type = FZ_LINK_DEST_FIT_V;
		else if (typeobj == PDF_NAME(FitR))
			destination.type = FZ_LINK_DEST_FIT_R;
		else if (typeobj == PDF_NAME(FitB))
			destination.type = FZ_LINK_DEST_FIT_B;
		else if (typeobj == PDF_NAME(FitBH))
			destination.type = FZ_LINK_DEST_FIT_BH;
		else if (typeobj == PDF_NAME(FitBV))
			destination.type = FZ_LINK_DEST_FIT_BV;
		else
			destination.type = FZ_LINK_DEST_XYZ;

		arg1 = pdf_array_get(ctx, dest, 2);
		arg2 = pdf_array_get(ctx, dest, 3);
		arg3 = pdf_array_get(ctx, dest, 4);
		arg4 = pdf_array_get(ctx, dest, 5);

		arg1v = pdf_to_real(ctx, arg1);
		arg2v = pdf_to_real(ctx, arg2);
		arg3v = pdf_to_real(ctx, arg3);
		arg4v = pdf_to_real(ctx, arg4);

		if (pageobj)
			pdf_page_obj_transform(ctx, pageobj, NULL, &page_ctm);
		else
			page_ctm = fz_identity;

		switch (destination.type)
		{
		default:
		case FZ_LINK_DEST_FIT:
		case FZ_LINK_DEST_FIT_B:
			break;
		case FZ_LINK_DEST_FIT_H:
		case FZ_LINK_DEST_FIT_BH:
			p = fz_transform_point_xy(0, arg1v, page_ctm);
			destination.y = arg1 ? p.y : NAN;
			break;
		case FZ_LINK_DEST_FIT_V:
		case FZ_LINK_DEST_FIT_BV:
			p = fz_transform_point_xy(arg1v, 0, page_ctm);
			destination.x = arg1 ? p.x : NAN;
			break;
		case FZ_LINK_DEST_XYZ:
			p = fz_transform_point_xy(arg1v, arg2v, page_ctm);
			destination.x = arg1 ? p.x : NAN;
			destination.y = arg2 ? p.y : NAN;
			destination.zoom = arg3 ? (arg3v > 0 ? (arg3v * 100) : 100) : NAN;
			break;
		case FZ_LINK_DEST_FIT_R:
			rect.x0 = arg1v;
			rect.y0 = arg2v;
			rect.x1 = arg3v;
			rect.y1 = arg4v;
			fz_transform_rect(rect, page_ctm);
			destination.x = rect.x0;
			destination.y = rect.y0;
			destination.w = rect.x1 - rect.x0;
			destination.h = rect.y1 - rect.y0;
			break;
		}
	}

	uri = pdf_format_remote_link_uri(ctx, doc, file, is_url, name, destination);
	return uri;
}

char *
pdf_parse_link_dest(fz_context *ctx, pdf_document *doc, pdf_obj *dest)
{
	return pdf_parse_link_dest_to_file(ctx, doc, NULL, 0, dest);
}

static char *
pdf_parse_file_spec(fz_context *ctx, pdf_document *doc, pdf_obj *file_spec, pdf_obj *dest)
{
	pdf_obj *filename = NULL;
	const char *file = NULL;
	int is_url;

	if (pdf_is_string(ctx, file_spec))
		filename = file_spec;

	if (pdf_is_dict(ctx, file_spec)) {
#ifdef _WIN32
		filename = pdf_dict_get(ctx, file_spec, PDF_NAME(DOS));
#else
		filename = pdf_dict_get(ctx, file_spec, PDF_NAME(Unix));
#endif
		if (!filename)
			filename = pdf_dict_geta(ctx, file_spec, PDF_NAME(UF), PDF_NAME(F));
	}

	if (!pdf_is_string(ctx, filename))
	{
		fz_warn(ctx, "cannot parse file specification");
		return NULL;
	}

	is_url = pdf_dict_get(ctx, file_spec, PDF_NAME(FS)) == PDF_NAME(URL);
	file = pdf_to_text_string(ctx, filename, NULL);
	return pdf_parse_link_dest_to_file(ctx, doc, file, is_url, dest);
}

pdf_obj *
pdf_embedded_file_stream(fz_context *ctx, pdf_obj *fs)
{
	pdf_obj *ef = pdf_dict_get(ctx, fs, PDF_NAME(EF));
	pdf_obj *file = pdf_dict_get(ctx, ef, PDF_NAME(UF));
	if (!file) file = pdf_dict_get(ctx, ef, PDF_NAME(F));
	if (!file) file = pdf_dict_get(ctx, ef, PDF_NAME(Unix));
	if (!file) file = pdf_dict_get(ctx, ef, PDF_NAME(DOS));
	if (!file) file = pdf_dict_get(ctx, ef, PDF_NAME(Mac));
	return file;
}

int
pdf_is_embedded_file(fz_context *ctx, pdf_obj *fs)
{
	return pdf_is_stream(ctx, pdf_embedded_file_stream(ctx, fs));
}

void
pdf_get_embedded_file_params(fz_context *ctx, pdf_obj *fs, pdf_embedded_file_params *out)
{
	pdf_obj *file, *params, *filename, *subtype;

	if (!pdf_is_embedded_file(ctx, fs) || !out)
		return;

	file = pdf_embedded_file_stream(ctx, fs);
	params = pdf_dict_get(ctx, file, PDF_NAME(Params));

	filename = pdf_dict_get(ctx, fs, PDF_NAME(UF));
	if (!filename) filename = pdf_dict_get(ctx, fs, PDF_NAME(F));
	if (!filename) filename = pdf_dict_get(ctx, fs, PDF_NAME(Unix));
	if (!filename) filename = pdf_dict_get(ctx, fs, PDF_NAME(DOS));
	if (!filename) filename = pdf_dict_get(ctx, fs, PDF_NAME(Mac));
	out->filename = pdf_to_text_string(ctx, filename, NULL);

	subtype = pdf_dict_get(ctx, file, PDF_NAME(Subtype));
	if (!subtype)
		out->mimetype = "application/octet-stream";
	else
		out->mimetype = pdf_to_name(ctx, subtype);
	out->size = pdf_dict_get_int(ctx, params, PDF_NAME(Size));
	out->created = pdf_dict_get_date(ctx, params, PDF_NAME(CreationDate));
	out->modified = pdf_dict_get_date(ctx, params, PDF_NAME(ModDate));
}

fz_buffer *
pdf_load_embedded_file_contents(fz_context *ctx, pdf_obj *fs)
{
	if (!pdf_is_embedded_file(ctx, fs))
		return NULL;
	return pdf_load_stream(ctx, pdf_embedded_file_stream(ctx, fs));
}

int
pdf_verify_embedded_file_checksum(fz_context *ctx, pdf_obj *fs)
{
	unsigned char digest[16];
	pdf_obj *file, *params;
	const char *checksum;
	fz_buffer *contents;
	int valid = 0;
	size_t len;

	if (!pdf_is_embedded_file(ctx, fs))
		return 1;

	file = pdf_embedded_file_stream(ctx, fs);
	params = pdf_dict_get(ctx, file, PDF_NAME(Params));
	checksum = pdf_dict_get_string(ctx, params, PDF_NAME(CheckSum), &len);
	if (!checksum || strlen(checksum) == 0)
		return 1;

	valid = 0;

	fz_try(ctx)
	{
		file = pdf_embedded_file_stream(ctx, fs);
		contents = pdf_load_stream(ctx, file);
		fz_md5_buffer(ctx, contents, digest);
		if (len == nelem(digest) && !memcmp(digest, checksum, nelem(digest)))
			valid = 1;
	}
	fz_always(ctx)
		fz_drop_buffer(ctx, contents);
	fz_catch(ctx)
		fz_rethrow(ctx);

	return valid;
}

static const char *
pdf_guess_mime_type_from_file_name(fz_context *ctx, const char *filename)
{
	const char *ext = strrchr(filename, '.');
	if (ext)
	{
		if (!fz_strcasecmp(ext, ".pdf")) return "application/pdf";
		if (!fz_strcasecmp(ext, ".xml")) return "application/xml";
		if (!fz_strcasecmp(ext, ".zip")) return "application/zip";
		if (!fz_strcasecmp(ext, ".tar")) return "application/x-tar";

		/* Text */
		if (!fz_strcasecmp(ext, ".txt")) return "text/plain";
		if (!fz_strcasecmp(ext, ".rtf")) return "application/rtf";
		if (!fz_strcasecmp(ext, ".csv")) return "text/csv";
		if (!fz_strcasecmp(ext, ".html")) return "text/html";
		if (!fz_strcasecmp(ext, ".htm")) return "text/html";
		if (!fz_strcasecmp(ext, ".css")) return "text/css";

		/* Office */
		if (!fz_strcasecmp(ext, ".doc")) return "application/msword";
		if (!fz_strcasecmp(ext, ".ppt")) return "application/vnd.ms-powerpoint";
		if (!fz_strcasecmp(ext, ".xls")) return "application/vnd.ms-excel";
		if (!fz_strcasecmp(ext, ".docx")) return "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
		if (!fz_strcasecmp(ext, ".pptx")) return "application/vnd.openxmlformats-officedocument.presentationml.presentation";
		if (!fz_strcasecmp(ext, ".xlsx")) return "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
		if (!fz_strcasecmp(ext, ".odt")) return "application/vnd.oasis.opendocument.text";
		if (!fz_strcasecmp(ext, ".odp")) return "application/vnd.oasis.opendocument.presentation";
		if (!fz_strcasecmp(ext, ".ods")) return "application/vnd.oasis.opendocument.spreadsheet";

		/* Image */
		if (!fz_strcasecmp(ext, ".bmp")) return "image/bmp";
		if (!fz_strcasecmp(ext, ".gif")) return "image/gif";
		if (!fz_strcasecmp(ext, ".jpeg")) return "image/jpeg";
		if (!fz_strcasecmp(ext, ".jpg")) return "image/jpeg";
		if (!fz_strcasecmp(ext, ".png")) return "image/png";
		if (!fz_strcasecmp(ext, ".svg")) return "image/svg+xml";
		if (!fz_strcasecmp(ext, ".tif")) return "image/tiff";
		if (!fz_strcasecmp(ext, ".tiff")) return "image/tiff";

		/* Sound */
		if (!fz_strcasecmp(ext, ".flac")) return "audio/flac";
		if (!fz_strcasecmp(ext, ".mp3")) return "audio/mpeg";
		if (!fz_strcasecmp(ext, ".ogg")) return "audio/ogg";
		if (!fz_strcasecmp(ext, ".wav")) return "audio/wav";

		/* Movie */
		if (!fz_strcasecmp(ext, ".avi")) return "video/x-msvideo";
		if (!fz_strcasecmp(ext, ".mov")) return "video/quicktime";
		if (!fz_strcasecmp(ext, ".mp4")) return "video/mp4";
		if (!fz_strcasecmp(ext, ".webm")) return "video/webm";
	}
	return "application/octet-stream";
}

pdf_obj *
pdf_add_embedded_file(fz_context *ctx, pdf_document *doc,
	const char *filename, const char *mimetype, fz_buffer *contents,
	int64_t created, int64_t modified, int add_checksum)
{
	pdf_obj *file = NULL;
	pdf_obj *filespec = NULL;
	pdf_obj *params = NULL;

	fz_var(file);
	fz_var(filespec);

	if (!mimetype)
		mimetype = pdf_guess_mime_type_from_file_name(ctx, filename);

	pdf_begin_operation(ctx, doc, "Embed file");
	fz_try(ctx)
	{
		file = pdf_add_new_dict(ctx, doc, 3);
		pdf_dict_put(ctx, file, PDF_NAME(Type), PDF_NAME(EmbeddedFile));
		pdf_dict_put_name(ctx, file, PDF_NAME(Subtype), mimetype);
		pdf_update_stream(ctx, doc, file, contents, 0);

		params = pdf_dict_put_dict(ctx, file, PDF_NAME(Params), 4);
		pdf_dict_put_int(ctx, params, PDF_NAME(Size), fz_buffer_storage(ctx, contents, NULL));
		if (created >= 0)
				pdf_dict_put_date(ctx, params, PDF_NAME(CreationDate), created);
		if (modified >= 0)
				pdf_dict_put_date(ctx, params, PDF_NAME(ModDate), modified);
		if (add_checksum)
		{
			unsigned char digest[16];
			fz_md5_buffer(ctx, contents, digest);
				pdf_dict_put_string(ctx, params, PDF_NAME(CheckSum), (const char *) digest, nelem(digest));
		}

		filespec = pdf_new_filespec(ctx, doc, filename, file);
		pdf_end_operation(ctx, doc);
	}
	fz_always(ctx)
	{
		pdf_drop_obj(ctx, file);
	}
	fz_catch(ctx)
	{
		pdf_abandon_operation(ctx, doc);
		pdf_drop_obj(ctx, filespec);
		fz_rethrow(ctx);
	}

	return filespec;
}

char *
pdf_parse_link_action(fz_context *ctx, pdf_document *doc, pdf_obj *action, int pagenum)
{
	pdf_obj *obj, *dest, *file_spec;

	if (!action)
		return NULL;

	obj = pdf_dict_get(ctx, action, PDF_NAME(S));
	if (pdf_name_eq(ctx, PDF_NAME(GoTo), obj))
	{
		dest = pdf_dict_get(ctx, action, PDF_NAME(D));
		return pdf_parse_link_dest(ctx, doc, dest);
	}
	else if (pdf_name_eq(ctx, PDF_NAME(URI), obj))
	{
		/* URI entries are ASCII strings */
		const char *uri = pdf_dict_get_text_string(ctx, action, PDF_NAME(URI));
		if (!fz_is_external_link(ctx, uri))
		{
			pdf_obj *uri_base_obj = pdf_dict_getp(ctx, pdf_trailer(ctx, doc), "Root/URI/Base");
			const char *uri_base = uri_base_obj ? pdf_to_text_string(ctx, uri_base_obj, NULL) : "file://";
			char *new_uri = Memento_label(fz_malloc(ctx, strlen(uri_base) + strlen(uri) + 1), "link_action");
			strcpy(new_uri, uri_base);
			strcat(new_uri, uri);
			return new_uri;
		}
		return fz_strdup(ctx, uri);
	}
	else if (pdf_name_eq(ctx, PDF_NAME(Launch), obj))
	{
		file_spec = pdf_dict_get(ctx, action, PDF_NAME(F));
		return pdf_parse_file_spec(ctx, doc, file_spec, NULL);
	}
	else if (pdf_name_eq(ctx, PDF_NAME(GoToR), obj))
	{
		dest = pdf_dict_get(ctx, action, PDF_NAME(D));
		file_spec = pdf_dict_get(ctx, action, PDF_NAME(F));
		return pdf_parse_file_spec(ctx, doc, file_spec, dest);
	}
	else if (pdf_name_eq(ctx, PDF_NAME(Named), obj))
	{
		dest = pdf_dict_get(ctx, action, PDF_NAME(N));

		if (pdf_name_eq(ctx, PDF_NAME(FirstPage), dest))
			pagenum = 0;
		else if (pdf_name_eq(ctx, PDF_NAME(LastPage), dest))
			pagenum = pdf_count_pages(ctx, doc) - 1;
		else if (pdf_name_eq(ctx, PDF_NAME(PrevPage), dest) && pagenum >= 0)
		{
			if (pagenum > 0)
				pagenum--;
		}
		else if (pdf_name_eq(ctx, PDF_NAME(NextPage), dest) && pagenum >= 0)
		{
			if (pagenum < pdf_count_pages(ctx, doc) - 1)
				pagenum++;
		}
		else
			return NULL;

		return fz_asprintf(ctx, "#page=%d", pagenum + 1);
	}

	return NULL;
}

static void pdf_drop_link_imp(fz_context *ctx, fz_link *link)
{
	pdf_drop_obj(ctx, ((pdf_link *) link)->obj);
}

static void pdf_set_link_rect(fz_context *ctx, fz_link *link_, fz_rect rect)
{
	pdf_link *link = (pdf_link *) link_;
	if (link == NULL)
		return;

	if (!link->page)
		fz_throw(ctx, FZ_ERROR_GENERIC, "link not bound to a page");

	pdf_begin_operation(ctx, link->page->doc, "Set link rectangle");

	fz_try(ctx)
	{
		pdf_dict_put_rect(ctx, link->obj, PDF_NAME(Rect), rect);
		link->super.rect = rect;
		pdf_end_operation(ctx, link->page->doc);
	}
	fz_catch(ctx)
	{
		pdf_abandon_operation(ctx, link->page->doc);
		fz_rethrow(ctx);
	}
}

static void pdf_set_link_uri(fz_context *ctx, fz_link *link_, const char *uri)
{
	pdf_link *link = (pdf_link *) link_;
	if (link == NULL)
		return;

	if (!link->page)
		fz_throw(ctx, FZ_ERROR_GENERIC, "link not bound to a page");

	pdf_begin_operation(ctx, link->page->doc, "Set link uri");

	fz_try(ctx)
	{
		pdf_dict_put_drop(ctx, link->obj, PDF_NAME(A),
				pdf_new_action_from_link(ctx, link->page->doc, uri));
		fz_free(ctx, link->super.uri);
		link->super.uri = fz_strdup(ctx, uri);
		pdf_end_operation(ctx, link->page->doc);
	}
	fz_catch(ctx)
	{
		pdf_abandon_operation(ctx, link->page->doc);
		fz_rethrow(ctx);
	}
}

fz_link *pdf_new_link(fz_context *ctx, pdf_page *page, fz_rect rect, int count, fz_quad* quads, const char *uri, pdf_obj *obj)
{
	pdf_link *link = fz_new_derived_link(ctx, pdf_link, rect, count, quads, uri);
	link->super.drop = (fz_link_drop_link_fn*) pdf_drop_link_imp;
	link->super.set_rect_fn = pdf_set_link_rect;
	link->super.set_uri_fn = pdf_set_link_uri;
	link->page = page; /* only borrowed, as the page owns the link */
	link->obj = pdf_keep_obj(ctx, obj);
	return &link->super;
}

static fz_link *
pdf_load_link(fz_context *ctx, pdf_document *doc, pdf_page *page, pdf_obj *dict, int pagenum, fz_matrix page_ctm)
{
	pdf_obj *action;
	pdf_obj *obj;
	fz_rect bbox;
	char *uri = NULL;
	fz_link *link = NULL;
	int count = 0;
	fz_quad *quads = NULL;
	int i;

	fz_var(uri);
	fz_var(quads);

	obj = pdf_dict_get(ctx, dict, PDF_NAME(Subtype));
	if (!pdf_name_eq(ctx, obj, PDF_NAME(Link)))
		return NULL;

	obj = pdf_dict_get(ctx, dict, PDF_NAME(Rect));
	if (!obj)
		return NULL;

	bbox = pdf_to_rect(ctx, obj);
	bbox = fz_transform_rect(bbox, page_ctm);

	fz_try(ctx)
	{
		obj = pdf_dict_get(ctx, dict, PDF_NAME(QuadPoints));
		if (obj)
		{
			count = pdf_array_len(ctx, obj) / 8;

			quads = fz_malloc(ctx, count * sizeof(fz_quad));
			for (i = 0; i < count; i++)
			{
				quads[i] = pdf_to_quad(ctx, obj, i);
				fz_transform_quad(quads[i], page_ctm);
				if (!fz_is_quad_inside_rect(quads[i], bbox))
				{
					fz_free(ctx, quads);
					quads = NULL;
					count = 0;
					break;
				}
			}
		}

		obj = pdf_dict_get(ctx, dict, PDF_NAME(Dest));
		if (obj)
			uri = pdf_parse_link_dest(ctx, doc, obj);
		else
		{
			action = pdf_dict_get(ctx, dict, PDF_NAME(A));
			/* fall back to additional action button's down/up action */
			if (!action)
				action = pdf_dict_geta(ctx, pdf_dict_get(ctx, dict, PDF_NAME(AA)), PDF_NAME(U), PDF_NAME(D));
			uri = pdf_parse_link_action(ctx, doc, action, pagenum);
		}

		if (uri)
			link = pdf_new_link(ctx, page, bbox, count, quads, uri, dict);
	}
	fz_always(ctx)
	{
		fz_free(ctx, uri);
		fz_free(ctx, quads);
	}
	fz_catch(ctx)
		fz_rethrow(ctx);

	return link;
}

fz_link *
pdf_load_link_annots(fz_context *ctx, pdf_document *doc, pdf_page *page, pdf_obj *annots, int pagenum, fz_matrix page_ctm)
{
	fz_link *link, *head, *tail;
	pdf_obj *obj;
	int i, n;

	head = tail = NULL;
	link = NULL;

	n = pdf_array_len(ctx, annots);
	for (i = 0; i < n; i++)
	{
		/* FIXME: Move the try/catch out of the loop for performance? */
		fz_try(ctx)
		{
			obj = pdf_array_get(ctx, annots, i);
			link = pdf_load_link(ctx, doc, page, obj, pagenum, page_ctm);
		}
		fz_catch(ctx)
		{
			fz_rethrow_if(ctx, FZ_ERROR_TRYLATER);
			link = NULL;
		}

		if (link)
		{
			if (!head)
				head = tail = link;
			else
			{
				tail->next = link;
				tail = link;
			}
		}
	}

	return head;
}

#define IS_ALPHA(c)	(((c) >= 'A' && (c) <= 'Z') || ((c) >= 'a' && (c) <= 'z'))
#define IS_DIGIT(c)	((c) >= '0' && (c) <= '9')
#define IS_DELIMITER(c)	((c) == '/' || (c) == '!' || (c) == '$' || (c) == '&' || (c) == '\'' || (c) == '(' || (c) == ')' || (c) == '*' || (c) == '+' || (c) == ',' || (c) == ';' || (c) == '=' || (c) == ':' || (c) == '@')

static char *
url_encode_string(fz_context *ctx, const char *s, int isfile)
{
	const char *sp;
	char *encoded, *dp;
	size_t len;

	if (s == NULL)
		return NULL;

	len = 0;
	sp = s;
	while (*sp)
	{
		if (IS_ALPHA(*sp) || IS_DIGIT(*sp) || *sp == '-' || *sp == '.' || *sp == '_' || *sp == '~')
			len++;
		else if (isfile && IS_DELIMITER(*sp))
			len++;
		else
			len += 3;
		sp++;
	}

	encoded = fz_malloc(ctx, len + 1);
	dp = encoded;
	sp = s;
	while (*sp)
	{
		if (IS_ALPHA(*sp) || IS_DIGIT(*sp) || *sp == '-' || *sp == '.' || *sp == '_' || *sp == '~')
			*dp++ = *sp++;
		else if (isfile && IS_DELIMITER(*sp))
			*dp++ = *sp++;
		else
		{
			*dp++ = '%';
			*dp++ = "0123456789abcdef"[(*sp >> 4) & 0xf];
			*dp++ = "0123456789abcdef"[(*sp >> 0) & 0xf];
			sp++;
		}
	}
	*dp = 0;

	return encoded;
}

/* See explanation of this format in pdf-outline.c */
char *
pdf_format_remote_link_uri(fz_context *ctx, pdf_document *doc, const char *file, int is_url, const char *name, fz_link_dest dest)
{
	const char *schema;
	char *windows_file = NULL;
	char *encoded_name = NULL;
	char *encoded_file = NULL;
	char *uri = NULL;

	fz_var(encoded_name);
	fz_var(encoded_file);
	fz_var(windows_file);

	fz_try(ctx)
	{
		name = encoded_name = url_encode_string(ctx, name, 0);
		file = encoded_file = url_encode_string(ctx, file, 1);

#ifdef _WIN32
		/* Fix up the drive letter (change "/C/Documents/Foo" to "C:/Documents/Foo") */
		if (!is_url && file && file[0] == '/' && (('A' <= file[1] && file[1] <= 'Z') || ('a' <= file[1] && file[1] <= 'z')) && file[2] == '/')
			file = windows_file = fz_asprintf(ctx, "/%c:%s", file[1], file+2);
#endif

		if (file)
		{
			if (file[0] == '/')
				schema = "file://";
			else
				schema = "file:";
		}
		else
		{
			schema = "";
			file = "";
		}

		if (name)
			uri = fz_asprintf(ctx, "%s%s#%s", schema, file, name);
		else
		{
			int pageno = dest.loc.page < 0 ? 1 : dest.loc.page + 1;
			switch (dest.type)
			{
			default:
			case FZ_LINK_DEST_FIT:
				uri = fz_asprintf(ctx, "%s%s#page=%d&view=Fit", schema, file, pageno);
				break;
			case FZ_LINK_DEST_FIT_B:
				uri = fz_asprintf(ctx, "%s%s#page=%d&view=FitB", schema, file, pageno);
				break;
			case FZ_LINK_DEST_FIT_H:
				if (isnan(dest.y))
					uri = fz_asprintf(ctx, "%s%s#page=%d&view=FitH", schema, file, pageno);
				else
					uri = fz_asprintf(ctx, "%s%s#page=%d&view=FitH,%g", schema, file, pageno, dest.y);
				break;
			case FZ_LINK_DEST_FIT_BH:
				if (isnan(dest.y))
					uri = fz_asprintf(ctx, "%s%s#page=%d&view=FitBH", schema, file, pageno);
				else
					uri = fz_asprintf(ctx, "%s%s#page=%d&view=FitBH,%g", schema, file, pageno, dest.y);
				break;
			case FZ_LINK_DEST_FIT_V:
				if (isnan(dest.x))
					uri = fz_asprintf(ctx, "%s%s#page=%d&view=FitV", schema, file, pageno);
				else
					uri = fz_asprintf(ctx, "%s%s#page=%d&view=FitV,%g", schema, file, pageno, dest.x);
				break;
			case FZ_LINK_DEST_FIT_BV:
				if (isnan(dest.x))
					uri = fz_asprintf(ctx, "%s%s#page=%d&view=FitBV", schema, file, pageno);
				else
					uri = fz_asprintf(ctx, "%s%s#page=%d&view=FitBV,%g", schema, file, pageno, dest.x);
				break;
			case FZ_LINK_DEST_XYZ:
				if (!isnan(dest.zoom) && !isnan(dest.x) && !isnan(dest.y))
					uri = fz_asprintf(ctx, "%s%s#page=%d&zoom=%g,%g,%g", schema, file, pageno, dest.zoom, dest.x, dest.y);
				else if (!isnan(dest.zoom) && !isnan(dest.x) && isnan(dest.y))
					uri = fz_asprintf(ctx, "%s%s#page=%d&zoom=%g,%g,nan", schema, file, pageno, dest.zoom, dest.x);
				else if (!isnan(dest.zoom) && isnan(dest.x) && !isnan(dest.y))
					uri = fz_asprintf(ctx, "%s%s#page=%d&zoom=%g,nan,%g", schema, file, pageno, dest.zoom, dest.y);
				else if (!isnan(dest.zoom) && isnan(dest.x) && isnan(dest.y))
					uri = fz_asprintf(ctx, "%s%s#page=%d&zoom=%g,nan,nan", schema, file, pageno, dest.zoom);
				else if (isnan(dest.zoom) && !isnan(dest.x) && !isnan(dest.y))
					uri = fz_asprintf(ctx, "%s%s#page=%d&zoom=nan,%g,%g", schema, file, pageno, dest.x, dest.y);
				else if (isnan(dest.zoom) && !isnan(dest.x) && isnan(dest.y))
					uri = fz_asprintf(ctx, "%s%s#page=%d&zoom=nan,%g,nan", schema, file, pageno, dest.x);
				else if (isnan(dest.zoom) && isnan(dest.x) && !isnan(dest.y))
					uri = fz_asprintf(ctx, "%s%s#page=%d&zoom=nan,nan,%g", schema, file, pageno, dest.y);
				else
					uri = fz_asprintf(ctx, "%s%s#page=%d&zoom=nan,nan,nan", schema, file, pageno);
				break;
			case FZ_LINK_DEST_FIT_R:
				uri = fz_asprintf(ctx, "%s%s#page=%d&viewrect=%g,%g,%g,%g", schema, file, pageno,
					dest.x, dest.y, dest.w, dest.h);
				break;
			}
		}
	}
	fz_always(ctx)
	{
		fz_free(ctx, encoded_name);
		fz_free(ctx, encoded_file);
		fz_free(ctx, windows_file);
	}
	fz_catch(ctx)
		fz_rethrow(ctx);

	return uri;
}

char *
pdf_format_link_uri(fz_context *ctx, pdf_document *doc, fz_link_dest dest)
{
	return pdf_format_remote_link_uri(ctx, doc, NULL, 0, NULL, dest);
}

static float next_float(const char *str, int eatcomma, char **end)
{
	if (eatcomma && *str == ',')
		++str;
	return fz_strtof(str, end);
}

#define IS_HEX(c)	(((c) >= '0' && (c) <= '9') || ((c) >= 'a' && (c) <= 'f') || ((c) >= 'A' && (c) <= 'F'))

static char *uri_decode_string(fz_context *ctx, const char *s, const char *end)
{
	char *decoded, *dp;
	const char *sp;
	size_t len;

	if (end)
		len = end - s;
	else
	{
		len = strlen(s);
		end = s + len;
	}

	decoded = fz_malloc(ctx, len + 1);

	dp = decoded;
	sp = s;

	while (sp < end)
	{
		if (sp[0] == '%' && IS_HEX(sp[1]) && IS_HEX(sp[2]))
		{
			int val = 0;

			if (sp[1] >= '0' && sp[1] <= '9')
				val |= (sp[1] - '0') << 4;
			else if (sp[1] >= 'a' && sp[1] <= 'f')
				val |= (sp[1] - 'a') << 4;
			else if (sp[1] >= 'A' && sp[1] <= 'F')
				val |= (sp[1] - 'A') << 4;
			if (sp[2] >= '0' && sp[2] <= '9')
				val |= sp[2] - '0';
			else if (sp[2] >= 'a' && sp[2] <= 'f')
				val |= sp[2] - 'a';
			else if (sp[2] >= 'A' && sp[2] <= 'F')
				val |= sp[2] - 'A';

			*dp++ = val;
			sp += 3;
		}
		else
		{
			*dp++ = *sp++;
		}
	}
	*dp = 0;

	return decoded;

}

fz_link_dest
pdf_parse_link_uri(fz_context *ctx, const char *uri, char **file, char **name)
{
	fz_link_dest dest = fz_make_link_dest_none();
	char *page_s, *view_s, *rect_s, *zoom_s, *nameddest_s;

	if (uri && !strncmp(uri, "file:", 5))
	{
		const char *path = strncmp(uri, "file://", 7) == 0 ? uri + 7 : uri + 5;
		const char *frag = strchr(uri, '#');
		size_t len;

		if (frag)
			len = frag - path + 1;
		else
			len = strlen(path) + 1;

		if (file)
		{
			*file = fz_malloc(ctx, len);
			fz_strlcpy(*file, path, len);
		}

		uri = frag;
	}

	if (uri && uri[0] == '#')
		uri++;

	page_s = uri ? strstr(uri, "page=") : NULL;
	nameddest_s = uri ? strstr(uri, "nameddest=") : NULL;

	if (page_s)
	{
		dest.loc.page = fz_atoi(page_s+5) - 1;

		rect_s = strstr(uri, "viewrect=");
		zoom_s = strstr(uri, "zoom=");
		view_s = strstr(uri, "view=");

		if (rect_s)
		{
			rect_s += 9;
			dest.type = FZ_LINK_DEST_FIT_R;
			dest.x = next_float(rect_s, 0, &rect_s);
			dest.y = next_float(rect_s, 1, &rect_s);
			dest.w = next_float(rect_s, 1, &rect_s);
			dest.h = next_float(rect_s, 1, &rect_s);
		}
		else if (zoom_s)
		{
			zoom_s += 5;
			dest.type = FZ_LINK_DEST_XYZ;
			dest.zoom = next_float(zoom_s, 0, &zoom_s);
			dest.x = next_float(zoom_s, 1, &zoom_s);
			dest.y = next_float(zoom_s, 1, &zoom_s);
			if (dest.zoom <= 0 || isinf(dest.zoom))
				dest.zoom = 100;
		}
		else if (view_s)
		{
			view_s += 5;
			if (!fz_strncasecmp(view_s, "FitH", 4))
			{
				view_s += 4;
				dest.type = FZ_LINK_DEST_FIT_H;
				dest.y = strchr(view_s, ',') ? next_float(view_s, 1, &view_s) : NAN;
			}
			else if (!fz_strncasecmp(view_s, "FitBH", 5))
			{
				view_s += 5;
				dest.type = FZ_LINK_DEST_FIT_BH;
				dest.y = strchr(view_s, ',') ? next_float(view_s, 1, &view_s) : NAN;
			}
			else if (!fz_strncasecmp(view_s, "FitV", 4))
			{
				view_s += 4;
				dest.type = FZ_LINK_DEST_FIT_V;
				dest.x = strchr(view_s, ',') ? next_float(view_s, 1, &view_s) : NAN;
			}
			else if (!fz_strncasecmp(view_s, "FitBV", 5))
			{
				view_s += 5;
				dest.type = FZ_LINK_DEST_FIT_BV;
				dest.x = strchr(view_s, ',') ? next_float(view_s, 1, &view_s) : NAN;
			}
			else if (!fz_strncasecmp(view_s, "FitB", 4))
			{
				dest.type = FZ_LINK_DEST_FIT_B;
			}
			else if (!fz_strncasecmp(view_s, "Fit", 3))
			{
				dest.type = FZ_LINK_DEST_FIT;
			}
		}
	}
	else if (nameddest_s)
	{
		const char *ampersand = strchr(nameddest_s, '&');
		if (ampersand)
			*name = uri_decode_string(ctx, nameddest_s + 10, ampersand);
		else
			*name = uri_decode_string(ctx, nameddest_s + 10, NULL);
	}
	else if (uri)
	{
		*name = uri_decode_string(ctx, uri, NULL);
	}
	else
	{
		dest.loc.page = 0;
	}

	return dest;
}

pdf_obj *
pdf_new_action_from_link(fz_context *ctx, pdf_document *doc, const char *uri)
{
	pdf_obj *action = pdf_new_dict(ctx, doc, 2);
	char *file = NULL;

	fz_var(file);

	fz_try(ctx)
	{
		if (!strncmp(uri, "file:", 5))
		{
			pdf_dict_put(ctx, action, PDF_NAME(S), PDF_NAME(GoToR));
			pdf_dict_put_drop(ctx, action, PDF_NAME(D),
				pdf_new_destination_from_link(ctx, doc, uri, &file));
			if (file)
				pdf_dict_put_drop(ctx, action, PDF_NAME(F),
					pdf_new_filespec(ctx, doc, file, NULL));
		}
		else if (fz_is_external_link(ctx, uri))
		{
			pdf_dict_put(ctx, action, PDF_NAME(S), PDF_NAME(URI));
			pdf_dict_put_text_string(ctx, action, PDF_NAME(URI), uri);
		}
		else
		{
			pdf_dict_put(ctx, action, PDF_NAME(S), PDF_NAME(GoTo));
			pdf_dict_put_drop(ctx, action, PDF_NAME(D),
				pdf_new_destination_from_link(ctx, doc, uri, NULL));
		}
	}
	fz_catch(ctx)
	{
		fz_free(ctx, file);
		pdf_drop_obj(ctx, action);
		fz_rethrow(ctx);
	}

	return action;
}


pdf_obj *pdf_new_filespec(fz_context *ctx, pdf_document *doc, const char *filename, pdf_obj *embedded_file)
{
	pdf_obj *filespec = NULL;
	char *asciiname = NULL;
	const char *s;
	size_t len, i;

	fz_var(asciiname);

	fz_try(ctx)
	{
		len = strlen(filename) + 1;
		asciiname = fz_malloc(ctx, len);

		for (i = 0, s = filename; *s && i + 1 < len; ++i)
		{
			int c;
			s += fz_chartorune_unsafe(&c, s);
			asciiname[i] = (c >= 32 && c <= 126) ? c : '_';
		}
		asciiname[i] = 0;

		filespec = pdf_add_new_dict(ctx, doc, 4);
		pdf_dict_put(ctx, filespec, PDF_NAME(Type), PDF_NAME(Filespec));
		pdf_dict_put_text_string(ctx, filespec, PDF_NAME(F), asciiname);
		pdf_dict_put_text_string(ctx, filespec, PDF_NAME(UF), filename);
		if (embedded_file)
		{
			pdf_obj *ef = pdf_dict_put_dict(ctx, filespec, PDF_NAME(EF), 1);
			pdf_dict_put(ctx, ef, PDF_NAME(F), embedded_file);
		}
	}
	fz_always(ctx)
		fz_free(ctx, asciiname);
	fz_catch(ctx)
		fz_rethrow(ctx);



	return filespec;
}

#endif
