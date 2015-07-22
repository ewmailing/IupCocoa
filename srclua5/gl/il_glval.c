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


static int glval_valuechanging_cb(Ihandle *self, int p0)
{
  lua_State *L = iuplua_call_start(self, "valuechanging_cb");
  lua_pushinteger(L, p0);
  return iuplua_call(L, 1);
}

static int GLVal(lua_State *L)
{
  Ihandle *ih = IupGLVal();
  iuplua_plugstate(L, ih);
  iuplua_pushihandle_raw(L, ih);
  return 1;
}

int iupglvallua_open(lua_State * L)
{
  iuplua_register(L, GLVal, "GLVal");

  iuplua_register_cb(L, "VALUECHANGING_CB", (lua_CFunction)glval_valuechanging_cb, NULL);

#ifdef IUPLUA_USELOH
#include "glval.loh"
#else
#ifdef IUPLUA_USELH
#include "glval.lh"
#else
  iuplua_dofile(L, "glval.lua");
#endif
#endif

  return 0;
}

