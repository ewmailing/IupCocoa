------------------------------------------------------------------------------
-- ColorDlg class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "colordlg",
  parent = iup.WIDGET,
  subdir = "elem",
  creation = "",
  funcname = "ColorDlg",
  callback = {
    colorupdate_cb = "",
  }
} 

function ctrl.popup(ih, x, y)
  iup.Popup(ih,x,y)
end

function ctrl.createElement(class, param)
   return iup.ColorDlg()
end
   
iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iupWidget")

