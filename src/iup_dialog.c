/** \file
 * \brief IupDialog class
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <stdarg.h>
#include <assert.h>

#include "iup.h"
#include "iupcbs.h"

#include "iup_class.h"
#include "iup_object.h"
#include "iup_dlglist.h"
#include "iup_layout.h"
#include "iup_attrib.h"
#include "iup_drv.h"
#include "iup_drvinfo.h"
#include "iup_drvfont.h"
#include "iup_focus.h"
#include "iup_str.h"
#define _IUPDLG_PRIVATE
#include "iup_dialog.h"


static int dlg_popup_level = 1;

InativeHandle* iupDialogGetNativeParent(Ihandle* ih)
{
  Ihandle* parent = IupGetAttributeHandle(ih, "PARENTDIALOG");
  if (parent && parent->handle)
    return parent->handle;
  else
    return (InativeHandle*)iupAttribGet(ih, "NATIVEPARENT");
}

static char* iDialogGetBorderSizeAttrib(Ihandle* ih)
{
  int border, caption, menu;
  iupdrvDialogGetDecoration(ih, &border, &caption, &menu);
  return iupStrReturnInt(border);
}

int iupDialogSetClientSizeAttrib(Ihandle* ih, const char* value)
{
  int width = 0, height = 0;
  int border = 0, caption = 0, menu = 0;
  iupStrToIntInt(value, &width, &height, 'x');
  iupdrvDialogGetDecoration(ih, &border, &caption, &menu);
  if (width) width = width + 2*border;
  if (height) height = height + 2*border + caption + menu;
  IupSetfAttribute(ih, "RASTERSIZE", "%dx%d", width, height);
  return 0;
}

static void iDialogAdjustPos(Ihandle *ih, int *x, int *y)
{
  int cursor_x = 0, cursor_y = 0;
  int screen_width = 0, screen_height = 0;
  int current_x = 0, current_y = 0;
  int parent_x = 0, parent_y = 0;

  /* the dialog is already mapped here */

  if (*x == IUP_CURRENT || *y == IUP_CURRENT)
  {
    /* if first time, there is no current position */
    if (!ih->data->first_show)
    {
      int center = IUP_CENTER;
      InativeHandle* parent = iupDialogGetNativeParent(ih);
      if (parent)
        center = IUP_CENTERPARENT;

      if (*x == IUP_CURRENT) *x = center;
      if (*y == IUP_CURRENT) *y = center;
    }
    else
      iupdrvDialogGetPosition(ih, NULL, &current_x, &current_y);
  }

  if (*x == IUP_CENTER || *y == IUP_CENTER ||
      *x == IUP_RIGHT  || *y == IUP_RIGHT ||
      *x == IUP_CENTERPARENT || *y == IUP_CENTERPARENT)
    iupdrvGetScreenSize(&screen_width, &screen_height);

  if (*x == IUP_CENTERPARENT || *y == IUP_CENTERPARENT)
  {
    InativeHandle* parent = iupDialogGetNativeParent(ih);
    if (parent)
    {
      Ihandle* ih_parent = IupGetAttributeHandle(ih, "PARENTDIALOG");

      iupdrvDialogGetPosition(ih_parent, parent, &parent_x, &parent_y);

      if (*x == IUP_CENTERPARENT && *y == IUP_CENTERPARENT)
        iupdrvDialogGetSize(ih_parent, parent, &screen_width, &screen_height);
      else if (*x == IUP_CENTERPARENT)
        iupdrvDialogGetSize(ih_parent, parent, &screen_width, NULL);
      else if (*y == IUP_CENTERPARENT)
        iupdrvDialogGetSize(ih_parent, parent, NULL, &screen_height);
    }
  }

  if (*x == IUP_MOUSEPOS || *y == IUP_MOUSEPOS)
    iupdrvGetCursorPos(&cursor_x, &cursor_y);

  if (iupAttribGetBoolean(ih, "MDICHILD"))
  {
    Ihandle* client = (Ihandle*)iupAttribGet(ih, "MDICLIENT_HANDLE");
    if (client)
    {
      /* position is relative to mdi client */
      parent_x = 0; 
      parent_y = 0;

      /* screen size is now the size of the mdi client */
      screen_width = client->currentwidth;
      screen_height = client->currentheight;

      iupdrvScreenToClient(client, &current_x, &current_y);
      iupdrvScreenToClient(client, &cursor_x, &cursor_y);
    }
  }

  switch (*x)
  {
  case IUP_CENTERPARENT:
    *x = (screen_width - ih->currentwidth)/2 + parent_x;
    break;
  case IUP_CENTER:
    *x = (screen_width - ih->currentwidth)/2;
    break;
  case IUP_LEFT:
    *x = 0;
    break;
  case IUP_RIGHT:
    *x = screen_width - ih->currentwidth;
    break;
  case IUP_MOUSEPOS:
    *x = cursor_x;
    break;
  case IUP_CURRENT:
    *x = current_x;
    break;
  }

  switch (*y)
  {
  case IUP_CENTERPARENT:
    *y = (screen_height - ih->currentheight)/2 + parent_y;
    break;
  case IUP_CENTER:
    *y = (screen_height - ih->currentheight)/2;
    break;
  case IUP_LEFT:
    *y = 0;
    break;
  case IUP_RIGHT:
    *y = screen_height - ih->currentheight;
    break;
  case IUP_MOUSEPOS:
    *y = cursor_y;
    break;
  case IUP_CURRENT:
    *y = current_y;
    break;
  }

  iupdrvAddScreenOffset(x, y, 1);
}

