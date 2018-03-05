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


static int GLFrame(lua_State *L)
{
  Ihandle *ih = IupGLFrame(iuplua_checkihandleornil(L, 1));
  iuplua_plugstate(L, ih);
  iuplua_pushihandle_raw(L, ih);
  return 1;
}

int iupglframelua_open(lua_State * L)
{
  iuplua_register(L, GLFrame, "GLFrame");


#ifdef IUPLUA_USELOH
#include "glframe.loh"
#else
#ifdef IUPLUA_USELH
#include "glframe.lh"
#else
  iuplua_dofile(L, "glframe.lua");
#endif
#endif

  return 0;
}

