
#include "timeval.h"

#include "pdfapp.h"
#include "mupdf/helpers/dir.h"

#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#if defined(_MSC_VER)
#include <crtdbg.h>
#endif

int mutool_main(int argc, const char** argv);

/*
	A useful bit of bash script to call this is:
	for f in ../ghostpcl/tests_private/pdf/forms/v1.3/ *.pdf ; do g=${f%.*} ; echo $g ; win32/debug/mujstest-v8.exe -o $g-%d.png -p ../ghostpcl/ $g.mjs > $g.log 2>&1 ; done

	Remove the space from "/ *.pdf" before running - can't leave that
	in here, as it causes a warning about a possibly malformed comment.
*/

#define LONGLINE 4096

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
	fprintf(stderr, "warning: %s\n", msg);
}

void winerror(pdfapp_t *app, char *msg)
{
	fprintf(stderr, "%s\n", msg);
	exit(1);
}

void winalert(pdfapp_t *app, pdf_alert_event *alert)
{
	fprintf(stderr, "Alert %s: %s\n", alert->title, alert->message);
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
	fprintf(stderr, "The MuPDF library supports printing, but this application currently does not\n");
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
	fprintf(stderr, "mujstest: Scriptable tester for mupdf + js\n");
	fprintf(stderr, "\nSyntax: mujstest -o <filename> [ -p <prefix> ] [-v] <scriptfile>\n");
	fprintf(stderr, "\n<filename> should sensibly be of the form file-%%d.png\n");
	fprintf(stderr, "\n<prefix> is a path prefix to apply to filenames within the script\n");
	fprintf(stderr, "\n-v\tverbose\n");
	fprintf(stderr, "\nscriptfile contains a list of commands:\n");
	fprintf(stderr, "\tPASSWORD <password>\tSet the password\n");
	fprintf(stderr, "\tOPEN <filename>\tOpen a file\n");
	fprintf(stderr, "\tGOTO <page>\tJump to a particular page\n");
	fprintf(stderr, "\tSCREENSHOT\tSave a screenshot\n");
	fprintf(stderr, "\tRESIZE <w> <h>\tResize the screen to a given size\n");
	fprintf(stderr, "\tCLICK <x> <y> <btn>\tClick at a given position\n");
	fprintf(stderr, "\tTEXT <string>\tSet a value to be entered\n");
	fprintf(stderr, "\tMUTOOL <arguments>\tRun any mutool command as specified\n");
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
		fprintf(stderr, "getline: line too long.\n");
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
	char** start = malloc(len * sizeof(start[0]) + len + 2);
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
				fz_throw(ctx, FZ_ERROR_GENERIC, "MUTOOL command error: unterminated string parameter: %s\n", s);
				return;
			}

			// point at terminating quote, check if it is followed by whitespace or EOL:
			if (e[1] != 0 && !isspace((unsigned char)e[1]))
			{
				fz_throw(ctx, FZ_ERROR_GENERIC, "MUTOOL command error: whitespace or end of command expected after quoted string parameter: %s\n", s);
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

// return true when string s matches all specified parts *sequentially*, i.e. in order of appearance.
//
// parts are a list of strings, terminated by a NULL entry.
static int strmatch(const char* s, const char* part, ...)
{
	va_list argptr;

	for (va_start(argptr, part); part; part = va_arg(argptr, const char *))
	{
		const char* m = strstr(s, part);
		if (!m)
			return FALSE;
		s = m + strlen(part);
	}
	va_end(argptr);
	return TRUE;
}

static const char* strGiveNewline(const char* s)
{
	if (!s)
		return "";
	if (!*s)
		return "\n";
	s += strlen(s) - 1;
	if (*s != '\n')
		return "\n";
	return "";
}

static char memTestMsgBuf[8192] = "";
static int memTestReportType = -1;
static int memTestInMemLeakDump = 0;

static int __cdecl TestMemHook(int nReportType, const char* szMsg, int* pnRet)
{
	static const char* RptTypes[] = { "Warning", "Error", "Assert", "???Unknown???" };
	if (nReportType < 0 || nReportType > 2)
		nReportType = 3;

	if (strmatch(szMsg, "Dumping objects ->", NULL))
	{
		memTestInMemLeakDump = 2; // start dump...
	}
	else if (strmatch(szMsg, "Object dump complete.", NULL))
	{
		memTestInMemLeakDump = 0;
	}

	// don't list memory dump statistics about 0 bytes in 0 blocks: that's noise for our purposes:
	if (!strmatch(szMsg, "0 bytes in 0 ", " Blocks.", NULL)
		&& !strmatch(szMsg, "Largest number used : 0 bytes.", NULL)
		&& !strmatch(szMsg, "Total allocations: 0 bytes.", NULL)
	)
	{
		if (*memTestMsgBuf && memTestReportType != nReportType)
		{
			fprintf(stderr, "**%s**: %s%s", RptTypes[memTestReportType], memTestMsgBuf, strGiveNewline(memTestMsgBuf));
			memTestMsgBuf[0] = 0;
		}
		if (memTestInMemLeakDump != 1)
		{
			memTestReportType = nReportType;
			strcat(memTestMsgBuf, szMsg);
			// has last message bit appended an EOL as last char?
			if (!*strGiveNewline(szMsg))
			{
				if (memTestInMemLeakDump == 2 && (
					strmatch(memTestMsgBuf, "minkernel\\crts\\ucrt\\src\\appcrt\\stdio\\stream.cpp", " crt block at ", NULL) ||
					strmatch(memTestMsgBuf, "minkernel\\crts\\ucrt\\src\\appcrt\\stdio\\_getbuf.cpp", " crt block at ", NULL) ||
					strmatch(memTestMsgBuf, "minkernel\\crts\\ucrt\\src\\appcrt\\stdio\\_sftbuf.cpp", " crt block at ", NULL)
					)
				)
				{
					memTestInMemLeakDump = 1; // skip till end
					strcpy(memTestMsgBuf, ">>> CRT app start allocations ignored <<<\n");
				}

				if (memTestInMemLeakDump != 1)
					fprintf(stderr, "**%s**: %s", RptTypes[memTestReportType], memTestMsgBuf);
				memTestMsgBuf[0] = 0;
			}
		}
	}

	if (strmatch(szMsg, "HEAP CORRUPTION DETECTED", NULL))
	{
		szMsg = "blub";
	}

	if (pnRet)
		*pnRet = 0;

	return FALSE;
}

static int __cdecl TestMemDumpCB(
	int      nAllocType,
	void* pvData,
	size_t   nSize,
	int      nBlockUse,
	long     lRequest,
	const unsigned char* szFileName,
	int      nLine
)
{
	static const char* operation[] = { "", "allocating", "re-allocating", "freeing", "??unknown action??" };
	static const char* blockType[] = { "Free", "Normal", "CRT", "Ignore", "Client", "??Unknown??" };

	if (nBlockUse == _CRT_BLOCK)   // Ignore internal C runtime library allocations
		return TRUE;

	if (nAllocType <= 0 || nAllocType > 3)
		nAllocType = 4;
	if (nBlockUse < 0 || nBlockUse > 4)
		nBlockUse = 5;

	if (nBlockUse == 3 && nSize == 0 && nBlockUse == _NORMAL_BLOCK && lRequest == 0 && nLine == 0)
		return TRUE;

#if 0
	fprintf(stderr, "MEM: %s %zu bytes '%s' block (A:%p) {#%ld} in %s @ %d\n",
		operation[nAllocType], nSize,
		blockType[nBlockUse], pvData, lRequest, szFileName, nLine);
#endif

	return TRUE;         // Allow the memory operation to proceed
}

int
main(int argc, char *argv[])
{
	fz_context *ctx;
	FILE *script = NULL;
	FILE* logfile = NULL;
	int c;
	int errored = 0;
	unsigned int linecounter = 0;
	int rv = 0;
	struct curltime start_time;
	struct curltime begin_time;
	const char* line_command = NULL;

	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF
		| _CRTDBG_CHECK_EVERY_1024_DF
		//| _CRTDBG_CHECK_EVERY_16_DF
		//| _CRTDBG_CHECK_ALWAYS_DF
		| _CRTDBG_DELAY_FREE_MEM_DF
		| _CRTDBG_LEAK_CHECK_DF
	    | _CRTDBG_CHECK_CRT_DF
	);

	// set all report modes to NIL as the custom report function will take care of all reporting instead!
	_CrtSetReportMode(_CRT_ASSERT, 0); // _CRTDBG_MODE_FILE | _CRTDBG_MODE_DEBUG);
	_CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
	_CrtSetReportMode(_CRT_WARN, 0); // _CRTDBG_MODE_FILE | _CRTDBG_MODE_DEBUG);
	_CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDERR);
	_CrtSetReportMode(_CRT_ERROR, 0); // _CRTDBG_MODE_FILE | _CRTDBG_MODE_DEBUG);
	_CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDERR);

	_CrtSetReportHook2(_CRT_RPTHOOK_INSTALL, TestMemHook);

	_CrtSetAllocHook(TestMemDumpCB);

	fz_getopt_reset();
	while ((c = fz_getopt(argc, argv, "o:p:v")) != -1)
	{
		switch(c)
		{
		case 'o': output = fz_optarg; break;
		case 'p': prefix = fz_optarg; break;
		case 'v': verbosity ^= 1; break;
		default: usage(); return EXIT_FAILURE;
		}
	}

	if (fz_optind == argc)
	{
		usage();
		return EXIT_FAILURE;
	}

	ctx = fz_new_context(NULL, NULL, FZ_STORE_DEFAULT);
	if (!ctx)
	{
		fprintf(stderr, "cannot initialise context\n");
		return EXIT_FAILURE;
	}
	pdfapp_init(ctx, &gapp);
	gapp.scrw = 640;
	gapp.scrh = 480;
	gapp.colorspace = fz_device_rgb(ctx);

	_CrtMemState mem_start, mem_begin, mem_end;
	_CrtMemCheckpoint(&mem_start);

	fz_try(ctx)
	{
		start_time = Curl_now();

		while (fz_optind < argc)
		{
			char logfilename[4096];

			scriptname = argv[fz_optind++];
			script = fopen(scriptname, "rb");
			if (script == NULL)
				fz_throw(ctx, FZ_ERROR_GENERIC, "cannot open script: %s", scriptname);

			fz_snprintf(logfilename, sizeof(logfilename), "%s.log", scriptname);
			logfile = fopen(logfilename, "w");

			for(;;)
			{
				bool report_time = true;
				char *line = my_getline(script);
				line_command = line;
				rv = 0;

				linecounter++;
				fflush(logfile);

				_CrtMemCheckpoint(&mem_begin);

				if (line == NULL)
				{
					if (ferror(script))
					{
						fprintf(stderr, "script read error: %s (%s)\n", strerror(errno), scriptname);
					}
					break;
				}
				if (verbosity)
					fprintf(stderr, "'%s'\n", line);

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
						fprintf(stderr, "ERR: error executing MUTOOL command: %s\n", line);
						errored = 1;
					}
					else if (verbosity)
					{
						fprintf(stderr, "OK: MUTOOL command: %s\n", line);
					}
					free((void *)argv);
				}
				else
				{
					report_time = false;
					if (verbosity)
						fprintf(stderr, "Ignoring line without script statement.\n");
				}

				if (report_time)
				{
					struct curltime now = Curl_now();
					fprintf(stderr, "T:%03dms D:%0.3lfs %s %s\n", (int)Curl_timediff(now, begin_time), (double)Curl_timediff(now, start_time) / 1E3, (rv ? "ERR" : "OK"), line_command);

					fprintf(logfile, "L:%05u T:%03dms D:%0.3lfs %s %s\n", linecounter, (int)Curl_timediff(now, begin_time), (double)Curl_timediff(now, start_time) / 1E3, (rv ? "ERR" : "OK"), line_command);
				}

				_CrtMemCheckpoint(&mem_end);

				_CrtCheckMemory();

				// _CrtDumpMemoryLeaks();
				_CrtMemState mem_diff = { 0 };
				_CrtMemDifference(&mem_diff, &mem_begin, &mem_end);
				_CrtMemDumpStatistics(&mem_diff);

				_CrtDumpMemoryLeaks();
			}
			while (!feof(script));

			fclose(script);
		}
	}
	fz_catch(ctx)
	{
		fprintf(stderr, "error: cannot execute '%s': %s\n", scriptname, fz_caught_message(ctx));

		if (logfile)
		{
			struct curltime now = Curl_now();

			fprintf(logfile, "L:%05u T:%03dms D:%0.3lfs FAIL error: exception thrown in script file '%s' at line '%s': %s\n", linecounter, (int)Curl_timediff(now, begin_time), (double)Curl_timediff(now, start_time) / 1E3, scriptname, (line_command ? line_command : "%--no-line--"), fz_caught_message(ctx));
		}
		errored = 1;
		_CrtCheckMemory();
	}

	if (logfile)
	{
		fclose(logfile);
		logfile = NULL;
	}

	if (file_open)
		pdfapp_close(&gapp);

	fz_flush_warnings(ctx);
	fz_drop_context(ctx);

	_CrtDumpMemoryLeaks();

	return errored;
}


