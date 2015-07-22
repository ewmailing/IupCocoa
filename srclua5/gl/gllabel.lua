------------------------------------------------------------------------------
-- GLLabel class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "gllabel",
  funcname = "GLLabel",
  include = "iupglcontrols.h",
  parent = iup.WIDGET,
  creation = "S",
  subdir = "gl",
  callback = {}
}

function ctrl.createElement(class, param)
   return iup.GLLabel(param.title)
end

iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iupWidget")
