//
// Local "web server" for use as Qiqqa backend & generic (scripted?) access
//
// This server should ultimately host the Qiqqa libraries' databases, OCR cache, etc.
//
// The 'web interface' is intended to be used by both the Qiqqa user apps, import/export apps
// and arbitrary custom user scripts: the core intent is opening up *all* content
// managed by Qiqqa: the PDFs (documents) and all metadata both.
//

#if defined(_MSC_VER)
#ifndef _CRTDBG_MAP_ALLOC
#define _CRTDBG_MAP_ALLOC
#endif
#include <crtdbg.h>
#endif

#include "mupdf/mutool.h"
#include "mupdf/fitz.h"
#include "mupdf/helpers/dir.h"

#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#if defined(_WIN32)
#include <windows.h>
#endif

#define LONGLINE 4096

static inline void memclr(void* ptr, size_t size)
{
	memset(ptr, 0, size);
}

static fz_context* ctx = NULL;
static fz_output* out = NULL;
static fz_stream* datafeed = NULL;

static void usage(void)
{
	fz_info(ctx,
		"muserver: Local web server for use as Qiqqa backend & generic (scripted?) access.\n"
		"\n"
		"Syntax: muserver [options]\n"
		"\n"
		"Options:\n"
		"  -v      verbose (toggle)\n"
		"\n"
		"  -V      display the version of this application and terminate\n"
	);
}


static void mu_drop_context(void)
{
	fz_close_output(ctx, out);
	fz_drop_output(ctx, out);
	out = NULL;
	fz_drop_stream(ctx, datafeed);
	datafeed = NULL;
	fz_drop_context(ctx); // also done here for those rare exit() calls inside the library code.
	ctx = NULL;
}


extern "C" int
qiqqa_safe_file_copier_main(int argc, const char** argv)
{
	int verbosity = 0;
	int c;
	const char* output = NULL;

	ctx = NULL;
	out = NULL;
	datafeed = NULL;

	ctx = fz_new_context(NULL, NULL, FZ_STORE_DEFAULT);
	if (!ctx)
	{
		fz_error(ctx, "cannot initialise MuPDF context");
		return EXIT_FAILURE;
	}

	fz_getopt_reset();
	while ((c = fz_getopt(argc, argv, "o:vV")) != -1)
	{
		switch (c)
		{
		case 'o': output = fz_optarg; break;

		case 'v': verbosity ^= 1; break;

		case 'V': fz_info(ctx, "muserver version %s/%s", FZ_VERSION, "SHA1"); return EXIT_FAILURE;

		default: usage(); return EXIT_FAILURE;
		}
	}

	atexit(mu_drop_context);

	if (fz_optind == argc)
	{
		fz_error(ctx, "No files specified to process\n\n");
		usage();
		return EXIT_FAILURE;
	}

#ifdef _WIN32
	// Get us a debug stream we have a chance of seeing *independently* of stderr.
	fz_set_stddbg(ctx, fz_stdods(ctx));
#endif

	const char* datafilename = NULL;
	int errored = 0;

	fz_try(ctx)
	{
		if (!output || *output == 0 || !strcmp(output, "-"))
		{
			out = fz_stdout(ctx);
			output = NULL;
		}
		else
		{
			char fbuf[4096];
			fz_format_output_path(ctx, fbuf, sizeof fbuf, output, 0);
			fz_normalize_path(ctx, fbuf, sizeof fbuf, fbuf);
			fz_sanitize_path(ctx, fbuf, sizeof fbuf, fbuf);
			out = fz_new_output_with_path(ctx, fbuf, 0);
		}

		while (fz_optind < argc)
		{
			// load a datafile if we already have a script AND we're in "template mode".
			datafilename = argv[fz_optind++];

			datafeed = fz_open_file(ctx, datafilename);
			if (datafeed == NULL)
				fz_throw(ctx, FZ_ERROR_GENERIC, "cannot open datafile: %s", datafilename);

			// Finalize the hash. BLAKE3_OUT_LEN is the default output length, 32 bytes.
			uint8_t hashbytes[32] = { 1 };
			size_t hash_width = 32;

			// Print the hash as hexadecimal.
			char qhbuf[60];
			fz_snprintf(qhbuf, sizeof(qhbuf), "%.0H", hashbytes, hash_width);
			// Now do the Qiqqa b0rk:
			char* d = qhbuf;
			char* s = qhbuf;
			while (*s)
			{
				// ditch most-significant-nibbles that are ZERO:
				if (*s != '0')
					*d++ = *s++;
				else
					s++;
				*d++ = *s++;
			}
			*d = 0;

			// now print the resulting qiqqa hash
			if (verbosity)
			{
				fz_write_printf(ctx, out, "%q: %s\n", datafilename, qhbuf);
			}
			else
			{
				fz_write_printf(ctx, out, "%s\n", qhbuf);
			}

			if (datafeed)
			{
				fz_drop_stream(ctx, datafeed);
				datafeed = NULL;
			}
		}
	}
	fz_catch(ctx)
	{
		if (datafeed)
		{
			fz_drop_stream(ctx, datafeed);
			datafeed = NULL;
		}

		fz_error(ctx, "Failure while processing %q: %s", datafilename, fz_caught_message(ctx));

		errored = 1;
	}

	fz_flush_warnings(ctx);
	mu_drop_context();

	return errored;
}






















