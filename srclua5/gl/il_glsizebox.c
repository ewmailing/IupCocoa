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


static int GLSizeBox(lua_State *L)
{
  Ihandle *ih = IupGLSizeBox(iuplua_checkihandleornil(L, 1));
  iuplua_plugstate(L, ih);
  iuplua_pushihandle_raw(L, ih);
  return 1;
}

int iupglsizeboxlua_open(lua_State * L)
{
  iuplua_register(L, GLSizeBox, "GLSizeBox");


#ifdef IUPLUA_USELOH
#include "glsizebox.loh"
#else
#ifdef IUPLUA_USELH
#include "glsizebox.lh"
#else
  iuplua_dofile(L, "glsizebox.lua");
#endif
#endif

  return 0;
}

