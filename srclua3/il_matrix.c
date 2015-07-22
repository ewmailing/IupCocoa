/** \file
 * \brief Bindig of iupmatrix to Lua 3.
 *
 * See Copyright Notice in "iup.h"
 */
 
#include <stdlib.h>

#include "iup.h"
#include "iupcontrols.h"

#include <cd.h>

#include <lua.h>

#include <cdlua.h>

#include "iuplua.h"
#include "il.h"
#include "il_controls.h"


static int MATRIX_draw(Ihandle *handle, int lin, int col, 
                                        int x1, int x2, int y1, int y2, cdCanvas* cnv)
{
  iuplua_call_start(handle, "draw");
  lua_pushnumber(lin);
  lua_pushnumber(col);
  lua_pushnumber(x1);
  lua_pushnumber(x2);
  lua_pushnumber(y1);
  lua_pushnumber(y2);
  cdlua_pushcanvas(cnv);
  return iuplua_call();
}

static int MATRIX_action (Ihandle *handle, int c, int lin, int col, int active, char *after)
{
  iuplua_call_start(handle, "action");
  lua_pushnumber(c);
  lua_pushnumber(lin);
  lua_pushnumber(col);
  lua_pushnumber(active);
  lua_pushstring(after);
  return iuplua_call();
}

static int MATRIX_edition (Ihandle *handle, int lin, int col, int modo, int update)
{
  iuplua_call_start(handle, "edition");
  lua_pushnumber(lin);
  lua_pushnumber(col);
  lua_pushnumber(modo);
  lua_pushnumber(update);
  return iuplua_call();
}

static int MATRIX_dropcheck (Ihandle *handle, int lin, int col)
{
  iuplua_call_start(handle, "dropcheck");
  lua_pushnumber(lin);
  lua_pushnumber(col);
  return iuplua_call();
}

static int MATRIX_markedit_cb (Ihandle *handle, int lin, int col, int marked)
{
  iuplua_call_start(handle, "markedit_cb");
  lua_pushnumber(lin);
  lua_pushnumber(col);
  lua_pushnumber(marked);
  return iuplua_call();
}

static int MATRIX_mark_cb (Ihandle *handle, int lin, int col)
{
  iuplua_call_start(handle, "mark_cb");
  lua_pushnumber(lin);
  lua_pushnumber(col);
  return iuplua_call();
}

static int MATRIX_drop (Ihandle *handle, Ihandle *drop, int lin, int col)
{
  iuplua_regihandle(drop);
  iuplua_call_start(handle, "drop");
  iuplua_pushihandle_cb(drop);
  lua_pushnumber(lin);
  lua_pushnumber(col);
  return iuplua_call();
}

static int MATRIX_dropselect (Ihandle *handle, int lin, int col,
                              Ihandle *list, char *t, int i, int v)
{
  iuplua_regihandle(list);
  iuplua_call_start(handle, "dropselect");
  lua_pushnumber(lin);
  lua_pushnumber(col);
  iuplua_pushihandle_cb(list);
  lua_pushstring(t);
  lua_pushnumber(i);
  lua_pushnumber(v);
  return iuplua_call();
}

static int MATRIX_enteritem (Ihandle *handle, int lin, int col)
{
  iuplua_call_start(handle, "enteritem");
  lua_pushnumber(lin);
  lua_pushnumber(col);
  return iuplua_call();
}

static int MATRIX_leaveitem (Ihandle *handle, int lin, int col)
{
  iuplua_call_start(handle, "leaveitem");
  lua_pushnumber(lin);
  lua_pushnumber(col);
  return iuplua_call();
}

static int MATRIX_mousemove (Ihandle *handle, int lin, int col)
{
  iuplua_call_start(handle, "mousemove");
  lua_pushnumber(lin);
  lua_pushnumber(col);
  return iuplua_call();
}

static int MATRIX_click (Ihandle *handle, int lin, int col, char *r)
{
  iuplua_call_start(handle, "click");
  lua_pushnumber(lin);
  lua_pushnumber(col);
  lua_pushstring(r);
  return iuplua_call();
}

static int MATRIX_release_cb (Ihandle *handle, int lin, int col, char *r)
{
  iuplua_call_start(handle, "release_cb");
  lua_pushnumber(lin);
  lua_pushnumber(col);
  lua_pushstring(r);
  return iuplua_call();
}

static int MATRIX_scrolltop (Ihandle *handle, int lin, int col)
{
  iuplua_call_start(handle, "scrolltop");
  lua_pushnumber(lin);
  lua_pushnumber(col);
  return iuplua_call();
}

