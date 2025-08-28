
DSTDIR=$(PROJDIR)

# always only copy Release build executables to the base directory

SRCDIR32=$(DSTDIR)/bin/Release-Unicode-32bit-x86/
SRCDIR64=$(DSTDIR)/bin/Release-Unicode-64bit-x64/

ICONV_TOOL="$(SRCDIR64)libiconv_tools.exe"

DEPENDENCIES=											            		\
		$(PROJDIR)/../../scripts/libiconv/Makefile

		
.PHONY: all clean forced-rebuild regenerate


all: regenerate
	@echo "copy $(SRCDIR64)/*.exe to $(DSTDIR)"
	-$(ICONV_TOOL) -h
#	-robocopy /NP /COPY:DAT /DCOPY:DAT $(SRCDIR64) $(DSTDIR) *.exe

regenerate: 
	@echo "Utilities copied to $(DSTDIR)"
	-$(ICONV_TOOL) -h

forced-rebuild: clean all
	@echo "forced-rebuild"

clean:
	@echo "clean"
	-rm -f $(DSTDIR)*.exe


