
// using libpathutils.
// tool to help clean file names & file paths to prevent filesystem b0rkage and other foul mishaps.
// can be used to produce clean output filenames for web scrapers, e.g. cURL tool output paths.

#include "unicode/utypes.h"
#include "unicode/normalizer2.h"
#include "unicode/uspoof.h"
#include "unicode/ustring.h"
#include "unicode/utf16.h"

