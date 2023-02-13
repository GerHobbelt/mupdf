#

QJSC=qjsc.exe

DSTDIR=$(PROJDIR)/../../scripts/libQuickJS/
SRCDIR=$(PROJDIR)/../../thirdparty/owemdjee/QuickJS/

all:						\
		$(DSTDIR)qjscalc.c  \
		$(DSTDIR)qjsrepl.c

forced-rebuild: clean all

clean:
	-rm -f $(DSTDIR)qjscalc.c
	-rm -f $(DSTDIR)qjsrepl.c


$(DSTDIR)qjscalc.c: $(SRCDIR)qjscalc.js
	$(QJSC) -v -fbignum -m -c -o $@ $?

$(DSTDIR)qjsrepl.c: $(SRCDIR)repl.js
	$(QJSC) -v -fbignum -m -c -o $@ $?



.PHONY: all clean forced-rebuild
