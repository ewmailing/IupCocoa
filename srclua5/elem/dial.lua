------------------------------------------------------------------------------
-- Dial class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "dial",
  parent = iup.WIDGET,
  creation = "S",
  subdir = "elem",
  callback = {
--     mousemove_cb = "d",       -- already registered by the val
--     button_press_cb = "d",    -- already registered by the val
--     button_release_cb = "d",  -- already registered by the val
  },
}

function ctrl.createElement(class, param)
   return iup.Dial(param[1])
end

iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iupWidget")
