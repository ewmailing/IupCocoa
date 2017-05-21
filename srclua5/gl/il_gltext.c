/******************************************************************************
 * Automatically generated file. Please don't change anything.                *
 *****************************************************************************/

#include <stdlib.h>

#include <lua.h>
#include <lauxlib.h>

#include "iup.h"
#include "iuplua.h"
#include "iupglcontrols.h"
#include "il.h"


static int gltext_valuechanged_cb(Ihandle *self)
{
  lua_State *L = iuplua_call_start(self, "valuechanged_cb");
  return iuplua_call(L, 0);
}

static int GLText(lua_State *L)
{
  Ihandle *ih = IupGLText();
  iuplua_plugstate(L, ih);
  iuplua_pushihandle_raw(L, ih);
  return 1;
}

int iupgltextlua_open(lua_State * L)
{
  iuplua_register(L, GLText, "GLText");

  iuplua_register_cb(L, "VALUECHANGED_CB", (lua_CFunction)gltext_valuechanged_cb, NULL);

#ifdef IUPLUA_USELOH
#include "gltext.loh"
#else
#ifdef IUPLUA_USELH
#include "gltext.lh"
#else
  iuplua_dofile(L, "gltext.lua");
#endif
#endif

  return 0;
}

