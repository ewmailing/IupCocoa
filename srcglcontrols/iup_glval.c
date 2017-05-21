/** \file
 * \brief GLVal control
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
#include "iup_attrib.h"
#include "iup_str.h"
#include "iup_register.h"

#include "iup_glcontrols.h"
#include "iup_glfont.h"
#include "iup_gldraw.h"
#include "iup_glimage.h"
#include "iup_glsubcanvas.h"


typedef struct _iGLVal
{
  double value;  /* min<=value<max */
  double vmin;
  double vmax;
} iGLVal;


static void iGLValGetHandlerSize(Ihandle* ih, int is_horizontal, int *width, int *height)
{
  char *image = iupAttribGet(ih, "IMAGE");
  if (image)
    iupGLImageGetInfo(image, width, height, NULL);
  else
  {
    int handler_size = iupAttribGetInt(ih, "HANDLERSIZE");

    if (is_horizontal)
    {
      if (handler_size == 0)
        handler_size = ih->currentheight / 2;

      *width = handler_size;
      *height = ih->currentheight;
    }
    else
    {
      if (handler_size == 0)
        handler_size = ih->currentwidth / 2;

      *width = ih->currentwidth;
      *height = handler_size;
    }
  }
}

static int iGLValACTION_CB(Ihandle* ih)
{
  iGLVal* val = (iGLVal*)iupAttribGet(ih, "_IUP_GLVAL");
  float bwidth = iupAttribGetFloat(ih, "BORDERWIDTH");
  char* bordercolor = iupAttribGetStr(ih, "BORDERCOLOR");
  int active = iupAttribGetInt(ih, "ACTIVE");
  char* bgcolor = iupAttribGetStr(ih, "BGCOLOR");
  int slider_size = iupAttribGetInt(ih, "SLIDERSIZE");
  int border_width = (int)ceil(bwidth);
  int is_horizontal = iupStrEqualNoCase(iupAttribGetStr(ih, "ORIENTATION"), "HORIZONTAL");
  int handler_width, handler_height;
  char *image = iupAttribGet(ih, "IMAGE");
  char* bgimage = iupAttribGet(ih, "BACKIMAGE");
  double percent = (val->value - val->vmin) / (val->vmax - val->vmin);
  int x1, y1, x2, y2;

  iGLValGetHandlerSize(ih, is_horizontal, &handler_width, &handler_height);

  if (is_horizontal)
  {
    x1 = handler_width / 2;
    x2 = ih->currentwidth - 1 - handler_width / 2;
    y1 = (ih->currentheight - slider_size) / 2;
    y2 = y1 + slider_size;
  }
  else
  {
    y1 = handler_height / 2;
    y2 = ih->currentheight - 1 - handler_height / 2;
    x1 = (ih->currentwidth - slider_size) / 2;
    x2 = x1 + slider_size;
  }

  /* draw slider background */
  if (bgimage)
    iupGLDrawIconImageZoom(ih, border_width, border_width,
                           border_width, border_width,
                           "BACKIMAGE", bgimage, active);
  else
  {
    iupGLDrawBox(ih, x1 + border_width, x2 - border_width,
                     y1 + border_width, y2 - border_width, 
                     bgcolor, 1);  /* always active */

    /* draw slider border - can be disabled setting bwidth=0 */
    iupGLDrawRect(ih, x1, x2, y1, y2, bwidth, bordercolor, active, 0);
  }

  if (is_horizontal)
  {
    int xmid = x1 + iupRound((x2 - x1) * percent);

    x1 = xmid - handler_width / 2;  if (x1 < 0) x1 = 0;
    x2 = xmid + handler_width / 2;  if (x2 > ih->currentwidth - 1) x2 = ih->currentwidth - 1;
    y1 = 0;
    y2 = ih->currentheight - 1;
  }
  else
  {
    int ymid = y1 + iupRound((y2 - y1) * (1.0 - percent));

    y1 = ymid - handler_height / 2;  if (y1 < 0) y1 = 0;
    y2 = ymid + handler_height / 2;  if (y2 > ih->currentheight - 1) y2 = ih->currentheight - 1;
    x1 = 0;
    x2 = ih->currentwidth - 1;
  }

  if (image)
  {
    int x, y, width, height;
    iupGLImageGetInfo(image, &width, &height, NULL);

    /* always center the image */
    x = (x2 - x1 + 1 - width) / 2;
    y = (y2 - y1 + 1 - height) / 2;

    if (x1 + x < 0) x = 0;
    if (y1 + y < 0) y = 0;
    if (x1 + x + width > ih->currentwidth - 1) x = ih->currentwidth - 1 - width - x1;
    if (y1 + y + height > ih->currentheight - 1) y = ih->currentheight - 1 - height - y1;

    iupGLDrawIconImage(ih, x1 + x, y1 + y, "IMAGE", image, active);
  }
  else
  {
    int pressed = iupAttribGetInt(ih, "PRESSED");
    int highlight = iupAttribGetInt(ih, "HIGHLIGHT");
    char* fgcolor = iupAttribGetStr(ih, "FGCOLOR");

    if (pressed)
    {
      char* presscolor = iupAttribGetStr(ih, "PSCOLOR");
      if (presscolor)
        fgcolor = presscolor;
    }
    else if (highlight)
    {
      char* hlcolor = iupAttribGetStr(ih, "HLCOLOR");
      if (hlcolor)
        fgcolor = hlcolor;
    }

    /* draw handler foreground */
    iupGLDrawBox(ih, x1 + border_width, x2 - border_width, y1 + border_width, y2 - border_width, fgcolor, active);

    /* draw handler border - can still be disabled setting bwidth=0
       after the background because of the round rect */
    iupGLDrawRect(ih, x1, x2, y1, y2, bwidth, bordercolor, active, 1);
  }

  return IUP_DEFAULT;
}

