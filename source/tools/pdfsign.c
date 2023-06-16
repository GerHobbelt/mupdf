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
// Artifex Software, Inc., 39 Mesa Street, Suite 108A, San Francisco,
// CA 94129, USA, for further information.

/*
 * PDF signature tool: verify and sign digital signatures in PDF files.
 */

#include "mupdf/mutool.h"
#include "mupdf/fitz.h"
#include "mupdf/pdf.h"
#include "mupdf/helpers/pkcs7-openssl.h"
#include "mupdf/helpers/jmemcust.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#if FZ_ENABLE_PDF

static fz_context *ctx = NULL;
static const char *infile = NULL;
static const char *outfile = NULL;
static const char *certificatefile = NULL;
static const char *certificatepassword = "";
static int verify = 0;
static int clear = 0;
static int sign = 0;
static int list = 1;

static int usage(void)
{
	fz_info(ctx,
		"usage: mutool sign [options] input.pdf [signature object numbers]\n"
		"  -p -  password\n"
		"  -v    verify signature\n"
		"  -c    clear signatures\n"
		"  -s -  sign signatures using certificate file\n"
		"  -P -  certificate password\n"
		"  -o -  output file name\n"
	);

	return EXIT_FAILURE;
}

static void verify_signature(fz_context *ctx, pdf_document *doc, pdf_obj *signature)
{
	char *name;
	pdf_signature_error err;
	pdf_pkcs7_verifier *verifier;
	int edits;
	pdf_pkcs7_distinguished_name *dn = NULL;

	fz_info(ctx, "Verifying signature %d:\n", pdf_to_num(ctx, signature));

	if (!pdf_signature_is_signed(ctx, doc, signature))
	{
		fz_info(ctx, "\tSignature is not signed.\n");
		return;
	}

	verifier = pkcs7_openssl_new_verifier(ctx);
	fz_var(dn);
	fz_try(ctx)
	{
		dn = pdf_signature_get_signatory(ctx, verifier, doc, signature);
		if (dn)
		{
			name = pdf_signature_format_distinguished_name(ctx, dn);
			fz_info(ctx, "\tDistinguished name: %s\n", name);
			fz_free(ctx, name);
		}
		else
		{
			fz_info(ctx, "\tSignature information missing.\n");
		}

		err = pdf_check_certificate(ctx, verifier, doc, signature);
		if (err)
			fz_info(ctx, "\tCertificate error: %s\n", pdf_signature_error_description(err));
		else
			fz_info(ctx, "\tCertificate is trusted.\n");

		err = pdf_check_digest(ctx, verifier, doc, signature);
		edits = pdf_signature_incremental_change_since_signing(ctx, doc, signature);
		if (err)
			fz_info(ctx, "\tDigest error: %s\n", pdf_signature_error_description(err));
		else if (edits)
			fz_info(ctx, "\tThe signature is valid but there have been edits since signing.\n");
		else
			fz_info(ctx, "\tThe document is unchanged since signing.\n");
	}
	fz_always(ctx)
	{
		pdf_signature_drop_distinguished_name(ctx, dn);
		pdf_drop_verifier(ctx, verifier);
	}
	fz_catch(ctx)
		fz_info(ctx, "\tVerification error: %s\n", fz_caught_message(ctx));
}

static void clear_signature(fz_context *ctx, pdf_document *doc, pdf_obj *signature)
{
	pdf_page *page = NULL;
	pdf_annot *widget;
	pdf_obj *parent;
	int pageno, pagenoend;

	fz_var(page);

	fz_info(ctx, "Clearing signature %d.\n", pdf_to_num(ctx, signature));

	fz_try(ctx)
	{
		parent = pdf_dict_get(ctx, signature, PDF_NAME(P));
		if (parent != NULL)
		{
			pageno = pdf_lookup_page_number(ctx, doc, parent);
			pagenoend = pageno+1;
		}
		else
		{
			pageno = 0;
			pagenoend = pdf_count_pages(ctx, doc);
		}
		for (; pageno < pagenoend; pageno++)
		{
			page = pdf_load_page(ctx, doc, pageno);
			for (widget = pdf_first_widget(ctx, page); widget; widget = pdf_next_widget(ctx, widget))
				if (pdf_widget_type(ctx, widget) == PDF_WIDGET_TYPE_SIGNATURE && !pdf_objcmp_resolve(ctx, pdf_annot_obj(ctx, widget), signature))
					pdf_clear_signature(ctx, widget);
			fz_drop_page(ctx, (fz_page *)page);  // the clean way is a typecast via pdf_page_from_fz_page() but that's overkill here
			page = NULL;
		}
	}
	fz_always(ctx)
		fz_drop_page(ctx, (fz_page*)page);
	fz_catch(ctx)
		fz_rethrow(ctx);
}

