
extra-sources:
	../../scripts/libiconv/config.h
	
also-ignore:
	.*[.]filters
	.*[.]vcxproj
	.*[.]dll
	gnulib-local/
	relocatable-stub.c
	
	lib/iconv.c
	
	src/monolithic_main.c
	
    libcharset/tools/locale_codeset.c
    libcharset/tools/locale_monthnames.c
    libcharset/tools/locale_x11encoding.c
	
	/woe32dll/

	srclib/[a-z0-9_-]*[.]c

	/tools/
	
	/srclib/malloc/.*[.]c
	
	[a-z0-9_-]*[.]rc
	
	.*[.]tlog
	_props/
	