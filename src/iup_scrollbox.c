/** \file
 * \brief iupscrollbox control
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "iup.h"
#include "iupcbs.h"
#include "iupkey.h"

#include "iup_object.h"
#include "iup_register.h"
#include "iup_attrib.h"
#include "iup_str.h"
#include "iup_stdcontrols.h"
#include "iup_layout.h"
#include "iup_drv.h"
#include "iup_childtree.h"


/*****************************************************************************\
|* Canvas Callbacks                                                          *|
\*****************************************************************************/

static int iScrollBoxScroll_CB(Ihandle *ih, int op, float posx, float posy)
{
  if (ih->firstchild)
  {
    int x, y;

    char* offset = iupAttribGet(ih, "CHILDOFFSET");

    if ((op == IUP_SBDRAGH || op == IUP_SBDRAGV) && !iupAttribGetBoolean(ih, "LAYOUTDRAG"))
      return IUP_DEFAULT;

    /* Native container, position is reset */
    x = 0;
    y = 0;

    if (offset) iupStrToIntInt(offset, &x, &y, 'x');

    if (IupGetInt(ih, "DX") > IupGetInt(ih, "XMAX")-iupdrvGetScrollbarSize())
      posx = 0;
    if (IupGetInt(ih, "DY") > IupGetInt(ih, "YMAX")-iupdrvGetScrollbarSize())
      posy = 0;

    x -= (int)posx;
    y -= (int)posy;

    iupBaseSetPosition(ih->firstchild, x, y);
    iupLayoutUpdate(ih->firstchild);
  }
  (void)op;
  return IUP_DEFAULT;
}

static int iScrollBoxButton_CB(Ihandle *ih, int but, int pressed, int x, int y, char* status)
{
  if (but==IUP_BUTTON1 && pressed)
  {
    iupAttribSetInt(ih, "_IUP_START_X", x);
    iupAttribSetInt(ih, "_IUP_START_Y", y);
    iupAttribSetInt(ih, "_IUP_START_POSX", IupGetInt(ih, "POSX"));
    iupAttribSetInt(ih, "_IUP_START_POSY", IupGetInt(ih, "POSY"));
    iupAttribSet(ih, "_IUP_DRAG_SB", "1");
  }
  if (but==IUP_BUTTON1 && !pressed)
    iupAttribSet(ih, "_IUP_DRAG_SB", NULL);
  (void)status;
  return IUP_DEFAULT;
}

static int iScrollBoxMotion_CB(Ihandle *ih, int x, int y, char* status)
{
  if (iup_isbutton1(status) && iupAttribGet(ih, "_IUP_DRAG_SB"))
  {
    int start_x = iupAttribGetInt(ih, "_IUP_START_X");
    int start_y = iupAttribGetInt(ih, "_IUP_START_Y");
    int dx = x - start_x;
    int dy = y - start_y;
    int posx = iupAttribGetInt(ih, "_IUP_START_POSX");
    int posy = iupAttribGetInt(ih, "_IUP_START_POSY");
    IupSetInt(ih, "POSX", posx-dx);  /* drag direction is opposite to scrollbar */
    IupSetInt(ih, "POSY", posy-dy);

    iScrollBoxScroll_CB(ih, 0, IupGetFloat(ih, "POSX"), IupGetFloat(ih, "POSY"));
  }
  return IUP_DEFAULT;
}


/*****************************************************************************\
|* Methods                                                                   *|
\*****************************************************************************/


static int iScrollBoxGetChildPosition(Ihandle* ih, Ihandle* child, int *posx, int *posy)
{
  while (child->parent && child != ih)
  {
    *posx += child->x;
    *posy += child->y;

    child = iupChildTreeGetNativeParent(child);
  }

  if (!child->parent)
    return 0;
  else
    return 1;
}

static int iScrollBoxSetScrollToChildHandleAttrib(Ihandle* ih, const char* value)
{
  Ihandle* child = (Ihandle*)value;
  if (iupObjectCheck(child))
  {
    int posx = 0, posy = 0;
    if (iScrollBoxGetChildPosition(ih, child, &posx, &posy))
    {
      IupSetInt(ih, "POSX", posx);
      IupSetInt(ih, "POSY", posy);
      iScrollBoxScroll_CB(ih, 0, IupGetFloat(ih, "POSX"), IupGetFloat(ih, "POSY"));
    }
  }
  return 0;
}

static int iScrollBoxSetScrollToChildAttrib(Ihandle* ih, const char* value)
{
  return iScrollBoxSetScrollToChildHandleAttrib(ih, (char*)IupGetHandle(value));
}

