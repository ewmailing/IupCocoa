/** \file
 * \brief Bindig of iupscrollbox to Lua 3.
 *
 * See Copyright Notice in "iup.h"
 */
 
#include <stdlib.h>

#include "iup.h"

#include <lua.h>

#include "iuplua.h"
#include "il.h"
#include "il_controls.h"

static void CreateScrollBox(void)
{
  int tag = (int)lua_getnumber(lua_getglobal("iuplua_tag"));
  lua_pushusertag(IupScrollBox(iuplua_checkihandle(1)), tag);
}

int scrollboxlua_open(void)
{
  lua_register("iupCreateScrollBox", CreateScrollBox);

#ifdef IUPLUA_USELOH
#ifdef TEC_BIGENDIAN
#ifdef TEC_64
#include "loh/scrollbox_be64.loh"
#else
#include "loh/scrollbox_be32.loh"
#endif  
#else
#ifdef TEC_64
#ifdef WIN64
#include "loh/scrollbox_le64w.loh"
#else
#include "loh/scrollbox_le64.loh"
#endif  
#else
#include "loh/scrollbox.loh"
#endif  
#endif  
#else
  iuplua_dofile("luascrollbox.lua");
#endif

  return 1;
}
