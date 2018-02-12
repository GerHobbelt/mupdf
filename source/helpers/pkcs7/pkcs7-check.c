#include "mupdf/fitz.h"
#include "mupdf/pdf.h"
#include "mupdf/helpers/pkcs7-openssl.h"
#include "mupdf/helpers/pkcs7-check.h"

#include <string.h>


static void pdf_print_designated_name(pdf_pkcs7_designated_name *name, char *buf, int buflen)
{
	int i, n;
	const char *part[] = {
		"/CN=", name->cn,
		"/O=", name->o,
		"/OU=", name->ou,
		"/emailAddress=", name->email,
		"/C=", name->c};

	if (buflen)
		buf[0] = 0;

	n = sizeof(part)/sizeof(*part);
	for (i = 0; i < n; i++)
		if (part[i])
			fz_strlcat(buf, part[i], buflen);
}

int pdf_check_signature(fz_context *ctx, pdf_document *doc, pdf_widget *widget, char *ebuf, int ebufsize)
{
	fz_stream *bytes = NULL;
	char *contents = NULL;
	int contents_len;
	int res = 0;

	if (pdf_xref_obj_is_unsaved_signature(doc, ((pdf_annot *)widget)->obj))
	{
		fz_strlcpy(ebuf, "Signed but document yet to be saved", ebufsize);
		if (ebufsize > 0)
			ebuf[ebufsize-1] = 0;
		return 0;
	}

	fz_var(bytes);
	fz_var(res);
	fz_try(ctx)
	{
		contents_len = pdf_signature_widget_contents(ctx, doc, widget, &contents);
		if (contents)
		{
			SignatureError err;

			bytes = pdf_signature_widget_hash_bytes(ctx, doc, widget);
			err = pkcs7_openssl_check_digest(ctx, bytes, contents, contents_len);
			if (err == SignatureError_Okay)
				err = pkcs7_openssl_check_certificate(contents, contents_len);
			switch (err)
			{
			case SignatureError_Okay:
				ebuf[0] = 0;
				res = 1;
				break;
			case SignatureError_NoSignatures:
				fz_strlcpy(ebuf, "No signatures", ebufsize);
				break;
			case SignatureError_NoCertificate:
				fz_strlcpy(ebuf, "No certificate", ebufsize);
				break;
			case SignatureError_DocumentChanged:
				fz_strlcpy(ebuf, "Document changed since signing", ebufsize);
				break;
			case SignatureError_SelfSigned:
				fz_strlcpy(ebuf, "Self-signed certificate", ebufsize);
				break;
			case SignatureError_SelfSignedInChain:
				fz_strlcpy(ebuf, "Self-signed certificate in chain", ebufsize);
				break;
			case SignatureError_NotTrusted:
				fz_strlcpy(ebuf, "Certificate not trusted", ebufsize);
				break;
			default:
			case SignatureError_Unknown:
				fz_strlcpy(ebuf, "Unknown error", ebufsize);
				break;
			}

			switch (err)
			{
			case SignatureError_SelfSigned:
			case SignatureError_SelfSignedInChain:
			case SignatureError_NotTrusted:
				{
					pdf_pkcs7_designated_name *name = pkcs7_openssl_designated_name(ctx, contents, contents_len);
					if (name)
					{
						int len;

						fz_strlcat(ebuf, ": ", ebufsize);
						len = strlen(ebuf);
						pdf_print_designated_name(name, ebuf + len, ebufsize - len);
						pkcs7_opensll_drop_designated_name(ctx, name);
					}
				}
				break;
			default:
				break;
			}
		}
		else
		{
			res = 0;
			fz_strlcpy(ebuf, "Not signed", ebufsize);
		}
	}
	fz_always(ctx)
	{
		fz_drop_stream(ctx, bytes);
	}
	fz_catch(ctx)
	{
		res = 0;
		fz_strlcpy(ebuf, fz_caught_message(ctx), ebufsize);
	}

	if (ebufsize > 0)
		ebuf[ebufsize-1] = 0;

	return res;
}
