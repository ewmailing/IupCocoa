/** \file
 * \brief IUP Canvas Lua 5 Binding
 *
 * See Copyright Notice in cd.h
 */

#include <stdlib.h>
#include <stdio.h>

#include <cd.h>
#include <cdiup.h>

#include <lua.h>
#include <lauxlib.h>

#include "iup.h"
#include "iuplua.h"

#include <cdlua.h>
#include <cdluaiup.h>
#include <cdlua5_private.h>


static void *cdiup_checkdata(lua_State *L, int param)
{
  return iuplua_checkihandle(L, param);
}

static cdluaContext cdluaiupdbufferrgbctx =
{
  0,
  "IUPDBUFFERRGB",
  cdContextIupDBufferRGB,
  cdiup_checkdata,
  NULL,
  0
};

static cdluaContext cdluaiupdbufferctx =
{
  0,
  "IUPDBUFFER",
  cdContextIupDBuffer,
  cdiup_checkdata,
  NULL,
  0
};

static cdluaContext cdluaiupctx = 
{
  0,
  "IUP",
  cdContextIup,
  cdiup_checkdata,
  NULL,
  0
};

static const luaL_Reg funcs[] = {
  { NULL, NULL },
  };

int cdluaiup_open (lua_State *L)
{
  cdluaLuaState* cdL = cdlua_getstate(L);
  cdlua_register_lib(L, funcs);  /* leave "cd" table at the top of the stack */
  cdlua_addcontext(L, cdL, &cdluaiupctx);
  cdlua_addcontext(L, cdL, &cdluaiupdbufferctx);
  cdlua_addcontext(L, cdL, &cdluaiupdbufferrgbctx);
  return 1;
}

int luaopen_iupluacd(lua_State* L)
{
  return cdluaiup_open(L);
}

