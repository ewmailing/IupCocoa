/** \file
 * \brief Bindig of iupsplit to Lua 3.
 *
 * See Copyright Notice in "iup.h"
 */
 
#include <stdlib.h>

#include "iup.h"

#include <lua.h>

#include "iuplua.h"
#include "il.h"
#include "il_controls.h"

static void CreateSplit(void)
{
  int tag = (int)lua_getnumber(lua_getglobal("iuplua_tag"));
  lua_pushusertag(IupSplit(iuplua_checkihandle(1), iuplua_checkihandle(2)), tag);
}

int splitlua_open(void)
{
  lua_register("iupCreateSplit", CreateSplit);

#ifdef IUPLUA_USELOH
#ifdef TEC_BIGENDIAN
#ifdef TEC_64
#include "loh/split_be64.loh"
#else
#include "loh/split_be32.loh"
#endif  
#else
#ifdef TEC_64
#ifdef WIN64
#include "loh/split_le64w.loh"
#else
#include "loh/split_le64.loh"
#endif  
#else
#include "loh/split.loh"
#endif  
#endif  
#else
  iuplua_dofile("luasplit.lua");
#endif

  return 1;
}
