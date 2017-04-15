------------------------------------------------------------------------------
-- DatePick class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "datepick",
  parent = iup.WIDGET,
  subdir = "elem",
  funcname = "DatePick",
  creation = "",
  callback = {}
}

function ctrl.createElement(class, param)
  return iup.DatePick()
end

iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iupWidget")