static int MATRIX_color(Ihandle *handle, char* name, int lin, int col, unsigned int *red, unsigned int *green, unsigned int *blue)
{
  lua_Object obj;
  iuplua_call_start(handle, name);
  lua_pushnumber(lin);
  lua_pushnumber(col);
  if (lua_call ("iupCallMethod"))
  {
    lua_endblock ();
    return IUP_IGNORE;
  }
  obj = lua_getresult (1);
  if (obj == LUA_NOOBJECT)
  {
    lua_endblock ();
    return IUP_IGNORE;
  }
  else if (lua_isnumber (obj))
  {
    int ret;
    *red = (unsigned int)lua_getnumber (obj);

    obj = lua_getresult (2);
    if (obj == LUA_NOOBJECT || !lua_isnumber (obj))
    {
      lua_endblock ();
      return IUP_IGNORE;
    }
    *green = (unsigned int)lua_getnumber (obj);

    obj = lua_getresult (3);
    if (obj == LUA_NOOBJECT || !lua_isnumber (obj))
    {
      lua_endblock ();
      return IUP_IGNORE;
    }
    *blue = (unsigned int)lua_getnumber (obj);

    obj = lua_getresult (4);
    if (obj == LUA_NOOBJECT || !lua_isnumber (obj))
    {
      lua_endblock ();
      return IUP_DEFAULT;
    }
    ret = (int)lua_getnumber (obj);
    lua_endblock ();
    return ret;
  }

  lua_endblock ();
  return IUP_IGNORE;
}

static int MATRIX_fgcolor(Ihandle *handle, int lin, int col, unsigned int *red, unsigned int *green, unsigned int *blue)
{
  return MATRIX_color(handle, "fgcolorcb", lin, col, red, green, blue);
}

static int MATRIX_bgcolor(Ihandle *handle, int lin, int col, unsigned int *red, unsigned int *green, unsigned int *blue)
{
  return MATRIX_color(handle, "bgcolorcb", lin, col, red, green, blue);
}

static char* MATRIX_value (Ihandle *handle, int lin, int col)
{
  iuplua_call_start(handle, "valuecb");
  lua_pushnumber(lin);
  lua_pushnumber(col);
  return iuplua_call_rs();
}

static int MATRIX_value_edit (Ihandle *handle, int lin, int col, char* val)
{
  iuplua_call_start(handle, "value_edit");
  lua_pushnumber(lin);
  lua_pushnumber(col);
  lua_pushstring(val);
  return iuplua_call();
}

static char* MATRIX_font_cb (Ihandle *handle, int lin, int col)
{
  iuplua_call_start(handle, "font_cb");
  lua_pushnumber(lin);
  lua_pushnumber(col);
  return iuplua_call_rs();
}

static void CreateMatrix(void)
{
  int tag = (int)lua_getnumber(lua_getglobal("iuplua_tag"));
  lua_pushusertag(IupMatrix(NULL), tag);
}

int matrixlua_open(void)
{
  struct AssocList {
    char *name;
    lua_CFunction func;
  } MatAssocList [] = {
    {"iup_mat_action_cb", (lua_CFunction)MATRIX_action},
    {"iup_mat_edition_cb", (lua_CFunction)MATRIX_edition},
    {"iup_mat_enteritem_cb", (lua_CFunction)MATRIX_enteritem},
    {"iup_mat_drop_cb", (lua_CFunction)MATRIX_drop},
    {"iup_mat_dropselect_cb", (lua_CFunction)MATRIX_dropselect},
    {"iup_mat_leaveitem_cb", (lua_CFunction)MATRIX_leaveitem},
    {"iup_mat_click_cb", (lua_CFunction)MATRIX_click},
    {"iup_mat_release_cb", (lua_CFunction)MATRIX_release_cb},
    {"iup_mat_mousemove_cb", (lua_CFunction)MATRIX_mousemove},
    {"iup_mat_scrolltop_cb", (lua_CFunction)MATRIX_scrolltop},
    {"iup_mat_fgcolor_cb", (lua_CFunction)MATRIX_fgcolor},
    {"iup_mat_bgcolor_cb", (lua_CFunction)MATRIX_bgcolor},
    {"iup_mat_draw_cb", (lua_CFunction)MATRIX_draw},
    {"iup_mat_dropcheck_cb", (lua_CFunction)MATRIX_dropcheck},
    {"iup_mat_font_cb", (lua_CFunction)MATRIX_font_cb},
    {"iup_mat_value_cb", (lua_CFunction)MATRIX_value},
    {"iup_mat_value_edit_cb", (lua_CFunction)MATRIX_value_edit},
    {"iup_mat_mark_cb", (lua_CFunction)MATRIX_mark_cb},
    {"iup_mat_markedit_cb", (lua_CFunction)MATRIX_markedit_cb}
  };
  int SizeMatAssocList = (sizeof(MatAssocList)/sizeof(struct AssocList));
  int i;

  lua_register("iupCreateMatrix", CreateMatrix);

  for (i = 0; i < SizeMatAssocList; i++)
    lua_register(MatAssocList[i].name, MatAssocList[i].func);

#ifdef IUPLUA_USELOH
#ifdef TEC_BIGENDIAN
#ifdef TEC_64
#include "loh/matrix_be64.loh"
#else
#include "loh/matrix_be32.loh"
#endif  
#else
#ifdef TEC_64
#ifdef WIN64
#include "loh/matrix_le64w.loh"
#else
#include "loh/matrix_le64.loh"
#endif  
#else
#include "loh/matrix.loh"
#endif  
#endif  
#else
  iuplua_dofile("luamatrix.lua");
#endif

  return 1;
}