static int iScrollBoxSetScrollToAttrib(Ihandle* ih, const char* value)
{
  if (iupStrEqualNoCase(value, "TOP"))
  {
    IupSetInt(ih, "POSX", 0);
    IupSetInt(ih, "POSY", 0);
    iScrollBoxScroll_CB(ih, 0, IupGetFloat(ih, "POSX"), IupGetFloat(ih, "POSY"));
  }
  else if (iupStrEqualNoCase(value, "BOTTOM"))
  {
    IupSetInt(ih, "POSX", 0);
    IupSetInt(ih, "POSY", IupGetInt(ih, "YMAX") - IupGetInt(ih, "DY"));
    iScrollBoxScroll_CB(ih, 0, IupGetFloat(ih, "POSX"), IupGetFloat(ih, "POSY"));
  }
  else
  {
    int posx, posy;
    if (iupStrToIntInt(value, &posx, &posy, ',') == 2)
    {
      IupSetInt(ih, "POSX", posx);
      IupSetInt(ih, "POSY", posy);
      iScrollBoxScroll_CB(ih, 0, IupGetFloat(ih, "POSX"), IupGetFloat(ih, "POSY"));
    }
  }
  return 0;
}

static char* iScrollBoxGetExpandAttrib(Ihandle* ih)
{
  if (iupAttribGetBoolean(ih, "CANVASBOX"))
    return iupBaseGetExpandAttrib(ih);
  else
    return iupBaseContainerGetExpandAttrib(ih);
}

static int iScrollBoxSetExpandAttrib(Ihandle* ih, const char* value)
{
  if (iupAttribGetBoolean(ih, "CANVASBOX"))
    return iupBaseSetExpandAttrib(ih, value);
  else
    return 1;  /* store on the hash table */
}

static void iScrollBoxComputeNaturalSizeMethod(Ihandle* ih, int *w, int *h, int *children_expand)
{
  if (ih->firstchild)
  {
    /* update child natural size first */
    iupBaseComputeNaturalSize(ih->firstchild);
  }

  if (!iupAttribGetBoolean(ih, "CANVASBOX"))
  {
    /* ScrollBox size does not depends on the child size,
     its natural size must be 0 to be free of restrictions. */
    if (ih->currentwidth == 0 && ih->currentheight == 0 && ih->firstchild)
    {
      *w = ih->firstchild->naturalwidth;
      *h = ih->firstchild->naturalheight;
    }
    else
    {
      *w = 0;
      *h = 0;
    }
  }

  /* Also set expand to its own expand so it will not depend on children */
  *children_expand = ih->expand;
}

static void iScrollBoxUpdateVisibleArea(Ihandle* ih, int xmax, int ymax)
{
  int width = ih->currentwidth,
    height = ih->currentheight;

  /* if child is greater than scrollbox in one direction,
  then it has scrollbars
  but this affects the opposite direction */

  if (xmax > ih->currentwidth)
    height -= iupdrvGetScrollbarSize();

  if (ymax > ih->currentheight)
    width -= iupdrvGetScrollbarSize();

  if (xmax <= ih->currentwidth && xmax > width)
    height -= iupdrvGetScrollbarSize();

  if (ymax <= ih->currentheight && ymax > height)
    width -= iupdrvGetScrollbarSize();

  if (width < 0) width = 0;
  if (height < 0) height = 0;

  IupSetInt(ih, "DX", width);
  IupSetInt(ih, "DY", height);
}

static void iScrollBoxSetChildrenCurrentSizeMethod(Ihandle* ih, int shrink)
{
  Ihandle* child = ih->firstchild;

  if (child)
  {
    int w, h, has_sb_horiz=0, has_sb_vert=0;

    /* If child is greater than scrollbox area, use child natural size,
       else use current scrollbox size;
       So this will let the child be greater than the scrollbox,
       or let the child expand to the scrollbox.  */

    if (child->naturalwidth > ih->currentwidth)
    {
      w = child->naturalwidth;
      has_sb_horiz = 1;
    }
    else
      w = ih->currentwidth;  /* expand space */

    if (child->naturalheight > ih->currentheight)
    {
      h = child->naturalheight;
      has_sb_vert = 1;
    }
    else
      h = ih->currentheight; /* expand space */

    if (!has_sb_horiz && has_sb_vert)
      w -= iupdrvGetScrollbarSize();  /* reduce expand space */

    if (has_sb_horiz && !has_sb_vert)
      h -= iupdrvGetScrollbarSize();  /* reduce expand space */

    /* Now w and h is a possible child size */
    iupBaseSetCurrentSize(child, w, h, shrink);

    /* Now we use the actual child size as the virtual area */
    IupSetInt(ih, "XMAX", child->currentwidth);
    IupSetInt(ih, "YMAX", child->currentheight);

    /* Finally update the visible area */
    iScrollBoxUpdateVisibleArea(ih, child->currentwidth, child->currentheight);
  }
  else
  {
    IupSetAttribute(ih, "XMAX", "0");
    IupSetAttribute(ih, "YMAX", "0");
    IupSetAttribute(ih, "DX", "0");
    IupSetAttribute(ih, "DY", "0");
  }
}

