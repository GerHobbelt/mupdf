// Copyright (C) 2004-2022 Artifex Software, Inc.
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

/* Archive interface */

JNIEXPORT void JNICALL
FUN(Archive_finalize)(JNIEnv *env, jobject self)
{
	fz_context *ctx = get_context(env);
	fz_archive *zip = from_Archive_safe(env, self);
	if (!ctx || !zip) return;
	(*env)->SetLongField(env, self, fid_Archive_pointer, 0);
	fz_drop_archive(ctx, zip);
}

JNIEXPORT jobject JNICALL
FUN(Archive_nativeOpenDirectory)(JNIEnv *env, jclass cls, jstring jdir)
{
	fz_context *ctx = get_context(env);
	fz_archive *zip = NULL;
	const char *dir = NULL;
	jobject jzip;

	if (!ctx) return 0;
	if (!jdir) jni_throw_arg(env, "directory must not be null");

	dir = (*env)->GetStringUTFChars(env, jdir, NULL);
	if (!dir) return 0;

	fz_try(ctx)
		zip = fz_open_directory(ctx, dir);
	fz_always(ctx)
		(*env)->ReleaseStringUTFChars(env, jdir, dir);
	fz_catch(ctx)
		jni_rethrow(env, ctx);

	jzip = (*env)->NewObject(env, cls_Archive, mid_Archive_init, (jlong)(intptr_t)zip);
	if (!jzip || (*env)->ExceptionCheck(env))
	{
		fz_drop_archive(ctx, zip);
		return NULL;
	}

	return jzip;
}

JNIEXPORT jobject JNICALL
FUN(Archive_nativeOpenArchive)(JNIEnv *env, jclass cls, jstring jpath)
{
	fz_context *ctx = get_context(env);
	fz_archive *zip = NULL;
	const char *path = NULL;
	jobject jzip;

	if (!ctx) return 0;
	if (!jpath) jni_throw_arg(env, "archive name must not be null");

	path = (*env)->GetStringUTFChars(env, jpath, NULL);
	if (!path) return 0;

	fz_try(ctx)
		zip = fz_open_archive(ctx, path);
	fz_always(ctx)
		(*env)->ReleaseStringUTFChars(env, jpath, path);
	fz_catch(ctx)
		jni_rethrow(env, ctx);

	jzip = (*env)->NewObject(env, cls_Archive, mid_Archive_init, (jlong)(intptr_t)zip);
	if (!jzip || (*env)->ExceptionCheck(env))
	{
		fz_drop_archive(ctx, zip);
		return NULL;
	}

	return jzip;
}

JNIEXPORT jobject JNICALL
FUN(Archive_nativeOpenArchiveBuffer)(JNIEnv *env, jclass cls, jobject jbuf)
{
	fz_context *ctx = get_context(env);
	fz_archive *zip = NULL;
	fz_buffer *buf = from_Buffer_safe(env, jbuf);
	fz_stream *stream = NULL;
	jobject jzip;

	if (!ctx) return 0;
	if (!buf) jni_throw_arg(env, "buffer must not be null");

	fz_var(stream);

	fz_try(ctx)
	{
		stream = fz_open_buffer(ctx, buf);
		zip = fz_open_archive_with_stream(ctx, stream);
	}
	fz_always(ctx)
		fz_drop_stream(ctx, stream);
	fz_catch(ctx)
		jni_rethrow(env, ctx);

	jzip = (*env)->NewObject(env, cls_Archive, mid_Archive_init, (jlong)(intptr_t)zip);
	if (!jzip || (*env)->ExceptionCheck(env))
	{
		fz_drop_archive(ctx, zip);
		return NULL;
	}

	return jzip;
}
