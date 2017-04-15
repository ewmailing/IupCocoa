
------------------------------------------------------------------------------
-- User class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "user",
  parent = iup.BOX,
  subdir = "elem",
  creation = "",
  callback = {}
}

function ctrl.createElement(class, param)
  return iup.User()
end

iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iupWidget")
