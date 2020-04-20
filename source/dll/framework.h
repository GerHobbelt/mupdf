#pragma once

#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>

#include "mupdf/fitz.h"

#if FZ_ENABLE_PDF
#include "mupdf/pdf.h" /* for pdf output */
#endif

#ifndef DISABLE_MUTHREADS
#include "mupdf/helpers/mu-threads.h"
#endif

#include "mupdf/helpers/pkcs7-openssl.h"

/* Allow for windows stdout being made binary */
#ifdef _WIN32
#include <io.h>
#include <fcntl.h>
#endif

#include <string.h>
#include <limits.h>
#include <sys/stat.h>

#ifdef _MSC_VER
#define stat _stat
#endif

#ifdef _WIN32
#include <windows.h>
#include <direct.h> /* for getcwd */
#else
#include <unistd.h> /* for getcwd */
#endif

#define BEYOND_THRESHHOLD 40

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

#ifndef MAX
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#endif
