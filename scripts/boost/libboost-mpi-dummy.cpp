
// fix ubiquitous warning C5204: 'boost::blah::blah<Blah>': class has virtual functions, but its trivial destructor is not virtual; instances of objects derived from this class may not be destructed correctly
//
// while this generally CAN/MAY be an issue, we brutally assume that the Boost Boys know their shit and NEVER make this mistake, i.e. the classes/structs in question are *implicitly `final`*!
// To play it kinda safe, we apply this 'please shut up' fix to this dummy=see-if-we-can-compile-Ev'ryTing source file only:

#if defined(_MSC_VER)
#pragma warning(disable: 5204)

// fix/shut up warning C4266: 'void boost::blah::blah::blah(...)': no override available for virtual member function from base 'boost::blah::blah'; function is hidden
#pragma warning(disable: 4266)
#endif

#include <boost/mpi.hpp>
#include <boost/mpi/allocator.hpp>
#include <boost/mpi/cartesian_communicator.hpp>
#include <boost/mpi/collectives.hpp>
#include <boost/mpi/collectives/all_gather.hpp>
#include <boost/mpi/collectives/all_gatherv.hpp>
#include <boost/mpi/collectives/all_reduce.hpp>
#include <boost/mpi/collectives/all_to_all.hpp>
#include <boost/mpi/collectives/broadcast.hpp>
#include <boost/mpi/collectives/gather.hpp>
#include <boost/mpi/collectives/gatherv.hpp>
#include <boost/mpi/collectives/reduce.hpp>
#include <boost/mpi/collectives/scan.hpp>
#include <boost/mpi/collectives/scatter.hpp>
#include <boost/mpi/collectives/scatterv.hpp>
#include <boost/mpi/collectives_fwd.hpp>
#include <boost/mpi/communicator.hpp>
#include <boost/mpi/config.hpp>
#include <boost/mpi/datatype.hpp>
#include <boost/mpi/datatype_fwd.hpp>
#include <boost/mpi/environment.hpp>
#include <boost/mpi/error_string.hpp>
#include <boost/mpi/exception.hpp>
#include <boost/mpi/graph_communicator.hpp>
#include <boost/mpi/group.hpp>
#include <boost/mpi/inplace.hpp>
#include <boost/mpi/intercommunicator.hpp>
#include <boost/mpi/nonblocking.hpp>
#include <boost/mpi/operations.hpp>
#include <boost/mpi/packed_iarchive.hpp>
#include <boost/mpi/packed_oarchive.hpp>
#include <boost/mpi/python.hpp>
#include <boost/mpi/python/config.hpp>
#include <boost/mpi/python/serialize.hpp>
#include <boost/mpi/python/skeleton_and_content.hpp>
#include <boost/mpi/request.hpp>
#include <boost/mpi/skeleton_and_content.hpp>
#include <boost/mpi/skeleton_and_content_fwd.hpp>
#include <boost/mpi/skeleton_and_content_types.hpp>
#include <boost/mpi/status.hpp>
#include <boost/mpi/timer.hpp>

