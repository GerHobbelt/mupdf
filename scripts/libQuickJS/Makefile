
QJSC=qjsc.exe

DSTDIR=$(PROJDIR)/../../scripts/libQuickJS/
SRCDIR=$(PROJDIR)/../../thirdparty/owemdjee/QuickJS/

DEPENDENCIES=													\
		$(PROJDIR)/../../scripts/libQuickJS/Makefile			\
		$(QJSC)				

all:						\
		$(DSTDIR)qjscalc.c  \
		$(DSTDIR)qjsrepl.c

forced-rebuild: clean all

clean:
	-rm -f $(DSTDIR)qjscalc.c
	-rm -f $(DSTDIR)qjsrepl.c


$(DSTDIR)qjscalc.c: $(SRCDIR)qjscalc.js $(DEPENDENCIES)
	$(QJSC) -v -fbignum -m -c -o $@  $(SRCDIR)qjscalc.js

$(DSTDIR)qjsrepl.c: $(SRCDIR)repl.js $(DEPENDENCIES)
	$(QJSC) -v -fbignum -m -c -o $@  $(SRCDIR)repl.js



.PHONY: all clean forced-rebuild
