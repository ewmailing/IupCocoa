------------------------------------------------------------------------------
-- GLToggle class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "gltoggle",
  funcname = "GLToggle",
  include = "iupglcontrols.h",
  parent = iup.WIDGET,
  creation = "S",
  subdir = "gl",
  callback = {
    action = "n",
  }
} 

function ctrl.createElement(class, param)
  return iup.GLToggle(param.title)
end
   
iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iupWidget")
