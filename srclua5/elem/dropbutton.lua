------------------------------------------------------------------------------
-- DropButton class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "dropbutton",
  parent = iup.WIDGET,
  subdir = "elem",
  creation = "I",
  funcname = "DropButton",
  callback = {
    flat_action = "", 
    flat_button_cb = "nnnns",
    flat_motion_cb = "nns",
    flat_enterwindow_cb = "",
    flat_leavewindow_cb = "",
    flat_focus_cb = "n",
    dropdown_cb = "n",
    dropshow_cb = "n",
  }
} 

function ctrl.createElement(class, param)
  return iup.DropButton(param[0]) -- not a BOX
end

iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iupWidget")
