/** \file
 * \brief GLFrame Control.
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "iup.h"
#include "iupcbs.h"
#include "iupglcontrols.h"

#include "iup_object.h"
#include "iup_attrib.h"
#include "iup_str.h"
#include "iup_register.h"

#include "iup_glcontrols.h"
#include "iup_gldraw.h"
#include "iup_glicon.h"
#include "iup_glsubcanvas.h"


static int iGLFrameACTION(Ihandle* ih)
{
  char *image = iupAttribGet(ih, "IMAGE");
  char* title = iupAttribGet(ih, "TITLE");
  char* bordercolor = iupAttribGetStr(ih, "FRAMECOLOR");
  float bwidth = iupAttribGetFloat(ih, "FRAMEWIDTH");
  int border_width = (int)ceil(bwidth);
  int active = iupAttribGetInt(ih, "ACTIVE");

  if (image || title)
  {
    char* fgcolor = iupAttribGetStr(ih, "FORECOLOR");
    int off = iupAttribGetInt(ih, "TITLEOFFSET");
    int title_box = iupAttribGetInt(ih, "TITLEBOX");
    int w = 0, h = 0;
    iupGLIconGetSize(ih, image, title, &w, &h);
    if (w > ih->currentwidth - 2 * border_width)
      w = ih->currentwidth - 2 * border_width;

    if (title_box)
    {
      char* bgimage = iupAttribGet(ih, "TITLEBACKIMAGE");

      /* draw border */
      iupGLDrawRect(ih, 0, ih->currentwidth - 1, 0, ih->currentheight - 1, bwidth, bordercolor, 1, 0);

      /* draw box */
      if (bgimage)
        iupGLDrawImageZoom(ih, border_width, ih->currentwidth - 1 - border_width,
                               border_width, border_width + h - 1, 
                               "TITLEBACKIMAGE", bgimage, active);
      else
        iupGLDrawBox(ih, border_width, ih->currentwidth-1 - border_width,
                         border_width, border_width + h - 1, 
                         bordercolor, 1);  /* always active */
    }
    else
    {
      /* draw frame border */
      iupGLDrawFrameRect(ih, 0, ih->currentwidth - 1, 0, ih->currentheight - 1, bwidth, bordercolor, 1, off, w, h);
    }

    iupGLIconDraw(ih, off, 0, w, h, "IMAGE", image, title, fgcolor, active);
  }
  else
  {
    /* draw background */
    char* bgcolor = iupAttribGetStr(ih, "BACKCOLOR");
    char* bgimage = iupAttribGet(ih, "BACKIMAGE");

    if (bgimage)
      iupGLDrawImageZoom(ih, border_width, ih->currentwidth - 1 - border_width,
                             border_width, ih->currentheight - 1 - border_width, 
                             "BACKIMAGE", bgimage, active);
    else
      iupGLDrawBox(ih, border_width, ih->currentwidth - 1 - border_width,
                       border_width, ih->currentheight - 1 - border_width, 
                       bgcolor, 1);  /* always active */

    /* draw border - after background because of the round rect */
    iupGLDrawRect(ih, 0, ih->currentwidth - 1, 0, ih->currentheight - 1, bwidth, bordercolor, 1, 1);
  }

  return IUP_DEFAULT;
}

static int iGLFrameBUTTON_CB(Ihandle* ih, int button, int pressed, int x, int y, char* status)
{
  /* Only called when MOVEABLE=Yes */

  if (button == IUP_BUTTON1 && pressed)
    iupGLSubCanvasStartMoving(ih, x, y);

  (void)status;
  return IUP_DEFAULT;
}

static int iGLFrameMOTION_CB(Ihandle* ih, int x, int y, char* status)
{
  int pressed = iupAttribGetInt(ih, "PRESSED");

  /* Only called when MOVEABLE=Yes */

  if (pressed)
    iupGLSubCanvasMove(ih, x, y);

  (void)status;
  return IUP_DEFAULT;
}

