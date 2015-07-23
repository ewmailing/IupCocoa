------------------------------------------------------------------------------
-- DetachBox class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "detachbox",
  parent = iup.BOX,
  subdir = "elem",
  creation = "I",
  funcname = "DetachBox",
  callback = {
    detached_cb = "inn",
  }
}

function ctrl.createElement(class, param)
  return iup.DetachBox()
end

iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iupWidget")
