PROJNAME = iup
APPNAME = ledc
APPTYPE = console
OPT = YES

INCLUDES = .

SRC = lex.yy.c y.tab.c ledc.c

ifeq ($(TEC_UNAME), vc8)
  ifdef DBG
    #debug info not working for vc8 linker
    define DBG
    endef
  endif
endif         
