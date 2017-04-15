------------------------------------------------------------------------------
-- SpinBox class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "spinbox",
  parent = iup.BOX,
  subdir = "elem",
  creation = "I",
  callback = {
    spin_cb = "n",
  },
}

function ctrl.createElement(class, param)
   return iup.Spinbox()
end

iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iupWidget")
