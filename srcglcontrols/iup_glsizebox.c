/** \file
 * \brief iupsbox control
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "iup.h"
#include "iupglcontrols.h"

#include "iup_object.h"
#include "iup_attrib.h"
#include "iup_str.h"
#include "iup_register.h"
#include "iup_layout.h"

#include "iup_glcontrols.h"
#include "iup_gldraw.h"
#include "iup_glsubcanvas.h"


#define ISBOX_THICK 4

enum { ISBOX_VERTICAL=1, ISBOX_HORIZONTAL=2, ISBOX_BOTH=3 };

struct _IcontrolData
{
  int isholding, hold_resizer;
  int start_x, start_y;
  int start_w, start_h;

  int resizers;
};


static int iGLSizeBoxGetYborder(Ihandle* ih)
{
  if (ih->data->resizers & ISBOX_VERTICAL)
    return ISBOX_THICK;
  else
    return 0;
}

static int iGLSizeBoxGetXborder(Ihandle* ih)
{
  if (ih->data->resizers & ISBOX_HORIZONTAL)
    return ISBOX_THICK;
  else
    return 0;
}

static int iGLSizeBoxSetCursor(Ihandle* ih, int x, int y)
{
  int resizer = 0;

  if (ih->data->resizers & ISBOX_HORIZONTAL)
  {
    if (x >= ih->currentwidth - 1 - ISBOX_THICK)
      resizer = ISBOX_HORIZONTAL;
  }

  if (ih->data->resizers & ISBOX_VERTICAL)
  {
    if (y >= ih->currentheight - 1 - ISBOX_THICK)
    {
      if (resizer != 0)
        resizer = ISBOX_BOTH;
      else
        resizer = ISBOX_VERTICAL;
    }
  }

  if (resizer == ISBOX_HORIZONTAL)
    iupAttribSet(ih, "CURSOR", "RESIZE_WE");
  else if (resizer == ISBOX_VERTICAL)
    iupAttribSet(ih, "CURSOR", "RESIZE_NS");
  else if (resizer == ISBOX_BOTH)
    iupAttribSet(ih, "CURSOR", "RESIZE_NW");
  else
    iupAttribSet(ih, "CURSOR", NULL);

  return resizer;
}

static void iGLSizeBoxResizeChild(Ihandle* ih, int resizer, int w, int h)
{
  Ihandle* gl_parent = (Ihandle*)iupAttribGet(ih, "GL_CANVAS");
  Ihandle* child = ih->firstchild;
  int redraw = 0;

  iupLayoutApplyMinMaxSize(child, &w, &h);

  if (resizer & ISBOX_HORIZONTAL)
  {
    if (w != child->currentwidth)
    {
      child->userwidth = w;
      redraw = 1;
    }
  }

  if (resizer & ISBOX_VERTICAL)
  {
    if (h != child->currentheight)
    {
      child->userheight = h;
      redraw = 1;
    }
  }

  if (redraw)
  {
    IupRefreshChildren(gl_parent);  /* this will recompute the layout of the glcanvasbox only */
    IupSetAttribute(gl_parent, "REDRAW", NULL);  /* redraw the whole box */
  }
}


/*****************************************************************************\
|* Callbacks of canvas bar                                                   *|
\*****************************************************************************/


static int iGLSizeBoxBUTTON_CB(Ihandle* ih, int button, int pressed, int x, int y, char* status)
{
  Ihandle* child = ih->firstchild;
  ih->data->isholding = 0;

  if (button != IUP_BUTTON1 || !child)
    return IUP_DEFAULT;

  if (pressed)
  {
    ih->data->hold_resizer = iGLSizeBoxSetCursor(ih, x, y);

    if (ih->data->hold_resizer)
    {
      ih->data->isholding = 1;

      /* Save the cursor position */
      ih->data->start_x = ih->x + x;
      ih->data->start_y = ih->y + y;

      /* Save the initial size */
      ih->data->start_w = child->currentwidth;
      ih->data->start_h = child->currentheight;
    }
  }

  (void)status;
  return IUP_DEFAULT;
}

