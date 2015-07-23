------------------------------------------------------------------------------
-- GLButton class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "glbutton",
  funcname = "GLButton",
  include = "iupglcontrols.h",
  parent = iup.WIDGET,
  creation = "S",
  subdir = "gl",
  callback = {
    action = "", 
  }
} 

function ctrl.createElement(class, param)
  return iup.GLButton(param.title)
end

iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iupWidget")
