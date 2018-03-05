/** \file
 * \brief Bindig of iupcontrols to Lua 5.
 *
 * See Copyright Notice in "iup.h"
 */
 
#include <lua.h>
#include <lauxlib.h>

#include "iup.h"
#include "iupcontrols.h"

#include "iuplua.h"
#include "iupluacontrols.h"
#include "il.h"
#include "il_controls.h"

int iupcontrolslua_open(lua_State * L)
{
  if (iuplua_opencall_internal(L))
    IupControlsOpen();

  iuplua_get_env(L);

  iupmatrixlua_open(L);
  iupcellslua_open(L);
  iupmatrixlistlua_open(L);
  iupmatrixexlua_open(L);

  return 0;
}

/* obligatory to use require"iupluacontrols" */
int luaopen_iupluacontrols(lua_State* L)
{
  return iupcontrolslua_open(L);
}

