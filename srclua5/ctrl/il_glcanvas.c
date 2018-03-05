/******************************************************************************
 * Automatically generated file. Please don't change anything.                *
 *****************************************************************************/

#include <stdlib.h>

#include <lua.h>
#include <lauxlib.h>

#include "iup.h"
#include "iuplua.h"
#include "iupgl.h"
#include "il.h"


static int glcanvas_action(Ihandle *self, float p0, float p1)
{
  lua_State *L = iuplua_call_start(self, "action");
  lua_pushnumber(L, p0);
  lua_pushnumber(L, p1);
  return iuplua_call(L, 2);
}

static int glcanvas_swapbuffers_cb(Ihandle *self)
{
  lua_State *L = iuplua_call_start(self, "swapbuffers_cb");
  return iuplua_call(L, 0);
}

static int GLCanvas(lua_State *L)
{
  Ihandle *ih = IupGLCanvas(NULL);
  iuplua_plugstate(L, ih);
  iuplua_pushihandle_raw(L, ih);
  return 1;
}

void iuplua_glcanvasfuncs_open(lua_State *L);

int iupglcanvaslua_open(lua_State * L)
{
  iuplua_register(L, GLCanvas, "GLCanvas");

  iuplua_register_cb(L, "ACTION", (lua_CFunction)glcanvas_action, "glcanvas");
  iuplua_register_cb(L, "SWAPBUFFERS_CB", (lua_CFunction)glcanvas_swapbuffers_cb, NULL);

  iuplua_glcanvasfuncs_open(L);

#ifdef IUPLUA_USELOH
#include "glcanvas.loh"
#else
#ifdef IUPLUA_USELH
#include "glcanvas.lh"
#else
  iuplua_dofile(L, "glcanvas.lua");
#endif
#endif

  return 0;
}

 
int iupglbackgroundboxlua_open(lua_State * L);

int iupgllua_open(lua_State * L)
{
  if (iuplua_opencall_internal(L))
    IupGLCanvasOpen();

  iuplua_get_env(L);
  iupglcanvaslua_open(L);
  iupglbackgroundboxlua_open(L);
  return 0;
}

/* obligatory to use require"iupluagl" */
int luaopen_iupluagl(lua_State* L)
{
  return iupgllua_open(L);
}

