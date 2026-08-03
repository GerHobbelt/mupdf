
// fix ubiquitous warning C5204: 'boost::blah::blah<Blah>': class has virtual functions, but its trivial destructor is not virtual; instances of objects derived from this class may not be destructed correctly
//
// while this generally CAN/MAY be an issue, we brutally assume that the Boost Boys know their shit and NEVER make this mistake, i.e. the classes/structs in question are *implicitly `final`*!
// To play it kinda safe, we apply this 'please shut up' fix to this dummy=see-if-we-can-compile-Ev'ryTing source file only:

#if defined(_MSC_VER)
#pragma warning(disable: 5204)

// fix/shut up warning C4266: 'void boost::blah::blah::blah(...)': no override available for virtual member function from base 'boost::blah::blah'; function is hidden
#pragma warning(disable: 4266)
#endif

#include <boost/spirit/config.hpp>
#include <boost/spirit/core/type_traits.hpp>
#include <boost/spirit/x4.hpp>
#include <boost/spirit/x4/allocator.hpp>
#include <boost/spirit/x4/ast/position_tagged.hpp>
#include <boost/spirit/x4/auxiliary.hpp>
#include <boost/spirit/x4/auxiliary/attr.hpp>
#include <boost/spirit/x4/auxiliary/eoi.hpp>
#include <boost/spirit/x4/auxiliary/eol.hpp>
#include <boost/spirit/x4/auxiliary/eps.hpp>
#include <boost/spirit/x4/char.hpp>
#include <boost/spirit/x4/char/any_char.hpp>
#include <boost/spirit/x4/char/char.hpp>
#include <boost/spirit/x4/char/char_class.hpp>
#include <boost/spirit/x4/char/char_class_tags.hpp>
#include <boost/spirit/x4/char/char_parser.hpp>
#include <boost/spirit/x4/char/char_set.hpp>
#include <boost/spirit/x4/char/literal_char.hpp>
#include <boost/spirit/x4/char/negated_char.hpp>
#include <boost/spirit/x4/char/unicode_char_class.hpp>
#include <boost/spirit/x4/char_encoding/standard.hpp>
#include <boost/spirit/x4/char_encoding/standard_wide.hpp>
#include <boost/spirit/x4/char_encoding/unicode.hpp>
#include <boost/spirit/x4/char_encoding/unicode/classification.hpp>
#include <boost/spirit/x4/core/action.hpp>
#include <boost/spirit/x4/core/action_context.hpp>
#include <boost/spirit/x4/core/attribute.hpp>
#include <boost/spirit/x4/core/container_appender.hpp>
#include <boost/spirit/x4/core/context.hpp>
#include <boost/spirit/x4/core/expectation.hpp>
#include <boost/spirit/x4/core/move_to.hpp>
#include <boost/spirit/x4/core/parser.hpp>
#include <boost/spirit/x4/core/parser_traits.hpp>
#include <boost/spirit/x4/core/skip_over.hpp>
#include <boost/spirit/x4/core/unused.hpp>
#include <boost/spirit/x4/debug/annotate_on_success.hpp>
#include <boost/spirit/x4/debug/debug_handler_state.hpp>
#include <boost/spirit/x4/debug/error_reporting.hpp>
#include <boost/spirit/x4/debug/print_attribute.hpp>
#include <boost/spirit/x4/debug/print_token.hpp>
#include <boost/spirit/x4/debug/simple_trace.hpp>
#include <boost/spirit/x4/directive.hpp>
#include <boost/spirit/x4/directive/as.hpp>
#include <boost/spirit/x4/directive/expect.hpp>
#include <boost/spirit/x4/directive/lexeme.hpp>
#include <boost/spirit/x4/directive/matches.hpp>
#include <boost/spirit/x4/directive/no_case.hpp>
#include <boost/spirit/x4/directive/no_skip.hpp>
#include <boost/spirit/x4/directive/omit.hpp>
#include <boost/spirit/x4/directive/raw.hpp>
#include <boost/spirit/x4/directive/repeat.hpp>
#include <boost/spirit/x4/directive/seek.hpp>
#include <boost/spirit/x4/directive/skip.hpp>
#include <boost/spirit/x4/directive/with.hpp>
#include <boost/spirit/x4/directive/with_local.hpp>
#include <boost/spirit/x4/numeric.hpp>
#include <boost/spirit/x4/numeric/bool.hpp>
#include <boost/spirit/x4/numeric/int.hpp>
#include <boost/spirit/x4/numeric/real.hpp>
#include <boost/spirit/x4/numeric/uint.hpp>
#include <boost/spirit/x4/numeric/utils/extract_int.hpp>
#include <boost/spirit/x4/numeric/utils/extract_real.hpp>
#include <boost/spirit/x4/numeric/utils/pow10.hpp>
#include <boost/spirit/x4/operator.hpp>
#include <boost/spirit/x4/operator/alternative.hpp>
#include <boost/spirit/x4/operator/and_predicate.hpp>
#include <boost/spirit/x4/operator/difference.hpp>
#include <boost/spirit/x4/operator/kleene.hpp>
#include <boost/spirit/x4/operator/list.hpp>
#include <boost/spirit/x4/operator/not_predicate.hpp>
#include <boost/spirit/x4/operator/optional.hpp>
#include <boost/spirit/x4/operator/plus.hpp>
#include <boost/spirit/x4/operator/sequence.hpp>
#include <boost/spirit/x4/parse.hpp>
#include <boost/spirit/x4/parse_result.hpp>
#include <boost/spirit/x4/rule.hpp>
#include <boost/spirit/x4/string.hpp>
#include <boost/spirit/x4/string/case_compare.hpp>
#include <boost/spirit/x4/string/literal_string.hpp>
#include <boost/spirit/x4/string/string.hpp>
#include <boost/spirit/x4/string/tst.hpp>
#include <boost/spirit/x4/string/utf8.hpp>
#include <boost/spirit/x4/symbols.hpp>
#include <boost/spirit/x4/traits/attribute_category.hpp>
#include <boost/spirit/x4/traits/attribute_of_binary.hpp>
#include <boost/spirit/x4/traits/char_encoding_traits.hpp>
#include <boost/spirit/x4/traits/container_traits.hpp>
#include <boost/spirit/x4/traits/numeric_traits.hpp>
#include <boost/spirit/x4/traits/optional_traits.hpp>
#include <boost/spirit/x4/traits/string_traits.hpp>
#include <boost/spirit/x4/traits/subrange_traits.hpp>
#include <boost/spirit/x4/traits/substitution.hpp>
#include <boost/spirit/x4/traits/transform_attribute.hpp>
#include <boost/spirit/x4/traits/tuple_traits.hpp>
#include <boost/spirit/x4/traits/variant_traits.hpp>

