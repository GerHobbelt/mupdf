
#if defined(_MSC_VER)
#ifndef _CRTDBG_MAP_ALLOC
#define _CRTDBG_MAP_ALLOC
#endif
#include <crtdbg.h>
#endif

#include "timeval.h"

#include "mupdf/fitz.h"
#include "mupdf/helpers/dir.h"
#include "mupdf/assert.h"

#ifndef DISABLE_MUTHREADS
#include "mupdf/helpers/mu-threads.h"
#endif

#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#if defined(_WIN32)
#include <windows.h>
#endif

int mutool_main(int argc, const char** argv);

#define LONGLINE 4096

static inline void memclr(void* ptr, size_t size)
{
	memset(ptr, 0, size);
}

static fz_context* ctx = NULL;

/*
	In the presence of pthreads or Windows threads, we can offer
	a multi-threaded option. In the absence of such we degrade
	nicely.
*/
#ifndef DISABLE_MUTHREADS

static mu_mutex mutexes[FZ_LOCK_MAX];
static mu_mutex heap_debug_mutex;
static int heap_debug_mutex_is_initialized;

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
	heap_debug_mutex_is_initialized = 0;
	mu_destroy_mutex(&heap_debug_mutex);
}

static fz_locks_context *init_mudraw_locks(void)
{
	int i;
	int failed = 0;

	for (i = 0; i < FZ_LOCK_MAX; i++)
		failed |= mu_create_mutex(&mutexes[i]);
	failed |= mu_create_mutex(&heap_debug_mutex);
	heap_debug_mutex_is_initialized = !failed;

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

static int bulktest_is_toplevel_ctx = 0;

struct timing {
	struct curltime start_time;
	int count;
	timediff_t total;
	timediff_t min, max;
	int minscriptline, maxscriptline;
	const char* mincommand;
	const char* maxcommand;
};

static struct timing timing;

// Note: this allocated block' header must stick to the alignment agreement of the system/RTL: 128 bits (32 bytes) on x86 at least.
// It is also handy to stick to that alignment rule for other SEE/AVX/NEON platforms' ease of use.
typedef
#if defined(_MSC_VER)
__declspec(align(32))
#else 
__attribute__((aligned(32))
#endif
struct trace_header
{
	size_t size;
	size_t seqnum;
	size_t magic;

	// heap debugging:
	int origin_line;
	const char* origin_file;
	struct trace_header* prev;
	struct trace_header* next;
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

	// heap debugging:
	struct trace_header* last;
};

static struct trace_info trace_info = { 1 };

static void clear_trace_info(void)
{
	int seqnr = trace_info.sequence_number;
	memclr(&trace_info, sizeof(trace_info));
	trace_info.sequence_number = seqnr;
}

static void *hit_limit(void *val)
{
	return val;
}

static void *hit_memory_limit(struct trace_info *info, int is_malloc, size_t oldsize, size_t size)
{
	ASSERT(info == &trace_info);
	if (is_malloc)
		fz_error(ctx, "Memory limit (%zu) hit upon malloc(%zu) when %zu already allocated.", info->mem_limit, size, info->current);
	else
		fz_error(ctx, "Memory limit (%zu) hit upon realloc(%zu) from %zu bytes when %zu already allocated.", info->mem_limit, size, oldsize, info->current);
	return hit_limit(NULL);
}


static void *hit_alloc_limit(struct trace_info *info, int is_malloc, size_t oldsize, size_t size)
{
	ASSERT(info == &trace_info);
	if (is_malloc)
		fz_error(ctx, "Allocation limit (%zu) hit upon malloc(%zu) when %zu already allocated.", info->alloc_limit, size, info->current);
	else
		fz_error(ctx, "Allocation limit (%zu) hit upon realloc(%zu) from %zu bytes when %zu already allocated.", info->alloc_limit, size, oldsize, info->current);
	return hit_limit(NULL);
}

static void *
trace_malloc(void *arg, size_t size, const char *srcfile, int srcline)
{
	struct trace_info *info = (struct trace_info *) arg;
	ASSERT(info == &trace_info);
	trace_header *p;
	if (size == 0)
		return NULL;
	if (size > SIZE_MAX - sizeof(trace_header))
		return NULL;
	if (info->mem_limit > 0 && size > info->mem_limit - info->current)
		return hit_memory_limit(info, 1, 0, size);
	if (info->alloc_limit > 0 && info->allocs > info->alloc_limit)
		return hit_alloc_limit(info, 1, 0, size);
	p = _malloc_dbg(size + sizeof(trace_header), _NORMAL_BLOCK, srcfile, srcline);
	if (p == NULL)
		return NULL;
	p[0].size = size;
	p[0].magic = 0xEAD;
	p[0].seqnum = info->sequence_number++;
	p[0].origin_file = srcfile;
	p[0].origin_line = srcline;

	if (heap_debug_mutex_is_initialized)
		mu_lock_mutex(&heap_debug_mutex);
	if (info->last)
	{
		ASSERT(info->last->next == NULL);
		info->last->next = p;
	}
	p[0].prev = info->last;
	p[0].next = NULL;
	info->last = p;
	if (heap_debug_mutex_is_initialized)
		mu_unlock_mutex(&heap_debug_mutex);

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
	ASSERT(info == &trace_info);
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
		if (heap_debug_mutex_is_initialized)
			mu_lock_mutex(&heap_debug_mutex);
		struct trace_header* next = p[-1].next;
		struct trace_header* prev = p[-1].prev;
		if (next)
		{
			ASSERT(next->prev == &p[-1]);
		}
		if (prev)
		{
			ASSERT(prev->next == &p[-1]);
		}
		if (next)
		{
			next->prev = prev;
		}
		if (prev)
		{
			prev->next = next;
		}
		if (info->last == &p[-1])
		{
			ASSERT(next == NULL);
			info->last = prev;
		}
		if (heap_debug_mutex_is_initialized)
			mu_unlock_mutex(&heap_debug_mutex);

		free(&p[-1]);
	}
}

static void *
trace_realloc(void *arg, void *p_, size_t size, const char* srcfile, int srcline)
{
	struct trace_info *info = (struct trace_info *) arg;
	ASSERT(info == &trace_info);
	trace_header *p = (trace_header *)p_;

	if (size == 0)
	{
		trace_free(arg, p_);
		return NULL;
	}

	if (p_ == NULL)
		return trace_malloc(arg, size, srcfile, srcline);

	if (size > SIZE_MAX - sizeof(trace_header))
		return NULL;

	if (info->mem_limit > 0 && size > info->mem_limit - info->current + p[-1].size)
		return hit_memory_limit(info, 0, p[-1].size, size);
	if (info->alloc_limit > 0 && info->allocs > info->alloc_limit)
		return hit_alloc_limit(info, 0, p[-1].size, size);

	int rotten = 0;
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
		trace_header old = p[-1];
		trace_header* old_p = &p[-1];
		p = _realloc_dbg(&p[-1], size + sizeof(trace_header), _NORMAL_BLOCK, srcfile, srcline);
		if (p == NULL)
			return NULL;
		info->current += size - old.size;
		if (size > old.size)
			info->total += size - old.size;
		if (info->current > info->peak)
			info->peak = info->current;
		p[0].origin_file = srcfile;
		p[0].origin_line = srcline;
		p[0].size = size;
		info->allocs++;

		if (heap_debug_mutex_is_initialized)
			mu_lock_mutex(&heap_debug_mutex);
		if (old.next)
		{
			ASSERT(old.next->prev == old_p);
		}
		if (old.prev)
		{
			ASSERT(old.prev->next == old_p);
		}
		if (old.next)
		{
			old.next->prev = p;
		}
		if (old.prev)
		{
			old.prev->next = p;
		}
		if (info->last == old_p)
		{
			ASSERT(old.next == NULL);
			info->last = p;
		}
		if (heap_debug_mutex_is_initialized)
			mu_unlock_mutex(&heap_debug_mutex);

		return &p[1];
	}
}

