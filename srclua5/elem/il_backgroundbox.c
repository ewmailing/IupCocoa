/******************************************************************************
 * Automatically generated file. Please don't change anything.                *
 *****************************************************************************/

#include <stdlib.h>

#include <lua.h>
#include <lauxlib.h>

#include "iup.h"
#include "iuplua.h"
#include "il.h"


static int backgroundbox_action(Ihandle *self, float p0, float p1)
{
  lua_State *L = iuplua_call_start(self, "action");
  lua_pushnumber(L, p0);
  lua_pushnumber(L, p1);
  return iuplua_call(L, 2);
}

static int BackgroundBox(lua_State *L)
{
  Ihandle *ih = IupBackgroundBox(iuplua_checkihandleornil(L, 1));
  iuplua_plugstate(L, ih);
  iuplua_pushihandle_raw(L, ih);
  return 1;
}

int iupbackgroundboxlua_open(lua_State * L)
{
  iuplua_register(L, BackgroundBox, "BackgroundBox");

  iuplua_register_cb(L, "ACTION", (lua_CFunction)backgroundbox_action, "backgroundbox");

#ifdef IUPLUA_USELOH
#include "backgroundbox.loh"
#else
#ifdef IUPLUA_USELH
#include "backgroundbox.lh"
#else
  iuplua_dofile(L, "backgroundbox.lua");
#endif
#endif

  return 0;
}

