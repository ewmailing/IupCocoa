/** \file
 * \brief IULUA core - Bindig of iup to Lua 3.
 *
 * See Copyright Notice in "iup.h"
 */
 
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "iup.h"
#include "iupim.h"

#include <lua.h>
#include <lauxlib.h>

#include "iuplua.h"
#include "iupluaim.h"
#include "il.h"

static void SaveImage(void)
{
  Ihandle* image = iuplua_checkihandle(1);
  char *filename = luaL_check_string(2);
  char *format = luaL_check_string(3);
  lua_pushnumber(IupSaveImage(image, filename, format));
}

static void LoadImage(void)
{
  char *filename = luaL_check_string(1);
  iuplua_pushihandle(IupLoadImage(filename));
}

void iupimlua_open(void)
{
  iuplua_register("IupLoadImage", LoadImage);
  iuplua_register("IupSaveImage", SaveImage);
}
