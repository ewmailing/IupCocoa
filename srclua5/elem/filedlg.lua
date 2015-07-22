------------------------------------------------------------------------------
-- FileDlg class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "filedlg",
  parent = iup.WIDGET,
  subdir = "elem",
  creation = "",
  callback = {
    file_cb = "ss",
  },
  funcname = "FileDlg"
} 

function ctrl.popup(ih, x, y)
  iup.Popup(ih,x,y)
end

function ctrl.createElement(class, param)
   return iup.FileDlg()
end
   
iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iupWidget")

