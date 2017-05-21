/******************************************************************************
 * Automatically generated file. Please don't change anything.                *
 *****************************************************************************/

#include <stdlib.h>

#include <lua.h>
#include <lauxlib.h>

#include "iup.h"
#include "iuplua.h"
#include "iup_plot.h"
#include "il.h"


static int plot_drawsample_cb(Ihandle *self, int p0, int p1, double p2, double p3, int p4)
{
  lua_State *L = iuplua_call_start(self, "drawsample_cb");
  lua_pushinteger(L, p0);
  lua_pushinteger(L, p1);
  lua_pushnumber(L, p2);
  lua_pushnumber(L, p3);
  lua_pushinteger(L, p4);
  return iuplua_call(L, 5);
}

static int plot_deleteend_cb(Ihandle *self)
{
  lua_State *L = iuplua_call_start(self, "deleteend_cb");
  return iuplua_call(L, 0);
}

static int plot_plotmotion_cb(Ihandle *self, double p0, double p1, char * p2)
{
  lua_State *L = iuplua_call_start(self, "plotmotion_cb");
  lua_pushnumber(L, p0);
  lua_pushnumber(L, p1);
  lua_pushstring(L, p2);
  return iuplua_call(L, 3);
}

static int plot_clicksegment_cb(Ihandle *self, int p0, int p1, double p2, double p3, int p4, double p5, double p6, int p7)
{
  lua_State *L = iuplua_call_start(self, "clicksegment_cb");
  lua_pushinteger(L, p0);
  lua_pushinteger(L, p1);
  lua_pushnumber(L, p2);
  lua_pushnumber(L, p3);
  lua_pushinteger(L, p4);
  lua_pushnumber(L, p5);
  lua_pushnumber(L, p6);
  lua_pushinteger(L, p7);
  return iuplua_call(L, 8);
}

static int plot_menucontext_cb(Ihandle *self, Ihandle * p0, int p1, int p2)
{
  lua_State *L = iuplua_call_start(self, "menucontext_cb");
  iuplua_pushihandle(L, p0);
  lua_pushinteger(L, p1);
  lua_pushinteger(L, p2);
  return iuplua_call(L, 3);
}

static int plot_selectbegin_cb(Ihandle *self)
{
  lua_State *L = iuplua_call_start(self, "selectbegin_cb");
  return iuplua_call(L, 0);
}

static int plot_predraw_cb(Ihandle *self, int p0)
{
  lua_State *L = iuplua_call_start(self, "predraw_cb");
  lua_pushinteger(L, p0);
  return iuplua_call(L, 1);
}

static int plot_postdraw_cb(Ihandle *self, int p0)
{
  lua_State *L = iuplua_call_start(self, "postdraw_cb");
  lua_pushinteger(L, p0);
  return iuplua_call(L, 1);
}

static int plot_menucontextclose_cb(Ihandle *self, Ihandle * p0, int p1, int p2)
{
  lua_State *L = iuplua_call_start(self, "menucontextclose_cb");
  iuplua_pushihandle(L, p0);
  lua_pushinteger(L, p1);
  lua_pushinteger(L, p2);
  return iuplua_call(L, 3);
}

static int plot_clicksample_cb(Ihandle *self, int p0, int p1, double p2, double p3, int p4)
{
  lua_State *L = iuplua_call_start(self, "clicksample_cb");
  lua_pushinteger(L, p0);
  lua_pushinteger(L, p1);
  lua_pushnumber(L, p2);
  lua_pushnumber(L, p3);
  lua_pushinteger(L, p4);
  return iuplua_call(L, 5);
}

static int plot_selectend_cb(Ihandle *self)
{
  lua_State *L = iuplua_call_start(self, "selectend_cb");
  return iuplua_call(L, 0);
}

static int plot_delete_cb(Ihandle *self, int p0, int p1, double p2, double p3)
{
  lua_State *L = iuplua_call_start(self, "delete_cb");
  lua_pushinteger(L, p0);
  lua_pushinteger(L, p1);
  lua_pushnumber(L, p2);
  lua_pushnumber(L, p3);
  return iuplua_call(L, 4);
}

static int plot_deletebegin_cb(Ihandle *self)
{
  lua_State *L = iuplua_call_start(self, "deletebegin_cb");
  return iuplua_call(L, 0);
}

static int plot_select_cb(Ihandle *self, int p0, int p1, double p2, double p3, int p4)
{
  lua_State *L = iuplua_call_start(self, "select_cb");
  lua_pushinteger(L, p0);
  lua_pushinteger(L, p1);
  lua_pushnumber(L, p2);
  lua_pushnumber(L, p3);
  lua_pushinteger(L, p4);
  return iuplua_call(L, 5);
}

static int plot_plotbutton_cb(Ihandle *self, int p0, int p1, double p2, double p3, char * p4)
{
  lua_State *L = iuplua_call_start(self, "plotbutton_cb");
  lua_pushinteger(L, p0);
  lua_pushinteger(L, p1);
  lua_pushnumber(L, p2);
  lua_pushnumber(L, p3);
  lua_pushstring(L, p4);
  return iuplua_call(L, 5);
}

static int Plot(lua_State *L)
{
  Ihandle *ih = IupPlot();
  iuplua_plugstate(L, ih);
  iuplua_pushihandle_raw(L, ih);
  return 1;
}

void iuplua_plotfuncs_open(lua_State *L);

int iupplotlua_open(lua_State * L)
{
  iuplua_register(L, Plot, "Plot");

  iuplua_register_cb(L, "DRAWSAMPLE_CB", (lua_CFunction)plot_drawsample_cb, NULL);
  iuplua_register_cb(L, "DELETEEND_CB", (lua_CFunction)plot_deleteend_cb, NULL);
  iuplua_register_cb(L, "PLOTMOTION_CB", (lua_CFunction)plot_plotmotion_cb, NULL);
  iuplua_register_cb(L, "CLICKSEGMENT_CB", (lua_CFunction)plot_clicksegment_cb, NULL);
  iuplua_register_cb(L, "MENUCONTEXT_CB", (lua_CFunction)plot_menucontext_cb, NULL);
  iuplua_register_cb(L, "SELECTBEGIN_CB", (lua_CFunction)plot_selectbegin_cb, NULL);
  iuplua_register_cb(L, "PREDRAW_CB", (lua_CFunction)plot_predraw_cb, NULL);
  iuplua_register_cb(L, "POSTDRAW_CB", (lua_CFunction)plot_postdraw_cb, NULL);
  iuplua_register_cb(L, "MENUCONTEXTCLOSE_CB", (lua_CFunction)plot_menucontextclose_cb, NULL);
  iuplua_register_cb(L, "CLICKSAMPLE_CB", (lua_CFunction)plot_clicksample_cb, NULL);
  iuplua_register_cb(L, "SELECTEND_CB", (lua_CFunction)plot_selectend_cb, NULL);
  iuplua_register_cb(L, "DELETE_CB", (lua_CFunction)plot_delete_cb, NULL);
  iuplua_register_cb(L, "DELETEBEGIN_CB", (lua_CFunction)plot_deletebegin_cb, NULL);
  iuplua_register_cb(L, "SELECT_CB", (lua_CFunction)plot_select_cb, NULL);
  iuplua_register_cb(L, "PLOTBUTTON_CB", (lua_CFunction)plot_plotbutton_cb, NULL);

  iuplua_plotfuncs_open(L);

#ifdef IUPLUA_USELOH
#include "plot.loh"
#else
#ifdef IUPLUA_USELH
#include "plot.lh"
#else
  iuplua_dofile(L, "plot.lua");
#endif
#endif

  return 0;
}