static void iScrollBoxSetChildrenPositionMethod(Ihandle* ih, int x, int y)
{
  if (ih->firstchild)
  {
    char* offset = iupAttribGet(ih, "CHILDOFFSET");

    /* Native container, position is reset */
    x = 0;
    y = 0;

    if (offset) iupStrToIntInt(offset, &x, &y, 'x');

    x -= IupGetInt(ih, "POSX");
    y -= IupGetInt(ih, "POSY");

    /* Child coordinates are relative to client left-top corner. */
    iupBaseSetPosition(ih->firstchild, x, y);
  }
}

static int iScrollBoxCreateMethod(Ihandle* ih, void** params)
{
  /* Setting callbacks */
  IupSetCallback(ih, "SCROLL_CB",    (Icallback)iScrollBoxScroll_CB);
  IupSetCallback(ih, "BUTTON_CB",    (Icallback)iScrollBoxButton_CB);
  IupSetCallback(ih, "MOTION_CB",    (Icallback)iScrollBoxMotion_CB);

  IupSetAttribute(ih, "CANFOCUS", "NO");

  if (params)
  {
    Ihandle** iparams = (Ihandle**)params;
    if (iparams[0]) IupAppend(ih, iparams[0]);
  }

  return IUP_NOERROR;
}

Iclass* iupScrollBoxNewClass(void)
{
  Iclass* ic = iupClassNew(iupRegisterFindClass("canvas"));

  ic->name   = "scrollbox";
  ic->format = "h";   /* one Ihandle* */
  ic->nativetype = IUP_TYPECANVAS;
  ic->childtype  = IUP_CHILDMANY+1;  /* 1 child */
  ic->is_interactive = 1;

  /* Class functions */
  ic->New = iupScrollBoxNewClass;
  ic->Create  = iScrollBoxCreateMethod;

  ic->ComputeNaturalSize = iScrollBoxComputeNaturalSizeMethod;
  ic->SetChildrenCurrentSize = iScrollBoxSetChildrenCurrentSizeMethod;
  ic->SetChildrenPosition = iScrollBoxSetChildrenPositionMethod;

  /* Base Container */
  iupClassRegisterAttribute(ic, "EXPAND", iScrollBoxGetExpandAttrib, iScrollBoxSetExpandAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NOT_MAPPED | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "CLIENTOFFSET", iupBaseGetClientOffsetAttrib, NULL, NULL, NULL, IUPAF_NOT_MAPPED | IUPAF_READONLY | IUPAF_NO_INHERIT);
  {
    IattribGetFunc drawsize_get = NULL;
    iupClassRegisterGetAttribute(ic, "DRAWSIZE", &drawsize_get, NULL, NULL, NULL, NULL);
    iupClassRegisterAttribute(ic, "CLIENTSIZE", drawsize_get, NULL, NULL, NULL, IUPAF_READONLY|IUPAF_NO_INHERIT);
  }

  /* Native Container */
  iupClassRegisterAttribute(ic, "CHILDOFFSET", NULL, NULL, NULL, NULL, IUPAF_NOT_MAPPED | IUPAF_NO_INHERIT);

  /* replace IupCanvas behavior */
  iupClassRegisterReplaceAttribFunc(ic, "BGCOLOR", iupBaseNativeParentGetBgColorAttrib, NULL);
  iupClassRegisterReplaceAttribDef(ic, "BGCOLOR", "DLGBGCOLOR", NULL);
  iupClassRegisterReplaceAttribDef(ic, "BORDER", "NO", NULL);
  iupClassRegisterReplaceAttribFlags(ic, "BORDER", IUPAF_READONLY | IUPAF_NO_INHERIT);
  iupClassRegisterReplaceAttribDef(ic, "SCROLLBAR", "YES", NULL);

  /* Scrollbox */
  iupClassRegisterAttribute(ic, "SCROLLTO", NULL, iScrollBoxSetScrollToAttrib, NULL, NULL, IUPAF_WRITEONLY | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SCROLLTOCHILD", NULL, iScrollBoxSetScrollToChildAttrib, NULL, NULL, IUPAF_IHANDLENAME | IUPAF_WRITEONLY | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SCROLLTOCHILD_HANDLE", NULL, iScrollBoxSetScrollToChildHandleAttrib, NULL, NULL, IUPAF_IHANDLE | IUPAF_WRITEONLY | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "LAYOUTDRAG", NULL, NULL, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NOT_MAPPED | IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "CANVASBOX", NULL, NULL, NULL, NULL, IUPAF_NOT_MAPPED | IUPAF_NO_INHERIT);

  return ic;
}

Ihandle* IupScrollBox(Ihandle* child)
{
  void *children[2];
  children[0] = (void*)child;
  children[1] = NULL;
  return IupCreatev("scrollbox", children);
}
