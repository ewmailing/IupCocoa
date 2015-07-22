------------------------------------------------------------------------------
-- MglPlot class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "mglplot",
  parent = iup.WIDGET,
  creation = "",
  funcname = "MglPlot",
  subdir = "ctrl",
  callback = {
    predraw_cb = "",
    postdraw_cb = "",
  },
  include = "iup_mglplot.h",
  extrafuncs = 1,
}

function ctrl.createElement(class, param)
   return iup.MglPlot(param.action)
end

iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iupWidget")
