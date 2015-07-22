/** \file
 * \brief IULUA core - Bindig of iup to Lua 3.
 *
 * See Copyright Notice in "iup.h"
 */
 
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <lua.h>
#include <lauxlib.h>

#include "iup.h"

#include "iuplua.h"
#include "il.h"


static int iuplua_tag = 0;

static int valuechanged_cb(Ihandle* handle)
{
  iuplua_call_start(handle, "valuechanged_cb");
  return iuplua_call();
}

static int file_cb(Ihandle* handle, char* file_name, char* status)
{
  iuplua_call_start(handle, "file_cb");
  lua_pushstring(file_name);
  lua_pushstring(status);
  return iuplua_call();
}

static int text_action(Ihandle* handle, int carac, char *newtext)
{
  iuplua_call_start(handle, "action");
  lua_pushnumber(carac);
  lua_pushstring(newtext);
  return iuplua_call();
}

static int timer_action(Ihandle* handle)
{
  iuplua_call_start(handle, "actioncb");
  return iuplua_call();
}

static int button_action(Ihandle* handle)
{
  iuplua_call_start(handle, "action");
  return iuplua_call();
}

static int toggle_action(Ihandle* handle, int v)
{
  iuplua_call_start(handle, "action");
  lua_pushnumber(v);
  return iuplua_call();
}

static int iupluaCanvasFocusCb(Ihandle* handle, int focus)
{
  iuplua_call_start(handle, "focus");
  lua_pushnumber(focus);
  return iuplua_call();
}

static int iupluaGetfocusCb(Ihandle* handle)
{
  iuplua_call_start(handle, "getfocus");
  return iuplua_call();
}

static int iupluaKillfocusCb(Ihandle* handle)
{
  iuplua_call_start(handle, "killfocus");
  return iuplua_call();
}

static int item_highlight(Ihandle* handle)
{
  iuplua_call_start(handle, "highlight");
  return iuplua_call();
}

static int menu_open(Ihandle* handle)
{
  iuplua_call_start(handle, "open");
  return iuplua_call();
}

static int menu_close(Ihandle* handle)
{
  iuplua_call_start(handle, "menu_close");
  return iuplua_call();
}

static int iupluaKanyCb(Ihandle* handle, int carac)
{
  iuplua_call_start(handle, "k_any");
  lua_pushnumber(carac);
  return iuplua_call();
}

static int iupluaHelpCb(Ihandle* handle)
{
  iuplua_call_start(handle, "help");
  return iuplua_call();
}

static int iupluaCanvasKeypressCb(Ihandle *handle, int carac, int press)
{
  iuplua_call_start(handle, "keypress");
  lua_pushnumber(carac);
  lua_pushnumber(press);
  return iuplua_call();
}

static int iupluaCanvasScrollCb(Ihandle *handle, int op, float posx, float posy)
{
  iuplua_call_start(handle, "scroll");
  lua_pushnumber(op);
  lua_pushnumber(posx);
  lua_pushnumber(posy);
  return iuplua_call();
}

static int text_caret(Ihandle *handle, int row, int col, int pos)
{
  iuplua_call_start(handle, "caretcb");
  lua_pushnumber(row);
  lua_pushnumber(col);
  lua_pushnumber(pos);
  return iuplua_call();
}
                  
static int list_dblclick_cb(Ihandle *handle, int p0, char * p1)
{
  iuplua_call_start(handle, "dblclick_cb");
  lua_pushnumber(p0);
  lua_pushstring(p1);
  return iuplua_call();
}

static int list_dropdown_cb(Ihandle *handle, int p0)
{
  iuplua_call_start(handle, "dropdown_cb");
  lua_pushnumber(p0);
  return iuplua_call();
}

static int dialog_close(Ihandle* handle)
{
  iuplua_call_start(handle, "close");
  return iuplua_call();
}

static int dialog_show(Ihandle* handle, int v)
{
  iuplua_call_start(handle, "showcb");
  lua_pushnumber(v);
  return iuplua_call();
}

static int iupluaUnMapCb(Ihandle* handle)
{
  iuplua_call_start(handle, "unmapcb");
  return iuplua_call();
}

