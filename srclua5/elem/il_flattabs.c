/******************************************************************************
 * Automatically generated file. Please don't change anything.                *
 *****************************************************************************/

#include <stdlib.h>

#include <lua.h>
#include <lauxlib.h>

#include "iup.h"
#include "iuplua.h"
#include "il.h"


static int flattabs_tabchange_cb(Ihandle *self, Ihandle * p0, Ihandle * p1)
{
  lua_State *L = iuplua_call_start(self, "tabchange_cb");
  iuplua_pushihandle(L, p0);
  iuplua_pushihandle(L, p1);
  return iuplua_call(L, 2);
}

static int flattabs_tabchangepos_cb(Ihandle *self, int p0, int p1)
{
  lua_State *L = iuplua_call_start(self, "tabchangepos_cb");
  lua_pushinteger(L, p0);
  lua_pushinteger(L, p1);
  return iuplua_call(L, 2);
}

static int flattabs_tabclose_cb(Ihandle *self, int p0)
{
  lua_State *L = iuplua_call_start(self, "tabclose_cb");
  lua_pushinteger(L, p0);
  return iuplua_call(L, 1);
}

static int FlatTabs(lua_State *L)
{
  Ihandle *ih = IupFlatTabs(NULL);
  iuplua_plugstate(L, ih);
  iuplua_pushihandle_raw(L, ih);
  return 1;
}

int iupflattabslua_open(lua_State * L)
{
  iuplua_register(L, FlatTabs, "FlatTabs");

  iuplua_register_cb(L, "TABCHANGE_CB", (lua_CFunction)flattabs_tabchange_cb, NULL);
  iuplua_register_cb(L, "TABCHANGEPOS_CB", (lua_CFunction)flattabs_tabchangepos_cb, NULL);
  iuplua_register_cb(L, "TABCLOSE_CB", (lua_CFunction)flattabs_tabclose_cb, NULL);

#ifdef IUPLUA_USELOH
#include "flattabs.loh"
#else
#ifdef IUPLUA_USELH
#include "flattabs.lh"
#else
  iuplua_dofile(L, "flattabs.lua");
#endif
#endif

  return 0;
}

