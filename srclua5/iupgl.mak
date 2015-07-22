PROJNAME = iup
LIBNAME = iupluagl

IUP := ..

OPT = YES
NO_LUAOBJECT = Yes
NO_LUALINK = Yes
USE_BIN2C_LUA = Yes

# Can not use USE_IUPLUA because Tecmake will include "iupluagl5X" in linker
USE_IUP3 = Yes
USE_OPENGL = Yes
USE_MACOS_OPENGL = Yes

INCLUDES = ../srclua5
DEF_FILE = ctrl/iupluagl.def

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
ifdef NO_LUAOBJECT
  DEFINES += IUPLUA_USELH
  USE_LH_SUBDIR = Yes
  LHDIR = lh
else
  DEFINES += IUPLUA_USELOH
  USE_LOH_SUBDIR = Yes
  LOHDIR = loh$(LUASFX)
endif

SRCLUA = glcanvas.lua
LIBS = iuplua$(LUASFX)

GC = $(addsuffix .c, $(basename $(SRCLUA)))
GC := $(addprefix ctrl/il_, $(GC))

$(GC) : ctrl/il_%.c : ctrl/%.lua generator.lua
	$(LUABIN) generator.lua $<

SRC	= ctrl/iuplua_glcanvas.c $(GC)

ifneq ($(findstring MacOS, $(TEC_UNAME)), )
  LIBS:=iupgl
  ifdef USE_MACOS_OPENGL
    LFLAGS = -framework OpenGL
    USE_OPENGL :=
  endif
endif
