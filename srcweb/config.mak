PROJNAME = iup
LIBNAME = iupweb
OPT = YES

INCLUDES =  ../include ../src
LDIR = ../lib/$(TEC_UNAME)  
LIBS = iup
SRC = iup_webbrowser.c

ifneq ($(findstring Win, $(TEC_SYSNAME)), )
  SRC += iupwin_webbrowser.cpp
  LIBS += iupole comsuppw
  USE_ATL = Yes
  DEFINES = _MBCS
else
  ifdef GTK_DEFAULT
    SRC  += iupgtk_webbrowser.c
    USE_GTK = Yes
    INCLUDES += ../src/gtk
    INCLUDES += $(GTK)/include/webkit-1.0 $(GTK)/include/libsoup-2.4
    
    ifneq ($(findstring Linux3, $(TEC_UNAME)), )
      ifneq ($(findstring Linux31, $(TEC_UNAME)), )
        USE_GTK3 = Yes
        LIBS += webkitgtk-3.0
        INCLUDES += $(GTK)/include/webkitgtk-3.0
      else
        LIBS += webkitgtk-1.0
        INCLUDES += $(GTK)/include/webkitgtk-1.0
      endif
    else
      LIBS += webkit-1.0
    endif
  else
#    SRC = iupmot_webbrowser.c
#    LIBS += XmHTML
  endif
endif

ifneq ($(findstring MacOS, $(TEC_UNAME)), )
  ifneq ($(TEC_SYSMINOR), 4)
    BUILD_DYLIB=Yes
  endif
endif
