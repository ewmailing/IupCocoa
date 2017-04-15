PROJNAME = iup
APPNAME = scintilla_notepad

INCLUDES = ../include

USE_IUP = Yes

USE_STATIC = Yes

#APPTYPE = console
IUP = ../../../..
PROJDIR = ../../../..
#DBG = Yes
OPT=Yes

SRC = scintilla_notepad.c

ifdef DBG_DIR
  SFX=d
endif

ifneq ($(findstring Win, $(TEC_SYSNAME)), )
  LIBS += iupimglib iup_scintilla
else
  SLIB += $(IUP)/lib/$(TEC_UNAME)$(SFX)/libiup_scintilla.a \
          $(IUP)/lib/$(TEC_UNAME)$(SFX)/libiupimglib.a
endif

ifneq ($(findstring Win, $(TEC_SYSNAME)), )
  INCLUDES += $(IUP)/etc
  SRC += $(IUP)/etc/iup.rc
endif
