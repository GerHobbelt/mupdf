# ditch any include paths which include `../` path particles:

/[.][.][/]/                d

# check for various path particles and when they occur, ditch the line as a whole.

/\/detail\//               d
/\/impl\//                 d
/\/support\//              d
/\/repository\//           d
/\/experimental\//         d
/\/pending\//              d
/\/preprocessed\//         d
/\/aux_\//                 d
/\/home\//                 d
/\/predef\/[^\/]*\//       d

# ... and now for the specials we don't want to occur in the generated boost-*-dummy.cpp files either!

/\/platform\//             d
/\/posix\//                d
/\/bsd\//                  d
/\/linux\//                d
/\/windows\//              d
/\/win32\//                d

# /<boost\/spirit\/include\//                d

# boost\spirit\include\phoenix.hpp: message : This header is deprecated. Use <boost/phoenix.hpp> instead.
# boost\spirit\include\phoenix_algorithm.hpp: message : This header is deprecated. Use <boost/phoenix/stl/algorithm.hpp> instead.
# ...

/<boost\/spirit\/include\/phoenix/         d

# Warning: This header is deprecated. Please use: boost/spirit/include/classic.hpp

/<boost\/spirit[.]hpp>/                    d

# boost\spirit\include\version.hpp(17,9): warning C4005: 'SPIRIT_VERSION': macro redefinition         (v20xx)
# boost\spirit\home\classic\version.hpp(16,9): message : see previous definition of 'SPIRIT_VERSION'  (v18xx)
# ...

/version[.]hpp/            d

# BOOST_PRAGMA_MESSAGE(
#  "The practice of declaring the Bind placeholders (_1, _2, ...) "
#  "in the global namespace is deprecated. Please use "
#  "<boost/bind/bind.hpp> + using namespace boost::placeholders, "
#  "or define BOOST_BIND_GLOBAL_PLACEHOLDERS to retain the current behavior."
# )

/<boost\/bind[.]hpp>/      d


