PROJNAME = iup
LIBNAME  = iup_plot
OPT = YES

USE_CD = Yes
USE_OPENGL = Yes
USE_MACOS_OPENGL = Yes

ifdef DBG
  DEFINES += IUP_ASSERT
  ifndef DBG_DIR
    ifneq ($(findstring Win, $(TEC_SYSNAME)), )
      LIBNAME := $(LIBNAME)_debug
    endif
  endif  
endif  

INCLUDES = ../include ../src ../srccd
LDIR = ../lib/$(TEC_UNAME)  
LIBS = iup iupgl iupcd cdgl 
ifneq ($(findstring Win, $(TEC_SYSNAME)), )
  LIBS += cdcontextplus
endif

DEFINES = CD_NO_OLD_INTERFACE

SRC = iup_plot.cpp  iupPlotCalc.cpp iupPlot.cpp iupPlotDraw.cpp iupPlotTick.cpp iup_plot_attrib.cpp

ifneq ($(findstring MacOS, $(TEC_UNAME)), )
  INCLUDES += $(X11_INC)
  ifdef USE_MACOS_OPENGL
    LFLAGS = -framework OpenGL
    USE_OPENGL :=
  endif
  ifneq ($(TEC_SYSMINOR), 4)
    BUILD_DYLIB=Yes
  endif
endif

ifneq ($(findstring Linux, $(TEC_UNAME)), )
  CPPFLAGS = -Wno-reorder -Wno-write-strings
endif
ifneq ($(findstring cygw, $(TEC_UNAME)), )
  CPPFLAGS = -Wno-reorder -Wno-write-strings
endif
