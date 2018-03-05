/******************************************************************************
 * Automatically generated file. Please don't change anything.                *
 *****************************************************************************/

#include <stdlib.h>

#include <lua.h>
#include <lauxlib.h>

#include "iup.h"
#include "iuplua.h"
#include "il.h"


static int link_action(Ihandle *self, char * p0)
{
  lua_State *L = iuplua_call_start(self, "action");
  lua_pushstring(L, p0);
  return iuplua_call(L, 1);
}

static int Link(lua_State *L)
{
  Ihandle *ih = IupLink((char *)luaL_optstring(L, 1, NULL), (char *)luaL_optstring(L, 2, NULL));
  iuplua_plugstate(L, ih);
  iuplua_pushihandle_raw(L, ih);
  return 1;
}

int iuplinklua_open(lua_State * L)
{
  iuplua_register(L, Link, "Link");

  iuplua_register_cb(L, "ACTION", (lua_CFunction)link_action, "link");

#ifdef IUPLUA_USELOH
#include "link.loh"
#else
#ifdef IUPLUA_USELH
#include "link.lh"
#else
  iuplua_dofile(L, "link.lua");
#endif
#endif

  return 0;
}

