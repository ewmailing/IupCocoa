#include <stdlib.h>

#include <lua.h>
#include <lauxlib.h>

#include "iup.h"
#include "iuplua.h"
#include "iup_pplot.h"

#include <cd.h>
#include <cdlua.h>

#include "il.h"


static int pplot_edit_cb(Ihandle *self, int p0, int p1, float p2, float p3, float *p4, float *p5)
{
  lua_Object obj;
  iuplua_call_start(self, "edit_cb");
  lua_pushnumber(p0);
  lua_pushnumber(p1);
  lua_pushnumber(p2);
  lua_pushnumber(p3);
  if (lua_call ("iupCallMethod"))
  {
    lua_endblock ();
    return IUP_IGNORE;
  }
  obj = lua_getresult (1);
  if (obj == LUA_NOOBJECT)
  {
    lua_endblock ();
    return IUP_IGNORE;
  }
  else if (lua_isnumber (obj))
  {
    int ret;
    *p4 = (float)lua_getnumber (obj);

    obj = lua_getresult (2);
    if (obj == LUA_NOOBJECT || !lua_isnumber (obj))
    {
      lua_endblock ();
      return IUP_IGNORE;
    }
    *p5 = (float)lua_getnumber (obj);

    obj = lua_getresult (4);
    if (obj == LUA_NOOBJECT || !lua_isnumber (obj))
    {
      lua_endblock ();
      return IUP_DEFAULT;
    }
    ret = (int)lua_getnumber (obj);
    lua_endblock ();
    return ret;
  }

  lua_endblock ();
  return IUP_IGNORE;
}

static int pplot_postdraw_cb(Ihandle *self, cdCanvas* cnv)
{
  iuplua_call_start(self, "postdraw_cb");
  cdlua_pushcanvas(cnv);
  return iuplua_call();
}

static int pplot_predraw_cb(Ihandle *self, cdCanvas* cnv)
{
  iuplua_call_start(self, "predraw_cb");
  cdlua_pushcanvas(cnv);
  return iuplua_call();
}

static int pplot_deleteend_cb(Ihandle *self)
{
  iuplua_call_start(self, "deleteend_cb");
  return iuplua_call();
}

static int pplot_selectbegin_cb(Ihandle *self)
{
  iuplua_call_start(self, "selectbegin_cb");
  return iuplua_call();
}

static int pplot_editend_cb(Ihandle *self)
{
  iuplua_call_start(self, "editend_cb");
  return iuplua_call();
}

static int pplot_editbegin_cb(Ihandle *self)
{
  iuplua_call_start(self, "editbegin_cb");
  return iuplua_call();
}

static int pplot_selectend_cb(Ihandle *self)
{
  iuplua_call_start(self, "selectend_cb");
  return iuplua_call();
}

static int pplot_select_cb(Ihandle *self, int p0, int p1, float p2, float p3, int p4)
{
  iuplua_call_start(self, "select_cb");
  lua_pushnumber(p0);
  lua_pushnumber(p1);
  lua_pushnumber(p2);
  lua_pushnumber(p3);
  lua_pushnumber(p4);
  return iuplua_call();
}

static int pplot_deletebegin_cb(Ihandle *self)
{
  iuplua_call_start(self, "deletebegin_cb");
  return iuplua_call();
}

static int pplot_delete_cb(Ihandle *self, int p0, int p1, float p2, float p3)
{
  iuplua_call_start(self, "delete_cb");
  lua_pushnumber(p0);
  lua_pushnumber(p1);
  lua_pushnumber(p2);
  lua_pushnumber(p3);
  return iuplua_call();
}

static void PPlot(void)
{
  int tag = (int)lua_getnumber(lua_getglobal("iuplua_tag"));
  lua_pushusertag(IupPPlot(), tag);
}

static void PlotBegin(void)
{
  Ihandle *ih = iuplua_checkihandle(1);
  IupPPlotBegin(ih, luaL_check_int(2));
}