static void iGLValCropValue(iGLVal* val)
{
  if (val->value>val->vmax)
    val->value = val->vmax;
  else if (val->value<val->vmin)
    val->value = val->vmin;
}

static int iGLValGetSliderInfo(Ihandle* ih, int dx, int dy, int is_horizontal, int *p, int *p1, int *p2, int *handler_op_size)
{
  int handler_width, handler_height;
  iGLValGetHandlerSize(ih, is_horizontal, &handler_width, &handler_height);

  if (is_horizontal)
  {
    *p1 = handler_width / 2;
    *p2 = ih->currentwidth - 1 - handler_width / 2;
    *p = dx;
    if (handler_op_size) *handler_op_size = handler_height;
    return handler_width;
  }
  else
  {
    *p1 = handler_height / 2;
    *p2 = ih->currentheight - 1 - handler_height / 2;
    *p = dy;
    if (handler_op_size) *handler_op_size = handler_width;
    return handler_height;
  }
}

static void iGLValGetSliderOpositeInfo(Ihandle* ih, int dx, int dy, int is_horizontal, int *q, int *op_size)
{
  if (is_horizontal)
  {
    *op_size = ih->currentheight;
    *q = dy;
  }
  else
  {
    *op_size = ih->currentwidth;
    *q = dx;
  }
}

static int iGLValMoveHandler(Ihandle* ih, int dx, int dy)
{
  iGLVal* val = (iGLVal*)iupAttribGet(ih, "_IUP_GLVAL");
  int is_horizontal = iupStrEqualNoCase(iupAttribGetStr(ih, "ORIENTATION"), "HORIZONTAL");
  double percent, old_percent, delta;
  int dp, p1, p2;

  iGLValGetSliderInfo(ih, dx, dy, is_horizontal, &dp, &p1, &p2, NULL);

  if (dp == 0)
    return 0;

  old_percent = percent = (val->value - val->vmin) / (val->vmax - val->vmin);

  delta = (double)dp / (double)(p2 - p1);
  if (!is_horizontal) delta *= -1.0;

  percent += delta;
  if (percent < 0.0) percent = 0;
  if (percent > 1.0) percent = 1.0;

  if (old_percent == percent)
    return 0;

  val->value = percent * (val->vmax - val->vmin) + val->vmin;
  return 1;
}

