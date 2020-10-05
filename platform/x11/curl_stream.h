#ifndef CURL_STREAM_H
#define CURL_STREAM_H

#ifdef __cplusplus
extern "C" {
#endif

fz_stream *fz_open_file_progressive(fz_context *ctx, const char *filename, int kbps,
	void (*on_data)(void*,int), void *arg);
fz_stream *fz_open_url(fz_context *ctx, const char *url, int kbps,
	void (*on_data)(void*,int), void *arg);

#ifdef __cplusplus
}
#endif

#endif