static int iupluaDestroyCb(Ihandle* handle)
{
  iuplua_call_start(handle, "destroycb");
  return iuplua_call();
}

static int iupluaMapCb(Ihandle* handle)
{
  iuplua_call_start(handle, "mapcb");
  return iuplua_call();
}

static int iupluaDropfilesCb(Ihandle *handle, char* filename, int numFile, int posx, int posy) 
{
  iuplua_call_start(handle, "dropfiles");
  lua_pushstring(filename);
  lua_pushnumber(numFile);
  lua_pushnumber(posx);
  lua_pushnumber(posy);
  return iuplua_call();
}

static int dialog_trayclick(Ihandle *handle, int p1, int p2, int p3)
{
  iuplua_call_start(handle, "trayclick");
  lua_pushnumber(p1);
  lua_pushnumber(p2);
  lua_pushnumber(p3);
  return iuplua_call();
}
                                      
static int dialog_copydata(Ihandle *handle, char* p1, int p2)
{
  iuplua_call_start(handle, "copydata");
  lua_pushstring(p1);
  lua_pushnumber(p2);
  return iuplua_call();
}

static int iupluaCanvasAction(Ihandle* handle, float x, float y)
{
  iuplua_call_start(handle, "action");
  lua_pushnumber(x);
  lua_pushnumber(y);
  return iuplua_call();
}

static int iupluaCanvasButtonCb(Ihandle* handle, int b, int e, int x, int y, char *r)
{
  iuplua_call_start(handle, "button");
  lua_pushnumber(b);
  lua_pushnumber(e);
  lua_pushnumber(x);
  lua_pushnumber(y);
  lua_pushstring(r);
  return iuplua_call();
}

static int iupluaCanvasResizeCb(Ihandle* handle, int w, int h)
{
  iuplua_call_start(handle, "resize");
  lua_pushnumber(w);
  lua_pushnumber(h);
  return iuplua_call();
}

static int iupluaCanvasMotionCb(Ihandle* handle, int x, int y, char *r)
{
  iuplua_call_start(handle, "motion");
  lua_pushnumber(x);
  lua_pushnumber(y);
  lua_pushstring(r);
  return iuplua_call();
}

static int iupluaDialogMoveCb(Ihandle* handle, int x, int y)
{
  iuplua_call_start(handle, "move");
  lua_pushnumber(x);
  lua_pushnumber(y);
  return iuplua_call();
}

static int canvas_mdiactivate(Ihandle* handle)
{
  iuplua_call_start(handle, "mdiactivatecb");
  return iuplua_call();
}

static int iupluaCanvasEnterwindowCb(Ihandle* handle)
{
  iuplua_call_start(handle, "enterwindow");
  return iuplua_call();
}

static int iupluaCanvasLeavewindowCb(Ihandle* handle)
{
  iuplua_call_start(handle, "leavewindow");
  return iuplua_call();
}

static int iupluaCanvasWheelCb(Ihandle* handle, float delta, int x, int y, char *r)
{
  iuplua_call_start(handle, "wheel");
  lua_pushnumber(delta);
  lua_pushnumber(x);
  lua_pushnumber(y);
  lua_pushstring(r);
  return iuplua_call();
}

static int iupluaCanvasWomCb(Ihandle* handle, int i)
{
  iuplua_call_start(handle, "wom");
  lua_pushnumber(i);
  return iuplua_call();
}

static int list_edit(Ihandle *handle, int c, char *after)
{
  iuplua_call_start(handle, "edit");
  lua_pushnumber(c);
  lua_pushstring(after);
  return iuplua_call();
}

static int list_action(Ihandle* handle, char *text, int opt, int select)
{
  iuplua_call_start(handle, "action");
  lua_pushstring(text);
  lua_pushnumber(opt);
  lua_pushnumber(select);
  return iuplua_call();
}

static int list_multiselect(Ihandle *handle, char *value)
{
  iuplua_call_start(handle, "multiselect");
  lua_pushstring(value);
  return iuplua_call();
}

/***********************************************************************************
***********************************************************************************/

