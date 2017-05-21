/** \file
 * \brief plot binding for Lua 5.
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdlib.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "iup.h"
#include "iup_plot.h"

#include <cd.h>
#include <cdlua.h>

#include "iuplua.h"
#include "iuplua_plot.h"
#include "il.h"

#include "iup_object.h"
#include "iup_assert.h"
#include "iup_predialogs.h"


typedef int(*IFnL)(Ihandle*, lua_State *L);

static void ShowFormulaError(Ihandle* ih, lua_State *L)
{
  const char* str_message = IupGetLanguageString("IUP_ERRORINVALIDFORMULA");
  const char* error = lua_tostring(L, -1);
  char msg[1024];
  sprintf(msg, "%s\n  Lua error: %s", str_message, error);
  iupShowError(IupGetDialog(ih), msg);
}

int IupPlotSetFormula(Ihandle* ih, int sample_count, const char* formula, const char* init)
{
  lua_State *L;
  int i, ds_index, ret_count = 1;
  double min, max, step, p, x, y;
  char formula_func[1024];
  IFnL init_cb;

  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return -1;

  /* must be an IupMatrix */
  if (ih->iclass->nativetype != IUP_TYPECANVAS ||
      !IupClassMatch(ih, "plot"))
    return -1;

  L = luaL_newstate();
  luaL_openlibs(L);

  {
    const char* register_global =
      "function openpackage(ns)\n"
      "  for n, v in pairs(ns) do _G[n] = v end\n"
      "end\n"
      "openpackage(math)\n";
    luaL_dostring(L, register_global);
  }

  if (init)
    luaL_dostring(L, init);

  init_cb = (IFnL)IupGetCallback(ih, "FORMULAINIT_CB");
  if (init_cb)
    init_cb(ih, L);

  lua_pushlightuserdata(L, ih);
  lua_setglobal(L, "plot");

  if (IupGetInt(ih, "FORMULA_PARAMETRIC"))
    ret_count = 2;

  sprintf(formula_func, "function plot_formula(sample_index, %s)\n"
          "  return %s\n"
          "end\n", ret_count == 2 ? "t" : "x", formula);

  if (luaL_dostring(L, formula_func) != 0)
  {
    ShowFormulaError(ih, L);
    lua_close(L);
    return -1;
  }

  min = IupGetDouble(ih, "FORMULA_MIN");
  max = IupGetDouble(ih, "FORMULA_MAX");
  step = (max - min) / (double)(sample_count - 1);

  IupPlotBegin(ih, 0);

  for (i = 0, p = min; i < sample_count; i++, p += step)
  {
    lua_getglobal(L, "plot_formula");
    lua_pushinteger(L, i);
    lua_pushnumber(L, p);

    if (lua_pcall(L, 2, ret_count, 0) != 0)
    {
      ShowFormulaError(ih, L);
      lua_close(L);
      return -1;
    }

    if (!lua_isnumber(L, -1))
    {
      const char* str_message = IupGetLanguageString("IUP_ERRORINVALIDFORMULA");
      iupShowError(IupGetDialog(ih), str_message);
      lua_close(L);
      return -1;
    }

    if (ret_count == 2)
    {
      if (!lua_isnumber(L, -2))
      {
        const char* str_message = IupGetLanguageString("IUP_ERRORINVALIDFORMULA");
        iupShowError(IupGetDialog(ih), str_message);
        lua_close(L);
        return -1;
      }

      x = lua_tonumber(L, -2);
      y = lua_tonumber(L, -1);

      lua_pop(L, 2);  /* remove the result from the stack */
    }
    else
    {
      x = p;
      y = lua_tonumber(L, -1);

      lua_pop(L, 1);  /* remove the result from the stack */
    }

    IupPlotAdd(ih, x, y);
  }

  ds_index = IupPlotEnd(ih);

  lua_close(L);
  return ds_index;
}

static int plot_postdraw_cb(Ihandle *self, cdCanvas* cnv)
{
  lua_State *L = iuplua_call_start(self, "postdraw_cb");
  cdlua_pushcanvas(L, cnv);
  return iuplua_call(L, 1);
}

