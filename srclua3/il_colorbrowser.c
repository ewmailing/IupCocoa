/** \file
 * \brief Bindig of iupcb to Lua 3.
 *
 * See Copyright Notice in "iup.h"
 */
 
#include <stdlib.h>

#include <lua.h>

#include "iup.h"
#include "iupcontrols.h"

#include "iuplua.h"
#include "il.h"
#include "il_controls.h"


static int CB_drag(Ihandle *handle, unsigned char r, unsigned char g, unsigned char b)
{
  iuplua_call_start(handle, "drag");
  lua_pushnumber(r);
  lua_pushnumber(g);
  lua_pushnumber(b);
  return iuplua_call();
}

static int CB_change(Ihandle *handle, unsigned char r, unsigned char g, unsigned char b)
{
  iuplua_call_start(handle, "change");
  lua_pushnumber(r);
  lua_pushnumber(g);
  lua_pushnumber(b);
  return iuplua_call();
}

static void CreateColorBrowser(void) 
{
  int tag = (int)lua_getnumber(lua_getglobal("iuplua_tag"));
  lua_pushusertag(IupColorBrowser(), tag);
}

int cblua_open(void) 
{
  lua_register("iupCreateColorBrowser", CreateColorBrowser);
  lua_register("iup_colorbrowser_drag_cb", (lua_CFunction)CB_drag);
  lua_register("iup_colorbrowser_change_cb", (lua_CFunction)CB_change);

#ifdef IUPLUA_USELOH
#ifdef TEC_BIGENDIAN
#ifdef TEC_64
#include "loh/colorbrowser_be64.loh"
#else
#include "loh/colorbrowser_be32.loh"
#endif  
#else
#ifdef TEC_64
#ifdef WIN64
#include "loh/colorbrowser_le64w.loh"
#else
#include "loh/colorbrowser_le64.loh"
#endif  
#else
#include "loh/colorbrowser.loh"
#endif  
#endif  
#else
  iuplua_dofile("luacb.lua");
#endif

  return 1;
}
