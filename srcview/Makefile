ifeq ($(OS), Windows_NT)
  TECMAKE_CMD = $(MAKE) --no-print-directory -f ../tecmakewin.mak TEC_UNAME=$(TEC_UNAME)
else
  TECMAKE_CMD = $(MAKE) --no-print-directory -f ../tecmake.mak
endif

.PHONY: do_all iupview iupviewgtk
do_all: iupview

iupview:
	@$(TECMAKE_CMD) 

iupviewgtk:
	@$(TECMAKE_CMD) USE_GTK=Yes