static size_t
trace_snapshot(void)
{
	return trace_info.sequence_number;
}

static void
trace_report_pending_allocations_since(size_t snapshot_id)
{
	if (heap_debug_mutex_is_initialized)
		mu_lock_mutex(&heap_debug_mutex);

	trace_header* p = trace_info.last;
	ASSERT(p->next == NULL);
	int hits = 0;
	while (p && p->seqnum >= snapshot_id)
	{
		fprintf(stderr, "\nLEAK? #%zu (size: %zu) (origin: %s(%d))", p->seqnum, p->size, p->origin_file, p->origin_line);
		hits++;
		
		p = p->prev;
	}
	if (!hits)
	{
		//fprintf(stderr, "\nNo leaks found.\n");
	}
	else
	{
		fprintf(stderr, "\n");
	}

	if (heap_debug_mutex_is_initialized)
		mu_unlock_mutex(&heap_debug_mutex);
}

typedef size_t fz_trace_snapshot_f(void);
typedef void fz_trace_report_pending_allocations_since_f(size_t snapshot_id);

extern __declspec(dllimport) fz_trace_snapshot_f* fz_trace_snapshot;
extern __declspec(dllimport) fz_trace_report_pending_allocations_since_f* fz_trace_report_pending_allocations_since;


static const char *prefix = NULL;
static int verbosity = 0;

static char getline_buffer[LONGLINE];


static void usage(void)
{
	fz_info(ctx,
		"bulktest: Scriptable tester for mupdf + js\n"
		"\n"
		"Syntax: bulktest [options] <scriptfile> [<datafile> ...]\n"
		"\n"
		"Options:\n"
		"  -T      scriptfile is a script TEMPLATE file, which is filled with data from\n"
		"          the datafiles, one line at a time, and repeated until all datafile records\n"
		"          have been processed that way.\n"
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
		"  CD <path>            Change current directory to the indicated path\n"
		"  MUTOOL <arguments>   Run any mutool command as specified\n"
	);
}

static const char *
mk_absolute_path(fz_context* ctx, const char* filepath)
{
	char buf[LONGLINE];
	char* q;
#ifdef _MSC_VER
	q = _fullpath(buf, filepath, sizeof(buf));
#else
	q = realpath(filepath, buf);
#endif
	if (!q) {
		fz_throw(ctx, FZ_ERROR_GENERIC, "Cannot get absolute path for script file %q.", filepath);
	}
	return fz_strdup(ctx, q);
}

// Get a line of text from file.
// Return NULL at EOF. Return a reference to a static buffer containing a string value otherwise.
static char *
my_getline(FILE *file, int *linecounter_ref)
{
	int c;
	char *d = getline_buffer;
	int space = sizeof(getline_buffer)-1;

	/* Skip over any prefix of whitespace */
	do
	{
		c = fgetc(file);
		if (c == '\n')
			(*linecounter_ref)++;
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
		if (c == '\n')
			(*linecounter_ref)++;
	}
	while (c >= 32 && --space);

	/* If we ran out of space, skip the rest of the line */
	if (space == 0)
	{
		fz_error(ctx, "getline: line too long.");
		while (c >= 32)
		{
			c = fgetc(file);
			if (c == '\n')
				(*linecounter_ref)++;
		}
	}

	*d = 0;

	return getline_buffer;
}

