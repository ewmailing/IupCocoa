/******************************************************************************
 * Automatically generated file. Please don't change anything.                *
 *****************************************************************************/

#include <stdlib.h>

#include <lua.h>
#include <lauxlib.h>

#include "iup.h"
#include "iuplua.h"
#include "il.h"


static int colordlg_colorupdate_cb(Ihandle *self)
{
  lua_State *L = iuplua_call_start(self, "colorupdate_cb");
  return iuplua_call(L, 0);
}

static int ColorDlg(lua_State *L)
{
  Ihandle *ih = IupColorDlg();
  iuplua_plugstate(L, ih);
  iuplua_pushihandle_raw(L, ih);
  return 1;
}

int iupcolordlglua_open(lua_State * L)
{
  iuplua_register(L, ColorDlg, "ColorDlg");

  iuplua_register_cb(L, "COLORUPDATE_CB", (lua_CFunction)colordlg_colorupdate_cb, NULL);

#ifdef IUPLUA_USELOH
#include "colordlg.loh"
#else
#ifdef IUPLUA_USELH
#include "colordlg.lh"
#else
  iuplua_dofile(L, "colordlg.lua");
#endif
#endif

  return 0;
}

