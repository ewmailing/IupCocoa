/******************************************************************************
 * Automatically generated file. Please don't change anything.                *
 *****************************************************************************/

#include <stdlib.h>

#include <lua.h>
#include <lauxlib.h>

#include "iup.h"
#include "iuplua.h"
#include "il.h"


static int FlatFrame(lua_State *L)
{
  Ihandle *ih = IupFlatFrame(iuplua_checkihandleornil(L, 1));
  iuplua_plugstate(L, ih);
  iuplua_pushihandle_raw(L, ih);
  return 1;
}

int iupflatframelua_open(lua_State * L)
{
  iuplua_register(L, FlatFrame, "FlatFrame");


#ifdef IUPLUA_USELOH
#include "flatframe.loh"
#else
#ifdef IUPLUA_USELH
#include "flatframe.lh"
#else
  iuplua_dofile(L, "flatframe.lua");
#endif
#endif

  return 0;
}

