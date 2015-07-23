PROJNAME = iup
LIBNAME  = iuplua_pplot3
OPT = YES

USE_LUA  = Yes
USE_CDLUA = Yes

LOHDIR = loh
SRCLUA = pplot.lua
SRC    = iuplua_pplot.c

DEFINES = IUPLUA_USELOH
INCLUDES = ../include
LDIR = ../lib/$(TEC_UNAME)  
LIBS = iup iuplua3 iup_pplot iupcd

ifneq ($(findstring MacOS, $(TEC_UNAME)), )
  ifneq ($(TEC_SYSMINOR), 4)
    BUILD_DYLIB=Yes
  endif
endif
