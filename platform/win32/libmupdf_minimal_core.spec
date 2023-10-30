#
# used by the refill-vcxproj.sh tool
#

directories:
	../../include
	../../source
	
ignore:
	/helpers/base58/base58X.c
	/helpers/base58/base58X_test.c
	/helpers/mu-office-lib/mu-office-lib.c
	/helpers/base58/base58X.h
	/tools/
	/tests/
	/system_override/
	/include/fake/
		
	#pdf/
	#html/
	#xps/
	#cbz/
	#svg/
	
	# the quickest way to mark this next item as a direct regex is using set brackets: []
	#tesserac[t]
	#oc[r]
	#output[-]
	#load[-]
	#stext[-]
	#color[-]
	#draw[-]
	#filter[-]
	#noto.c
	#reflow/
	#fon[t]
	#pixmap.c
	#image.c
	#colorspace.c
	#separation.c
	