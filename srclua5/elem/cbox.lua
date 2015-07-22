------------------------------------------------------------------------------
-- Cbox class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "cbox",
  parent = iup.BOX,
  subdir = "elem",
  creation = "-",
  callback = {},
}

function ctrl.createElement(class, param)
  return iup.Cbox()
end

iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iupWidget")
