------------------------------------------------------------------------------
-- HBox class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "hbox",
  parent = iup.BOX,
  subdir = "elem",
  creation = "-",
  callback = {}
}

function ctrl.createElement(class, param)
   return iup.Hbox()
end

iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iupWidget")
