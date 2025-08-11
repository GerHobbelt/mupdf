
// fix ubiquitous warning C5204: 'boost::blah::blah<Blah>': class has virtual functions, but its trivial destructor is not virtual; instances of objects derived from this class may not be destructed correctly
//
// while this generally CAN/MAY be an issue, we brutally assume that the Boost Boys know their shit and NEVER make this mistake, i.e. the classes/structs in question are *implicitly `final`*!
// To play it kinda safe, we apply this 'please shut up' fix to this dummy=see-if-we-can-compile-Ev'ryTing source file only:

#if defined(_MSC_VER)
#pragma warning(disable: 5204)

// fix/shut up warning C4266: 'void boost::blah::blah::blah(...)': no override available for virtual member function from base 'boost::blah::blah'; function is hidden
#pragma warning(disable: 4266)
#endif

#include <boost/proto/args.hpp>
#include <boost/proto/context.hpp>
#include <boost/proto/context/callable.hpp>
#include <boost/proto/context/default.hpp>
#include <boost/proto/context/null.hpp>
#include <boost/proto/core.hpp>
#include <boost/proto/debug.hpp>
#include <boost/proto/deep_copy.hpp>
#include <boost/proto/domain.hpp>
#include <boost/proto/eval.hpp>
#include <boost/proto/expr.hpp>
#include <boost/proto/extends.hpp>
#include <boost/proto/functional.hpp>
#include <boost/proto/functional/fusion.hpp>
#include <boost/proto/functional/fusion/at.hpp>
#include <boost/proto/functional/fusion/pop_back.hpp>
#include <boost/proto/functional/fusion/pop_front.hpp>
#include <boost/proto/functional/fusion/push_back.hpp>
#include <boost/proto/functional/fusion/push_front.hpp>
#include <boost/proto/functional/fusion/reverse.hpp>
#include <boost/proto/functional/range.hpp>
#include <boost/proto/functional/range/begin.hpp>
#include <boost/proto/functional/range/empty.hpp>
#include <boost/proto/functional/range/end.hpp>
#include <boost/proto/functional/range/rbegin.hpp>
#include <boost/proto/functional/range/rend.hpp>
#include <boost/proto/functional/range/size.hpp>
#include <boost/proto/functional/std.hpp>
#include <boost/proto/functional/std/iterator.hpp>
#include <boost/proto/functional/std/utility.hpp>
#include <boost/proto/fusion.hpp>
#include <boost/proto/generate.hpp>
#include <boost/proto/literal.hpp>
#include <boost/proto/make_expr.hpp>
#include <boost/proto/matches.hpp>
#include <boost/proto/operators.hpp>
#include <boost/proto/proto.hpp>
#include <boost/proto/proto_fwd.hpp>
#include <boost/proto/proto_typeof.hpp>
#include <boost/proto/repeat.hpp>
#include <boost/proto/tags.hpp>
#include <boost/proto/traits.hpp>
#include <boost/proto/transform.hpp>
#include <boost/proto/transform/arg.hpp>
#include <boost/proto/transform/call.hpp>
#include <boost/proto/transform/default.hpp>
#include <boost/proto/transform/env.hpp>
#include <boost/proto/transform/fold.hpp>
#include <boost/proto/transform/fold_tree.hpp>
#include <boost/proto/transform/impl.hpp>
#include <boost/proto/transform/integral_c.hpp>
#include <boost/proto/transform/lazy.hpp>
#include <boost/proto/transform/make.hpp>
#include <boost/proto/transform/pass_through.hpp>
#include <boost/proto/transform/when.hpp>

