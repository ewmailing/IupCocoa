/******************************************************************************
 * Automatically generated file. Please don't change anything.                *
 *****************************************************************************/

#include <stdlib.h>

#include <lua.h>
#include <lauxlib.h>

#include "iup.h"
#include "iuplua.h"
#include "il.h"


static int parambox_param_cb(Ihandle *self, int p0, void* p1)
{
  lua_State *L = iuplua_call_start(self, "param_cb");
  lua_pushinteger(L, p0);
  lua_pushlightuserdata(L, p1);
  return iuplua_call(L, 2);
}

static int ParamBoxv(lua_State *L)
{
  Ihandle *ih = IupParamBoxv(iuplua_checkihandle_array(L, 1,0));
  iuplua_plugstate(L, ih);
  iuplua_pushihandle_raw(L, ih);
  return 1;
}

int iupparamboxlua_open(lua_State * L)
{
  iuplua_register(L, ParamBoxv, "ParamBoxv");

  iuplua_register_cb(L, "PARAM_CB", (lua_CFunction)parambox_param_cb, NULL);

#ifdef IUPLUA_USELOH
#include "parambox.loh"
#else
#ifdef IUPLUA_USELH
#include "parambox.lh"
#else
  iuplua_dofile(L, "parambox.lua");
#endif
#endif

  return 0;
}