void iupDialogEnterModal(Ihandle* ih_popup, int popup_level)
{
  Ihandle *ih;

  assert(popup_level == dlg_popup_level);

  /* disable all visible dialogs, and mark popup level */
  for (ih = iupDlgListFirst(); ih; ih = iupDlgListNext())
  {
    if (ih != ih_popup &&
      ih->handle &&
      iupdrvDialogIsVisible(ih) &&
      ih->data->popup_level == 0)
    {
      iupdrvSetActive(ih, 0);
      ih->data->popup_level = dlg_popup_level;
    }
  }

  dlg_popup_level++;
}

static void iDialogSetModal(Ihandle* ih_popup)
{
  iupAttribSet(ih_popup, "MODAL", "YES");

  {
    IFi cb = (IFi)IupGetFunction("GLOBALENTERMODAL_CB");
    int popup_level = dlg_popup_level;  /* save before it is changed */

    iupDialogEnterModal(ih_popup, popup_level);

    if (cb)
      cb(popup_level);
  }
}

void iupDialogLeaveModal(int popup_level)
{
  Ihandle *ih;

  assert(popup_level == dlg_popup_level);

  /* must enable all visible dialogs at the marked popup level */
  for (ih = iupDlgListFirst(); ih; ih = iupDlgListNext())
  {
    if (ih->handle)
    {
      if (ih->data->popup_level == dlg_popup_level - 1)
      {
        iupdrvSetActive(ih, 1);
        ih->data->popup_level = 0;
      }
    }
  }

  dlg_popup_level--;
}

static void iDialogUnSetModal(Ihandle* ih_popup)
{
  if (!iupAttribGetBoolean(ih_popup, "MODAL"))
    return;

  iupAttribSet(ih_popup, "MODAL", NULL);

  {
    IFi cb = (IFi)IupGetFunction("GLOBALLEAVEMODAL_CB");
    int popup_level = dlg_popup_level;  /* save before it is changed */

    iupDialogLeaveModal(popup_level);

    if (cb)
      cb(popup_level);
  }
}

static int iDialogCreateMethod(Ihandle* ih, void** params)
{
 ih->data = iupALLOCCTRLDATA();

  ih->data->child_id = 100; /* initial number */
  ih->data->show_state = IUP_HIDE;

  if (params)
  {
    Ihandle** iparams = (Ihandle**)params;
    if (*iparams)
      IupAppend(ih, *iparams);
  }

  iupDlgListAdd(ih);

  return IUP_NOERROR;
}

