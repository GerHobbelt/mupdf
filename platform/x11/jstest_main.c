
#include "timeval.h"

#include "pdfapp.h"
#include "mupdf/helpers/dir.h"

#ifndef DISABLE_MUTHREADS
#include "mupdf/helpers/mu-threads.h"
#endif

#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#if defined(_MSC_VER)
#include <crtdbg.h>
#include <windows.h>
#endif


int mutool_main(int argc, const char** argv);

/*
	A useful bit of bash script to call this is:
	for f in ../ghostpcl/tests_private/pdf/forms/v1.3/ *.pdf ; do g=${f%.*} ; echo $g ; win32/debug/mujstest-v8.exe -o $g-%d.png -p ../ghostpcl/ $g.mjs > $g.log 2>&1 ; done

	Remove the space from "/ *.pdf" before running - can't leave that
	in here, as it causes a warning about a possibly malformed comment.
*/

#define LONGLINE 4096

static fz_context* ctx = NULL;

/*
	In the presence of pthreads or Windows threads, we can offer
	a multi-threaded option. In the absence, of such, we degrade
	nicely.
*/
#ifndef DISABLE_MUTHREADS

static mu_mutex mutexes[FZ_LOCK_MAX];

static void mudraw_lock(void *user, int lock)
{
	mu_lock_mutex(&mutexes[lock]);
}

static void mudraw_unlock(void *user, int lock)
{
	mu_unlock_mutex(&mutexes[lock]);
}

static fz_locks_context mudraw_locks =
{
	NULL, mudraw_lock, mudraw_unlock
};

static void fin_mudraw_locks(void)
{
	int i;

	for (i = 0; i < FZ_LOCK_MAX; i++)
		mu_destroy_mutex(&mutexes[i]);
}

static fz_locks_context *init_mudraw_locks(void)
{
	int i;
	int failed = 0;

	for (i = 0; i < FZ_LOCK_MAX; i++)
		failed |= mu_create_mutex(&mutexes[i]);

	if (failed)
	{
		fin_mudraw_locks();
		return NULL;
	}

	return &mudraw_locks;
}

#endif

static int showtime = 0;
static int showmemory = 0;

static int mujstest_is_toplevel_ctx = 0;

static struct {
	struct curltime start_time;
	int count;
	timediff_t total;
	timediff_t min, max;
	int minscriptline, maxscriptline;
	const char *mincommand;
	const char *maxcommand;
} timing;

typedef struct
{
	size_t size;
	size_t seqnum;
#if defined(_M_IA64) || defined(_M_AMD64) || defined(_WIN64)
	size_t magic;
	size_t align128;
#else
	size_t magic;
#endif
} trace_header;

struct trace_info
{
	int sequence_number;
	size_t current;
	size_t peak;
	size_t total;
	size_t allocs;
	size_t mem_limit;
	size_t alloc_limit;
};

static struct trace_info trace_info = { 1, 0, 0, 0, 0, 0, 0 };

static void *hit_limit(void *val)
{
	return val;
}

static void *hit_memory_limit(struct trace_info *info, int is_malloc, size_t oldsize, size_t size)
{
	if (is_malloc)
		fz_error(ctx, "Memory limit (%zu) hit upon malloc(%zu) when %zu already allocated.", info->mem_limit, size, info->current);
	else
		fz_error(ctx, "Memory limit (%zu) hit upon realloc(%zu) from %zu bytes when %zu already allocated.", info->mem_limit, size, oldsize, info->current);
	return hit_limit(NULL);
}


static void *hit_alloc_limit(struct trace_info *info, int is_malloc, size_t oldsize, size_t size)
{
	if (is_malloc)
		fz_error(ctx, "Allocation limit (%zu) hit upon malloc(%zu) when %zu already allocated.", info->alloc_limit, size, info->current);
	else
		fz_error(ctx, "Allocation limit (%zu) hit upon realloc(%zu) from %zu bytes when %zu already allocated.", info->alloc_limit, size, oldsize, info->current);
	return hit_limit(NULL);
}

