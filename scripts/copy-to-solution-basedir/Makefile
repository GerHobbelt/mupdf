
DSTDIR=$(PROJDIR)

# always only copy Release build executables to the base directory

SRCDIR32=$(DSTDIR)/bin/Release-Unicode-32bit-x86/
SRCDIR64=$(DSTDIR)/bin/Release-Unicode-64bit-x64/

DEPENDENCIES=											            		\
		$(PROJDIR)/../../scripts/copy-to-solution-basedir/Makefile

		
.PHONY: all clean forced-rebuild bin2coff msvc_sln_cleaner utilities


all: utilities
	@echo "copy $(SRCDIR64)/*.exe to $(DSTDIR)"
	-robocopy /NP /COPY:DAT /DCOPY:DAT $(SRCDIR64) $(DSTDIR) *.exe

# cp $(SRCDIR32)/*.exe $(DSTDIR)
# cp $(SRCDIR64)/*.exe $(DSTDIR)

utilities: bin2coff msvc_sln_cleaner
	@echo "Utilities copied to $(DSTDIR)"

bin2coff:
	@echo "copy bin2coff.exe to $(DSTDIR)"
	-robocopy /NP /IM /IS /COPY:DAT /DCOPY:DAT $(SRCDIR32) $(DSTDIR) bin2coff.exe

msvc_sln_cleaner:
	@echo "copy msvc_sln_cleaner.exe to $(DSTDIR)"
	-robocopy /NP /IM /IS /COPY:DAT /DCOPY:DAT $(SRCDIR32) $(DSTDIR) msvc_sln_cleaner.exe

forced-rebuild: clean all
	@echo "forced-rebuild"

clean:
	@echo "clean"
	-rm -f $(DSTDIR)*.exe


