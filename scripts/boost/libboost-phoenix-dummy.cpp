
// fix ubiquitous warning C5204: 'boost::blah::blah<Blah>': class has virtual functions, but its trivial destructor is not virtual; instances of objects derived from this class may not be destructed correctly
//
// while this generally CAN/MAY be an issue, we brutally assume that the Boost Boys know their shit and NEVER make this mistake, i.e. the classes/structs in question are *implicitly `final`*!
// To play it kinda safe, we apply this 'please shut up' fix to this dummy=see-if-we-can-compile-Ev'ryTing source file only:

#if defined(_MSC_VER)
#pragma warning(disable: 5204)

// fix/shut up warning C4266: 'void boost::blah::blah::blah(...)': no override available for virtual member function from base 'boost::blah::blah'; function is hidden
#pragma warning(disable: 4266)
#endif

#include <boost/phoenix.hpp>
#include <boost/phoenix/bind.hpp>
#include <boost/phoenix/bind/bind_function.hpp>
#include <boost/phoenix/bind/bind_function_object.hpp>
#include <boost/phoenix/bind/bind_member_function.hpp>
#include <boost/phoenix/bind/bind_member_variable.hpp>
#include <boost/phoenix/config.hpp>
#include <boost/phoenix/core.hpp>
#include <boost/phoenix/core/actor.hpp>
#include <boost/phoenix/core/argument.hpp>
#include <boost/phoenix/core/arity.hpp>
#include <boost/phoenix/core/as_actor.hpp>
#include <boost/phoenix/core/call.hpp>
#include <boost/phoenix/core/debug.hpp>
#include <boost/phoenix/core/domain.hpp>
#include <boost/phoenix/core/environment.hpp>
#include <boost/phoenix/core/expression.hpp>
#include <boost/phoenix/core/function_equal.hpp>
#include <boost/phoenix/core/is_actor.hpp>
#include <boost/phoenix/core/is_nullary.hpp>
#include <boost/phoenix/core/is_value.hpp>
#include <boost/phoenix/core/limits.hpp>
#include <boost/phoenix/core/meta_grammar.hpp>
#include <boost/phoenix/core/nothing.hpp>
#include <boost/phoenix/core/reference.hpp>
#include <boost/phoenix/core/terminal.hpp>
#include <boost/phoenix/core/terminal_fwd.hpp>
#include <boost/phoenix/core/v2_eval.hpp>
#include <boost/phoenix/core/value.hpp>
#include <boost/phoenix/debug.hpp>
#include <boost/phoenix/function.hpp>
#include <boost/phoenix/function/adapt_callable.hpp>
#include <boost/phoenix/function/adapt_function.hpp>
#include <boost/phoenix/function/function.hpp>
#include <boost/phoenix/function/lazy_list.hpp>
#include <boost/phoenix/function/lazy_operator.hpp>
#include <boost/phoenix/function/lazy_prelude.hpp>
#include <boost/phoenix/function/lazy_reuse.hpp>
#include <boost/phoenix/function/lazy_signature.hpp>
#include <boost/phoenix/function/lazy_smart.hpp>
#include <boost/phoenix/fusion.hpp>
#include <boost/phoenix/fusion/at.hpp>
#include <boost/phoenix/object.hpp>
#include <boost/phoenix/object/const_cast.hpp>
#include <boost/phoenix/object/construct.hpp>
#include <boost/phoenix/object/delete.hpp>
#include <boost/phoenix/object/dynamic_cast.hpp>
#include <boost/phoenix/object/new.hpp>
#include <boost/phoenix/object/reinterpret_cast.hpp>
#include <boost/phoenix/object/static_cast.hpp>
#include <boost/phoenix/operator.hpp>
#include <boost/phoenix/operator/arithmetic.hpp>
#include <boost/phoenix/operator/bitwise.hpp>
#include <boost/phoenix/operator/comparison.hpp>
#include <boost/phoenix/operator/if_else.hpp>
#include <boost/phoenix/operator/io.hpp>
#include <boost/phoenix/operator/logical.hpp>
#include <boost/phoenix/operator/member.hpp>
#include <boost/phoenix/operator/self.hpp>
#include <boost/phoenix/phoenix.hpp>
#include <boost/phoenix/scope.hpp>
#include <boost/phoenix/scope/dynamic.hpp>
#include <boost/phoenix/scope/lambda.hpp>
#include <boost/phoenix/scope/let.hpp>
#include <boost/phoenix/scope/local_variable.hpp>
#include <boost/phoenix/scope/scoped_environment.hpp>
#include <boost/phoenix/scope/this.hpp>
#include <boost/phoenix/statement.hpp>
#include <boost/phoenix/statement/do_while.hpp>
#include <boost/phoenix/statement/for.hpp>
#include <boost/phoenix/statement/if.hpp>
#include <boost/phoenix/statement/sequence.hpp>
#include <boost/phoenix/statement/switch.hpp>
#include <boost/phoenix/statement/throw.hpp>
#include <boost/phoenix/statement/try_catch.hpp>
#include <boost/phoenix/statement/while.hpp>
#include <boost/phoenix/stl.hpp>
#include <boost/phoenix/stl/algorithm.hpp>
#include <boost/phoenix/stl/algorithm/iteration.hpp>
#include <boost/phoenix/stl/algorithm/querying.hpp>
#include <boost/phoenix/stl/algorithm/transformation.hpp>
#include <boost/phoenix/stl/cmath.hpp>
#include <boost/phoenix/stl/container.hpp>
#include <boost/phoenix/stl/container/container.hpp>
#include <boost/phoenix/stl/tuple.hpp>

