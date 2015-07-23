PROJNAME = iup
LIBNAME  = iupluagl3
OPT = YES

USE_LUA  = Yes
USE_OPENGL = Yes
USE_MACOS_OPENGL = Yes

SRC = iuplua_glcanvas.c

INCLUDES = ../include
LDIR = ../lib/$(TEC_UNAME)  
LIBS = iup iuplua3 iupgl

ifneq ($(findstring MacOS, $(TEC_UNAME)), )
  ifdef USE_MACOS_OPENGL
    LFLAGS = -framework OpenGL
    USE_OPENGL :=
  endif
  ifneq ($(TEC_SYSMINOR), 4)
    BUILD_DYLIB=Yes
  endif
endif
