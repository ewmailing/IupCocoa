------------------------------------------------------------------------------
-- VBox class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "vbox",
  parent = iup.BOX,
  subdir = "elem",
  creation = "-",
  callback = {}
}

function ctrl.createElement(class, param)
   return iup.Vbox()
end

iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iupWidget")
