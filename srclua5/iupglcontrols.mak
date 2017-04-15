PROJNAME = iup
LIBNAME = iupluaglcontrols

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

INCLUDES = ../src gl ../srclua5
LIBS = iupglcontrols iuplua$(LUASFX)
DEF_FILE = gl/iupluaglcontrols.def

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

SRCLUA = glcanvasbox.lua glsubcanvas.lua gllabel.lua glbutton.lua \
         glexpander.lua glframe.lua gllink.lua glprogressbar.lua \
         glseparator.lua gltoggle.lua glval.lua glscrollbox.lua \
         glsizebox.lua gltext.lua

GC := $(addsuffix .c, $(basename $(SRCLUA)))
GC := $(addprefix gl/il_, $(GC))

$(GC) : gl/il_%.c : gl/%.lua generator.lua
	$(LUABIN) generator.lua $<

SRC := gl/iuplua_glcontrols.c $(GC)

ifneq ($(findstring MacOS, $(TEC_UNAME)), )
  USE_IUPLUA:=
  LIBS:=iupglcontrols
endif
