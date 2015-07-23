PROJNAME = iup
APPNAME := iuplua
APPTYPE = console

STRIP = 
OPT = YES      
NO_SCRIPTS = Yes
# IM and IupPlot uses C++
LINKER = $(CPPC)

NO_LUAOBJECT = Yes
USE_BIN2C_LUA = Yes

ifdef USE_LUA53
  LUASFX = 53
  
  ifneq ($(findstring SunOS, $(TEC_UNAME)), )
    ifneq ($(findstring x86, $(TEC_UNAME)), )
      FLAGS = -std=gnu99
    endif
  endif
else
ifdef USE_LUA52
  LUASFX = 52
else
  USE_LUA51 = Yes
  LUASFX = 51
endif
endif

APPNAME := $(APPNAME)$(LUASFX)
SRC = iup_lua$(LIBLUASFX).c

ifdef NO_LUAOBJECT
  DEFINES += IUPLUA_USELH
  USE_LH_SUBDIR = Yes
  LHDIR = lh
else
  DEFINES += IUPLUA_USELOH
  USE_LOH_SUBDIR = Yes
  LOHDIR = loh$(LIBLUASFX)
endif

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

SRCLUA = console5.lua indent.lua

ifdef DBG
  ALL_STATIC=Yes
endif

ifdef ALL_STATIC
  # Statically link everything only when debugging
  IUP := ..
  USE_IUPLUA = Yes
  USE_IUP3 = Yes
  USE_STATIC = Yes
  
  ifdef DBG_DIR
    IUP_LIB = $(IUP)/lib/$(TEC_UNAME)d
    CD_LIB = $(CD)/lib/$(TEC_UNAME)d
    IM_LIB = $(IM)/lib/$(TEC_UNAME)d
#    LUA_LIB = $(LUA)/lib/$(TEC_UNAME)d
  else
    IUP_LIB = $(IUP)/lib/$(TEC_UNAME)
    CD_LIB = $(CD)/lib/$(TEC_UNAME)
    IM_LIB = $(IM)/lib/$(TEC_UNAME)
#    LUA_LIB = $(LUA)/lib/$(TEC_UNAME)
  endif  
  
  DEFINES += USE_STATIC

  ifeq "$(TEC_UNAME)" "SunOS510x86"
    IUPLUA_NO_GL = Yes
  endif
    
  #IUPLUA_NO_GL = Yes
  ifndef IUPLUA_NO_GL 
    USE_OPENGL = Yes
    USE_IUPGLCONTROLS = Yes
  else
    DEFINES += IUPLUA_NO_GL
  endif

  #IUPLUA_NO_CD = Yes
  ifndef IUPLUA_NO_CD 
    USE_CDLUA = Yes
    USE_IUPCONTROLS = Yes
    ifneq ($(findstring Win, $(TEC_SYSNAME)), )
      LIBS += iuplua_plot$(LIBLUASFX) iup_plot cdgl
      LIBS += iupluamatrixex$(LIBLUASFX) iupmatrixex
    else
      SLIB += $(IUP_LIB)/libiuplua_plot$(LIBLUASFX).a $(IUP_LIB)/libiup_plot.a $(CD_LIB)/libcdgl.a
      SLIB += $(IUP_LIB)/libiupluamatrixex$(LIBLUASFX).a $(IUP_LIB)/libiupmatrixex.a
    endif
      
    ifndef IUPLUA_NO_IM
      ifneq ($(findstring Win, $(TEC_SYSNAME)), )
        LIBS += cdluaim$(LIBLUASFX)
      else
        SLIB += $(CD_LIB)/libcdluaim$(LIBLUASFX).a
      endif
    endif
    ifneq ($(findstring Win, $(TEC_SYSNAME)), )
      ifndef USE_GTK
        USE_GDIPLUS=Yes
      endif
    else
      ifdef USE_MOTIF
        USE_XRENDER=Yes
      endif
    endif
  else
    DEFINES += IUPLUA_NO_CD
  endif

  #IUPLUA_NO_IM = Yes
  ifndef IUPLUA_NO_IM
    USE_IMLUA = Yes
    
    ifneq ($(findstring Win, $(TEC_SYSNAME)), )
      LIBS += imlua_process$(LIBLUASFX) iupluaim$(LIBLUASFX) im_process iupim
    else
      SLIB += $(IM_LIB)/libimlua_process$(LIBLUASFX).a $(IUP_LIB)/libiupluaim$(LIBLUASFX).a $(IM_LIB)/libim_process.a $(IUP_LIB)/libiupim.a
    endif
    
  else
    DEFINES += IUPLUA_NO_IM
  endif

  IUPLUA_IMGLIB = Yes
  ifdef IUPLUA_IMGLIB
    DEFINES += IUPLUA_IMGLIB
    ifneq ($(findstring Win, $(TEC_SYSNAME)), )
      LIBS += iupluaimglib$(LIBLUASFX) iupimglib
    else
      SLIB += $(IUP_LIB)/libiupluaimglib$(LIBLUASFX).a $(IUP_LIB)/libiupimglib.a
    endif
  endif
  
  #IUPLUA_TUIO = Yes
  ifdef IUPLUA_TUIO
    DEFINES += IUPLUA_TUIO
    ifneq ($(findstring Win, $(TEC_SYSNAME)), )
      LIBS += iupluatuio$(LIBLUASFX) iuptuio
      LIBS += ws2_32 winmm
    else
      SLIB += $(IUP_LIB)/libiupluatuio$(LIBLUASFX).a $(IUP_LIB)/libiuptuio.a
    endif
  endif
else
  ifneq ($(findstring Win, $(TEC_SYSNAME)), )
    # Dinamically link in Windows, when not debugging
    # Must call "tecmake dll8" so USE_* will use the correct TEC_UNAME
    USE_DLL = Yes
    GEN_MANIFEST = No
  else
    # In UNIX Lua is always statically linked, late binding is used.
    USE_STATIC = Yes
    
    # Except in Cygwin
    ifneq ($(findstring cygw, $(TEC_UNAME)), )
      USE_STATIC:=
    endif
  endif
endif


ifneq ($(findstring Win, $(TEC_SYSNAME)), )
  #Comment the following line to build under MingW
  SLIB += setargv.obj
  SRC += iuplua5.rc
endif

ifneq ($(findstring cygw, $(TEC_UNAME)), )
  LIBS += readline history
endif

ifneq ($(findstring MacOS, $(TEC_UNAME)), )
  LIBS += readline
endif

ifneq ($(findstring Linux, $(TEC_UNAME)), )
  LIBS += dl 
  #To allow late binding
  LFLAGS = -Wl,-E
  LIBS += readline history 
#  LIBS += curses ncurses
endif

ifneq ($(findstring BSD, $(TEC_UNAME)), )
  #To allow late binding
  LFLAGS = -Wl,-E
  LIBS += readline history curses ncurses
endif

ifneq ($(findstring SunOS, $(TEC_UNAME)), )
  LIBS += dl
endif

ifneq ($(findstring AIX, $(TEC_UNAME)), )
  FLAGS  += -mminimal-toc
  OPTFLAGS = -mminimal-toc -ansi -pedantic 
  LFLAGS = -Xlinker "-bbigtoc"
endif

