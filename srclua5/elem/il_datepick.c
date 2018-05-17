/******************************************************************************
 * Automatically generated file. Please don't change anything.                *
 *****************************************************************************/

#include <stdlib.h>

#include <lua.h>
#include <lauxlib.h>

#include "iup.h"
#include "iuplua.h"
#include "il.h"


static int DatePick(lua_State *L)
{
  Ihandle *ih = IupDatePick();
  iuplua_plugstate(L, ih);
  iuplua_pushihandle_raw(L, ih);
  return 1;
}

int iupdatepicklua_open(lua_State * L)
{
  iuplua_register(L, DatePick, "DatePick");


#ifdef IUPLUA_USELOH
#include "datepick.loh"
#else
#ifdef IUPLUA_USELH
#include "datepick.lh"
#else
  iuplua_dofile(L, "datepick.lua");
#endif
#endif

  return 0;
}

