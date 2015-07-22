PROJNAME = iup
APPNAME = iuplua3
APPTYPE = console
OPT = YES

IUP := ..
                
# IM is a C++ library with a C API                
LINKER = $(CPPC)

SRC = iup_lua3.c
SRCLUA = console3.lua
LOHDIR = loh3

USE_LUA = Yes
USE_IUPLUA = Yes
USE_IUP3 = Yes
          
USE_STATIC = Yes

ifdef BUILD_64
  ifneq ($(findstring SunOS, $(TEC_UNAME)), )
    LINKER = cc
    IUPLUA_NO_IM = Yes
  endif
  ifneq ($(findstring AIX, $(TEC_UNAME)), )
    LINKER = cc
    IUPLUA_NO_IM = Yes
  endif
endif

ifeq "$(TEC_UNAME)" "SunOS510x86"
  IUPLUA_NO_GL = Yes
endif

#IUPLUA_NO_GL = Yes
ifndef IUPLUA_NO_GL
  USE_OPENGL = Yes
else
  DEFINES = IUPLUA_NO_GL
endif

#IUPLUA_NO_CD = Yes
ifndef IUPLUA_NO_CD 
  USE_CD = Yes
  USE_CDLUA = Yes
  USE_IUPCONTROLS = Yes
  ifneq ($(findstring Win, $(TEC_SYSNAME)), )
    LIBS += iup_pplot iuplua_pplot3
  else
    IUPLIB = $(IUP)/lib/$(TEC_UNAME)
    SLIB += $(IUPLIB)/libiuplua_pplot3.a $(IUPLIB)/libiup_pplot.a
  endif
else
  DEFINES += IUPLUA_NO_CD
endif

#IUPLUA_NO_IM = Yes
ifndef IUPLUA_NO_IM
  USE_IM = Yes   
  USE_IMLUA = Yes
  
  ifneq ($(findstring Win, $(TEC_SYSNAME)), )
    LIBS += iupim iupluaim3
  else
    IUPLIB = $(IUP)/lib/$(TEC_UNAME)
    SLIB += $(IUPLIB)/libiupluaim3.a $(IUPLIB)/libiupim.a
  endif
else
  DEFINES += IUPLUA_NO_IM
endif

ifneq ($(findstring AIX, $(TEC_UNAME)), )
  FLAGS  += -mminimal-toc
  OPTFLAGS = -mminimal-toc -ansi -pedantic 
  LFLAGS = -Xlinker "-bbigtoc"
endif

ifneq ($(findstring Win, $(TEC_SYSNAME)), )
  SRC += iuplua3.rc
endif

ifneq ($(findstring cygw, $(TEC_UNAME)), )
  LIBS += fontconfig
endif

ifeq ($(TEC_UNAME), vc8)
  ifdef DBG
    #debug info not working for vc8 linker
    define DBG
    endef
  endif
endif     
    
