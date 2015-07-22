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


static int glcanvasbox_action(Ihandle *self, float p0, float p1)
{
  lua_State *L = iuplua_call_start(self, "action");
  lua_pushnumber(L, p0);
  lua_pushnumber(L, p1);
  return iuplua_call(L, 2);
}

static int glcanvasbox_swapbuffers_cb(Ihandle *self)
{
  lua_State *L = iuplua_call_start(self, "swapbuffers_cb");
  return iuplua_call(L, 0);
}

static int GLCanvasBox(lua_State *L)
{
  Ihandle *ih = IupGLCanvasBox(NULL);
  iuplua_plugstate(L, ih);
  iuplua_pushihandle_raw(L, ih);
  return 1;
}

int iupglcanvasboxlua_open(lua_State * L)
{
  iuplua_register(L, GLCanvasBox, "GLCanvasBox");

  iuplua_register_cb(L, "ACTION", (lua_CFunction)glcanvasbox_action, "glcanvasbox");
  iuplua_register_cb(L, "SWAPBUFFERS_CB", (lua_CFunction)glcanvasbox_swapbuffers_cb, NULL);

#ifdef IUPLUA_USELOH
#include "glcanvasbox.loh"
#else
#ifdef IUPLUA_USELH
#include "glcanvasbox.lh"
#else
  iuplua_dofile(L, "glcanvasbox.lua");
#endif
#endif

  return 0;
}