static void *
trace_malloc(void *arg, size_t size)
{
	struct trace_info *info = (struct trace_info *) arg;
	trace_header *p;
	if (size == 0)
		return NULL;
	if (size > SIZE_MAX - sizeof(trace_header))
		return NULL;
	if (info->mem_limit > 0 && size > info->mem_limit - info->current)
		return hit_memory_limit(info, 1, 0, size);
	if (info->alloc_limit > 0 && info->allocs > info->alloc_limit)
		return hit_alloc_limit(info, 1, 0, size);
	p = malloc(size + sizeof(trace_header));
	if (p == NULL)
		return NULL;
	p[0].size = size;
	p[0].magic = 0xEAD;
	p[0].seqnum = info->sequence_number++;
	info->current += size;
	info->total += size;
	if (info->current > info->peak)
		info->peak = info->current;
	info->allocs++;
	return (void *)&p[1];
}

static void
trace_free(void *arg, void *p_)
{
	struct trace_info *info = (struct trace_info *) arg;
	trace_header *p = (trace_header *)p_;

	if (p_ == NULL)
		return;

	size_t size = p[-1].size;
	int rotten = 0;
	info->current -= size;
	if (p[-1].magic != 0xEAD)
	{
		fz_error(ctx, "*!* double free! %d", (int)(p[-1].magic - 0xEAD));
		p[-1].magic++;
		rotten = 1;
	}
	if (rotten)
	{
		fz_error(ctx, "*!* corrupted heap record! %p", &p[-1]);
	}
	else
	{
		free(&p[-1]);
	}
}

static void *
trace_realloc(void *arg, void *p_, size_t size)
{
	struct trace_info *info = (struct trace_info *) arg;
	trace_header *p = (trace_header *)p_;
	size_t oldsize;

	if (size == 0)
	{
		trace_free(arg, p_);
		return NULL;
	}

	if (p_ == NULL)
		return trace_malloc(arg, size);
	if (size > SIZE_MAX - sizeof(trace_header))
		return NULL;
	oldsize = p[-1].size;
	if (info->mem_limit > 0 && size > info->mem_limit - info->current + oldsize)
		return hit_memory_limit(info, 0, oldsize, size);
	if (info->alloc_limit > 0 && info->allocs > info->alloc_limit)
		return hit_alloc_limit(info, 0, oldsize, size);

	int rotten = 0;
	oldsize = p[-1].size;
	if (p[-1].magic != 0xEAD)
	{
		fz_error(ctx, "*!* double free! %d", (int)(p[-1].magic - 0xEAD));
		p[-1].magic++;
		rotten = 1;
	}
	if (rotten)
	{
		fz_error(ctx, "*!* corrupted heap record! %p", &p[-1]);
		return NULL;
	}
	else
	{
		p = realloc(&p[-1], size + sizeof(trace_header));
		if (p == NULL)
			return NULL;
		info->current += size - oldsize;
		if (size > oldsize)
			info->total += size - oldsize;
		if (info->current > info->peak)
			info->peak = info->current;
		p[0].size = size;
		info->allocs++;
		return &p[1];
	}
}

static pdfapp_t gapp;
static int file_open = 0;
static const char *scriptname;
static const char *output = "out-%04d.png";
static const char *prefix = NULL;
static int shotcount = 0;
static int verbosity = 0;

static char getline_buffer[LONGLINE];
static char output_buffer[LONGLINE];
static char prefix_buffer[LONGLINE];

void winwarn(pdfapp_t *app, char *msg)
{
	fz_warn(ctx, "%s", msg);
}

void winerror(pdfapp_t *app, char *msg)
{
	fz_error(ctx, "%s", msg);
	exit(1);
}

void winalert(pdfapp_t *app, pdf_alert_event *alert)
{
	fz_error(ctx, "Alert %s: %s", alert->title, alert->message);
	switch (alert->button_group_type)
	{
	case PDF_ALERT_BUTTON_GROUP_OK:
	case PDF_ALERT_BUTTON_GROUP_OK_CANCEL:
		alert->button_pressed = PDF_ALERT_BUTTON_OK;
		break;
	case PDF_ALERT_BUTTON_GROUP_YES_NO:
	case PDF_ALERT_BUTTON_GROUP_YES_NO_CANCEL:
		alert->button_pressed = PDF_ALERT_BUTTON_YES;
		break;
	}
}

