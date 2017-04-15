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


static int GLScrollBox(lua_State *L)
{
  Ihandle *ih = IupGLScrollBox(iuplua_checkihandleornil(L, 1));
  iuplua_plugstate(L, ih);
  iuplua_pushihandle_raw(L, ih);
  return 1;
}

int iupglscrollboxlua_open(lua_State * L)
{
  iuplua_register(L, GLScrollBox, "GLScrollBox");


#ifdef IUPLUA_USELOH
#include "glscrollbox.loh"
#else
#ifdef IUPLUA_USELH
#include "glscrollbox.lh"
#else
  iuplua_dofile(L, "glscrollbox.lua");
#endif
#endif

  return 0;
}

