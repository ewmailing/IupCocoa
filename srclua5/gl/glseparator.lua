------------------------------------------------------------------------------
-- GLSeparator class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "glseparator",
  funcname = "GLSeparator",
  include = "iupglcontrols.h",
  parent = iup.WIDGET,
  creation = "",
  subdir = "gl",
  callback = {}
}

function ctrl.createElement(class, param)
   return iup.GLSeparator()
end

iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iupWidget")
