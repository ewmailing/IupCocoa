/** \file
 * \brief Bindig of iupgc to Lua 3.
 *
 * See Copyright Notice in "iup.h"
 */
 
#include <stdlib.h>

#include "iup.h"

#include <lua.h>
#include <lauxlib.h>

#include "iuplua.h"
#include "il.h"
#include "il_controls.h"

static void GetColor(void)
{
  int x = luaL_check_int(1);
  int y = luaL_check_int(2);
  unsigned char r = (unsigned char)luaL_check_int(3);
  unsigned char g = (unsigned char)luaL_check_int(4);
  unsigned char b = (unsigned char)luaL_check_int(5);

  if (IupGetColor(x, y, &r, &g, &b ))
  {
    lua_pushnumber(r);
    lua_pushnumber(g);
    lua_pushnumber(b);
  }
  else
  {
    lua_pushnil();
  }
}

int gclua_open(void)
{
  iuplua_register("IupGetColor", GetColor);
  return 1;
}
