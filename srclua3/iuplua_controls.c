/** \file
 * \brief Bindig of iupcontrols to Lua 3.
 *
 * See Copyright Notice in "iup.h"
 */
 
#include <lua.h>

#include "iup.h"
#include "iuplua.h"
#include "iupluacontrols.h"
#include "il.h"
#include "il_controls.h"

int iupcontrolslua_open(void)
{
  gaugelua_open();
  matrixlua_open();
  masklua_open();
  diallua_open();
  cblua_open();
  colorbarlua_open();
  cellslua_open();
  return 1;
}
