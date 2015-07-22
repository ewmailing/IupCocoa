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


static int glexpander_action(Ihandle *self)
{
  lua_State *L = iuplua_call_start(self, "action");
  return iuplua_call(L, 0);
}

static int GLExpander(lua_State *L)
{
  Ihandle *ih = IupGLExpander(iuplua_checkihandleornil(L, 1));
  iuplua_plugstate(L, ih);
  iuplua_pushihandle_raw(L, ih);
  return 1;
}

int iupglexpanderlua_open(lua_State * L)
{
  iuplua_register(L, GLExpander, "GLExpander");

  iuplua_register_cb(L, "ACTION", (lua_CFunction)glexpander_action, "glexpander");

#ifdef IUPLUA_USELOH
#include "glexpander.loh"
#else
#ifdef IUPLUA_USELH
#include "glexpander.lh"
#else
  iuplua_dofile(L, "glexpander.lua");
#endif
#endif

  return 0;
}

