
also-ignore:
	/app/
	/python/
	monolithic_main.c
	regexp_benchmark.cc
	testinstall.cc
	/util/

	# the library sources themselves should be ignored, but NOT the /re2/testing/*.* test sources!
	/re2/(?!testing).*
			