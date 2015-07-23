------------------------------------------------------------------------------
-- GLScrollBox class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "glscrollbox",
  funcname = "GLScrollBox",
  include = "iupglcontrols.h",
  parent = iup.BOX,
  subdir = "gl",
  creation = "I",
  callback = {
  }
}

function ctrl.createElement(class, param)
   return iup.GLScrollBox()
end

iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iupWidget")
