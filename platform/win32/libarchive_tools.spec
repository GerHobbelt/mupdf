
sources:
	../../scripts/libarchive/libarchive_config.h
	
also-ignore:
	/build/
	test/
	test_utils/
	/libarchive/[^/]+[.]c
	
	# and the shar code is giving us grief, so we ditch it:
	/contrib/shar/
	