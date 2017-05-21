------------------------------------------------------------------------------
-- Calendar class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "calendar",
  parent = iup.WIDGET,
  subdir = "elem",
  creation = "",
  callback = {}
}

function ctrl.createElement(class, param)
  return iup.Calendar()
end

iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iupWidget")
