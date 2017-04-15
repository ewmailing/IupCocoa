/******************************************************************************
 * Automatically generated file. Please don't change anything.                *
 *****************************************************************************/

#include <stdlib.h>

#include <lua.h>
#include <lauxlib.h>

#include "iup.h"
#include "iuplua.h"
#include "iupmatrixex.h"
#include "il.h"


static int matrixex_numericsetvalue_cb(Ihandle *self, int p0, int p1, double p2)
{
  lua_State *L = iuplua_call_start(self, "numericsetvalue_cb");
  lua_pushinteger(L, p0);
  lua_pushinteger(L, p1);
  lua_pushnumber(L, p2);
  return iuplua_call(L, 3);
}

static int matrixex_menucontextclose_cb(Ihandle *self, Ihandle * p0, int p1, int p2)
{
  lua_State *L = iuplua_call_start(self, "menucontextclose_cb");
  iuplua_pushihandle(L, p0);
  lua_pushinteger(L, p1);
  lua_pushinteger(L, p2);
  return iuplua_call(L, 3);
}

static int matrixex_pastesize_cb(Ihandle *self, int p0, int p1)
{
  lua_State *L = iuplua_call_start(self, "pastesize_cb");
  lua_pushinteger(L, p0);
  lua_pushinteger(L, p1);
  return iuplua_call(L, 2);
}

static int matrixex_busy_cb(Ihandle *self, int p0, int p1, char * p2)
{
  lua_State *L = iuplua_call_start(self, "busy_cb");
  lua_pushinteger(L, p0);
  lua_pushinteger(L, p1);
  lua_pushstring(L, p2);
  return iuplua_call(L, 3);
}

static int matrixex_menucontext_cb(Ihandle *self, Ihandle * p0, int p1, int p2)
{
  lua_State *L = iuplua_call_start(self, "menucontext_cb");
  iuplua_pushihandle(L, p0);
  lua_pushinteger(L, p1);
  lua_pushinteger(L, p2);
  return iuplua_call(L, 3);
}

static int matrixex_sortcolumncompare_cb(Ihandle *self, int p0, int p1, int p2)
{
  lua_State *L = iuplua_call_start(self, "sortcolumncompare_cb");
  lua_pushinteger(L, p0);
  lua_pushinteger(L, p1);
  lua_pushinteger(L, p2);
  return iuplua_call(L, 3);
}

static double matrixex_numericgetvalue_cb(Ihandle *self, int p0, int p1)
{
  lua_State *L = iuplua_call_start(self, "numericgetvalue_cb");
  lua_pushinteger(L, p0);
  lua_pushinteger(L, p1);
  return iuplua_call_ret_d(L, 2);
}

static int MatrixEx(lua_State *L)
{
  Ihandle *ih = IupMatrixEx();
  iuplua_plugstate(L, ih);
  iuplua_pushihandle_raw(L, ih);
  return 1;
}

void iuplua_matrixexfuncs_open(lua_State *L);

int iup_matrixexlua_open(lua_State * L)
{
  iuplua_register(L, MatrixEx, "MatrixEx");

  iuplua_register_cb(L, "NUMERICSETVALUE_CB", (lua_CFunction)matrixex_numericsetvalue_cb, NULL);
  iuplua_register_cb(L, "MENUCONTEXTCLOSE_CB", (lua_CFunction)matrixex_menucontextclose_cb, NULL);
  iuplua_register_cb(L, "PASTESIZE_CB", (lua_CFunction)matrixex_pastesize_cb, NULL);
  iuplua_register_cb(L, "BUSY_CB", (lua_CFunction)matrixex_busy_cb, NULL);
  iuplua_register_cb(L, "MENUCONTEXT_CB", (lua_CFunction)matrixex_menucontext_cb, NULL);
  iuplua_register_cb(L, "SORTCOLUMNCOMPARE_CB", (lua_CFunction)matrixex_sortcolumncompare_cb, NULL);
  iuplua_register_cb(L, "NUMERICGETVALUE_CB", (lua_CFunction)matrixex_numericgetvalue_cb, NULL);

  iuplua_matrixexfuncs_open(L);

#ifdef IUPLUA_USELOH
#include "matrixex.loh"
#else
#ifdef IUPLUA_USELH
#include "matrixex.lh"
#else
  iuplua_dofile(L, "matrixex.lua");
#endif
#endif

  return 0;
}