static int plot_predraw_cb(Ihandle *self, cdCanvas* cnv)
{
  lua_State *L = iuplua_call_start(self, "predraw_cb");
  cdlua_pushcanvas(L, cnv);
  return iuplua_call(L, 1);
}

static int PlotBegin(lua_State *L)
{
  Ihandle *ih = iuplua_checkihandle(L,1);
  IupPlotBegin(ih, luaL_checkinteger(L,2));
  return 0;
}

static int PlotAdd(lua_State *L)
{
  Ihandle *ih = iuplua_checkihandle(L,1);
  IupPlotAdd(ih, luaL_checknumber(L,2), luaL_checknumber(L,3));
  return 0;
}

static int PlotAddStr(lua_State *L)
{
  Ihandle *ih = iuplua_checkihandle(L,1);
  IupPlotAddStr(ih, luaL_checkstring(L,2), luaL_checknumber(L,3));
  return 0;
}

static int PlotAddSegment(lua_State *L)
{
  Ihandle *ih = iuplua_checkihandle(L, 1);
  IupPlotAddSegment(ih, luaL_checknumber(L, 2), luaL_checknumber(L, 3));
  return 0;
}

static int PlotEnd(lua_State *L)
{
  Ihandle *ih = iuplua_checkihandle(L,1);
  int ret = IupPlotEnd(ih);
  lua_pushinteger(L, ret);
  return 1;
}

static int PlotLoadData(lua_State *L)
{
  Ihandle *ih = iuplua_checkihandle(L, 1);
  int ret = IupPlotLoadData(ih, luaL_checkstring(L, 2), luaL_checkinteger(L, 3));
  lua_pushinteger(L, ret);
  return 1;
}

static int PlotSetFormula(lua_State *L)
{
  Ihandle *ih = iuplua_checkihandle(L, 1);
  int ret = IupPlotSetFormula(ih, luaL_checkinteger(L, 2), luaL_checkstring(L, 3), luaL_optstring(L, 4, NULL));
  lua_pushinteger(L, ret);
  return 1;
}

static int PlotFindSample(lua_State *L)
{
  Ihandle *ih = iuplua_checkihandle(L, 1);
  int ds_index, sample_index;
  int ret = IupPlotFindSample(ih, luaL_checknumber(L, 2), luaL_checknumber(L, 3), &ds_index, &sample_index);
  lua_pushinteger(L, ret);
  if (ret)
  {
    lua_pushinteger(L, ds_index);
    lua_pushinteger(L, sample_index);
    return 3;
  }
  else
    return 1;
}

static int PlotFindSegment(lua_State *L)
{
  Ihandle *ih = iuplua_checkihandle(L, 1);
  int ds_index, sample_index1, sample_index2;
  int ret = IupPlotFindSegment(ih, luaL_checknumber(L, 2), luaL_checknumber(L, 3), &ds_index, &sample_index1, &sample_index2);
  lua_pushinteger(L, ret);
  if (ret)
  {
    lua_pushinteger(L, ds_index);
    lua_pushinteger(L, sample_index1);
    lua_pushinteger(L, sample_index2);
    return 4;
  }
  else
    return 1;
}

static int PlotInsert(lua_State *L)
{
  Ihandle *ih = iuplua_checkihandle(L, 1);
  IupPlotInsert(ih, luaL_checkinteger(L, 2), luaL_checkinteger(L, 3), luaL_checknumber(L, 4), luaL_checknumber(L, 5));
  return 0;
}

static int PlotInsertStr(lua_State *L)
{
  Ihandle *ih = iuplua_checkihandle(L,1);
  IupPlotInsertStr(ih, luaL_checkinteger(L,2), luaL_checkinteger(L,3), luaL_checkstring(L,4), luaL_checknumber(L,5));
  return 0;
}

static int PlotInsertSegment(lua_State *L)
{
  Ihandle *ih = iuplua_checkihandle(L, 1);
  IupPlotInsertSegment(ih, luaL_checkinteger(L, 2), luaL_checkinteger(L, 3), luaL_checknumber(L, 4), luaL_checknumber(L, 5));
  return 0;
}

