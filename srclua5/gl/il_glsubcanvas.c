/******************************************************************************
 * Automatically generated file. Please don't change anything.                *
 *****************************************************************************/

#include <stdlib.h>

#include <lua.h>
#include <lauxlib.h>

#include "iup.h"
#include "iuplua.h"
#include "iupglcontrols.h"
#include "il.h"


static int glsubcanvas_gl_action(Ihandle *self)
{
  lua_State *L = iuplua_call_start(self, "gl_action");
  return iuplua_call(L, 0);
}

static int glsubcanvas_gl_wheel_cb(Ihandle *self, float p0, int p1, int p2, char * p3)
{
  lua_State *L = iuplua_call_start(self, "gl_wheel_cb");
  lua_pushnumber(L, p0);
  lua_pushinteger(L, p1);
  lua_pushinteger(L, p2);
  lua_pushstring(L, p3);
  return iuplua_call(L, 4);
}

static int glsubcanvas_gl_button_cb(Ihandle *self, int p0, int p1, int p2, int p3, char * p4)
{
  lua_State *L = iuplua_call_start(self, "gl_button_cb");
  lua_pushinteger(L, p0);
  lua_pushinteger(L, p1);
  lua_pushinteger(L, p2);
  lua_pushinteger(L, p3);
  lua_pushstring(L, p4);
  return iuplua_call(L, 5);
}

static int glsubcanvas_gl_enterwindow_cb(Ihandle *self)
{
  lua_State *L = iuplua_call_start(self, "gl_enterwindow_cb");
  return iuplua_call(L, 0);
}

static int glsubcanvas_gl_leavewindow_cb(Ihandle *self)
{
  lua_State *L = iuplua_call_start(self, "gl_leavewindow_cb");
  return iuplua_call(L, 0);
}

static int glsubcanvas_gl_motion_cb(Ihandle *self, int p0, int p1, char * p2)
{
  lua_State *L = iuplua_call_start(self, "gl_motion_cb");
  lua_pushinteger(L, p0);
  lua_pushinteger(L, p1);
  lua_pushstring(L, p2);
  return iuplua_call(L, 3);
}

static int GLSubCanvas(lua_State *L)
{
  Ihandle *ih = IupGLSubCanvas();
  iuplua_plugstate(L, ih);
  iuplua_pushihandle_raw(L, ih);
  return 1;
}

int iupglsubcanvaslua_open(lua_State * L)
{
  iuplua_register(L, GLSubCanvas, "GLSubCanvas");

  iuplua_register_cb(L, "GL_ACTION", (lua_CFunction)glsubcanvas_gl_action, NULL);
  iuplua_register_cb(L, "GL_WHEEL_CB", (lua_CFunction)glsubcanvas_gl_wheel_cb, NULL);
  iuplua_register_cb(L, "GL_BUTTON_CB", (lua_CFunction)glsubcanvas_gl_button_cb, NULL);
  iuplua_register_cb(L, "GL_ENTERWINDOW_CB", (lua_CFunction)glsubcanvas_gl_enterwindow_cb, NULL);
  iuplua_register_cb(L, "GL_LEAVEWINDOW_CB", (lua_CFunction)glsubcanvas_gl_leavewindow_cb, NULL);
  iuplua_register_cb(L, "GL_MOTION_CB", (lua_CFunction)glsubcanvas_gl_motion_cb, NULL);

#ifdef IUPLUA_USELOH
#include "glsubcanvas.loh"
#else
#ifdef IUPLUA_USELH
#include "glsubcanvas.lh"
#else
  iuplua_dofile(L, "glsubcanvas.lua");
#endif
#endif

  return 0;
}

