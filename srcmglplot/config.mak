PROJNAME = iup
LIBNAME  = iup_mglplot
OPT = YES

USE_OPENGL = Yes

ifdef DBG
  DEFINES += IUP_ASSERT
  ifndef DBG_DIR
    ifneq ($(findstring Win, $(TEC_SYSNAME)), )
      LIBNAME := $(LIBNAME)_debug
    endif
  endif  
endif  

DEF_FILE = iup_mglplot.def

INCLUDES = ../include ../src . 
LDIR = ../lib/$(TEC_UNAME) $(CD)/lib/$(TEC_UNAME)
LIBS := iup iupgl $(LIBS)
DEFINES += MGL_STATIC_DEFINE MGL_SRC

# File config.h modified for IUP

SRCMGLPLOT = addon.cpp complex.cpp data_gr.cpp evalp.cpp fit.cpp pde.cpp vect.cpp \
             axis.cpp complex_io.cpp data_io.cpp exec.cpp font.cpp pixel.cpp volume.cpp \
             base.cpp cont.cpp data_png.cpp export.cpp obj.cpp plot.cpp window.cpp \
             base_cf.cpp crust.cpp export_2d.cpp opengl.cpp prim.cpp \
             canvas.cpp data.cpp eval.cpp export_3d.cpp other.cpp surf.cpp \
             canvas_cf.cpp data_ex.cpp evalc.cpp fft.cpp parser.cpp \
             complex_ex.cpp fractal.cpp s_hull/s_hull_pro.cpp
SRCMGLPLOT := $(addprefix src/, $(SRCMGLPLOT))

SRC = iup_mglplot.cpp $(SRCMGLPLOT)

ifneq ($(findstring MacOS, $(TEC_UNAME)), )
  INCLUDES += $(X11_INC)
  ifneq ($(TEC_SYSMINOR), 4)
    BUILD_DYLIB=Yes
  endif
endif
