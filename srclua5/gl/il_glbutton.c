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


static int glbutton_action(Ihandle *self)
{
  lua_State *L = iuplua_call_start(self, "action");
  return iuplua_call(L, 0);
}

static int GLButton(lua_State *L)
{
  Ihandle *ih = IupGLButton((char *)luaL_optstring(L, 1, NULL));
  iuplua_plugstate(L, ih);
  iuplua_pushihandle_raw(L, ih);
  return 1;
}

int iupglbuttonlua_open(lua_State * L)
{
  iuplua_register(L, GLButton, "GLButton");

  iuplua_register_cb(L, "ACTION", (lua_CFunction)glbutton_action, "glbutton");

#ifdef IUPLUA_USELOH
#include "glbutton.loh"
#else
#ifdef IUPLUA_USELH
#include "glbutton.lh"
#else
  iuplua_dofile(L, "glbutton.lua");
#endif
#endif

  return 0;
}

