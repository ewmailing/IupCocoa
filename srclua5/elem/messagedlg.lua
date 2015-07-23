------------------------------------------------------------------------------
-- MessageDlg class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "messagedlg",
  parent = iup.WIDGET,
  subdir = "elem",
  creation = "",
  funcname = "MessageDlg",
  callback = {}
} 

function ctrl.popup(ih, x, y)
  iup.Popup(ih,x,y)
end

function ctrl.createElement(class, param)
   return iup.MessageDlg()
end
   
iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iupWidget")

