------------------------------------------------------------------------------
-- FlatLabel class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "flatlabel",
  parent = iup.WIDGET,
  subdir = "elem",
  funcname = "FlatLabel",
  creation = "S",
  callback = {}
}

function ctrl.createElement(class, param)
   return iup.FlatLabel(param.title)
end

iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iupWidget")