static int iGLValIsInsideHandler(Ihandle* ih, int x, int y)
{
  iGLVal* val = (iGLVal*)iupAttribGet(ih, "_IUP_GLVAL");
  int is_horizontal = iupStrEqualNoCase(iupAttribGetStr(ih, "ORIENTATION"), "HORIZONTAL");
  double percent;
  int p, p1, p2, pmid, handler_op_size;

  int handler_size = iGLValGetSliderInfo(ih, x, y, is_horizontal, &p, &p1, &p2, &handler_op_size);

  percent = (val->value - val->vmin) / (val->vmax - val->vmin);
  if (!is_horizontal)
    percent = 1.0 - percent;
  pmid = p1 + iupRound((p2 - p1) * percent);

  p1 = pmid - handler_size / 2;
  p2 = pmid + handler_size / 2;

  if (p >= p1 && p <= p2)
  {
    int q, q1, q2, qmid, op_size;

    iGLValGetSliderOpositeInfo(ih, x, y, is_horizontal, &q, &op_size);

    qmid = op_size / 2;
    q1 = qmid - handler_op_size / 2;
    q2 = qmid + handler_op_size / 2;

    if (q >= q1 && q <= q2)
      return 1;
    else
      return 0;
  }
  else
    return 0;
}

static int iGLValBUTTON_CB(Ihandle* ih, int button, int pressed, int x, int y, char* status)
{
  if (button == IUP_BUTTON1)
  {
    if (pressed)
    {
      if (!iGLValIsInsideHandler(ih, x, y))
        iupAttribSet(ih, "PRESSED", NULL);
      else
      {
        iupAttribSetInt(ih, "_IUP_START_X", x);
        iupAttribSetInt(ih, "_IUP_START_Y", y);
      }
    }
    else
    {
      if (iupAttribGet(ih, "_IUP_DRAG"))
      {
        IFni cb = (IFni)IupGetCallback(ih, "VALUECHANGING_CB");
        if (cb) cb(ih, 0);

        iupAttribSet(ih, "_IUP_DRAG", NULL);
      }
    }

    iupGLSubCanvasRedraw(ih);
  }

  (void)status;
  return IUP_DEFAULT;
}

static int iGLValMOTION_CB(Ihandle* ih, int x, int y, char* status)
{
  int redraw = 0;
  int pressed = iupAttribGetInt(ih, "PRESSED");

  /* special highlight processing for handler area */
  if (iGLValIsInsideHandler(ih, x, y))
  {
    if (!iupAttribGet(ih, "HIGHLIGHT"))
    {
      redraw = 1;
      iupAttribSet(ih, "HIGHLIGHT", "1");
    }
  }
  else
  {
    if (iupAttribGet(ih, "HIGHLIGHT"))
    {
      redraw = 1;
      iupAttribSet(ih, "HIGHLIGHT", NULL);
    }
  }

  if (pressed)
  {
    int start_x = iupAttribGetInt(ih, "_IUP_START_X");
    int start_y = iupAttribGetInt(ih, "_IUP_START_Y");

    if (iGLValMoveHandler(ih, x - start_x, y - start_y))
    {
      iupGLSubCanvasRedraw(ih);
      redraw = 0;

      if (!iupAttribGet(ih, "_IUP_DRAG"))
      {
        IFni cb = (IFni)IupGetCallback(ih, "VALUECHANGING_CB");
        if (cb) cb(ih, 1);
      }

      iupBaseCallValueChangedCb(ih);

      iupAttribSet(ih, "_IUP_DRAG", "1");
    }

    iupAttribSetInt(ih, "_IUP_START_X", x);
    iupAttribSetInt(ih, "_IUP_START_Y", y);
  }

  if (redraw)
    iupGLSubCanvasRedraw(ih);

  (void)status;
  return IUP_DEFAULT;
}