static void
expand_template_variables(fz_context* ctx, const char** argv, int linecounter, int varcount, const char** vars, int datalinecounter, const char *scriptfilename, const char *dataspecfilename)
{
	for (const char** a = argv; *a; a++)
	{
		const char* arg = *a;

		// first copy `line` to internal buffer as it'll be pointing at getline_buffer
		int space = strlen(arg) + LONGLINE;
		char* dst = fz_malloc(ctx, space + 1);
		strncpy(dst, arg, space);
		dst[space - 1] = 0;
		dst[space] = 0;
		const char* s = arg;
		char* d = dst;
		const char marker = '%';
		int has_expansion = 0;

		while (*s)
		{
			const char* m = strchr(s, marker);
			if (!m)
			{
				strncpy(d, s, space);
				break;
			}
			has_expansion = 1;
			if (m > s)
			{
				size_t l = m - s;
				if (l >= space)
					fz_throw(ctx, FZ_ERROR_GENERIC, "out of template expansion buffer space.");
				memcpy(d, s, l);
				space -= l;
				d += l;
			}
			m++;
			// escape: double marker -> marker
			if (*m == marker)
			{
				if (1 >= space)
					fz_throw(ctx, FZ_ERROR_GENERIC, "out of template expansion buffer space.");

				*d++ = *m++;
				space--;
				s = m;
			}
			else if (strncmp(m, "datarow", 7) == 0 || strncmp(m, "{datarow}", 9) == 0)
			{
				size_t skip_dist = (strncmp(m, "datarow", 7) == 0 ? 7 : 9);
				// `%datarow`: print the current dataline number.
				fz_snprintf(d, space, "%05d", datalinecounter);
				if (strlen(d) >= space)
					fz_throw(ctx, FZ_ERROR_GENERIC, "out of template expansion buffer space.");
				s = m + skip_dist;
				size_t l = strlen(d);
				d += l;
				space -= l;
			}
			else if (strncmp(m, "dataspecdir", 11) == 0 || strncmp(m, "{dataspecdir}", 13) == 0)
			{
				size_t skip_dist = (strncmp(m, "dataspecdir", 11) == 0 ? 11 : 13);
				// `%dataspecdir`: print the directory part of the (absolute) data spec file path `dataspecfilename`.
				const char* path_end = strrchr(dataspecfilename, '/');
				if (!path_end)
					path_end = strrchr(dataspecfilename, '\\');
				// do NOT include the terminating / path sep, UNLESS we're looking at the UNIX root directory itself:
				if (path_end == dataspecfilename)
					path_end++;
				else if (!path_end)
					path_end = dataspecfilename;

				size_t pathlen = path_end - dataspecfilename;
				if (pathlen >= space)
					fz_throw(ctx, FZ_ERROR_GENERIC, "out of template expansion buffer space.");
				strncpy(d, dataspecfilename, pathlen);
				d[pathlen] = 0;
				s = m + skip_dist;
				d += pathlen;
				space -= pathlen;
			}
			else if (strncmp(m, "scriptdir", 9) == 0 || strncmp(m, "{scriptdir}", 11) == 0)
			{
				size_t skip_dist = (strncmp(m, "scriptdir", 9) == 0 ? 9 : 11);
				// `%scriptdir`: print the directory part of the (absolute) script file path `scriptfilename`.
				const char* path_end = strrchr(scriptfilename, '/');
				if (!path_end)
					path_end = strrchr(scriptfilename, '\\');
				// do NOT include the terminating / path sep, UNLESS we're looking at the UNIX root directory itself:
				if (path_end == scriptfilename)
					path_end++;
				else if (!path_end)
					path_end = dataspecfilename;

				size_t pathlen = path_end - scriptfilename;
				if (pathlen >= space)
					fz_throw(ctx, FZ_ERROR_GENERIC, "out of template expansion buffer space.");
				strncpy(d, scriptfilename, pathlen);
				d[pathlen] = 0;
				s = m + skip_dist;
				d += pathlen;
				space -= pathlen;
			}
			else if (!*m || !strchr("123456789", *m))
			{
				// when marker isn't immediately followed by a decimal number (without leading zeroes),
				// then it's not a template marker either. Copy marker verbatim.
				if (1 >= space)
					fz_throw(ctx, FZ_ERROR_GENERIC, "out of template expansion buffer space.");

				*d++ = marker;
				space--;
                s = m;
			}
			else
			{
				char* e = NULL;
				int param_index = (int)strtol(m, &e, 10);  // %NNN parameters are expected to range 1..argc
				if (param_index <= 0 || param_index > varcount)
				{
					fz_throw(ctx, FZ_ERROR_GENERIC, "TEST: error at script line %d: script template parameter index %d is out of available range 1..%d.", linecounter, param_index, varcount);
				}
				const char* tpl_arg = vars[param_index - 1];
				size_t l = strlen(tpl_arg);
				if (l >= space)
					fz_throw(ctx, FZ_ERROR_GENERIC, "out of template expansion buffer space.");
				strncpy(d, tpl_arg, space);
				s = e;
				d += l;
				space -= l;
			}
		}

		if (!has_expansion)
		{
			fz_free(ctx, dst);
		}
		else
		{
			fz_free(ctx, arg);
			*a = dst;
		}
	}
}


static int
match(const char *arg, const char *match)
{
	if (arg == NULL || match == NULL)
		return 0;

	return strcmp(arg, match) == 0;
}

static int unescape_string(char *d, const char *s)
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
			case '\\':
				break;
			default:
				return 1;
			}
		}
		*d++ = c;
	}
	*d = 0;

	return 0;
}

// convert line to arguments, starting at start_index.
static void convert_string_to_argv(fz_context* ctx, const char*** argv, int* argc, char* line, int start_index)
{
	int count = 0;
	size_t len = strlen(line);

	// allocate supra worst-case number of start+end+sentinel slots for line decoding
	char** start = fz_malloc(ctx, (len + start_index + 1) * sizeof(start[0]));
	*argv = start;
	*argc = 0;

	char* buf = fz_malloc(ctx, len + 2);
	strcpy(buf, line);
	buf[len + 1] = 0;  // make sure there's a double NUL sentinel at the end.

	for (; start_index > 0; start_index--)
	{
		start[count++] = fz_strdup(ctx, "bulktest");
	}

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

			char* buf2 = fz_malloc(ctx, (e - s) + 2);
			if (unescape_string(buf2, s + 1))
			{
				fz_throw(ctx, FZ_ERROR_GENERIC, "Invalid escape in line: %s", s);
				return;
			}
			start[count++] = buf2;

			s = e + 1;
		}
		else
		{
			// assume regular arg: sentinel is first whitespace:
			char* e = s;
			while (*e && !isspace(*(unsigned char*)e))
				e++;
			*e = 0;
			char *buf2 = fz_strdup(ctx, s);
			start[count++] = buf2;

			s = e;
		}
		s++;
	}

	fz_free(ctx, buf);

	ASSERT(count < (len + start_index + 1));

	// end argv[] with a sentinel NULL:
	start[count] = NULL;

	*argc = count;
}

static void fz_free_argv_array(fz_context* ctx, const char** argv)
{
	if (!argv)
		return;

	for (const char** p = argv; *p; p++)
	{
		fz_free(ctx, *p);
	}
	fz_free(ctx, argv);
}

struct logconfig
{
	int quiet;
	FILE* logfile;
	const char* logfilepath;
};

