
#include "mupdf/fitz.h"
#include "mupdf/helpers/dir.h"
#include "mupdf/assertions.h"
#include "mupdf/helpers/mu-threads.h"
#include "mupdf/helpers/system-header-files.h"
#include "mupdf/helpers/jmemcust.h"

#include "timeval.h"

#include "jbig2.h"

#include "plf_nanotimer_c_api.h"

#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include <signal.h>
#if defined(_WIN32)
#include <windows.h>
#include <intrin.h>
#include <direct.h>
#endif

#define countof(e)   (sizeof(e) / sizeof((e)[0]))

int mutool_main(int argc, const char** argv);

static inline void memclr(void* ptr, size_t size)
{
    memset(ptr, 0, size);
}

static void should_I_wait_for_key(void);
static int running_in_profiler_wait_for_my_key = 0;

static fz_context* ctx = NULL;

static const char* match_regex = NULL;
static const char* ignore_match_regex = NULL;
static const char* match_numbers_s = NULL;
static const char* ignore_match_numbers_s = NULL;
static float random_exec_perunage = 1.0;

struct range
{
    int first;
    int last;
};

static const struct range* match_test_numbers_ranges = NULL;
static const struct range* ignore_match_test_numbers_ranges = NULL;

/*
    In the presence of pthreads or Windows threads, we can offer
    a multi-threaded option. In the absence of such we degrade
    nicely.
*/
#ifndef DISABLE_MUTHREADS

static mu_mutex mutexes[FZ_LOCK_MAX];
static mu_mutex logging_mutex;
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
	mu_destroy_mutex(&logging_mutex);
    mu_destroy_mutex(&heap_debug_mutex);
}

static fz_locks_context *init_mudraw_locks(void)
{
    int i;
    int failed = 0;

    for (i = 0; i < FZ_LOCK_MAX; i++)
        failed |= mu_create_mutex(&mutexes[i]);
    failed |= mu_create_mutex(&logging_mutex);
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
    int mindataline, maxdataline;
    const char* mincommand;
    const char* maxcommand;
};

static time_t start_timestamp;
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

#if defined(FZDBG_HAS_TRACING)
    // heap debugging:
    int origin_line;
    const char* origin_file;
#endif

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
trace_malloc(void *arg, size_t size   FZDBG_DECL_ARGS)
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
#if defined(FZDBG_HAS_TRACING) && defined(_NORMAL_BLOCK)
    p = _malloc_dbg(size + sizeof(trace_header), _NORMAL_BLOCK, trace_srcfile, trace_srcline);
#else
    p = malloc(size + sizeof(trace_header));
#endif
    if (p == NULL)
        return NULL;
    p[0].size = size;
    p[0].magic = 0xEAD;
    p[0].seqnum = info->sequence_number++;
#if defined(FZDBG_HAS_TRACING)
    p[0].origin_file = trace_srcfile;
    p[0].origin_line = trace_srcline;
#endif

#ifndef DISABLE_MUTHREADS
    if (heap_debug_mutex_is_initialized)
        mu_lock_mutex(&heap_debug_mutex);
#endif

    if (info->last)
    {
        ASSERT(info->last->next == NULL);
        info->last->next = p;
    }
    p[0].prev = info->last;
    p[0].next = NULL;
    info->last = p;

#ifndef DISABLE_MUTHREADS
    if (heap_debug_mutex_is_initialized)
        mu_unlock_mutex(&heap_debug_mutex);
#endif

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
#ifndef DISABLE_MUTHREADS
        if (heap_debug_mutex_is_initialized)
            mu_lock_mutex(&heap_debug_mutex);
#endif

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

#ifndef DISABLE_MUTHREADS
        if (heap_debug_mutex_is_initialized)
            mu_unlock_mutex(&heap_debug_mutex);
#endif

        free(&p[-1]);
    }
}

static void *
trace_realloc(void *arg, void *p_, size_t size   FZDBG_DECL_ARGS)
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
        return trace_malloc(arg, size   FZDBG_PASS);

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
#if defined(FZDBG_HAS_TRACING) && defined(_NORMAL_BLOCK)
        p = _realloc_dbg(&p[-1], size + sizeof(trace_header), _NORMAL_BLOCK, trace_srcfile, trace_srcline);
#else
        p = realloc(&p[-1], size + sizeof(trace_header));
#endif
        if (p == NULL)
            return NULL;
        info->current += size - old.size;
        if (size > old.size)
            info->total += size - old.size;
        if (info->current > info->peak)
            info->peak = info->current;
#if defined(FZDBG_HAS_TRACING)
        p[0].origin_file = trace_srcfile;
        p[0].origin_line = trace_srcline;
#endif
        p[0].size = size;
        info->allocs++;

#ifndef DISABLE_MUTHREADS
        if (heap_debug_mutex_is_initialized)
            mu_lock_mutex(&heap_debug_mutex);
#endif

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

#ifndef DISABLE_MUTHREADS
        if (heap_debug_mutex_is_initialized)
            mu_unlock_mutex(&heap_debug_mutex);
#endif

        return &p[1];
    }
}

static size_t
trace_snapshot(void)
{
    return trace_info.sequence_number;
}

static void
trace_report_pending_allocations_since(size_t snapshot_id, int use_fzerror)
{
#ifndef DISABLE_MUTHREADS
    if (heap_debug_mutex_is_initialized)
        mu_lock_mutex(&heap_debug_mutex);
#endif

    trace_header* p = trace_info.last;
    ASSERT(p->next == NULL);
    int hits = 0;
    while (p && p->seqnum >= snapshot_id)
    {
        if (use_fzerror)
        {
#if defined(FZDBG_HAS_TRACING)
            fz_error(ctx, "LEAK? #%zu (size: %zu) (origin: %s(%d))", p->seqnum, p->size, p->origin_file, p->origin_line);
#else
            fz_error(ctx, "LEAK? #%zu (size: %zu)", p->seqnum, p->size);
#endif
        }
        else
        {
#if defined(FZDBG_HAS_TRACING)
            fprintf(stderr, "\nLEAK? #%zu (size: %zu) (origin: %s(%d))", p->seqnum, p->size, p->origin_file, p->origin_line);
#else
            fprintf(stderr, "\nLEAK? #%zu (size: %zu)", p->seqnum, p->size);
#endif
        }
        hits++;

        p = p->prev;
    }
    if (!hits)
    {
        //fprintf(stderr, "\nNo leaks found.\n");
    }
    else
    {
        if (!use_fzerror)
            fprintf(stderr, "\n");
    }

#ifndef DISABLE_MUTHREADS
    if (heap_debug_mutex_is_initialized)
        mu_unlock_mutex(&heap_debug_mutex);
#endif
}


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
		"  -V      display the version of this application and terminate\n"
		"  -T      scriptfile is a script TEMPLATE file, which is filled with data from\n"
        "          the datafiles, one line at a time, and repeated until all datafile records\n"
        "          have been processed that way.\n"
        "  -v      verbose (repeat to dial up the verbosity level)\n"
        "  -q      be quiet (don't print progress messages); the inverse of 'verbose'.\n"
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
		"  The next few filter options apply to the DATAFILE content only:\n"
		"\n"
		"  -n LLL  restrict the test to only the specified tests at lines LLL, where\n"
        "          LLL is any set of comma-separated line specifications:\n"
        "            nnn     : the line number nnn\n"
        "            mmm-nnn : the lines numbers mmm up to and including nnn\n"
        "            'N'     : a literal 'N' represents 'the maximum line number',\n"
        "                      which is assumed to be 1,000,000\n"
        "          Example: '-n 4,10-31,110-N'\n"
        "  -N LLL  the inverse of '-n': /exclude/ the lines in the NNN spec from\n"
        "          the test run.\n"
        "          Note: '-N' is applied after '-n'. Hence, f.e. '-n 5-10 -N 8-10'\n"
        "          will only run the tests for lines 5-7.\n"
        "  -x RRR  restrict the test to only those test lines which match one of\n"
        "          the phrases ('regexes') in the RRR set.\n"
        "          Example: '-x dried-frog-pills' will only execute the tests for\n"
        "          any file path that includes 'dried-frog-pills', e.g.\n"
        "          'research/dried-frog-pills/effectiveness.pdf', and will skip\n"
        "          all non-matching lines, e.g. 'research/frog/disect1.pdf'\n"
		"  -X RRR  the inverse of '-x RRR': ignore any RRR regex-matching tests.\n"
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

