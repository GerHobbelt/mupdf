
// fix ubiquitous warning C5204: 'boost::blah::blah<Blah>': class has virtual functions, but its trivial destructor is not virtual; instances of objects derived from this class may not be destructed correctly
//
// while this generally CAN/MAY be an issue, we brutally assume that the Boost Boys know their shit and NEVER make this mistake, i.e. the classes/structs in question are *implicitly `final`*!
// To play it kinda safe, we apply this 'please shut up' fix to this dummy=see-if-we-can-compile-Ev'ryTing source file only:

#if defined(_MSC_VER)
#pragma warning(disable: 5204)

// fix/shut up warning C4266: 'void boost::blah::blah::blah(...)': no override available for virtual member function from base 'boost::blah::blah'; function is hidden
#pragma warning(disable: 4266)
#endif

#include <boost/process.hpp>
#include <boost/process/bind_launcher.hpp>
#include <boost/process/cstring_ref.hpp>
#include <boost/process/default_launcher.hpp>
#include <boost/process/environment.hpp>
#include <boost/process/error.hpp>
#include <boost/process/execute.hpp>
#include <boost/process/exit_code.hpp>
#include <boost/process/ext.hpp>
#include <boost/process/ext/cmd.hpp>
#include <boost/process/ext/cwd.hpp>
#include <boost/process/ext/env.hpp>
#include <boost/process/ext/exe.hpp>
#include <boost/process/pid.hpp>
#include <boost/process/popen.hpp>
#include <boost/process/process.hpp>
#include <boost/process/process_handle.hpp>
#include <boost/process/shell.hpp>
#include <boost/process/start_dir.hpp>
#include <boost/process/stdio.hpp>
#include <boost/process/v1.hpp>
#include <boost/process/v1/args.hpp>
#include <boost/process/v1/async.hpp>
#include <boost/process/v1/async_pipe.hpp>
#include <boost/process/v1/async_system.hpp>
#include <boost/process/v1/child.hpp>
#include <boost/process/v1/cmd.hpp>
#include <boost/process/v1/env.hpp>
#include <boost/process/v1/environment.hpp>
#include <boost/process/v1/error.hpp>
#include <boost/process/v1/exception.hpp>
#include <boost/process/v1/exe.hpp>
#include <boost/process/v1/extend.hpp>
#include <boost/process/v1/filesystem.hpp>
#include <boost/process/v1/group.hpp>
#include <boost/process/v1/handles.hpp>
#include <boost/process/v1/io.hpp>
#include <boost/process/v1/locale.hpp>
#include <boost/process/v1/pipe.hpp>
#include <boost/process/v1/posix.hpp>
#include <boost/process/v1/search_path.hpp>
#include <boost/process/v1/shell.hpp>
#include <boost/process/v1/spawn.hpp>
#include <boost/process/v1/start_dir.hpp>
#include <boost/process/v1/system.hpp>
#include <boost/process/v1/windows.hpp>
#include <boost/process/v2/bind_launcher.hpp>
#include <boost/process/v2/cstring_ref.hpp>
#include <boost/process/v2/default_launcher.hpp>
#include <boost/process/v2/environment.hpp>
#include <boost/process/v2/error.hpp>
#include <boost/process/v2/execute.hpp>
#include <boost/process/v2/exit_code.hpp>
#include <boost/process/v2/ext.hpp>
#include <boost/process/v2/ext/cmd.hpp>
#include <boost/process/v2/ext/cwd.hpp>
#include <boost/process/v2/ext/env.hpp>
#include <boost/process/v2/ext/exe.hpp>
#include <boost/process/v2/pid.hpp>
#include <boost/process/v2/popen.hpp>
#include <boost/process/v2/process.hpp>
#include <boost/process/v2/process_handle.hpp>
#include <boost/process/v2/shell.hpp>
#include <boost/process/v2/start_dir.hpp>
#include <boost/process/v2/stdio.hpp>

