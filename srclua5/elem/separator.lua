------------------------------------------------------------------------------
-- Separator class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "separator",
  parent = iup.WIDGET,
  subdir = "elem",
  creation = "",
  callback = {}
}

function ctrl.createElement(class, param)
   return iup.Separator()
end

iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iupWidget")
