PROJNAME = iup
APPNAME = iupsimple
APPTYPE = console

ifdef GTK_DEFAULT
  ifdef USE_MOTIF
    # Build Motif version in Linux and BSD
    APPNAME := $(APPNAME)mot
  endif
else  
  ifdef USE_GTK
    # Build GTK version in IRIX,SunOS,AIX,Win32
    APPNAME := $(APPNAME)gtk
  endif
endif

INCLUDES = ../include

USE_IUP3 = Yes
USE_STATIC = Yes

IUP = ../../..
PROJDIR = ../../..

#USE_CD = Yes
#LIBS += iupimglib
SRC += sample.c

