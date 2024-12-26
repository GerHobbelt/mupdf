
// image processing (+ OCR'ing) tuning tool: can be instructed to try various
// approaches and discover which one works best for the given input.
// Can save this 'optimal process' as a configuration script for human tweaking and/or
// later re-application.
//
// As such, it acts like a piano tuner to your image OCR+preprocessing act.

#include "unicode/utypes.h"
#include "unicode/normalizer2.h"
#include "unicode/uspoof.h"
#include "unicode/ustring.h"
#include "unicode/utf16.h"

