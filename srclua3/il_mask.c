/** \file
 * \brief Bindig of iupmask to Lua 3.
 *
 * See Copyright Notice in "iup.h"
 */
 
#include <stdlib.h>

#include "iup.h"
#include "iupmask.h"

#include <lua.h>
#include <lauxlib.h>

#include "iuplua.h"
#include "il.h"
#include "il_controls.h"


static void cfMaskSet (void)
{
  iupmaskSet(iuplua_checkihandle(1), 
             luaL_check_string(2), 
             luaL_check_int(3), 
             luaL_check_int(4));
}

static void cfMaskRemove (void)
{
  iupmaskRemove(iuplua_checkihandle(1));
}

static void cfMaskMatSet(void)
{
  iupmaskMatSet(iuplua_checkihandle(1),
                luaL_check_string(2),
                luaL_check_int(3),
                luaL_check_int(4),
                luaL_check_int(5),
                luaL_check_int(6));

}

static void cfMaskMatRemove(void)
{
  iupmaskMatRemove(iuplua_checkihandle(1),
                   luaL_check_int(2),
                   luaL_check_int(3));

}

static void cfMaskSetInt(void)
{
  iupmaskSetInt(iuplua_checkihandle(1),
                luaL_check_int(2),
                luaL_check_int(3),
                luaL_check_int(4));

}

static void cfMaskSetFloat(void) 
{
  iupmaskSetFloat(iuplua_checkihandle(1),
                  luaL_check_int(2),
                  (float)luaL_check_number(3),
                  (float)luaL_check_number(4));

      
}

static void cfMaskMatSetInt (void)
{
  iupmaskMatSetInt(iuplua_checkihandle(1),
                   luaL_check_int(2),
                   luaL_check_int(3),
                   luaL_check_int(4),
                   luaL_check_int(5),
                   luaL_check_int(6));

}

static void cfMaskMatSetFloat (void)
{
  iupmaskMatSetFloat(iuplua_checkihandle(1),
                     luaL_check_int(2),
                     (float)luaL_check_number(3),
                     (float)luaL_check_number(4),
                     luaL_check_int(5),
                     luaL_check_int(6));
}

static void cfMaskCheck (void)
{
  iupmaskCheck(iuplua_checkihandle(1));
}

static void cfMaskMatCheck (void)
{
  iupmaskMatCheck(iuplua_checkihandle(1),
                  luaL_check_int(2),
                  luaL_check_int(3));
}

static int match_cb (Ihandle *handle)
{
  iuplua_call_start(handle, "match_cb");
  return iuplua_call();
}


int masklua_open(void)
{
  iuplua_regstring_mask(IUP_MASK_FLOAT, "IUPMASK_FLOAT");
  iuplua_regstring_mask(IUP_MASK_UFLOAT, "IUPMASK_UFLOAT");
  iuplua_regstring_mask(IUP_MASK_EFLOAT, "IUPMASK_EFLOAT");
  iuplua_regstring_mask(IUP_MASK_INT, "IUPMASK_INT");
  iuplua_regstring_mask(IUP_MASK_UINT, "IUPMASK_UINT");
  iuplua_regstring(IUP_MASK_FLOAT, "IUP_MASK_FLOAT");
  iuplua_regstring(IUP_MASK_UFLOAT, "IUP_MASK_UFLOAT");
  iuplua_regstring(IUP_MASK_EFLOAT, "IUP_MASK_EFLOAT");
  iuplua_regstring(IUP_MASK_INT, "IUP_MASK_INT");
  iuplua_regstring(IUP_MASK_UINT, "IUP_MASK_UINT");

  iuplua_register("iupmaskRemove", cfMaskRemove);
  iuplua_register("iupmaskMatRemove", cfMaskMatRemove);
  iuplua_register("iupmaskSet", cfMaskSet);
  iuplua_register("iupmaskMatSet", cfMaskMatSet);
  iuplua_register("iupmaskSetInt", cfMaskSetInt);
  iuplua_register("iupmaskSetFloat", cfMaskSetFloat);
  iuplua_register("iupmaskMatSetInt", cfMaskMatSetInt);
  iuplua_register("iupmaskMatSetFloat", cfMaskMatSetFloat);
  iuplua_register("iupmaskCheck", cfMaskCheck);
  iuplua_register("iupmaskMatCheck", cfMaskMatCheck);

  lua_register("iup_match_cb", (lua_CFunction)match_cb);

  return lua_dostring("iup_callbacks.match_cb = {\"MATCH_CB\", iup_match_cb}");
}
