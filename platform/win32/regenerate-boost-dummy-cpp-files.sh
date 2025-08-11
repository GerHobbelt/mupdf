#! /bin/bash

for f in *boost*.vcxproj ; do 
	echo $f 
	echo "../../scripts/boost/$( basename $f .vcxproj )-dummy.cpp" 
	if test -f ../../scripts/boost/$( basename $f .vcxproj )-dummy.cpp ; then 
		#cat $f | grep '<ClInclude Include' | sed -E -e 's/^[^"]*"/\/\/ #include "/' -e 's/"[^"]*$/"/'
		cat > ../../scripts/boost/$( basename $f .vcxproj )-dummy.cpp  <<EOF

// fix ubiquitous warning C5204: 'boost::blah::blah<Blah>': class has virtual functions, but its trivial destructor is not virtual; instances of objects derived from this class may not be destructed correctly
//
// while this generally CAN/MAY be an issue, we brutally assume that the Boost Boys know their shit and NEVER make this mistake, i.e. the classes/structs in question are *implicitly \`final\`*!
// To play it kinda safe, we apply this 'please shut up' fix to this dummy=see-if-we-can-compile-Ev'ryTing source file only:

#if defined(_MSC_VER)
#pragma warning(disable: 5204)

// fix/shut up warning C4266: 'void boost::blah::blah::blah(...)': no override available for virtual member function from base 'boost::blah::blah'; function is hidden
#pragma warning(disable: 4266)
#endif

EOF
		
		cat $f | grep '<ClInclude Include' | sed -E -e 's/^[^"]*"//' -e 's/"[^"]*$//' -e 's/^.*\/include\/boost\///' -e 's/^/#include <boost\//' -e 's/$/>/'   \
			| sed -E -f regenerate-boost-dummy-cpp-files.sed                                                                                                   \
			>> ../../scripts/boost/$( basename $f .vcxproj )-dummy.cpp 
			
		echo "" >> ../../scripts/boost/$( basename $f .vcxproj )-dummy.cpp
	else 
		echo '---' 
	fi 
done
