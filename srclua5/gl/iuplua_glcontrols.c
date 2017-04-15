/** \file
 * \brief Bindig of iupglcontrols to Lua 5.
 *
 * See Copyright Notice in "iup.h"
 */
 
#include <lua.h>
#include <lauxlib.h>

#include "iup.h"
#include "iupglcontrols.h"

#include "iuplua.h"
#include "iupluaglcontrols.h"
#include "il.h"
#include "il_glcontrols.h"

#include "iup_attrib.h"


static int GLDrawText(lua_State *L)
{
  size_t len = 0;
  IupGLDrawText(iuplua_checkihandle(L, 1), luaL_checklstring(L, 2, &len), (int)len, (int)luaL_checkinteger(L, 3), (int)luaL_checkinteger(L, 4));
  return 0;
}

static int GLDrawImage(lua_State *L)
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

  IupGLDrawImage(iuplua_checkihandle(L, 1), name, (int)luaL_checkinteger(L, 3), (int)luaL_checkinteger(L, 4), (int)luaL_checkinteger(L, 5));
  return 0;
}

static int GLDrawGetTextSize(lua_State *L)
{
  int w, h;
  IupGLDrawGetTextSize(iuplua_checkihandle(L, 1), luaL_checkstring(L, 2), &w, &h);
  lua_pushinteger(L, w);
  lua_pushinteger(L, h);
  return 2;
}

static int GLDrawGetImageInfo(lua_State *L)
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

  IupGLDrawGetImageInfo(name, &w, &h, &bpp);
  lua_pushinteger(L, w);
  lua_pushinteger(L, h);
  lua_pushinteger(L, bpp);
  return 3;
}

static void iupluagldraw_open(lua_State * L)
{
  struct luaL_Reg funcs[] = {
    { "GLDrawText", GLDrawText },
    { "GLDrawImage", GLDrawImage },
    { "GLDrawGetTextSize", GLDrawGetTextSize },
    { "GLDrawGetImageInfo", GLDrawGetImageInfo },
    { NULL, NULL },
  };

  /* iup table is already at the top of the stack */
  iuplua_register_funcs(L, funcs);
}


int iupglcontrolslua_open(lua_State * L)
{
  if (iuplua_opencall_internal(L))
    IupGLControlsOpen();

  iuplua_get_env(L);

  iupglcanvasboxlua_open(L);
  iupglsubcanvaslua_open(L);
  iupgllabellua_open(L);
  iupglbuttonlua_open(L);
  iupglexpanderlua_open(L);
  iupglframelua_open(L);
  iupgllinklua_open(L);
  iupglprogressbarlua_open(L);
  iupglseparatorlua_open(L);
  iupglsizeboxlua_open(L);
  iupgltogglelua_open(L);
  iupglvallua_open(L); 
  iupglscrollboxlua_open(L); 
  iupgltextlua_open(L); 

  iupluagldraw_open(L);

  return 0;
}

/* obligatory to use require"iupluaglcontrols" */
int luaopen_iupluaglcontrols(lua_State* L)
{
  return iupglcontrolslua_open(L);
}

