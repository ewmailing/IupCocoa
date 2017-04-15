------------------------------------------------------------------------------
-- FlatFrame class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "flatframe",
  parent = iup.BOX,
  subdir = "elem",
  creation = "I",
  funcname = "FlatFrame",
  callback = {}
}

function ctrl.createElement(class, param)
   return iup.FlatFrame()
end

iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iupWidget")