static void iDialogDestroyMethod(Ihandle* ih)
{
  iupDlgListRemove(ih);
}

static int iDialogSetMenuAttrib(Ihandle* ih, const char* value);

static void iDialogComputeNaturalSizeMethod(Ihandle* ih, int *w, int *h, int *children_expand)
{
  int decorwidth, decorheight;
  Ihandle* child = ih->firstchild;

  /* if does not have a menu, but the attribute is defined,
     try to update the menu before retrieving the decoration. */
  char* value = iupAttribGet(ih, "MENU");
  if (!ih->data->menu && value)
    iDialogSetMenuAttrib(ih, value);

  iupDialogGetDecorSize(ih, &decorwidth, &decorheight);
  *w = decorwidth;
  *h = decorheight;

  if (child)
  {
    /* update child natural size first */
    iupBaseComputeNaturalSize(child);

    *children_expand = child->expand;
    *w += child->naturalwidth;
    *h += child->naturalheight;
  }
}

static void iDialogSetChildrenCurrentSizeMethod(Ihandle* ih, int shrink)
{
  int decorwidth, decorheight, client_width, client_height;

  if (shrink)
  {
    client_width = ih->currentwidth;
    client_height = ih->currentheight;
  }
  else
  {
    client_width = iupMAX(ih->naturalwidth, ih->currentwidth);
    client_height = iupMAX(ih->naturalheight, ih->currentheight);
  }

  iupDialogGetDecorSize(ih, &decorwidth, &decorheight);

  client_width  -= decorwidth;
  client_height -= decorheight;
  if (client_width < 0) client_width = 0;
  if (client_height < 0) client_height = 0;

  iupBaseSetCurrentSize(ih->firstchild, client_width, client_height, shrink);
}

static void iDialogSetChildrenPositionMethod(Ihandle* ih, int x, int y)
{
  if (ih->firstchild)
  {
    char* offset = iupAttribGet(ih, "CHILDOFFSET");

    /* Native container, position is reset */
    x = 0;
    y = 0;

    if (offset) iupStrToIntInt(offset, &x, &y, 'x');

    if (iupAttribGetBoolean(ih, "CUSTOMFRAMEDRAW"))  /* Windows Only */
    {
      int border, caption, menu;
      iupdrvDialogGetDecoration(ih, &border, &caption, &menu);

      x += border;
      y += border + caption + menu;
    }

    /* Child coordinates are relative to client left-top corner. */
    iupBaseSetPosition(ih->firstchild, x, y);
  }
}

static void iDialogAfterHide(Ihandle* ih)
{
  IFni show_cb;

  /* process all pending messages, make sure the dialog is hidden */
  IupFlush();

  show_cb = (IFni)IupGetCallback(ih, "SHOW_CB");
  if (show_cb && show_cb(ih, ih->data->show_state) == IUP_CLOSE)
  {
    IupExitLoop();
  }
}

static void iDialogAfterShow(Ihandle* ih)
{
  Ihandle* old_focus;
  IFni show_cb;
  int old_show_state;

  /* process all pending messages, make sure the dialog is visible */
  IupFlush();

  old_focus = IupGetFocus();
  old_show_state = ih->data->show_state;

  show_cb = (IFni)IupGetCallback(ih, "SHOW_CB");
  if (show_cb && show_cb(ih, ih->data->show_state) == IUP_CLOSE)
  {
    IupExitLoop();
    return;
  }

  if (old_show_state == IUP_SHOW)
  {
    if (show_cb)
      IupFlush();  /* again to update focus */

    /* do it only if show_cb did NOT changed the current focus */
    if (old_focus == IupGetFocus())
    {
      Ihandle *startfocus = IupGetAttributeHandle(ih, "STARTFOCUS");
      if (startfocus)
        IupSetFocus(startfocus);
      else
        IupNextField(ih);
    }
  }
}