struct target_path_mapping
{
    // the absolute path (sans trailing '/') to map any output paths TO:
    // (Note: this path is EMPTY when this mapping feature has been disabled.)
    char abs_target_path[PATH_MAX];
    // the absolute 'current working directory' (at the time of mapping construction), i.e.
    // the part of the SOURCE PATH that will be REPLACED by the abs_target_path:
    // (when the SOURCE PATH is located outside this 'working dir', it is prefixed
    // by the abs_target_path and used in its entirety to ensure discernibility
    // during analysis.
    char abs_cwd_as_mapping_source[PATH_MAX];
};

// map these paths:
// - current working directory --> .
// - executable's path --> EXE
// - test data file path --> DATA-nn
// - test script file path --> SCRIPT
// ...
static struct target_path_mapping output_path_mapping_spec[10];

static const char* find_next_dirsep(const char* p)
{
    if (!*p)
        return p;

    const char* e = strchr(p + 1, '/');
    if (!e)
    {
        e = p + strlen(p);
    }
    return e;
}

static void map_path_to_dest(char* dst, size_t dstsiz, const char* inpath)
{
    char srcpath[PATH_MAX];

#if 0
    // deal with 'specials' too:
    if (!strcmp(inpath, "/dev/null") || !fz_strcasecmp(inpath, "nul:") || !strcmp(inpath, "/dev/stdout"))
    {
        fz_strncpy_s(dst, inpath, dstsiz);
        return;
    }
#endif

    if (!fz_realpath(inpath, srcpath))
    {
        fz_throw(ctx, FZ_ERROR_GENERIC, "cannot process file path to a sane absolute path: %s", inpath);
    }

    // did the user request output file path mapping?
    if (!output_path_mapping_spec[0].abs_target_path[0])
    {
        ASSERT(dstsiz >= PATH_MAX);
        strncpy(dst, srcpath, dstsiz);
    }
    else
    {
        ASSERT(dstsiz >= PATH_MAX);
        ASSERT(dst != NULL);
        *dst = 0;
        size_t dst_len = 0;

        // find the best = shortest mapping:
        for (int idx = 0; idx < countof(output_path_mapping_spec) && output_path_mapping_spec[idx].abs_target_path[0]; idx++)
        {
            // TODO: cope with UNC paths mixed & mashed with classic paths.

            // First we find the common prefix.
            // Then we check how many path parts are left over from the CWD.
            // each left-over part is represented by a single _
            // concat those into the first part to be appended.
            // append the remainder of the source path then, cleaning it up
            // to get rid of drive colons and other 'illegal' chars, replacing them with _.
            // This is your mapped destination path, guaranteed to be positioned
            // WITHIN the given target path (which is prefixed to the generated
            // RELATIVE path!)
            //
            // Example:
            // given
            //   CWD = 	  C:/a/b/c
            //   TARGET = T:/t
            // we then get for these inputs:
            //   C:/a/b/c/d1  -> d1        (leftover: <nil>)     -> d1             -> T:/t/d1
            //   C:/a/b/d     -> d         (leftover: c)         -> _/c            -> T:/t/_/c
            //   C:/a/e/f     -> e/f       (leftover: b/c)       -> __/e/f         -> T:/t/__/e/f
            //   C:/x/y/z     -> x/y/z     (leftover: a/b/c)     -> ___/x/y/z      -> T:/t/___/x/y/z
            //   D:/a/b/c     -> D:/a/b/c  (leftover: C:/a/b/c)  -> ____/D:/a/b/c  -> T:/t/____/D_/a/b/c
            //   C:/a         ->           (leftover: b/c)       -> __             -> T:/t/__
            //   C:/a/b       ->           (leftover: c)         -> _              -> T:/t/_
            //   C:/x         -> x         (leftover: a/b/c)     -> ___/x          -> T:/t/___/x
            //   D:/a         -> D:/a      (leftover: C:/a/b/c)  -> ____/D:/a      -> T:/t/____/D_/a
            // thus every position in the directory tree *anywhere in the system* gets encoded to its own
            // unique subdirectory path within the "target path" directory tree -- of course, ASSUMING
            // you don't have any underscore-only leading directories in any of (relative) paths you feed
            // this mapper... ;-)
            const char* common_prefix = output_path_mapping_spec[idx].abs_cwd_as_mapping_source;
            const char* sep = common_prefix;
            const char* prefix_end = common_prefix;

            do
            {
                sep = find_next_dirsep(sep);
                // match common prefix:
                size_t cpfxlen = sep - common_prefix;
                if (strncmp(common_prefix, srcpath, cpfxlen) != 0)
                {
                    // failed to match; the common prefix is our previous match length.
                    break;
                }
                // check edge case: srcpath has a longer part name at the end of the match,
                // e.g. `b` vs. `bb`: `/a/b[/...]` vs. `/a/b[b/...]`
                if (*sep == 0 && srcpath[cpfxlen] != '/')
                {
                    // failed to match; the common prefix is our previous match length.
                    break;
                }
                prefix_end = sep;
            } while (*sep);

            // count the path parts left over from the CWD:
            char dotdot[PATH_MAX] = "";
            int ddpos = 0;

            // each left-over part is represented by a single _
            while (*sep)
            {
                dotdot[ddpos++] = '_';
                sep = find_next_dirsep(sep);
            }

            if (ddpos > 0)
                dotdot[ddpos++] = '/';
            dotdot[ddpos] = 0;

            // append the remainder of the source path then, cleaning it up
            // to get rid of drive colons and other 'illegal' chars, replacing them with _.
            size_t common_prefix_length = prefix_end - common_prefix;
            const char* remaining_inpath_part = srcpath + common_prefix_length;
            // skip leading '/' separators in remaining_inpath_part as they will only clutter the output
            while (*remaining_inpath_part == '/')
                remaining_inpath_part++;

            char appendedpath[PATH_MAX];
            int rv = fz_snprintf(appendedpath, sizeof(appendedpath), "%s/%s%s", output_path_mapping_spec[idx].abs_target_path, dotdot, remaining_inpath_part);
            if (rv <= 0 || rv >= dstsiz)
            {
                appendedpath[sizeof(appendedpath) - 1] = 0;
                fz_throw(ctx, FZ_ERROR_GENERIC, "cannot map file path to a sane sized absolute path: dstsize: %zu, srcpath: %s, dstpath: %s", dstsiz, srcpath, appendedpath);
            }

            // sanitize the appended part: lingering drive colons, wildcards, etc. will be replaced by _:
            fz_sanitize_path_ex(appendedpath, "^$!", "_", 0, strlen(output_path_mapping_spec[idx].abs_target_path));

            if (dst_len == 0 || dst_len > strlen(appendedpath))
            {
                ASSERT(dstsiz >= PATH_MAX);
                strncpy(dst, appendedpath, dstsiz);
                dst[dstsiz - 1] = 0;
                dst_len = strlen(appendedpath);
            }
        }
    }
}

