PROJNAME = iup
LIBNAME = iupluacd3
OPT = YES

DEF_FILE = iupluacd3.def
SRC = iuplua_cd.c

# Can not use USE_IUPLUA because Tecmake will include "iupluacd3" in linker

INCLUDES = ../include
LIBS = iuplua3
LDIR = ../lib/$(TEC_UNAME)

IUP := ..

USE_CD = YES
USE_IUP3 = YES
USE_LUA = YES
USE_CDLUA = YES

ifneq ($(findstring MacOS, $(TEC_UNAME)), )
  ifneq ($(TEC_SYSMINOR), 4)
    BUILD_DYLIB=Yes
  endif
endif
