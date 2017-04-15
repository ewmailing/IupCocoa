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


static int GLSeparator(lua_State *L)
{
  Ihandle *ih = IupGLSeparator();
  iuplua_plugstate(L, ih);
  iuplua_pushihandle_raw(L, ih);
  return 1;
}

int iupglseparatorlua_open(lua_State * L)
{
  iuplua_register(L, GLSeparator, "GLSeparator");


#ifdef IUPLUA_USELOH
#include "glseparator.loh"
#else
#ifdef IUPLUA_USELH
#include "glseparator.lh"
#else
  iuplua_dofile(L, "glseparator.lua");
#endif
#endif

  return 0;
}

