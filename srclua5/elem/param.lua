
------------------------------------------------------------------------------
-- Param class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "param",
  parent = iup.WIDGET,
  subdir = "elem",
  creation = "s",
  callback = {}
}

function ctrl.createElement(class, param)
  return iup.Param(param.format)
end

function ctrl.GetParamHandle(ih, name)
  return iup.GetParamHandle(ih, name)
end

iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iupWidget")
