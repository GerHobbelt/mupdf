#include "mupdf/fitz.h"
#include "mupdf/helpers/dir.h"

#ifdef _MSC_VER
#include <direct.h>
#else
#include <unistd.h>
#endif
#include <errno.h>
#include <string.h>

int fz_chdir(fz_context* ctx, const char *path)
{
#ifdef _MSC_VER
	if (_chdir(path))
#else
	if (chdir(path))
#endif
	{
		if (ctx)
		{
			switch (errno)
			{
			case ENOENT:
				fz_throw(ctx, FZ_ERROR_GENERIC, "chdir: Unable to locate the directory: %s", path);
				break;
			case EINVAL:
				fz_throw(ctx, FZ_ERROR_GENERIC, "chdir: Invalid buffer.");
				break;
			default:
				fz_throw(ctx, FZ_ERROR_GENERIC, "chdir: Unknown error %d.", (int)errno);
				break;
			}
		}
		return errno;
	}
	return E_OK;
}

void fz_mkdir_for_file(fz_context* ctx, const char* path)
{
	char* buf = strdup(path);
	if (!buf)
		fz_throw(ctx, FZ_ERROR_GENERIC, "fz_mkdir_for_file: out of memory.");
	char* e = strrchr(buf, '/');
	if (!e) {
		// unixify MSDOS path:
		e = strchr(buf, '\\');
		while (e)
		{
			*e = '/';
			e = strchr(e, '\\');
		}
		e = strrchr(buf, '/');
	}

	if (e)
	{
		// strip off the *filename*: keep the (nested?) path intact for recursive mkdir:
		*e = 0;

		// construct the path:
		for (e = strchr(buf, '/'); e; e = strchr(e + 1, '/'))
		{
			*e = 0;
			// do not mkdir UNIX or MSDOS/WIN/Network root directory:
			if (e == buf || e[-1] == ':' || e[-1] == '/')
			{
				*e = '/';
				continue;
			}
			// just bluntly attempt to create the directory: we don't care if it fails.
			mkdir(buf);
			*e = '/';
		}
		mkdir(buf);
	}
}
