PROJNAME = iup
APPNAME = iuptest
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

ifeq "$(TEC_SYSNAME)" "Haiku"
  USE_HAIKU = Yes
  USE_STATIC =
endif

ifdef DBG_DIR
  IUP_LIB = $(IUP)/lib/$(TEC_UNAME)d
  CD_LIB = $(CD)/lib/$(TEC_UNAME)d
  IM_LIB = $(IM)/lib/$(TEC_UNAME)d
else
  IUP_LIB = $(IUP)/lib/$(TEC_UNAME)
  CD_LIB = $(CD)/lib/$(TEC_UNAME)
  IM_LIB = $(IM)/lib/$(TEC_UNAME)
endif  

# Must uncomment all SRC lines
DEFINES = BIG_TEST
SRC += bigtest.c  

SRC += tray.c
SRC += dialog.c
SRC += calendar.c
SRC += predialogs.c
SRC += timer.c
SRC += label.c
SRC += canvas.c
SRC += frame.c
SRC += idle.c
SRC += button.c
SRC += flatbutton.c
SRC += flatframe.c
SRC += toggle.c
SRC += vbox.c
SRC += hbox.c
SRC += progressbar.c
SRC += progressdlg.c
SRC += text.c
SRC += val.c
SRC += tabs.c
SRC += sample.c
SRC += flatsample.c
SRC += menu.c
SRC += spin.c
SRC += text_spin.c
SRC += list.c
SRC += sysinfo.c
SRC += mdi.c
SRC += getparam.c
SRC += getcolor.c
SRC += class_conf.c
SRC += tree.c
SRC += zbox.c
SRC += scanf.c
SRC += sbox.c
SRC += scrollbox.c
SRC += clipboard.c
SRC += split.c
SRC += link.c
SRC += gridbox.c
SRC += expander.c
SRC += cbox.c
SRC += detachbox.c
SRC += flattabs.c
SRC += flatscrollbox.c
SRC += dropbutton.c
SRC += flatlabel.c
SRC += flattoggle.c

ifneq ($(findstring Win, $(TEC_SYSNAME)), )
  LIBS += iupimglib
else
  SLIB += $(IUP_LIB)/libiupimglib.a
endif

USE_CD = Yes
SRC += canvas_scrollbar.c
SRC += canvas_cddbuffer.c
SRC += canvas_cdsimple.c

USE_OPENGL = Yes
USE_FTGL = Yes
DEFINES += USE_OPENGL
SRC += glcanvas.c
SRC += glcanvas_cube.c
SRC += charac.c
ifneq ($(findstring Win, $(TEC_SYSNAME)), )
  LIBS += cdgl iupglcontrols
  LDIR += $(CD)/lib/$(TEC_UNAME)
else
  SLIB += $(CD_LIB)/libcdgl.a \
          $(IUP_LIB)/libiupglcontrols.a 
  ifneq ($(findstring CYGW, $(TEC_SYSNAME)), )
    LIBS += fontconfig iconv 
  endif  
  ifneq ($(findstring MacOS, $(TEC_SYSNAME)), )
    INCLUDES += $(X11_INC)
    LIBS += fontconfig iconv
  endif
  ifdef USE_MOTIF
    LIBS += fontconfig
  endif  
endif

USE_IUPCONTROLS = Yes
SRC += colorbrowser.c
SRC += dial.c
SRC += dial_led.c
SRC += colorbar.c
SRC += cells_numbering.c
SRC += cells_degrade.c
SRC += cells_checkboard.c
SRC += gauge.c
SRC += matrix.c
SRC += matrix_cbs.c
SRC += matrix_cbmode.c
SRC += matrixlist.c

LINKER = g++
DEFINES += PLOT_TEST
SRC += plot.c
ifneq ($(findstring Win, $(TEC_SYSNAME)), )
  LIBS += iup_plot cdcontextplus gdiplus
#  LIBS += cdpdflib
#  LDIR += $(IUP)/lib/$(TEC_UNAME)
else
  SLIB += $(IUP_LIB)/libiup_plot.a
  SLIB += $(CD_LIB)/libcdcontextplus.a
#  SLIB += $(CD_LIB)/libcdpdflib.a
endif

DEFINES += MGLPLOT_TEST
SRC += mglplot.c
SRC += mathglsamples.c
SRC += mgllabel.c
ifneq ($(findstring Win, $(TEC_SYSNAME)), )
  LIBS += iup_mglplot
else
  SLIB += $(IUP_LIB)/libiup_mglplot.a
endif

SRC += webbrowser.c
USE_IUPWEB = Yes

SRC += scintilla.c
ifneq ($(findstring Win, $(TEC_SYSNAME)), )
  LIBS += iup_scintilla imm32
else
  SLIB += $(IUP_LIB)/libiup_scintilla.a
endif

USE_IM = Yes
ifdef USE_IM
ifneq ($(findstring Win, $(TEC_SYSNAME)), )
  LIBS += iupim im_process
else
  SLIB += $(IUP_LIB)/libiupim.a $(IM_LIB)/libim_process.a
endif
endif

ifneq ($(findstring Win, $(TEC_SYSNAME)), )
  SRC += iuptest.rc 
  INCLUDES += ../../../etc
else
  ifneq ($(findstring cygw, $(TEC_UNAME)), )
    SRC += iuptest.rc
  endif
endif
 
#ifneq ($(findstring Win, $(TEC_SYSNAME)), )
#  USE_GDIPLUS=Yes
#else
#  USE_XRENDER=Yes
#endif
