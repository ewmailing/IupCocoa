------------------------------------------------------------------------------
-- Split class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "split",
  parent = iup.BOX,
  subdir = "elem",
  creation = "II",
  callback = {}
}

function ctrl.createElement(class, param)
   return iup.Split()
end

iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iupWidget")
