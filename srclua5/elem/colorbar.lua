------------------------------------------------------------------------------
-- Colorbar class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "colorbar",
  parent = iup.WIDGET,
  creation = "",
  subdir = "elem",
  callback = {
    select_cb = "nn",
    cell_cb = {"n", ret = "s"},
    switch_cb = "nn",
    extended_cb = "n",
   },
  funcname = "Colorbar",
}

iup.PRIMARY = -1
iup.SECONDARY = -2

function ctrl.createElement(class, param)
   return iup.Colorbar(param.action)
end

iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iupWidget")
