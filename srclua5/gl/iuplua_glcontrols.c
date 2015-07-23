/** \file
 * \brief Bindig of iupglcontrols to Lua 5.
 *
 * See Copyright Notice in "iup.h"
 */
 
#include <lua.h>
#include <lualib.h>

#include "iup.h"
#include "iupglcontrols.h"

#include "iuplua.h"
#include "iupluaglcontrols.h"
#include "il.h"
#include "il_glcontrols.h"

int iupglcontrolslua_open(lua_State * L)
{
  if (iuplua_opencall_internal(L))
    IupGLControlsOpen();

  iuplua_get_env(L);

  iupglcanvasboxlua_open(L);
  iupglsubcanvaslua_open(L);
  iupgllabellua_open(L);
  iupglbuttonlua_open(L);
  iupglexpanderlua_open(L);
  iupglframelua_open(L);
  iupgllinklua_open(L);
  iupglprogressbarlua_open(L);
  iupglseparatorlua_open(L);
  iupgltogglelua_open(L);
  iupglvallua_open(L); 
  iupglscrollboxlua_open(L); 

  return 0;
}

/* obligatory to use require"iupluaglcontrols" */
int luaopen_iupluaglcontrols(lua_State* L)
{
  return iupglcontrolslua_open(L);
}

