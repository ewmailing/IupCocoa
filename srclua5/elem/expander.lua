------------------------------------------------------------------------------
-- Expander class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "expander",
  parent = iup.BOX,
  subdir = "elem",
  creation = "I",
  callback = {
    action = "", 
    openclose_cb = "n", 
    extrabutton_cb = "nn", 
  }
}

function ctrl.createElement(class, param)
  return iup.Expander()
end

iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iupWidget")
