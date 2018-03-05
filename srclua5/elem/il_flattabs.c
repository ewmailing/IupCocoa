/******************************************************************************
 * Automatically generated file. Please don't change anything.                *
 *****************************************************************************/

#include <stdlib.h>

#include <lua.h>
#include <lauxlib.h>

#include "iup.h"
#include "iuplua.h"
#include "il.h"


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

