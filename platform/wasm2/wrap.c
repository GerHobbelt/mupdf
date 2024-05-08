// Copyright (C) 2004-2021 Artifex Software, Inc.
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
// Artifex Software, Inc., 1305 Grant Avenue - Suite 200, Novato,
// CA 94945, U.S.A., +1(415)492-9861, for further information.

#include "emscripten.h"
#include "mupdf/fitz.h"
#include "mupdf/pdf.h"

// These exist to silence VSCode Intellisense and should not impact code.
#ifndef EMSCRIPTEN_KEEPALIVE
#define EMSCRIPTEN_KEEPALIVE
#define EM_ASM(...)
#endif


static fz_context *ctx;

__attribute__((noinline)) void
wasm_rethrow(fz_context *ctx)
{
	if (fz_caught(ctx) == FZ_ERROR_TRYLATER)
		EM_ASM({ throw new libmupdf.TryLaterError("operation in progress"); });
	else
		EM_ASM({ throw new Error(UTF8ToString($0)); }, fz_caught_message(ctx));
}

EMSCRIPTEN_KEEPALIVE
void initContext(void)
{
	ctx = fz_new_context(NULL, NULL, 100<<20);
	if (!ctx)
		EM_ASM({ throw new Error("Cannot create MuPDF context!"); });
	fz_register_document_handlers(ctx);
}

EMSCRIPTEN_KEEPALIVE
fz_document *openDocumentFromBuffer(char *magic, unsigned char *data, size_t len)
{
	fz_document *document = NULL;
	fz_buffer *buf = NULL;
	fz_stream *stm = NULL;

	fz_var(buf);
	fz_var(stm);

	/* NOTE: We take ownership of input data! */

	fz_try(ctx)
	{
		buf = fz_new_buffer_from_data(ctx, data, len);
		stm = fz_open_buffer(ctx, buf);
		document = fz_open_document_with_stream(ctx, magic, stm);
	}
	fz_always(ctx)
	{
		fz_drop_stream(ctx, stm);
		fz_drop_buffer(ctx, buf);
	}
	fz_catch(ctx)
	{
		fz_free(ctx, data);
		wasm_rethrow(ctx);
	}
	return document;
}

// Function used for pdf-unlock tool
EMSCRIPTEN_KEEPALIVE
void writeDocument()
{

	char *infile = "test_1.pdf";
	char *outfile = "test_2.pdf";
	char *password = "";
	char *argv = "";	

	pdf_write_options opts = pdf_default_write_options;
	opts.do_encrypt = PDF_ENCRYPT_NONE;

	pdf_clean_file(ctx, infile, outfile, password, &opts, 0, argv);
}

EMSCRIPTEN_KEEPALIVE
void freeDocument(fz_document *doc)
{
	fz_drop_document(ctx, doc);
}

EMSCRIPTEN_KEEPALIVE
int countPages(fz_document *doc)
{
	int n = 1;
	fz_try(ctx)
		n = fz_count_pages(ctx, doc);
	fz_catch(ctx)
		wasm_rethrow(ctx);
	return n;
}

static fz_page *lastPage = NULL;

static void loadPage(fz_document *doc, int number)
{
	static fz_document *lastPageDoc = NULL;
	static int lastPageNumber = -1;
	if (lastPageNumber != number || lastPageDoc != doc)
	{
		if (lastPage)
		{
			fz_drop_page(ctx, lastPage);
			lastPage = NULL;
			lastPageDoc = NULL;
			lastPageNumber = -1;
		}
		lastPage = fz_load_page(ctx, doc, number-1);
		lastPageDoc = doc;
		lastPageNumber = number;
	}
}


