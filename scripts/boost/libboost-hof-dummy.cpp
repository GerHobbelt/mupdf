
// fix ubiquitous warning C5204: 'boost::blah::blah<Blah>': class has virtual functions, but its trivial destructor is not virtual; instances of objects derived from this class may not be destructed correctly
//
// while this generally CAN/MAY be an issue, we brutally assume that the Boost Boys know their shit and NEVER make this mistake, i.e. the classes/structs in question are *implicitly `final`*!
// To play it kinda safe, we apply this 'please shut up' fix to this dummy=see-if-we-can-compile-Ev'ryTing source file only:

#if defined(_MSC_VER)
#pragma warning(disable: 5204)

// fix/shut up warning C4266: 'void boost::blah::blah::blah(...)': no override available for virtual member function from base 'boost::blah::blah'; function is hidden
#pragma warning(disable: 4266)
#endif

#include <boost/hof.hpp>
#include <boost/hof/alias.hpp>
#include <boost/hof/always.hpp>
#include <boost/hof/apply.hpp>
#include <boost/hof/apply_eval.hpp>
#include <boost/hof/arg.hpp>
#include <boost/hof/capture.hpp>
#include <boost/hof/combine.hpp>
#include <boost/hof/compose.hpp>
#include <boost/hof/config.hpp>
#include <boost/hof/construct.hpp>
#include <boost/hof/decay.hpp>
#include <boost/hof/decorate.hpp>
#include <boost/hof/eval.hpp>
#include <boost/hof/first_of.hpp>
#include <boost/hof/fix.hpp>
#include <boost/hof/flip.hpp>
#include <boost/hof/flow.hpp>
#include <boost/hof/fold.hpp>
#include <boost/hof/function.hpp>
#include <boost/hof/function_param_limit.hpp>
#include <boost/hof/identity.hpp>
#include <boost/hof/if.hpp>
#include <boost/hof/implicit.hpp>
#include <boost/hof/indirect.hpp>
#include <boost/hof/infix.hpp>
#include <boost/hof/is_invocable.hpp>
#include <boost/hof/is_unpackable.hpp>
#include <boost/hof/lambda.hpp>
#include <boost/hof/lazy.hpp>
#include <boost/hof/lift.hpp>
#include <boost/hof/limit.hpp>
#include <boost/hof/match.hpp>
#include <boost/hof/mutable.hpp>
#include <boost/hof/pack.hpp>
#include <boost/hof/partial.hpp>
#include <boost/hof/pipable.hpp>
#include <boost/hof/placeholders.hpp>
#include <boost/hof/proj.hpp>
#include <boost/hof/protect.hpp>
#include <boost/hof/repeat.hpp>
#include <boost/hof/repeat_while.hpp>
#include <boost/hof/result.hpp>
#include <boost/hof/returns.hpp>
#include <boost/hof/reveal.hpp>
#include <boost/hof/reverse_fold.hpp>
#include <boost/hof/rotate.hpp>
#include <boost/hof/static.hpp>
#include <boost/hof/tap.hpp>
#include <boost/hof/unpack.hpp>
#include <boost/hof/unpack_sequence.hpp>

