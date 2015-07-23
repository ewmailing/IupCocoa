------------------------------------------------------------------------------
-- SpinBox class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "spinbox",
  parent = iup.BOX,
  subdir = "elem",
  creation = "i",
  callback = {
    spin_cb = "n",
  },
}

function ctrl.createElement(class, param)
   return iup.Spinbox()
end

iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iupWidget")
