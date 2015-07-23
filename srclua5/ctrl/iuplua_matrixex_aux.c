/** \file
 * \brief matrixex binding for Lua 5.
 *
 * See Copyright Notice in "iup.h"
 */

#include <lua.h>
#include <lauxlib.h>

#include "iup.h"
#include "iupmatrixex.h"

#include "iuplua.h"
#include "iupluamatrixex.h"
#include "il.h"

#include "iup_attrib.h"
#include "iup_object.h"


static int MatrixExInit(lua_State *L)
{
  Ihandle *ih = iuplua_checkihandle(L,1);
  IupMatrixExInit(ih);
  return 0;
}

void iuplua_matrixexfuncs_open (lua_State *L)
{
  iuplua_register(L, MatrixExInit, "MatrixExInit");
}

int iup_matrixexlua_open(lua_State * L);

int iupmatrixexlua_open(lua_State* L)
{
  if (iuplua_opencall_internal(L))
    IupMatrixExOpen();
    
  iuplua_get_env(L);
  iup_matrixexlua_open(L);
  return 0;
}

/* obligatory to use require"iupluamatrixex" */
int luaopen_iupluamatrixex(lua_State* L)
{
  return iupmatrixexlua_open(L);
}

