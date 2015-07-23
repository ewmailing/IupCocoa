PROJNAME = iup
LIBNAME = iupcontrols
OPT = YES

INCLUDES = ../include ../src . ../srccd

SRCCOLOR = iup_colorbrowser.c iup_colorhsi.c iup_colorbrowserdlg.c
SRCCOLOR := $(addprefix color/, $(SRCCOLOR))

SRCMATRIX = iupmat_key.c iupmat_mark.c iupmat_aux.c iupmat_mem.c iupmat_mouse.c iupmat_numlc.c \
            iupmat_colres.c iupmat_draw.c iupmat_getset.c iupmatrix.c \
            iupmat_scroll.c iupmat_edit.c iupmat_ex.c
SRCMATRIX := $(addprefix matrix/, $(SRCMATRIX))

SRC = iup_gauge.c iup_cells.c iup_colorbar.c iup_controls.c \
      iup_dial.c iup_oldmask.c iup_matrixlist.c \
      $(SRCCOLOR) $(SRCMATRIX)

LIBS = iup iupcd
LDIR = ../lib/$(TEC_UNAME)
USE_CD = Yes

# force the definition of math functions using float
# Some compilers do not define them
ifeq ($(TEC_UNAME), dll)
  DEFINES += IUP_DEFMATHFLOAT
endif         
ifeq ($(TEC_UNAME), vc6)
  DEFINES += IUP_DEFMATHFLOAT
endif         
ifeq ($(TEC_UNAME), dll7)
  DEFINES += IUP_DEFMATHFLOAT
endif         
ifeq ($(TEC_UNAME), vc7)
  DEFINES += IUP_DEFMATHFLOAT
endif         
ifneq ($(findstring ow, $(TEC_UNAME)), )
  DEFINES += IUP_DEFMATHFLOAT
endif         
ifneq ($(findstring bc, $(TEC_UNAME)), )
  DEFINES += IUP_DEFMATHFLOAT
endif
ifneq ($(findstring AIX, $(TEC_UNAME)), )
  DEFINES += IUP_DEFMATHFLOAT
endif
ifneq ($(findstring SunOS, $(TEC_UNAME)), )
  DEFINES += IUP_DEFMATHFLOAT
endif

ifneq ($(findstring MacOS, $(TEC_UNAME)), )
  ifneq ($(TEC_SYSMINOR), 4)
    BUILD_DYLIB=Yes
  endif
endif
