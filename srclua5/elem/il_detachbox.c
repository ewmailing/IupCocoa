/******************************************************************************
 * Automatically generated file. Please don't change anything.                *
 *****************************************************************************/

#include <stdlib.h>

#include <lua.h>
#include <lauxlib.h>

#include "iup.h"
#include "iuplua.h"
#include "il.h"


static int detachbox_detached_cb(Ihandle *self, Ihandle * p0, int p1, int p2)
{
  lua_State *L = iuplua_call_start(self, "detached_cb");
  iuplua_pushihandle(L, p0);
  lua_pushinteger(L, p1);
  lua_pushinteger(L, p2);
  return iuplua_call(L, 3);
}

static int DetachBox(lua_State *L)
{
  Ihandle *ih = IupDetachBox(iuplua_checkihandleornil(L, 1));
  iuplua_plugstate(L, ih);
  iuplua_pushihandle_raw(L, ih);
  return 1;
}

int iupdetachboxlua_open(lua_State * L)
{
  iuplua_register(L, DetachBox, "DetachBox");

  iuplua_register_cb(L, "DETACHED_CB", (lua_CFunction)detachbox_detached_cb, NULL);

#ifdef IUPLUA_USELOH
#include "detachbox.loh"
#else
#ifdef IUPLUA_USELH
#include "detachbox.lh"
#else
  iuplua_dofile(L, "detachbox.lua");
#endif
#endif

  return 0;
}

