#
# used by the refill-vcxproj.sh tool
#

sources:
	..\..\scripts\QuickJS\quickjs-config.h
	
also-ignore:
	/test262
	/thirdparty/uint128_t
	/tests/
	/fuzz/
	test-closefrom.c
	
	^/[^u][\w\d-]+[.]c
	