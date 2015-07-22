PROJNAME = iup
LIBNAME = iupluaglcontrols

IUP := ..

OPT = YES
NO_LUAOBJECT = Yes
NO_LUALINK = Yes
USE_BIN2C_LUA = Yes

# Can not use USE_IUPLUA because Tecmake will include "iupluagl5X" in linker
USE_IUP3 = Yes
USE_OPENGL = Yes
USE_MACOS_OPENGL = Yes

INCLUDES = ../src gl ../srclua5
LIBS = iupglcontrols iuplua$(LUASFX)
DEF_FILE = gl/iupluaglcontrols.def

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

SRCLUA = glcanvasbox.lua glsubcanvas.lua gllabel.lua glbutton.lua \
         glexpander.lua glframe.lua gllink.lua glprogressbar.lua \
         glseparator.lua gltoggle.lua glval.lua glscrollbox.lua \
         glsizebox.lua

GC := $(addsuffix .c, $(basename $(SRCLUA)))
GC := $(addprefix gl/il_, $(GC))

$(GC) : gl/il_%.c : gl/%.lua generator.lua
	$(LUABIN) generator.lua $<

SRC := gl/iuplua_glcontrols.c $(GC)

ifneq ($(findstring MacOS, $(TEC_UNAME)), )
  LIBS:=iupglcontrols
  ifdef USE_MACOS_OPENGL
    LFLAGS = -framework OpenGL
    USE_OPENGL :=
  endif
endif