static struct logconfig logcfg = { 0 };

static void close_active_logfile(void)
{
	if (logcfg.logfile)
	{
		fclose(logcfg.logfile);
		logcfg.logfile = NULL;
	}
}

// at least on Windows, you loose your logfile contents if you don't *close* the blasted file!
//
// Hence this is a hard flush, which writes the logfile, then *re-opens* it in *append mode*.
static void flush_active_logfile_hard(void)
{
	long file_size = 0;

	if (logcfg.logfile)
	{
		file_size = ftell(logcfg.logfile);
		fclose(logcfg.logfile);
		logcfg.logfile = NULL;
	}

	// append when the logfile is not very huge yet, otherwise 'rotate':
	if (logcfg.logfilepath)
	{
		// logfiles are cut up into ~20MB chunks:
		if (file_size < 20000000)
		{
			logcfg.logfile = fopen(logcfg.logfilepath, "a");
		}
		else
		{
			char logfilename[LONGLINE];
			char basename[LONGLINE];
			strncpy(basename, logcfg.logfilepath, sizeof(basename));
			char* dotp = strrchr(basename, '.');
			*dotp = 0;
			fz_snprintf(logfilename, sizeof(logfilename), "%s.log", basename);
			int count = 1;
			while (fz_file_exists(ctx, logfilename))
			{
				// rename old logfile:
				fz_snprintf(logfilename, sizeof(logfilename), "%s.%04d.log", basename, count++);
			}
			if (strcmp(logcfg.logfilepath, logfilename))
			{
				(void)rename(logcfg.logfilepath, logfilename);
				int errcode = errno;
				if (fz_file_exists(ctx, logcfg.logfilepath))
				{
					fz_throw(ctx, FZ_ERROR_GENERIC, "%s: failed to rename old logfile %q to %q.", errcode ? strerror(errcode) : "Unknown rename error", logfilename, logcfg.logfilepath);
				}
			}

			logcfg.logfile = fopen(logcfg.logfilepath, "w");
		}
	}
}

static void open_logfile(const char* scriptname)
{
	char logfilename[LONGLINE];
	char logfilename_0[LONGLINE];

	fz_snprintf(logfilename, sizeof(logfilename), "%s.log", scriptname);
	strncpy(logfilename_0, logfilename, sizeof(logfilename_0));
	int count = 1;
	while (fz_file_exists(ctx, logfilename))
	{
		// rename old logfile:
		fz_snprintf(logfilename, sizeof(logfilename), "%s.%04d.log", scriptname, count++);
	}
	if (strcmp(logfilename_0, logfilename))
	{
		(void)rename(logfilename_0, logfilename);
		int errcode = errno;
		if (fz_file_exists(ctx, logfilename_0))
		{
			fz_throw(ctx, FZ_ERROR_GENERIC, "%s: failed to rename old logfile %q to %q.", errcode ? strerror(errcode) : "Unknown rename error", logfilename, logfilename_0);
		}
	}

	logcfg.logfile = fopen(logfilename_0, "w");
	logcfg.logfilepath = fz_strdup(ctx, logfilename_0);
}

static float humanize(size_t value, const char** unit)
{
	float result;
	if (value >= 1024 * 1024 * 1024.0f)
	{
		*unit = "G";
		result = value / (1024 * 1024 * 1024.0f);
	}
	else if (value >= 1024 * 1024.0f)
	{
		*unit = "M";
		result = value / (1024 * 1024.0f);
	}
	else if (value >= 1024.0f)
	{
		*unit = "k";
		result = value / 1024.0f;
	}
	else
	{
		*unit = "";
		result = value;
	}

	return result;
}

static void mu_drop_context(void)
{
	if (!ctx && fz_has_global_context())
	{
		ASSERT(!"Should never get here.");
		ctx = fz_get_global_context();
	}
	if (ctx)
	{
		if (showtime)
		{
			timediff_t duration = Curl_timediff(Curl_now(), timing.start_time);

			if (timing.count > 0)
			{
				fz_info(ctx, "total %lldms / %d commands for an average of %lldms in %d commands",
					timing.total / 1000, timing.count, timing.total / (1000 * timing.count), timing.count);
				fz_info(ctx, "fastest command line %d: %lldms (%s)", timing.minscriptline, timing.min / 1000, timing.mincommand);
				fz_info(ctx, "slowest command line %d: %lldms (%s)", timing.maxscriptline, timing.max / 1000, timing.maxcommand);

				// reset timing after reporting: this atexit handler MAY be invoked multiple times!
				memclr(&timing, sizeof(timing));
			}

			fz_dump_lock_times(ctx, duration);
		}
		showtime = 0;

		if (trace_info.allocs && (trace_info.mem_limit || trace_info.alloc_limit || showmemory))
		{
			float total, peak, current;
			const char* total_unit;
			const char* peak_unit;
			const char* current_unit;

			total = humanize(trace_info.total, &total_unit);
			peak = humanize(trace_info.peak, &peak_unit);
			current = humanize(trace_info.current, &current_unit);

			fz_info(ctx, "Memory use total=%.2f%s peak=%.2f%s current=%.2f%s",
				total, total_unit, peak, peak_unit, current, current_unit);
			fz_info(ctx, "Allocations total=%zu", trace_info.allocs);

			// reset heap tracing after reporting: this atexit handler MAY be invoked multiple times!
			clear_trace_info();
		}

		close_active_logfile();
		fz_free(ctx, logcfg.logfilepath);
		logcfg.logfilepath = NULL;

		fz_free(ctx, timing.mincommand);
		fz_free(ctx, timing.maxcommand);
		timing.mincommand = NULL;
		timing.maxcommand = NULL;
	}
	ASSERT(!ctx || (ctx->error.top == ctx->error.stack_base));

	fz_drop_context(ctx); // also done here for those rare exit() calls inside the library code.
	ctx = NULL;

	// nuke the locks last as they are still used by the heap free ('drop') calls in the lines just above!
	if (bulktest_is_toplevel_ctx)
	{
		// as we registered a global context, we should clean the locks on it now
		// so the atexit handler won't have to bother with it.
		//
		// Note: our atexit handler CAN be invoked multiple times, so the second time through (etc.),
		// we will ALREADY have cleaned up the global context: DO NOT attempt to recreate it implicitly
		// then via fz_get_global_context() internals!
		if (fz_has_global_context())
		{
			ctx = fz_get_global_context();
		}
		fz_drop_context_locks(ctx);

		ctx = NULL;

		fz_drop_global_context();

#ifndef DISABLE_MUTHREADS
		fin_mudraw_locks();
#endif /* DISABLE_MUTHREADS */

		bulktest_is_toplevel_ctx = 0;
	}
}

