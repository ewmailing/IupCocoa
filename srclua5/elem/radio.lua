------------------------------------------------------------------------------
-- Radio class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "radio",
  parent = iup.BOX,
  subdir = "elem",
  creation = "I",
  callback = {}
} 

function ctrl.createElement(class, param)
   return iup.Radio()
end
   
iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iupWidget")