static void CreateTimer(void)
{
  lua_pushusertag(IupTimer(), iuplua_tag);
}

static void CreateClipboard(void)
{
  lua_pushusertag(IupClipboard(), iuplua_tag);
}

static void CreateDialog(void)
{
  lua_pushusertag(IupDialog(iuplua_checkihandle(1)), iuplua_tag);
}

static void CreateRadio(void)
{
  lua_pushusertag(IupRadio(iuplua_checkihandle(1)), iuplua_tag);
}

static void CreateMenu(void)
{
  lua_pushusertag(IupMenu(NULL), iuplua_tag);
}

static void CreateHbox(void)
{
  lua_pushusertag(IupHbox(NULL), iuplua_tag);
}

static void CreateVbox(void)
{
  lua_pushusertag(IupVbox(NULL), iuplua_tag);
}

static void CreateZbox(void)
{
  lua_pushusertag(IupZbox(NULL), iuplua_tag);
}

static void CreateFill(void)
{
  lua_pushusertag(IupFill(), iuplua_tag);
}

static void CreateNormalizer(void)
{
  lua_pushusertag(IupNormalizer(NULL), iuplua_tag);
}

static void CreateButton(void)
{
  lua_pushusertag(IupButton(luaL_opt_string(1, NULL), NULL), iuplua_tag);
}

static void CreateText(void)
{
  lua_pushusertag(IupText(NULL), iuplua_tag);
}

static void CreateMultiLine(void)
{
  lua_pushusertag(IupMultiLine(NULL), iuplua_tag);
}

static void CreateLabel(void)
{
  lua_pushusertag(IupLabel(luaL_opt_string(1, NULL)), iuplua_tag);
}

static void CreateToggle(void)
{
  lua_pushusertag(IupToggle(luaL_opt_string(1, NULL), NULL), iuplua_tag);
}

static void CreateItem(void)
{
  lua_pushusertag(IupItem(luaL_opt_string(1, NULL), NULL), iuplua_tag);
}

static void CreateSubmenu(void)
{
  lua_pushusertag(IupSubmenu(luaL_opt_string(1, NULL), iuplua_checkihandle(2)), iuplua_tag);
}

static void CreateSeparator(void)
{
  lua_pushusertag(IupSeparator(), iuplua_tag);
}

static void CreateFileDlg(void)
{
  lua_pushusertag(IupFileDlg(), iuplua_tag);
}

static void CreateMessageDlg(void)
{
  lua_pushusertag(IupMessageDlg(), iuplua_tag);
}

static void CreateFontDlg(void)
{
  lua_pushusertag(IupFontDlg(), iuplua_tag);
}

static void CreateColorDlg(void)
{
  lua_pushusertag(IupColorDlg(), iuplua_tag);
}

static void CreateProgressBar(void)
{
  lua_pushusertag(IupProgressBar(), iuplua_tag);
}

static void CreateUser(void)
{
  lua_pushusertag(IupUser(), iuplua_tag);
}

static void CreateFrame(void)
{
  lua_pushusertag(IupFrame(iuplua_checkihandle(1)), iuplua_tag);
}

static void CreateCanvas(void)
{
  lua_pushusertag(IupCanvas(NULL), iuplua_tag);
}

static void CreateList(void)
{
  lua_pushusertag(IupList(NULL), iuplua_tag);
}

static void CreateImage(void)
{
  int i, j;
  lua_Object linha;
  int width, height;
  unsigned char *pixels;
  lua_Object obj = luaL_tablearg(3);
  width = luaL_check_int(1);
  height = luaL_check_int(2);
  pixels = (unsigned char *) malloc(width*height);

  for (i = 1; i <= height; i++) 
  {
    lua_beginblock();
    lua_pushobject(obj);
    lua_pushnumber(i);
    linha = lua_gettable();
    if (!lua_istable(linha)) 
    {
      lua_endblock();
      lua_error("iupCreateImage: incorrect value in argument");
    }

    for (j = 1; j <= width; j++) 
    {
      lua_Object n;
      lua_beginblock();
      lua_pushobject(linha);
      lua_pushnumber(j);
      n = lua_gettable();
      if (!lua_isnumber(n)) 
      {
        lua_endblock();
        lua_error("iupCreateImage: incorrect value in argument");
      }
      pixels[(i - 1) * width + (j - 1)] = (unsigned char) lua_getnumber(n);
      lua_endblock();
    }

    lua_pushobject(obj);
    lua_pushnumber(i);
    lua_pushnil();
    lua_settable();
    lua_endblock();
  }

  lua_pushusertag(IupImage(width, height, pixels), iuplua_tag);
  free(pixels);
}

