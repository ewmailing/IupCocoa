------------------------------------------------------------------------------
-- GLCanvas class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "glcanvas",
  parent = iup.WIDGET,
  creation = "-",
  funcname = "GLCanvas",
  include = "iupgl.h",
  subdir = "ctrl",
  callback = {
    action = "ff",
    swapbuffers_cb = "",
  },
  extrafuncs = 1,
  extracode = [[ 
int iupgllua_open(lua_State * L)
{
  if (iuplua_opencall_internal(L))
    IupGLCanvasOpen();

  iuplua_get_env(L);
  iupglcanvaslua_open(L);
  iupglbackgroundboxlua_open(L);
  return 0;
}

/* obligatory to use require"iupluagl" */
int luaopen_iupluagl(lua_State* L)
{
  return iupgllua_open(L);
}

]]
}

-- Defined in C
ctrl.SwapBuffers = iup.GLSwapBuffers
ctrl.IsCurrent = iup.GLIsCurrent
ctrl.MakeCurrent = iup.GLMakeCurrent
ctrl.Palette = iup.GLPalette
ctrl.UseFont = iup.GLUseFont

function ctrl.createElement(class, param)
   return iup.GLCanvas()
end

iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iupWidget")
