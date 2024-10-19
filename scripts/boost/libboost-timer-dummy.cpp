
#if 01

#include <boost/timer/timer.hpp>

#else

#define BOOST_TIMER_ENABLE_DEPRECATED
#define BOOST_ALLOW_DEPRECATED_HEADERS

#include <boost/timer.hpp>
#include <boost/progress.hpp>

#endif

