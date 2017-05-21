PROJNAME = iup
LIBNAME = iupluamatrixex

IUP := ..

OPT = YES
NO_LUAOBJECT = Yes
# To not link with the Lua dynamic library in UNIX
NO_LUALINK = Yes
# To use a subfolder with the Lua version for binaries
LUAMOD_DIR = Yes
USE_BIN2C_LUA = Yes

USE_IUP3 = Yes
USE_IUPLUA = Yes
USE_CDLUA = Yes

INCLUDES = ../src ctrl ../srclua5
LIBS = iupmatrixex
DEF_FILE = ctrl/iupluamatrixex.def

ifdef USE_LUA_VERSION
  USE_LUA51:=
  USE_LUA52:=
  USE_LUA53:=
  ifeq ($(USE_LUA_VERSION), 53)
    USE_LUA53:=Yes
  endif
  ifeq ($(USE_LUA_VERSION), 52)
    USE_LUA52:=Yes
  endif
  ifeq ($(USE_LUA_VERSION), 51)
    USE_LUA51:=Yes
  endif
endif

ifdef USE_LUA53
  LUASFX = 53
else
ifdef USE_LUA52
  LUASFX = 52
else
  USE_LUA51 = Yes
  LUASFX = 51
endif
endif

LIBNAME := $(LIBNAME)$(LUASFX)
ifdef NO_LUAOBJECT
  DEFINES += IUPLUA_USELH
  USE_LH_SUBDIR = Yes
  LHDIR = lh
else
  DEFINES += IUPLUA_USELOH
  USE_LOH_SUBDIR = Yes
  LOHDIR = loh$(LUASFX)
endif

SRCLUA = matrixex.lua

GC := $(addsuffix .c, $(basename $(SRCLUA)))
GC := $(addprefix ctrl/il_, $(GC))

$(GC) : ctrl/il_%.c : ctrl/%.lua generator.lua
	$(LUABIN) generator.lua $<

SRC := iuplua_matrixex_aux.c $(GC)

ifneq ($(findstring MacOS, $(TEC_UNAME)), )
  USE_IUPLUA:=
  USE_CDLUA:=
  USE_CD = Yes
endif
