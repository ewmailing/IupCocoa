/******************************************************************************
 * Automatically generated file. Please don't change anything.                *
 *****************************************************************************/

#include <stdlib.h>

#include <lua.h>
#include <lauxlib.h>

#include "iup.h"
#include "iuplua.h"
#include "il.h"


static int FlatLabel(lua_State *L)
{
  Ihandle *ih = IupFlatLabel((char *)luaL_optstring(L, 1, NULL));
  iuplua_plugstate(L, ih);
  iuplua_pushihandle_raw(L, ih);
  return 1;
}

int iupflatlabellua_open(lua_State * L)
{
  iuplua_register(L, FlatLabel, "FlatLabel");


#ifdef IUPLUA_USELOH
#include "flatlabel.loh"
#else
#ifdef IUPLUA_USELH
#include "flatlabel.lh"
#else
  iuplua_dofile(L, "flatlabel.lua");
#endif
#endif

  return 0;
}