void winadvancetimer(pdfapp_t *app, float duration)
{
}

void winprint(pdfapp_t *app)
{
	fz_warn(ctx, "The MuPDF library supports printing, but this application currently does not");
}

int winquery(pdfapp_t *app, const char *query)
{
	return QUERY_NO;
}

int wingetcertpath(pdfapp_t* app, char *buf, int len)
{
	return 0;
}

static char pd_password[256] = "";
static char td_textinput[LONGLINE] = "";

const char *winpassword(pdfapp_t *app, const char *filename)
{
	if (pd_password[0] == 0)
		return NULL;
	return pd_password;
}

char *wintextinput(pdfapp_t *app, char *inittext, int retry)
{
	if (retry)
		return NULL;

	if (td_textinput[0] != 0)
		return td_textinput;
	return inittext;
}

int winchoiceinput(pdfapp_t *app, int nopts, const char *opts[], int *nvals, const char *vals[])
{
	return 0;
}

void winhelp(pdfapp_t*app)
{
}

void winclose(pdfapp_t *app)
{
	pdfapp_close(app);
	exit(0);
}

int winsavequery(pdfapp_t *app)
{
	return DISCARD;
}

int wingetsavepath(pdfapp_t *app, char *buf, int len)
{
	return 0;
}

void winreplacefile(pdfapp_t* app, char *source, char *target)
{
}

void wincopyfile(pdfapp_t* app, char *source, char *target)
{
}

void wincursor(pdfapp_t *app, int curs)
{
}

void wintitle(pdfapp_t *app, char *title)
{
}

void windrawrect(pdfapp_t *app, int x0, int y0, int x1, int y1)
{
}

void windrawstring(pdfapp_t *app, int x, int y, char *s)
{
}

void winresize(pdfapp_t *app, int w, int h)
{
}

void winrepaint(pdfapp_t *app)
{
}

void winrepaintsearch(pdfapp_t *app)
{
}

void winfullscreen(pdfapp_t *app, int state)
{
}

/*
 * Event handling
 */

void windocopy(pdfapp_t *app)
{
}

void winreloadpage(pdfapp_t *app)
{
}

void winopenuri(pdfapp_t *app, char *buf)
{
}

static void usage(void)
{
	fz_info(ctx,
		"mujstest: Scriptable tester for mupdf + js\n"
		"\n"
		"Syntax: mujstest -o <filename> [ -p <prefix> ] [options] <scriptfile>\n"
		"\n"
		"Options:\n"
		"  -o -    output file name (%%d for page number) template for the SCREENSHOT\n"
		"          script command\n"
		"     <filename>\n"
		"          should sensibly be of the form file-%%d.png\n"
		"\n"
		"  -p -    input path prefix for the OPEN script command\n"
		"     <prefix>\n"
		"          is a path prefix to apply to filenames within the script\n"
		"\n"
		"  -v      verbose (toggle)\n"
		"  -q      be quiet (don't print progress messages)\n"
		"  -s -    show extra information:\n"
		"     m    show memory use\n"
		"     t    show timings\n"
		"\n"
		"  -m -    specify custom memory limits:\n"
		"    sNNN  set memory limit to NNN bytes: the application will not be allowed\n"
		"          to consume more that NNN bytes heap memory at any time\n"
		"    aNNN  set alloc limit to NNN: the application will not execute more than\n"
		"          NNN heap allocation requests\n"
		"    NNN   set memory limit to NNN bytes (same as 'sNNN' above)\n"
		"  -L      low memory mode (avoid caching, clear objects after each page)\n"
		"\n"
		"  -V      display the version of this application and terminate\n"
		"\n"
		"\nscriptfile contains a list of commands:\n"
		"  PASSWORD <password>  Set the password\n"
		"  OPEN <filename>      Open a file\n"
		"  GOTO <page>          Jump to a particular page\n"
		"  SCREENSHOT           Save a screenshot\n"
		"  RESIZE <w> <h>       Resize the screen to a given size\n"
		"  CLICK <x> <y> <btn>  Click at a given position\n"
		"  TEXT <string>        Set a value to be entered\n"
		"  MUTOOL <arguments>   Run any mutool command as specified\n"
	);
}

