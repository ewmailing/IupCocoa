/******************************************************************************
 * Automatically generated file. Please don't change anything.                *
 *****************************************************************************/

#include <stdlib.h>

#include <lua.h>
#include <lauxlib.h>

#include "iup.h"
#include "iuplua.h"
#include "il.h"


static int Calendar(lua_State *L)
{
  Ihandle *ih = IupCalendar();
  iuplua_plugstate(L, ih);
  iuplua_pushihandle_raw(L, ih);
  return 1;
}

int iupcalendarlua_open(lua_State * L)
{
  iuplua_register(L, Calendar, "Calendar");


#ifdef IUPLUA_USELOH
#include "calendar.loh"
#else
#ifdef IUPLUA_USELH
#include "calendar.lh"
#else
  iuplua_dofile(L, "calendar.lua");
#endif
#endif

  return 0;
}

