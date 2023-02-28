#
# NOTE: copy command does not accept / as a directory separator.
#
# NOTE: BINDIR is specified on the command line (see MSVC project file)
#

TARGETS=                            \
		 $(BINDIR)\testdll.dll		\
		 $(BINDIR)\testdll2.dll		\
		 $(BINDIR)\testdll3.dll

all : $(TARGETS)
	echo "Target: all --> done"

rebuild : clean all
	echo "Target: rebuild --> done"

clean :
	echo "Target: clean --> done"
	-del $(TARGETS)

$(BINDIR)\testdll.dll : $(BINDIR)\libdlfcn-win32-testdll1.dll
	copy /Y $** $@

$(BINDIR)\testdll2.dll : $(BINDIR)\libdlfcn-win32-testdll2.dll
	copy /Y $** $@

$(BINDIR)\testdll3.dll : $(BINDIR)\libdlfcn-win32-testdll3.dll
	copy /Y $** $@


.PHONY : all clean rebuild
#
# https://stackoverflow.com/questions/4509033/nmake-phony-analogue
# but MSVC2022's nmake keeps yakking about fatal error U1073: https://learn.microsoft.com/en-us/cpp/error-messages/tool-errors/nmake-fatal-error-u1073?view=msvc-170
#
# meanwhile:
# https://learn.microsoft.com/en-us/cpp/build/reference/description-blocks?view=msvc-170#pseudotargets
# and it *looks* like putting a space before the colons at least removed the /fatal/ but the report remains: "clean target does not exist"
# Alas.
#

