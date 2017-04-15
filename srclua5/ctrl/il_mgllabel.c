/******************************************************************************
 * Automatically generated file. Please don't change anything.                *
 *****************************************************************************/

#include <stdlib.h>

#include <lua.h>
#include <lauxlib.h>

#include "iup.h"
#include "iuplua.h"
#include "iup_mglplot.h"
#include "il.h"


static int MglLabel(lua_State *L)
{
  Ihandle *ih = IupMglLabel((char *)luaL_optstring(L, 1, NULL));
  iuplua_plugstate(L, ih);
  iuplua_pushihandle_raw(L, ih);
  return 1;
}

int iupmgllabellua_open(lua_State * L)
{
  iuplua_register(L, MglLabel, "MglLabel");


#ifdef IUPLUA_USELOH
#include "mgllabel.loh"
#else
#ifdef IUPLUA_USELH
#include "mgllabel.lh"
#else
  iuplua_dofile(L, "mgllabel.lua");
#endif
#endif

  return 0;
}

