/******************************************************************************
 * Automatically generated file. Please don't change anything.                *
 *****************************************************************************/

#include <stdlib.h>

#include <lua.h>
#include <lauxlib.h>

#include "iup.h"
#include "iuplua.h"
#include "il.h"


static int ScrollBox(lua_State *L)
{
  Ihandle *ih = IupScrollBox(iuplua_checkihandleornil(L, 1));
  iuplua_plugstate(L, ih);
  iuplua_pushihandle_raw(L, ih);
  return 1;
}

int iupscrollboxlua_open(lua_State * L)
{
  iuplua_register(L, ScrollBox, "ScrollBox");


#ifdef IUPLUA_USELOH
#include "scrollbox.loh"
#else
#ifdef IUPLUA_USELH
#include "scrollbox.lh"
#else
  iuplua_dofile(L, "scrollbox.lua");
#endif
#endif

  return 0;
}

