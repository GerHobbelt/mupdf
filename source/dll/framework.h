#pragma once

#include "mupdf/helpers/system-header-files.h"

#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif

#include "mupdf/fitz.h"

#if FZ_ENABLE_PDF
#include "mupdf/pdf.h" /* for pdf output */
#endif

#include "mupdf/helpers/mu-threads.h"

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
