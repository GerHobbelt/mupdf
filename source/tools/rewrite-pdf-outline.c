#include "mupdf/fitz.h"
#include "mupdf/pdf.h"

int main(int argc, char **argv)
{
	fz_context *ctx;
	fz_document *doc;
	fz_outline *outline;

	ctx = fz_new_context(0,0,0);
	fz_register_document_handlers(ctx);

	doc = fz_open_document(ctx, "pdfref17.pdf");
	outline = fz_load_outline(ctx, doc);
	fz_save_outline(ctx, doc, outline);
	fz_drop_outline(ctx, outline);

	pdf_save_document(ctx, (pdf_document*)doc, "out.pdf", NULL);
	fz_drop_document(ctx, doc);
	fz_drop_context(ctx);

	return 0;
}
