------------------------------------------------------------------------------
-- GLProgressBar class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "glprogressbar",
  include = "iupglcontrols.h",
  parent = iup.WIDGET,
  creation = "",
  subdir = "gl",
  funcname = "GLProgressBar",
  callback = {}
} 

function ctrl.createElement(class, param)
   return iup.GLProgressBar()
end
   
iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iupWidget")
