BUILD_IUP3 = Yes

PROJNAME = iup
LIBNAME = iuplua3
OPT = YES

LOHDIR = loh
SRCLUA = iuplua.lua iuplua_widgets.lua constants.lua spin.lua \
         sbox.lua scrollbox.lua split.lua val.lua tree.lua tabs.lua
SRC    = iuplua.c iuplua_api.c iuplua_widgets.c il_scanf.c il_cbox.c \
         il_sbox.c il_scrollbox.c il_split.c il_spin.c il_val.c il_tree.c \
         il_tabs.c il_getcolor.c il_getparam.c

USE_LUA = Yes

DEFINES = IUPLUA_USELOH
INCLUDES = ../include ../src
LDIR = ../lib/$(TEC_UNAME)  
LIBS = iup

ifneq ($(findstring MacOS, $(TEC_UNAME)), )
  ifneq ($(TEC_SYSMINOR), 4)
    BUILD_DYLIB=Yes
  endif
endif
