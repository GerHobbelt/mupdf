#ifndef PDF_ANNOT_IMP_H
#define PDF_ANNOT_IMP_H

/*
	Map from a PDF name specifying an annotation line ending
	to an enumerated line ending value.
*/
enum pdf_line_ending pdf_line_ending_from_name(fz_context *ctx, pdf_obj *end);

/*
	Map from a (non-NULL, case sensitive) C string specifying
	an annotation line ending to an enumerated line ending value.
*/
enum pdf_line_ending pdf_line_ending_from_string(fz_context *ctx, const char *end);

/*
	Map from an enumerated line ending to a pdf name object that
	specifies it.
*/
pdf_obj *pdf_name_from_line_ending(fz_context *ctx, enum pdf_line_ending end);

/*
	Map from an enumerated line ending to a C string that specifies
	it.

	The caller must not free the returned string.
*/
const char *pdf_string_from_line_ending(fz_context *ctx, enum pdf_line_ending end);

/*
	Lookup needle in the nametree of the document given by which.

	The returned reference is borrowed, and should not be dropped,
	unless it is kept first.
*/
pdf_obj *pdf_lookup_name(fz_context *ctx, pdf_document *doc, pdf_obj *which, pdf_obj *needle);

/*
	Lookup needle in the given number tree.

	The returned reference is borrowed, and should not be dropped,
	unless it is kept first.
*/
pdf_obj *pdf_lookup_number(fz_context *ctx, pdf_obj *root, int needle);

/*
	Resolve a link within a document.
*/
int pdf_resolve_link(fz_context *ctx, pdf_document *doc, const char *uri, float *xp, float *yp);

/*
	Create transform to fit appearance stream to annotation Rect
*/
fz_matrix pdf_annot_transform(fz_context *ctx, pdf_annot *annot);

/*
	create a new annotation of the specified type on the
	specified page. The returned pdf_annot structure is owned by the
	page and does not need to be freed.
*/
pdf_annot *pdf_create_annot_raw(fz_context *ctx, pdf_page *page, enum pdf_annot_type type);

void pdf_dirty_annot(fz_context *ctx, pdf_annot *annot);

void pdf_update_signature_appearance(fz_context *ctx, pdf_annot *annot, const char *name, const char *text, const char *date);

fz_layout_block *pdf_layout_text_widget(fz_context *ctx, pdf_annot *annot);

const char *pdf_guess_mime_type_from_file_name(fz_context *ctx, const char *filename);
pdf_obj *pdf_embedded_file_stream(fz_context *ctx, pdf_obj *fs);

char *pdf_parse_link_dest(fz_context *ctx, pdf_document *doc, pdf_obj *obj);
char *pdf_parse_link_action(fz_context *ctx, pdf_document *doc, pdf_obj *obj, int pagenum);
pdf_obj *pdf_lookup_dest(fz_context *ctx, pdf_document *doc, pdf_obj *needle);
fz_link *pdf_load_link_annots(fz_context *ctx, pdf_document *, pdf_obj *annots, int pagenum, fz_matrix page_ctm);
void pdf_load_annots(fz_context *ctx, pdf_page *page, pdf_obj *annots);
void pdf_drop_annots(fz_context *ctx, pdf_annot *annot_list);
void pdf_drop_widgets(fz_context *ctx, pdf_widget *widget_list);

void pdf_annot_MK_BG(fz_context *ctx, pdf_annot *annot, int *n, float color[4]);
void pdf_annot_MK_BC(fz_context *ctx, pdf_annot *annot, int *n, float color[4]);
int pdf_annot_MK_BG_rgb(fz_context *ctx, pdf_annot *annot, float rgb[3]);
int pdf_annot_MK_BC_rgb(fz_context *ctx, pdf_annot *annot, float rgb[3]);

struct pdf_annot
{
	int refs;

	pdf_page *page;
	pdf_obj *obj;

	pdf_obj *ap;

	int is_hot;
	int is_active;

	int needs_new_ap;
	int has_new_ap;
	int ignore_trigger_events;

	pdf_annot *next;
};

#endif
