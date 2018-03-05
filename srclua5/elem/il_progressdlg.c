/******************************************************************************
 * Automatically generated file. Please don't change anything.                *
 *****************************************************************************/

#include <stdlib.h>

#include <lua.h>
#include <lauxlib.h>

#include "iup.h"
#include "iuplua.h"
#include "il.h"


static int progressdlg_cancel_cb(Ihandle *self)
{
  lua_State *L = iuplua_call_start(self, "cancel_cb");
  return iuplua_call(L, 0);
}

static int ProgressDlg(lua_State *L)
{
  Ihandle *ih = IupProgressDlg();
  iuplua_plugstate(L, ih);
  iuplua_pushihandle_raw(L, ih);
  return 1;
}

int iupprogressdlglua_open(lua_State * L)
{
  iuplua_register(L, ProgressDlg, "ProgressDlg");

  iuplua_register_cb(L, "CANCEL_CB", (lua_CFunction)progressdlg_cancel_cb, NULL);

#ifdef IUPLUA_USELOH
#include "progressdlg.loh"
#else
#ifdef IUPLUA_USELH
#include "progressdlg.lh"
#else
  iuplua_dofile(L, "progressdlg.lua");
#endif
#endif

  return 0;
}

