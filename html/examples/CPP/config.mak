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

SRC = alarm.cpp
#SRC = button.cpp
#SRC = dialog1.cpp
#SRC = dialog2.cpp
#SRC = filedlg.cpp
#SRC = fill.cpp
#SRC = frame.cpp
#SRC = getattribute.cpp
#SRC = getfile.cpp
#SRC = hbox.cpp
#SRC = idle.cpp
#SRC = image.cpp
#SRC = item.cpp
#SRC = label.cpp
#SRC = list1.cpp
#SRC = list2.cpp
#SRC = listdialog.cpp
#SRC = lua_init.cpp
#SRC = mask.cpp
#SRC = menu.cpp
#SRC = message.cpp
#SRC = multiline1.cpp
#SRC = multiline2.cpp
#SRC = matrixlist.cpp
#SRC = progressbar.cpp
#SRC = radio.cpp
#SRC = sample.cpp
#SRC = sbox1.cpp
#SRC = sbox2.cpp
#SRC = scanf.cpp
#SRC = separator.cpp
#SRC = submenu.cpp
#SRC = tabs.cpp
#SRC = text.cpp
#SRC = timer.cpp
#SRC = toggle.cpp
#SRC = tree.cpp
#SRC = val.cpp
#SRC = vbox.cpp
#SRC = zbox.cpp

#ifneq ($(findstring Win, $(TEC_SYSNAME)), )
#  LIBS += iupimglib
#else
#  SLIB += $(IUP)/lib/$(TEC_UNAME)/libiupimglib.a
#endif

#USE_CD = Yes
#SRC = canvas1.cpp
#SRC = canvas2.cpp
#SRC = canvas3.cpp
#SRC = scrollbar.cpp

#LIBS = cdcontextplus gdiplus

#USE_OPENGL = Yes
#SRC = glcanvas.cpp

#IUPWEB_SAMPLE=Yes
ifdef IUPWEB_SAMPLE
  SRC = webbrowser.cpp
  ifneq ($(findstring Win, $(TEC_SYSNAME)), )
    LIBS += iupweb iupole
  else
    LIBS += webkit-1.0
    SLIB += $(IUP)/lib/$(TEC_UNAME)d/libiupweb.a
  endif
endif

#USE_IUPCONTROLS = Yes
#SRC = canvas3.cpp
#SRC = cbox.cpp
#SRC = cells_checkboard.cpp
#SRC = cells_degrade.cpp
#SRC = cells_numbering.cpp
#SRC = colorbar.cpp
#SRC = colorbrowser.cpp
#SRC = gauge.cpp
#SRC = getcolor.cpp
#SRC = getparam.cpp
#SRC = matrix.cpp
#SRC = mdi.cpp

#IUPPLOT_SAMPLE=Yes
ifdef IUPPLOT_SAMPLE
  USE_IUPCONTROLS = Yes
  LINKER = g++
  SRC = plot.cpp
  ifneq ($(findstring Win, $(TEC_SYSNAME)), )
    LIBS += iup_plot cdpdflib
  else
    SLIB += $(IUP)/lib/$(TEC_UNAME)/libiup_plot.a $(CD)/lib/$(TEC_UNAME)/libcdpdflib.a
  endif
endif

#IUPMGLPLOT_SAMPLE=Yes
ifdef IUPMGLPLOT_SAMPLE
  USE_OPENGL = Yes
  USE_IUPCONTROLS = Yes
  LINKER = g++
  SRC = mathglsamples.cpp
  ifneq ($(findstring Win, $(TEC_SYSNAME)), )
    LIBS += iup_mglplot
  else
    SLIB += $(IUP)/lib/$(TEC_UNAME)/libiup_mglplot.a
  endif
  
  USE_IM = Yes
  ifdef USE_IM
    ifneq ($(findstring Win, $(TEC_SYSNAME)), )
      LIBS += iupim im_process
    else
      SLIB += $(IUPLIB)/libiupim.a $(IMLIB)/libim_process.a
    endif
  endif
endif

#IUPSCINTILLA_SAMPLE=Yes
ifdef IUPSCINTILLA_SAMPLE
  USE_IUPCONTROLS = Yes
  LINKER = g++  
  SRC = scintilla.cpp
  ifneq ($(findstring Win, $(TEC_SYSNAME)), )
    LIBS += iup_scintilla
  else
    SLIB += $(IUP)/lib/$(TEC_UNAME)d/libiup_scintilla.a
  endif
endif

#USE_LUA51=Yes
#USE_IUPLUA=Yes
#SRC = lua_init.cpp

ifneq ($(findstring Win, $(TEC_SYSNAME)), )
  INCLUDES += ../../../etc/
  SRC += ../../../etc/iup.rc
endif
