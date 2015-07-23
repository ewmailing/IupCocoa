------------------------------------------------------------------------------
-- GridBox class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "gridbox",
  parent = iup.BOX,
  subdir = "elem",
  creation = "-",
  funcname = "GridBox",
  callback = {}
}

function ctrl.createElement(class, param)
   return iup.GridBox()
end

iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iupWidget")
