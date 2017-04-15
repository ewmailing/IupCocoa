/******************************************************************************
 * Automatically generated file. Please don't change anything.                *
 *****************************************************************************/

#include <stdlib.h>

#include <lua.h>
#include <lauxlib.h>

#include "iup.h"
#include "iuplua.h"
#include "il.h"


static int GridBox(lua_State *L)
{
  Ihandle *ih = IupGridBox(NULL);
  iuplua_plugstate(L, ih);
  iuplua_pushihandle_raw(L, ih);
  return 1;
}

int iupgridboxlua_open(lua_State * L)
{
  iuplua_register(L, GridBox, "GridBox");


#ifdef IUPLUA_USELOH
#include "gridbox.loh"
#else
#ifdef IUPLUA_USELH
#include "gridbox.lh"
#else
  iuplua_dofile(L, "gridbox.lua");
#endif
#endif

  return 0;
}