// Get a line of text from file.
// Return NULL at EOF. Return a reference to a static buffer containing a string value otherwise.
static char *
my_getline(FILE *file, int *linecounter_ref)
{
    int c;
    char *d = getline_buffer;
    int space = sizeof(getline_buffer)-1;

    *d = 0;

    /* Skip over any prefix of whitespace */
    do
    {
        c = fgetc(file);
        if (c == '\n')
            (*linecounter_ref)++;
        // abort on EOF, error or when you encounter an illegal NUL byte in the script
        if (c <= 0)
            return NULL;
    }
    while (isspace(c));

    /* Read the line in */
    do
    {
        *d++ = (char)c;
        --space;
        if (!space)
            break;
        c = fgetc(file);
        if (c == '\n' || c <= 0)
            (*linecounter_ref)++;
    }
    while (c >= 32 || c == '\t');

    /* If we ran out of space, skip the rest of the line */
    if (space == 0)
    {
        fz_error(ctx, "getline: line too long.");
        while (c >= 32 || c == '\t')
        {
            c = fgetc(file);
            if (c == '\n')
                (*linecounter_ref)++;
        }
    }

	/* Skip over trailing CRLF */
	while (c == '\r')
	{
		c = fgetc(file);
		if (c == '\n')
			(*linecounter_ref)++;
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
            else if (strncmp(m, "datetime", 8) == 0 || strncmp(m, "{datetime}", 10) == 0)
            {
                size_t skip_dist = 8 + (m[0] == '{' ? 2 : 0);
                // `%datetime`: print a short version of the date+time when bulktest was started.
                // Useful when you want to produce a clean, fresh output/dump directory tree for
                // every bulktest run.
                struct tm timeinfo;
                timeinfo = *localtime(&start_timestamp);
                strftime(d, space, "%Y%M%d-%H%M%S", &timeinfo);
                if (strlen(d) >= space)
                    fz_throw(ctx, FZ_ERROR_GENERIC, "out of template expansion buffer space.");
                s = m + skip_dist;
                size_t l = strlen(d);
                d += l;
                space -= l;
            }
            else if (strncmp(m, "datarow", 7) == 0 || strncmp(m, "{datarow}", 9) == 0)
            {
                size_t skip_dist = 7 + (m[0] == '{' ? 2 : 0);
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
                size_t skip_dist = 11 + (m[0] == '{' ? 2 : 0);
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
                size_t skip_dist = 9 + (m[0] == '{' ? 2 : 0);
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
            else if (strncmp(m, "remap", 5) == 0 || strncmp(m, "{remap", 6) == 0)
            {
                // format: %{remap(arg)}, where (){}<>[] pairs are accepted as arg delims.
                //
                // remap the given path to the target path, IFF the bulktest user specified
                // a path remapping should be applied.
                size_t skip_dist = 5 + (m[0] == '{' ? 1 : 0);

                s = m + skip_dist;
                char delim = *s++;
                static const char start_delims[] = "({<[";
                static const char end_delims[] =   ")}>]";
                const char* delim_pos = strchr(start_delims, delim);
                if (!delim_pos)
                    fz_throw(ctx, FZ_ERROR_GENERIC, "missing start delimiter: %remap template function expects at brace-delimited argument. Faulty line snippet: %q (as part of %q)", m, arg);
                size_t pos = delim_pos - start_delims;
                char end_delim = end_delims[pos];
                delim_pos = strchr(s, end_delim);
                if (!delim_pos)
                    fz_throw(ctx, FZ_ERROR_GENERIC, "missing end delimiter: %remap template function expects at brace-delimited argument. Faulty line snippet: %q (as part of %q)", s, arg);
                char remap_arg[PATH_MAX];
                // amount to copy: (delim_pos - s) chars, +1 for the enforced NUL sentinel:
                fz_strncpy_s(ctx, remap_arg, s, fz_mini((delim_pos - s) + 1, sizeof(remap_arg)));
                // %remap itself should also be terminated when it was written as "%{remap":
                if (m[0] == '{')
                {
                    if (delim_pos[1] != '}')
                        fz_throw(ctx, FZ_ERROR_GENERIC, "%{remap} command not properly delineated: missing closing curly brace '}'. Faulty line snippet: %q (as part of %q)", s, arg);
                    s = delim_pos + 2;
                }
                else
                {
                    s = delim_pos + 1;
                }

                char target_path[PATH_MAX];
                map_path_to_dest(target_path, sizeof(target_path), remap_arg);

                strncpy(d, target_path, space);

                size_t l = strlen(d);
                d += l;
                space -= l;
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

static const char* rangespec2str(char* buf, size_t bufsiz, const struct range* spec)
{
    if (!spec || spec->first <= 0)
        return "-";

    buf[0] = 0;
    char* d = buf;
    for (; spec->first > 0; spec++)
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
static void convert_string_to_argv(fz_context* ctx, const char*** argv, int* argc, char* line, int start_index, int tab_separated_args)
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
			if (!tab_separated_args)
			{
				while (*e && !isspace(*(unsigned char*)e))
					e++;
			}
			else
			{
				while (*e && *e != '\t' && *e != '\r' && *e != '\n')
					e++;
			}
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
            logcfg.logfile = fz_fopen_utf8(ctx, logcfg.logfilepath, "a");
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
                fz_snprintf(logfilename, sizeof(logfilename), "%s.C-%04d.log", basename, count++);
            }
            ASSERT(strcmp(logcfg.logfilepath, logfilename));

                (void)rename(logcfg.logfilepath, logfilename);
                int errcode = errno;
                if (fz_file_exists(ctx, logcfg.logfilepath))
                {
                    fz_throw(ctx, FZ_ERROR_GENERIC, "%s: failed to rename old logfile %q to %q.", errcode ? strerror(errcode) : "Unknown rename error", logfilename, logcfg.logfilepath);
                }

            logcfg.logfile = fz_fopen_utf8(ctx, logcfg.logfilepath, "w");

            fz_info(ctx, "bulktest: logfile rotated. Previous logging at %q.\n", logfilename);

            // report test run restrictions to every logfile we produce:
            if (random_exec_perunage < 1.0)
            {
                fz_info(ctx, "bulktest: using random_exec_percentage: %.1f%%\n", random_exec_perunage * 100);
            }

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
        }
    }
}

static void open_logfile(const char* scriptname)
{
    char logfilename[PATH_MAX];
    char logfilename_0[PATH_MAX];

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

    fz_mkdir_for_file(ctx, logfilename_0);

    logcfg.logfile = fz_fopen_utf8(ctx, logfilename_0, "w");
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
                    timing.total, timing.count, timing.total / timing.count, timing.count);
                fz_info(ctx, "fastest command line %d (dataline: %d): %lldms (%s)", timing.minscriptline, timing.mindataline, timing.min, timing.mincommand);
                fz_info(ctx, "slowest command line %d (dataline: %d): %lldms (%s)", timing.maxscriptline, timing.maxdataline, timing.max, timing.maxcommand);

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

    // WARNING: as we point `ctx` at the GLOBAL context in the app init phase, it MAY already be an INVALID
    // pointer reference by now!
    //
    // WARNING: this assert fires when you run `mutool raster` (and probably other tools as well) and hit Ctrl+C
    // to ABORT/INTERRUPT the running application: the MSVC RTL calls this function in the atexit() handler
    // and the assert fires due to (ctx->error.top != ctx->error.stack).
    //
    // We are okay with that, as that scenario is an immediate abort anyway and the OS will be responsible
    // for cleaning up. That our fz_try/throw/catch exception stack hasn't been properly rewound at such times
    // is obvious, I suppose...
    ASSERT_AND_CONTINUE(!ctx || !fz_has_global_context() || (ctx->error.top == ctx->error.stack_base));

    if (ctx != __fz_get_RAW_global_context())
    {
        fz_drop_context(ctx); // also done here for those rare exit() calls inside the library code.
        ctx = NULL;
    }

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
            fz_drop_context_locks(ctx);
        }

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
    //
    // clean up as best we can:
    mu_drop_context();
}

