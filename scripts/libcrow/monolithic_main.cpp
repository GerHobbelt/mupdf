
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#ifdef _MSC_VER
#include <io.h>
#else
#include <unistd.h>
#endif
#include <errno.h>

#include "../../thirdparty/owemdjee/crow/include/crow/monolithic_examples.h"

#ifdef _WIN32
#include "windows.h"
#include "shellscalingapi.h"
#endif



typedef int tool_f();

static int help();
static int quit();

static struct cmd_info
{
	const char* cmd;
	tool_f* f;
} commands[] = {
	{ "basic", crow_example_basic_main },
	{ "catch_all", crow_example_catch_all_main },
	{ "chat", crow_example_chat_main },
	{ "compress", crow_example_compression_main },
	{ "json_map", crow_example_json_map_main },
	{ "static", crow_example_static_file_main },
	{ "vs", crow_example_vs_main },
	{ "with_all", crow_example_with_all_main },
	{ "hello", crow_hello_world_main },
	{ "ssl", crow_example_ssl_main },
	{ "ws", crow_example_ws_main },
	{ "mustache", crow_mustache_main },

	{ "?", help },
	{ "h", help },
	{ "help", help },
	{ "q", quit },
	{ "quit", quit },
};

static const char* xoptarg; /* Global argument pointer. */
static int xoptind = 0; /* Global argv index. */
static int xgetopt(int argc, const char** argv, const char* optstring)
{
	static const char* scan = NULL; /* Private scan pointer. */

	char c;
	const char* place;

	xoptarg = NULL;

	if (!scan || *scan == '\0') {
		if (xoptind == 0)
			xoptind++;

		if (xoptind >= argc || argv[xoptind][0] != '-' || argv[xoptind][1] == '\0')
			return EOF;
		if (argv[xoptind][1] == '-' && argv[xoptind][2] == '\0') {
			xoptind++;
			return EOF;
		}

		scan = argv[xoptind] + 1;
		xoptind++;
	}

	c = *scan++;
	place = strchr(optstring, c);

	if (!place || c == ':') {
		fprintf(stderr, "%s: unknown option -%c\n", argv[0], c);
		return '?';
	}

	place++;
	if (*place == ':') {
		if (*scan != '\0') {
			xoptarg = scan;
			scan = NULL;
		}
		else if (xoptind < argc) {
			xoptarg = argv[xoptind];
			xoptind++;
		}
		else {
			fprintf(stderr, "%s: option requires argument -%c\n", argv[0], c);
			return ':';
		}
	}

	return c;
}

#ifdef HAVE_READLINE
#include <readline/readline.h>
#include <readline/history.h>
#else
void using_history(void) { }
void add_history(const char* string) { }
void rl_bind_key(int key, void (*fun)(void)) { }
void rl_insert(void) { }
char* readline(const char* prompt)
{
	static char line[500], * p;
	size_t n;
	fputs(prompt, stdout);
	p = fgets(line, sizeof line, stdin);
	if (p) {
		n = strlen(line);
		if (n > 0 && line[n - 1] == '\n')
			line[--n] = 0;
		p = (char *)malloc(n + 1);
		memcpy(p, line, n + 1);
		return p;
	}
	return NULL;
}
#endif

#define PS1 "> "


static int parse(const char* source)
{
	for (int i = 0; i < sizeof(commands) / sizeof(commands[0]); i++)
	{
		struct cmd_info el = commands[i];
		size_t cmd_len = strlen(el.cmd);
		char sentinel = 0;

		if (strlen(source) >= cmd_len)
			sentinel = source[cmd_len];

		if (strncmp(source, el.cmd, cmd_len) == 0 && (sentinel == 0 || isspace(sentinel)))
		{
			return el.f();
		}
	}
	return 0;
}

