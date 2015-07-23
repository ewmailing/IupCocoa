/** \file
 * \brief Bindig of iupsbox to Lua 3.
 *
 * See Copyright Notice in "iup.h"
 */
 
#include <stdlib.h>

#include "iup.h"

#include <lua.h>

#include "iuplua.h"
#include "il.h"
#include "il_controls.h"

static void CreateSbox(void)
{
  int tag = (int)lua_getnumber(lua_getglobal("iuplua_tag"));
  lua_pushusertag(IupSbox(iuplua_checkihandle(1)), tag);
}

int sboxlua_open(void)
{
  lua_register("iupCreateSbox", CreateSbox);

#ifdef IUPLUA_USELOH
#ifdef TEC_BIGENDIAN
#ifdef TEC_64
#include "loh/sbox_be64.loh"
#else
#include "loh/sbox_be32.loh"
#endif  
#else
#ifdef TEC_64
#ifdef WIN64
#include "loh/sbox_le64w.loh"
#else
#include "loh/sbox_le64.loh"
#endif  
#else
#include "loh/sbox.loh"
#endif  
#endif  
#else
  iuplua_dofile("luasbox.lua");
#endif

  return 1;
}
