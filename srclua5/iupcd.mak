PROJNAME = iup
LIBNAME = iupluacd

IUP := ..

OPT = YES
NO_LUALINK = Yes

# Can not use USE_IUPLUA because Tecmake will include "iupluacd5X" in linker
USE_CD = YES
USE_CDLUA = YES
USE_IUP3 = YES

INCLUDES = ../include
LDIR = ../lib/$(TEC_UNAME)
DEFINES = CD_NO_OLD_INTERFACE
SRC = iuplua_cd.c
DEF_FILE = iupluacd.def

ifdef USE_LUA53
  LUASFX = 53
  DEFINES += LUA_COMPAT_MODULE
else
ifdef USE_LUA52
  LUASFX = 52
  DEFINES += LUA_COMPAT_MODULE
else
  USE_LUA51 = Yes
  LUASFX = 51
endif
endif

LIBNAME := $(LIBNAME)$(LUASFX)
LIBS = iuplua$(LUASFX)

ifneq ($(findstring MacOS, $(TEC_UNAME)), )
  LIBS:=
  USE_CDLUA:=
endif
