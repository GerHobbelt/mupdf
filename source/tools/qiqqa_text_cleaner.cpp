
// tool to help clean text; possibly removing cruft, accents, normalization,
// stemming, ASCII-fication via pinyin and other means, ... all you'd ever
// want in a text preprocessor before haring off into h-gram production for FTS
// and their ilk.

#include "unicode/utypes.h"
#include "unicode/normalizer2.h"
#include "unicode/uspoof.h"
#include "unicode/ustring.h"
#include "unicode/utf16.h"

