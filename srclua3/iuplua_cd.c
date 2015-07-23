/** \file
 * \brief IUP Canvas Lua 3 Binding
 *
 * See Copyright Notice in cd.h
 */

#include <stdlib.h>
#include <stdio.h>

#include <cd.h>
#include <cdiup.h>

#include <lua.h>
#include <lauxlib.h>

#include "iup.h"
#include "iuplua.h"

#include <cdlua.h>
#include <cdluaiup.h>
#include <cdlua3_private.h>


static void *cdiup_checkdata(int param)
{
  return iuplua_checkihandle(param);
}

static cdContextLUA cdluaiupctx = 
{
  0,
  "CD_IUP",
  cdContextIup,
  cdiup_checkdata,
  NULL,
  0
};

void cdluaiup_open(void)
{
  cdlua_addcontext(&cdluaiupctx);
}