static int iGLSizeBoxMOTION_CB(Ihandle* ih, int x, int y, char* status)
{
  iGLSizeBoxSetCursor(ih, x, y);

  if (ih->data->isholding)
  {
    int final_x = ih->x + x;
    int final_y = ih->y + y;

    int diff_x = final_x - ih->data->start_x;
    int diff_y = final_y - ih->data->start_y;

    int w = diff_x + ih->data->start_w;
    int h = diff_y + ih->data->start_h;

    iGLSizeBoxResizeChild(ih, ih->data->hold_resizer, w, h);
  }

  (void)status;
  return IUP_DEFAULT;
}

static int iGLSizeBoxACTION_CB(Ihandle* ih)
{
  int active = iupAttribGetInt(ih, "ACTIVE");
  char* fgcolor = iupAttribGetStr(ih, "FORECOLOR");
  int xmin, xmax, ymin, ymax;

  if (ih->data->resizers & ISBOX_HORIZONTAL)
  {
    xmin = ih->currentwidth - ISBOX_THICK;
    xmax = ih->currentwidth - 1;
    ymin = 0;
    ymax = ih->currentheight - 1;

    iupGLDrawBox(ih, xmin, xmax, ymin, ymax, fgcolor, active);
  }

  if (ih->data->resizers & ISBOX_VERTICAL)
  {
    xmin = 0;
    xmax = ih->currentwidth - 1;
    ymin = ih->currentheight - ISBOX_THICK;
    ymax = ih->currentheight - 1;

    iupGLDrawBox(ih, xmin, xmax, ymin, ymax, fgcolor, active);
  }

  return IUP_DEFAULT;
}

static int iGLSizeBoxENTERWINDOW_CB(Ihandle* ih, int x, int y)
{
  iGLSizeBoxSetCursor(ih, x, y);

  return iupGLSubCanvasRedraw(ih);
}

static int iGLSizeBoxLEAVEWINDOW_CB(Ihandle* ih)
{
  iupAttribSet(ih, "CURSOR", NULL);

  return iupGLSubCanvasRedraw(ih);
}

    
/*****************************************************************************\
|* Attributes                                                                *|
\*****************************************************************************/


static char* iGLSizeBoxGetClientSizeAttrib(Ihandle* ih)
{
  int width = ih->currentwidth - iGLSizeBoxGetXborder(ih);
  int height = ih->currentheight - iGLSizeBoxGetYborder(ih);
  if (width < 0) width = 0;
  if (height < 0) height = 0;
  return iupStrReturnIntInt(width, height, 'x');
}

static int iGLSizeBoxSetResizersAttrib(Ihandle* ih, const char* value)
{
  if (iupStrEqualNoCase(value, "VERTICAL"))
    ih->data->resizers = ISBOX_VERTICAL;
  else if(iupStrEqualNoCase(value, "HORIZONTAL"))
    ih->data->resizers = ISBOX_HORIZONTAL;
  else  /* Default = BOTH */
    ih->data->resizers = ISBOX_BOTH;
  return 0;  /* do not store value in hash table */
}

static char* iGLSizeBoxGetResizersAttrib(Ihandle* ih)
{
  if (ih->data->resizers == ISBOX_HORIZONTAL)
    return "HORIZONTAL";
  else if (ih->data->resizers == ISBOX_VERTICAL)
    return "VERTICAL";
  else 
    return "BOTH";
}


/*****************************************************************************\
|* Methods                                                                   *|
\*****************************************************************************/


static void iGLSizeBoxComputeNaturalSizeMethod(Ihandle* ih, int *w, int *h, int *children_expand)
{
  int natural_w = iGLSizeBoxGetXborder(ih); 
  int natural_h = iGLSizeBoxGetYborder(ih);
  Ihandle* child = ih->firstchild;

  if (child)
  {
    /* update child natural size first */
    iupBaseComputeNaturalSize(child);

    natural_w += child->naturalwidth;
    natural_h += child->naturalheight;
    
    *children_expand = child->expand;
  }

  *w = natural_w;
  *h = natural_h;
}

