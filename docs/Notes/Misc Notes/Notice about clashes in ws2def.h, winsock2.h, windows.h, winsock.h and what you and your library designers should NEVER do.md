*sigh* ... it's been a while since I saw this last; not because because everybody has smartened up but because I was on a hiatus of C/C++ coding...

# Notice about clashes in `ws2def.h`, `winsock2.h`, `windows.h`, `winsock.h` and what you and your library designers should NEVER do

Here's the rant straight from today's git commit that fixes this type of crap, **once again**:

----

... the patch below fixes some extremely obnoxious and hard-to-diagnose errors (this time, we dug them out using the /P render-to-preprocessed-file compiler mode plus a few grep incantations to find the starting point of this crap, which recurs again and again because all eedjits follow MS advice about `LEAN_AND_MEAN` and let userland cope with the mess; ditto for `#include <windows.h>` in your library header file and NOT loading `winsock2.h` (duh!), but then userland code loads the latter one after as, hey, it's all part of some bigger scheme now!  !@#\$%^&*(*&^%@#\$#!!!

```
#if 0	// [GHo: WIN32_LEAN_AND_MEAN is cute, but no cigar: when header files such as these do this and are themselves loaded in sourcefiles of larger applications/userland code, things can break in very hard-to-diagnose ways! So... DON'T!

#define WIN32_LEAN_AND_MEAN

#endif

// ...
// ...
// ...

// ALWAYS include the next header file FIRST when you happen to include windows.h:

#include <winsock2.h>   // [GHo] fixes issues elsewhere, e.g. spdlog, when this header file is loaded before another, which loads winsock2.h or (*shudder*) winsock.h, e.g. windows.h: you'll get clashes in preprocessor defines in ws2def.h vs. winsock2.h  :-((
#include <ws2tcpip.h>   // [GHo] optional; if you want, grab it. This one doesn't cause the trouble windows.h & winsock2.h/ws2def.h do, as a pair.  >-((
#include <windows.h>

```

----

... and that's it for today's message. Sometimes... :facepalm:


## P.S.

This is the stuff that's part of the compiler error noises when the above situation happens to you:

```
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\shared\ws2def.h(103,9): warning C4005: 'AF_IPX': macro redefinition (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock.h(457,9): message : see previous definition of 'AF_IPX' (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\shared\ws2def.h(147,9): warning C4005: 'AF_MAX': macro redefinition (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock.h(476,9): message : see previous definition of 'AF_MAX' (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\shared\ws2def.h(187,9): warning C4005: 'SO_DONTLINGER': macro redefinition (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock.h(399,9): message : see previous definition of 'SO_DONTLINGER' (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\shared\ws2def.h(246,16): error C2011: 'sockaddr': 'struct' type redefinition (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock.h(482,8): message : see declaration of 'sockaddr' (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\shared\ws2def.h(448,5): error C2143: syntax error: missing '}' before 'constant' (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\shared\ws2def.h(448,5): error C2059: syntax error: 'constant' (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\shared\ws2def.h(502,1): error C2143: syntax error: missing ';' before '}' (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\shared\ws2def.h(502,3): error C4430: missing type specifier - int assumed. Note: C++ does not support default-int (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\shared\ws2def.h(502,13): error C4430: missing type specifier - int assumed. Note: C++ does not support default-int (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\shared\ws2def.h(583,9): warning C4005: 'IN_CLASSA': macro redefinition (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock.h(284,9): message : see previous definition of 'IN_CLASSA' (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\shared\ws2def.h(589,9): warning C4005: 'IN_CLASSB': macro redefinition (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock.h(290,9): message : see previous definition of 'IN_CLASSB' (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\shared\ws2def.h(595,9): warning C4005: 'IN_CLASSC': macro redefinition (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock.h(296,9): message : see previous definition of 'IN_CLASSC' (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\shared\ws2def.h(606,9): warning C4005: 'INADDR_ANY': macro redefinition (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock.h(301,9): message : see previous definition of 'INADDR_ANY' (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\shared\ws2def.h(608,9): warning C4005: 'INADDR_BROADCAST': macro redefinition (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock.h(303,9): message : see previous definition of 'INADDR_BROADCAST' (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\shared\ws2def.h(644,16): error C2011: 'sockaddr_in': 'struct' type redefinition (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock.h(309,8): message : see declaration of 'sockaddr_in' (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock2.h(136,16): error C2011: 'fd_set': 'struct' type redefinition (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock.h(65,16): message : see declaration of 'fd_set' (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock2.h(158,9): warning C4005: 'FD_SET': macro redefinition (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock.h(96,9): message : see previous definition of 'FD_SET' (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock2.h(180,8): error C2011: 'timeval': 'struct' type redefinition (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock.h(108,8): message : see declaration of 'timeval' (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock2.h(236,9): error C2011: 'hostent': 'struct' type redefinition (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock.h(164,9): message : see declaration of 'hostent' (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock2.h(249,9): error C2011: 'netent': 'struct' type redefinition (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock.h(177,9): message : see declaration of 'netent' (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock2.h(256,9): error C2011: 'servent': 'struct' type redefinition (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock.h(184,9): message : see declaration of 'servent' (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock2.h(268,9): error C2011: 'protoent': 'struct' type redefinition (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock.h(196,9): message : see declaration of 'protoent' (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock2.h(364,16): error C2011: 'WSAData': 'struct' type redefinition (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock.h(319,16): message : see declaration of 'WSAData' (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock2.h(462,8): error C2011: 'sockproto': 'struct' type redefinition (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock.h(491,8): message : see declaration of 'sockproto' (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock2.h(504,9): error C2011: 'linger': 'struct' type redefinition (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock.h(528,9): message : see declaration of 'linger' (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock2.h(517,9): warning C4005: 'SOMAXCONN': macro redefinition (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock.h(541,9): message : see previous definition of 'SOMAXCONN' (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock2.h(551,9): warning C4005: 'FD_READ': macro redefinition (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock.h(559,9): message : see previous definition of 'FD_READ' (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock2.h(554,9): warning C4005: 'FD_WRITE': macro redefinition (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock.h(560,9): message : see previous definition of 'FD_WRITE' (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock2.h(557,9): warning C4005: 'FD_OOB': macro redefinition (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock.h(561,9): message : see previous definition of 'FD_OOB' (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock2.h(560,9): warning C4005: 'FD_ACCEPT': macro redefinition (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock.h(562,9): message : see previous definition of 'FD_ACCEPT' (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock2.h(563,9): warning C4005: 'FD_CONNECT': macro redefinition (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock.h(563,9): message : see previous definition of 'FD_CONNECT' (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock2.h(566,9): warning C4005: 'FD_CLOSE': macro redefinition (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock.h(564,9): message : see previous definition of 'FD_CLOSE' (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock2.h(1633,1): error C2375: 'accept': redefinition; different linkage (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock.h(739,19): message : see declaration of 'accept' (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock2.h(1655,1): error C2375: 'bind': redefinition; different linkage (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock.h(744,16): message : see declaration of 'bind' (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock2.h(1676,1): error C2375: 'closesocket': redefinition; different linkage (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock.h(749,16): message : see declaration of 'closesocket' (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock2.h(1693,1): error C2375: 'connect': redefinition; different linkage (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock.h(751,16): message : see declaration of 'connect' (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock2.h(1714,1): error C2375: 'ioctlsocket': redefinition; different linkage (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock.h(756,16): message : see declaration of 'ioctlsocket' (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock2.h(1737,1): error C2375: 'getpeername': redefinition; different linkage (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock.h(761,16): message : see declaration of 'getpeername' (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock2.h(1758,1): error C2375: 'getsockname': redefinition; different linkage (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock.h(766,16): message : see declaration of 'getsockname' (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock2.h(1779,1): error C2375: 'getsockopt': redefinition; different linkage (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock.h(771,16): message : see declaration of 'getsockopt' (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock2.h(1804,1): error C2375: 'htonl': redefinition; different linkage (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock.h(778,19): message : see declaration of 'htonl' (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock2.h(1821,1): error C2375: 'htons': redefinition; different linkage (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock.h(780,20): message : see declaration of 'htons' (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock2.h(1839,1): error C2375: 'inet_addr': redefinition; different linkage (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock.h(782,26): message : see declaration of 'inet_addr' (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock2.h(1857,1): error C2375: 'inet_ntoa': redefinition; different linkage (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock.h(784,23): message : see declaration of 'inet_ntoa' (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock2.h(1957,1): error C2375: 'listen': redefinition; different linkage (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock.h(786,16): message : see declaration of 'listen' (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock2.h(1976,1): error C2375: 'ntohl': redefinition; different linkage (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock.h(790,19): message : see declaration of 'ntohl' (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock2.h(1993,1): error C2375: 'ntohs': redefinition; different linkage (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock.h(792,20): message : see declaration of 'ntohs' (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock2.h(2010,1): error C2375: 'recv': redefinition; different linkage (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock.h(794,16): message : see declaration of 'recv' (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock2.h(2033,1): error C2375: 'recvfrom': redefinition; different linkage (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock.h(800,16): message : see declaration of 'recvfrom' (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock2.h(2060,1): error C2375: 'select': redefinition; different linkage (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock.h(808,16): message : see declaration of 'select' (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock2.h(2085,1): error C2375: 'send': redefinition; different linkage (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock.h(815,16): message : see declaration of 'send' (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock2.h(2108,1): error C2375: 'sendto': redefinition; different linkage (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock.h(821,16): message : see declaration of 'sendto' (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock2.h(2135,1): error C2375: 'setsockopt': redefinition; different linkage (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock.h(829,16): message : see declaration of 'setsockopt' (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock2.h(2160,1): error C2375: 'shutdown': redefinition; different linkage (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock.h(836,16): message : see declaration of 'shutdown' (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock2.h(2180,1): error C2375: 'socket': redefinition; different linkage (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock.h(840,19): message : see declaration of 'socket' (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock2.h(2205,1): error C2375: 'gethostbyaddr': redefinition; different linkage (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock.h(847,33): message : see declaration of 'gethostbyaddr' (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock2.h(2227,1): error C2375: 'gethostbyname': redefinition; different linkage (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock.h(852,33): message : see declaration of 'gethostbyname' (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock2.h(2244,1): error C2375: 'gethostname': redefinition; different linkage (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock.h(854,16): message : see declaration of 'gethostname' (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock2.h(2284,1): error C2375: 'getservbyport': redefinition; different linkage (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock.h(858,33): message : see declaration of 'getservbyport' (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock2.h(2303,1): error C2375: 'getservbyname': redefinition; different linkage (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock.h(862,33): message : see declaration of 'getservbyname' (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock2.h(2322,1): error C2375: 'getprotobynumber': redefinition; different linkage (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock.h(866,34): message : see declaration of 'getprotobynumber' (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock2.h(2339,1): error C2375: 'getprotobyname': redefinition; different linkage (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock.h(868,34): message : see declaration of 'getprotobyname' (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock2.h(2359,1): error C2375: 'WSAStartup': redefinition; different linkage (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock.h(872,16): message : see declaration of 'WSAStartup' (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock2.h(2379,1): error C2375: 'WSACleanup': redefinition; different linkage (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock.h(876,16): message : see declaration of 'WSACleanup' (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock2.h(2396,1): error C2375: 'WSASetLastError': redefinition; different linkage (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock.h(878,17): message : see declaration of 'WSASetLastError' (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock2.h(2413,1): error C2375: 'WSAGetLastError': redefinition; different linkage (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock.h(880,16): message : see declaration of 'WSAGetLastError' (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock2.h(2434,1): error C2375: 'WSAIsBlocking': redefinition; different linkage (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock.h(882,17): message : see declaration of 'WSAIsBlocking' (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock2.h(2452,1): error C2375: 'WSAUnhookBlockingHook': redefinition; different linkage (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock.h(884,16): message : see declaration of 'WSAUnhookBlockingHook' (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock2.h(2470,1): error C2375: 'WSASetBlockingHook': redefinition; different linkage (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock.h(886,20): message : see declaration of 'WSASetBlockingHook' (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock2.h(2488,1): error C2375: 'WSACancelBlockingCall': redefinition; different linkage (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock.h(888,16): message : see declaration of 'WSACancelBlockingCall' (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock2.h(2506,1): error C2375: 'WSAAsyncGetServByName': redefinition; different linkage (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock.h(890,19): message : see declaration of 'WSAAsyncGetServByName' (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock2.h(2534,1): error C2375: 'WSAAsyncGetServByPort': redefinition; different linkage (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock.h(898,19): message : see declaration of 'WSAAsyncGetServByPort' (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock2.h(2562,1): error C2375: 'WSAAsyncGetProtoByName': redefinition; different linkage (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock.h(906,19): message : see declaration of 'WSAAsyncGetProtoByName' (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock2.h(2588,1): error C2375: 'WSAAsyncGetProtoByNumber': redefinition; different linkage (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock.h(913,19): message : see declaration of 'WSAAsyncGetProtoByNumber' (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock2.h(2614,1): error C2375: 'WSAAsyncGetHostByName': redefinition; different linkage (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock.h(920,19): message : see declaration of 'WSAAsyncGetHostByName' (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock2.h(2640,1): error C2375: 'WSAAsyncGetHostByAddr': redefinition; different linkage (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock.h(927,19): message : see declaration of 'WSAAsyncGetHostByAddr' (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock2.h(2670,1): error C2375: 'WSACancelAsyncRequest': redefinition; different linkage (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock.h(936,16): message : see declaration of 'WSACancelAsyncRequest' (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock2.h(2688,1): error C2375: 'WSAAsyncSelect': redefinition; different linkage (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock.h(938,16): message : see declaration of 'WSAAsyncSelect' (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock2.h(4320,1): error C2059: syntax error: '}' (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\winsock2.h(4320,1): error C2143: syntax error: missing ';' before '}' (compiling source file ../../thirdparty/owemdjee/fmtlog/bench/bench.cc)
```



