PROJNAME = iup
LIBNAME  = iupluaim3
OPT = YES

USE_LUA  = Yes
USE_IM = Yes
                     
SRC = iuplua_im.c

INCLUDES = ../include ../src
LDIR = ../lib/$(TEC_UNAME)  
LIBS = iup iuplua3 iupim

ifneq ($(findstring MacOS, $(TEC_UNAME)), )
  ifneq ($(TEC_SYSMINOR), 4)
    BUILD_DYLIB=Yes
  endif
endif