static char *
my_getline(FILE *file)
{
	int c;
	char *d = getline_buffer;
	int space = sizeof(getline_buffer)-1;

	/* Skip over any prefix of whitespace */
	do
	{
		c = fgetc(file);
	}
	while (c > 0 && isspace(c));

	// abort on EOF, error or when you encounter an illegal NUL byte in the script
	if (c <= 0)
		return NULL;

	/* Read the line in */
	do
	{
		*d++ = (char)c;
		c = fgetc(file);
	}
	while (c >= 32 && --space);

	/* If we ran out of space, skip the rest of the line */
	if (space == 0)
	{
		fz_error(ctx, "getline: line too long.");
		while (c >= 32)
			c = fgetc(file);
	}

	*d = 0;

	return getline_buffer;
}

static int
match(char **line, const char *match)
{
	char *s = *line;

	if (s == NULL)
		return 0;

	while (isspace(*(unsigned char *)s))
		s++;

	while (*s == *match)
	{
		if (*s == 0)
		{
			*line = s;
			return 1;
		}
		s++;
		match++;
	}

	if (*match != 0)
		return 0;

	/* We matched! Skip over any whitespace */
	while (isspace(*(unsigned char *)s))
		s++;

	*line = s;

	/* Trim whitespace off the end of the line */
	/* Run to the end of the line */
	while (*s)
		s++;

	/* Run back until we find where we started, or non whitespace */
	while (s != *line && isspace((unsigned char)s[-1]))
		s--;

	/* Remove the suffix of whitespace */
	*s = 0;

	return 1;
}

static void unescape_string(char *d, const char *s)
{
	char c;

	while ((c = *s++) != 0)
	{
		if (c == '\\')
		{
			c = *s++;
			switch(c)
			{
			case 'n':
				c = '\n';
				break;
			case 'r':
				c = '\r';
				break;
			case 't':
				c = '\t';
				break;
			}
		}
		*d++ = c;
	}
	*d = 0;
}

static void convert_string_to_argv(fz_context* ctx, const char*** argv, int* argc, char* line)
{
	int count = 0;
	size_t len = strlen(line);

	// allocate supra worst-case number of start+end slots for line decoding
	// PLUS enough space to store the (dequoted) argument strings themselves:
	char** start = fz_malloc(ctx, len * sizeof(start[0]) + len + 2);
	*argv = start;
	*argc = 0;

	char* buf = (char*)&start[len];
	strcpy(buf, line);
	buf[len + 1] = 0;  // make sure there's a double NUL sentinel at the end.

	char* s = buf;

	while (*s)
	{
		// skip leading whitespace:
		while (*s && isspace(*(unsigned char*)s))
			s++;

		// if double-quote, assume quoted string: find next (unescaped) doublequote:
		if (*s == '"')
		{
			char* p = s + 1;
			char* e = strchr(p, *s);
			const char* esc = strchr(p, '\\');

			while (e)
			{
				// see if the doublequote we find is escaped:
				// algo = skip all escaped chars until we hit or pass over the doublequote:
				while (esc && esc < e - 1)
				{
					esc++; // skip escaped character
					esc = strchr(esc + 1, '\\');
				}
				if (esc != e - 1)
					break;
				// doublequote is escaped: skip
				e = strchr(e + 1, *s);
			}

			if (!e)
			{
				fz_throw(ctx, FZ_ERROR_GENERIC, "MUTOOL command error: unterminated string parameter: %s", s);
				return;
			}

			// point at terminating quote, check if it is followed by whitespace or EOL:
			if (e[1] != 0 && !isspace((unsigned char)e[1]))
			{
				fz_throw(ctx, FZ_ERROR_GENERIC, "MUTOOL command error: whitespace or end of command expected after quoted string parameter: %s", s);
				return;
			}
			*e = 0;

			unescape_string(buf, s + 1);
			start[count++] = buf;
			buf += strlen(buf) + 1;

			s = e + 1;
		}
		else
		{
			// assume regular arg: sentinel is first whitespace:
			char* e = s;
			while (*e && !isspace(*(unsigned char*)e))
				e++;
			*e = 0;
			if (buf != s)
				strcpy(buf, s);
			start[count++] = buf;
			buf += strlen(buf) + 1;

			s = e;
		}
		s++;
	}

	// end argv[] with a sentinel NULL:
	start[count] = NULL;

	*argc = count;
}

