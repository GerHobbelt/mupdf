
// fix ubiquitous warning C5204: 'boost::blah::blah<Blah>': class has virtual functions, but its trivial destructor is not virtual; instances of objects derived from this class may not be destructed correctly
//
// while this generally CAN/MAY be an issue, we brutally assume that the Boost Boys know their shit and NEVER make this mistake, i.e. the classes/structs in question are *implicitly `final`*!
// To play it kinda safe, we apply this 'please shut up' fix to this dummy=see-if-we-can-compile-Ev'ryTing source file only:

#if defined(_MSC_VER)
#pragma warning(disable: 5204)

// fix/shut up warning C4266: 'void boost::blah::blah::blah(...)': no override available for virtual member function from base 'boost::blah::blah'; function is hidden
#pragma warning(disable: 4266)
#endif

#include <boost/wave.hpp>
#include <boost/wave/cpp_context.hpp>
#include <boost/wave/cpp_exceptions.hpp>
#include <boost/wave/cpp_iteration_context.hpp>
#include <boost/wave/cpp_throw.hpp>
#include <boost/wave/cpplexer/convert_trigraphs.hpp>
#include <boost/wave/cpplexer/cpp_lex_interface.hpp>
#include <boost/wave/cpplexer/cpp_lex_interface_generator.hpp>
#include <boost/wave/cpplexer/cpp_lex_iterator.hpp>
#include <boost/wave/cpplexer/cpp_lex_token.hpp>
#include <boost/wave/cpplexer/cpplexer_exceptions.hpp>
#include <boost/wave/cpplexer/detect_include_guards.hpp>
#include <boost/wave/cpplexer/re2clex/aq.hpp>
#include <boost/wave/cpplexer/re2clex/cpp_re.hpp>
#include <boost/wave/cpplexer/re2clex/cpp_re.inc>
#include <boost/wave/cpplexer/re2clex/cpp_re2c_lexer.hpp>
#include <boost/wave/cpplexer/re2clex/scanner.hpp>
#include <boost/wave/cpplexer/re2clex/strict_cpp_re.inc>
#include <boost/wave/cpplexer/token_cache.hpp>
#include <boost/wave/cpplexer/validate_universal_char.hpp>
#include <boost/wave/grammars/cpp_chlit_grammar.hpp>
#include <boost/wave/grammars/cpp_defined_grammar.hpp>
#include <boost/wave/grammars/cpp_defined_grammar_gen.hpp>
#include <boost/wave/grammars/cpp_expression_grammar.hpp>
#include <boost/wave/grammars/cpp_expression_grammar_gen.hpp>
#include <boost/wave/grammars/cpp_expression_value.hpp>
#include <boost/wave/grammars/cpp_grammar.hpp>
#include <boost/wave/grammars/cpp_grammar_gen.hpp>
#include <boost/wave/grammars/cpp_has_include_grammar.hpp>
#include <boost/wave/grammars/cpp_has_include_grammar_gen.hpp>
#include <boost/wave/grammars/cpp_intlit_grammar.hpp>
#include <boost/wave/grammars/cpp_literal_grammar_gen.hpp>
#include <boost/wave/grammars/cpp_predef_macros_gen.hpp>
#include <boost/wave/grammars/cpp_predef_macros_grammar.hpp>
#include <boost/wave/grammars/cpp_value_error.hpp>
#include <boost/wave/language_support.hpp>
#include <boost/wave/preprocessing_hooks.hpp>
#include <boost/wave/token_ids.hpp>
#include <boost/wave/util/cpp_ifblock.hpp>
#include <boost/wave/util/cpp_include_paths.hpp>
#include <boost/wave/util/cpp_iterator.hpp>
#include <boost/wave/util/cpp_macromap.hpp>
#include <boost/wave/util/cpp_macromap_predef.hpp>
#include <boost/wave/util/cpp_macromap_utils.hpp>
#include <boost/wave/util/file_position.hpp>
#include <boost/wave/util/filesystem_compatibility.hpp>
#include <boost/wave/util/flex_string.hpp>
#include <boost/wave/util/functor_input.hpp>
#include <boost/wave/util/insert_whitespace_detection.hpp>
#include <boost/wave/util/interpret_pragma.hpp>
#include <boost/wave/util/iteration_context.hpp>
#include <boost/wave/util/macro_definition.hpp>
#include <boost/wave/util/macro_helpers.hpp>
#include <boost/wave/util/pattern_parser.hpp>
#include <boost/wave/util/symbol_table.hpp>
#include <boost/wave/util/time_conversion_helper.hpp>
#include <boost/wave/util/transform_iterator.hpp>
#include <boost/wave/util/unput_queue_iterator.hpp>
#include <boost/wave/wave_config.hpp>
#include <boost/wave/wave_config_constant.hpp>
#include <boost/wave/whitespace_handling.hpp>

