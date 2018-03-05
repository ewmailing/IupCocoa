/******************************************************************************
 * Automatically generated file. Please don't change anything.                *
 *****************************************************************************/

#include <stdlib.h>

#include <lua.h>
#include <lauxlib.h>

#include "iup.h"
#include "iuplua.h"
#include "iupgl.h"
#include "il.h"


static int glbackgroundbox_action(Ihandle *self, float p0, float p1)
{
  lua_State *L = iuplua_call_start(self, "action");
  lua_pushnumber(L, p0);
  lua_pushnumber(L, p1);
  return iuplua_call(L, 2);
}

static int GLBackgroundBox(lua_State *L)
{
  Ihandle *ih = IupGLBackgroundBox(iuplua_checkihandleornil(L, 1));
  iuplua_plugstate(L, ih);
  iuplua_pushihandle_raw(L, ih);
  return 1;
}

int iupglbackgroundboxlua_open(lua_State * L)
{
  iuplua_register(L, GLBackgroundBox, "GLBackgroundBox");

  iuplua_register_cb(L, "ACTION", (lua_CFunction)glbackgroundbox_action, "glbackgroundbox");

#ifdef IUPLUA_USELOH
#include "glbackgroundbox.loh"
#else
#ifdef IUPLUA_USELH
#include "glbackgroundbox.lh"
#else
  iuplua_dofile(L, "glbackgroundbox.lua");
#endif
#endif

  return 0;
}

