------------------------------------------------------------------------------
-- FontDlg class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "fontdlg",
  parent = iup.WIDGET,
  subdir = "elem",
  creation = "",
  funcname = "FontDlg",
  callback = {}
} 

function ctrl.popup(ih, x, y)
  iup.Popup(ih,x,y)
end

function ctrl.createElement(class, param)
   return iup.FontDlg()
end
   
iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iupWidget")

