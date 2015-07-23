BUILD_IUP3 = Yes

PROJNAME = iup
LIBNAME  = iupluacontrols3
OPT = YES

USE_LUA  = Yes
USE_CDLUA = Yes

LOHDIR = loh
SRCLUA =  dial.lua gauge.lua colorbar.lua \
          matrix.lua colorbrowser.lua cells.lua
SRC    =  il_dial.c il_gauge.c il_colorbrowser.c il_colorbar.c il_cells.c \
          il_mask.c il_matrix.c iuplua_controls.c

DEFINES = IUPLUA_USELOH
INCLUDES = ../include
LDIR = ../lib/$(TEC_UNAME)  
LIBS = iup iuplua3 iupcontrols iupcd

ifneq ($(findstring MacOS, $(TEC_UNAME)), )
  ifneq ($(TEC_SYSMINOR), 4)
    BUILD_DYLIB=Yes
  endif
endif
