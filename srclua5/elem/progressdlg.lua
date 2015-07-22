------------------------------------------------------------------------------
-- ProgressDlg class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "progressdlg",
  parent = iup.WIDGET,
  subdir = "elem",
  creation = "",
  callback = {
    cancel_cb = "",
  },
  funcname = "ProgressDlg"
} 

function ctrl.showxy(ih, x, y)
  return iup.ShowXY(ih, x, y)
end

function ctrl.popup(ih, x, y)
  iup.Popup(ih,x,y)
end

function ctrl.createElement(class, param)
   return iup.ProgressDlg()
end
   
iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iupWidget")
