/******************************************************************************
 * Automatically generated file. Please don't change anything.                *
 *****************************************************************************/

#include <stdlib.h>

#include <lua.h>
#include <lauxlib.h>

#include "iup.h"
#include "iuplua.h"
#include "il.h"


static int flatbutton_flat_button_cb(Ihandle *self, int p0, int p1, int p2, int p3, char * p4)
{
  lua_State *L = iuplua_call_start(self, "flat_button_cb");
  lua_pushinteger(L, p0);
  lua_pushinteger(L, p1);
  lua_pushinteger(L, p2);
  lua_pushinteger(L, p3);
  lua_pushstring(L, p4);
  return iuplua_call(L, 5);
}

static int flatbutton_flat_action(Ihandle *self)
{
  lua_State *L = iuplua_call_start(self, "flat_action");
  return iuplua_call(L, 0);
}

static int flatbutton_flat_enterwindow_cb(Ihandle *self)
{
  lua_State *L = iuplua_call_start(self, "flat_enterwindow_cb");
  return iuplua_call(L, 0);
}

static int flatbutton_flat_leavewindow_cb(Ihandle *self)
{
  lua_State *L = iuplua_call_start(self, "flat_leavewindow_cb");
  return iuplua_call(L, 0);
}

static int flatbutton_flat_focus_cb(Ihandle *self, int p0)
{
  lua_State *L = iuplua_call_start(self, "flat_focus_cb");
  lua_pushinteger(L, p0);
  return iuplua_call(L, 1);
}

static int FlatButton(lua_State *L)
{
  Ihandle *ih = IupFlatButton((char *)luaL_optstring(L, 1, NULL));
  iuplua_plugstate(L, ih);
  iuplua_pushihandle_raw(L, ih);
  return 1;
}

int iupflatbuttonlua_open(lua_State * L)
{
  iuplua_register(L, FlatButton, "FlatButton");

  iuplua_register_cb(L, "FLAT_BUTTON_CB", (lua_CFunction)flatbutton_flat_button_cb, NULL);
  iuplua_register_cb(L, "FLAT_ACTION", (lua_CFunction)flatbutton_flat_action, NULL);
  iuplua_register_cb(L, "FLAT_ENTERWINDOW_CB", (lua_CFunction)flatbutton_flat_enterwindow_cb, NULL);
  iuplua_register_cb(L, "FLAT_LEAVEWINDOW_CB", (lua_CFunction)flatbutton_flat_leavewindow_cb, NULL);
  iuplua_register_cb(L, "FLAT_FOCUS_CB", (lua_CFunction)flatbutton_flat_focus_cb, NULL);

#ifdef IUPLUA_USELOH
#include "flatbutton.loh"
#else
#ifdef IUPLUA_USELH
#include "flatbutton.lh"
#else
  iuplua_dofile(L, "flatbutton.lua");
#endif
#endif

  return 0;
}

