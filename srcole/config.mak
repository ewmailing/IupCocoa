PROJNAME = iup
LIBNAME = iupole
OPT = YES

INCLUDES =  ../include ../src
LDIR = ../lib/$(TEC_UNAME)  
LIBS = iup

SRC = iup_olecontrol.cpp \
			tLegacy.cpp \
			tAmbientProperties.cpp \
			tDispatch.cpp \
			tOleClientSite.cpp \
			tOleControlSite.cpp \
			tOleHandler.cpp \
			tOleInPlaceFrame.cpp \
			tOleInPlaceSite.cpp


ifneq ($(findstring cygw, $(TEC_UNAME)), )
  LIBS += uuid gdi32 oleaut32 ole32 
endif

#IMPORTANT: there is a libuuid in cygwin/lib that is not related to Win32 uuid
#           this will affect linking the DLL or the application
#           one solution was to move it to a safe place 