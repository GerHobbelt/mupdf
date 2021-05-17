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
TARGETNAME=qiqqa-mutools-packager
!ENDIF

!IF EXIST("C:\Program Files (x86)\Inno Setup 6\iscc.exe")
ISCC=C:\Program Files (x86)\Inno Setup 6\iscc.exe
!ELSEIF EXIST("C:\Program Files\Inno Setup 6\iscc.exe")
ISCC=C:\Program Files\Inno Setup 6\iscc.exe
!ELSE
!ERROR "Inno Setup Compiler 6.* has not been installed or cannot be located in your program files."
!ENDIF

TARGET_PACKAGE_EXE=$(OUTDIR)\$(TARGETNAME).exe

misc:
	@echo "MISC... Should not be invoked..."

build:    "$(TARGET_PACKAGE_EXE)"

"$(TARGET_PACKAGE_EXE)":
	@echo "-----------------------------------------------"
	@echo "build: OUTDIR=$(OUTDIR)"
	@echo "build: PLATFORM=$(PLATFORM)"
	@echo "build: PLATFORM_TARGET=$(PLATFORM_TARGET)"
	@echo "build: PROJECTDIR=$(PROJECTDIR)"
	@echo "build: INTERMEDIATEDIR=$(INTERMEDIATEDIR)"
	@echo "build: SOLUTIONDIR=$(SOLUTIONDIR)"
	@echo "build: TARGETNAME=$(TARGETNAME)"
	@echo "-----------------------------------------------"
	-mkdir "$(INTERMEDIATEDIR)"
	-"$(ISCC)"
	-mkdir "$(OUTDIR)"
	echo x > "$(TARGET_PACKAGE_EXE)"

rebuild: clean build

clean:
	-del "$(TARGET_PACKAGE_EXE)"

.PHONY: clean rebuild