int count_stext_page_letters(fz_context *ctx, fz_stext_page *page)
{
	fz_stext_block *block;
	fz_stext_line *line;
	fz_stext_char *ch;
	char utf[10];
	int i, n;
	int ct = 0;
	int nnmlCt = 0;
	int ctrCt = 0;
	int imageCt = 0;

	for (block = page->first_block; block; block = block->next)
	{
		if (block->type == FZ_STEXT_BLOCK_TEXT)
		{
			for (line = block->u.t.first_line; line; line = line->next)
			{
				for (ch = line->first_char; ch; ch = ch->next)
				{
					// This excludes whitespace, control chars, etc.
					if (ch->c >= 33 && ch->c <= 127) {
						nnmlCt++;
						ct++;
					// Characters 0-31 are control characters, which are uncommon in legitimate text but commom in invalid encodings,
					// which often arbitrarily map glyphs to codes starting at 0.
					// Character 65533 is the replacement character, which is an explicit indication that the character is unknown.
					} else if (ch->c < 32 || ch->c == 65533) {
						ctrCt++;
						ct-=5;
					}
					
				}
			}
		} else {
			imageCt++;
		}
	}

	// printf("Normal characters: %d\nControl characters: %d\nTotal score: %d\nImage count: %d\n", nnmlCt, ctrCt, ct, imageCt);

	return ct;
}


EMSCRIPTEN_KEEPALIVE
int checkNativeText(fz_document *doc, int extract_text)
{
	static unsigned char *data = NULL;
	fz_rect mediabox;
	fz_device *dev = NULL;

	float zoom;
	float rotation = 0;

	fz_stext_page *stext_page = NULL;
	
	fz_output *out = NULL;

	fz_page *page;

	fz_matrix ctm;
	fz_rect tmediabox;

	if (extract_text) {
		char *output = "/download.txt";
		out = fz_new_output_with_path(ctx, output, 0);
	}

	int letterCountTotal = 0;
	int letterCountVis = 0;
	
	int pageCountVisText = 0;
	int pageCountTotalText = 0;
	int pageCount = fz_count_pages(ctx, doc);

	fz_cookie cookie = {0};

	for (int i=0; i<=(pageCount - 1); i++) {

		fz_try(ctx)
		{
			page = fz_load_page(ctx, doc, i);
			mediabox = fz_bound_page(ctx, page);

			const int dpi = 72;
			zoom = dpi / 72;
			ctm = fz_pre_scale(fz_rotate(rotation), zoom, zoom);

			// fz_stext_options stext_options;
			fz_stext_options stext_options = { FZ_STEXT_INHIBIT_SPACES };

			tmediabox = fz_transform_rect(mediabox, ctm);


			// Calculate total number of letters on the page
			stext_page = fz_new_stext_page(ctx, tmediabox);
			dev = fz_new_stext_device(ctx, stext_page, &stext_options);
			cookie.skip_text_invis = 0;
			fz_run_page(ctx, page, dev, ctm, &cookie);
			fz_close_device(ctx, dev);
			fz_drop_device(ctx, dev);
			dev = NULL;

			int letterCountTotalI = count_stext_page_letters(ctx, stext_page);

			letterCountTotal = letterCountTotal + letterCountTotalI;

			if (letterCountTotalI >= 100) {
				pageCountTotalText++;
			}

			if (extract_text) {
				fz_print_stext_page_as_text(ctx, out, stext_page);
			}

			fz_drop_stext_page(ctx, stext_page);
			stext_page = NULL;

			// Calculate number of visible letters on the page
			stext_page = fz_new_stext_page(ctx, tmediabox);
			dev = fz_new_stext_device(ctx, stext_page, &stext_options);
			cookie.skip_text_invis = 1;
			fz_run_page(ctx, page, dev, ctm, &cookie);

			int letterCountVisI = count_stext_page_letters(ctx, stext_page);

			if (letterCountVisI) {
				pageCountVisText++;
			}
			
			letterCountVis = letterCountVis + letterCountVisI;

		}
		fz_always(ctx)
		{
			fz_drop_page(ctx, page);
			fz_close_device(ctx, dev);
			fz_drop_device(ctx, dev);
			fz_drop_stext_page(ctx, stext_page);
		}
		fz_catch(ctx)
		{
			fz_rethrow(ctx);
		}

	}

	if (extract_text) {
		fz_close_output(ctx, out);
		fz_drop_output(ctx, out);
	}

	// Text native
    if (letterCountTotal >= pageCount * 100 && letterCountVis >= letterCountTotal * 0.9 && pageCountVisText >= pageCount / 2) {
		// printf("Native text\n");
		// printf("pageCountVisText: %d\npageCount: %d\n", pageCountVisText, pageCount);
        return 0;
	// Image + OCR text
    } else if (letterCountTotal >= pageCount * 100 && pageCountTotalText >= pageCount / 2) {
		// printf("Image + OCR text\n");
        return 1;
	// Image native
    } else {
		// printf("Image native\n");
        return 2;
    }
}