int iupDialogGetChildId(Ihandle* ih)
{
  int id;
  ih = IupGetDialog(ih);
  if (!ih) return -1;
  id = ih->data->child_id;
  ih->data->child_id = id+1;
  return id;
}

char* iupDialogGetChildIdStr(Ihandle* ih)
{
  Ihandle* dialog = IupGetDialog(ih);
  return iupStrReturnStrf("iup-%s-%d", ih->iclass->name, dialog->data->child_id);
}

static void iDialogListCheckLastVisible(int was_modal)
{
  if (iupDlgListVisibleCount() <= 0)
  {
    /* if this is the last window visible,
    exit message loop except when LOCKLOOP==YES */
    if (!was_modal && !iupStrBoolean(IupGetGlobal("LOCKLOOP")))
    {
      IupExitLoop();
    }
  }
}

static int iDialogUpdateVisibility(Ihandle* ih, int *x, int *y)
{
  /* save visible state before iupdrvDialogSetPlacement */
  /* because it can also show the window when changing placement. */
  int was_visible = iupdrvDialogIsVisible(ih);

  /* Update the position and placement */
  if (!iupdrvDialogSetPlacement(ih))
  {
    iDialogAdjustPos(ih, x, y);
    iupdrvDialogSetPosition(ih, *x, *y);
  }

  if (was_visible) /* already visible */
  {
    /* only re-show to raise the window */
    iupdrvDialogSetVisible(ih, 1);

    /* flush, then process show_cb and startfocus */
    iDialogAfterShow(ih);
    return 1;
  }

  return 0;
}

static void iDialogFirstShow(Ihandle* ih)
{
  ih->data->first_show = 1;

  /* actually show the window */
  /* test if placement already turned the dialog visible */
  if (!iupdrvDialogIsVisible(ih))
    iupdrvDialogSetVisible(ih, 1);

  /* increment visible count */
  iupDlgListVisibleInc();

  /* flush, then process show_cb and startfocus */
  iDialogAfterShow(ih);
}

static void iDialogModalLoop(Ihandle* ih)
{
  iDialogSetModal(ih);

  /* interrupt processing here */
  IupMainLoop();

  /* if window is still valid (IupDestroy not called),
  hide the dialog if still visible. */
  if (iupObjectCheck(ih))
  {
    iupAttribSet(ih, "_IUPDLG_WAS_MODAL", "1");

    iDialogUnSetModal(ih);
    iupDialogHide(ih);

    if (iupObjectCheck(ih))
      iupAttribSet(ih, "_IUPDLG_WAS_MODAL", NULL);
  }
  else
    iDialogListCheckLastVisible(1);
}

int iupDialogPopup(Ihandle* ih, int x, int y)
{
  int ret = iupClassObjectDlgPopup(ih, x, y);
  if (ret != IUP_INVALID) /* IUP_INVALID means it is not implemented */
    return ret;

  ih->data->show_state = IUP_SHOW;

  if (iDialogUpdateVisibility(ih, &x, &y))
  {
    if (!iupAttribGetBoolean(ih, "MODAL"))
      iDialogModalLoop(ih);

    return IUP_NOERROR;  /* if already visible, returns */
  }

  iDialogFirstShow(ih);

  iDialogModalLoop(ih);

  return IUP_NOERROR;
}

int iupDialogShowXY(Ihandle* ih, int x, int y)
{
  if (iupAttribGetBoolean(ih, "MODAL")) 
  {
    /* is modal, just update visibility and return */
    iDialogUpdateVisibility(ih, &x, &y);
    return IUP_NOERROR;
  }

  if (ih->data->popup_level != 0)
  {
    /* was disabled by a Popup, re-enable it */
    iupdrvSetActive(ih, 1);
    ih->data->popup_level = 0; /* Now it is at the current popup level */
  }

  if (iDialogUpdateVisibility(ih, &x, &y))
    return IUP_NOERROR;  /* if already visible, returns */

  iDialogFirstShow(ih);

  return IUP_NOERROR;
}

