------------------------------------------------------------------------------
-- Fill class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "fill",
  parent = iup.WIDGET,
  subdir = "elem",
  creation = "",
  callback = {}
}

function ctrl.createElement(class, param)
   return iup.Fill()
end

iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iupWidget")
