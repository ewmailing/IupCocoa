
require("iuplua")
require("iuplua_mglplot")


mgp = iup.mgllabel{
  rastersize = "400x80", 
  labeltitle = "\\int \\alpha \\sqrt{sin(\\pi x)^2 + \\gamma_{i_k}} dx",
  labelfontsize="10"}
sb = iup.vbox{mgp}
dlg = iup.dialog{sb; title="Dialog"}
dlg:show()


if (iup.MainLoopLevel()==0) then
  iup.MainLoop()
end