static void CreateImageRGB(void)
{
  int i, count, width, height;
  unsigned char *pixels;
  lua_Object n, obj = luaL_tablearg(3);
  width = luaL_check_int(1);
  height = luaL_check_int(2);
  count = width * height * 3;
  pixels = (unsigned char *) malloc(count);

  for (i = 0; i < count; i++)
  {
    lua_beginblock();
    lua_pushobject(obj);
    lua_pushnumber(i+1);

    n = lua_gettable();
    if (!lua_isnumber(n)) 
    {
      lua_endblock();
      lua_error("iupCreateImage: incorrect value in argument");
    }
    pixels[i] = (unsigned char)lua_getnumber(n);
    lua_endblock();
  }

  lua_pushusertag(IupImageRGB(width, height, pixels), iuplua_tag);
  free(pixels);
}

static void CreateImageRGBA(void)
{
  int i, count, width, height;
  unsigned char *pixels;
  lua_Object n, obj = luaL_tablearg(3);
  width = luaL_check_int(1);
  height = luaL_check_int(2);
  count = width * height * 4;
  pixels = (unsigned char *) malloc(count);

  for (i = 0; i < count; i++)
  {
    lua_beginblock();
    lua_pushobject(obj);
    lua_pushnumber(i+1);

    n = lua_gettable();
    if (!lua_isnumber(n)) 
    {
      lua_endblock();
      lua_error("iupCreateImage: incorrect value in argument");
    }
    pixels[i] = (unsigned char)lua_getnumber(n);
    lua_endblock();
  }

  lua_pushusertag(IupImageRGBA(width, height, pixels), iuplua_tag);
  free(pixels);
}

/***********************************************************************************
***********************************************************************************/

