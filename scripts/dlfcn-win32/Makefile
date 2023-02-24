#
# NOTE: copy command does not accept / as a directory separator.
#

all:								\
		 $(BINDIR)\testdll.dll		\
		 $(BINDIR)\testdll2.dll		\
		 $(BINDIR)\testdll3.dll

$(BINDIR)\testdll.dll: $(BINDIR)\libdlfcn-win32-testdll1.dll
	copy /Y $** $@
	
$(BINDIR)\testdll2.dll: $(BINDIR)\libdlfcn-win32-testdll2.dll
	copy /Y $** $@

$(BINDIR)\testdll3.dll: $(BINDIR)\libdlfcn-win32-testdll3.dll
	copy /Y $** $@


.PHONY: all clean