EMSCRIPTEN_KEEPALIVE
char *pageText(fz_document *doc, int pagenum, float dpi, int format, int skip_text_invis)
{
	static unsigned char *data = NULL;
	fz_rect mediabox;
	fz_device *dev = NULL;

	float zoom;
	float rotation = 0;

	fz_stext_page *stext_page = NULL;
	fz_buffer *buf = NULL;
	fz_output *out = NULL;

	fz_page *page;
	page = fz_load_page(ctx, doc, pagenum-1);

	mediabox = fz_bound_page(ctx, page);

	fz_matrix ctm;
	fz_rect tmediabox;


	fz_try(ctx)
	{
		zoom = dpi / 72;
		ctm = fz_pre_scale(fz_rotate(rotation), zoom, zoom);

		fz_stext_options stext_options;

		buf = fz_new_buffer(ctx, 0);
		out = fz_new_output_with_buffer(ctx, buf);


		tmediabox = fz_transform_rect(mediabox, ctm);
		stext_page = fz_new_stext_page(ctx, tmediabox);
		dev = fz_new_stext_device(ctx, stext_page, &stext_options);

		fz_cookie cookie = {0};
		if (skip_text_invis) {
			cookie.skip_text_invis = 1;
		} else {
			cookie.skip_text_invis = 0;
		}

		fz_run_page(ctx, page, dev, ctm, &cookie);
		fz_close_device(ctx, dev);
		fz_drop_device(ctx, dev);
		dev = NULL;

		// Format numbers are copied from mutool draw for consistency
		// See "mudraw.c"
		if (format == 0) {
			fz_print_stext_page_as_text(ctx, out, stext_page);
		} else if (format == 1) {
			fz_print_stext_page_as_html(ctx, out, stext_page, pagenum);
		} else if (format == 2) {
			fz_print_stext_page_as_xhtml(ctx, out, stext_page, pagenum);
		} else if (format == 3) {
			fz_print_stext_page_as_xml(ctx, out, stext_page, pagenum);
		} else {
			fz_print_stext_page_as_json(ctx, out, stext_page, pagenum);
		}

		fz_close_output(ctx, out);
		fz_terminate_buffer(ctx, buf);

		fz_buffer_extract(ctx, buf, &data);

	}
	fz_always(ctx)
	{
		fz_drop_page(ctx, page);
		fz_drop_device(ctx, dev);
		fz_drop_stext_page(ctx, stext_page);
		fz_drop_output(ctx, out);
		fz_drop_buffer(ctx, buf);
	}
	fz_catch(ctx)
	{
		fz_rethrow(ctx);
	}

	return (char*)data;
}

static fz_buffer *lastDrawBuffer = NULL;

EMSCRIPTEN_KEEPALIVE
void doDrawPageAsPNG(fz_document *doc, int number, float dpi, int skip_text)
{
	float zoom = dpi / 72;
	fz_pixmap *pix = NULL;

	fz_var(pix);

	if (lastDrawBuffer)
		fz_drop_buffer(ctx, lastDrawBuffer);
	lastDrawBuffer = NULL;

	fz_try(ctx)
	{
		loadPage(doc, number);
		pix = fz_new_pixmap_from_page(ctx, lastPage, fz_scale(zoom, zoom), fz_device_rgb(ctx), 0, skip_text);
		lastDrawBuffer = fz_new_buffer_from_pixmap_as_png(ctx, pix, fz_default_color_params);
	}
	fz_always(ctx)
		fz_drop_pixmap(ctx, pix);
	fz_catch(ctx)
		wasm_rethrow(ctx);
}

EMSCRIPTEN_KEEPALIVE
void doDrawPageAsPNGGray(fz_document *doc, int number, float dpi, int skip_text)
{
	float zoom = dpi / 72;
	fz_pixmap *pix = NULL;

	fz_var(pix);

	if (lastDrawBuffer)
		fz_drop_buffer(ctx, lastDrawBuffer);
	lastDrawBuffer = NULL;

	fz_try(ctx)
	{
		loadPage(doc, number);
		pix = fz_new_pixmap_from_page(ctx, lastPage, fz_scale(zoom, zoom), fz_device_gray(ctx), 0, skip_text);
		lastDrawBuffer = fz_new_buffer_from_pixmap_as_png(ctx, pix, fz_default_color_params);
	}
	fz_always(ctx)
		fz_drop_pixmap(ctx, pix);
	fz_catch(ctx)
		wasm_rethrow(ctx);
}

