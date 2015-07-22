/** \file
 * \brief IULUA core - Bindig of iup to Lua 3.
 *
 * See Copyright Notice in "iup.h"
 */
 
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <lua.h>
#include <lauxlib.h>

#include "iup.h"
#include "iupkey.h"
#include "iup_str.h"

#include "iuplua.h"
#include "il.h"

#include "iup_str.h"


/* from iupkey.c */
void iupKeyForEach(void (*func)(char *name, int code, void* user_data), void* user_data);

static int iuplua_tag = 0;
static lua_Object iuplua_namespace;


void iuplua_call_start(Ihandle* handle, char* name)
{                              
  lua_beginblock();
  lua_pushstring(name);
  lua_pushusertag(handle, iuplua_tag);
}

int iuplua_call(void)
{
  lua_Object obj;

  if (lua_call("iupCallMethod")) 
  {
    lua_endblock();
    return IUP_DEFAULT;  /* call failed */
  }

  obj = lua_getresult(1);
  if (obj == LUA_NOOBJECT) 
  {
    lua_endblock();
    return IUP_DEFAULT; /* no return, provides IUP_DEFAULT return */
  } 
  else if (lua_isnumber(obj)) 
  {
    int ret = (int) lua_getnumber(obj);
    lua_endblock();
    return ret;
  }
  lua_endblock();
  return IUP_DEFAULT; /* returned a non normal value. what to do? */
}

char* iuplua_call_rs(void)
{
  lua_Object obj;

  if (lua_call("iupCallMethod")) 
  {
    lua_endblock();
    return NULL;  /* call failed */
  }

  obj = lua_getresult(1);
  if (obj == LUA_NOOBJECT) 
  {
    lua_endblock();
    return NULL; /* no return, provides IUP_DEFAULT return */
  } 
  else if (lua_isstring(obj)) 
  {
    char* ret = lua_getstring(obj);
    lua_endblock();
    return ret;
  }
  lua_endblock();
  return NULL; /* returned a non normal value. what to do? */
}

static void iuplua_set_callback(void)
{
  lua_Object o;
  Icallback func;

  Ihandle* ih = iuplua_checkihandle(1);
  char* name = luaL_check_string(2);

  o = lua_getparam(3);
  if (!lua_iscfunction(o))
    lua_error("invalid function when set callback");
  func = (Icallback)lua_getcfunction(o);

  o = lua_getparam(4);
  if (lua_isnil(o))
    IupSetCallback(ih, name, (Icallback)NULL);
  else
    IupSetCallback(ih, name, func);
}

/***********************************************************************************
***********************************************************************************/

static int idle_ref = 0;
static int default_idle(void)
{
  lua_Object obj;
  lua_beginblock();
  obj = lua_getref(idle_ref);
  if (lua_callfunction(obj)) 
  {
    lua_endblock();
    return IUP_IGNORE;
  }

  obj = lua_getresult(1);
  if (lua_isnumber(obj)) 
  {
    int ret = (int) lua_getnumber(obj);
    lua_endblock();
    return ret;
  }
  lua_endblock();
  return IUP_DEFAULT;
}

static void IupSetIdle(void)
{
  lua_Object obj = lua_getparam(1);
  lua_Object old;
  if (idle_ref != 0)
    old = lua_getref(idle_ref);
  else
    old = LUA_NOOBJECT;
  if (lua_isnil(obj)) 
  {
    lua_unref(idle_ref);
    idle_ref = 0;
    IupSetFunction("IDLE_ACTION", (Icallback) NULL);
  } 
  else 
  {
    if (!lua_isfunction(obj) && !lua_iscfunction(obj))
      lua_error("IupSetIdle: parameter must be a function or a cfunction");
    lua_pushobject(obj);
    idle_ref = lua_ref(1);
    IupSetFunction("IDLE_ACTION", (Icallback) default_idle);
  }

  if (old == LUA_NOOBJECT)
    lua_pushnil();
  else
    lua_pushobject(old);
}

/***********************************************************************************
***********************************************************************************/