static void iGLSizeBoxSetChildrenCurrentSizeMethod(Ihandle* ih, int shrink)
{
  if (ih->firstchild)
  {
    int width = ih->currentwidth - iGLSizeBoxGetXborder(ih);
    int height = ih->currentheight - iGLSizeBoxGetYborder(ih);
    if (width < 0) width = 0;
    if (height < 0) height = 0;
    iupBaseSetCurrentSize(ih->firstchild, width, height, shrink);
  }
}

static void iGLSizeBoxSetChildrenPositionMethod(Ihandle* ih, int x, int y)
{
  if (ih->firstchild)
    iupBaseSetPosition(ih->firstchild, x, y);
}

static int iGLSizeBoxCreateMethod(Ihandle* ih, void** params)
{
  ih->data = iupALLOCCTRLDATA();

  ih->data->resizers = ISBOX_BOTH;

  /* Setting callbacks */
  IupSetCallback(ih, "GL_ACTION", (Icallback)iGLSizeBoxACTION_CB);
  IupSetCallback(ih, "GL_BUTTON_CB", (Icallback)iGLSizeBoxBUTTON_CB);
  IupSetCallback(ih, "GL_MOTION_CB", (Icallback)iGLSizeBoxMOTION_CB);
  IupSetCallback(ih, "GL_LEAVEWINDOW_CB", iGLSizeBoxLEAVEWINDOW_CB);
  IupSetCallback(ih, "GL_ENTERWINDOW_CB", (Icallback)iGLSizeBoxENTERWINDOW_CB);

  if (params)
  {
    Ihandle** iparams = (Ihandle**)params;
    if (*iparams)
      IupAppend(ih, *iparams);
  }

  return IUP_NOERROR;
}

Iclass* iupGLSizeBoxNewClass(void)
{
  Iclass* ic = iupClassNew(iupRegisterFindClass("glsubcanvas"));

  ic->name   = "glsizebox";
  ic->format = "h";   /* one Ihandle* */
  ic->nativetype = IUP_TYPEVOID;
  ic->childtype  = IUP_CHILDMANY+1;  /* one child */
  ic->is_interactive = 0;

  /* Class functions */
  ic->New = iupGLSizeBoxNewClass;
  ic->Create  = iGLSizeBoxCreateMethod;
  ic->Map     = iupBaseTypeVoidMapMethod;

  ic->ComputeNaturalSize = iGLSizeBoxComputeNaturalSizeMethod;
  ic->SetChildrenCurrentSize = iGLSizeBoxSetChildrenCurrentSizeMethod;
  ic->SetChildrenPosition    = iGLSizeBoxSetChildrenPositionMethod;

  /* Base Container */
  iupClassRegisterAttribute(ic, "EXPAND", iupBaseContainerGetExpandAttrib, NULL, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "CLIENTSIZE", iGLSizeBoxGetClientSizeAttrib, NULL, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_READONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "CLIENTOFFSET", iupBaseGetClientOffsetAttrib, NULL, NULL, NULL, IUPAF_NOT_MAPPED | IUPAF_READONLY | IUPAF_NO_INHERIT);

  /* Visual */
  iupClassRegisterAttribute(ic, "FORECOLOR", NULL, NULL, IUPAF_SAMEASSYSTEM, "90 190 255", IUPAF_NO_INHERIT);
  
  /* IupSizeBox only */
  iupClassRegisterAttribute(ic, "RESIZERS", iGLSizeBoxGetResizersAttrib, iGLSizeBoxSetResizersAttrib, IUPAF_SAMEASSYSTEM, "BOTH", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);

  return ic;
}

Ihandle* IupGLSizeBox(Ihandle* child)
{
  void *children[2];
  children[0] = (void*)child;
  children[1] = NULL;
  return IupCreatev("glsizebox", children);
}
