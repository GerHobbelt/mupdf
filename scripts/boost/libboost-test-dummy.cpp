
// fix ubiquitous warning C5204: 'boost::blah::blah<Blah>': class has virtual functions, but its trivial destructor is not virtual; instances of objects derived from this class may not be destructed correctly
//
// while this generally CAN/MAY be an issue, we brutally assume that the Boost Boys know their shit and NEVER make this mistake, i.e. the classes/structs in question are *implicitly `final`*!
// To play it kinda safe, we apply this 'please shut up' fix to this dummy=see-if-we-can-compile-Ev'ryTing source file only:

#if defined(_MSC_VER)
#pragma warning(disable: 5204)

// fix/shut up warning C4266: 'void boost::blah::blah::blah(...)': no override available for virtual member function from base 'boost::blah::blah'; function is hidden
#pragma warning(disable: 4266)
#endif

#include <boost/test/auto_unit_test.hpp>
#include <boost/test/data/config.hpp>
#include <boost/test/data/dataset.hpp>
#include <boost/test/data/for_each_sample.hpp>
#include <boost/test/data/index_sequence.hpp>
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/monomorphic/array.hpp>
#include <boost/test/data/monomorphic/collection.hpp>
#include <boost/test/data/monomorphic/delayed.hpp>
#include <boost/test/data/monomorphic/fwd.hpp>
#include <boost/test/data/monomorphic/generate.hpp>
#include <boost/test/data/monomorphic/generators.hpp>
#include <boost/test/data/monomorphic/generators/keywords.hpp>
#include <boost/test/data/monomorphic/generators/random.hpp>
#include <boost/test/data/monomorphic/generators/xrange.hpp>
#include <boost/test/data/monomorphic/grid.hpp>
#include <boost/test/data/monomorphic/initializer_list.hpp>
#include <boost/test/data/monomorphic/join.hpp>
#include <boost/test/data/monomorphic/sample_merge.hpp>
#include <boost/test/data/monomorphic/singleton.hpp>
#include <boost/test/data/monomorphic/zip.hpp>
#include <boost/test/data/size.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/debug.hpp>
#include <boost/test/debug_config.hpp>
#include <boost/test/execution_monitor.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include <boost/test/framework.hpp>
#include <boost/test/included/execution_monitor.hpp>
#include <boost/test/included/prg_exec_monitor.hpp>
#include <boost/test/included/test_exec_monitor.hpp>
#include <boost/test/included/unit_test.hpp>
#include <boost/test/included/unit_test_framework.hpp>
#include <boost/test/minimal.hpp>
#include <boost/test/output/compiler_log_formatter.hpp>
#include <boost/test/output/junit_log_formatter.hpp>
#include <boost/test/output/plain_report_formatter.hpp>
#include <boost/test/output/xml_log_formatter.hpp>
#include <boost/test/output/xml_report_formatter.hpp>
#include <boost/test/output_test_stream.hpp>
#include <boost/test/parameterized_test.hpp>
#include <boost/test/predicate_result.hpp>
#include <boost/test/prg_exec_monitor.hpp>
#include <boost/test/progress_monitor.hpp>
#include <boost/test/results_collector.hpp>
#include <boost/test/results_reporter.hpp>
#include <boost/test/test_case_template.hpp>
#include <boost/test/test_exec_monitor.hpp>
#include <boost/test/test_framework_init_observer.hpp>
#include <boost/test/test_tools.hpp>
#include <boost/test/tools/assertion.hpp>
#include <boost/test/tools/assertion_result.hpp>
#include <boost/test/tools/collection_comparison_op.hpp>
#include <boost/test/tools/context.hpp>
#include <boost/test/tools/cstring_comparison_op.hpp>
#include <boost/test/tools/floating_point_comparison.hpp>
#include <boost/test/tools/fpc_op.hpp>
#include <boost/test/tools/fpc_tolerance.hpp>
#include <boost/test/tools/interface.hpp>
#include <boost/test/tools/old/impl.hpp>
#include <boost/test/tools/old/interface.hpp>
#include <boost/test/tools/output_test_stream.hpp>
#include <boost/test/tree/auto_registration.hpp>
#include <boost/test/tree/decorator.hpp>
#include <boost/test/tree/fixture.hpp>
#include <boost/test/tree/global_fixture.hpp>
#include <boost/test/tree/observer.hpp>
#include <boost/test/tree/test_case_counter.hpp>
#include <boost/test/tree/test_case_template.hpp>
#include <boost/test/tree/test_unit.hpp>
#include <boost/test/tree/traverse.hpp>
#include <boost/test/tree/visitor.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_log.hpp>
#include <boost/test/unit_test_log_formatter.hpp>
#include <boost/test/unit_test_monitor.hpp>
#include <boost/test/unit_test_parameters.hpp>
#include <boost/test/unit_test_suite.hpp>
#include <boost/test/utils/algorithm.hpp>
#include <boost/test/utils/assign_op.hpp>
#include <boost/test/utils/basic_cstring/basic_cstring.hpp>
#include <boost/test/utils/basic_cstring/basic_cstring_fwd.hpp>
#include <boost/test/utils/basic_cstring/bcs_char_traits.hpp>
#include <boost/test/utils/basic_cstring/compare.hpp>
#include <boost/test/utils/basic_cstring/io.hpp>
#include <boost/test/utils/class_properties.hpp>
#include <boost/test/utils/custom_manip.hpp>
#include <boost/test/utils/foreach.hpp>
#include <boost/test/utils/is_cstring.hpp>
#include <boost/test/utils/is_forward_iterable.hpp>
#include <boost/test/utils/iterator/input_iterator_facade.hpp>
#include <boost/test/utils/iterator/token_iterator.hpp>
#include <boost/test/utils/lazy_ostream.hpp>
#include <boost/test/utils/named_params.hpp>
#include <boost/test/utils/nullstream.hpp>
#include <boost/test/utils/rtti.hpp>
#include <boost/test/utils/runtime/argument.hpp>
#include <boost/test/utils/runtime/argument_factory.hpp>
#include <boost/test/utils/runtime/cla/argv_traverser.hpp>
#include <boost/test/utils/runtime/cla/parser.hpp>
#include <boost/test/utils/runtime/env/fetch.hpp>
#include <boost/test/utils/runtime/errors.hpp>
#include <boost/test/utils/runtime/finalize.hpp>
#include <boost/test/utils/runtime/fwd.hpp>
#include <boost/test/utils/runtime/modifier.hpp>
#include <boost/test/utils/runtime/parameter.hpp>
#include <boost/test/utils/setcolor.hpp>
#include <boost/test/utils/string_cast.hpp>
#include <boost/test/utils/timer.hpp>
#include <boost/test/utils/wrap_stringstream.hpp>
#include <boost/test/utils/xml_printer.hpp>