struct logconfig
{
	int quiet;
	FILE* logfile;
};

static struct logconfig logcfg = { 0 };

static void mu_drop_context(void)
{
	if (showtime && timing.count > 0)
	{
		timediff_t duration = Curl_timediff(Curl_now(), timing.start_time);

		fz_info(ctx, "total %lldms / %d commands for an average of %lldms in %d commands",
			timing.total / 1000, timing.count, timing.total / (1000 * timing.count), timing.count);
		fz_info(ctx, "fastest command line %d: %lldms (%s)", timing.minscriptline, timing.min / 1000, timing.mincommand);
		fz_info(ctx, "slowest command line %d: %lldms (%s)", timing.maxscriptline, timing.max / 1000, timing.maxcommand);

		// reset timing after reporting: this atexit handler MAY be invoked multiple times!
		memset(&timing, 0, sizeof(timing));
	}

	if (mujstest_is_toplevel_ctx)
	{
		if (trace_info.allocs && (trace_info.mem_limit || trace_info.alloc_limit || showmemory))
		{
			fz_info(ctx, "Memory use total=%zu peak=%zu current=%zu", trace_info.total, trace_info.peak, trace_info.current);
			fz_info(ctx, "Allocations total=%zu", trace_info.allocs);

			// reset heap tracing after reporting: this atexit handler MAY be invoked multiple times!
			memset(&trace_info, 0, sizeof(trace_info));
		}
	}

	if (logcfg.logfile)
	{
		fclose(logcfg.logfile);
		logcfg.logfile = NULL;
	}

	fz_free(ctx, timing.mincommand);
	fz_free(ctx, timing.maxcommand);
	timing.mincommand = NULL;
	timing.maxcommand = NULL;

	assert(!ctx || (ctx->error.top == ctx->error.stack));

	fz_drop_context(ctx); // also done here for those rare exit() calls inside the library code.
	ctx = NULL;

	// nuke the locks last as they are still used by the heap free ('drop') calls in the lines just above!
	if (mujstest_is_toplevel_ctx)
	{
		// as we registered a global context, we should clean the locks on it now
		// so the atexit handler won't have to bother with it.
		assert(fz_has_global_context());
		ctx = fz_get_global_context();
		fz_drop_context_locks(ctx);
		ctx = NULL;

		fz_drop_global_context();

#ifndef DISABLE_MUTHREADS
		fin_mudraw_locks();
#endif /* DISABLE_MUTHREADS */

		mujstest_is_toplevel_ctx = 0;
	}
}

static void show_progress_on_stderr(struct logconfig* logcfg, const char *message)
{
	if (!logcfg->quiet)
	{
		FILE* logfile = (logcfg && logcfg->logfile) ? logcfg->logfile : stderr;

		// show progress on stderr, while we log the real data to logfile:
		if (logfile != stderr)
		{
			if (!strncmp(message, "OK:", 3))
				fprintf(stderr, "#");
			else if (!strncmp(message, "ERR:", 4))
				fprintf(stderr, "/");
			else if (strstr(message, "*!*"))
				fprintf(stderr, "*!*");
			else
				fprintf(stderr, ".");
		}
	}
}

