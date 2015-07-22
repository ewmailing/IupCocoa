ifeq ($(OS), Windows_NT)
  TECMAKE_CMD = $(MAKE) --no-print-directory -f ../tecmakewin.mak TEC_UNAME=$(TEC_UNAME)
else
  TECMAKE_CMD = $(MAKE) --no-print-directory -f ../tecmake.mak
endif

.PHONY: do_all iuplua iupluacd iupluacontrols iuplua_pplot iupluagl iupluaim
do_all: iuplua iupluacd iupluacontrols iuplua_pplot iupluagl iupluaim

iuplua:
	@$(TECMAKE_CMD) 

iupluacd:
	@$(TECMAKE_CMD) MF=iupcd

iupluacontrols:
	@$(TECMAKE_CMD) MF=iupcontrols

iuplua_pplot:
	@$(TECMAKE_CMD) MF=iup_pplot

iupluagl:
	@$(TECMAKE_CMD) MF=iupgl

iupluaim:
	@$(TECMAKE_CMD) MF=iupim
