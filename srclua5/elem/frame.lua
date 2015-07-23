------------------------------------------------------------------------------
-- Frame class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "frame",
  parent = iup.BOX,
  subdir = "elem",
  creation = "I",
  callback = {}
}

function ctrl.createElement(class, param)
   return iup.Frame()
end

iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iupWidget")