static void tst_error_callback(void* user, const char* message)
{
	struct logconfig* logcfg = (struct logconfig*)user;
	FILE* logfile = (logcfg && logcfg->logfile) ? logcfg->logfile : stderr;

	// show progress on stderr, while we log the real data to logfile:
	show_progress_on_stderr(logcfg, message);
	fprintf(logfile, "error: %s\n", message);
	fflush(logfile);
#ifdef USE_OUTPUT_DEBUG_STRING
	OutputDebugStringA("error: ");
	OutputDebugStringA(message);
	OutputDebugStringA("\n");
#endif
}

static void tst_warning_callback(void* user, const char* message)
{
	struct logconfig* logcfg = (struct logconfig*)user;
	FILE* logfile = (logcfg && logcfg->logfile) ? logcfg->logfile : stderr;

	if (!logcfg->quiet)
	{
		// show progress on stderr, while we log the real data to logfile:
		show_progress_on_stderr(logcfg, message);
		fprintf(logfile, "warning: %s\n", message);
		fflush(logfile);
#ifdef USE_OUTPUT_DEBUG_STRING
		OutputDebugStringA("warning: ");
		OutputDebugStringA(message);
		OutputDebugStringA("\n");
#endif
	}
}

static void tst_info_callback(void* user, const char* message)
{
	struct logconfig* logcfg = (struct logconfig*)user;
	FILE* logfile = (logcfg && logcfg->logfile) ? logcfg->logfile : stderr;

	if (!logcfg->quiet)
	{
		// show progress on stderr, while we log the real data to logfile:
		show_progress_on_stderr(logcfg, message);
		fprintf(logfile, "%s\n", message);
#ifdef USE_OUTPUT_DEBUG_STRING
		OutputDebugStringA(message);
		OutputDebugStringA("\n");
#endif
	}
}

