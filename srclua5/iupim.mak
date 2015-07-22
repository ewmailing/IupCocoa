PROJNAME = iup
LIBNAME  = iupluaim

IUP := ..

OPT = YES
NO_LUALINK = Yes

USE_IUP3 = Yes
USE_IUPLUA = Yes
USE_IMLUA = Yes

SRC = iuplua_im.c
DEF_FILE = iupluaim.def
INCLUDES = ../src
LIBS = iupim

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

ifneq ($(findstring MacOS, $(TEC_UNAME)), )
  USE_IUPLUA:=
  USE_IMLUA:=
  USE_IM = Yes
endif
