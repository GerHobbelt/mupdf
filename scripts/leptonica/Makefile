#
# NOTE: copy test images to the expected target directory.
#
# NOTE: BINDIR is specified on the command line (see MSVC project file)
#

TARGETS=                            

# "all target does not exist"
# See notes about 'pseudotarget' a.k.a. phony target at the bottom of this makefile. That's what you get when using nmake instead of (GNU) make.
all : $(TARGETS)
	@echo "Target: all --> done"
	@echo "leptonica/MSVC: copy test images to the expected target directory."
	@echo "BINDIR=$(BINDIR)"
	-mkdir "$(BINDIR)/../../lept/"
	-mkdir "$(BINDIR)/../../lept/demo-data/"
	-robocopy /S /COPY:DAT /DCOPY:DAT /PURGE ../../thirdparty/leptonica/prog/ "$(BINDIR)/../../lept/demo-data/" * /XF *.c /XF *.cc /XF *.h
	-del "$(BINDIR)/../../lept/demo-data/*.c"
	-del "$(BINDIR)/../../lept/demo-data/*.cc"
	-del "$(BINDIR)/../../lept/demo-data/*.h"
	-robocopy /COPY:DAT /DCOPY:DAT ../../thirdparty/leptonica/prog/ "$(BINDIR)/../../lept/demo-data/" kernel_reg.c

rebuild : clean all
	@echo "Target: rebuild --> done"

clean :
	@echo "Target: clean --> done"
	-rmdir -Recurse -Force "$(BINDIR)/../../lept/demo-data/"
	-rmdir /S "$(BINDIR)/../../lept/demo-data/"
	# First rmdir is for cmd/powershell environment, second if for rmdir from unix tools (git-for-windows).
	# You need either one when running this from MSVC IDE or direct from MSVC command line.

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

