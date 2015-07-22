------------------------------------------------------------------------------
-- GLFrame class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "glframe",
  funcname = "GLFrame",
  include = "iupglcontrols.h",
  parent = iup.BOX,
  subdir = "gl",
  creation = "I",
  callback = {
  }
}

function ctrl.createElement(class, param)
   return iup.GLFrame()
end

iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iupWidget")
