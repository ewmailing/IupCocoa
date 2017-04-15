------------------------------------------------------------------------------
-- GLText class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "gltext",
  funcname = "GLText",
  include = "iupglcontrols.h",
  parent = iup.WIDGET,
  creation = "",
  subdir = "gl",
  callback = {
    valuechanged_cb = "", 
  }
} 

function ctrl.createElement(class, param)
  return iup.GLText()
end

iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iupWidget")