typedef enum progress_msg_level {
    PML_INFO,
    PML_WARNING,
    PML_ERROR,
} progress_msg_level_t;

static void show_progress_on_stderr(struct logconfig* logcfg, progress_msg_level_t level, const char* message)
{
    if (!logcfg->quiet)
    {
        FILE* logfile = (logcfg && logcfg->logfile) ? logcfg->logfile : stderr;

        // show progress on stderr, while we log the real data to logfile:
        if (logfile != stderr)
        {
            if (!strncmp(message, ":L#0", 4))
            {
                return; // don't show progress for simple 'this line was just read' log messages.
            }
            else if (!strncmp(message, "OK:", 3))
            {
                //fprintf(stderr, u8"✅");
                fprintf(stderr, "#");
            }
            else if (!strncmp(message, "ERR:", 4))
            {
                //fprintf(stderr, u8"❎");
				fprintf(stderr, "[E]");
				fprintf(stderr, "\n%s\n", message);
            }
            else if (!strncmp(message, "::ECHO: ", 8))
            {
                fprintf(stderr, "\n%s", message + 8);
            }
            else if (!strncmp(message, "::SKIPPED: ", 11))
            {
                fprintf(stderr, "\n%s", message + 2);
            }
            else if (!strncmp(message, "...also skipped ", 16))
            {
                fprintf(stderr, "s");
            }
            else if (!strncmp(message, "::SKIP-DUE-TO-RANDOM-SAMPLING: ", 31))
            {
                fprintf(stderr, "^");
            }
            else if (strstr(message, "*!*"))
            {
                fprintf(stderr, "*!*");
            }
            else
            {
				static int state = 0;
				switch (level)
                {
                case PML_ERROR:
					fprintf(stderr, "?");
					if (!strncmp(message, "LEAK? #", 7))
					{
						state++;
					}
					else
					{
						state = 0;
					}
					// only list a limited number of reported leaks per file/run:
					// we 'fake' that heuristic by shutting up the leak reporter until
					// we've observed some *other* error in the meantime:
					if (state < 3)
						fprintf(stderr, "\n%s\n", message);
					break;

                case PML_WARNING:
                    fprintf(stderr, "w");
                    break;

                default:
                    {
                        // time-based reduction of progress output:
                        // the first filter is RDTSC based and *fast*, reducing the progress update rate to something around 1/10th of a second,
                        // give or take some CPU clock rate fluctuations, etc.
                        // the inner, second, filter is clock()-based and thus quite a bit slower, yet more accurate. Here we filter the
                        // incoming rate of ~ msgs per second down to once every third of a second, 10 per sec to 3 per sec.
                        static uint64_t prev_rtcnt = 0;
                        static uint64_t offset = 1E6;
                        uint64_t t = __rdtsc();
                        if (t > prev_rtcnt + offset)
                        {
                            // RDTSC and clock() must be collected together; for offset correction, we cannot use `prev_rtcnt` as that one will
                            // have updated in the meantime, where this code chunk was then skipped, resulting in (t - prev_rtcnt) measuring
                            // quite a different time interval then we do here using clock(), hence we need to track the matching RDTSC
                            // value for proper offset correction calculus:
                            static uint64_t prev_rtcnt_on_clock = 0;

                            static nanotimer_data_t prev_t = { 0 };
                            nanotimer_data_t new_t = { 0 };
                            nanotimer_start(&new_t);
                            double t_delta = nanotimer_get_elapsed_us(&prev_t);

                            // Stage 1:
                            //
                            // adjust ("tune") your RDTSC count offset to a poll period of about 0.1 seconds, i.e. to visit
                            // this section about once every 100 msecs.
                            if (t_delta > 0 && prev_rtcnt != 0)
                            {
                                // apparently, `offset` is small enough to land us here again *within* 1/10rd of a second: adjust `offset`!
                                uint64_t clockdelta = (t - prev_rtcnt_on_clock);
                                // slowly grow the offset towards the proper value; when we measure across tiny clock() intervals,
                                // extrapolating to 0.1 second (~ 100 clock()s on most hardware) is overzealous and highly
                                // inaccurate. So we limit the extrapolation factor to 2, thus growing the offset slowly towards
                                // a proper value. We don't loose much performance with this either, i.e. this is very low extra overhead.
                                //
                                // Another, additional, measure to improve accuracy of the calculus is to ignore any delta frames
                                // which last least than 20 clock() ticks: that way, we'll have a 5% or better accuracy.
                                if (clockdelta >= 20)
                                {
                                    double dt = clockdelta / t_delta;   // --> RDTSC count per microsecond
                                    dt *= 0.1 * 1E6;                    // --> RDTSC ticks count for 0.1 second (100 0000 microseconds)
                                    if (dt > offset)
                                    {
                                        // also limit the offset increase angle to prevent crazy changes due to unknown CPU gas bubbles. ;-)
                                        //
                                        // Note: I was wandering for a bit why those 'crazy jumps' occurred, but they happen, of course,
                                        // when you're debugging the codebase: while you, the human, check your debugger screens, the timers
                                        // keep ticking and thus your next time delta will be way off from usual reality, resulting in
                                        // sometimes *insane* `dt` value jumps in here.
                                        // By limiting the increase rate, we prevent that artifact from badly impacting our progress time delta
                                        // filters above.
                                        offset = fz_min(5 * offset, dt);
                                        //fprintf(stderr, "+");
                                    }

                                    prev_t = new_t;
                                    prev_rtcnt_on_clock = t;
                                }
                            }
                            else
                            {
                                prev_t = new_t;
                                prev_rtcnt_on_clock = t;
                            }

                            // Stage 2:
                            //
                            // As we're now in this "once per 100msec" section, we know we can do relatively costly
                            // things, such as measuring the time (as done above) and print porogress.
                            // We do the later at most 3 times per second:
                            static double pdelta = 0;
                            static uint32_t tocks = 0;
                            pdelta += t_delta;
                            tocks++;

                            if (tocks >= 3 || pdelta > 0.33 * 1E6)
                            {
                                //fprintf(stderr, ".%d%d", tocks, pdelta > 0.33 * 1E6);
                                fprintf(stderr, ".");
                                tocks = 0;
                                pdelta = 0;
                            }
                        }
                        else
                        {
                            // slowly decrease the timing offset: this compensates for CPU clock variations, when
                            // the CPU clock rate slows down: then the previously determined `offset` value won't be suitable any more.
                            //
                            // As this path will execute quite often, we don't want any particular (and costly!) time measurement calls
                            // in here, but simply apply some quick math to slowly decrease the offset.
                            offset = offset * 0.8f;
                            //fprintf(stderr, "-");
                        }
                        prev_rtcnt = t;
                    }
                    break;
                }
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
    int rc = fz_set_output_buffer(ctx, dbg, 160 /* LONGLINE is too much here, just try to buffer about 2 regular lines of text... */);
    return dbg;
}


static void tst_error_callback(fz_context* ctx, void* user, const char* message)
{
    struct logconfig* logcfg = (struct logconfig*)user;
    FILE* logfile = (logcfg && logcfg->logfile) ? logcfg->logfile : stderr;

    // show progress on stderr, while we log the real data to logfile:
    show_progress_on_stderr(logcfg, PML_ERROR, message);


#ifndef DISABLE_MUTHREADS
	mu_lock_mutex(&logging_mutex);
#endif

	fprintf(logfile, "error: %s\n", message);
    fflush(logfile);

#ifndef DISABLE_MUTHREADS
	mu_unlock_mutex(&logging_mutex);
#endif

    fz_output* dbg = stddbgchannel();
	if (dbg)
	{
		fz_write_strings(ctx, dbg, "error: ", message, "\n", NULL);
	}
}

static void tst_warning_callback(fz_context* ctx, void* user, const char* message)
{
    struct logconfig* logcfg = (struct logconfig*)user;
    FILE* logfile = (logcfg && logcfg->logfile) ? logcfg->logfile : stderr;

    if (!logcfg->quiet)
    {
        // show progress on stderr, while we log the real data to logfile:
        show_progress_on_stderr(logcfg, PML_WARNING, message);

#ifndef DISABLE_MUTHREADS
		mu_lock_mutex(&logging_mutex);
#endif

		fprintf(logfile, "warning: %s\n", message);
        fflush(logfile);

#ifndef DISABLE_MUTHREADS
		mu_unlock_mutex(&logging_mutex);
#endif

        fz_output* dbg = stddbgchannel();
		if (dbg)
		{
			fz_write_strings(ctx, dbg, "warning: ", message, "\n", NULL);
		}
	}
}

static void tst_info_callback(fz_context* ctx, void* user, const char* message)
{
    struct logconfig* logcfg = (struct logconfig*)user;
    FILE* logfile = (logcfg && logcfg->logfile) ? logcfg->logfile : stderr;

    if (!logcfg->quiet)
    {
        // show progress on stderr, while we log the real data to logfile:
        show_progress_on_stderr(logcfg, PML_INFO, message);

#ifndef DISABLE_MUTHREADS
		mu_lock_mutex(&logging_mutex);
#endif

		fprintf(logfile, "%s\n", message);

#ifndef DISABLE_MUTHREADS
		mu_unlock_mutex(&logging_mutex);
#endif

        fz_output* dbg = stddbgchannel();
		if (dbg)
		{
			fz_write_strings(ctx, dbg, message, "\n", NULL);
		}
	}
}

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
            ASSERT(spage >= 1);
            ASSERT(epage >= 1);
            rv[i].first = spage;
            rv[i].last = epage;
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
    /* 1 */ "empty line or comment",
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
    if (!rejected && match_ranges_spec && match_ranges_spec[0].first > 0)
    {
        int hit = 4;
        for (; match_ranges_spec[0].first > 0; match_ranges_spec++)
        {
            if (match_ranges_spec[0].first <= linenumber && match_ranges_spec[0].last >= linenumber)
            {
                hit = 0;
                break;
            }
        }
        rejected = hit;
    }
    if (!rejected && ignore_match_ranges_spec && ignore_match_ranges_spec[0].first > 0)
    {
        int hit = 0;
        for (; ignore_match_ranges_spec[0].first > 0; ignore_match_ranges_spec++)
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


int
bulktest_main(int argc, const char **argv)
{
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
    const char* forced_output_basedir = NULL;

    match_regex = NULL;
    ignore_match_regex = NULL;
    match_numbers_s = NULL;
    ignore_match_numbers_s = NULL;
    random_exec_perunage = 1.0;

    // reset global vars: this tool MAY be re-invoked via bulktest or others and should RESET completely between runs:
    //bulktest_is_toplevel_ctx = 0;

    time(&start_timestamp);

    showtime = 0;
    showmemory = 0;

    running_in_profiler_wait_for_my_key = 0;

    clear_trace_info();
    memclr(&logcfg, sizeof logcfg);
    memclr(&timing, sizeof(timing));
    timing.min = 1 << 30;

    fz_getopt_reset();
    while ((c = fz_getopt(argc, argv, "BTLvqVm:r:s:x:n:X:N:O:h")) != -1)
    {
        switch(c)
        {
        case 'B': running_in_profiler_wait_for_my_key = 2; break;
        case 'T': script_is_template = 1; break;
        case 'q': logcfg.quiet = 1; verbosity = 0; break;
        case 'v': logcfg.quiet = 0; verbosity++; break;
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

        case 'r':
            // expect a percentage / perunage
            char pct = 0;
            int pc = sscanf(fz_optarg, "%f%c", &random_exec_perunage, &pct);
            if (pct == '%')
                random_exec_perunage /= 100;
            // sanity check/limiter @ 1%
            if (random_exec_perunage < 0.0)
                random_exec_perunage = 0.0;
            else if (random_exec_perunage > 1.0)
                random_exec_perunage = 1.0;
            break;

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
        case 'O':
            // output files must use this basedir: map current working directory onto it; absolute paths get mapped as if they were relative.
            forced_output_basedir = fz_optarg;
            break;

        case 'V': fz_info(ctx, "bulktest version %s", FZ_VERSION); return EXIT_FAILURE;

        default: usage(); return EXIT_FAILURE;
        }
    }

    should_I_wait_for_key();

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
        fz_enable_dbg_output();
        fz_enable_dbg_output_on_stdio_unreachable();

        ASSERT(ctx == NULL);

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

        bulktest_is_toplevel_ctx = 1;
    }

	// register a mupdf-aligned default heap memory manager for jpeg/jpeg-turbo
	fz_set_default_jpeg_sys_mem_mgr();

	atexit(mu_drop_context_at_exit);

    if (ctx != __fz_get_RAW_global_context())
    {
        fz_drop_context(ctx); // drop our previous context IFF this happens to be a re-run in monolithic mode.
        ctx = NULL;
    }

    ctx = fz_new_context(NULL, NULL, max_store);
    if (!ctx)
    {
        fz_error(ctx, "cannot initialise MuPDF context");
        return EXIT_FAILURE;
    }

    // shut up JBig2Dec (which can be very verbose) unless we've dialed up our own verbosity levels:
    jbig2_set_error_log_prefilter_level(JBIG2_SEVERITY_FATAL - verbosity);

    {
        if (!getcwd(output_path_mapping_spec[0].abs_cwd_as_mapping_source, sizeof(output_path_mapping_spec[0].abs_cwd_as_mapping_source)))
        {
            fz_error(ctx, "cannot initialise bulktest::cwd: %s; path(s) too long?", strerror(errno));
            return EXIT_FAILURE;
        }
        // when no output mapping/override was specified, don't use any:
        if (!forced_output_basedir)
        {
            output_path_mapping_spec[0].abs_target_path[0] = 0;
        }
        // same 'realpath' treatment for both sides of the mapping:
        else if (!fz_realpath(forced_output_basedir, output_path_mapping_spec[0].abs_target_path) ||
            !fz_realpath(output_path_mapping_spec[0].abs_cwd_as_mapping_source, output_path_mapping_spec[0].abs_cwd_as_mapping_source))
        {
            fz_error(ctx, "cannot initialise bulktest::output_path_mapping spec; path(s) too long?");
            return EXIT_FAILURE;
        }

        for (int i = 1; i < countof(output_path_mapping_spec); i++)
        {
            memcpy(&output_path_mapping_spec[i], &output_path_mapping_spec[0], sizeof(output_path_mapping_spec[0]));
        }

        int optind = fz_optind + 1;
        int datafile_count = argc - optind;
        int idx;

        for (idx = 1; optind < argc && idx < countof(output_path_mapping_spec) - 2; idx++)
        {
            const char* p = argv[optind++];

            if (!fz_realpath(p, output_path_mapping_spec[idx].abs_cwd_as_mapping_source))
            {
                fz_error(ctx, "cannot initialise bulktest::output_path_mapping spec; path(s) too long?");
                return EXIT_FAILURE;
            }
            // nuke the filename + trailing '/' to make this a [source] path directory:
            char* dst = (char*)fz_basename(output_path_mapping_spec[idx].abs_cwd_as_mapping_source);
            ASSERT(dst[-1] == '/' || dst[-1] == '\\');
            dst[-1] = 0;

            // now pad the destination path:
            dst = output_path_mapping_spec[idx].abs_target_path;
            size_t l = strlen(dst);
            dst += l;
            if (datafile_count > 1)
                fz_snprintf(dst, sizeof(output_path_mapping_spec[idx].abs_target_path) - l, "/DATA-%02d", idx);
            else
                fz_strlcpy(dst, "/DATA", sizeof(output_path_mapping_spec[idx].abs_target_path) - l);
        }

        optind = fz_optind;

        ASSERT(idx < countof(output_path_mapping_spec));
        if (optind < argc)
        {
            const char* p = argv[optind];

            if (!fz_realpath(p, output_path_mapping_spec[idx].abs_cwd_as_mapping_source))
            {
                fz_error(ctx, "cannot initialise bulktest::output_path_mapping spec; path(s) too long?");
                return EXIT_FAILURE;
            }
            // nuke the filename + trailing '/' to make this a [source] path directory:
            char* dst = (char*)fz_basename(output_path_mapping_spec[idx].abs_cwd_as_mapping_source);
            ASSERT(dst[-1] == '/' || dst[-1] == '\\');
            dst[-1] = 0;

            // now pad the destination path:
            //
            // NOTE: pad ("__SCRIPT__" must be equal or longer than the longest "DATA-%02d" pad for the mapping code to successfully
            // pick the desired shortest mapping path! Hence the extra underscores surrounding SCRIPT here.
            fz_strlcat(output_path_mapping_spec[idx].abs_target_path, "/__SCRIPT__", sizeof(output_path_mapping_spec[idx].abs_target_path));
        }

        idx++;

        ASSERT(idx < countof(output_path_mapping_spec));
        {
            const char* p = argv[0];

            if (!fz_realpath(p, output_path_mapping_spec[idx].abs_cwd_as_mapping_source))
            {
                fz_error(ctx, "cannot initialise bulktest::output_path_mapping spec; path(s) too long?");
                return EXIT_FAILURE;
            }
            // nuke the filename + trailing '/' to make this a [source] path directory:
            char* dst = (char*)fz_basename(output_path_mapping_spec[idx].abs_cwd_as_mapping_source);
            ASSERT(dst[-1] == '/' || dst[-1] == '\\');
            dst[-1] = 0;

            // now pad the destination path:
            //
            // NOTE: pad ("EXECUTABLE" must be equal or longer than the longest "DATA-%02d" or "__SCRIPT__" pad for the mapping code to successfully
            // pick the desired shortest mapping path! Hence the "EXECUTABLE" name instead of simply "EXE" or "__EXE__" here.
            fz_strlcat(output_path_mapping_spec[idx].abs_target_path, "/EXECUTABLE", sizeof(output_path_mapping_spec[idx].abs_target_path));
        }

        // clean the remainder of the slots:
        for (idx++; idx < countof(output_path_mapping_spec); idx++)
        {
            memset(&output_path_mapping_spec[idx], 0, sizeof(output_path_mapping_spec[idx]));
        }
    }

    int linecounter = 0;
    int datalinecounter = 0;
    FILE* script = NULL;
    FILE* datafeed = NULL;
    char scriptname[PATH_MAX] = "";
    const char* datafilename = NULL;

    match_test_numbers_ranges = NULL;
    ignore_match_test_numbers_ranges = NULL;

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
            if (!scriptname[0] || !script_is_template)
            {
                scriptname[0] = 0;

                // load a script
                const char* p = argv[optind++];

                close_active_logfile();

                map_path_to_dest(scriptname, sizeof(scriptname), p);
                // extra: we want all bulktest logfiles to be dumped straight into the remap target path, if remapping was turned on.
                // in that case, we want the entire remapped path to be visible in the logfile name, so we can easily match
                // logfiles to target path subtrees (and the generated output files there-in).
                //
                // As we are guaranteed to now have a path pointing INSIDE the target base path, we can simply use length of
                // base path as the starting point for this next transformation:
                if (output_path_mapping_spec[0].abs_target_path[0])
                {
                    size_t offset = strlen(output_path_mapping_spec[0].abs_target_path);

                    // replace all '/' with '.' to produce a filename representing the mapped path yet will be dumped in the base dir itself for quick & easy access:
                    char* p = scriptname + offset;
                    ASSERT(*p == '/');
                    p = strchr(p + 1, '/');
                    while (p)
                    {
                        *p = '.';
                        p = strchr(p + 1, '/');
                    }
                }

                open_logfile(scriptname);

                if (random_exec_perunage < 1.0)
                {
                    fz_info(ctx, "bulktest: using random_exec_percentage: %.1f%%\n", random_exec_perunage * 100);
                }

                // report test run restrictions to every logfile we produce:
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

                if (!fz_realpath(p, scriptname))
                {
                    fz_throw(ctx, FZ_ERROR_GENERIC, "cannot process script file path to a sane absolute path: %s", p);
                }

                script = fz_fopen_utf8(ctx, scriptname, "rb");
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
                const char* p = argv[optind++];
                char abspathbuf[PATH_MAX];

                if (!fz_realpath(p, abspathbuf))
                {
                    fz_throw(ctx, FZ_ERROR_GENERIC, "cannot process bulktest data file path to a sane absolute path: %s", p);
                }
                datafilename = fz_strdup(ctx, abspathbuf);

                datafeed = fz_fopen_utf8(ctx, datafilename, "rb");
                if (datafeed == NULL)
                    fz_throw(ctx, FZ_ERROR_GENERIC, "cannot open datafile %q: %s", datafilename, fz_ctx_pop_system_errormsg(ctx));
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
                    int last_skipped = 0;
                    int last_reject = 0;

                    do
                    {
                        dataline = my_getline(datafeed, &datalinecounter);
                        if (!dataline)
                            break;		// EOF
                        const char* dataline_for_reporting = dataline;
                        size_t pos = strspn(dataline, " \t\r\n");
                        // comment lines in datafeeds start with # or %
                        if (dataline[pos] == 0 || strchr("#%", dataline[pos]))
                        {
                            // % CANNOT introduce a macro in a DATA LINE, so we're safe to just nuke any of these lines:
                            dataline = NULL;  // discard
                        }
                        // also check against any user-specified match/ignore rules:
                        int reject = test_dataline_against_matchspecs(dataline, datalinecounter, match_regex, ignore_match_regex, match_test_numbers_ranges, ignore_match_test_numbers_ranges);
                        if (reject)
                        {
                            dataline = NULL;  // discard
                            size_t len = strlen(dataline_for_reporting);
                            char last_skipped_msg[LONGLINE];
                            fz_snprintf(last_skipped_msg, sizeof(last_skipped_msg), "(reason: %s):: #%04d: %.*s%s", test_fail_reason[reject], datalinecounter, (len > 45 ? 40 : len), dataline_for_reporting, (len > 45 ? "..." : ""));
                            if (!last_skipped || last_reject != reject)
                            {
                                fz_info(ctx, "::SKIPPED: %s\n", last_skipped_msg);
                                last_skipped = 1;
                                last_reject = reject;
                            }
                            else
                            {
                                fz_info(ctx, "...also skipped %s\n", last_skipped_msg);
                                last_skipped++;
                            }
                        }
                        else
                        {
                            last_skipped = 0;
                        }
                    } while (!dataline || skip_to_datalinecounter > datalinecounter);

                    // when we've reached the end of the datafeed, it's time to check if there's another datafile waiting for us...
                    if (!dataline /* ~ feof(datafeed) */)
                        break;

                    // parse datafeed line (record)
                    convert_string_to_argv(ctx, &template_argv, &template_argc, dataline, 0, 1);

					//# nr.:      %datarow     -- index number of the test record
					//# PDF:      % 1          -- full RELATIVE path to the PDF
					//# dir :     % 2          -- basedir = path part of that
					//# name :    % 3          -- filename = PDF filename part of that one (with.pdf extension)
					//# base :    % 4          -- basename = PDF filename "    "  "    "   (without the.pdf extension)
					//# cd root : % 5          -- path to root of the repo
					if (template_argc == 1)
					{
						// apparently we're loading a LST file instead of a full-fledged TSV.
						//
						// construct the other parameters from the first:
						char* p = (char *)template_argv[0];
						fz_normalize_path(ctx, p, strlen(p) + 1, p);
						p = fz_strdup(ctx, p);
						char* q1 = strrchr(p, '/');
						if (q1)
						{
							*q1 = 0;
						}
						else
						{
							q1 = p;
							*q1 = 0;
						}
						char* f = fz_strdup(ctx, fz_basename(template_argv[0]));
						char* f_no_e = fz_strdup(ctx, fz_basename(template_argv[0]));
						char* e = strrchr(f_no_e, '.');
						if (e)
						{
							*e = 0;
						}
						template_argv[1] = p;
						template_argv[2] = f;
						template_argv[3] = f_no_e;
						template_argv[4] = fz_strdup(ctx, "./");
						template_argv[5] = NULL;
						template_argc = 5;
					}
                }

                if (verbosity >= 1)
                {
                    fz_info(ctx, "\n");
                }

                do
                {
                    enum {
                        RPT_NOTHING = 0,
                        RPT_A_SKIPPED_COMMAND,
                        RPT_A_SIMPLE_CONTROLFLOW_COMMAND,
                        RPT_A_SIMPLE_USERINFO_COMMAND,
                        RPT_A_SIMPLE_COMMAND,
                        RPT_AN_IMPORTANT_TEST_COMMAND
                    } report_time = RPT_NOTHING;
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

                    // ignore comments.
                    //
                    // comments start with '% ' (note the extra ' ' SPACE char in there!), '# ' or '// '
                    for (char* comment_start = strpbrk(line, "%#/"); comment_start; comment_start = strpbrk(comment_start + 1, "%#/"))
                    {
                        switch (comment_start[0])
                        {
                        case '%':
                        case '#':
                            // accept on line by itself, i.e. followed by NUL, otherwise we require a whitespace to follow it, for otherwise it could be a script macro or other important bit!
                            if (!comment_start[1] || isspace(comment_start[1]))
                            {
                                if (verbosity > 1)
                                {
                                    fz_info(ctx, ":L#%05u: COMMENT %s", linecounter, line);
                                }

                                comment_start[0] = 0;
                                comment_start[1] = 0;
                            }
                            break;

                        case '/':
                            // no additional whitespace after required...
                            if (comment_start[1] == '/')
                            {
                                if (verbosity > 1)
                                {
                                    fz_info(ctx, ":L#%05u: COMMENT %s", linecounter, line);
                                }

                                comment_start[0] = 0;
                                comment_start[1] = 0;
                            }
                            break;
                        }
                    }

                    convert_string_to_argv(ctx, &argv, &argc, line, 0, 0);

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

                    if (verbosity >= 1)
                    {
                        fz_info(ctx, ":L#%05u: %s", linecounter, line);
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

                    size_t memory_consumed = 0;
                    size_t memory_allocations = 0;
                    size_t memory_leaked = 0;
                    size_t memory_snapshot_id = trace_snapshot();

                    begin_time = Curl_now();

                    if (match(argv[0], "IF"))
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
                        report_time = RPT_A_SIMPLE_CONTROLFLOW_COMMAND;
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
                        report_time = RPT_A_SIMPLE_CONTROLFLOW_COMMAND;
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
                        report_time = RPT_A_SIMPLE_CONTROLFLOW_COMMAND;
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
                        report_time = RPT_A_SIMPLE_CONTROLFLOW_COMMAND;
                    }
                    else if (*skip_to_label || skip_this_statement)
                    {
                        // skip command as we're skipping to label X / out of an IF/ELSE/ENDIF conditional block
                        fz_info(ctx, "SKIPPING: %s\n", line);
                        report_time = RPT_A_SKIPPED_COMMAND;
                    }
                    else if (skip_to_datalinecounter > datalinecounter)
                    {
                        // skip rest of script as we're skipping to *dataline* X
                        fz_info(ctx, "SKIPPING TO DATALINE: %d (currently at dataline %d)\n", skip_to_datalinecounter, datalinecounter);
                        report_time = RPT_A_SKIPPED_COMMAND;
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
                        report_time = RPT_A_SIMPLE_CONTROLFLOW_COMMAND;
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
                        report_time = RPT_A_SIMPLE_CONTROLFLOW_COMMAND;
                    }
                    else if (match(argv[0], "ECHO"))
                    {
                        // Use the new, reformatted line for this...
                        fz_info(ctx, "::ECHO: %s\n", line + 5);
                        report_time = RPT_A_SIMPLE_USERINFO_COMMAND;
                    }
                    // `:` at start of line is *soft* ECHO, i.e. LOG but do not ECHO to screen/stdout:
                    else if (match(argv[0], ":"))
                    {
                        // Halt! Only log this line when we haven't already, due to the verbosity level!
                        if (verbosity < 1)
                        {
                            fz_info(ctx, "::%s\n", line);
                        }
                        report_time = RPT_A_SIMPLE_USERINFO_COMMAND;
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
                            if (verbosity >= 1)
                            {
                                fz_info(ctx, "OK: CD %s", argv[1]);
                            }
                        }
                        report_time = RPT_A_SIMPLE_COMMAND;
                    }
                    else if (match(argv[0], "MUTOOL"))
                    {
                        // decide whether to execute this command:
                        int chance = rand() % 1009;  // 1009 is prime and close to 1000
                        float m = random_exec_perunage * 1009 + chance;
                        int c = lroundf(m);
                        if (c < 1009)
                        {
                            fz_info(ctx, "::SKIP-DUE-TO-RANDOM-SAMPLING: MUTOOL command: %s", line);
                            report_time = RPT_A_SKIPPED_COMMAND;
                        }
                        else
                        {
                            struct trace_info prev_trace_info = trace_info;
                            trace_info.peak = 0;

                            fz_try(ctx)
                            {
                                rv = mutool_main(argc - 1, argv + 1);
                            }
                            fz_catch(ctx)
                            {
                                fz_error(ctx, "bulktest/mutool: caught otherwise unhandled exception when executing '%s': %s", scriptname, fz_convert_error(ctx, NULL));
																rv = 666;
                            }

                            if (rv != EXIT_SUCCESS)
                            {
                                fz_error(ctx, "ERR: error (exit code: %d) executing MUTOOL command: %s\n", rv, line);
                                errored++;
                            }
                            else if (verbosity >= 1)
                            {
                                fz_info(ctx, "OK: MUTOOL command: %s", line);
                            }

                            memory_consumed = trace_info.peak - prev_trace_info.current;
                            memory_allocations = trace_info.allocs - prev_trace_info.allocs;
                            memory_leaked = trace_info.current - prev_trace_info.current;

                            report_time = RPT_AN_IMPORTANT_TEST_COMMAND;

                            flush_active_logfile_hard();
                        }
                    }
                    else if (match(argv[0], "MUSERVE"))
                    {
                        rv = 1;
                        if (rv != EXIT_SUCCESS)
                        {
                            fz_error(ctx, "ERR: error (exit code: %d) executing MUSERVE command: %s", rv, line);
                            errored++;
                        }
                        else if (verbosity >= 1)
                        {
                            fz_info(ctx, "OK: MUSERVE command: %s", line);
                        }

                        report_time = RPT_AN_IMPORTANT_TEST_COMMAND;

                        flush_active_logfile_hard();
                    }
                    else if (match(argv[0], "STOPSERVE"))
                    {
                        rv = 1;
                        if (rv != EXIT_SUCCESS)
                        {
                            fz_error(ctx, "ERR: error (exit code: %d) executing STOPSERVE command: %s", rv, line);
                            errored++;
                        }
                        else if (verbosity >= 1)
                        {
                            fz_info(ctx, "OK: STOPSERVE command: %s", line);
                        }

                        report_time = RPT_AN_IMPORTANT_TEST_COMMAND;

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
                            fz_error(ctx, "ERR: error (exit code: %d) executing BULKTEST command at script line %d.", rv, linecounter);
                            errored++;
                        }
                        else if (verbosity >= 1)
                        {
                            fz_info(ctx, "OK: BULKTEST command: %s", line);
                        }

                        report_time = RPT_AN_IMPORTANT_TEST_COMMAND;

                        flush_active_logfile_hard();
                    }
                    else
                    {
                        report_time = false;
                        fz_throw(ctx, FZ_ERROR_GENERIC, "Ignoring line with UNSUPPORTED script statement:\n    %s", line);
                    }

                    if (report_time >= RPT_AN_IMPORTANT_TEST_COMMAND)
                    {
                        struct curltime now = Curl_now();
                        timediff_t diff = Curl_timediff(now, begin_time);

                        char memreport[100] = "";
                        if (memory_consumed || memory_leaked)
                        {
                            float used, leaked;
                            const char* used_unit;
                            const char* leaked_unit;

                            used = humanize(memory_consumed, &used_unit);
                            leaked = humanize(memory_leaked, &leaked_unit);

                            fz_snprintf(memreport, sizeof(memreport), "USED:%.2f%sb ", used, used_unit);
                            if (memory_leaked)
                            {
                                trace_report_pending_allocations_since(memory_snapshot_id, 1);

                                size_t l = strlen(memreport);
                                fz_snprintf(memreport + l, sizeof(memreport) - l, "LEAKED:%.2f%sb ", leaked, leaked_unit);
                            }
                        }

                        fz_info(ctx, ">L#%05u> T:%dms %s%s %s", linecounter, (int)diff, memreport, (rv ? "ERR" : "OK"), line_command);
                        if (diff >= 2000)
                        {
                            fz_info(ctx, ">L#%05u> T:%dms %s**NOTICABLY SLOW COMMAND**:: %s %s", linecounter, (int)diff, memreport, (rv ? "ERR" : "OK"), line_command);
                            if (diff >= 30000)
                            {
                                fz_info(ctx, ">L#%05u> T:%dms %s**LETHARGICALLY SLOW COMMAND**:: %s %s", linecounter, (int)diff, memreport, (rv ? "ERR" : "OK"), line_command);
                            }
                        }

                        if (showtime)
                        {
                            if (diff < timing.min)
                            {
                                timing.min = diff;
                                fz_free(ctx, timing.mincommand);
                                timing.mincommand = fz_strdup(ctx, line_command);
                                timing.minscriptline = linecounter;
                                timing.mindataline = datalinecounter;
                            }
                            if (diff > timing.max)
                            {
                                timing.max = diff;
                                fz_free(ctx, timing.maxcommand);
                                timing.maxcommand = fz_strdup(ctx, line_command);
                                timing.maxscriptline = linecounter;
                                timing.maxdataline = datalinecounter;
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

        fz_error(ctx, "Failure when executing '%s': %s", scriptname, fz_convert_error(ctx, NULL));

        struct curltime now = Curl_now();

        fz_info(ctx, "!L#%05u> T:%03dms D:%0.3lfs FAIL error: exception thrown in script file '%s' at line '%s': %s", linecounter, (int)Curl_timediff(now, begin_time), (double)Curl_timediff(now, timing.start_time) / 1E3, scriptname, (line_command ? line_command : "%--no-line--"), fz_caught_message(ctx));

        // also log to stderr if we haven't already:
        if (logcfg.logfile)
        {
            fprintf(stderr, "\n!L#%05u> T:%03dms D:%0.3lfs FAIL error: exception thrown in script file '%s' at line '%s': %s\n", linecounter, (int)Curl_timediff(now, begin_time), (double)Curl_timediff(now, timing.start_time) / 1E3, scriptname, (line_command ? line_command : "%--no-line--"), fz_caught_message(ctx));
        }

        errored += 100;
    }

    scriptname[0] = 0;
    fz_free(ctx, datafilename);
    datafilename = NULL;

    fz_flush_warnings(ctx);
    mu_drop_context();

    should_I_wait_for_key();
    return errored;
}


#include <conio.h>

static void should_I_wait_for_key(void)
{
    if (running_in_profiler_wait_for_my_key-- > 0)
    {
        if (running_in_profiler_wait_for_my_key)
            fprintf(stderr, "\n\nWaiting for your keypress to start the run...\n\n");
        else
            fprintf(stderr, "\n\nWaiting for keypress to terminate...\n\n");

        // flush keyboard buffer:
        while (kbhit())
            getch();

        // now wait...
        getch();
    }
}


static void SignalHandler(int signal)
{
	if (signal == SIGABRT) {
		// abort signal handler code
	}
	else {
		// ...
	}
	printf("BUGGER!!!!\n");
}

int
main(int argc, const char** argv)
{
	signal(SIGABRT, SignalHandler);

	atexit(should_I_wait_for_key);
    return bulktest_main(argc, argv);
}
