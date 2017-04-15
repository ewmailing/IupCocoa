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


static int gllink_action(Ihandle *self, char * p0)
{
  lua_State *L = iuplua_call_start(self, "action");
  lua_pushstring(L, p0);
  return iuplua_call(L, 1);
}

static int GLLink(lua_State *L)
{
  Ihandle *ih = IupGLLink((char *)luaL_optstring(L, 1, NULL), (char *)luaL_optstring(L, 2, NULL));
  iuplua_plugstate(L, ih);
  iuplua_pushihandle_raw(L, ih);
  return 1;
}

int iupgllinklua_open(lua_State * L)
{
  iuplua_register(L, GLLink, "GLLink");

  iuplua_register_cb(L, "ACTION", (lua_CFunction)gllink_action, "gllink");

#ifdef IUPLUA_USELOH
#include "gllink.loh"
#else
#ifdef IUPLUA_USELH
#include "gllink.lh"
#else
  iuplua_dofile(L, "gllink.lua");
#endif
#endif

  return 0;
}

