
DSTDIR=$(PROJDIR)

# always only copy Release build executables to the base directory

SRCDIR32=$(DSTDIR)/bin/Release-Unicode-32bit-x86/
SRCDIR64=$(DSTDIR)/bin/Release-Unicode-64bit-x64/

DEPENDENCIES=											            		\
		$(PROJDIR)/../../scripts/copy-to-solution-basedir/Makefile

all:
	@echo "copy $(SRCDIR64)/*.exe to $(DSTDIR)"
	cp $(SRCDIR32)/bin2coff.exe $(DSTDIR)
	-robocopy /NP /COPY:DAT /DCOPY:DAT $(SRCDIR64) $(DSTDIR) *.exe

# cp $(SRCDIR32)/*.exe $(DSTDIR)
# cp $(SRCDIR64)/*.exe $(DSTDIR)

forced-rebuild: clean all
	@echo "forced-rebuild"

clean:
	@echo "clean"
	-rm -f $(DSTDIR)*.exe



.PHONY: all clean forced-rebuild
