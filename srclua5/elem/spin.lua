------------------------------------------------------------------------------
-- Spin class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "spin",
  parent = iup.WIDGET,
  subdir = "elem",
  creation = "",
  callback = {
    spin_cb = "n",
  },
}

function ctrl.createElement(class, param)
   return iup.Spin(param.action)
end

iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iupWidget")