static int iGLValENTERWINDOW_CB(Ihandle* ih, int x, int y)
{
  /* special highlight processing for handler area */
  if (iGLValIsInsideHandler(ih, x, y))
    iupAttribSet(ih, "HIGHLIGHT", "1");
  else
    iupAttribSet(ih, "HIGHLIGHT", NULL);

  return iupGLSubCanvasRedraw(ih);
}

static int iGLValSetValueAttrib(Ihandle* ih, const char* value)
{
  iGLVal* val = (iGLVal*)iupAttribGet(ih, "_IUP_GLVAL");
  if (iupStrToDouble(value, &(val->value)))
    iGLValCropValue(val);
  return 0; /* do not store value in hash table */
}

static char* iGLValGetValueAttrib(Ihandle* ih)
{
  iGLVal* val = (iGLVal*)iupAttribGet(ih, "_IUP_GLVAL");
  return iupStrReturnDouble(val->value);
}

static int iGLValSetMinAttrib(Ihandle* ih, const char* value)
{
  iGLVal* val = (iGLVal*)iupAttribGet(ih, "_IUP_GLVAL");
  if (iupStrToDouble(value, &(val->vmin)))
    iGLValCropValue(val);
  return 0; /* do not store value in hash table */
}

static char* iGLValGetMinAttrib(Ihandle* ih)
{
  iGLVal* val = (iGLVal*)iupAttribGet(ih, "_IUP_GLVAL");
  return iupStrReturnDouble(val->vmin);
}

static int iGLValSetMaxAttrib(Ihandle* ih, const char* value)
{
  iGLVal* val = (iGLVal*)iupAttribGet(ih, "_IUP_GLVAL");
  if (iupStrToDouble(value, &(val->vmax)))
    iGLValCropValue(val);
  return 0; /* do not store value in hash table */
}

static char* iGLValGetMaxAttrib(Ihandle* ih)
{
  iGLVal* val = (iGLVal*)iupAttribGet(ih, "_IUP_GLVAL");
  return iupStrReturnDouble(val->vmax);
}

static void iGLValComputeNaturalSizeMethod(Ihandle* ih, int *w, int *h, int *children_expand)
{
  int natural_w = 0,
      natural_h = 0;
  int fit2backimage = iupAttribGetBoolean(ih, "FITTOBACKIMAGE");
  char* bgimage = iupAttribGet(ih, "BACKIMAGE");

  if (fit2backimage && bgimage)
  {
    iupAttribSet(ih, "BORDERWIDTH", "0");
    iupGLImageGetInfo(bgimage, &natural_w, &natural_h, NULL);
  }
  else
  {
    int charwidth, charheight;
    int is_horizontal = iupStrEqualNoCase(iupAttribGetStr(ih, "ORIENTATION"), "HORIZONTAL");

    iupGLFontGetCharSize(ih, &charwidth, &charheight);

    if (is_horizontal)
    {
      natural_h = charheight;
      if (ih->userwidth <= 0)
        natural_w = 15 * charwidth;
    }
    else
    {
      natural_w = charheight;
      if (ih->userheight <= 0)
        natural_h = 15 * charwidth;
    }
  }

  *w = natural_w;
  *h = natural_h;

  (void)children_expand; /* unset if not a container */
}