void iupDialogHide(Ihandle* ih)
{
  int was_modal = iupAttribGet(ih, "_IUPDLG_WAS_MODAL") != NULL;

  /* hidden at the system and marked hidden in IUP */
  if (!iupdrvDialogIsVisible(ih) && ih->data->show_state == IUP_HIDE) 
    return;

  /* marked hidden in IUP */
  ih->data->show_state = IUP_HIDE;

  /* if called IupHide for a Popup window */
  if (iupAttribGetBoolean(ih, "MODAL"))
  {
    was_modal = 1;
    iDialogUnSetModal(ih);
    IupExitLoop();
  }

  ih->data->ignore_resize = 1;

  /* actually hide the window */
  iupdrvDialogSetVisible(ih, 0);

  ih->data->ignore_resize = 0;

  /* decrement visible count */
  iupDlgListVisibleDec();
    
  /* process flush and process show_cb */
  iDialogAfterHide(ih);

  iDialogListCheckLastVisible(was_modal);
}


/****************************************************************/


static int iDialogSizeGetScale(const char* sz)
{
  if (!sz || sz[0] == 0) return 0;
  if (iupStrEqualNoCase(sz, "FULL"))     return 1;
  if (iupStrEqualNoCase(sz, "HALF"))     return 2;
  if (iupStrEqualNoCase(sz, "THIRD"))    return 3;
  if (iupStrEqualNoCase(sz, "QUARTER"))  return 4;
  if (iupStrEqualNoCase(sz, "EIGHTH"))   return 8;
  return 0;
}

static int iDialogSetSizeAttrib(Ihandle* ih, const char* value)
{
  if (!value)
  {
    ih->userwidth = 0;
    ih->userheight = 0;
  }
  else
  {
    char *sh, sw[40];
    strcpy(sw, value);
    sh = strchr(sw, 'x');
    if (!sh)
      sh = "";
    else
    {
      *sh = '\0';  /* to mark the end of sw */
      ++sh;
    }

    {
      int charwidth, charheight;
      int screen_width, screen_height;
      int wscale = iDialogSizeGetScale(sw);
      int hscale = iDialogSizeGetScale(sh);

      int width = 0, height = 0; 
      iupStrToIntInt(value, &width, &height, 'x');
      if (width < 0) width = 0;
      if (height < 0) height = 0;

      iupdrvFontGetCharSize(ih, &charwidth, &charheight);

      /* desktop size, excluding task bars and menu bars */
      iupdrvGetScreenSize(&screen_width, &screen_height);

      if (wscale)
        width = screen_width/wscale;
      else
        width = iupWIDTH2RASTER(width, charwidth);

      if (hscale)
        height = screen_height/hscale;
      else
        height = iupHEIGHT2RASTER(height, charheight);

      ih->userwidth = width;
      ih->userheight = height;
    }
  }

  /* must reset the current size,  */
  /* so the user or the natural size will be used to resize the dialog */
  ih->currentwidth = 0;
  ih->currentheight = 0;

  return 0;
}

static char* iDialogGetSizeAttrib(Ihandle* ih)
{
  int charwidth, charheight, width, height;

  if (ih->handle)
  {
    /* ih->currentwidth and/or ih->currentheight could have been reset in SetSize */
    iupdrvDialogGetSize(ih, NULL, &width, &height);
  }
  else
  {
    width = ih->userwidth;
    height = ih->userheight;
  }

  iupdrvFontGetCharSize(ih, &charwidth, &charheight);
  if (charwidth == 0 || charheight == 0)
    return NULL;  /* if font failed get from the hash table */

  return iupStrReturnIntInt(iupRASTER2WIDTH(width, charwidth), iupRASTER2HEIGHT(height, charheight), 'x');
}

