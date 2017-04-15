/** \file
 * \brief IupConfig binding to Lua 5.
 *
 * See Copyright Notice in "iup.h"
 */
 
#include <stdlib.h>
#include <string.h>

#include <lua.h>
#include <lauxlib.h>

#include "iup.h"
#include "iup_config.h"

#include "iuplua.h"
#include "il.h"


static int Config(lua_State *L)
{
  Ihandle* ih = IupConfig();
  iuplua_plugstate(L, ih);
  iuplua_pushihandle_raw(L, ih);
  return 1;
}

static int ConfigLoad(lua_State *L)
{
  Ihandle* ih = iuplua_checkihandle(L, 1);
  int ret = IupConfigLoad(ih);
  lua_pushinteger(L, ret);
  return 1;
}

static int ConfigSave(lua_State *L)
{
  Ihandle* ih = iuplua_checkihandle(L, 1);
  int ret = IupConfigSave(ih);
  lua_pushinteger(L, ret);
  return 1;
}

static int ConfigSetVariable(lua_State *L)
{
  Ihandle* ih = iuplua_checkihandle(L, 1);
  const char *group = luaL_checkstring(L, 2);
  const char *key = luaL_checkstring(L, 3);
  if (lua_isnil(L, 4))
    IupConfigSetVariableStr(ih, group, key, NULL);
  else
  {
    const char *value = luaL_checkstring(L, 4);
    IupConfigSetVariableStr(ih, group, key, value);
  }
  return 0;
}

static int ConfigSetVariableId(lua_State *L)
{
  Ihandle* ih = iuplua_checkihandle(L, 1);
  const char *group = luaL_checkstring(L, 2);
  const char *key = luaL_checkstring(L, 3);
  int id = (int)luaL_checkinteger(L, 4);
  if (lua_isnil(L, 5))
    IupConfigSetVariableStrId(ih, group, key, id, NULL);
  else
  {
    const char *value = luaL_checkstring(L, 5);
    IupConfigSetVariableStrId(ih, group, key, id, value);
  }
  return 0;
}

static int ConfigGetVariable(lua_State *L)
{
  Ihandle* ih = iuplua_checkihandle(L, 1);
  const char *group = luaL_checkstring(L, 2);
  const char *key = luaL_checkstring(L, 3);
  const char* value = IupConfigGetVariableStr(ih, group, key);
  if (!value)
    lua_pushnil(L);
  else
    lua_pushstring(L, value);
  return 1;
}

static int ConfigGetVariableId(lua_State *L)
{
  Ihandle* ih = iuplua_checkihandle(L, 1);
  const char *group = luaL_checkstring(L, 2);
  const char *key = luaL_checkstring(L, 3);
  int id = (int)luaL_checkinteger(L, 4);
  const char* value = IupConfigGetVariableStrId(ih, group, key, id);
  if (!value)
    lua_pushnil(L);
  else
    lua_pushstring(L, value);
  return 1;
}

static int ConfigGetVariableDef(lua_State *L)
{
  Ihandle* ih = iuplua_checkihandle(L, 1);
  const char *group = luaL_checkstring(L, 2);
  const char *key = luaL_checkstring(L, 3);
  const char *def = luaL_optstring(L, 4, NULL);
  const char* value = IupConfigGetVariableStrDef(ih, group, key, def);
  if (!value)
    lua_pushnil(L);
  else
    lua_pushstring(L, value);
  return 1;
}

static int ConfigGetVariableIdDef(lua_State *L)
{
  Ihandle* ih = iuplua_checkihandle(L, 1);
  const char *group = luaL_checkstring(L, 2);
  const char *key = luaL_checkstring(L, 3);
  int id = (int)luaL_checkinteger(L, 4);
  const char *def = luaL_optstring(L, 4, NULL);
  const char* value = IupConfigGetVariableStrIdDef(ih, group, key, id, def);
  if (!value)
    lua_pushnil(L);
  else
    lua_pushstring(L, value);
  return 1;
}

static int config_recent_cb(Ihandle* ih)
{
  lua_State *L = iuplua_call_start(ih, "recent_cb");
  return iuplua_call(L, 0);
}

static int ConfigRecentInit(lua_State *L)
{
  Ihandle* ih = iuplua_checkihandle(L, 1);
  Ihandle* menu = iuplua_checkihandle(L, 2);
  int max_recent = (int)luaL_checkinteger(L, 3);

  IupConfigRecentInit(ih, menu, config_recent_cb, max_recent);
  return 0;
}

static int ConfigRecentUpdate(lua_State *L)
{
  Ihandle* ih = iuplua_checkihandle(L, 1);
  const char *filename = luaL_checkstring(L, 2);
  IupConfigRecentUpdate(ih, filename);
  return 0;
}

static int ConfigDialogShow(lua_State *L)
{
  Ihandle* ih = iuplua_checkihandle(L, 1);
  Ihandle* dialog = iuplua_checkihandle(L, 2);
  const char *name = luaL_checkstring(L, 3);
  IupConfigDialogShow(ih, dialog, name);
  return 0;
}

static int ConfigDialogClosed(lua_State *L)
{
  Ihandle* ih = iuplua_checkihandle(L, 1);
  Ihandle* dialog = iuplua_checkihandle(L, 2);
  const char *name = luaL_checkstring(L, 3);
  IupConfigDialogClosed(ih, dialog, name);
  return 0;
}

void iupconfiglua_open(lua_State * L)
{
  iuplua_register_cb(L, "RECENT_CB", (lua_CFunction)config_recent_cb, NULL);

  iuplua_register(L, Config, "Config");

  iuplua_register(L, ConfigSetVariable, "ConfigSetVariable");
  iuplua_register(L, ConfigSetVariableId, "ConfigSetVariableId");
  iuplua_register(L, ConfigGetVariable, "ConfigGetVariable");
  iuplua_register(L, ConfigGetVariableId, "ConfigGetVariableId");
  iuplua_register(L, ConfigGetVariableDef, "ConfigGetVariableDef");
  iuplua_register(L, ConfigGetVariableIdDef, "ConfigGetVariableIdDef");

  iuplua_register(L, ConfigLoad, "ConfigLoad");
  iuplua_register(L, ConfigSave, "ConfigSave");

  iuplua_register(L, ConfigRecentInit, "ConfigRecentInit");
  iuplua_register(L, ConfigRecentUpdate, "ConfigRecentUpdate");

  iuplua_register(L, ConfigDialogShow, "ConfigDialogShow");
  iuplua_register(L, ConfigDialogClosed, "ConfigDialogClosed");

#ifdef IUPLUA_USELOH        
#include "iup_config.loh"
#else
#ifdef IUPLUA_USELH
#include "iup_config.lh"
#else
  iuplua_dofile(L, "iup_config.lua");
#endif
#endif
}
