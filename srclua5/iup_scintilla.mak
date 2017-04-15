PROJNAME = iup
LIBNAME = iuplua_scintilla

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

INCLUDES = ../srclua5
DEF_FILE = ctrl/iuplua_scintilla.def
LIBS = iup_scintilla

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

SRCLUA = scintilla.lua

GC := $(addsuffix .c, $(basename $(SRCLUA)))
GC := $(addprefix ctrl/il_, $(GC))

$(GC) : ctrl/il_%.c : ctrl/%.lua generator.lua
	$(LUABIN) generator.lua $<

SRC := $(GC)

ifneq ($(findstring MacOS, $(TEC_UNAME)), )
  USE_IUPLUA:=
endif
