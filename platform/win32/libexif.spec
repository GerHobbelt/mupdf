
only-directories:
	../../scripts/libexif/
	
	# to help force libexif project to point at libexif submodule dir ...
	#
	# ( `exif` is the (demo) application repo that sits alongside `libexif` )

	../../thirdparty/owemdjee/libexif/

also-ignore:
	contrib/aolserver/
	contrib/c[+][+]/

	contrib/examples/cam_features.c
	