static void iuplua_gettable(void)
{
  lua_Object t, index, tab, result;
  t = lua_getparam(1);
  index = lua_getparam(2);
  lua_pushobject(lua_getglobal("iup_handles"));
  lua_pushobject(t);
  tab = lua_gettable();
  if (lua_isnil(tab)) 
  {
    lua_pushnil();
    return;
  } 
  else 
  {
    lua_Object method;
    lua_pushobject(tab);
    lua_pushstring("get");
    method = lua_gettable();
    lua_pushobject(tab);
    lua_pushobject(index);
    if (lua_callfunction(method))
      return;
  }

  result = lua_getresult(1);
  if (result != LUA_NOOBJECT)
    lua_pushobject(result);
  else
    lua_pushnil();
}

static void iuplua_settable(void)
{
  lua_Object t, index, value, tab, result;
  t = lua_getparam(1);
  index = lua_getparam(2);
  value = lua_getparam(3);
  lua_pushobject(lua_getglobal("iup_handles"));
  lua_pushobject(t);
  tab = lua_gettable();
  if (lua_isnil(tab)) 
  {
    lua_pushnil();
    return;
  } 
  else 
  {
    lua_Object method;
    lua_pushobject(tab);
    lua_pushstring("set");
    method = lua_gettable();
    lua_pushobject(tab);
    lua_pushobject(index);
    lua_pushobject(value);
    if (lua_callfunction(method))
      return;
  }

  result = lua_getresult(1);
  if (result != LUA_NOOBJECT)
    lua_pushobject(result);
}

static void iuplua_index(void)
{
  lua_Object t, f, result;
  t = lua_getparam(1);
  f = lua_getparam(2);
  if (!(lua_isstring(f) && iupStrEqual(lua_getstring(f), "parent"))) 
  {
    lua_Object p;
    lua_pushobject(t);
    lua_pushstring("parent");
    p = lua_gettable();
    if (lua_istable(p)) 
    {
      lua_pushobject(p);
      lua_pushobject(f);
      result = lua_gettable();
      if (lua_isnil(result))
        lua_pushnil();
      else
        lua_pushobject(result);
      return;
    }
  }
  lua_pushnil();
}

/***********************************************************************************
***********************************************************************************/

static void iuplua_setnamespace(char* name, char* new_name)
{
  lua_Object obj = lua_getglobal(name);
  lua_pushobject(iuplua_namespace);
  lua_pushstring(new_name);
  lua_pushobject(obj);
  lua_settable();
}

void iuplua_register(char* name, lua_CFunction func)
{
  lua_register(name, func);
  iuplua_setnamespace(name, name+3);  /* IupXxx */
}

void iuplua_register_macro(char* name, lua_CFunction func)
{
  lua_register(name, func);
  iuplua_setnamespace(name, name);  /* xxx */
}

void iuplua_regstring_mask(char* str, char* name)
{
  lua_pushstring(str); lua_setglobal(name);
  iuplua_setnamespace(name, name+3);     /* IUPMASK_XXX */
}

void iuplua_regnumber(double num, char* name)
{
  lua_pushnumber(num); lua_setglobal(name);
  iuplua_setnamespace(name, name+4);     /* IUP_XXXX */
}

void iuplua_regstring(char* str, char* name)
{
  lua_pushstring(str); lua_setglobal(name);
  iuplua_setnamespace(name, name+4);     /* IUP_XXXX */
}

void iuplua_regihandle(Ihandle *h)
{
  if (h)
  {
    lua_pushusertag((void*)h, iuplua_tag);
    lua_pushstring(IupGetClassName(h));
    lua_call("IupRegisterHandle");
  }
}

void iuplua_pushihandle_cb(Ihandle *h)
{
  if (h)
    lua_pushusertag((void*)h, iuplua_tag);
  else
    lua_pushnil();
}

void iuplua_pushihandle(Ihandle *h)
{
  if (h)
  {
    lua_pushusertag((void*)h, iuplua_tag);
    lua_pushstring(IupGetClassName(h));
    lua_call("IupRegisterHandle");
    lua_pushobject(lua_getresult(1));
  }
  else
    lua_pushnil();
}

Ihandle* iuplua_checkihandle(int numArg)
{
  lua_Object o = lua_getparam(numArg);
  luaL_arg_check(lua_tag(o)==iuplua_tag, numArg, "handle expected");
  return (Ihandle*)lua_getuserdata(o);
}