static int iGLValCreateMethod(Ihandle* ih, void **params)
{
  iGLVal* val = (iGLVal*)malloc(sizeof(iGLVal));
  iupAttribSet(ih, "_IUP_GLVAL", (char*)val);

  memset(val, 0, sizeof(iGLVal));

  /* default values */
  val->vmax = 1;

  IupSetCallback(ih, "GL_ACTION", (Icallback)iGLValACTION_CB);
  IupSetCallback(ih, "GL_BUTTON_CB", (Icallback)iGLValBUTTON_CB);
  IupSetCallback(ih, "GL_MOTION_CB", (Icallback)iGLValMOTION_CB);
  IupSetCallback(ih, "GL_LEAVEWINDOW_CB", iupGLSubCanvasRedraw);
  IupSetCallback(ih, "GL_ENTERWINDOW_CB", (Icallback)iGLValENTERWINDOW_CB);

  (void)params;
  return IUP_NOERROR;
}

static void iGLValDestroyMethod(Ihandle* ih)
{
  iGLVal* val = (iGLVal*)iupAttribGet(ih, "_IUP_GLVAL");
  free(val);
}

Iclass* iupGLValNewClass(void)
{
  Iclass* ic = iupClassNew(iupRegisterFindClass("glsubcanvas"));

  ic->name = "glval";
  ic->format = NULL; /* no parameters */
  ic->nativetype = IUP_TYPEVOID;
  ic->childtype   = IUP_CHILDNONE;
  ic->is_interactive = 0;

  /* Class functions */
  ic->New = iupGLValNewClass;
  ic->Create  = iGLValCreateMethod;
  ic->Destroy = iGLValDestroyMethod;
  ic->ComputeNaturalSize = iGLValComputeNaturalSizeMethod;

  /* Callbacks */
  iupClassRegisterCallback(ic, "VALUECHANGED_CB", "");
  iupClassRegisterCallback(ic, "VALUECHANGING_CB", "i");

  /* IupGLVal only */
  iupClassRegisterAttribute(ic, "MIN", iGLValGetMinAttrib, iGLValSetMinAttrib, IUPAF_SAMEASSYSTEM, "0", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "MAX", iGLValGetMaxAttrib, iGLValSetMaxAttrib, IUPAF_SAMEASSYSTEM, "1", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "VALUE", iGLValGetValueAttrib, iGLValSetValueAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "ORIENTATION", NULL, NULL, IUPAF_SAMEASSYSTEM, "HORIZONTAL", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SLIDERSIZE", NULL, NULL, IUPAF_SAMEASSYSTEM, "5", IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "FGCOLOR", NULL, NULL, IUPAF_SAMEASSYSTEM, "200 225 245", IUPAF_DEFAULT);  /* inheritable */
  iupClassRegisterAttribute(ic, "HLCOLOR", NULL, NULL, IUPAF_SAMEASSYSTEM, "190 210 230", IUPAF_DEFAULT);  /* inheritable */
  iupClassRegisterAttribute(ic, "HANDLERSIZE", NULL, NULL, IUPAF_SAMEASSYSTEM, "0", IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "IMAGE", NULL, NULL, NULL, NULL, IUPAF_IHANDLENAME | IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "IMAGEPRESS", NULL, NULL, NULL, NULL, IUPAF_IHANDLENAME | IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "IMAGEHIGHLIGHT", NULL, NULL, NULL, NULL, IUPAF_IHANDLENAME | IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "IMAGEINACTIVE", NULL, NULL, NULL, NULL, IUPAF_IHANDLENAME | IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "BACKIMAGE", NULL, NULL, NULL, NULL, IUPAF_IHANDLENAME | IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "BACKIMAGEPRESS", NULL, NULL, NULL, NULL, IUPAF_IHANDLENAME | IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "BACKIMAGEHIGHLIGHT", NULL, NULL, NULL, NULL, IUPAF_IHANDLENAME | IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "BACKIMAGEINACTIVE", NULL, NULL, NULL, NULL, IUPAF_IHANDLENAME | IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "FITTOBACKIMAGE", NULL, NULL, NULL, NULL, IUPAF_NO_INHERIT);

  return ic;
}

Ihandle* IupGLVal(void)
{
  return IupCreate("glval");
}
