------------------------------------------------------------------------------
-- Item class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "item",
  parent = iup.WIDGET,
  subdir = "elem",
  creation = "S-",
  callback = {
    action = "", 
    highlight_cb = "",
  }
} 

function ctrl.createElement(class, param)
   return iup.Item(param.title)
end
   
iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iupWidget")
