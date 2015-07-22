------------------------------------------------------------------------------
-- BackgroundBox class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "backgroundbox",
  parent = iup.BOX,
  subdir = "elem",
  creation = "I",
  funcname = "BackgroundBox",
  callback = {
    action = "ff",
    }
}

function ctrl.createElement(class, param)
   return iup.BackgroundBox()
end

iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iupWidget")
