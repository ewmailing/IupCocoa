
#include <stdlib.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "iup.h"
#include "iup_config.h"

#include "iuplua.h"
#include "iupluascripterdlg.h"

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
#include "iuplua_scintilla.h"

int luaopen_iupluaimglib(lua_State* L);
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

  luaopen_iupluaimglib(L);

#ifdef IUPLUA_TUIO
  iuptuiolua_open(L);
#endif
#ifdef IUPLUA_WEB
  iupweblua_open(L);
#endif

  iup_scintillalua_open(L);

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

static int item_help_action_cb(void)
{
  IupHelp("http://www.tecgraf.puc-rio.br/iup");
  return IUP_DEFAULT;
}

static int item_about_action_cb(void)
{
  IupMessage("About", "   IupLua Scripter\n\nAutors:\n   Camilo Freire\n   Antonio Scuri");
  return IUP_DEFAULT;
}

static void createargtable(lua_State *L, char **argv, int argc)
{
  /* try to mimic lua.c */
  int i;
   
  if (argc < 2) /* arg table is not set if no arguments */
    return;

  /* arg[-1] = argv[0] -- executable */
  /* arg[0] = argv[1] -- filename */
  /* arg[1] = argv[2] -- and so on... */

  lua_createtable(L, argc - 2, 2);
  for (i = 0; i < argc; i++)
  {
    lua_pushstring(L, argv[i]);
    lua_rawseti(L, -2, i - 1);
  }
  lua_setglobal(L, "arg");
}

int main(int argc, char **argv)
{
  Ihandle *config;
  lua_State *L;
  Ihandle *menu;
  Ihandle *main_dialog;
  int i;

  L = luaL_newstate();
  luaL_openlibs(L);

  createargtable(L, argv, argc);  /* create table 'arg' */

  iuplua_openlibs(L);
  IupLuaScripterDlgOpen();

  IupSetGlobal("GLOBALLAYOUTDLGKEY", "Yes");

  config = IupConfig();
  IupSetAttribute(config, "APP_NAME", "iupluascripter");
  IupConfigLoad(config);

  main_dialog = IupLuaScripterDlg();

  IupSetAttribute(main_dialog, "SUBTITLE", "IupLuaScripter");
  IupSetAttributeHandle(main_dialog, "CONFIG", config);

  menu = IupGetAttributeHandle(main_dialog, "MENU");
  IupAppend(menu, IupSubmenu("&Help", IupMenu(
    IupSetCallbacks(IupItem("&Help...", NULL), "ACTION", (Icallback)item_help_action_cb, NULL),
    IupSetCallbacks(IupItem("&About...", NULL), "ACTION", (Icallback)item_about_action_cb, NULL),
    NULL)));

  /* show the dialog at the last position, with the last size */
  IupConfigDialogShow(config, main_dialog, IupGetAttribute(main_dialog, "SUBTITLE"));

  /* open a file from the command line (allow file association in Windows) */
  for (i = 1; i < argc; i++)
  {
    const char* filename = argv[i];
    IupSetStrAttribute(main_dialog, "OPENFILE", filename);
  }

  IupSetGlobal("EXITLOOP", "NO");
  IupMainLoop();

  IupDestroy(config);
  IupDestroy(main_dialog);

  IupClose();

  lua_close(L);
  return EXIT_SUCCESS;
}
