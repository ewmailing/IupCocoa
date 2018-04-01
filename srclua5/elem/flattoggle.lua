------------------------------------------------------------------------------
-- FlatToggle class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "flattoggle",
  parent = iup.WIDGET,
  subdir = "elem",
  creation = "S",
  funcname = "FlatToggle",
  callback = {
    flat_action = "n", 
    --flat_button_cb = "nnnns",
    --flat_enterwindow_cb = "",
    --flat_leavewindow_cb = "",
    --flat_focus_cb = "n",
  }
} 

function ctrl.createElement(class, param)
  return iup.FlatToggle(param.title)
end

iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iupWidget")
