
#include "monolithic_examples.h"

#define USAGE_NAME   "OpenImageIO"

#include "monolithic_main_internal_defs.h"

// declare your own monolith dispatch table:
MONOLITHIC_CMD_TABLE_START()
{"iconvert", { .fa = oiio_iconvert_main }},
{"idiff", { .fa = oiio_idiff_main }},
{"igrep", { .fa = oiio_igrep_main }},
{"iinfo", { .fa = oiio_iinfo_main }},
{"color_test", { .fa = oiio_color_test_main }},
{"compute_test", { .fa = oiio_compute_test_main }},
{"imagebufalgo_test", { .fa = oiio_imagebufalgo_test_main }},
{"imagebuf_test", { .f = oiio_imagebuf_test_main }},
{"imagecache_test", { .f = oiio_imagecache_test_main }},
{"imageinout_test", { .fa = oiio_imageinout_test_main }},
{"imagespec_test", { .f = oiio_imagespec_test_main }},
{"imagespeed_test", { .fa = oiio_imagespeed_test_main }},
{"argparse_test", { .fa = oiio_argparse_test_main }},
{"atomic_test", { .fa = oiio_atomic_test_main }},
{"filesystem_test", { .f = oiio_filesystem_test_main }},
{"filter_test", { .fa = oiio_filter_test_main }},
{"fmath_test", { .fa = oiio_fmath_test_main }},
{"hash_test", { .fa = oiio_hash_test_main }},
{"optparser_test", { .f = oiio_optparser_test_main }},
{"parallel_test", { .fa = oiio_parallel_test_main }},
{"paramlist_test", { .fa = oiio_paramlist_test_main }},
{"simd_test", { .fa = oiio_simd_test_main }},
{"span_test", { .fa = oiio_span_test_main }},
{"spinlock_test", { .fa = oiio_spinlock_test_main }},
{"spin_rw_test", { .fa = oiio_spin_rw_test_main }},
{"strongparam_test", { .f = oiio_strongparam_test_main }},
{"strutil_test", { .f = oiio_strutil_test_main }},
{"thread_test", { .fa = oiio_thread_test_main }},
{"timer_test", { .fa = oiio_timer_test_main }},
{"typedesc_test", { .f = oiio_typedesc_test_main }},
{"type_traits_test", { .f = oiio_type_traits_test_main }},
{"ustring_test", { .fa = oiio_ustring_test_main }},
{"maketx", { .fa = oiio_maketx_main }},
{"oiiotool", { .fa = oiio_oiiotool_main }},
{"testtex", { .fa = oiio_testtex_main }},
{"imagebuf", { .f = oiio_imagebuf_main }},
{"imagebufalgo", { .f = oiio_imagebufalgo_main }},
{"imagecache", { .f = oiio_imagecache_main }},
{"imageinput", { .f = oiio_imageinput_main }},
{"imageioapi", { .f = oiio_imageioapi_main }},
{"imageoutput", { .f = oiio_imageoutput_main }},
{"texturesys", { .f = oiio_texturesys_main }},
{"writingplugins", { .f = oiio_writingplugins_main }},
MONOLITHIC_CMD_TABLE_END();

#include "monolithic_main_tpl.h"