static void sign_signature(fz_context *ctx, pdf_document *doc, pdf_obj *signature)
{
	pdf_pkcs7_signer *signer = NULL;
	pdf_page *page = NULL;
	pdf_annot *widget;
	pdf_obj *parent;
	int pageno, pagenoend;

	fz_var(page);
	fz_var(signer);

	fz_info(ctx, "Signing signature %d.\n", pdf_to_num(ctx, signature));

	fz_try(ctx)
	{
		signer = pkcs7_openssl_read_pfx(ctx, certificatefile, certificatepassword);

		parent = pdf_dict_get(ctx, signature, PDF_NAME(P));
		if (parent != NULL)
		{
			pageno = pdf_lookup_page_number(ctx, doc, parent);
			pagenoend = pageno+1;
		}
		else
		{
			pageno = 0;
			pagenoend = pdf_count_pages(ctx, doc);
		}
		for (; pageno < pagenoend; pageno++)
		{
			page = pdf_load_page(ctx, doc, pageno);
			for (widget = pdf_first_widget(ctx, page); widget; widget = pdf_next_widget(ctx, widget))
				if (pdf_widget_type(ctx, widget) == PDF_WIDGET_TYPE_SIGNATURE && !pdf_objcmp_resolve(ctx, pdf_annot_obj(ctx, widget), signature))
					pdf_sign_signature(ctx, widget, signer,
						PDF_SIGNATURE_DEFAULT_APPEARANCE,
						NULL,
						NULL,
						NULL);
			fz_drop_page(ctx, (fz_page *)page);
			page = NULL;
		}
	}
	fz_always(ctx)
	{
		fz_drop_page(ctx, (fz_page*)page);
		pdf_drop_signer(ctx, signer);
	}
	fz_catch(ctx)
		fz_rethrow(ctx);

}

static void list_signature(fz_context *ctx, pdf_document *doc, pdf_obj *signature)
{
	pdf_pkcs7_distinguished_name *dn;
	pdf_pkcs7_verifier *verifier;

	if (!pdf_signature_is_signed(ctx, doc, signature))
	{
		fz_info(ctx, "%5d: Signature is not signed.\n", pdf_to_num(ctx, signature));
		return;
	}

	verifier = pkcs7_openssl_new_verifier(ctx);

	dn = pdf_signature_get_signatory(ctx, verifier, doc, signature);
	if (dn)
	{
		char *s = pdf_signature_format_distinguished_name(ctx, dn);
		fz_info(ctx, "%5d: Distinguished name: %s\n", pdf_to_num(ctx, signature), s);
		fz_free(ctx, s);
		pdf_signature_drop_distinguished_name(ctx, dn);
	}
	else
	{
		fz_info(ctx, "%5d: Signature information missing.\n", pdf_to_num(ctx, signature));
	}

	pdf_drop_verifier(ctx, verifier);

}

static void process_field(fz_context *ctx, pdf_document *doc, pdf_obj *field)
{
	if (pdf_dict_get_inheritable(ctx, field, PDF_NAME(FT)) != PDF_NAME(Sig))
		fz_warn(ctx, "%d is not a signature, skipping", pdf_to_num(ctx, field));
	else
	{
		if (list)
			list_signature(ctx, doc, field);
		if (verify)
			verify_signature(ctx, doc, field);
		if (clear)
			clear_signature(ctx, doc, field);
		if (sign)
			sign_signature(ctx, doc, field);
	}
}

