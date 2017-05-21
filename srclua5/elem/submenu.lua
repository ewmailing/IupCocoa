------------------------------------------------------------------------------
-- Submenu class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "submenu",
  parent = iup.BOX,
  subdir = "elem",
  creation = "SI",
  callback = {
--    open_cb = "",       -- already registered by the menu
--    menuclose_cb = "",  -- already registered by the menu
  }
} 

function ctrl.createElement(class, param)
  return iup.Submenu(param.title)
end
   
iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iupWidget")
