------------------------------------------------------------------------------
-- GLSizeBox class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "glsizebox",
  funcname = "GLSizeBox",
  include = "iupglcontrols.h",
  parent = iup.WIDGET,
  subdir = "gl",
  creation = "I",
  callback = {
  }
}

function ctrl.createElement(class, param)
   return iup.GLSizeBox(param[1])
end

iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iup widget")
