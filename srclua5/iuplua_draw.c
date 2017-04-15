/** \file
* \brief IUP binding for Lua 5.
*
* See Copyright Notice in "iup.h"
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "iup.h"
#include "iupkey.h"
#include "iupdraw.h"

#include <lua.h>
#include <lauxlib.h>

#include "iuplua.h"
#include "il.h"

#include "iup_attrib.h"
#include "iup_globalattrib.h"
#include "iup_object.h"
#include "iup_str.h"


static int DrawBegin(lua_State *L)
{
  IupDrawBegin(iuplua_checkihandle(L, 1));
  return 0;
}

static int DrawEnd(lua_State *L)
{
  IupDrawEnd(iuplua_checkihandle(L, 1));
  return 0;
}

static int DrawParentBackground(lua_State *L)
{
  IupDrawParentBackground(iuplua_checkihandle(L, 1));
  return 0;
}

static int DrawLine(lua_State *L)
{
  IupDrawLine(iuplua_checkihandle(L, 1), (int)luaL_checkinteger(L, 2), (int)luaL_checkinteger(L, 3), (int)luaL_checkinteger(L, 4), (int)luaL_checkinteger(L, 5));
  return 0;
}

static int DrawRectangle(lua_State *L)
{
  IupDrawRectangle(iuplua_checkihandle(L, 1), (int)luaL_checkinteger(L, 2), (int)luaL_checkinteger(L, 3), (int)luaL_checkinteger(L, 4), (int)luaL_checkinteger(L, 5));
  return 0;
}

static int DrawArc(lua_State *L)
{
  IupDrawArc(iuplua_checkihandle(L, 1), (int)luaL_checkinteger(L, 2), (int)luaL_checkinteger(L, 3), (int)luaL_checkinteger(L, 4), (int)luaL_checkinteger(L, 5), luaL_checknumber(L, 6), luaL_checknumber(L, 7));
  return 0;
}

static int DrawPolygon(lua_State *L)
{
  int* points = iuplua_checkint_array(L, 2, 0);
  IupDrawPolygon(iuplua_checkihandle(L, 1), points, iuplua_getn(L, 2));
  free(points);
  return 0;
}

static int DrawText(lua_State *L)
{
  size_t len = 0;
  IupDrawText(iuplua_checkihandle(L, 1), luaL_checklstring(L, 2, &len), (int)len, (int)luaL_checkinteger(L, 3), (int)luaL_checkinteger(L, 4));
  return 0;
}

static int DrawImage(lua_State *L)
{
  const char* name;
  if (lua_isuserdata(L, 2))
  {
    Ihandle* image = iuplua_checkihandle(L, 2);
    name = iupAttribGetHandleName(image);
    if (!name)
    {
      iupAttribSetHandleName(image);
      name = iupAttribGetHandleName(image);
    }
  }
  else
    name = luaL_checkstring(L, 2);

  IupDrawImage(iuplua_checkihandle(L, 1), name, (int)luaL_checkinteger(L, 3), (int)luaL_checkinteger(L, 4), (int)luaL_checkinteger(L, 5));
  return 0;
}

static int DrawSetClipRect(lua_State *L)
{
  IupDrawSetClipRect(iuplua_checkihandle(L, 1), (int)luaL_checkinteger(L, 2), (int)luaL_checkinteger(L, 3), (int)luaL_checkinteger(L, 4), (int)luaL_checkinteger(L, 5));
  return 0;
}

static int DrawResetClip(lua_State *L)
{
  IupDrawResetClip(iuplua_checkihandle(L, 1));
  return 0;
}

static int DrawSelectRect(lua_State *L)
{
  IupDrawSelectRect(iuplua_checkihandle(L, 1), (int)luaL_checkinteger(L, 2), (int)luaL_checkinteger(L, 3), (int)luaL_checkinteger(L, 4), (int)luaL_checkinteger(L, 5));
  return 0;
}

static int DrawFocusRect(lua_State *L)
{
  IupDrawFocusRect(iuplua_checkihandle(L, 1), (int)luaL_checkinteger(L, 2), (int)luaL_checkinteger(L, 3), (int)luaL_checkinteger(L, 4), (int)luaL_checkinteger(L, 5));
  return 0;
}

static int DrawGetSize(lua_State *L)
{
  int w, h;
  IupDrawGetSize(iuplua_checkihandle(L, 1), &w, &h);
  lua_pushinteger(L, w);
  lua_pushinteger(L, h);
  return 2;
}

static int DrawGetTextSize(lua_State *L)
{
  int w, h;
  IupDrawGetTextSize(iuplua_checkihandle(L, 1), luaL_checkstring(L, 2), &w, &h);
  lua_pushinteger(L, w);
  lua_pushinteger(L, h);
  return 2;
}

static int DrawGetImageInfo(lua_State *L)
{
  int w, h, bpp;
  const char* name;
  if (lua_isuserdata(L, 1))
  {
    Ihandle* image = iuplua_checkihandle(L, 1);
    name = iupAttribGetHandleName(image);
    if (!name)
    {
      iupAttribSetHandleName(image);
      name = iupAttribGetHandleName(image);
    }
  }
  else
    name = luaL_checkstring(L, 1);

  IupDrawGetImageInfo(name, &w, &h, &bpp);
  lua_pushinteger(L, w);
  lua_pushinteger(L, h);
  lua_pushinteger(L, bpp);
  return 3;
}

void iupluadraw_open(lua_State * L)
{
  struct luaL_Reg funcs[] = {
    { "DrawBegin", DrawBegin },
    { "DrawEnd", DrawEnd },
    { "DrawParentBackground", DrawParentBackground },
    { "DrawLine", DrawLine },
    { "DrawRectangle", DrawRectangle },
    { "DrawArc", DrawArc },
    { "DrawPolygon", DrawPolygon },
    { "DrawText", DrawText },
    { "DrawImage", DrawImage },
    { "DrawSetClipRect", DrawSetClipRect },
    { "DrawResetClip", DrawResetClip },
    { "DrawSelectRect", DrawSelectRect },
    { "DrawFocusRect", DrawFocusRect },
    { "DrawGetSize", DrawGetSize },
    { "DrawGetTextSize", DrawGetTextSize },
    { "DrawGetImageInfo", DrawGetImageInfo },
    { NULL, NULL },
  };

  /* iup table is already at the top of the stack */
  iuplua_register_funcs(L, funcs);
}
