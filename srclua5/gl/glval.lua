------------------------------------------------------------------------------
-- GLVal class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "glval",
  funcname = "GLVal",
  include = "iupglcontrols.h",
  parent = iup.WIDGET,
  creation = "",
  subdir = "gl",
  callback = {
    valuechanging_cb = "n",
  },
}

function ctrl.createElement(class, param)
   return iup.GLVal(param[1])
end

iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iupWidget")
