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

ctrl.DrawBegin = iup.DrawBegin
ctrl.DrawEnd = iup.DrawEnd
ctrl.DrawParentBackground = iup.DrawParentBackground
ctrl.DrawLine  = iup.DrawLine
ctrl.DrawRectangle = iup.DrawRectangle
ctrl.DrawArc = iup.DrawArc
ctrl.DrawPolygon = iup.DrawPolygon
ctrl.DrawText = iup.DrawText
ctrl.DrawImage = iup.DrawImage
ctrl.DrawSetClipRect = iup.DrawSetClipRect
ctrl.DrawResetClip = iup.DrawResetClip
ctrl.DrawSelectRect = iup.DrawSelectRect
ctrl.DrawFocusRect = iup.DrawFocusRect
ctrl.DrawGetSize = iup.DrawGetSize
ctrl.DrawGetTextSize = iup.DrawGetTextSize
ctrl.DrawGetImageInfo = iup.DrawGetImageInfo

iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iupWidget")