#include <stdio.h>
#include <limits.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <math.h>
#include <sys/wait.h>
#include <fcntl.h>

struct fcpy {
	struct source {
		int fd;
		char filename[PATH_MAX + 1];
		struct stat stat;
	} source;
	struct dest {
		char filename[PATH_MAX + 1];
		int fd;
	} dest;
} data;

int MAX_SPLIT = 5;

struct splits {
	off_t offset;
	size_t length;
};

void init() {
	if ((data.source.fd = open(data.source.filename, O_RDONLY)) < 0) {
		perror(data.source.filename);
		exit(EXIT_FAILURE);
	}

	if ((data.dest.fd = open(data.dest.filename, O_WRONLY | O_CREAT | O_TRUNC | O_SYNC, 0664)) < 0) {
		perror(data.dest.filename);
		exit(EXIT_FAILURE);
	}

	if (stat(data.source.filename, &data.source.stat) < 0) {
		perror("stat");
		exit(EXIT_FAILURE);
	}

	if (ftruncate(data.dest.fd, data.source.stat.st_size) < 0) {
		perror("truncate");
		exit(EXIT_FAILURE);
	}
}

void split_(struct splits buf[]) {
	unsigned int split = data.source.stat.st_size / MAX_SPLIT;
	unsigned int mod = data.source.stat.st_size % MAX_SPLIT;

	for (int i = 0; i < MAX_SPLIT; i++) {
		if (i == 0) {
			buf[i].offset = lseek(data.source.fd, 0, SEEK_CUR);
			buf[i].length = split;
			continue;
		}

		if (lseek(data.source.fd, split, SEEK_CUR) < 0) {
			perror("offset");
			exit(EXIT_FAILURE);
		}

		buf[i].offset = lseek(data.source.fd, 0, SEEK_CUR);
		buf[i].length = (i == (MAX_SPLIT - 1) ? split + mod : split);
	}

	lseek(data.source.fd, 0, SEEK_SET);
}

void copy_chunk(off_t offset, size_t length) {
	char* buf = (char*)malloc(length);
	size_t virtual_len = length;

	size_t r = 0;
	size_t read_bytes = 0;
	while ((r = pread(data.source.fd, buf, virtual_len, offset)) != 0) {
		pwrite(data.dest.fd, buf, r, offset);
		if (r < length) {
			offset = r + offset;
			virtual_len = virtual_len - r;
		}
		read_bytes += r;
		printf("Written: %u\n", read_bytes);
		if (read_bytes >= length)
			break;
	}
	free(buf);
}

void copy_large() {
	struct splits splits[MAX_SPLIT];
	split_(splits);
	size_t total = 0;

	for (int i = 0; i < MAX_SPLIT; i++) {
		printf("Offset %u till %u\n", splits[i].offset, splits[i].offset + splits[i].length);
		total += splits[i].length;
		copy_chunk(splits[i].offset, splits[i].length);
	}
	printf("Total Bytes written: %lu\n", total);
}

int main(int argc, char* argv[]) {
	if (argc < 3) {
		fprintf(stderr, "Incomplete arguments\n");
		exit(EXIT_FAILURE);
	}
	else {
		strcpy(data.dest.filename, argv[2]);
		strcpy(data.source.filename, argv[1]);
	}

	init();
	copy_large();
	// if( data.source.stat.st_size >= 209715200 ) {
	// 	printf( "This file is over 200MB\n" );
	// 	copy_large();
	// }
	// else {
	// 	printf("This is a small file\n");
	// 	copy_large();
	// }

	close(data.source.fd);
	close(data.dest.fd);

	return 0;
}

