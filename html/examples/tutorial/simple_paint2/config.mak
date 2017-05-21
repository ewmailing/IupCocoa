PROJNAME = iup
APPNAME = simple_paint

INCLUDES = ../include

USE_IUP = Yes
USE_CD = Yes
USE_IM = Yes

USE_STATIC = Yes

#APPTYPE = console
IUP = ../../../..
PROJDIR = ../../../..
#DBG = Yes
OPT=Yes

SRC = simple_paint_file.cpp simple_paint_main.cpp simple_paint_toolbox.cpp \
      simple_paint_util.cpp simple_paint.cpp simple_paint_canvas.cpp \
      simple_paint_splash.cpp

#USE_CONTEXTPLUS = Yes
#USE_OPENGL = Yes

ifdef DBG_DIR
  SFX=d
endif

ifneq ($(findstring Win, $(TEC_SYSNAME)), )
  LIBS += iupimglib im_process iupim cdim
else
  SLIB += $(CD)/lib/$(TEC_UNAME)$(SFX)/libcdim.a \
          $(IUP)/lib/$(TEC_UNAME)$(SFX)/libiupim.a \
          $(IUP)/lib/$(TEC_UNAME)$(SFX)/libiupimglib.a \
          $(IM)/lib/$(TEC_UNAME)$(SFX)/libim_process.a
endif

ifneq ($(findstring Win, $(TEC_SYSNAME)), )
  INCLUDES += $(IUP)/etc
  SRC += $(IUP)/etc/iup.rc
endif

ifdef USE_CONTEXTPLUS
  DEFINES = USE_CONTEXTPLUS
  LIBS += cdcontextplus
  ifneq ($(findstring Win, $(TEC_SYSNAME)), )
    LIBS += gdiplus
  endif
endif

ifdef USE_OPENGL
  DEFINES = USE_OPENGL
  LINK_FTGL = Yes
  ifneq ($(findstring Win, $(TEC_SYSNAME)), )
    LIBS += iupgl cdgl
  else
    SLIB += $(IUP)/lib/$(TEC_UNAME)$(SFX)/libiupgl.a \
            $(CD)/lib/$(TEC_UNAME)$(SFX)/libcdgl.a
  endif
endif
