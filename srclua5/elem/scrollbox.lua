------------------------------------------------------------------------------
-- ScrollBox class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "scrollbox",
  parent = iup.BOX,
  subdir = "elem",
  creation = "I",
  funcname = "ScrollBox",
  callback = {}
}

function ctrl.createElement(class, param)
   return iup.ScrollBox()
end

iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iupWidget")
