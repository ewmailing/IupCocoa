/** \file
 * \brief Bindig of iupval to Lua 3.
 *
 * See Copyright Notice in "iup.h"
 */
 
#include <stdlib.h>

#include "iup.h"
#include "iupcontrols.h"

#include <lua.h>
#include <lauxlib.h>

#include "iuplua.h"
#include "il.h"
#include "il_controls.h"


static int VAL_mousemove (Ihandle *handle, double v)
{
  iuplua_call_start(handle, "mousemove");
  lua_pushnumber(v);
  return iuplua_call();
}

static int VAL_button_press (Ihandle *handle, double v)
{
  iuplua_call_start(handle, "buttonpress");
  lua_pushnumber(v);
  return iuplua_call();
}

static int VAL_button_release (Ihandle *handle, double v)
{
  iuplua_call_start(handle, "buttonrelease");
  lua_pushnumber(v);
  return iuplua_call();
}

static void CreateVal(void)
{
  int tag = (int)lua_getnumber(lua_getglobal("iuplua_tag"));
  lua_pushusertag(IupVal(luaL_check_string(1)), tag);
}

int vallua_open(void)
{
  lua_register("iupCreateVal", CreateVal);
  lua_register("iup_val_mousemove_cb", (lua_CFunction)VAL_mousemove);
  lua_register("iup_val_button_press_cb", (lua_CFunction)VAL_button_press);
  lua_register("iup_val_button_release_cb", (lua_CFunction)VAL_button_release);

#ifdef IUPLUA_USELOH
#ifdef TEC_BIGENDIAN
#ifdef TEC_64
#include "loh/val_be64.loh"
#else
#include "loh/val_be32.loh"
#endif  
#else
#ifdef TEC_64
#ifdef WIN64
#include "loh/val_le64w.loh"
#else
#include "loh/val_le64.loh"
#endif  
#else
#include "loh/val.loh"
#endif  
#endif  
#else
  iuplua_dofile("luaval.lua");
#endif

  return 1;
}
