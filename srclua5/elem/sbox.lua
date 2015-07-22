------------------------------------------------------------------------------
-- Sbox class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "sbox",
  parent = iup.BOX,
  subdir = "elem",
  creation = "I",
  callback = {}
}

function ctrl.createElement(class, param)
   return iup.Sbox()
end

iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iupWidget")
