/** \file
 * \brief Bindig of iupcb to Lua 3.
 *
 * See Copyright Notice in "iup.h"
 */
 
#include <stdlib.h>

#include "iup.h"
#include "iupcontrols.h"

#include <lua.h>

#include "iuplua.h"
#include "il.h"
#include "il_controls.h"


static char* COLORBAR_cell_cb(Ihandle *handle, int cell)
{
  iuplua_call_start(handle, "cellcb");
  lua_pushnumber(cell);
  return iuplua_call_rs();
}

static int COLORBAR_select_cb(Ihandle *handle, int cell, int type)
{
  iuplua_call_start(handle, "selectcb");
  lua_pushnumber(cell);
  lua_pushnumber(type);
  return iuplua_call();
}

static int COLORBAR_switch_cb(Ihandle *handle, int prim_cell, int sec_cell)
{
  iuplua_call_start(handle, "switchcb");
  lua_pushnumber(prim_cell);
  lua_pushnumber(sec_cell);
  return iuplua_call();
}

static int COLORBAR_extended_cb(Ihandle *handle, int cell)
{
  iuplua_call_start(handle, "extendedcb");
  lua_pushnumber(cell);
  return iuplua_call();
}

static void CreateColorbar(void) 
{
  int tag = (int)lua_getnumber(lua_getglobal("iuplua_tag"));
  lua_pushusertag(IupColorbar(), tag);
}

int colorbarlua_open(void) 
{
  struct AssocList {
    char *name;
    lua_CFunction func;
  } COLORBARAssocList [] = {
    {"iup_colorbar_cell_cb",   (lua_CFunction)COLORBAR_cell_cb},
    {"iup_colorbar_select_cb", (lua_CFunction)COLORBAR_select_cb},
    {"iup_colorbar_switch_cb", (lua_CFunction)COLORBAR_switch_cb},
    {"iup_colorbar_extended_cb", (lua_CFunction)COLORBAR_extended_cb}
  };
  int SizeCOLORBARAssocList = (sizeof(COLORBARAssocList)/sizeof(struct AssocList));
  int i;

  lua_register("iupCreateColorbar", CreateColorbar);

  for (i = 0; i < SizeCOLORBARAssocList; i++)
    lua_register(COLORBARAssocList[i].name, COLORBARAssocList[i].func);

#ifdef IUPLUA_USELOH
#ifdef TEC_BIGENDIAN
#ifdef TEC_64
#include "loh/colorbar_be64.loh"
#else
#include "loh/colorbar_be32.loh"
#endif  
#else
#ifdef TEC_64
#ifdef WIN64
#include "loh/colorbar_le64w.loh"
#else
#include "loh/colorbar_le64.loh"
#endif  
#else
#include "loh/colorbar.loh"
#endif  
#endif  
#else
  iuplua_dofile("luacolorbar.lua");
#endif

  return 1;
}