static int PlotInsertSamples(lua_State *L)
{
  double *px, *py;
  int count = luaL_checkinteger(L, 6);
  px = iuplua_checkdouble_array(L, 4, count);
  py = iuplua_checkdouble_array(L, 5, count);
  IupPlotInsertSamples(iuplua_checkihandle(L,1), luaL_checkinteger(L,2), luaL_checkinteger(L,3), px, py, count);
  free(px);
  free(py);
  return 0;
}

static int PlotInsertStrSamples(lua_State *L)
{
  double *py;
  char* *px;
  int count = luaL_checkinteger(L, 6);
  px = iuplua_checkstring_array(L, 4, count);
  py = iuplua_checkdouble_array(L, 5, count);
  IupPlotInsertStrSamples(iuplua_checkihandle(L,1), luaL_checkinteger(L,2), luaL_checkinteger(L,3), px, py, count);
  free(px);
  free(py);
  return 0;
}

static int PlotAddSamples(lua_State *L)
{
  double *px, *py;
  int count = luaL_checkinteger(L, 5);
  px = iuplua_checkdouble_array(L, 3, count);
  py = iuplua_checkdouble_array(L, 4, count);
  IupPlotAddSamples(iuplua_checkihandle(L,1), luaL_checkinteger(L,2), px, py, count);
  free(px);
  free(py);
  return 0;
}

static int PlotAddStrSamples(lua_State *L)
{
  double *py;
  char* *px;
  int count = luaL_checkinteger(L, 5);
  px = iuplua_checkstring_array(L, 3, count);
  py = iuplua_checkdouble_array(L, 4, count);
  IupPlotAddStrSamples(iuplua_checkihandle(L,1), luaL_checkinteger(L,2), px, py, count);
  free(px);
  free(py);
  return 0;
}

static int PlotGetSample(lua_State *L)
{
  Ihandle *ih = iuplua_checkihandle(L, 1);
  double x, y;
  IupPlotGetSample(ih, luaL_checkinteger(L, 2), luaL_checkinteger(L, 3), &x, &y);
  lua_pushnumber(L, x);
  lua_pushnumber(L, y);
  return 2;
}

static int PlotGetSampleStr(lua_State *L)
{
  Ihandle *ih = iuplua_checkihandle(L, 1);
  double y;
  const char* x;
  IupPlotGetSampleStr(ih, luaL_checkinteger(L, 2), luaL_checkinteger(L, 3), &x, &y);
  lua_pushstring(L, x);
  lua_pushnumber(L, y);
  return 2;
}

static int PlotGetSampleSelection(lua_State *L)
{
  Ihandle *ih = iuplua_checkihandle(L, 1);
  int selected = IupPlotGetSampleSelection(ih, luaL_checkinteger(L, 2), luaL_checkinteger(L, 3));
  lua_pushboolean(L, selected);
  return 1;
}

static int PlotGetSampleExtra(lua_State *L)
{
  Ihandle *ih = iuplua_checkihandle(L, 1);
  double extra = IupPlotGetSampleExtra(ih, luaL_checkinteger(L, 2), luaL_checkinteger(L, 3));
  lua_pushnumber(L, extra);
  return 1;
}

static int PlotSetSample(lua_State *L)
{
  Ihandle *ih = iuplua_checkihandle(L, 1);
  IupPlotSetSample(ih, luaL_checkinteger(L, 2), luaL_checkinteger(L, 3), luaL_checknumber(L, 4), luaL_checknumber(L, 5));
  return 0;
}

static int PlotSetSampleStr(lua_State *L)
{
  Ihandle *ih = iuplua_checkihandle(L, 1);
  IupPlotSetSampleStr(ih, luaL_checkinteger(L, 2), luaL_checkinteger(L, 3), luaL_checkstring(L, 4), luaL_checknumber(L, 5));
  return 0;
}

static int PlotSetSampleSelection(lua_State *L)
{
  Ihandle *ih = iuplua_checkihandle(L, 1);
  IupPlotSetSampleSelection(ih, luaL_checkinteger(L, 2), luaL_checkinteger(L, 3), luaL_checkinteger(L, 4));
  return 0;
}