static int iDialogSetRasterSizeAttrib(Ihandle* ih, const char* value)
{
  if (!value)
  {
    ih->userwidth = 0;
    ih->userheight = 0;
  }
  else
  {
    int w = 0, h = 0;
    iupStrToIntInt(value, &w, &h, 'x');
    if (w < 0) w = 0;
    if (h < 0) h = 0;
    ih->userwidth = w;
    ih->userheight = h;
  }

  /* must reset the current size also,  */
  /* so the user or the natural size will be used to resize the dialog */
  ih->currentwidth = 0;
  ih->currentheight = 0;

  return 0;
}

static char* iDialogGetRasterSizeAttrib(Ihandle* ih)
{
  int width, height;

  if (ih->handle)
  {
    /* ih->currentwidth and/or ih->currentheight could have been reset in SetRasterSize */
    iupdrvDialogGetSize(ih, NULL, &width, &height);
  }
  else
  {
    width = ih->userwidth;
    height = ih->userheight;
  }

  if (!width && !height)
    return NULL;

  return iupStrReturnIntInt(width, height, 'x');
}

static int iDialogSetNActiveAttrib(Ihandle* ih, const char* value)
{
  iupdrvSetActive(ih, iupStrBoolean(value));
  return 0;
}

static int iDialogSetVisibleAttrib(Ihandle* ih, const char* value)
{
  if (iupStrBoolean(value))
    IupShow(ih);
  else
    IupHide(ih);
  return 0;
}

void iupDialogUpdatePosition(Ihandle* ih)
{
  /* This function is used only by pre-defined popup native dialogs */

  int x = iupAttribGetInt(ih, "_IUPDLG_X");
  int y = iupAttribGetInt(ih, "_IUPDLG_Y");

  iupdrvDialogGetSize(ih, NULL, &(ih->currentwidth), &(ih->currentheight));

  /* handle always as visible for the first time */
  ih->data->first_show = 0;

  iDialogAdjustPos(ih, &x, &y);
  iupdrvDialogSetPosition(ih, x, y);
}

void iupDialogGetDecorSize(Ihandle* ih, int *decorwidth, int *decorheight)
{
  int border, caption, menu;
  iupdrvDialogGetDecoration(ih, &border, &caption, &menu);

  if (iupAttribGetBoolean(ih, "CUSTOMFRAME"))
  {
    *decorwidth = 0;
    *decorheight = 0;
  }
  else
  {
    *decorwidth = 2 * border;
    *decorheight = 2 * border + caption + menu;
  }
}

static int iDialogSetHideTaskbarAttrib(Ihandle *ih, const char *value)
{
  iupdrvDialogSetVisible(ih, !iupStrBoolean(value));
  return 0;
}

static int iDialogSetSimulateModalAttrib(Ihandle *ih, const char *value)
{
  Ihandle *ih_dlg;

  int sim_modal = iupStrBoolean(value);

  /* disable all visible dialogs different than this one */
  for (ih_dlg = iupDlgListFirst(); ih_dlg; ih_dlg = iupDlgListNext())
  {
    if (ih_dlg != ih &&
        ih_dlg->handle &&
        iupdrvDialogIsVisible(ih_dlg))
    {
      if (sim_modal)
        iupdrvSetActive(ih_dlg, 0);
      else
        iupdrvSetActive(ih_dlg, 1);
    }
  }

  return 0;
}

static int iDialogSetParentDialogAttrib(Ihandle *ih, const char *value)
{
  Ihandle* parent = IupGetHandle(value);
  InativeHandle* native_parent;
  if (parent && parent->handle)
    native_parent = parent->handle;
  else
    native_parent = (InativeHandle*)iupAttribGet(ih, "NATIVEPARENT");

  if (native_parent != iupDialogGetNativeParent(ih))
    iupdrvDialogSetParent(ih, native_parent);

  return 1;
}

static int iDialogSetDialogFrameAttrib(Ihandle *ih, const char *value)
{
  if (iupStrBoolean(value))
  {
    iupAttribSet(ih, "RESIZE", "NO");
    iupAttribSet(ih, "MAXBOX", "NO");
    iupAttribSet(ih, "MINBOX", "NO");
  }
  else
  {
    iupAttribSet(ih, "RESIZE", NULL);
    iupAttribSet(ih, "MAXBOX", NULL);
    iupAttribSet(ih, "MINBOX", NULL);
  }
  return 1;
}

