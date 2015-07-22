------------------------------------------------------------------------------
-- MglLabel class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "mgllabel",
  parent = iup.WIDGET,
  creation = "S",
  funcname = "MglLabel",
  subdir = "ctrl",
  include = "iup_mglplot.h",
  callback = {}
}

function ctrl.createElement(class, param)
   return iup.MglLabel(param.action)
end

iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iupWidget")