EMSCRIPTEN_KEEPALIVE
static void runpageOverlayPDF(int number, fz_document *doc, fz_document *doc2, fz_document_writer *out, int pagewidth, int pageheight, int skip_text)
{
	fz_rect mediabox;
	fz_rect mediabox2;
	fz_rect mediabox3;

	fz_page *page;
	fz_page *page2;
	fz_device *dev = NULL;

	// The coordinate system from the Javascript assumes a DPI of 300 by default.
	// 72 / 300 = 0.24
	// TODO: Make this a variable and compatible with other DPIs (it is not ALWAYS 300).
	fz_matrix text_matrix;
	fz_matrix immat;

	page = fz_load_page(ctx, doc, number);
	page2 = fz_load_page(ctx, doc2, number);

	fz_var(dev);

	fz_try(ctx)
	{
		mediabox = fz_bound_page(ctx, page);
		mediabox2 = fz_bound_page(ctx, page2);

		// Rectangle for new page
		mediabox3 = fz_bound_page(ctx, page);
		// Change width/height to user-specified values (if applicable)
		if (pagewidth > 0 && pageheight > 0) {
			// The pagewidth/pageheight arguments use the coordinate system from the overlay text. 
			// They need to be scaled to correspond to the background pdf. 
			mediabox3.x1 = pagewidth * mediabox.x1 / mediabox2.x1;
			mediabox3.y1 = pageheight * mediabox.y1 / mediabox2.y1;
		}

		dev = fz_begin_page(ctx, out, mediabox3);

		text_matrix = fz_make_matrix(mediabox.x1/mediabox2.x1, 0, 0, mediabox.x1/mediabox2.x1, 0, 0);

		fz_cookie cookie = {0};
		cookie.skip_text = skip_text;
		cookie.skip_text_invis = 1;

		fz_run_page(ctx, page, dev, fz_identity, &cookie);

		fz_run_page(ctx, page2, dev, text_matrix, NULL);

		fz_end_page(ctx, out);
	}
	fz_always(ctx)
	{
		fz_drop_page(ctx, page);
		fz_drop_page(ctx, page2);
	}
	fz_catch(ctx)
		fz_rethrow(ctx);
}

EMSCRIPTEN_KEEPALIVE
static void runpage(int number, fz_document *doc, fz_document_writer *out, int pagewidth, int pageheight)
{
	fz_rect mediabox3;

	fz_page *page;
	fz_device *dev = NULL;

	// The coordinate system from the Javascript assumes a DPI of 300 by default.
	// 72 / 300 = 0.24
	// TODO: Make this a variable and compatible with other DPIs (it is not ALWAYS 300).
	fz_matrix immat;

	page = fz_load_page(ctx, doc, number);

	fz_var(dev);

	fz_try(ctx)
	{
		// Rectangle for new page
		mediabox3 = fz_bound_page(ctx, page);
		// Change width/height to user-specified values (if applicable)
		if (pagewidth > 0 && pageheight > 0) {
			mediabox3.x1 = pagewidth;
			mediabox3.y1 = pageheight;
		}

		dev = fz_begin_page(ctx, out, mediabox3);


		fz_run_page(ctx, page, dev, fz_identity, NULL);


		fz_end_page(ctx, out);
	}
	fz_always(ctx)
	{
		fz_drop_page(ctx, page);
	}
	fz_catch(ctx)
		fz_rethrow(ctx);
}