static void process_field_hierarchy(fz_context *ctx, pdf_document *doc, pdf_obj *field)
{
	pdf_obj *kids = pdf_dict_get(ctx, field, PDF_NAME(Kids));
	if (kids)
	{
		int i, n;
		n = pdf_array_len(ctx, kids);
		for (i = 0; i < n; ++i)
		{
			pdf_obj *kid = pdf_array_get(ctx, kids, i);
			process_field_hierarchy(ctx, doc, kid);
		}
	}
	else if (pdf_dict_get_inheritable(ctx, field, PDF_NAME(FT)) == PDF_NAME(Sig))
		process_field(ctx, doc, field);
}

static void process_acro_form(fz_context *ctx, pdf_document *doc)
{
	pdf_obj *trailer = pdf_trailer(ctx, doc);
	pdf_obj *root = pdf_dict_get(ctx, trailer, PDF_NAME(Root));
	pdf_obj *acroform = pdf_dict_get(ctx, root, PDF_NAME(AcroForm));
	pdf_obj *fields = pdf_dict_get(ctx, acroform, PDF_NAME(Fields));
	int i, n = pdf_array_len(ctx, fields);
	for (i = 0; i < n; ++i)
		process_field_hierarchy(ctx, doc, pdf_array_get(ctx, fields, i));
}

int pdfsign_main(int argc, const char** argv)
{
	pdf_document *doc = NULL;
	const char *password = "";
	int c;

	ctx = NULL;
	infile = NULL;
	outfile = NULL;
	certificatefile = NULL;
	certificatepassword = "";
	verify = 0;
	clear = 0;
	sign = 0;
	list = 1;

	fz_getopt_reset();
	while ((c = fz_getopt(argc, argv, "co:p:s:vP:")) != -1)
	{
		switch (c)
		{
		case 'c': list = 0; clear = 1; break;
		case 'o': outfile = fz_optarg; break;
		case 'p': password = fz_optarg; break;
		case 'P': certificatepassword = fz_optarg; break;
		case 's': list = 0; sign = 1; certificatefile = fz_optarg; break;
		case 'v': list = 0; verify = 1; break;
		default: return usage();
		}
	}

	if (argc == fz_optind)
	{
		return usage();
	}

	if (!fz_has_global_context())
	{
		ctx = fz_new_context(NULL, NULL, FZ_STORE_UNLIMITED);
		if (!ctx)
		{
			fz_error(ctx, "cannot initialise MuPDF context");
			return EXIT_FAILURE;
		}
		fz_set_global_context(ctx);
	}

	ctx = fz_new_context(NULL, NULL, FZ_STORE_UNLIMITED);
	if (!ctx)
	{
		fz_error(ctx, "cannot initialise MuPDF context");
		return EXIT_FAILURE;
	}

	// register a mupdf-aligned default heap memory manager for jpeg/jpeg-turbo
	fz_set_default_jpeg_sys_mem_mgr();

	infile = argv[fz_optind++];

	if (!clear && !sign && !verify && argc - fz_optind > 0)
	{
		list = 0;
		verify = 1;
	}

	fz_var(doc);

	fz_try(ctx)
	{
		doc = pdf_open_document(ctx, infile);
		if (pdf_needs_password(ctx, doc))
			if (!pdf_authenticate_password(ctx, doc, password))
				fz_warn(ctx, "cannot authenticate password: %s", infile);

		if (argc - fz_optind <= 0 || list)
			process_acro_form(ctx, doc);
		else
		{
			while (argc - fz_optind)
			{
				pdf_obj *field = pdf_new_indirect(ctx, doc, fz_atoi(argv[fz_optind]), 0);
				process_field(ctx, doc, field);
				pdf_drop_obj(ctx, field);
				fz_optind++;
			}
		}

		if (clear || sign)
		{
			pdf_write_options opts = pdf_default_write_options;
			opts.do_incremental = 1;
			if (!outfile)
				outfile = "out.pdf";
			pdf_save_document(ctx, doc, outfile, &opts);
		}
	}
	fz_always(ctx)
		pdf_drop_document(ctx, doc);
	fz_catch(ctx)
	{
		fz_log_error(ctx, fz_caught_message(ctx));
		fz_log_error(ctx, "error processing signatures");
	}

	fz_flush_warnings(ctx);
	fz_drop_context(ctx);
	return EXIT_SUCCESS;
}

#else

int pdfsign_main(int argc, const char** argv)
{
	fz_error(NULL, "PDFsign utility is not supported in this build (PDF support has been disabled).");
	return EXIT_FAILURE;
}

#endif
