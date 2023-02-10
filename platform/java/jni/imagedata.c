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
// Artifex Software, Inc., 1305 Grant Avenue - Suite 200, Novato,
// CA 94945, U.S.A., +1(415)492-9861, for further information.

/* ImageData interface */

JNIEXPORT void JNICALL
FUN(ImageData_finalize)(JNIEnv *env, jobject self)
{
	fz_context *ctx = get_context(env);
	fz_compressed_buffer *image_data = from_ImageData_safe(env, self);
	if (!ctx || !image_data) return;
	(*env)->SetLongField(env, self, fid_ImageData_pointer, 0);
	fz_drop_compressed_buffer(ctx, image_data);
}

JNIEXPORT jlong JNICALL
FUN(Image_newNative)(JNIEnv *env, jobject self, jobject jbuffer, jobject jparams)
{
	fz_context *ctx = get_context(env);
	fz_buffer *buffer = from_Buffer_safe(env, jbuffer);
	fz_compressed_buffer *image_data = NULL;
	int type, columns, rows, k, predictor, colors, bpc, early_change;
	int end_of_line, encoded_byte_align, end_of_block, black_is_1;
	int embedded, color_transform, smask_in_data, damaged_rows_before_error;
	fz_buffer *globals = NULL;
	jobject jglobals;

	if (!ctx) return 0;
	if (!jbuffer) jni_throw_arg(env, "buffer must not be null");
	if (!jparams) jni_throw_arg(env, "params must not be null");

	type = (*env)->GetIntField(env, jparams, fid_ImageData_Params_type);
	switch (type)
	{
	case FZ_IMAGE_FAX:
		columns = (*env)->GetIntField(env, jparams, fid_ImageData_FaxParams_columns);
		rows = (*env)->GetIntField(env, jparams, fid_ImageData_FaxParams_rows);
		k = (*env)->GetIntField(env, jparams, fid_ImageData_FaxParams_k);
		end_of_line = (*env)->GetBooleanField(env, jparams, fid_ImageData_FaxParams_endOfLine);
		encoded_byte_align = (*env)->GetBooleanField(env, jparams, fid_ImageData_FaxParams_encodedByteAlign);
		end_of_block = (*env)->GetBooleanField(env, jparams, fid_ImageData_FaxParams_endOfBlock);
		black_is_1 = (*env)->GetBooleanField(env, jparams, fid_ImageData_FaxParams_blackIs1);
		damaged_rows_before_error = (*env)->GetIntField(env, jparams, fid_ImageData_FaxParams_damagedRowsBeforeError);
		break;
	case FZ_IMAGE_FLATE:
		predictor = (*env)->GetIntField(env, jparams, fid_ImageData_FlateParams_predictor);
		columns = (*env)->GetIntField(env, jparams, fid_ImageData_FlateParams_columns);
		colors = (*env)->GetIntField(env, jparams, fid_ImageData_FlateParams_colors);
		bpc = (*env)->GetIntField(env, jparams, fid_ImageData_FlateParams_bitsPerComponent);
		break;
	case FZ_IMAGE_LZW:
		predictor = (*env)->GetIntField(env, jparams, fid_ImageData_LZWParams_predictor);
		columns = (*env)->GetIntField(env, jparams, fid_ImageData_LZWParams_columns);
		colors = (*env)->GetIntField(env, jparams, fid_ImageData_LZWParams_colors);
		bpc = (*env)->GetIntField(env, jparams, fid_ImageData_LZWParams_bitsPerComponent);
		early_change = (*env)->GetIntField(env, jparams, fid_ImageData_LZWParams_earlyChange);
		break;
	case FZ_IMAGE_JBIG2:
		embedded = (*env)->GetIntField(env, jparams, fid_ImageData_JBIG2Params_embedded);
		jglobals = (*env)->GetObjectField(env, jparams, fid_ImageData_JBIG2Params_globals);
		globals = from_Buffer_safe(env, jglobals);
		break;
	case FZ_IMAGE_JPEG:
		color_transform = (*env)->GetIntField(env, jparams, fid_ImageData_JPEGParams_colorTransform);
		break;
	case FZ_IMAGE_JPX:
		smask_in_data = (*env)->GetIntField(env, jparams, fid_ImageData_JPXParams_sMaskInData);
		break;
	}

	fz_try(ctx)
	{
		image_data = fz_malloc_struct(ctx, fz_compressed_buffer);
		image_data->params.type = type;
		switch (type)
		{
		case FZ_IMAGE_FAX:
			image_data->params.u.fax.columns = columns;
			image_data->params.u.fax.rows = rows;
			image_data->params.u.fax.k = k;
			image_data->params.u.fax.end_of_line = end_of_line;
			image_data->params.u.fax.encoded_byte_align = encoded_byte_align;
			image_data->params.u.fax.end_of_block = end_of_block;
			image_data->params.u.fax.black_is_1 = black_is_1;
			image_data->params.u.fax.damaged_rows_before_error = damaged_rows_before_error;
			break;
		case FZ_IMAGE_FLATE:
			image_data->params.u.flate.predictor = predictor;
			image_data->params.u.flate.columns = columns;
			image_data->params.u.flate.colors = colors;
			image_data->params.u.flate.bpc = bpc;
			break;
		case FZ_IMAGE_LZW:
			image_data->params.u.lzw.predictor = predictor;
			image_data->params.u.lzw.columns = columns;
			image_data->params.u.lzw.colors = colors;
			image_data->params.u.lzw.bpc = bpc;
			image_data->params.u.lzw.early_change = early_change;
			break;
		case FZ_IMAGE_JBIG2:
			image_data->params.u.jbig2.embedded = embedded;
			image_data->params.u.jbig2.globals = fz_new_jbig2_globals(ctx, globals);
			break;
		case FZ_IMAGE_JPEG:
			image_data->params.u.jpeg.color_transform = color_transform;
			break;
		case FZ_IMAGE_JPX:
			image_data->params.u.jpx.smask_in_data = smask_in_data;
			break;
		}
		image_data->buffer = fz_keep_buffer(ctx, buffer);
	}
	fz_catch(ctx)
		jni_rethrow(env, ctx);

	return jlong_cast(image_data);
}

JNIEXPORT jlong JNICALL
FUN(Image_newNativeRecognizeFormat)(JNIEnv *env, jobject self, jobject jbuffer)
{
	fz_context *ctx = get_context(env);
	fz_buffer *buffer = from_Buffer_safe(env, jbuffer);
	fz_compressed_buffer *image_data = NULL;

	if (!ctx) return 0;
	if (!jbuffer) jni_throw_arg(env, "buffer must not be null");
	if (buffer->len < 8) jni_throw_arg(env, "buffer must be longer than 8 bytes");

	fz_try(ctx)
	{
		image_data = fz_malloc_struct(ctx, fz_compressed_buffer);
		image_data->params.type = fz_recognize_image_format(ctx, buffer->data);
		if (image_data->params.type == FZ_IMAGE_JPEG)
			image_data->params.u.jpeg.color_transform = -1;
		image_data->buffer = fz_keep_buffer(ctx, buffer);
	}
	fz_catch(ctx)
		jni_rethrow(env, ctx);

	return jlong_cast(image_data);
}
