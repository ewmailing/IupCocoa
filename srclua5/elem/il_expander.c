/******************************************************************************
 * Automatically generated file. Please don't change anything.                *
 *****************************************************************************/

#include <stdlib.h>

#include <lua.h>
#include <lauxlib.h>

#include "iup.h"
#include "iuplua.h"
#include "il.h"


static int expander_openclose_cb(Ihandle *self, int p0)
{
  lua_State *L = iuplua_call_start(self, "openclose_cb");
  lua_pushinteger(L, p0);
  return iuplua_call(L, 1);
}

static int expander_action(Ihandle *self)
{
  lua_State *L = iuplua_call_start(self, "action");
  return iuplua_call(L, 0);
}

static int expander_extrabutton_cb(Ihandle *self, int p0, int p1)
{
  lua_State *L = iuplua_call_start(self, "extrabutton_cb");
  lua_pushinteger(L, p0);
  lua_pushinteger(L, p1);
  return iuplua_call(L, 2);
}

static int Expander(lua_State *L)
{
  Ihandle *ih = IupExpander(iuplua_checkihandleornil(L, 1));
  iuplua_plugstate(L, ih);
  iuplua_pushihandle_raw(L, ih);
  return 1;
}

int iupexpanderlua_open(lua_State * L)
{
  iuplua_register(L, Expander, "Expander");

  iuplua_register_cb(L, "OPENCLOSE_CB", (lua_CFunction)expander_openclose_cb, NULL);
  iuplua_register_cb(L, "ACTION", (lua_CFunction)expander_action, "expander");
  iuplua_register_cb(L, "EXTRABUTTON_CB", (lua_CFunction)expander_extrabutton_cb, NULL);

#ifdef IUPLUA_USELOH
#include "expander.loh"
#else
#ifdef IUPLUA_USELH
#include "expander.lh"
#else
  iuplua_dofile(L, "expander.lua");
#endif
#endif

  return 0;
}

