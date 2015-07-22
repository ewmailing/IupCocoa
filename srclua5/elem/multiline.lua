------------------------------------------------------------------------------
-- Multiline class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "multiline",
  parent = iup.WIDGET,
  subdir = "elem",
  creation = "-",
  callback = {
    action = "ns", 
  },
  funcname = "MultiLine",
} 

function ctrl.createElement(class, param)
   return iup.MultiLine()
end
   
iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iupWidget")
