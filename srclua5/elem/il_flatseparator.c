/******************************************************************************
 * Automatically generated file. Please don't change anything.                *
 *****************************************************************************/

#include <stdlib.h>

#include <lua.h>
#include <lauxlib.h>

#include "iup.h"
#include "iuplua.h"
#include "il.h"


static int FlatSeparator(lua_State *L)
{
  Ihandle *ih = IupFlatSeparator();
  iuplua_plugstate(L, ih);
  iuplua_pushihandle_raw(L, ih);
  return 1;
}

int iupflatseparatorlua_open(lua_State * L)
{
  iuplua_register(L, FlatSeparator, "FlatSeparator");


#ifdef IUPLUA_USELOH
#include "flatseparator.loh"
#else
#ifdef IUPLUA_USELH
#include "flatseparator.lh"
#else
  iuplua_dofile(L, "flatseparator.lua");
#endif
#endif

  return 0;
}

