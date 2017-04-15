------------------------------------------------------------------------------
-- ParamBox class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "parambox",
  parent = iup.WIDGET,
  subdir = "elem",
  creation = "v",
  funcname = "ParamBoxv",
  callback = {
    param_cb = "no",
  },
}

function ctrl.createElement(class, param)
  return iup.ParamBoxv(param)
end

function ctrl.GetParamParam(ih, index)
  return iup.GetParamParam(ih, index)
end

iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iupWidget")
