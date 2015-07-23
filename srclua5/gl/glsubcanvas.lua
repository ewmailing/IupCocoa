------------------------------------------------------------------------------
-- GLSubCanvas class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "glsubcanvas",
  parent = iup.WIDGET,
  creation = "",
  funcname = "GLSubCanvas",
  include = "iupglcontrols.h",
  subdir = "gl",
  callback = {
    gl_action = "",
    gl_button_cb = "nnnns",
    gl_enterwindow_cb = "",
    gl_leavewindow_cb = "",
    gl_motion_cb = "nns",
    gl_wheel_cb = "fnns",
  },
}

function ctrl.createElement(class, param)
   return iup.GLSubCanvas()
end

iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iupWidget")
