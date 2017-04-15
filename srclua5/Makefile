ifeq ($(OS), Windows_NT)
  TECMAKE_CMD = $(MAKE) --no-print-directory -f ../tecmakewin.mak TEC_UNAME=$(TEC_UNAME)
  WINLIBS = iupluaole
else
  TECMAKE_CMD = $(MAKE) --no-print-directory -f ../tecmake.mak
endif

.PHONY: do_all iuplua iupluacd iupluacontrols iupluamatrixex iupluagl iupluaglcontrols iuplua_plot iuplua_mglplot iuplua_scintilla iupluaim iupluaimglib iupluatuio iupluaole iupluaweb
do_all: iuplua iupluacd iupluacontrols iupluamatrixex iupluagl iupluaglcontrols iuplua_plot iuplua_mglplot iuplua_scintilla iupluaim iupluaimglib iupluatuio $(WINLIBS) iupluaweb

iuplua:
	@$(TECMAKE_CMD)  

iupluacd:
	@$(TECMAKE_CMD) MF=iupcd 

iupluacontrols:
	@$(TECMAKE_CMD) MF=iupcontrols 

iupluamatrixex:
	@$(TECMAKE_CMD) MF=iupmatrixex

iupluagl:
	@$(TECMAKE_CMD) MF=iupgl 

iupluaglcontrols:
	@$(TECMAKE_CMD) MF=iupglcontrols 

iuplua_plot:
	@$(TECMAKE_CMD) MF=iup_plot 

iuplua_mglplot:
	@$(TECMAKE_CMD) MF=iup_mglplot 

iuplua_scintilla:
	@$(TECMAKE_CMD) MF=iup_scintilla 

iupluaim:
	@$(TECMAKE_CMD) MF=iupim 

iupluaimglib:
	@$(TECMAKE_CMD) MF=iupimglib 
  
iupluatuio:
	@$(TECMAKE_CMD) MF=iuptuio 
  
iupluaole:
	@$(TECMAKE_CMD) MF=iupole 
  
iupluaweb:
	@$(TECMAKE_CMD) MF=iupweb 
