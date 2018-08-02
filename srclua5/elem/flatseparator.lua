------------------------------------------------------------------------------
-- FlatSeparator class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "flatseparator",
  parent = iup.WIDGET,
  funcname = "FlatSeparator",
  subdir = "elem",
  creation = "",
  callback = {}
}

function ctrl.createElement(class, param)
   return iup.FlatSeparator()
end

iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iupWidget")
