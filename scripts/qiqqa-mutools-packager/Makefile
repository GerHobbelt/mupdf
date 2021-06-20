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
#-----------------------------------------------
# Sample values for my dev box:
#
#::OUTDIR=Z:\lib\tooling\qiqqa\MuPDF\platform\win32\bin\Release-NotSet-32bit-x86\
#::PLATFORM=Win32
#::PLATFORM_TARGET=x86
#::PROJECTDIR=Z:\lib\tooling\qiqqa\MuPDF\platform\win32\
#::INTERMEDIATEDIR=Z:\lib\tooling\qiqqa\MuPDF\platform\win32\obj\Release-NotSet-32bit-x86\-Makefile-qiqqa-mutools-packager\
#::SOLUTIONDIR=Z:\lib\tooling\qiqqa\MuPDF\platform\win32\
#::TARGETNAME=qiqqa-mutools-packager"
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

"$(TARGET_PACKAGE_EXE)": ../../scripts/qiqqa-mutools-packager/adjust-inno-setup-script.js ../../scripts/qiqqa-mutools-packager/inno-setup-script.iss ../../scripts/qiqqa-mutools-packager/Makefile
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
	node ../../scripts/qiqqa-mutools-packager/adjust-inno-setup-script.js ../../scripts/qiqqa-mutools-packager/inno-setup-script.iss \
		"::OUTDIR=$(OUTDIR) \
		::PLATFORM=$(PLATFORM) \
		::PLATFORM_TARGET=$(PLATFORM_TARGET) \
		::PROJECTDIR=$(PROJECTDIR) \
		::INTERMEDIATEDIR=$(INTERMEDIATEDIR) \
		::SOLUTIONDIR=$(SOLUTIONDIR) \
		::TARGETNAME=$(TARGETNAME)"
	"$(ISCC)" "..\..\scripts\qiqqa-mutools-packager\inno-setup-script.iss"
# echo x > "$(TARGET_PACKAGE_EXE)"

rebuild: clean build

clean:
	-del "$(TARGET_PACKAGE_EXE)"



# =============================================================================================================
#
# Recursive calls to build all release flavours all at once.
#
# =============================================================================================================

build-all:

rebuild-all: 

clean-all:
	nmake /help



.PHONY: clean rebuild misc

