PROJNAME = iup
LIBNAME = iupmatrixex
OPT = YES

INCLUDES = ../include ../src .

SRC = iup_matrixex.c iupmatex_clipboard.c iupmatex_busy.c \
      iupmatex_export.c iupmatex_visible.c iupmatex_copy.c \
      iupmatex_units.c iupmatex_find.c iupmatex_undo.c \
      iupmatex_sort.c

LIBS = iup iupcontrols
LDIR = ../lib/$(TEC_UNAME)
USE_CD = Yes

ifneq ($(findstring MacOS, $(TEC_UNAME)), )
  ifneq ($(TEC_SYSMINOR), 4)
    BUILD_DYLIB=Yes
  endif
endif
