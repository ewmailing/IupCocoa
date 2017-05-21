
#include <stdlib.h>

#include "lua.h"

#include "lauxlib.h"
#include "lualib.h"

#include "iup.h"
#include "iuplua.h"

#ifdef USE_STATIC

#ifndef IUPLUA_NO_GL
#include "iupgl.h"
#include "iupglcontrols.h"
#include "iupluagl.h"
#include "iupluaglcontrols.h"
#ifdef USE_LUAGL
#include "luagl.h"
#endif
#endif

#ifndef IUPLUA_NO_CD
#include "iupcontrols.h"
#include "iupluacontrols.h"
#include "iupmatrixex.h"
#include "iupluamatrixex.h"
#include "iup_plot.h"
#include "iuplua_plot.h"
#include <cd.h>
#include <cdlua.h>
#include <cdluaiup.h>
#endif

#ifndef IUPLUA_NO_IM
#include "iupluaim.h"
#include <im.h>
#include <im_image.h>
#include <imlua.h>
#endif

#ifndef IUPLUA_NO_IM
#ifndef IUPLUA_NO_CD
#include <cdluaim.h>
#endif
#endif

#ifdef IUPLUA_TUIO
#include "iupluatuio.h"
#endif
#ifdef IUPLUA_WEB
#include "iupluaweb.h"
#endif
#ifdef IUPLUA_SCINTILLA
#include "iuplua_scintilla.h"
#endif

#endif

#ifdef USE_STATIC
#ifdef IUPLUA_IMGLIB
int luaopen_iupluaimglib(lua_State* L);
#endif
#endif

static void iuplua_openlibs(lua_State *L) 
{
#if LUA_VERSION_NUM < 502
  lua_pushliteral(L, LUA_RELEASE "  " LUA_COPYRIGHT);
#else
  lua_pushliteral(L, LUA_COPYRIGHT);
#endif
  lua_setglobal(L, "_COPYRIGHT");  /* set global _COPYRIGHT */

  /* iuplua initialization */
  iuplua_open(L);

#ifdef USE_STATIC
  /* disable require */
  iuplua_dostring(L, "function require() end ", "static_require");

#ifdef IUPLUA_IMGLIB
  luaopen_iupluaimglib(L);
#endif
#ifdef IUPLUA_TUIO
  iuptuiolua_open(L);
#endif
#ifdef IUPLUA_WEB
  iupweblua_open(L);
#endif
#ifdef IUPLUA_SCINTILLA
  iup_scintillalua_open(L);
#endif

/* luaopen_lfs(L); */

#ifndef IUPLUA_NO_GL
  iupgllua_open(L);
  iupglcontrolslua_open(L);
#ifdef USE_LUAGL
  luaopen_luagl(L);
#endif
#endif
#ifndef IUPLUA_NO_CD
  iupcontrolslua_open(L);
  iupmatrixexlua_open(L);
  iup_plotlua_open(L);
  cdlua_open(L);
  cdluaiup_open(L);
  cdInitContextPlus();
#endif
#ifndef IUPLUA_NO_IM
  iupimlua_open(L);
  imlua_open(L);
  imlua_open_process(L);
#endif
#ifndef IUPLUA_NO_IM
#ifndef IUPLUA_NO_CD
  cdluaim_open(L);
#endif
#endif
#endif
}

static void iuplua_input (lua_State *L) 
{
#ifdef IUPLUA_USELOH
  /* #include "indent.loh" */
#include "console5.loh"
#else
#ifdef IUPLUA_USELH
  /* #include "indent.lh" */
#include "console5.lh"
#else
  /*  iuplua_dofile(L, "indent.lua"); */
  iuplua_dofile(L, "console5.lua");
#endif
#endif
}

static void createargtable(lua_State *L, char **argv, int argc)
{
  int i, narg = argc - 1;  /* number of positive indices (non-zero) */
  lua_createtable(L, narg, 1);
  for (i = 0; i < argc; i++)
  {
    lua_pushstring(L, argv[i]);
    lua_rawseti(L, -2, i);
  }
  lua_setglobal(L, "arg");
}

int main (int argc, char **argv) 
{
  lua_State *L = luaL_newstate();  /* create state */

  luaL_openlibs(L);  /* open standard libraries */

  iuplua_openlibs(L);

  createargtable(L, argv, argc);  /* create table 'arg' */

  iuplua_input(L);

  lua_close(L);
  return EXIT_SUCCESS;
}