static void PlotAdd(void)
{
  Ihandle *ih = iuplua_checkihandle(1);
  IupPPlotAdd(ih, (float)luaL_check_number(2), (float)luaL_check_number(3));
}

static void PlotAddStr(void)
{
  Ihandle *ih = iuplua_checkihandle(1);
  IupPPlotAddStr(ih, luaL_check_string(2), (float)luaL_check_number(3));
}

static void PlotEnd(void)
{
  Ihandle *ih = iuplua_checkihandle(1);
  int ret = IupPPlotEnd(ih);
  lua_pushnumber(ret);
}

static void PlotInsertStr(void)
{
  Ihandle *ih = iuplua_checkihandle(1);
  IupPPlotInsertStr(ih, luaL_check_int(2), luaL_check_int(3), luaL_check_string(4), (float)luaL_check_number(5));
}

static void PlotInsert(void)
{
  Ihandle *ih = iuplua_checkihandle(1);
  IupPPlotInsert(ih, luaL_check_int(2), luaL_check_int(3), (float)luaL_check_number(4), (float)luaL_check_number(5));
}

static void PlotTransform(void)
{
  Ihandle *ih = iuplua_checkihandle(1);
  int ix, iy;
  IupPPlotTransform(ih, (float)luaL_check_number(2), (float)luaL_check_number(3), &ix, &iy);
  lua_pushnumber(ix);
  lua_pushnumber(iy);
}

static void PlotPaintTo(void)
{
  Ihandle *ih = iuplua_checkihandle(1);
  IupPPlotPaintTo(ih, cdlua_checkcanvas(2));
}

int iup_pplotlua_open(void)
{
  lua_register("iupCreatePPlot", PPlot);

  lua_register("iup_pplot_edit_cb", (lua_CFunction)pplot_edit_cb);
  lua_register("iup_pplot_deleteend_cb", (lua_CFunction)pplot_deleteend_cb);
  lua_register("iup_pplot_selectbegin_cb", (lua_CFunction)pplot_selectbegin_cb);
  lua_register("iup_pplot_postdraw_cb", (lua_CFunction)pplot_postdraw_cb);
  lua_register("iup_pplot_editend_cb", (lua_CFunction)pplot_editend_cb);
  lua_register("iup_pplot_editbegin_cb", (lua_CFunction)pplot_editbegin_cb);
  lua_register("iup_pplot_selectend_cb", (lua_CFunction)pplot_selectend_cb);
  lua_register("iup_pplot_select_cb", (lua_CFunction)pplot_select_cb);
  lua_register("iup_pplot_deletebegin_cb", (lua_CFunction)pplot_deletebegin_cb);
  lua_register("iup_pplot_predraw_cb", (lua_CFunction)pplot_predraw_cb);
  lua_register("iup_pplot_delete_cb", (lua_CFunction)pplot_delete_cb);

  iuplua_register("IupPPlotBegin", PlotBegin);
  iuplua_register("IupPPlotAdd", PlotAdd);
  iuplua_register("IupPPlotAddStr", PlotAddStr);
  iuplua_register("IupPPlotEnd", PlotEnd);
  iuplua_register("IupPPlotInsertStr", PlotInsertStr);
  iuplua_register("IupPPlotInsert", PlotInsert);
  iuplua_register("IupPPlotTransform", PlotTransform);
  iuplua_register("IupPPlotPaintTo", PlotPaintTo);

#ifdef IUPLUA_USELOH
#ifdef TEC_BIGENDIAN
#ifdef TEC_64
#include "loh/pplot_be64.loh"
#else
#include "loh/pplot_be32.loh"
#endif
#else
#ifdef TEC_64
#ifdef WIN64
#include "loh/pplot_le64w.loh"
#else
#include "loh/pplot_le64.loh"
#endif
#else
#include "loh/pplot.loh"
#endif
#endif
#else
  iuplua_dofile("luapplot.lua");
#endif

  return 1;
}