int iupluawidgets_open(int tag)
{
  struct FuncList {
      char *name;
      lua_CFunction func;
  } FuncList[] = {

    { "iupCreateButton", CreateButton },
    { "iupCreateDialog", CreateDialog },
    { "iupCreateTimer", CreateTimer },
    { "iupCreateClipboard", CreateClipboard },
    { "iupCreateHbox", CreateHbox },
    { "iupCreateVbox", CreateVbox },
    { "iupCreateZbox", CreateZbox },
    { "iupCreateText", CreateText },
    { "iupCreateMultiLine", CreateMultiLine },
    { "iupCreateLabel", CreateLabel },
    { "iupCreateToggle", CreateToggle },
    { "iupCreateItem", CreateItem },
    { "iupCreateSubmenu", CreateSubmenu },
    { "iupCreateSeparator", CreateSeparator },
    { "iupCreateFileDlg", CreateFileDlg },
    { "iupCreateMessageDlg", CreateMessageDlg},
    { "iupCreateColorDlg", CreateColorDlg},
    { "iupCreateFontDlg", CreateFontDlg},
    { "iupCreateUser", CreateUser },
    { "iupCreateFrame", CreateFrame },
    { "iupCreateCanvas", CreateCanvas },
    { "iupCreateList", CreateList },
    { "iupCreateImage", CreateImage },
    { "iupCreateImageRGB", CreateImageRGB },
    { "iupCreateImageRGBA", CreateImageRGBA },
    { "iupCreateFill", CreateFill },
    { "iupCreateRadio", CreateRadio },
    { "iupCreateMenu", CreateMenu },
    { "iupCreateProgressBar", CreateProgressBar },
    { "iupCreateNormalizer", CreateNormalizer },

    { "iup_action_timer", (lua_CFunction)timer_action},
    { "iup_action_text", (lua_CFunction)text_action},
    { "iup_action_toggle", (lua_CFunction)toggle_action},
    { "iup_action_button", (lua_CFunction)button_action},
    { "iup_action_list", (lua_CFunction)list_action},
    { "iup_action_canvas", (lua_CFunction)iupluaCanvasAction},
    { "iup_focus_cb", (lua_CFunction)iupluaCanvasFocusCb},
    { "iup_keypress_cb", (lua_CFunction)iupluaCanvasKeypressCb},
    { "iup_scroll_cb", (lua_CFunction)iupluaCanvasScrollCb},
    { "iup_resize_cb", (lua_CFunction)iupluaCanvasResizeCb},
    { "iup_move_cb", (lua_CFunction)iupluaDialogMoveCb},
    { "iup_motion_cb", (lua_CFunction)iupluaCanvasMotionCb},
    { "iup_mdiactivate_cb", (lua_CFunction)canvas_mdiactivate},
    { "iup_enterwindow_cb", (lua_CFunction)iupluaCanvasEnterwindowCb},
    { "iup_leavewindow_cb", (lua_CFunction)iupluaCanvasLeavewindowCb},
    { "iup_wom_cb", (lua_CFunction)iupluaCanvasWomCb},
    { "iup_wheel_cb", (lua_CFunction)iupluaCanvasWheelCb},
    { "iup_highlight_cb", (lua_CFunction)item_highlight},
    { "iup_button_cb", (lua_CFunction)iupluaCanvasButtonCb},
    { "iup_close_cb", (lua_CFunction)dialog_close},
    { "iup_show_cb", (lua_CFunction)dialog_show},
    { "iup_map_cb", (lua_CFunction)iupluaMapCb},
    { "iup_unmap_cb", (lua_CFunction)iupluaUnMapCb},
    { "iup_destroy_cb", (lua_CFunction)iupluaDestroyCb},
    { "iup_dropfiles_cb", (lua_CFunction)iupluaDropfilesCb},
    { "iup_trayclick_cb", (lua_CFunction)dialog_trayclick},
    { "iup_copydata_cb", (lua_CFunction)dialog_copydata},
    { "iup_getfocus_cb", (lua_CFunction)iupluaGetfocusCb},
    { "iup_killfocus_cb", (lua_CFunction)iupluaKillfocusCb},
    { "iup_k_any", (lua_CFunction)iupluaKanyCb},
    { "iup_help_cb", (lua_CFunction)iupluaHelpCb},
    { "iup_edit_cb", (lua_CFunction)list_edit},
    { "iup_caret_cb", (lua_CFunction)text_caret},
    { "iup_multiselect_cb", (lua_CFunction)list_multiselect},
    { "iup_open_cb", (lua_CFunction)menu_open},
    { "iup_menuclose_cb", (lua_CFunction)menu_close},
    { "iup_file_cb", (lua_CFunction)file_cb},
    { "iup_dropdown_cb", (lua_CFunction)list_dropdown_cb},
    { "iup_dblclick_cb", (lua_CFunction)list_dblclick_cb},
    { "iup_valuechanged_cb", (lua_CFunction)valuechanged_cb}
  };

  int SizeFuncList = (sizeof(FuncList)/sizeof(struct FuncList));
  int ret_val = 1; 
  int i;

  iuplua_tag = tag;

  for (i = 0; i < SizeFuncList; i++)
    lua_register(FuncList[i].name, FuncList[i].func);

#ifdef IUPLUA_USELOH
#ifdef TEC_BIGENDIAN
#ifdef TEC_64
#include "loh/iuplua_widgets_be64.loh"
#else
#include "loh/iuplua_widgets_be32.loh"
#endif  
#else
#ifdef TEC_64
#ifdef WIN64
#include "loh/iuplua_widgets_le64w.loh"
#else
#include "loh/iuplua_widgets_le64.loh"
#endif  
#else
#include "loh/iuplua_widgets.loh"
#endif  
#endif  
#else
  iuplua_dofile("iuplua_widgets.lua");
#endif

  return ret_val;
}
