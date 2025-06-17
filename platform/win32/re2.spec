
also-ignore:
	/app/
	/python/

	# the library sources themselves should be ignored, but keep: regexp_benchmark.cc !
	/re2/(?!.*?benchmark|.*?backtrack|.*?walker|.*?dump).*
	