static char* iDialogGetXAttrib(Ihandle *ih)
{
  int x = 0;
  iupdrvDialogGetPosition(ih, NULL, &x, NULL);
  return iupStrReturnInt(x);
}

static char* iDialogGetYAttrib(Ihandle *ih)
{
  int y = 0;
  iupdrvDialogGetPosition(ih, NULL, NULL, &y);
  return iupStrReturnInt(y);
}

static char* iDialogGetScreenPositionAttrib(Ihandle *ih)
{
  int x = 0, y = 0;
  iupdrvDialogGetPosition(ih, NULL, &x, &y);
  return iupStrReturnIntInt(x, y, ',');
}

static int iDialogSetMenuAttrib(Ihandle* ih, const char* value)
{
  if (!ih->handle)
  {
    Ihandle* menu = IupGetHandle(value);
    ih->data->menu = menu;
    return 1;
  }

  if (!value)
  {
    if (ih->data->menu && ih->data->menu->handle)
    {
      ih->data->ignore_resize = 1;
      IupUnmap(ih->data->menu);  /* this will remove the menu from the dialog */
      ih->data->ignore_resize = 0;

      ih->data->menu = NULL;
    }
  }
  else
  {
    Ihandle* menu = IupGetHandle(value);
    if (!menu || menu->iclass->nativetype != IUP_TYPEMENU || menu->parent)
      return 0;

    /* already the current menu and it is mapped */
    if (ih->data->menu && ih->data->menu==menu && menu->handle)
      return 1;

    /* the current menu is mapped, so unmap it */
    if (ih->data->menu && ih->data->menu->handle && ih->data->menu!=menu)
    {
      ih->data->ignore_resize = 1;
      IupUnmap(ih->data->menu);   /* this will remove the menu from the dialog */
      ih->data->ignore_resize = 0;
    }

    ih->data->menu = menu;

    menu->parent = ih;    /* use this to create a menu bar instead of a popup menu */

    ih->data->ignore_resize = 1;
    IupMap(menu);     /* this will automatically add the menu to the dialog */
    ih->data->ignore_resize = 0;
  }
  return 1;
}


/****************************************************************/


Ihandle* IupDialog(Ihandle* child)
{
  void *children[2];
  children[0] = child;
  children[1] = NULL;
  return IupCreatev("dialog", children);
}