EMSCRIPTEN_KEEPALIVE
void overlayPDFText(fz_document *doc, fz_document *doc2, int minpage, int maxpage, int pagewidth, int pageheight, int humanReadable, int skip_text)
{
	fz_document_writer *out;

	fz_rect mediabox;

	fz_page *page;
	fz_page *page2;

	char *output = "/download.pdf";

	char *optionsDefault = "compress";
	char *optionsHumanReadable = "ascii,decompress,pretty,compress-images,compress-fonts";
	char *options = humanReadable == 1 ? optionsHumanReadable : optionsDefault;

	out = fz_new_pdf_writer(ctx, output, options);

	// The coordinate system from the Javascript assumes a DPI of 300 by default.
	// 72 / 300 = 0.24
	// TODO: Make this a variable and compatible with other DPIs (it is not ALWAYS 300).
	fz_matrix text_matrix = fz_make_matrix(0.24, 0, 0, 0.24, 0, 0);

	int count = fz_count_pages(ctx, doc);

	if (maxpage == -1) {
		maxpage = count-1;
	}

	for (int i=minpage; i<=maxpage; i++) {

		runpageOverlayPDF(i, doc, doc2, out, pagewidth, pageheight, skip_text);

	}

	fz_close_document_writer(ctx, out);
	fz_drop_document_writer(ctx, out);

}

EMSCRIPTEN_KEEPALIVE
static void runpageOverlayImage(int number, fz_document *doc, fz_document_writer *out, int pagewidth, int pageheight, float angle)
{
	fz_rect mediabox;

	fz_page *page;
	fz_device *dev = NULL;

	fz_matrix immat;
	fz_matrix rotmat;
	fz_matrix transmat1;
	fz_matrix transmat2;

	fz_image *background_img;

	page = fz_load_page(ctx, doc, number);

	fz_var(dev);

	fz_try(ctx)
	{

		char path[128];
		sprintf(path, "%d.png", number);

		background_img = fz_new_image_from_file(ctx, path);

		mediabox = fz_bound_page(ctx, page);

		// Change width/height to user-specified values (if applicable)
		if (pagewidth > 0 && pageheight > 0) {
			mediabox.x1 = pagewidth;
			mediabox.y1 = pageheight;
		}

		dev = fz_begin_page(ctx, out, mediabox);

		// Create initial matrix for image using orientation and scale.
		immat = fz_image_orientation_matrix(ctx, background_img);
		immat = fz_post_scale(immat, mediabox.x1, mediabox.y1);

		// Rotate image around center point		
		rotmat = fz_rotate(angle);

		transmat1 = fz_translate(-background_img->w / 2, -background_img->h / 2);
		transmat2 = fz_translate(background_img->w / 2, background_img->h / 2);

		immat = fz_concat(immat, transmat1);
		immat = fz_concat(immat, rotmat);
		immat = fz_concat(immat, transmat2);

		fz_fill_image(ctx, dev, background_img, immat, 1.0f, fz_default_color_params);

		fz_cookie cookie = {0};
		cookie.skip_text = 0;

		fz_run_page(ctx, page, dev, fz_identity, &cookie);

		fz_end_page(ctx, out);

	}
	fz_always(ctx)
	{
		fz_drop_image(ctx, background_img);
		fz_drop_page(ctx, page);
	}
	fz_catch(ctx)
		fz_rethrow(ctx);
}

// Unlike for combining multiple PDF documents (overlayPDFText), when creating a PDF from images,
// a function is invoked from JavaScript for each individual page.
// This avoids an issue where many copies of the same images are created in different processes
// for (potentially) hundreds of different images. 
static fz_document_writer *out;
EMSCRIPTEN_KEEPALIVE
void overlayPDFTextImageStart(int humanReadable){
	char *output = "/download.pdf";

	char *optionsDefault = "compress";
	char *optionsHumanReadable = "ascii,decompress,pretty,compress-images,compress-fonts";
	char *options = humanReadable == 1 ? optionsHumanReadable : optionsDefault;
	out = fz_new_pdf_writer(ctx, output, options);
}

EMSCRIPTEN_KEEPALIVE
void overlayPDFTextImageAddPage(fz_document *doc, int i, int pagewidth, int pageheight, float angle){

	runpageOverlayImage(i, doc, out, pagewidth, pageheight, angle);
	
}

EMSCRIPTEN_KEEPALIVE
void overlayPDFTextImageEnd(){
	fz_close_document_writer(ctx, out);
	fz_drop_document_writer(ctx, out);
}


