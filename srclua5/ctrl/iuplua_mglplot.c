/** \file
 * \brief mglplot binding for Lua 5.
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdlib.h>

#include <lua.h>
#include <lauxlib.h>

#include "iup.h"
#include "iup_mglplot.h"

#include "iuplua.h"
#include "iuplua_mglplot.h"
#include "il.h"

static int PlotBegin(lua_State *L)
{
  Ihandle *ih = iuplua_checkihandle(L,1);
  IupMglPlotBegin(ih, luaL_checkinteger(L,2));
  return 0;
}

static int PlotAdd1D(lua_State *L)
{
  Ihandle *ih = iuplua_checkihandle(L,1);
  IupMglPlotAdd1D(ih, luaL_checkstring(L,2), luaL_checknumber(L,3));
  return 0;
}

static int PlotAdd2D(lua_State *L)
{
  Ihandle *ih = iuplua_checkihandle(L,1);
  IupMglPlotAdd2D(ih, luaL_checknumber(L,2), luaL_checknumber(L,3));
  return 0;
}

static int PlotAdd3D(lua_State *L)
{
  Ihandle *ih = iuplua_checkihandle(L,1);
  IupMglPlotAdd3D(ih, luaL_checknumber(L,2), luaL_checknumber(L,3), luaL_checknumber(L,4));
  return 0;
}

static int PlotEnd(lua_State *L)
{
  Ihandle *ih = iuplua_checkihandle(L,1);
  int ret = IupMglPlotEnd(ih);
  lua_pushinteger(L, ret);
  return 1;
}

static int PlotInsert1D(lua_State *L)
{
  double *py;
  char* *px;
  int count = luaL_checkinteger(L, 6);
  px = iuplua_checkstring_array(L, 4, count);
  py = iuplua_checkdouble_array(L, 5, count);
  IupMglPlotInsert1D(iuplua_checkihandle(L,1), luaL_checkinteger(L,2), luaL_checkinteger(L,3), px, py, count);
  free(px);
  free(py);
  return 0;
}

static int PlotInsert2D(lua_State *L)
{
  double *px, *py;
  int count = luaL_checkinteger(L, 6);
  px = iuplua_checkdouble_array(L, 4, count);
  py = iuplua_checkdouble_array(L, 5, count);
  IupMglPlotInsert2D(iuplua_checkihandle(L,1), luaL_checkinteger(L,2), luaL_checkinteger(L,3), px, py, count);
  free(px);
  free(py);
  return 0;
}

static int PlotInsert3D(lua_State *L)
{
  double *px, *py, *pz;
  int count = luaL_checkinteger(L, 7);
  px = iuplua_checkdouble_array(L, 4, count);
  py = iuplua_checkdouble_array(L, 5, count);
  pz = iuplua_checkdouble_array(L, 6, count);
  IupMglPlotInsert3D(iuplua_checkihandle(L,1), luaL_checkinteger(L,2), luaL_checkinteger(L,3), px, py, pz, count);
  free(px);
  free(py);
  free(pz);
  return 0;
}

static int PlotSet1D(lua_State *L)
{
  double *py;
  char* *px;
  int count = luaL_checkinteger(L, 5);
  px = iuplua_checkstring_array(L, 3, count);
  py = iuplua_checkdouble_array(L, 4, count);
  IupMglPlotSet1D(iuplua_checkihandle(L,1), luaL_checkinteger(L,2), px, py, count);
  free(px);
  free(py);
  return 0;
}

static int PlotSet2D(lua_State *L)
{
  double *px, *py;
  int count = luaL_checkinteger(L, 5);
  px = iuplua_checkdouble_array(L, 3, count);
  py = iuplua_checkdouble_array(L, 4, count);
  IupMglPlotSet2D(iuplua_checkihandle(L,1), luaL_checkinteger(L,2), px, py, count);
  free(px);
  free(py);
  return 0;
}

static int PlotSet3D(lua_State *L)
{
  double *px, *py, *pz;
  int count = luaL_checkinteger(L, 6);
  px = iuplua_checkdouble_array(L, 3, count);
  py = iuplua_checkdouble_array(L, 4, count);
  pz = iuplua_checkdouble_array(L, 5, count);
  IupMglPlotSet3D(iuplua_checkihandle(L,1), luaL_checkinteger(L,2), px, py, pz, count);
  free(px);
  free(py);
  free(pz);
  return 0;
}

static int PlotSetFormula(lua_State *L)
{
  IupMglPlotSetFormula(iuplua_checkihandle(L,1), luaL_checkinteger(L,2), luaL_checkstring(L,3), luaL_checkstring(L,4), luaL_checkstring(L,5), luaL_checkinteger(L,6));
  return 0;
}

static int PlotLoadData(lua_State *L)
{
  IupMglPlotLoadData(iuplua_checkihandle(L,1), luaL_checkinteger(L,2), luaL_checkstring(L,3), luaL_checkinteger(L,4), luaL_checkinteger(L,5), luaL_checkinteger(L,6));
  return 0;
}

static int PlotSetFromFormula(lua_State *L)
{
  IupMglPlotSetFromFormula(iuplua_checkihandle(L,1), luaL_checkinteger(L,2), luaL_checkstring(L,3), luaL_checkinteger(L,4), luaL_checkinteger(L,5), luaL_checkinteger(L,6));
  return 0;
}

static int PlotSetData(lua_State *L)
{
  int count_x = luaL_checkinteger(L,4), 
      count_y = luaL_checkinteger(L,5), 
      count_z = luaL_checkinteger(L,6);
  double* data = iuplua_checkdouble_array(L, 3, count_x*count_y*count_z);
  IupMglPlotSetData(iuplua_checkihandle(L,1), luaL_checkinteger(L,2), data, count_x, count_y, count_z);
  free(data);
  return 0;
}

static int PlotTransform(lua_State *L)
{
  Ihandle *ih = iuplua_checkihandle(L,1);
  int ix, iy;
  IupMglPlotTransform(ih, luaL_checknumber(L,2), luaL_checknumber(L,3), luaL_checknumber(L,4), &ix, &iy);
  lua_pushinteger(L, ix);
  lua_pushinteger(L, iy);
  return 2;
}

static int PlotTransformTo(lua_State *L)
{
  Ihandle *ih = iuplua_checkihandle(L,1);
  double x, y, z;
  IupMglPlotTransformTo(ih, luaL_checkinteger(L,2), luaL_checkinteger(L,3), &x, &y, &z);
  lua_pushnumber(L, x);
  lua_pushnumber(L, y);
  lua_pushnumber(L, z);
  return 3;
}

static int PlotNewDataSet(lua_State *L)
{
  Ihandle *ih = iuplua_checkihandle(L,1);
  int ret = IupMglPlotNewDataSet(ih, luaL_checkinteger(L,2));
  lua_pushinteger(L, ret);
  return 1;
}

static int PlotPaintTo(lua_State *L)
{
  Ihandle *ih = iuplua_checkihandle(L,1);
  const char* format = luaL_checkstring(L,2);
  void* data = NULL;

  if (*format == 'E' || *format == 'S') //EPS or SVG
    data = (void*)luaL_checkstring(L, 6);
  else if (*format == 'R') //RGB
  {
    if (!lua_islightuserdata(L, 6))
      luaL_argerror(L, 6, "data must be a light user data");
    data = lua_touserdata(L, 6);
  }
  else
    luaL_argerror(L, 2, "invalid format");

  IupMglPlotPaintTo(ih, format, luaL_checkinteger(L,3), luaL_checkinteger(L,4), luaL_checknumber(L,5), data);
  return 0;
}

static int PlotDrawMark(lua_State *L)
{
  Ihandle *ih = iuplua_checkihandle(L,1);
  IupMglPlotDrawMark(ih, luaL_checknumber(L,2), luaL_checknumber(L,3), luaL_checknumber(L,4));
  return 0;
}

static int PlotDrawLine(lua_State *L)
{
  Ihandle *ih = iuplua_checkihandle(L,1);
  IupMglPlotDrawLine(ih, luaL_checknumber(L,2), luaL_checknumber(L,3), luaL_checknumber(L,4),
                         luaL_checknumber(L,5), luaL_checknumber(L,6), luaL_checknumber(L,7));
  return 0;
}

static int PlotDrawText(lua_State *L)
{
  Ihandle *ih = iuplua_checkihandle(L,1);
  IupMglPlotDrawText(ih, luaL_checkstring(L,2), luaL_checknumber(L,3), luaL_checknumber(L,4), luaL_checknumber(L,5));
  return 0;
}

int iupmgllabellua_open(lua_State * L);

void iuplua_mglplotfuncs_open(lua_State *L)
{
  iupmgllabellua_open(L);

  iuplua_register(L, PlotBegin       ,"MglPlotBegin");
  iuplua_register(L, PlotAdd1D       ,"MglPlotAdd1D");
  iuplua_register(L, PlotAdd2D       ,"MglPlotAdd2D");
  iuplua_register(L, PlotAdd3D       ,"MglPlotAdd3D");
  iuplua_register(L, PlotEnd         ,"MglPlotEnd");
  iuplua_register(L, PlotNewDataSet  ,"MglPlotNewDataSet");
  iuplua_register(L, PlotInsert1D    ,"MglPlotInsert1D");
  iuplua_register(L, PlotInsert2D    ,"MglPlotInsert2D");
  iuplua_register(L, PlotInsert3D    ,"MglPlotInsert3D");
  iuplua_register(L, PlotSet1D       ,"MglPlotSet1D");
  iuplua_register(L, PlotSet2D       ,"MglPlotSet2D");
  iuplua_register(L, PlotSet3D       ,"MglPlotSet3D");
  iuplua_register(L, PlotSetFormula  ,"MglPlotSetFormula");
  iuplua_register(L, PlotSetData     ,"MglPlotSetData");
  iuplua_register(L, PlotLoadData    ,"MglPlotLoadData");
  iuplua_register(L, PlotSetFromFormula,"MglPlotSetFromFormula");
  iuplua_register(L, PlotTransform   ,"MglPlotTransform");
  iuplua_register(L, PlotTransformTo ,"MglPlotTransformTo");
  iuplua_register(L, PlotDrawMark    ,"MglPlotDrawMark");
  iuplua_register(L, PlotDrawLine    ,"MglPlotDrawLine");
  iuplua_register(L, PlotDrawText    ,"MglPlotDrawText");
  iuplua_register(L, PlotPaintTo     ,"MglPlotPaintTo");
}

int iupmglplotlua_open(lua_State * L);

int iup_mglplotlua_open(lua_State * L)
{
  if (iuplua_opencall_internal(L))
    IupMglPlotOpen();

  iuplua_get_env(L);
  iupmglplotlua_open(L);
  return 0;
}

/* obligatory to use require"iuplua_mglplot" */
int luaopen_iuplua_mglplot(lua_State* L)
{
  return iup_mglplotlua_open(L);
}

