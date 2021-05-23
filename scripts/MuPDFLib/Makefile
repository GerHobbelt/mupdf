#
# This Makefile is supposed to be executed using Microsoft NMAKE (**NOT** GNU make!)
#
# Defined by caller:
# OUTDIR=$(OutDir)
# PLATFORM=$(Platform) 
# PLATFORM_TARGET=$(PlatformTarget) 
# PROJECTDIR=$(ProjectDir) 
# INTERMEDIATEDIR=$(IntDir) 
# SOLUTIONDIR=$(SolutionDir) 
# TARGETNAME=$(TargetName)
#

!IFNDEF OUTDIR
OUTDIR=tmp
!ENDIF
!IFNDEF PLATFORM
PLATFORM=Win32
!ENDIF
!IFNDEF PLATFORM_TARGET
PLATFORM_TARGET=x86
!ENDIF
!IFNDEF PROJECTDIR
PROJECTDIR=..\..\platform\win32
!ENDIF
!IFNDEF INTERMEDIATEDIR
INTERMEDIATEDIR=tmp
!ENDIF
!IFNDEF SOLUTIONDIR
SOLUTIONDIR=..\..\platform\win32
!ENDIF
!IFNDEF TARGETNAME
TARGETNAME=MuPDFLib
!ENDIF

TARGET_DLL_DEF=$(PROJECTDIR)libmupdf.def

misc:
	@echo "MISC... Should not be invoked..."

build:    "$(TARGET_DLL_DEF)"

"$(TARGET_DLL_DEF)": "$(PROJECTDIR)gen_libmupdf.def.py" "$(PROJECTDIR)..\..\scripts\MuPDFLib\Makefile"
	@echo "-----------------------------------------------"
	@echo "build: OUTDIR=$(OUTDIR)"
	@echo "build: PLATFORM=$(PLATFORM)"
	@echo "build: PLATFORM_TARGET=$(PLATFORM_TARGET)"
	@echo "build: PROJECTDIR=$(PROJECTDIR)"
	@echo "build: INTERMEDIATEDIR=$(INTERMEDIATEDIR)"
	@echo "build: SOLUTIONDIR=$(SOLUTIONDIR)"
	@echo "build: TARGETNAME=$(TARGETNAME)"
	@echo "-----------------------------------------------"
	-cd
	-cd ../..
	-cd
	python --version
	python "$(PROJECTDIR)gen_libmupdf.def.py"

rebuild: clean build

clean:
	-del "$(TARGET_DLL_DEF)"

.PHONY: clean rebuild
