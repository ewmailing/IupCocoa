/******************************************************************************
 * Automatically generated file. Please don't change anything.                *
 *****************************************************************************/

#include <stdlib.h>

#include <lua.h>
#include <lauxlib.h>

#include "iup.h"
#include "iuplua.h"
#include "il.h"


static int Dial(lua_State *L)
{
  Ihandle *ih = IupDial((char *)luaL_optstring(L, 1, NULL));
  iuplua_plugstate(L, ih);
  iuplua_pushihandle_raw(L, ih);
  return 1;
}

int iupdiallua_open(lua_State * L)
{
  iuplua_register(L, Dial, "Dial");


#ifdef IUPLUA_USELOH
#include "dial.loh"
#else
#ifdef IUPLUA_USELH
#include "dial.lh"
#else
  iuplua_dofile(L, "dial.lua");
#endif
#endif

  return 0;
}

