/** \file
 * \brief Bindig of iuptabs to Lua 3.
 *
 * See Copyright Notice in "iup.h"
 */
 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "iup.h"

#include <lua.h>
#include <lauxlib.h>

#include "iuplua.h"
#include "il.h"
#include "il_controls.h"


static int TABS_tabchange (Ihandle *handle, Ihandle *newtab, Ihandle *oldtab )
{
  iuplua_regihandle(newtab);
  iuplua_regihandle(oldtab);
  iuplua_call_start(handle, "tabchange");
  iuplua_pushihandle_cb(newtab);
  iuplua_pushihandle_cb(oldtab);
  return iuplua_call();
}

static void CreateTabs(void)
{
  int tag = (int)lua_getnumber(lua_getglobal("iuplua_tag"));
  Ihandle** params = iuplua_checkihandle_array(1);
  lua_pushusertag(IupTabsv(params), tag);
  free(params);
}

int tabslua_open(void)
{
  lua_register("iupCreateTabs", CreateTabs);
  lua_register("iup_tabchange_cb", (lua_CFunction)TABS_tabchange);
                                               
#ifdef IUPLUA_USELOH
#ifdef TEC_BIGENDIAN
#ifdef TEC_64
#include "loh/tabs_be64.loh"
#else
#include "loh/tabs_be32.loh"
#endif  
#else
#ifdef TEC_64
#ifdef WIN64
#include "loh/tabs_le64w.loh"
#else
#include "loh/tabs_le64.loh"
#endif  
#else
#include "loh/tabs.loh"
#endif  
#endif  
#else
  iuplua_dofile("luatabs.lua");
#endif

  return 1;
}