// override run-time library functions in debug mode: all code which is not properly
// wrapped/processed ends up calling one or more of these and then we can stacktrace
// them to find out where and what needs to be done to complete the heap check/validation
// code patches:

#undef _calloc_base
__declspec(dllexport)
void* __cdecl _calloc_base(size_t a, size_t b)
{
	printf("kaboom");
	return NULL;
}
#undef calloc
__declspec(dllexport)
void* __cdecl calloc(size_t a, size_t b)
{
	printf("kaboom");
	return NULL;
}
#undef free
__declspec(dllexport)
void  __cdecl free(void *p)
{
	printf("kaboom");
}
#undef malloc
__declspec(dllexport)
void * __cdecl malloc(size_t s)
{
	//printf("kaboom");
	return _malloc_dbg(s, _NORMAL_BLOCK, __FILE__, __LINE__);
}
#undef realloc
__declspec(dllexport)
void * __cdecl realloc(void *p, size_t s)
{
	printf("kaboom");
	return NULL;
}
#undef _strdup
__declspec(dllexport)
char * __cdecl _strdup(const char *s)
{
	printf("kaboom");
	return NULL;
}
#undef strdup
__declspec(dllexport)
char * __cdecl strdup(const char *s)
{
	printf("kaboom");
	return NULL;
}
#undef wcsdup
__declspec(dllexport)
wchar_t * __cdecl wcsdup(const wchar_t *s)
{
	printf("kaboom");
	return NULL;
}