static void iGLFrameGetDecorOffset(Ihandle* ih, int *dx, int *dy)
{
  char* image = iupAttribGet(ih, "IMAGE");
  char* title = iupAttribGet(ih, "TITLE");
  float bwidth = iupAttribGetFloat(ih, "FRAMEWIDTH");
  int border_width = (int)ceil(bwidth);
  *dx = border_width;
  *dy = border_width;

  if (image || title)
  {
    int w = 0, h = 0;
    iupGLIconGetSize(ih, image, title, &w, &h);

    (*dy) += h;
  }
}

static void iGLFrameGetDecorSize(Ihandle* ih, int *width, int *height, int* title_width)
{
  char* image = iupAttribGet(ih, "IMAGE");
  char* title = iupAttribGet(ih, "TITLE");
  float bwidth = iupAttribGetFloat(ih, "FRAMEWIDTH");
  int border_width = (int)ceil(bwidth);

  *width = 2 * border_width;
  *height = 2 * border_width;
  if (title_width)
    *title_width = 0;

  if (image || title)
  {
    int w = 0,
        h = 0;
    iupGLIconGetSize(ih, image, title, &w, &h);

    (*height) += h;
    if (title_width)
      *title_width = w;
  }
}

static char* iGLFrameGetClientSizeAttrib(Ihandle* ih)
{
  int width, height, decorwidth, decorheight;
  width = ih->currentwidth;
  height = ih->currentheight;
  iGLFrameGetDecorSize(ih, &decorwidth, &decorheight, NULL);
  width -= decorwidth;
  height -= decorheight;
  if (width < 0) width = 0;
  if (height < 0) height = 0;
  return iupStrReturnIntInt(width, height, 'x');
}

static char* iGLFrameGetClipMinAttrib(Ihandle* ih)
{
  int x, y;
  iGLFrameGetDecorOffset(ih, &x, &y);
  return iupStrReturnIntInt(x, y, 'x');
}

static int iGLFrameSetMoveableAttrib(Ihandle* ih, const char* value)
{
  Ihandle* gl_parent = (Ihandle*)iupAttribGet(ih, "GL_CANVAS");
  /* only a direct child of the canvasbox can be moved */
  if (iupStrBoolean(value) && ih->parent == gl_parent)
  {
    IupSetCallback(ih, "GL_BUTTON_CB", (Icallback)iGLFrameBUTTON_CB);
    IupSetCallback(ih, "GL_MOTION_CB", (Icallback)iGLFrameMOTION_CB);
    return 1;
  }
  else
  {
    IupSetCallback(ih, "GL_BUTTON_CB", NULL);
    IupSetCallback(ih, "GL_MOTION_CB", NULL);
    iupAttribSet(ih, "MOVEABLE", "NO");
    return 0;
  }
}

static void iGLFrameComputeNaturalSizeMethod(Ihandle* ih, int *w, int *h, int *children_expand)
{
  int decorwidth, decorheight, title_width;
  Ihandle* child = ih->firstchild;

  iGLFrameGetDecorSize(ih, &decorwidth, &decorheight, &title_width);
  *w = decorwidth;  
  *h = decorheight;

  if (child)
  {
    /* update child natural size first */
    iupBaseComputeNaturalSize(child);

    *children_expand = child->expand;
    *w += iupMAX(child->naturalwidth, title_width);  /* make room for title always */
    *h += child->naturalheight;
  }
  else
    *w += title_width;
}

static void iGLFrameSetChildrenCurrentSizeMethod(Ihandle* ih, int shrink)
{
  if (ih->firstchild)
  {
    int width, height, decorwidth, decorheight;

    iGLFrameGetDecorSize(ih, &decorwidth, &decorheight, NULL);

    width = ih->currentwidth - decorwidth;
    height = ih->currentheight - decorheight;
    if (width < 0) width = 0;
    if (height < 0) height = 0;

    iupBaseSetCurrentSize(ih->firstchild, width, height, shrink);
  }
}

static void iGLFrameSetChildrenPositionMethod(Ihandle* ih, int x, int y)
{
  if (ih->firstchild)
  {
    int dx, dy;
    iGLFrameGetDecorOffset(ih, &dx, &dy);

    iupBaseSetPosition(ih->firstchild, x + dx, y + dy);
  }
}

static int iGLFrameCreateMethod(Ihandle* ih, void** params)
{
  if (params)
  {
    Ihandle** iparams = (Ihandle**)params;
    if (*iparams)
      IupAppend(ih, *iparams);
  }

  IupSetCallback(ih, "GL_ACTION", iGLFrameACTION);

  return IUP_NOERROR;
}


