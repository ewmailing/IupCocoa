PROJNAME = iup
APPNAME = iupsample
#APPTYPE = console

ifdef GTK_DEFAULT
  ifdef USE_MOTIF
    # Build Motif version in Linux and BSD
    APPNAME = iupsamplemot
  endif
else  
  ifdef USE_GTK
    # Build GTK version in IRIX,SunOS,AIX,Win32
    APPNAME = iupsamplegtk
  endif
endif

INCLUDES = ../include

USE_IUP3 = Yes
USE_STATIC = Yes

IUP = ../../..
PROJDIR = ../../..

#DBG = Yes

SRC = counter.c
#SRC = circle_drawer.c
#SRC = crud.c
#SRC = flight_booker.c
#SRC = temperature_converter.c
#SRC = timer.c

#USE_IUPCONTROLS = Yes
#SRC = cells.c
