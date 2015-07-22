/** \file
 * \brief Bindig of iupcbox to Lua 3.
 *
 * See Copyright Notice in "iup.h"
 */
#include <stdlib.h>
 
#include <lua.h>

#include "iup.h"

#include "iuplua.h"
#include "il.h"
#include "il_controls.h"

static void CreateCbox(void)
{
  int tag = (int)lua_getnumber(lua_getglobal("iuplua_tag"));
  Ihandle** params = iuplua_checkihandle_array(1);
  lua_pushusertag(IupCboxv(params),tag);
  free(params);
}

int cboxlua_open (void)
{
  lua_register("iupCreateCbox",CreateCbox);

  return lua_dostring("IUPCBOX = {parent = IUPCANVAS}\n"
                      "function IUPCBOX:CreateIUPelement (obj)\n"
                      "  return iupCreateCbox (obj, getn(obj))\n"
                      "end\n"
                      "function iupcbox (o)\n"
                      "  return IUPCBOX:Constructor (o)\n"
                      "end\n"
                      "iup.cbox = iupcbox\n");
}
