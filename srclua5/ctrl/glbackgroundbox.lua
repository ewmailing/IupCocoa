------------------------------------------------------------------------------
-- GLBackgroundBox class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "glbackgroundbox",
  parent = iup.BOX,
  include = "iupgl.h",
  subdir = "ctrl",
  creation = "I",
  funcname = "GLBackgroundBox",
  callback = {
    action = "ff",
    }
}

function ctrl.createElement(class, param)
   return iup.GLBackgroundBox()
end

iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iupWidget")
