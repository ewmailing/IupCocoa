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


static int gltoggle_action(Ihandle *self, int p0)
{
  lua_State *L = iuplua_call_start(self, "action");
  lua_pushinteger(L, p0);
  return iuplua_call(L, 1);
}

static int GLToggle(lua_State *L)
{
  Ihandle *ih = IupGLToggle((char *)luaL_optstring(L, 1, NULL));
  iuplua_plugstate(L, ih);
  iuplua_pushihandle_raw(L, ih);
  return 1;
}

int iupgltogglelua_open(lua_State * L)
{
  iuplua_register(L, GLToggle, "GLToggle");

  iuplua_register_cb(L, "ACTION", (lua_CFunction)gltoggle_action, "gltoggle");

#ifdef IUPLUA_USELOH
#include "gltoggle.loh"
#else
#ifdef IUPLUA_USELH
#include "gltoggle.lh"
#else
  iuplua_dofile(L, "gltoggle.lua");
#endif
#endif

  return 0;
}

