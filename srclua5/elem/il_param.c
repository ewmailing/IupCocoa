/******************************************************************************
 * Automatically generated file. Please don't change anything.                *
 *****************************************************************************/

#include <stdlib.h>

#include <lua.h>
#include <lauxlib.h>

#include "iup.h"
#include "iuplua.h"
#include "il.h"


static int Param(lua_State *L)
{
  Ihandle *ih = IupParam((char *)luaL_checkstring(L, 1));
  iuplua_plugstate(L, ih);
  iuplua_pushihandle_raw(L, ih);
  return 1;
}

int iupparamlua_open(lua_State * L)
{
  iuplua_register(L, Param, "Param");


#ifdef IUPLUA_USELOH
#include "param.loh"
#else
#ifdef IUPLUA_USELH
#include "param.lh"
#else
  iuplua_dofile(L, "param.lua");
#endif
#endif

  return 0;
}

