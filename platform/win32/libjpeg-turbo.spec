
sources:
    ..\..\scripts\libjpeg-turbo\jconfig.h
    ..\..\scripts\libjpeg-turbo\jconfigint.h

also-ignore:
	/simd/arm/
	/simd/mips/
	/simd/mips64/
	/simd/powerpc/
	/simd/rvv/
	/simd/rvv_andes/
	-jni[.]c$
	/java/
	example.c
	
	# temporarily REVERT the 8/12/16 bit depth TurboJPEG feature:
	
	/12-bits/
	/16-bits/
	
	
