
also-ignore:
	.*test.*cc
	
	# does not contain 'benchmark', as per https://stackoverflow.com/questions/406230/regular-expression-to-match-a-line-that-doesnt-contain-a-word
	^((?!benchmark).)*[.]cc$