/******************************************************************************
 * Automatically generated file. Please don't change anything.                *
 *****************************************************************************/

#include <stdlib.h>

#include <lua.h>
#include <lauxlib.h>

#include "iup.h"
#include "iuplua.h"
#include "il.h"


static int flattoggle_flat_action(Ihandle *self, int p0)
{
  lua_State *L = iuplua_call_start(self, "flat_action");
  lua_pushinteger(L, p0);
  return iuplua_call(L, 1);
}

static int FlatToggle(lua_State *L)
{
  Ihandle *ih = IupFlatToggle((char *)luaL_optstring(L, 1, NULL));
  iuplua_plugstate(L, ih);
  iuplua_pushihandle_raw(L, ih);
  return 1;
}

int iupflattogglelua_open(lua_State * L)
{
  iuplua_register(L, FlatToggle, "FlatToggle");

  iuplua_register_cb(L, "FLAT_ACTION", (lua_CFunction)flattoggle_flat_action, NULL);

#ifdef IUPLUA_USELOH
#include "flattoggle.loh"
#else
#ifdef IUPLUA_USELH
#include "flattoggle.lh"
#else
  iuplua_dofile(L, "flattoggle.lua");
#endif
#endif

  return 0;
}