EMSCRIPTEN_KEEPALIVE
void overlayPDFTextImage(fz_document *doc, int minpage, int maxpage, int pagewidth, int pageheight, int humanReadable)
{
	fz_document_writer *out;

	fz_page *page;
	fz_device *dev = NULL;

	char *output = "/download.pdf";
	char *optionsDefault = "compress";
	char *optionsHumanReadable = "ascii,decompress,pretty,compress-images,compress-fonts";
	char *options = humanReadable == 1 ? optionsHumanReadable : optionsDefault;

	out = fz_new_pdf_writer(ctx, output, options);

	int count = fz_count_pages(ctx, doc);

	if (maxpage == -1) {
		maxpage = count-1;
	}

	for (int i=minpage; i<=maxpage; i++) {

		runpageOverlayImage(i, doc, out, pagewidth, pageheight, 0.0);

	}

	fz_close_document_writer(ctx, out);
	fz_drop_document_writer(ctx, out);

}

EMSCRIPTEN_KEEPALIVE
void writePDF(fz_document *doc, int minpage, int maxpage, int pagewidth, int pageheight, int humanReadable)
{
	fz_document_writer *out;

	fz_page *page;
	fz_device *dev = NULL;

	char *output = "/download.pdf";
	char *optionsDefault = "compress";
	char *optionsHumanReadable = "ascii,decompress,pretty,compress-images,compress-fonts";
	char *options = humanReadable == 1 ? optionsHumanReadable : optionsDefault;

	out = fz_new_pdf_writer(ctx, output, options);

	int count = fz_count_pages(ctx, doc);

	if (maxpage == -1) {
		maxpage = count-1;
	}

	for (int i=minpage; i<=maxpage; i++) {

		runpage(i, doc, out, pagewidth, pageheight);

	}

	fz_close_document_writer(ctx, out);
	fz_drop_document_writer(ctx, out);

}


EMSCRIPTEN_KEEPALIVE
unsigned char *getLastDrawData(void)
{
	return lastDrawBuffer ? lastDrawBuffer->data : 0;
}

EMSCRIPTEN_KEEPALIVE
int getLastDrawSize(void)
{
	return lastDrawBuffer ? lastDrawBuffer->len : 0;
}

static fz_irect pageBounds(fz_document *doc, int number, float dpi)
{
	fz_irect bbox = fz_empty_irect;
	fz_try(ctx)
	{
		loadPage(doc, number);
		bbox = fz_round_rect(fz_transform_rect(fz_bound_page(ctx, lastPage), fz_scale(dpi/72, dpi/72)));
	}
	fz_catch(ctx)
		wasm_rethrow(ctx);
	return bbox;
}

EMSCRIPTEN_KEEPALIVE
int pageWidth(fz_document *doc, int number, float dpi)
{
	fz_irect bbox = fz_empty_irect;
	fz_try(ctx)
	{
		loadPage(doc, number);
		bbox = pageBounds(doc, number, dpi);
	}
	fz_catch(ctx)
		wasm_rethrow(ctx);
	return bbox.x1 - bbox.x0;
}

EMSCRIPTEN_KEEPALIVE
int pageHeight(fz_document *doc, int number, float dpi)
{
	fz_irect bbox = fz_empty_irect;
	fz_try(ctx)
	{
		loadPage(doc, number);
		bbox = pageBounds(doc, number, dpi);
	}
	fz_catch(ctx)
		wasm_rethrow(ctx);
	return bbox.y1 - bbox.y0;
}

EMSCRIPTEN_KEEPALIVE
char *pageLinks(fz_document *doc, int number, float dpi)
{
	static unsigned char *data = NULL;
	fz_buffer *buf = NULL;
	fz_link *links = NULL;
	fz_link *link;

	fz_var(buf);
	fz_var(links);

	fz_free(ctx, data);
	data = NULL;

	fz_try(ctx)
	{
		loadPage(doc, number);

		links = fz_load_links(ctx, lastPage);

		buf = fz_new_buffer(ctx, 0);

		fz_append_string(ctx, buf, "[");
		for (link = links; link; link = link->next)
		{
			fz_irect bbox = fz_round_rect(fz_transform_rect(link->rect, fz_scale(dpi/72, dpi/72)));
			fz_append_string(ctx, buf, "{");
			fz_append_printf(ctx, buf, "%q:%d,", "x", bbox.x0);
			fz_append_printf(ctx, buf, "%q:%d,", "y", bbox.y0);
			fz_append_printf(ctx, buf, "%q:%d,", "w", bbox.x1 - bbox.x0);
			fz_append_printf(ctx, buf, "%q:%d,", "h", bbox.y1 - bbox.y0);
			if (fz_is_external_link(ctx, link->uri))
			{
				fz_append_printf(ctx, buf, "%q:%q", "href", link->uri);
			}
			else
			{
				fz_location link_loc = fz_resolve_link(ctx, doc, link->uri, NULL, NULL);
				int link_page = fz_page_number_from_location(ctx, doc, link_loc);
				fz_append_printf(ctx, buf, "%q:\"#page%d\"", "href", link_page+1);
			}
			fz_append_string(ctx, buf, "}");
			if (link->next)
				fz_append_string(ctx, buf, ",");
		}
		fz_append_string(ctx, buf, "]");
		fz_terminate_buffer(ctx, buf);

		fz_buffer_extract(ctx, buf, &data);
	}
	fz_always(ctx)
	{
		fz_drop_buffer(ctx, buf);
		fz_drop_link(ctx, links);
	}
	fz_catch(ctx)
	{
		wasm_rethrow(ctx);
	}

	return (char*)data;
}

