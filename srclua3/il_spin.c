/** \file
 * \brief Bindig of iupspin to Lua 3.
 *
 * See Copyright Notice in "iup.h"
 */
 
#include <stdlib.h>

#include "iup.h"

#include <lua.h>

#include "iuplua.h"
#include "il.h"
#include "il_controls.h"


static int SPIN_spin_cb (Ihandle *handle, int v)
{
  iuplua_call_start(handle, "spincb");
  lua_pushnumber(v);
  return iuplua_call();
}

static void CreateSpin(void)
{
  int tag = (int)lua_getnumber(lua_getglobal("iuplua_tag"));
  lua_pushusertag(IupSpin(), tag);
}

static void CreateSpinbox(void)
{
  int tag = (int)lua_getnumber(lua_getglobal("iuplua_tag"));
  lua_pushusertag(IupSpinbox(iuplua_checkihandle(1)), tag);
}

int spinlua_open(void)
{
  lua_register("iupCreateSpin", CreateSpin);
  lua_register("iupCreateSpinbox", CreateSpinbox);
  lua_register("iup_spin_cb", (lua_CFunction)SPIN_spin_cb);

#ifdef IUPLUA_USELOH
#ifdef TEC_BIGENDIAN
#ifdef TEC_64
#include "loh/spin_be64.loh"
#else
#include "loh/spin_be32.loh"
#endif  
#else
#ifdef TEC_64
#ifdef WIN64
#include "loh/spin_le64w.loh"
#else
#include "loh/spin_le64.loh"
#endif  
#else
#include "loh/spin.loh"
#endif  
#endif  
#else
  iuplua_dofile("luaspin.lua");
#endif

  return 1;
}
