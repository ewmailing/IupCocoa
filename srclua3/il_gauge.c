/** \file
 * \brief Bindig of iupgauge to Lua 3.
 *
 * See Copyright Notice in "iup.h"
 */
 
#include <stdlib.h>

#include "iup.h"
#include "iupcontrols.h"

#include <lua.h>

#include "iuplua.h"
#include "il.h"
#include "il_controls.h"


static void CreateGauge(void)
{
  int tag = (int)lua_getnumber(lua_getglobal("iuplua_tag"));
  lua_pushusertag(IupGauge(), tag);
}

int gaugelua_open(void)
{
  lua_register("iupCreateGauge", CreateGauge);

#ifdef IUPLUA_USELOH
#ifdef TEC_BIGENDIAN
#ifdef TEC_64
#include "loh/gauge_be64.loh"
#else
#include "loh/gauge_be32.loh"
#endif  
#else
#ifdef TEC_64
#ifdef WIN64
#include "loh/gauge_le64w.loh"
#else
#include "loh/gauge_le64.loh"
#endif  
#else
#include "loh/gauge.loh"
#endif  
#endif  
#else
  iuplua_dofile("luagauge.lua");
#endif

  return 1;
}
