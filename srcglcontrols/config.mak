PROJNAME = iup
LIBNAME = iupglcontrols
OPT = YES

INCLUDES = ../include ../src .

SRC = iup_glcontrols.c iup_glcanvasbox.c iup_glsubcanvas.c iup_gllabel.c \
      iup_glimage.c iup_glfont.c iup_gldraw.c iup_glicon.c iup_glseparator.c \
      iup_glbutton.c iup_gltoggle.c iup_gllink.c iup_glprogressbar.c \
      iup_glval.c iup_glframe.c iup_glexpander.c iup_glscrollbars.c \
      iup_glscrollbox.c iup_glsizebox.c iup_gltext.c

LIBS = iupgl iup
LDIR = ../lib/$(TEC_UNAME) $(CD)/lib/$(TEC_UNAME)

USE_FTGL = Yes
USE_OPENGL = Yes

DEFINES = FTGL_LIBRARY_STATIC
ifneq ($(findstring dll, $(TEC_UNAME)), )
  DEFINES = FTGL_LIBRARY
endif

ifneq ($(findstring cygw, $(TEC_UNAME)), )
  LIBS += fontconfig
endif
ifneq ($(findstring MacOS, $(TEC_UNAME)), )
  LIBS += fontconfig
endif

ifneq ($(findstring MacOS, $(TEC_UNAME)), )
  STDINCS += $(X11_INC)
  ifneq ($(TEC_SYSMINOR), 4)
    BUILD_DYLIB=Yes
  endif
endif
