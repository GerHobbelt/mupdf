#
# used by the refill-vcxproj.sh tool
#

sources:
	..\..\scripts\QuickJS\quickjs-config.h
	
also-ignore:
	/test262
	/thirdparty/uint128_t
	/examples/
	/sample_app/
	test-closefrom.c

	/get[^/]+[.]c
	/lib[^/]+[.]c
	/[qju][^/]+[.]c
	/bjson.c
	
	