EMSCRIPTEN_KEEPALIVE
char *search(fz_document *doc, int number, float dpi, const char *needle)
{
	static unsigned char *data = NULL;
	fz_buffer *buf = NULL;
	fz_quad hits[500];
	int i, n;

	fz_var(buf);

	fz_free(ctx, data);
	data = NULL;

	fz_try(ctx)
	{
		loadPage(doc, number);

		n = fz_search_page(ctx, lastPage, needle, NULL, hits, nelem(hits));

		buf = fz_new_buffer(ctx, 0);

		fz_append_string(ctx, buf, "[");
		for (i = 0; i < n; ++i)
		{
			fz_rect rect = fz_rect_from_quad(hits[i]);
			fz_irect bbox = fz_round_rect(fz_transform_rect(rect, fz_scale(dpi/72, dpi/72)));
			if (i > 0) fz_append_string(ctx, buf, ",");
			fz_append_printf(ctx, buf, "{%q:%d,", "x", bbox.x0);
			fz_append_printf(ctx, buf, "%q:%d,", "y", bbox.y0);
			fz_append_printf(ctx, buf, "%q:%d,", "w", bbox.x1 - bbox.x0);
			fz_append_printf(ctx, buf, "%q:%d}", "h", bbox.y1 - bbox.y0);
		}
		fz_append_string(ctx, buf, "]");
		fz_terminate_buffer(ctx, buf);

		fz_buffer_extract(ctx, buf, &data);
	}
	fz_always(ctx)
	{
		fz_drop_buffer(ctx, buf);
	}
	fz_catch(ctx)
	{
		wasm_rethrow(ctx);
	}

	return (char*)data;
}

EMSCRIPTEN_KEEPALIVE
char *documentTitle(fz_document *doc)
{
	static char buf[100], *result = NULL;
	fz_try(ctx)
	{
		if (fz_lookup_metadata(ctx, doc, FZ_META_INFO_TITLE, buf, sizeof buf) > 0)
			result = buf;
	}
	fz_catch(ctx)
		wasm_rethrow(ctx);
	return result;
}

EMSCRIPTEN_KEEPALIVE
fz_outline *loadOutline(fz_document *doc)
{
	fz_outline *outline = NULL;
	fz_var(outline);
	fz_try(ctx)
	{
		outline = fz_load_outline(ctx, doc);
	}
	fz_catch(ctx)
	{
		fz_drop_outline(ctx, outline);
		wasm_rethrow(ctx);
	}
	return outline;
}

EMSCRIPTEN_KEEPALIVE
void freeOutline(fz_outline *outline)
{
	fz_drop_outline(ctx, outline);
}

EMSCRIPTEN_KEEPALIVE
char *outlineTitle(fz_outline *node)
{
	return node->title;
}

EMSCRIPTEN_KEEPALIVE
int outlinePage(fz_document *doc, fz_outline *node)
{
	return fz_page_number_from_location(ctx, doc, node->page);
}

EMSCRIPTEN_KEEPALIVE
fz_outline *outlineDown(fz_outline *node)
{
	return node->down;
}

EMSCRIPTEN_KEEPALIVE
fz_outline *outlineNext(fz_outline *node)
{
	return node->next;
}