Iclass* iupDialogNewClass(void)
{
  Iclass* ic = iupClassNew(NULL);

  ic->name = "dialog";
  ic->format = "h"; /* one Ihandle* */
  ic->nativetype = IUP_TYPEDIALOG;
  ic->childtype = IUP_CHILDMANY+1;  /* one child */
  ic->is_interactive = 1;

  /* Class functions */
  ic->New = iupDialogNewClass;
  ic->Create = iDialogCreateMethod;
  ic->Destroy = iDialogDestroyMethod;
  ic->ComputeNaturalSize = iDialogComputeNaturalSizeMethod;
  ic->SetChildrenCurrentSize = iDialogSetChildrenCurrentSizeMethod;
  ic->SetChildrenPosition = iDialogSetChildrenPositionMethod;

  /* Callbacks */
  iupClassRegisterCallback(ic, "SHOW_CB", "i");
  iupClassRegisterCallback(ic, "RESIZE_CB", "ii");
  iupClassRegisterCallback(ic, "CLOSE_CB", "");
  iupClassRegisterCallback(ic, "FOCUS_CB", "i");

  /* Common Callbacks */
  iupBaseRegisterCommonCallbacks(ic);

  /* Attribute functions */

  /* Common */
  iupBaseRegisterCommonAttrib(ic);

  /* Overwrite Common */
  iupClassRegisterAttribute(ic, "SIZE", iDialogGetSizeAttrib, iDialogSetSizeAttrib, NULL, NULL, IUPAF_NO_SAVE|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "RASTERSIZE", iDialogGetRasterSizeAttrib, iDialogSetRasterSizeAttrib, NULL, NULL, IUPAF_NO_SAVE|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "POSITION", NULL, NULL, NULL, NULL, IUPAF_NOT_SUPPORTED); /* forbidden in dialog */

  /* Base Container */
  iupClassRegisterAttribute(ic, "EXPAND", iupBaseContainerGetExpandAttrib, NULL, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);

  /* Native Container */
  iupClassRegisterAttribute(ic, "CHILDOFFSET", NULL, NULL, NULL, NULL, IUPAF_NOT_MAPPED | IUPAF_NO_INHERIT);

  /* Visual */
  iupBaseRegisterVisualAttrib(ic);

  /* Dialog only */
  iupClassRegisterAttribute(ic, "NACTIVE", iupBaseGetActiveAttrib, iDialogSetNActiveAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_DEFAULT | IUPAF_NO_INHERIT);

  /* Drag&Drop */
  iupdrvRegisterDragDropAttrib(ic);

  /* Overwrite Visual */
  /* the only case where VISIBLE default is NO, and must not be propagated to the dialog children */
  iupClassRegisterAttribute(ic, "VISIBLE", iupBaseGetVisibleAttrib, iDialogSetVisibleAttrib, IUPAF_SAMEASSYSTEM, "NO", IUPAF_NO_SAVE|IUPAF_NO_INHERIT);

  /* X and Y here are at the top left corner of the window, not the client area. */
  iupClassRegisterAttribute(ic, "X", iDialogGetXAttrib, NULL, NULL, NULL, IUPAF_NO_DEFAULTVALUE|IUPAF_READONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "Y", iDialogGetYAttrib, NULL, NULL, NULL, IUPAF_NO_DEFAULTVALUE|IUPAF_READONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SCREENPOSITION", iDialogGetScreenPositionAttrib, NULL, NULL, NULL, IUPAF_NO_DEFAULTVALUE|IUPAF_READONLY|IUPAF_NO_INHERIT);

  /* IupDialog only */
  iupClassRegisterAttribute(ic, "MENU", NULL, iDialogSetMenuAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "CURSOR", NULL, iupdrvBaseSetCursorAttrib, IUPAF_SAMEASSYSTEM, "ARROW", IUPAF_IHANDLENAME|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "HIDETASKBAR", NULL, iDialogSetHideTaskbarAttrib, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "MAXBOX", NULL, NULL, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "MENUBOX", NULL, NULL, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "MINBOX", NULL, NULL, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "RESIZE", NULL, NULL, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "BORDER", NULL, NULL, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "BORDERSIZE", iDialogGetBorderSizeAttrib, NULL, NULL, NULL, IUPAF_READONLY | IUPAF_NO_INHERIT);
  
  iupClassRegisterAttribute(ic, "DEFAULTENTER", NULL, NULL, NULL, NULL, IUPAF_IHANDLENAME|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "DEFAULTESC",   NULL, NULL, NULL, NULL, IUPAF_IHANDLENAME|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "DIALOGFRAME",  NULL, iDialogSetDialogFrameAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "PARENTDIALOG", NULL, iDialogSetParentDialogAttrib, NULL, NULL, IUPAF_NO_DEFAULTVALUE | IUPAF_IHANDLENAME | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SHRINK",       NULL, NULL, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "STARTFOCUS",   NULL, NULL, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "MODAL",        NULL, NULL, NULL, NULL, IUPAF_READONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "PLACEMENT",    NULL, NULL, "NORMAL", NULL, IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "SIMULATEMODAL", NULL, iDialogSetSimulateModalAttrib, NULL, NULL, IUPAF_WRITEONLY | IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "NATIVEPARENT", NULL, NULL, NULL, NULL, IUPAF_NO_STRING);

  iupdrvDialogInitClass(ic);

  return ic;
}
