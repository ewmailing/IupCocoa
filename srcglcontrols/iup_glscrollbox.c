/** \file
 * \brief GLScrollBox control
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "iup.h"
#include "iupcbs.h"
#include "iupglcontrols.h"

#include "iup_object.h"
#include "iup_register.h"
#include "iup_attrib.h"
#include "iup_str.h"
#include "iup_layout.h"

#include "iup_glcontrols.h"
#include "iup_glscrollbars.h"
#include "iup_glsubcanvas.h"


static int iGLScrollBoxBUTTON_CB(Ihandle *ih, int button, int pressed, int x, int y, char* status)
{
  if (button != IUP_BUTTON1)
    return IUP_DEFAULT;

  if (iupGLScrollbarsButton(ih, pressed, x, y))
    iupGLSubCanvasRedraw(ih);

  (void)status;
  return IUP_DEFAULT;
}

static int iGLScrollBoxMOTION_CB(Ihandle *ih, int x, int y, char* status)
{
  if (iupGLScrollbarsMotion(ih, x, y))
    iupGLSubCanvasRedraw(ih);

  (void)status;
  return IUP_DEFAULT;
}

static int iGLScrollBoxENTERWINDOW_CB(Ihandle* ih, int x, int y)
{
  iupGLScrollbarsEnterWindow(ih, x, y);

  return iupGLSubCanvasRedraw(ih);
}

static int iGLScrollBoxLEAVEWINDOW_CB(Ihandle* ih)
{
  iupGLScrollbarsLeaveWindow(ih);

  return iupGLSubCanvasRedraw(ih);
}

static int iGLScrollBoxACTION_CB(Ihandle* ih)
{
  int active = iupAttribGetInt(ih, "ACTIVE");

  iupGLScrollbarsDraw(ih, active);

  return IUP_DEFAULT;
}

static char* iGLScrollBoxGetClientSizeAttrib(Ihandle* ih)
{
  int dx = iupAttribGetInt(ih, "DX");
  int dy = iupAttribGetInt(ih, "DY");
  return iupStrReturnIntInt(dx, dy, 'x');
}


/*****************************************************************************\
|* Methods                                                                   *|
\*****************************************************************************/


static void iGLScrollBoxComputeNaturalSizeMethod(Ihandle* ih, int *w, int *h, int *children_expand)
{
  if (ih->firstchild)
  {
    /* update child natural size first */
    iupBaseComputeNaturalSize(ih->firstchild);
  }

  /* ScrollBox size does not depends on the child size,
     its natural size must be 0 to be free of restrictions. */
  (void)w;
  (void)h;

  /* Also set expand to its own expand so it will not depend on children */
  *children_expand = ih->expand;
}

static void iGLScrollBoxUpdateVisibleArea(Ihandle* ih, int xmax, int ymax)
{
  int width = ih->currentwidth,
    height = ih->currentheight;

  /* if child is greater than scrollbox in one direction,
  then it has scrollbars
  but this affects the opposite direction */

  if (xmax > ih->currentwidth)
    height -= iupGLScrollbarsGetSize(ih);

  if (ymax > ih->currentheight)
    width -= iupGLScrollbarsGetSize(ih);

  if (xmax <= ih->currentwidth && xmax > width)
    height -= iupGLScrollbarsGetSize(ih);

  if (ymax <= ih->currentheight && ymax > height)
    width -= iupGLScrollbarsGetSize(ih);

  if (width < 0) width = 0;
  if (height < 0) height = 0;

  iupAttribSetInt(ih, "DX", width);
  iupAttribSetInt(ih, "DY", height);

  iupGLScrollbarsCheckPosX(ih);
  iupGLScrollbarsCheckPosY(ih);
}

