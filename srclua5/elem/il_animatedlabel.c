/******************************************************************************
 * Automatically generated file. Please don't change anything.                *
 *****************************************************************************/

#include <stdlib.h>

#include <lua.h>
#include <lauxlib.h>

#include "iup.h"
#include "iuplua.h"
#include "il.h"


static int AnimatedLabel(lua_State *L)
{
  Ihandle *ih = IupAnimatedLabel(iuplua_checkihandleornil(L, 1));
  iuplua_plugstate(L, ih);
  iuplua_pushihandle_raw(L, ih);
  return 1;
}

int iupanimatedlabellua_open(lua_State * L)
{
  iuplua_register(L, AnimatedLabel, "AnimatedLabel");


#ifdef IUPLUA_USELOH
#include "animatedlabel.loh"
#else
#ifdef IUPLUA_USELH
#include "animatedlabel.lh"
#else
  iuplua_dofile(L, "animatedlabel.lua");
#endif
#endif

  return 0;
}

