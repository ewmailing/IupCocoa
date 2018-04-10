/******************************************************************************
 * Automatically generated file. Please don't change anything.                *
 *****************************************************************************/

#include <stdlib.h>

#include <lua.h>
#include <lauxlib.h>

#include "iup.h"
#include "iuplua.h"
#include "il.h"


static int dropbutton_dropdown_cb(Ihandle *self, int p0)
{
  lua_State *L = iuplua_call_start(self, "dropdown_cb");
  lua_pushinteger(L, p0);
  return iuplua_call(L, 1);
}

static int dropbutton_flat_motion_cb(Ihandle *self, int p0, int p1, char * p2)
{
  lua_State *L = iuplua_call_start(self, "flat_motion_cb");
  lua_pushinteger(L, p0);
  lua_pushinteger(L, p1);
  lua_pushstring(L, p2);
  return iuplua_call(L, 3);
}

static int dropbutton_flat_leavewindow_cb(Ihandle *self)
{
  lua_State *L = iuplua_call_start(self, "flat_leavewindow_cb");
  return iuplua_call(L, 0);
}

static int dropbutton_flat_action(Ihandle *self)
{
  lua_State *L = iuplua_call_start(self, "flat_action");
  return iuplua_call(L, 0);
}

static int dropbutton_flat_enterwindow_cb(Ihandle *self)
{
  lua_State *L = iuplua_call_start(self, "flat_enterwindow_cb");
  return iuplua_call(L, 0);
}

static int dropbutton_flat_focus_cb(Ihandle *self, int p0)
{
  lua_State *L = iuplua_call_start(self, "flat_focus_cb");
  lua_pushinteger(L, p0);
  return iuplua_call(L, 1);
}

static int dropbutton_dropshow_cb(Ihandle *self, int p0)
{
  lua_State *L = iuplua_call_start(self, "dropshow_cb");
  lua_pushinteger(L, p0);
  return iuplua_call(L, 1);
}

static int dropbutton_flat_button_cb(Ihandle *self, int p0, int p1, int p2, int p3, char * p4)
{
  lua_State *L = iuplua_call_start(self, "flat_button_cb");
  lua_pushinteger(L, p0);
  lua_pushinteger(L, p1);
  lua_pushinteger(L, p2);
  lua_pushinteger(L, p3);
  lua_pushstring(L, p4);
  return iuplua_call(L, 5);
}

static int DropButton(lua_State *L)
{
  Ihandle *ih = IupDropButton(iuplua_checkihandleornil(L, 1));
  iuplua_plugstate(L, ih);
  iuplua_pushihandle_raw(L, ih);
  return 1;
}

int iupdropbuttonlua_open(lua_State * L)
{
  iuplua_register(L, DropButton, "DropButton");

  iuplua_register_cb(L, "DROPDOWN_CB", (lua_CFunction)dropbutton_dropdown_cb, NULL);
  iuplua_register_cb(L, "FLAT_MOTION_CB", (lua_CFunction)dropbutton_flat_motion_cb, NULL);
  iuplua_register_cb(L, "FLAT_LEAVEWINDOW_CB", (lua_CFunction)dropbutton_flat_leavewindow_cb, NULL);
  iuplua_register_cb(L, "FLAT_ACTION", (lua_CFunction)dropbutton_flat_action, NULL);
  iuplua_register_cb(L, "FLAT_ENTERWINDOW_CB", (lua_CFunction)dropbutton_flat_enterwindow_cb, NULL);
  iuplua_register_cb(L, "FLAT_FOCUS_CB", (lua_CFunction)dropbutton_flat_focus_cb, NULL);
  iuplua_register_cb(L, "DROPSHOW_CB", (lua_CFunction)dropbutton_dropshow_cb, NULL);
  iuplua_register_cb(L, "FLAT_BUTTON_CB", (lua_CFunction)dropbutton_flat_button_cb, NULL);

#ifdef IUPLUA_USELOH
#include "dropbutton.loh"
#else
#ifdef IUPLUA_USELH
#include "dropbutton.lh"
#else
  iuplua_dofile(L, "dropbutton.lua");
#endif
#endif

  return 0;
}

