------------------------------------------------------------------------------
-- GLSizeBox class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "glsizebox",
  funcname = "GLSizeBox",
  include = "iupglcontrols.h",
  parent = iup.BOX,
  subdir = "gl",
  creation = "I",
  callback = {
  }
}

function ctrl.createElement(class, param)
   return iup.GLSizeBox()
end

iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iupWidget")
