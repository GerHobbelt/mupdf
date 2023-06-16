#
# NOTE: copy test images to the expected target directory.
#
# NOTE: BINDIR is specified on the command line (see MSVC project file)
#

TARGETS=                            

all : $(TARGETS)
	echo "Target: all --> done"
	echo "BINDIR=$(BINDIR)"
	-mkdir "$(BINDIR)/../../lept/"
	-mkdir "$(BINDIR)/../../lept/demo-data/"
	-robocopy /S /COPY:DAT /DCOPY:DAT /PURGE ../../thirdparty/leptonica/prog/ "$(BINDIR)/../../lept/demo-data/" * /XF *.c /XF *.cc /XF *.h
	-del "$(BINDIR)/../../lept/demo-data/*.c"
	-del "$(BINDIR)/../../lept/demo-data/*.cc"
	-del "$(BINDIR)/../../lept/demo-data/*.h"
	-copy ../../thirdparty/leptonica/prog/kernel_reg.c "$(BINDIR)/../../lept/demo-data/"

rebuild : clean all
	echo "Target: rebuild --> done"

clean :
	echo "Target: clean --> done"
	-rmdir /S "$(BINDIR)/../../lept/demo-data/"


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

