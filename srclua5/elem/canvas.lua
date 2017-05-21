------------------------------------------------------------------------------
-- Canvas class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "canvas",
  parent = iup.WIDGET,
  subdir = "elem",
  creation = "-",
  callback = {
    action = "ff",
    button_cb = "nnnns",
    enterwindow_cb = "",
    leavewindow_cb = "",
    motion_cb = "nns",
    resize_cb = "nn",
    scroll_cb = "nff",
    keypress_cb = "nn",
    wom_cb = "n",
    wheel_cb = "fnns",
    mdiactivate_cb = "",
    touch_cb = "nnns",
    focus_cb = "n",
  }
}

function ctrl.createElement(class, param)
   return iup.Canvas()
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
