
sources:
    ../../thirdparty/owemdjee/Catch2/tests/ExtraTests/X91-AmalgamatedCatch.cpp	
    ../../scripts/catch2/amalgamated-test-runner.cpp

also-ignore:
	/src/
	/extras/

    # really the only one we want to accept is this one:
	#
    #   ./tests/ExtraTests/X91-AmalgamatedCatch.cpp	
	#/tests/
	/examples/
	/tests/.*/[^amlgtd]
