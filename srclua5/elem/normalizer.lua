
------------------------------------------------------------------------------
-- Normalizer class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "normalizer",
  parent = iup.WIDGET,
  subdir = "elem",
  creation = "-",
  callback = {}
}

-- mimics the iup.BOX.setAttributes, but without using iup.Append
function ctrl.setAttributes(widget, param)
  local ih = widget.ihandle
  local n = #param
  for i = 1, n do
    if iup.GetClass(param[i]) == "iupHandle" then 
      ih.addcontrol = param[i]
    end
  end
  iup.WIDGET.setAttributes(widget, param)
end

function ctrl.createElement(class, param)
   return iup.Normalizer()
end

iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iupWidget")
