/******************************************************************************
 * Automatically generated file. Please don't change anything.                *
 *****************************************************************************/

#include <stdlib.h>

#include <lua.h>
#include <lauxlib.h>

#include "iup.h"
#include "iuplua.h"
#include "il.h"


static int FlatScrollBox(lua_State *L)
{
  Ihandle *ih = IupFlatScrollBox(iuplua_checkihandleornil(L, 1));
  iuplua_plugstate(L, ih);
  iuplua_pushihandle_raw(L, ih);
  return 1;
}

int iupflatscrollboxlua_open(lua_State * L)
{
  iuplua_register(L, FlatScrollBox, "FlatScrollBox");


#ifdef IUPLUA_USELOH
#include "flatscrollbox.loh"
#else
#ifdef IUPLUA_USELH
#include "flatscrollbox.lh"
#else
  iuplua_dofile(L, "flatscrollbox.lua");
#endif
#endif

  return 0;
}