static void mu_drop_context_at_exit(void)
{
	// we're aborting/exiting the application.
	// One thing's for sure anyhow: we are NOT in a recursive call anymore, so we can/MUST say:
	bulktest_is_toplevel_ctx = 1;
	// and then go and clean up as best we can:
	mu_drop_context();
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
			{
				//fprintf(stderr, u8"✅");
				fprintf(stderr, "#");
			}
			else if (!strncmp(message, "ERR:", 4))
			{
				//fprintf(stderr, u8"❎");
				fprintf(stderr, "[E]");
			}
			else if (!strncmp(message, "::ECHO: ", 8))
			{
				fprintf(stderr, "\n%s", message + 8);
			}
			else if (!strncmp(message, "::SKIPPED: ", 11))
			{
				fprintf(stderr, "\n%s", message + 2);
			}
			else if (strstr(message, "*!*"))
			{
				fprintf(stderr, "*!*");
			}
			else
			{
				fprintf(stderr, ".");
			}
		}
	}
}

static fz_output* stddbgchannel(void)
{
	fz_output* dbg = fz_stddbg(ctx);
	fz_output* err = fz_stderr(ctx);

	if (dbg == err)
	{
		return NULL;
	}
	return dbg;
}


static void tst_error_callback(fz_context* ctx, void* user, const char* message)
{
	struct logconfig* logcfg = (struct logconfig*)user;
	FILE* logfile = (logcfg && logcfg->logfile) ? logcfg->logfile : stderr;

	// show progress on stderr, while we log the real data to logfile:
	show_progress_on_stderr(logcfg, message);
	fprintf(logfile, "error: %s\n", message);
	fflush(logfile);

	fz_output* dbg = stddbgchannel();
	if (dbg)
		fz_write_printf(ctx, dbg, "error: %s\n", message);
}

static void tst_warning_callback(fz_context* ctx, void* user, const char* message)
{
	struct logconfig* logcfg = (struct logconfig*)user;
	FILE* logfile = (logcfg && logcfg->logfile) ? logcfg->logfile : stderr;

	if (!logcfg->quiet)
	{
		// show progress on stderr, while we log the real data to logfile:
		show_progress_on_stderr(logcfg, message);
		fprintf(logfile, "warning: %s\n", message);
		fflush(logfile);

		fz_output* dbg = stddbgchannel();
		if (dbg)
			fz_write_printf(ctx, dbg, "warning: %s\n", message);
	}
}

static void tst_info_callback(fz_context* ctx, void* user, const char* message)
{
	struct logconfig* logcfg = (struct logconfig*)user;
	FILE* logfile = (logcfg && logcfg->logfile) ? logcfg->logfile : stderr;

	if (!logcfg->quiet)
	{
		// show progress on stderr, while we log the real data to logfile:
		show_progress_on_stderr(logcfg, message);
		fprintf(logfile, "%s\n", message);

		fz_output* dbg = stddbgchannel();
		if (dbg)
			fz_write_printf(ctx, dbg, "%s\n", message);
	}
}

struct range
{
	int first;
	int last;
};

static struct range* decode_numbers_rangespec(const char* spec)
{
	int n_ranges = 0;
	const char* range = spec;
	int spage, epage;
	const int pagecount = 1E9; // better looking heuristic than INT_MAX
	struct range* rv = NULL;

	if (range)
	{
		while ((range = fz_parse_page_range(ctx, range, &spage, &epage, pagecount)))
		{
			n_ranges++;
		}
		rv = fz_malloc(ctx, (n_ranges + 1) * sizeof(*rv));

		range = spec;
		int i = 0;
		while ((range = fz_parse_page_range(ctx, range, &spage, &epage, pagecount)))
		{
			rv[i].first = fz_maxi(0, spage);
			rv[i].last = fz_maxi(0, epage);
			i++;
		}
		// sentinel:
		rv[i].first = -1;
		rv[i].last = -1;
	}
	return rv;
}

static const char* test_fail_reason[] = {
	"PASS",
	/* 1 */ "empty line",
	/* 2 */ "rejected by match regex",
	/* 3 */ "rejected by ignore-match regex",
	/* 4 */ "rejected by match-ranges (line number range spec)",
	/* 5 */ "rejected by ignore-ranges (line number range spec)",
};

static int test_dataline_against_matchspecs(const char *line, int linenumber, const char *match_re, const char *ignore_match_re, const struct range *match_ranges_spec, const struct range* ignore_match_ranges_spec)
{
	if (!line || !*line)
		return 1;

	int rejected = 0;

	if (match_re && *match_re)
	{
		rejected = (strstr(line, match_re) ? 0 : 2);
	}
	if (!rejected && ignore_match_re && *ignore_match_re)
	{
		int ignore = !!strstr(line, ignore_match_re);
		if (ignore)
			rejected = 3;
	}
	if (!rejected && match_ranges_spec && match_ranges_spec[0].first != -1)
	{
		int hit = 4;
		for (; match_ranges_spec[0].first; match_ranges_spec++)
		{
			if (match_ranges_spec[0].first <= linenumber && match_ranges_spec[0].last >= linenumber)
			{
				hit = 0;
				break;
			}
		}
		rejected = hit;
	}
	if (!rejected && ignore_match_ranges_spec && ignore_match_ranges_spec[0].first != -1)
	{
		int hit = 0;
		for (; ignore_match_ranges_spec[0].first; ignore_match_ranges_spec++)
		{
			if (ignore_match_ranges_spec[0].first <= linenumber && ignore_match_ranges_spec[0].last >= linenumber)
			{
				hit = 1;
				break;
			}
		}
		if (hit)
			rejected = 5;
	}
	return rejected;
}

