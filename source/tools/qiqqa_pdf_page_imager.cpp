
// tool which (judiciously) produces page images from PDF files.
// Can be set to act smart, i.e. only produce page images for those pdf pages which still need OCR'ing if you want those to have a text layer after all.

#include "unicode/utypes.h"
#include "unicode/normalizer2.h"
#include "unicode/uspoof.h"
#include "unicode/ustring.h"
#include "unicode/utf16.h"

