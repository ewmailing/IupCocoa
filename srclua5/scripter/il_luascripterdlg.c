/******************************************************************************
 * Automatically generated file. Please don't change anything.                *
 *****************************************************************************/

#include <stdlib.h>

#include <lua.h>
#include <lauxlib.h>

#include "iup.h"
#include "iuplua.h"
#include "iupluascripterdlg.h"
#include "il.h"


static int LuaScripterDlg(lua_State *L)
{
  Ihandle *ih = IupLuaScripterDlg();
  iuplua_plugstate(L, ih);
  iuplua_pushihandle_raw(L, ih);
  return 1;
}

int iup_luascripterdlglua_open(lua_State * L)
{
  iuplua_register(L, LuaScripterDlg, "LuaScripterDlg");


#ifdef IUPLUA_USELOH
#include "luascripterdlg.loh"
#else
#ifdef IUPLUA_USELH
#include "luascripterdlg.lh"
#else
  iuplua_dofile(L, "luascripterdlg.lua");
#endif
#endif

  return 0;
}

 
int iupluascripterdlglua_open(lua_State* L)
{
  if (iuplua_opencall_internal(L))
    IupLuaScripterDlgOpen();
    
  iuplua_get_env(L);
  iup_luascripterdlglua_open(L);
  return 0;
}

/* obligatory to use require"iupluascripterdlg" */
int luaopen_iupluascripterdlg(lua_State* L)
{
  return iupluascripterdlglua_open(L);
}