static const char* rangespec2str(char* buf, size_t bufsiz, const struct range* spec)
{
	if (!spec || spec->first == -1)
		return "-";

	buf[0] = 0;
	char* d = buf;
	for (; spec->first != -1; spec++)
	{
		if (bufsiz < 10)
		{
			strcpy(d, "...");
			break;
		}

		if (spec->first == spec->last)
			fz_snprintf(d, bufsiz, "%d,", spec->first);
		else
			fz_snprintf(d, bufsiz, "%d-%d,", spec->first, spec->last);
		size_t l = strlen(d);
		d += l;
		bufsiz -= l;
	}
	if (d > buf && d[-1] == ',')
		d[-1] = 0;
	return buf;
}

static const char* match_regex2str(const char* re)
{
	if (!re || !*re)
		return "-";
	return re;
}


int
bulktest_main(int argc, const char **argv)
{
	fz_trace_snapshot = &trace_snapshot;
	fz_trace_report_pending_allocations_since = &trace_report_pending_allocations_since;

	ASSERT(1 != 0);

	int c;
	int errored = 0;
	int script_is_template = 0;
	int rv = 0;
	struct curltime begin_time;
	const char* line_command = NULL;
	fz_alloc_context trace_alloc_ctx = { &trace_info, trace_malloc, trace_realloc, trace_free };
	fz_alloc_context *alloc_ctx = NULL;
	fz_locks_context *locks = NULL;
	size_t max_store = FZ_STORE_DEFAULT;
	int lowmemory = 0;
	const char *match_regex = NULL;
	const char* ignore_match_regex = NULL;
	const char* match_numbers_s = NULL;
	const char* ignore_match_numbers_s = NULL;

	// reset global vars: this tool MAY be re-invoked via bulktest or others and should RESET completely between runs:
	bulktest_is_toplevel_ctx = 0;

	showtime = 0;
	showmemory = 0;

	clear_trace_info();
	memclr(&logcfg, sizeof logcfg);
	memclr(&timing, sizeof(timing));
	timing.min = 1 << 30;

	fz_getopt_reset();
	while ((c = fz_getopt(argc, argv, "TLvqVm:s:x:n:X:N:h")) != -1)
	{
		switch(c)
		{
		case 'T': script_is_template = 1; break;
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

		case 'x':
			// expect a regex to match
			match_regex = fz_optarg;
			break;
		case 'X':
			// expect a regex to to *skip* (i.e. ignore)
			ignore_match_regex = fz_optarg;
			break;
		case 'n':
			// expect one or more line numbers and/or ranges to match
			match_numbers_s = fz_optarg;
			break;
		case 'N':
			// expect one or more line numbers and/or ranges to *skip* (i.e. ignore)
			ignore_match_numbers_s = fz_optarg;
			break;

		case 'V': fz_info(ctx, "bulktest version %s", FZ_VERSION); return EXIT_FAILURE;

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
		fz_error(ctx, "mutex initialisation failed");
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

#ifdef _WIN32
		// Get us a debug stream so we have a chance of seeing *independently* of stderr.
		fz_set_stddbg(ctx, fz_stdods(ctx));
#endif

		bulktest_is_toplevel_ctx = 1;
	}
	atexit(mu_drop_context_at_exit);

	ctx = fz_new_context(NULL, NULL, max_store);
	if (!ctx)
	{
		fz_error(ctx, "cannot initialise MuPDF context");
		return EXIT_FAILURE;
	}

	int linecounter = 0;
	int datalinecounter = 0;
	FILE* script = NULL;
	FILE* datafeed = NULL;
	const char* scriptname = NULL;
	const char* datafilename = NULL;
	const struct range* match_test_numbers_ranges = NULL;
	const struct range* ignore_match_test_numbers_ranges = NULL;

	fz_try(ctx)
	{
		char skip_to_label[LONGLINE] = "";
		int skip_to_datalinecounter = 0;

		timing.start_time = Curl_now();

		// page the match & ignore test number ranges, if any were specified:
		// 
		// (NOTE: we don't mind about the '~n' format (n-from-end) supported by the API we use:
		// we accept we'll be getting some odd behaviour as we DO NOT count the number of lines
		// in our input test file(s). This is hacky testcode, after all.)
		//
		match_test_numbers_ranges = decode_numbers_rangespec(match_numbers_s);
		ignore_match_test_numbers_ranges = decode_numbers_rangespec(ignore_match_numbers_s);

		if (match_regex || ignore_match_regex || match_test_numbers_ranges || ignore_match_test_numbers_ranges)
		{
			char numbuf1[LONGLINE];
			char numbuf2[LONGLINE];
			fz_info(ctx, "Using a RESTRICTED DATA SET:\n"
				"- ACCEPT: regex: %s\n"
				"          line numbers: %s\n"
				"- IGNORE: regex: %s\n"
				"          line numbers: %s\n"
				"-----------------------------------------------------------------------------------\n\n\n",
				match_regex2str(match_regex),
				rangespec2str(numbuf1, sizeof(numbuf1), match_test_numbers_ranges),
				match_regex2str(ignore_match_regex),
				rangespec2str(numbuf2, sizeof(numbuf2), ignore_match_test_numbers_ranges));
		}

		// fz_optind is a global that may change by recursive calls to this main. Keep a local copy and use that instead:
		int optind = fz_optind;

		while (optind < argc)
		{
			if (!scriptname || !script_is_template)
			{
				fz_free(ctx, scriptname);
				scriptname = NULL;

				// load a script
				const char *p = argv[optind++];

				close_active_logfile();

				scriptname = mk_absolute_path(ctx, p);

				open_logfile(scriptname);

				script = fopen(scriptname, "rb");
				if (script == NULL)
					fz_throw(ctx, FZ_ERROR_GENERIC, "cannot open script: %s", scriptname);
				linecounter = 0;
				*skip_to_label = 0;
			}

			if (script_is_template)
			{
				if (optind >= argc)
					fz_throw(ctx, FZ_ERROR_GENERIC, "expected at least one datafile to go with the script: %s", scriptname);

				fz_free(ctx, datafilename);
				datafilename = NULL;

				// load a datafile if we already have a script AND we're in "template mode".
				const char *p = argv[optind++];

				datafilename = mk_absolute_path(ctx, p);

				datafeed = fopen(datafilename, "rb");
				if (datafeed == NULL)
					fz_throw(ctx, FZ_ERROR_GENERIC, "cannot open datafile: %s", datafilename);
				datalinecounter = 0;
				skip_to_datalinecounter = 0;
			}

			do
			{
				int if_level = 0;
				int skip_if_block[20];

				// oh, and REWIND that (template) scriptfile again!
				fseek(script, 0, SEEK_SET);
				linecounter = 0;

				const char** template_argv = NULL;
				int template_argc = 0;
				const char** argv = NULL;
				int argc = 0;

				if (script_is_template)
				{
					// process another line = record from the datafeed.
					// skip comment and empty lines in there...
					char* dataline = NULL;
					do
					{
						dataline = my_getline(datafeed, &datalinecounter);
						if (!dataline)
							break;		// EOF
						const char* dataline_for_reporting = dataline;
						size_t pos = strcspn(dataline, " \t\r\n#%");  // comment lines in datafeeds start with # or %
						if (dataline[pos] == 0 || strchr("#%", dataline[pos]))
							dataline = NULL;  // discard
						// also check against any user-specified match/ignore rules:
						int reject = test_dataline_against_matchspecs(dataline, datalinecounter, match_regex, ignore_match_regex, match_test_numbers_ranges, ignore_match_test_numbers_ranges);
						if (reject)
						{
							dataline = NULL;  // discard
							size_t len = strlen(dataline_for_reporting);
							fz_info(ctx, "::SKIPPED: (reason: %s):: #%04d: %.*s%s\n", test_fail_reason[reject], datalinecounter, (len > 45 ? 40 : len), dataline_for_reporting, (len > 45 ? "..." : ""));
						}
					} while (!dataline || skip_to_datalinecounter > datalinecounter);

					// when we've reached the end of the datafeed, it's time to check if there's another datafile waiting for us...
					if (!dataline /* ~ feof(datafeed) */ )
						break;

					// parse datafeed line (record)
					convert_string_to_argv(ctx, &template_argv, &template_argc, dataline, 0);
				}

				do
				{
					bool report_time = true;
					char* line = my_getline(script, &linecounter);

					if (line == NULL)
					{
						if (ferror(script))
						{
							fz_error(ctx, "script read error: %s (%s)", strerror(errno), scriptname);
						}
						break;
					}

					fz_free_argv_array(ctx, argv);
					argv = NULL;
					argc = 0;
					convert_string_to_argv(ctx, &argv, &argc, line, 0);

					// skip empty lines
					if (argc == 0)
						continue;

					expand_template_variables(ctx, argv, linecounter, template_argc, template_argv, datalinecounter, scriptname, datafilename);

					// reformat command line, using the expanded arguments:
					{
						char* d = line;
						for (int i = 0; i < argc; i++)
						{
							const char* a = argv[i];
							if (a[strcspn(a, " \"")] != 0)
							{
								// arg needs escaping/quoting:
								fz_snprintf(d, LONGLINE, "%Q", a);
							}
							else
							{
								strcpy(d, a);
							}
							d += strlen(d);
							*d++ = ' ';
							*d = 0;
						}

						if (argc > 0)
							d[-1] = 0; // kill the trailing space in the reformatted line
					}

					line_command = line;
					rv = 0;

					fflush(logcfg.logfile);

					if (verbosity)
					{
						fz_info(ctx, "L#%04d: %s", linecounter, line);
					}

					// check if this statement is obscured by an outer IF/ENDIF condition
					int skip_this_statement = FALSE;
					for (int i = 0; i < if_level; i++)
					{
						if (skip_if_block[i])
						{
							skip_this_statement = TRUE;
							break;
						}
					}

					begin_time = Curl_now();

					if (match(argv[0], "%"))
					{
						/* Comment */
						report_time = false;
					}
					else if (match(argv[0], "IF"))
					{
						if (argc != 2)
						{
							fz_error(ctx, "script error: IF <ARG>: missing or multiple arguments at line %d: %s", linecounter, line);
							errored++;
						}
						else
						{
							int condition = atoi(argv[1]);
							int maxdepth = sizeof(skip_if_block) / sizeof(skip_if_block[0]);

							if (if_level >= maxdepth)
							{
								fz_error(ctx, "ERR: IF statements nested too deep (more than %d levels) at line %d.", maxdepth, linecounter);
								errored++;
							}
							else
							{
								skip_if_block[if_level++] = !condition;
							}
						}
						report_time = false;
					}
					else if (match(argv[0], "ELSE"))
					{
						if (argc != 1)
						{
							fz_error(ctx, "script error: ELSE never uses arguments. Error at line %d: %s", linecounter, line);
							errored++;
						}
						else if (if_level < 0)
						{
							fz_error(ctx, "ERR: unmatched superfluous ELSE statement at line %d.", linecounter);
							errored++;
						}
						else
						{
							skip_if_block[if_level - 1] = !skip_if_block[if_level - 1];
						}
						report_time = false;
					}
					else if (match(argv[0], "ENDIF"))
					{
						if (argc != 1)
						{
							fz_error(ctx, "script error: ENDIF never uses arguments. Error at line %d: %s", linecounter, line);
							errored++;
						}
						else
						{
							if_level--;
							if (if_level < 0)
							{
								if_level = 0;

								fz_error(ctx, "ERR: unmatched superfluous ENDIF statement at line %d.", linecounter);
								errored++;
							}
						}
						report_time = false;
					}
					else if (match(argv[0], "LABEL:"))
					{
						if (argc != 2)
						{
							fz_error(ctx, "script error: LABEL: <TAG> missing or multiple arguments. Error at line %d: %s", linecounter, line);
							errored++;
						}
						else
						{
							// Just a jump-to point in the script: if we don't have a pending 'skip-to' instruction
							// we hop over this one and continue.
							if (*skip_to_label)
							{
								const char* label = argv[1];
								if (!strcmp(label, skip_to_label))
								{
									*skip_to_label = 0;
									fz_info(ctx, "SKIP TO LABEL found. Going back to work.\n");
								}
							}
						}
						report_time = false;
					}
					else if (*skip_to_label || skip_this_statement)
					{
						// skip command as we're skipping to label X / out of an IF/ELSE/ENDIF conditional block
						fz_info(ctx, "SKIPPING: %s\n", line);
						report_time = false;
					}
					else if (skip_to_datalinecounter > datalinecounter)
					{
						// skip rest of script as we're skipping to *dataline* X
						fz_info(ctx, "SKIPPING TO DATALINE: %d (currently at dataline %d)\n", skip_to_datalinecounter, datalinecounter);
						report_time = false;
						break;
					}
					else if (match(argv[0], "SKIP_TO_LABEL"))
					{
						if (argc != 2)
						{
							fz_error(ctx, "script error: SKIP_TO_LABEL <TAG> missing or too many arguments. Error at line %d: %s", linecounter, line);
							errored++;
						}
						else
						{
							// Specify a label that SHOULD appear further down the script.
							// Skip all commands until we've hit that label.
							strncpy(skip_to_label, argv[1], sizeof(skip_to_label));
							fz_info(ctx, "Skip to label %s\n", skip_to_label);
						}
						report_time = false;
					}
					else if (match(argv[0], "SKIP_UNTIL_DATALINE"))
					{
						if (argc != 2)
						{
							fz_error(ctx, "script error: SKIP_UNTIL_DATALINE <N>: missing or too many arguments. Error at line %d: %s", linecounter, line);
							errored++;
						}
						else
						{
							// Specify a data linenumber that SHOULD be reached before we do anything further in this script.
							skip_to_datalinecounter = atoi(argv[1]);
							// Do we have some skipping to do? If so, mention this command, otherwise plain ignore it.
							if (skip_to_datalinecounter > datalinecounter)
							{
								fz_info(ctx, "Skip to data line %d\n", skip_to_datalinecounter);
							}
						}
						report_time = false;
					}
					else if (match(argv[0], "ECHO"))
					{
						// Use the new, reformatted line for this...
						fz_info(ctx, "::ECHO: %s\n", line + 5);
						report_time = false;
					}
					else if (match(argv[0], "CD"))
					{
						if (argc != 2)
						{
							fz_error(ctx, "script error: CD <PATH>: requires a single argument. Error at line %d: %s", linecounter, line);
							errored++;
						}
						else
						{
							fz_chdir(ctx, argv[1]);
							if (verbosity)
							{
								fz_info(ctx, "OK: CD %s", argv[1]);
							}
						}
					}
					else if (match(argv[0], "MUTOOL"))
					{
						rv = mutool_main(argc - 1, argv + 1);
						if (rv != EXIT_SUCCESS)
						{
							fz_error(ctx, "ERR: error executing MUTOOL command: %s", line);
							errored++;
						}
						else if (verbosity)
						{
							fz_info(ctx, "OK: MUTOOL command: %s", line);
						}

						flush_active_logfile_hard();
					}
					else if (match(argv[0], "BULKTEST"))
					{
						// Ah! Before we go in and recurse on ourselves, we need to do a bit of housekeeping:
						// we must keep the active logfile around (as it surely will be replaced in the recursive call!)
						// so we keep a temporary local copy:
						struct logconfig parent_logcfg = logcfg;
						close_active_logfile();

						// We also need to keep a local copy of some others, so we can restore them as well:
						int parent_showtime = showtime;
						int parent_showmemory = showmemory;
						struct timing parent_timing = timing;
						struct trace_info parent_trace_info = trace_info;
						fz_context* parent_ctx = ctx;

						rv = bulktest_main(argc - 1, argv + 1);

						// recover our own stored-away settings, but keep the heap memory trace sequence number continuous:
						{
							int seqnr = trace_info.sequence_number;
							trace_info = parent_trace_info;
							trace_info.sequence_number = seqnr;
						}
						timing = parent_timing;
						showmemory = parent_showmemory;
						showtime = parent_showtime;
						ctx = parent_ctx;

						// And before we go back to our job, we need to 'recover' the original 'active logfile':
						logcfg = parent_logcfg;
						flush_active_logfile_hard();   // <-- the easiest way to re-open our logfile now: one call is all it takes  :-)

						if (rv != EXIT_SUCCESS)
						{
							fz_error(ctx, "ERR: error executing MUTOOL command at script line %d.", linecounter);
							errored++;
						}
						else if (verbosity)
						{
							fz_info(ctx, "OK: MUTOOL command: %s", line);
						}

						flush_active_logfile_hard();
					}
					else
					{
						report_time = false;
						fz_throw(ctx, FZ_ERROR_GENERIC, "Ignoring line with UNSUPPORTED script statement:\n    %s", line);
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
				} while (!feof(script));

				fz_free_argv_array(ctx, argv);
				argv = NULL;
				argc = 0;

				fz_free_argv_array(ctx, template_argv);
			} while (script_is_template);

			if (datafeed)
			{
				fclose(datafeed);
				datafeed = NULL;
			}
			if (script)
			{
				fclose(script);
				script = NULL;
			}
		}
	}
	fz_catch(ctx)
	{
		if (datafeed)
		{
			fclose(datafeed);
			datafeed = NULL;
		}
		if (script)
		{
			fclose(script);
			script = NULL;
		}

		fz_error(ctx, "Failure when executing '%s': %s", scriptname, fz_caught_message(ctx));

		struct curltime now = Curl_now();

		fz_info(ctx, "L#%05u> T:%03dms D:%0.3lfs FAIL error: exception thrown in script file '%s' at line '%s': %s", linecounter, (int)Curl_timediff(now, begin_time), (double)Curl_timediff(now, timing.start_time) / 1E3, scriptname, (line_command ? line_command : "%--no-line--"), fz_caught_message(ctx));

		// also log to stderr if we haven't already:
		if (logcfg.logfile)
		{
			fprintf(stderr, "\nL#%05u> T:%03dms D:%0.3lfs FAIL error: exception thrown in script file '%s' at line '%s': %s\n", linecounter, (int)Curl_timediff(now, begin_time), (double)Curl_timediff(now, timing.start_time) / 1E3, scriptname, (line_command ? line_command : "%--no-line--"), fz_caught_message(ctx));
		}

		errored += 100;
	}

	fz_free(ctx, scriptname);
	scriptname = NULL;
	fz_free(ctx, datafilename);
	datafilename = NULL;

	fz_flush_warnings(ctx);
	mu_drop_context();

	return errored;
}

int
main(int argc, const char** argv)
{
	return bulktest_main(argc, argv);
}