/******************************************************************************/


Iclass* iupGLFrameNewClass(void)
{
  Iclass* ic = iupClassNew(iupRegisterFindClass("glsubcanvas"));

  ic->name = "glframe";
  ic->format = "h"; /* one Ihandle* */
  ic->nativetype = IUP_TYPEVOID;
  ic->childtype = IUP_CHILDMANY+1;   /* one child */
  ic->is_interactive = 0;

  /* Class functions */
  ic->New = iupGLFrameNewClass;
  ic->Create = iGLFrameCreateMethod;

  ic->ComputeNaturalSize = iGLFrameComputeNaturalSizeMethod;
  ic->SetChildrenCurrentSize = iGLFrameSetChildrenCurrentSizeMethod;
  ic->SetChildrenPosition = iGLFrameSetChildrenPositionMethod;

  iupClassRegisterCallback(ic, "MOVE_CB", "ii");

  /* Base Container */                                  
  iupClassRegisterAttribute(ic, "EXPAND", iupBaseContainerGetExpandAttrib, NULL, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NOT_MAPPED | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "CLIENTSIZE", iGLFrameGetClientSizeAttrib, NULL, NULL, NULL, IUPAF_READONLY | IUPAF_NOT_MAPPED | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "CLIENTOFFSET", iupBaseGetClientOffsetAttrib, NULL, NULL, NULL, IUPAF_READONLY | IUPAF_NOT_MAPPED | IUPAF_NO_INHERIT);

  /* Visual */
  /* NOTICE: avoid defining inheritable attributes for containers */

  iupGLIconRegisterAttrib(ic);

  iupClassRegisterAttribute(ic, "CLIP_MIN", iGLFrameGetClipMinAttrib, NULL, NULL, NULL, IUPAF_READONLY | IUPAF_NOT_MAPPED | IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "IMAGE", NULL, NULL, NULL, NULL, IUPAF_IHANDLENAME | IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "IMAGEPRESS", NULL, NULL, NULL, NULL, IUPAF_IHANDLENAME | IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "IMAGEHIGHLIGHT", NULL, NULL, NULL, NULL, IUPAF_IHANDLENAME | IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "IMAGEINACTIVE", NULL, NULL, NULL, NULL, IUPAF_IHANDLENAME | IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "FORECOLOR", NULL, NULL, IUPAF_SAMEASSYSTEM, "0 0 0", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "TITLE", NULL, NULL, NULL, NULL, IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "TITLEOFFSET", NULL, NULL, IUPAF_SAMEASSYSTEM, "5", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "TITLEBOX", NULL, NULL, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "TITLEBACKIMAGE", NULL, NULL, NULL, NULL, IUPAF_IHANDLENAME | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "TITLEBACKIMAGEINACTIVE", NULL, NULL, NULL, NULL, IUPAF_IHANDLENAME | IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "FRAMECOLOR", NULL, NULL, IUPAF_SAMEASSYSTEM, "50 150 255", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "FRAMEWIDTH", NULL, NULL, IUPAF_SAMEASSYSTEM, "1", IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "BACKIMAGE", NULL, NULL, NULL, NULL, IUPAF_IHANDLENAME | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "BACKIMAGEINACTIVE", NULL, NULL, NULL, NULL, IUPAF_IHANDLENAME | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "BACKCOLOR", NULL, NULL, NULL, NULL, IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "MOVEABLE", NULL, iGLFrameSetMoveableAttrib, NULL, NULL, IUPAF_NOT_MAPPED | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "MOVETOTOP", NULL, NULL, NULL, NULL, IUPAF_NOT_MAPPED | IUPAF_NO_INHERIT);

  /* replace default value */
  iupClassRegisterAttribute(ic, "PADDING", NULL, NULL, IUPAF_SAMEASSYSTEM, "2x0", IUPAF_NO_INHERIT);

  return ic;
}

Ihandle* IupGLFrame(Ihandle* child)
{
  void *children[2];
  children[0] = (void*)child;
  children[1] = NULL;
  return IupCreatev("glframe", children);
}
