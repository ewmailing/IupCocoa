------------------------------------------------------------------------------
-- AnimatedLabel class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "animatedlabel",
  parent = iup.WIDGET, -- not a BOX
  subdir = "elem",
  creation = "I",
  funcname = "AnimatedLabel",
  callback = {}
}

function ctrl.createElement(class, param)
   return iup.AnimatedLabel(param[0]) -- not a BOX
end

iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iupWidget")
