------------------------------------------------------------------------------
-- FlatButton class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "flatbutton",
  parent = iup.WIDGET,
  subdir = "elem",
  creation = "S",
  funcname = "FlatButton",
  callback = {
    flat_action = "", 
    flat_button_cb = "nnnns",
    flat_enterwindow_cb = "",
    flat_leavewindow_cb = "",
    flat_focus_cb = "n",
  }
} 

function ctrl.createElement(class, param)
  return iup.FlatButton(param.title)
end

iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iupWidget")