static int PlotSetSampleExtra(lua_State *L)
{
  Ihandle *ih = iuplua_checkihandle(L, 1);
  IupPlotSetSampleExtra(ih, luaL_checkinteger(L, 2), luaL_checkinteger(L, 3), luaL_checknumber(L, 4));
  return 0;
}

static int PlotTransform(lua_State *L)
{
  Ihandle *ih = iuplua_checkihandle(L,1);
  double ix, iy;
  IupPlotTransform(ih, luaL_checknumber(L,2), luaL_checknumber(L,3), &ix, &iy);
  lua_pushnumber(L, ix);
  lua_pushnumber(L, iy);
  return 2;
}

static int PlotTransformTo(lua_State *L)
{
  Ihandle *ih = iuplua_checkihandle(L,1);
  double rx, ry;
  IupPlotTransformTo(ih, (int)luaL_checkinteger(L, 2), (int)luaL_checkinteger(L, 3), &rx, &ry);
  lua_pushnumber(L, rx);
  lua_pushnumber(L, ry);
  return 2;
}

static int PlotPaintTo(lua_State *L)
{
  Ihandle *ih = iuplua_checkihandle(L,1);
  IupPlotPaintTo(ih, cdlua_checkcanvas(L,2));
  return 0;
}

void iuplua_plotfuncs_open (lua_State *L)
{
  iuplua_register_cb(L, "PREDRAW_CB", (lua_CFunction)plot_predraw_cb, NULL);
  iuplua_register_cb(L, "POSTDRAW_CB", (lua_CFunction)plot_postdraw_cb, NULL); 

  iuplua_register(L, PlotBegin       ,"PlotBegin");
  iuplua_register(L, PlotAdd         ,"PlotAdd");
  iuplua_register(L, PlotAddStr      ,"PlotAddStr");
  iuplua_register(L, PlotAddSegment  ,"PlotAddSegment");
  iuplua_register(L, PlotEnd         ,"PlotEnd");
  iuplua_register(L, PlotLoadData    ,"PlotLoadData");
  iuplua_register(L, PlotSetFormula  ,"PlotSetFormula");
  iuplua_register(L, PlotFindSample  ,"PlotFindSample");
  iuplua_register(L, PlotFindSegment, "PlotFindSegment");
  iuplua_register(L, PlotInsert, "PlotInsert");
  iuplua_register(L, PlotInsertStr   ,"PlotInsertStr");
  iuplua_register(L, PlotInsertSegment, "PlotInsertSegment");
  iuplua_register(L, PlotInsertSamples    ,"PlotInsertSamples");
  iuplua_register(L, PlotInsertStrSamples ,"PlotInsertStrSamples");
  iuplua_register(L, PlotAddSamples       ,"PlotAddSamples");
  iuplua_register(L, PlotAddStrSamples    ,"PlotAddStrSamples");
  iuplua_register(L, PlotGetSample, "PlotGetSample");
  iuplua_register(L, PlotGetSampleStr, "PlotGetSampleStr");
  iuplua_register(L, PlotGetSampleSelection, "PlotGetSampleSelection");
  iuplua_register(L, PlotGetSampleExtra, "PlotGetSampleExtra");
  iuplua_register(L, PlotSetSample, "PlotSetSample");
  iuplua_register(L, PlotSetSampleStr, "PlotSetSampleStr");
  iuplua_register(L, PlotSetSampleSelection, "PlotSetSampleSelection");
  iuplua_register(L, PlotSetSampleExtra, "PlotSetSampleExtra");

  iuplua_register(L, PlotTransform, "PlotTransform");
  iuplua_register(L, PlotTransformTo, "PlotTransformTo");
  iuplua_register(L, PlotPaintTo, "PlotPaintTo");
}

int iupplotlua_open(lua_State * L);

int iup_plotlua_open(lua_State * L)
{
  if (iuplua_opencall_internal(L))
    IupPlotOpen();

  iuplua_get_env(L);
  iupplotlua_open(L);
  return 0;
}

/* obligatory to use require"iuplua_plot" */
int luaopen_iuplua_plot(lua_State* L)
{
  return iup_plotlua_open(L);
}

