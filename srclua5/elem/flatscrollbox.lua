------------------------------------------------------------------------------
-- ScrollBox class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "flatscrollbox",
  parent = iup.BOX,
  subdir = "elem",
  creation = "I",
  funcname = "FlatScrollBox",
  callback = {}
}

function ctrl.createElement(class, param)
   return iup.FlatScrollBox()
end

iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iupWidget")
