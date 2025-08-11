
// fix ubiquitous warning C5204: 'boost::blah::blah<Blah>': class has virtual functions, but its trivial destructor is not virtual; instances of objects derived from this class may not be destructed correctly
//
// while this generally CAN/MAY be an issue, we brutally assume that the Boost Boys know their shit and NEVER make this mistake, i.e. the classes/structs in question are *implicitly `final`*!
// To play it kinda safe, we apply this 'please shut up' fix to this dummy=see-if-we-can-compile-Ev'ryTing source file only:

#if defined(_MSC_VER)
#pragma warning(disable: 5204)

// fix/shut up warning C4266: 'void boost::blah::blah::blah(...)': no override available for virtual member function from base 'boost::blah::blah'; function is hidden
#pragma warning(disable: 4266)
#endif

#include <boost/cregex.hpp>
#include <boost/regex.h>
#include <boost/regex.hpp>
#include <boost/regex/concepts.hpp>
#include <boost/regex/config.hpp>
#include <boost/regex/icu.hpp>
#include <boost/regex/mfc.hpp>
#include <boost/regex/pattern_except.hpp>
#include <boost/regex/regex_traits.hpp>
#include <boost/regex/user.hpp>
#include <boost/regex/v5/basic_regex.hpp>
#include <boost/regex/v5/basic_regex_creator.hpp>
#include <boost/regex/v5/basic_regex_parser.hpp>
#include <boost/regex/v5/c_regex_traits.hpp>
#include <boost/regex/v5/char_regex_traits.hpp>
#include <boost/regex/v5/cpp_regex_traits.hpp>
#include <boost/regex/v5/cregex.hpp>
#include <boost/regex/v5/error_type.hpp>
#include <boost/regex/v5/icu.hpp>
#include <boost/regex/v5/iterator_category.hpp>
#include <boost/regex/v5/iterator_traits.hpp>
#include <boost/regex/v5/match_flags.hpp>
#include <boost/regex/v5/match_results.hpp>
#include <boost/regex/v5/mem_block_cache.hpp>
#include <boost/regex/v5/object_cache.hpp>
#include <boost/regex/v5/pattern_except.hpp>
#include <boost/regex/v5/perl_matcher.hpp>
#include <boost/regex/v5/perl_matcher_common.hpp>
#include <boost/regex/v5/perl_matcher_non_recursive.hpp>
#include <boost/regex/v5/primary_transform.hpp>
#include <boost/regex/v5/regbase.hpp>
#include <boost/regex/v5/regex.hpp>
#include <boost/regex/v5/regex_format.hpp>
#include <boost/regex/v5/regex_fwd.hpp>
#include <boost/regex/v5/regex_grep.hpp>
#include <boost/regex/v5/regex_iterator.hpp>
#include <boost/regex/v5/regex_match.hpp>
#include <boost/regex/v5/regex_merge.hpp>
#include <boost/regex/v5/regex_raw_buffer.hpp>
#include <boost/regex/v5/regex_replace.hpp>
#include <boost/regex/v5/regex_search.hpp>
#include <boost/regex/v5/regex_split.hpp>
#include <boost/regex/v5/regex_token_iterator.hpp>
#include <boost/regex/v5/regex_traits.hpp>
#include <boost/regex/v5/regex_traits_defaults.hpp>
#include <boost/regex/v5/regex_workaround.hpp>
#include <boost/regex/v5/states.hpp>
#include <boost/regex/v5/sub_match.hpp>
#include <boost/regex/v5/syntax_type.hpp>
#include <boost/regex/v5/u32regex_iterator.hpp>
#include <boost/regex/v5/u32regex_token_iterator.hpp>
#include <boost/regex/v5/unicode_iterator.hpp>
#include <boost/regex/v5/w32_regex_traits.hpp>
#include <boost/regex_fwd.hpp>

