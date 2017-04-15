------------------------------------------------------------------------------
-- ZBox class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "zbox",
  parent = iup.BOX,
  subdir = "elem",
  creation = "-",
  callback = {}
}

function ctrl.createElement(class, param)
   return iup.Zbox()
end

iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iupWidget")
