------------------------------------------------------------------------------
-- GLLink class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "gllink",
  funcname = "GLLink",
  include = "iupglcontrols.h",
  parent = iup.WIDGET,
  creation = "SS",
  subdir = "gl",
  callback = {
    action = "s", 
  }
}

function ctrl.createElement(class, param)
   return iup.GLLink(param.url, param.title)
end

iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iupWidget")
