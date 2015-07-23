/** \file
 * \brief Bindig of IupCells to Lua 3.
 * @author André Luiz Clinio.
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


static int luacells_mouseclick_cb( Ihandle* handle, int b, int m, 
                                   int i, int j, int x, int y, char* r )
{
  iuplua_call_start(handle, "mouseclick");
  lua_pushnumber(b);
  lua_pushnumber(m);
  lua_pushnumber(i);
  lua_pushnumber(j);
  lua_pushnumber(x);
  lua_pushnumber(y);
  lua_pushstring(r);
  return iuplua_call();
}

static int luacells_mousemotion_cb( Ihandle* handle, int i, int j, 
                                    int x, int y, char* r )
{
  iuplua_call_start(handle, "mousemotion");
  lua_pushnumber(i);
  lua_pushnumber(j);
  lua_pushnumber(x);
  lua_pushnumber(y);
  lua_pushstring(r);
  return iuplua_call();
}

static int luacells_width_cb( Ihandle* handle, int col )
{
  iuplua_call_start(handle, "width");
  lua_pushnumber(col);
  return iuplua_call();
}

static int luacells_height_cb( Ihandle* handle, int line )
{
  iuplua_call_start(handle, "height");
  lua_pushnumber(line);
  return iuplua_call();
}

static int luacells_nlines_cb( Ihandle* handle )
{
  iuplua_call_start(handle, "nlines");
  return iuplua_call();
}

static int luacells_ncols_cb( Ihandle* handle )
{
  iuplua_call_start(handle, "ncols");
  return iuplua_call();
}

static int luacells_hspan_cb( Ihandle* handle, int i, int j )
{
  iuplua_call_start(handle, "hspan");
  lua_pushnumber(i);
  lua_pushnumber(j);
  return iuplua_call();
}

static int luacells_vspan_cb( Ihandle* handle, int i, int j )
{
  iuplua_call_start(handle, "vspan");
  lua_pushnumber(i);
  lua_pushnumber(j);
  return iuplua_call();
}

static int luacells_scrolling_cb( Ihandle* handle, int fline, int fcol )
{
  iuplua_call_start(handle, "scrolling");
  lua_pushnumber(fline);
  lua_pushnumber(fcol);
  return iuplua_call();
}

static void createCells(void) 
{
  int tag = (int)lua_getnumber(lua_getglobal("iuplua_tag"));
  lua_pushusertag(IupCells(), tag);
}

int cellslua_open(void) 
{
  int i;
  struct TypeAssocList {
    char *name; 
    lua_CFunction func; 
  } assoc_list [] = {
    {"iup_cells_mouseclick_cb", (lua_CFunction) luacells_mouseclick_cb },
    {"iup_cells_mousemotion_cb", (lua_CFunction) luacells_mousemotion_cb },
    {"iup_cells_width_cb", (lua_CFunction) luacells_width_cb },
    {"iup_cells_height_cb", (lua_CFunction) luacells_height_cb },
    {"iup_cells_nlines_cb", (lua_CFunction) luacells_nlines_cb },
    {"iup_cells_ncols_cb", (lua_CFunction) luacells_ncols_cb },
    {"iup_cells_hspan_cb", (lua_CFunction) luacells_hspan_cb },
    {"iup_cells_vspan_cb", (lua_CFunction) luacells_vspan_cb },
    {"iup_cells_scrolling_cb", (lua_CFunction) luacells_scrolling_cb }
  };
  int assoc_list_size = (sizeof(assoc_list)/sizeof(struct TypeAssocList));

  lua_register("iupCreateCells", createCells);

  for( i = 0; i < assoc_list_size; i++ )
    lua_register(assoc_list[i].name, assoc_list[i].func);

#ifdef IUPLUA_USELOH
#ifdef TEC_BIGENDIAN
#ifdef TEC_64
#include "loh/cells_be64.loh"
#else
#include "loh/cells_be32.loh"
#endif  
#else
#ifdef TEC_64
#ifdef WIN64
#include "loh/cells_le64w.loh"
#else
#include "loh/cells_le64.loh"
#endif  
#else
#include "loh/cells.loh"
#endif  
#endif  
#else
  iuplua_dofile("luacells.lua");
#endif

  return 1;
}
