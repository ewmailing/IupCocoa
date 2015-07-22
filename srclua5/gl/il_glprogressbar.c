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


static int GLProgressBar(lua_State *L)
{
  Ihandle *ih = IupGLProgressBar();
  iuplua_plugstate(L, ih);
  iuplua_pushihandle_raw(L, ih);
  return 1;
}

int iupglprogressbarlua_open(lua_State * L)
{
  iuplua_register(L, GLProgressBar, "GLProgressBar");


#ifdef IUPLUA_USELOH
#include "glprogressbar.loh"
#else
#ifdef IUPLUA_USELH
#include "glprogressbar.lh"
#else
  iuplua_dofile(L, "glprogressbar.lua");
#endif
#endif

  return 0;
}

