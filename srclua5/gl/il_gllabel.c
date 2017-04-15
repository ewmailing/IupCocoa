/******************************************************************************
 * Automatically generated file. Please don't change anything.                *
 *****************************************************************************/

#include <stdlib.h>

#include <lua.h>
#include <lauxlib.h>

#include "iup.h"
#include "iuplua.h"
#include "iupglcontrols.h"
#include "il.h"


static int GLLabel(lua_State *L)
{
  Ihandle *ih = IupGLLabel((char *)luaL_optstring(L, 1, NULL));
  iuplua_plugstate(L, ih);
  iuplua_pushihandle_raw(L, ih);
  return 1;
}

int iupgllabellua_open(lua_State * L)
{
  iuplua_register(L, GLLabel, "GLLabel");


#ifdef IUPLUA_USELOH
#include "gllabel.loh"
#else
#ifdef IUPLUA_USELH
#include "gllabel.lh"
#else
  iuplua_dofile(L, "gllabel.lua");
#endif
#endif

  return 0;
}