static char* read_stdin(void)
{
	size_t n = 0;
	size_t t = 512;
	char* s = NULL;

	for (;;) {
		char* ss = (char *)realloc(s, t);
		if (!ss) {
			free(s);
			fprintf(stderr, "cannot allocate storage for stdin contents\n");
			return NULL;
		}
		s = ss;
		n += fread(s + n, 1, t - n - 1, stdin);
		if (n < t - 1)
			break;
		t *= 2;
	}

	if (ferror(stdin)) {
		free(s);
		fprintf(stderr, "error reading stdin\n");
		return NULL;
	}

	s[n] = 0;
	return s;
}

static void trim(char* s)
{
	if (!s)
		return;

	char* d = s;
	while (*s && isspace(*s))
	{
		s++;
	}
	char* e = s + strlen(s) - 1;
	while (e >= s && isspace(*e))
	{
		e--;
	}
	if (e >= s)
	{
		size_t l = e - s + 1;
		if (s > d)
			memmove(s, d, l);
		s[l] = 0;
	}
	else
	{
		*d = 0;
	}
}

static int help()
{
	fprintf(stderr, "Commands:\n");
	for (int i = 0; i < sizeof(commands) / sizeof(commands[0]); i++)
	{
		struct cmd_info el = commands[i];
		fprintf(stderr, "  %s\n", el.cmd);
	}
	fprintf(stderr, "\n\nOr type Ctrl-C to quit.\n");

	return 0;
}


static int quit()
{
	fprintf(stderr, "Exiting by user demand...\n");

	return 999;
}


static void usage(void)
{
	fprintf(stderr, "Usage: crow_demo [options] [command]\n");
	fprintf(stderr, "\t-i: Enter interactive prompt after running code.\n");
	exit(1);
}

int main(int argc, const char** argv)
{
#ifdef _WIN32
	PROCESS_DPI_AWARENESS dpi = (PROCESS_DPI_AWARENESS)(-1);
	HRESULT rv = GetProcessDpiAwareness(NULL, &dpi);
	HMONITOR mon = NULL;
	const POINT zero = { 0, 0 };
	mon = MonitorFromPoint(zero, MONITOR_DEFAULTTOPRIMARY);
	UINT x = 1000000;
	UINT y = 1000000;
	rv = GetDpiForMonitor(mon, MDT_EFFECTIVE_DPI, &x, &y);
	UINT d = GetDpiForWindow(NULL);
	DEVICE_SCALE_FACTOR scale = (DEVICE_SCALE_FACTOR)(-1);
	rv = GetScaleFactorForMonitor(mon, &scale);

	HWND w = GetConsoleWindow();
	d = GetDpiForWindow(w);
	mon = MonitorFromWindow(w, MONITOR_DEFAULTTONEAREST);
	rv = GetScaleFactorForMonitor(mon, &scale);

	rv = ::SetProcessDpiAwareness(PROCESS_SYSTEM_DPI_AWARE);
	rv = ::SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
#endif

	char* input;
	int status = 0;
	int interactive = 0;
	int c;

	while ((c = xgetopt(argc, argv, "i")) != -1) {
		switch (c) {
		default: usage(); break;
		case 'i': interactive = 1; break;
		}
	}

	if (xoptind == argc) {
		interactive = 1;
	}
	else {
		int si = xoptind;
		size_t l = 0;

		for (int i = si; i < argc; i++)
			l += strlen(argv[i]) + 1;
		l += 2;
		input = (char *)malloc(l);
		*input = 0;

		for (int i = si; i < argc; i++)
		{
			snprintf(input + strlen(input), l - strlen(input), "%s ", argv[i]);
		}
		trim(input);
		if (!input || !*input)
			status = 1;
		else
			status = parse(input);
	}

	if (interactive) {
		if (_isatty(0)) {
			using_history();
			rl_bind_key('\t', rl_insert);
			input = readline(PS1);
			while (input) {
				trim(input);
				status = parse(input);
				if (status == 999)
					break;
				if (*input)
					add_history(input);
				free(input);
				input = readline(PS1);
			}
			putchar('\n');
		}
		else {
			input = read_stdin();
			trim(input);
			if (!input || !*input)
				status = 1;
			else
				status = parse(input);
			free(input);
		}
	}

	return status;
}

