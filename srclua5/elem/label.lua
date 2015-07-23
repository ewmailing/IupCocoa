------------------------------------------------------------------------------
-- Label class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "label",
  parent = iup.WIDGET,
  subdir = "elem",
  creation = "S",
  callback = {}
}

function ctrl.createElement(class, param)
   return iup.Label(param.title)
end

iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iupWidget")