static void iGLScrollBoxSetChildrenCurrentSizeMethod(Ihandle* ih, int shrink)
{
  Ihandle* child = ih->firstchild;

  if (child)
  {
    int w, h, has_sb_horiz=0, has_sb_vert=0;

    /* If child is greater than scrollbox area, use child natural size,
       else use current scrollbox size;
       So this will let the child be greater than the scrollbox,
       or let the child expand to the scrollbox. */

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
      w -= iupGLScrollbarsGetSize(ih);  /* reduce expand space */

    if (has_sb_horiz && !has_sb_vert)
      h -= iupGLScrollbarsGetSize(ih);  /* reduce expand space */

    /* Now w and h is a possible child size */
    iupBaseSetCurrentSize(child, w, h, shrink);

    /* Now we use the actual child size as the virtual area */
    iupAttribSetInt(ih, "XMAX", child->currentwidth);
    iupAttribSetInt(ih, "YMAX", child->currentheight);

    /* Finally update the visible area */
    iGLScrollBoxUpdateVisibleArea(ih, child->currentwidth, child->currentheight);
  }
  else
  {
    iupAttribSet(ih, "XMAX", "0");
    iupAttribSet(ih, "YMAX", "0");
    iupAttribSet(ih, "DX", "0");
    iupAttribSet(ih, "DY", "0");
    iupAttribSet(ih, "POSX", "0");
    iupAttribSet(ih, "POSY", "0");
  }
}

static void iGLScrollBoxSetChildrenPositionMethod(Ihandle* ih, int x, int y)
{
  if (ih->firstchild)
  {
    iupBaseSetPosition(ih->firstchild, x - iupAttribGetInt(ih, "POSX"),
                                       y - iupAttribGetInt(ih, "POSY"));
  }
}

static int iGLScrollBoxCreateMethod(Ihandle* ih, void** params)
{
  IupSetCallback(ih, "GL_ACTION", (Icallback)iGLScrollBoxACTION_CB);
  IupSetCallback(ih, "GL_BUTTON_CB", (Icallback)iGLScrollBoxBUTTON_CB);
  IupSetCallback(ih, "GL_MOTION_CB", (Icallback)iGLScrollBoxMOTION_CB);
  IupSetCallback(ih, "GL_LEAVEWINDOW_CB", (Icallback)iGLScrollBoxLEAVEWINDOW_CB);
  IupSetCallback(ih, "GL_ENTERWINDOW_CB", (Icallback)iGLScrollBoxENTERWINDOW_CB);

  if (params)
  {
    Ihandle** iparams = (Ihandle**)params;
    if (iparams[0]) IupAppend(ih, iparams[0]);
  }

  return IUP_NOERROR;
}

Iclass* iupGLScrollBoxNewClass(void)
{
  Iclass* ic = iupClassNew(iupRegisterFindClass("glsubcanvas"));

  ic->name   = "glscrollbox";
  ic->format = "h";   /* one Ihandle* */
  ic->nativetype = IUP_TYPEVOID;
  ic->childtype  = IUP_CHILDMANY+1;  /* 1 child */
  ic->is_interactive = 0;

  /* Class functions */
  ic->New = iupGLScrollBoxNewClass;
  ic->Create  = iGLScrollBoxCreateMethod;

  ic->ComputeNaturalSize = iGLScrollBoxComputeNaturalSizeMethod;
  ic->SetChildrenCurrentSize = iGLScrollBoxSetChildrenCurrentSizeMethod;
  ic->SetChildrenPosition = iGLScrollBoxSetChildrenPositionMethod;

  /* Base Container */
  iupClassRegisterAttribute(ic, "EXPAND", iupBaseContainerGetExpandAttrib, NULL, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "CLIENTSIZE", iGLScrollBoxGetClientSizeAttrib, NULL, NULL, NULL, IUPAF_NOT_MAPPED | IUPAF_READONLY | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "CLIENTOFFSET", iupBaseGetClientOffsetAttrib, NULL, NULL, NULL, IUPAF_NOT_MAPPED | IUPAF_READONLY | IUPAF_NO_INHERIT);

  iupGLScrollbarsRegisterAttrib(ic);

  return ic;
}

Ihandle* IupGLScrollBox(Ihandle* child)
{
  void *children[2];
  children[0] = (void*)child;
  children[1] = NULL;
  return IupCreatev("glscrollbox", children);
}
