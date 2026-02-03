
// fix ubiquitous warning C5204: 'boost::blah::blah<Blah>': class has virtual functions, but its trivial destructor is not virtual; instances of objects derived from this class may not be destructed correctly
//
// while this generally CAN/MAY be an issue, we brutally assume that the Boost Boys know their shit and NEVER make this mistake, i.e. the classes/structs in question are *implicitly `final`*!
// To play it kinda safe, we apply this 'please shut up' fix to this dummy=see-if-we-can-compile-Ev'ryTing source file only:

#if defined(_MSC_VER)
#pragma warning(disable: 5204)

// fix/shut up warning C4266: 'void boost::blah::blah::blah(...)': no override available for virtual member function from base 'boost::blah::blah'; function is hidden
#pragma warning(disable: 4266)
#endif

#include <boost/msm/active_state_switching_policies.hpp>
#include <boost/msm/back/args.hpp>
#include <boost/msm/back/bind_helpers.hpp>
#include <boost/msm/back/common_types.hpp>
#include <boost/msm/back/copy_policies.hpp>
#include <boost/msm/back/default_compile_policy.hpp>
#include <boost/msm/back/dispatch_table.hpp>
#include <boost/msm/back/favor_compile_time.hpp>
#include <boost/msm/back/fold_to_list.hpp>
#include <boost/msm/back/history_policies.hpp>
#include <boost/msm/back/metafunctions.hpp>
#include <boost/msm/back/mpl_graph_fsm_check.hpp>
#include <boost/msm/back/no_fsm_check.hpp>
#include <boost/msm/back/queue_container_circular.hpp>
#include <boost/msm/back/queue_container_deque.hpp>
#include <boost/msm/back/state_machine.hpp>
#include <boost/msm/back/tools.hpp>
#include <boost/msm/back/traits.hpp>
#include <boost/msm/back11/dispatch_table.hpp>
#include <boost/msm/back11/metafunctions.hpp>
#include <boost/msm/back11/state_machine.hpp>
#include <boost/msm/backmp11/common_types.hpp>
#include <boost/msm/backmp11/event_traits.hpp>
#include <boost/msm/backmp11/favor_compile_time.hpp>
#include <boost/msm/backmp11/state_machine.hpp>
#include <boost/msm/backmp11/state_machine_config.hpp>
#include <boost/msm/common.hpp>
#include <boost/msm/event_traits.hpp>
#include <boost/msm/front/common_states.hpp>
#include <boost/msm/front/completion_event.hpp>
#include <boost/msm/front/euml/algorithm.hpp>
#include <boost/msm/front/euml/common.hpp>
#include <boost/msm/front/euml/container.hpp>
#include <boost/msm/front/euml/euml.hpp>
#include <boost/msm/front/euml/euml_typeof.hpp>
#include <boost/msm/front/euml/guard_grammar.hpp>
#include <boost/msm/front/euml/iteration.hpp>
#include <boost/msm/front/euml/operator.hpp>
#include <boost/msm/front/euml/phoenix_placeholders.hpp>
#include <boost/msm/front/euml/querying.hpp>
#include <boost/msm/front/euml/state_grammar.hpp>
#include <boost/msm/front/euml/stl.hpp>
#include <boost/msm/front/euml/stt_grammar.hpp>
#include <boost/msm/front/euml/transformation.hpp>
#include <boost/msm/front/functor_row.hpp>
#include <boost/msm/front/history_policies.hpp>
#include <boost/msm/front/internal_row.hpp>
#include <boost/msm/front/operator.hpp>
#include <boost/msm/front/puml/puml.hpp>
#include <boost/msm/front/row2.hpp>
#include <boost/msm/front/state_machine_def.hpp>
#include <boost/msm/front/states.hpp>
#include <boost/msm/kleene_event.hpp>
#include <boost/msm/mpl_graph/adjacency_list_graph.hpp>
#include <boost/msm/mpl_graph/breadth_first_search.hpp>
#include <boost/msm/mpl_graph/depth_first_search.hpp>
#include <boost/msm/mpl_graph/incidence_list_graph.hpp>
#include <boost/msm/mpl_graph/mpl_graph.hpp>
#include <boost/msm/mpl_graph/mpl_utils.hpp>
#include <boost/msm/mpl_graph/search_colors.hpp>
#include <boost/msm/msm_grammar.hpp>
#include <boost/msm/proto_config.hpp>
#include <boost/msm/row_tags.hpp>