int
main(int argc, const char *argv[])
{
	FILE *script = NULL;
	int c;
	int errored = 0;
	unsigned int linecounter = 0;
	int rv = 0;
	struct curltime begin_time;
	const char* line_command = NULL;
	fz_alloc_context trace_alloc_ctx = { &trace_info, trace_malloc, trace_realloc, trace_free };
	fz_alloc_context *alloc_ctx = NULL;
	fz_locks_context *locks = NULL;
	size_t max_store = FZ_STORE_DEFAULT;
	int lowmemory = 0;

	// reset global vars: this tool MAY be re-invoked via mujstest or others and should RESET completely between runs:
	mujstest_is_toplevel_ctx = 0;
	ctx = NULL;

	showtime = 0;
	showmemory = 0;

	memset(&trace_info, 0, sizeof trace_info);
	memset(&logcfg, 0, sizeof logcfg);
	memset(&timing, 0, sizeof(timing));
	timing.min = 1 << 30;

	fz_getopt_reset();
	while ((c = fz_getopt(argc, argv, "o:p:LvqVm:s:h")) != -1)
	{
		switch(c)
		{
		case 'o': output = fz_optarg; break;
		case 'p': prefix = fz_optarg; break;
		case 'q': logcfg.quiet = 1; break;
		case 'v': verbosity ^= 1; break;
		case 's':
			if (strchr(fz_optarg, 't')) ++showtime;
			if (strchr(fz_optarg, 'm')) ++showmemory;
			break;
		case 'm':
			if (fz_optarg[0] == 's') trace_info.mem_limit = fz_atoi64(&fz_optarg[1]);
			else if (fz_optarg[0] == 'a') trace_info.alloc_limit = fz_atoi64(&fz_optarg[1]);
			else trace_info.mem_limit = fz_atoi64(fz_optarg);
			break;
		case 'L': lowmemory = 1; break;

		case 'V': fz_info(ctx, "mujstest version %s", FZ_VERSION); return EXIT_FAILURE;

		default: usage(); return EXIT_FAILURE;
		}
	}

	if (fz_optind == argc)
	{
		fz_error(ctx, "No files specified to process\n\n");
		usage();
		return EXIT_FAILURE;
	}

#ifndef DISABLE_MUTHREADS
	locks = init_mudraw_locks();
	if (locks == NULL)
	{
		fz_error(NULL, "mutex initialisation failed");
		return EXIT_FAILURE;
	}
#endif

	if (trace_info.mem_limit || trace_info.alloc_limit || showmemory)
		alloc_ctx = &trace_alloc_ctx;

	if (lowmemory)
		max_store = 1;

	if (!fz_has_global_context())
	{
		ctx = fz_new_context(alloc_ctx, locks, max_store);
		if (!ctx)
		{
			fz_error(ctx, "cannot initialise MuPDF context");
			return EXIT_FAILURE;
		}
		fz_set_global_context(ctx);

		fz_set_error_callback(ctx, tst_error_callback, &logcfg);
		fz_set_warning_callback(ctx, tst_warning_callback, &logcfg);
		fz_set_info_callback(ctx, tst_info_callback, &logcfg);

		mujstest_is_toplevel_ctx = 1;
	}
	atexit(mu_drop_context);

	ctx = fz_new_context(NULL, NULL, max_store);
	if (!ctx)
	{
		fz_error(ctx, "cannot initialise MuPDF context");
		return EXIT_FAILURE;
	}

	pdfapp_init(ctx, &gapp);
	gapp.scrw = 640;
	gapp.scrh = 480;
	gapp.colorspace = fz_device_rgb(ctx);

	fz_try(ctx)
	{
		timing.start_time = Curl_now();

		while (fz_optind < argc)
		{
			char logfilename[4096];

			scriptname = argv[fz_optind++];
			script = fopen(scriptname, "rb");
			if (script == NULL)
				fz_throw(ctx, FZ_ERROR_GENERIC, "cannot open script: %s", scriptname);

			fz_snprintf(logfilename, sizeof(logfilename), "%s.log", scriptname);
			logcfg.logfile = fopen(logfilename, "w");

			for(;;)
			{
				bool report_time = true;
				char *line = my_getline(script);
				line_command = line;
				rv = 0;

				linecounter++;
				fflush(logcfg.logfile);

				if (line == NULL)
				{
					if (ferror(script))
					{
						fz_error(ctx, "script read error: %s (%s)", strerror(errno), scriptname);
					}
					break;
				}
				if (verbosity)
				{
					fz_info(ctx, "L#%04d: %s", linecounter, line);
				}

				begin_time = Curl_now();

				if (match(&line, "%"))
				{
					/* Comment */
					report_time = false;
				}
				else if (match(&line, "PASSWORD"))
				{
					strcpy(pd_password, line);
					report_time = false;
				}
				else if (match(&line, "OPEN"))
				{
					char path[LONGLINE];
					if (file_open)
						pdfapp_close(&gapp);
					if (prefix)
					{
						sprintf(path, "%s%s", prefix, line);
					}
					else
					{
						strcpy(path, line);
					}
					pdfapp_open(&gapp, path, 0);
					file_open = 1;
				}
				else if (match(&line, "GOTO"))
				{
					pdfapp_gotopage(&gapp, atoi(line)-1);
				}
				else if (match(&line, "SCREENSHOT"))
				{
					char text[1024];

					fz_snprintf(text, sizeof(text), output, ++shotcount);
					if (strstr(text, ".pgm") || strstr(text, ".ppm") || strstr(text, ".pnm"))
						fz_save_pixmap_as_pnm(ctx, gapp.image, text);
					else
						fz_save_pixmap_as_png(ctx, gapp.image, text);
				}
				else if (match(&line, "RESIZE"))
				{
					int w, h;
					sscanf(line, "%d %d", &w, &h);
					pdfapp_onresize(&gapp, w, h);
				}
				else if (match(&line, "CLICK"))
				{
					float x, y, b;
					int n;
					n = sscanf(line, "%f %f %f", &x, &y, &b);
					if (n < 1)
						x = 0.0f;
					if (n < 2)
						y = 0.0f;
					if (n < 3)
						b = 1;
					/* state = 1 = transition down */
					pdfapp_onmouse(&gapp, (int)x, (int)y, b, 0, 1);
					/* state = -1 = transition up */
					pdfapp_onmouse(&gapp, (int)x, (int)y, b, 0, -1);
				}
				else if (match(&line, "TEXT"))
				{
					unescape_string(td_textinput, line);
					report_time = false;
				}
				else if (match(&line, "SET-PREFIX"))
				{
					unescape_string(prefix_buffer, line);
					prefix = prefix_buffer;
					report_time = false;
				}
				else if (match(&line, "SET-OUTPUT"))
				{
					unescape_string(output_buffer, line);
					output = output_buffer;
					report_time = false;
				}
				else if (match(&line, "CD"))
				{
					char path[LONGLINE];

					unescape_string(path, line);

					// expand dir macro {SCRIPTDIR} if it exists:
					const char* m = strstr(path, "{SCRIPTDIR}");
					if (m) {
						char buf[LONGLINE];
						char* l;
						char* k;

						memset(buf, 0, sizeof(buf));
						strncpy(buf, path, m - path);
						strncat(buf, scriptname, sizeof(buf));
						k = strrchr(buf, '/');
						l = strrchr(buf, '\\'); // Windows paths...
						if (l > k) k = l;
						if (k) *k = 0;
						strncat(buf, m + 11, sizeof(buf));
						strncpy(path, buf, sizeof(path));
					}

					fz_chdir(ctx, path);
				}
				else if (match(&line, "MUTOOL"))
				{
					const char** argv = NULL;
					int argc = 0;
					convert_string_to_argv(ctx, &argv, &argc, line);
					rv = mutool_main(argc, argv);
					if (rv != EXIT_SUCCESS)
					{
						fz_error(ctx, "ERR: error executing MUTOOL command: %s", line);
						errored = 1;
					}
					else if (verbosity)
					{
						fz_info(ctx, "OK: MUTOOL command: %s", line);
					}
					fz_free(ctx, (void *)argv);
				}
				else
				{
					report_time = false;
					if (verbosity)
						fz_info(ctx, "Ignoring line without script statement.");
				}

				if (report_time)
				{
					struct curltime now = Curl_now();
					timediff_t diff = Curl_timediff(now, begin_time);
					fz_info(ctx, "L#%05u> T:%03dms D:%0.3lfs %s %s", linecounter, (int)diff, (double)Curl_timediff(now, timing.start_time) / 1E3, (rv ? "ERR" : "OK"), line_command);

					if (showtime)
					{
						if (diff < timing.min)
						{
							timing.min = diff;
							fz_free(ctx, timing.mincommand);
							timing.mincommand = fz_strdup(ctx, line_command);
							timing.minscriptline = linecounter;
						}
						if (diff > timing.max)
						{
							timing.max = diff;
							fz_free(ctx, timing.maxcommand);
							timing.maxcommand = fz_strdup(ctx, line_command);
							timing.maxscriptline = linecounter;
						}
						timing.total += diff;
						timing.count++;
					}
				}
			}
			while (!feof(script));

			fclose(script);
			script = NULL;
		}
	}
	fz_catch(ctx)
	{
		fz_error(ctx, "Failure when executing '%s': %s", scriptname, fz_caught_message(ctx));

		struct curltime now = Curl_now();

		fz_info(ctx, "L#%05u> T:%03dms D:%0.3lfs FAIL error: exception thrown in script file '%s' at line '%s': %s", linecounter, (int)Curl_timediff(now, begin_time), (double)Curl_timediff(now, timing.start_time) / 1E3, scriptname, (line_command ? line_command : "%--no-line--"), fz_caught_message(ctx));

		// also log to stderr if we haven't already:
		if (logcfg.logfile)
		{
			fprintf(stderr, "\nL#%05u> T:%03dms D:%0.3lfs FAIL error: exception thrown in script file '%s' at line '%s': %s\n", linecounter, (int)Curl_timediff(now, begin_time), (double)Curl_timediff(now, timing.start_time) / 1E3, scriptname, (line_command ? line_command : "%--no-line--"), fz_caught_message(ctx));
		}

		errored = 1;
	}

	if (file_open)
		pdfapp_close(&gapp);

	fz_flush_warnings(ctx);
	mu_drop_context();

	return errored;
}
