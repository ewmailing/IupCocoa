/******************************************************************************
 * Automatically generated file. Please don't change anything.                *
 *****************************************************************************/

#include <stdlib.h>

#include <lua.h>
#include <lauxlib.h>

#include "iup.h"
#include "iuplua.h"
#include "iupcontrols.h"
#include "il.h"


static int matrixlist_imagevaluechanged_cb(Ihandle *self, int p0, int p1)
{
  lua_State *L = iuplua_call_start(self, "imagevaluechanged_cb");
  lua_pushinteger(L, p0);
  lua_pushinteger(L, p1);
  return iuplua_call(L, 2);
}

static int matrixlist_listrelease_cb(Ihandle *self, int p0, int p1, char * p2)
{
  lua_State *L = iuplua_call_start(self, "listrelease_cb");
  lua_pushinteger(L, p0);
  lua_pushinteger(L, p1);
  lua_pushstring(L, p2);
  return iuplua_call(L, 3);
}

static int matrixlist_listclick_cb(Ihandle *self, int p0, int p1, char * p2)
{
  lua_State *L = iuplua_call_start(self, "listclick_cb");
  lua_pushinteger(L, p0);
  lua_pushinteger(L, p1);
  lua_pushstring(L, p2);
  return iuplua_call(L, 3);
}

static int matrixlist_listinsert_cb(Ihandle *self, int p0)
{
  lua_State *L = iuplua_call_start(self, "listinsert_cb");
  lua_pushinteger(L, p0);
  return iuplua_call(L, 1);
}

static int matrixlist_listremove_cb(Ihandle *self, int p0)
{
  lua_State *L = iuplua_call_start(self, "listremove_cb");
  lua_pushinteger(L, p0);
  return iuplua_call(L, 1);
}

static int matrixlist_listaction_cb(Ihandle *self, int p0, int p1)
{
  lua_State *L = iuplua_call_start(self, "listaction_cb");
  lua_pushinteger(L, p0);
  lua_pushinteger(L, p1);
  return iuplua_call(L, 2);
}

static int matrixlist_listedition_cb(Ihandle *self, int p0, int p1, int p2, int p3)
{
  lua_State *L = iuplua_call_start(self, "listedition_cb");
  lua_pushinteger(L, p0);
  lua_pushinteger(L, p1);
  lua_pushinteger(L, p2);
  lua_pushinteger(L, p3);
  return iuplua_call(L, 4);
}

static int MatrixList(lua_State *L)
{
  Ihandle *ih = IupMatrixList();
  iuplua_plugstate(L, ih);
  iuplua_pushihandle_raw(L, ih);
  return 1;
}

int iupmatrixlistlua_open(lua_State * L)
{
  iuplua_register(L, MatrixList, "MatrixList");

  iuplua_register_cb(L, "IMAGEVALUECHANGED_CB", (lua_CFunction)matrixlist_imagevaluechanged_cb, NULL);
  iuplua_register_cb(L, "LISTRELEASE_CB", (lua_CFunction)matrixlist_listrelease_cb, NULL);
  iuplua_register_cb(L, "LISTCLICK_CB", (lua_CFunction)matrixlist_listclick_cb, NULL);
  iuplua_register_cb(L, "LISTINSERT_CB", (lua_CFunction)matrixlist_listinsert_cb, NULL);
  iuplua_register_cb(L, "LISTREMOVE_CB", (lua_CFunction)matrixlist_listremove_cb, NULL);
  iuplua_register_cb(L, "LISTACTION_CB", (lua_CFunction)matrixlist_listaction_cb, NULL);
  iuplua_register_cb(L, "LISTEDITION_CB", (lua_CFunction)matrixlist_listedition_cb, NULL);

#ifdef IUPLUA_USELOH
#include "matrixlist.loh"
#else
#ifdef IUPLUA_USELH
#include "matrixlist.lh"
#else
  iuplua_dofile(L, "matrixlist.lua");
#endif
#endif

  return 0;
}