static Ihandle* checkihandle_table(lua_Object tab, int index)
{
  Ihandle* ih;
  lua_Object o;
  lua_beginblock();
  lua_pushobject(tab);
  lua_pushnumber(index);
  o = lua_gettable();
  luaL_arg_check(lua_tag(o)==iuplua_tag, index, "handle expected");
  ih = (Ihandle*)lua_getuserdata(o);
  lua_endblock();
  return ih;
}

Ihandle** iuplua_checkihandle_array(int numArg)
{
  int i;
  lua_Object tab = luaL_tablearg(numArg);
  int count = luaL_check_int(numArg+1);

  Ihandle **params = (Ihandle **) malloc(sizeof(Ihandle *) * (count+1));

  for (i = 0; i < count; i++)
    params[i] = checkihandle_table(tab, i+1);
  params[count] = NULL;

  return params;
}

int iuplua_dofile(char *filename)
{
  if (lua_dofile(filename) != 0) 
  {
    char *dir = getenv("IUPLUA_DIR");
    if (!dir)
      return 0;
    else 
    {
      char* full_name = iupStrFileMakeFileName(dir, filename);
      if (!full_name)
        return 0;

      if (lua_dofile(full_name)) 
      {
        free(full_name);
        return 0;
      }
      free(full_name);
    }
  }
  return 1;
}

/***********************************************************************************
***********************************************************************************/

static void register_key(char *name, int code, void* user_data)
{
  (void)user_data;
  lua_pushnumber(code); lua_setglobal(name);
  iuplua_setnamespace(name, name);     /* K_xxx */
}

int iupkey_open(void)
{
  /* does nothing, kept for backward compatibility */
  return 1;
}

/***********************************************************************************
***********************************************************************************/

static void setinfo(void) 
{
  iuplua_regstring_mask(IUP_COPYRIGHT, "IUP_COPYRIGHT");
  iuplua_regstring_mask("Portable toolkit for building graphical user interfaces.", "IUP_DESCRIPTION");
  iuplua_regstring_mask("IUP - Portable User Interface", "IUP_NAME");
  iuplua_regstring_mask("IUP "IUP_VERSION, "IUP_VERSION");
  iuplua_regstring_mask(LUA_COPYRIGHT, "IUP_LUA_COPYRIGHT");
}

int iuplua_open(void)
{
  struct FuncList {
    char *name;
    lua_CFunction func;
  } FuncList[] = {
    { "IupSetIdle", IupSetIdle },
  };
  int SizeFuncList = (sizeof(FuncList)/sizeof(struct FuncList));
  int i;

  iuplua_tag = lua_newtag();
  lua_pushnumber(iuplua_tag);
  lua_setglobal("iuplua_tag");

  iuplua_namespace = lua_createtable();
  lua_pushobject(iuplua_namespace); lua_setglobal ("iup");

  setinfo();

  for (i = 0; i < SizeFuncList; i++)
    iuplua_register(FuncList[i].name, FuncList[i].func);

  lua_register("iup_gettable", iuplua_gettable);
  lua_register("iup_settable", iuplua_settable);
  lua_register("iup_index", iuplua_index);
  lua_register("iupSetCallback", iuplua_set_callback);

  iupKeyForEach(register_key, NULL);

  iupluaapi_open();

#ifdef IUPLUA_USELOH
#ifdef TEC_BIGENDIAN
#ifdef TEC_64
#include "loh/iuplua_be64.loh"
#include "loh/constants_be64.loh"
#else
#include "loh/iuplua_be32.loh"
#include "loh/constants_be32.loh"
#endif  
#else
#ifdef TEC_64
#ifdef WIN64
#include "loh/iuplua_le64w.loh"
#include "loh/constants_le64w.loh"
#else
#include "loh/iuplua_le64.loh"
#include "loh/constants_le64.loh"
#endif  
#else
#include "loh/iuplua.loh"
#include "loh/constants.loh"
#endif  
#endif  
#else
  iuplua_dofile("iuplua.lua");
  iuplua_dofile("constants.lua");
#endif

  iupluawidgets_open(iuplua_tag);

  sboxlua_open();
  splitlua_open();
  spinlua_open();
  cboxlua_open();
  vallua_open();
  tabslua_open();
  gclua_open();
  getparamlua_open();
  treelua_open();

  return 1;
}

