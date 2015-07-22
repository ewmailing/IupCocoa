------------------------------------------------------------------------------
-- GLExpander class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "glexpander",
  parent = iup.BOX,
  funcname = "GLExpander",
  include = "iupglcontrols.h",
  creation = "I",
  subdir = "gl",
  callback = {
    action = "", 
  }
}

function ctrl.createElement(class, param)
  return iup.GLExpander()
end

iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iupWidget")
