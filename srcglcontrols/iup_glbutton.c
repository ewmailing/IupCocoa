/** \file
 * \brief GLButton Control.
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
#include "iup_gldraw.h"
#include "iup_glicon.h"
#include "iup_glsubcanvas.h"
#include "iup_glimage.h"


void iupGLButtonDraw(Ihandle* ih)
{
  char *image = iupAttribGet(ih, "IMAGE");
  char* title = iupAttribGet(ih, "TITLE");
  int active = iupAttribGetInt(ih, "ACTIVE");
  int pressed = iupAttribGetInt(ih, "PRESSED");
  int highlight = iupAttribGetInt(ih, "HIGHLIGHT");
  int selected = IupGetInt(ih, "VALUE");
  char* fgcolor = iupAttribGetStr(ih, "FGCOLOR");
  char* bgcolor = iupAttribGetStr(ih, "BGCOLOR");
  float bwidth = iupAttribGetFloat(ih, "BORDERWIDTH");
  char* bgimage = iupAttribGet(ih, "BACKIMAGE");
  char* fgimage = iupAttribGet(ih, "FRONTIMAGE");
  int border_width = (int)ceil(bwidth);
  int draw_border = 0;

  if (pressed || selected)
  {
    char* presscolor = iupAttribGetStr(ih, "PSCOLOR");
    if (presscolor)
      bgcolor = presscolor;
    draw_border = 1;

    if (!pressed && (bgimage || image))
      iupAttribSet(ih, "PRESSED", "1");
  }
  else if (highlight)
  {
    char* hlcolor = iupAttribGetStr(ih, "HLCOLOR");
    if (hlcolor)
      bgcolor = hlcolor;
    draw_border = 1;
  }

  /* draw border - can still be disabled setting bwidth=0 */
  if (draw_border)
  {
    char* bordercolor = iupAttribGetStr(ih, "BORDERCOLOR");
    iupGLDrawRect(ih, 0, ih->currentwidth - 1, 
                      0, ih->currentheight - 1, 
                      bwidth, bordercolor, active, 0);
  }

  /* draw background */
  if (bgimage)
    iupGLDrawIconImageZoom(ih, border_width, ih->currentwidth - 1 - border_width,
                           border_width, ih->currentheight - 1 - border_width, 
                           "BACKIMAGE", bgimage, active);
  else
    iupGLDrawBox(ih, border_width, ih->currentwidth - 1 - border_width,
                     border_width, ih->currentheight - 1 - border_width, 
                     bgcolor, 1);  /* always active */

  iupGLIconDraw(ih, border_width, border_width,
                    ih->currentwidth - 2 * border_width, ih->currentheight - 2 * border_width,
                    "IMAGE", image, title, fgcolor, active);

  if (fgimage)
    iupGLDrawIconImageZoom(ih, border_width, ih->currentwidth - 1 - border_width,
                           border_width, ih->currentheight - 1 - border_width,
                           "FRONTIMAGE", fgimage, active);
  else if (!image && !title)
  {
    int space = border_width + 2;
    iupGLDrawRect(ih, space, ih->currentwidth - 1 - space,
                      space, ih->currentheight - 1 - space,
                      1, "0 0 0", active, 0);
    space++;
    iupGLDrawBox(ih, space, ih->currentwidth - 1 - space,
                     space, ih->currentheight - 1 - space,
                     fgcolor, active);
  }

  if (selected && !pressed && (bgimage || image))
    iupAttribSet(ih, "PRESSED", NULL);
}

static int iGLButtonACTION(Ihandle* ih)
{
  iupGLButtonDraw(ih);
  return IUP_DEFAULT;
}

static int iGLButtonBUTTON_CB(Ihandle* ih, int button, int pressed, int x, int y, char* status)
{
  if (button == IUP_BUTTON1)
  {
    /* "PRESSED" was already updated */
    iupGLSubCanvasRedraw(ih);

    if (!pressed)
    {
      Icallback cb = IupGetCallback(ih, "ACTION");
      if (cb)
      {
        int ret = cb(ih);
        if (ret == IUP_CLOSE)
          IupExitLoop();
      }
    }
  }

  (void)x;
  (void)y;
  (void)status;
  return IUP_DEFAULT;
}

static int iGLButtonCreateMethod(Ihandle* ih, void** params)
{
  IupSetCallback(ih, "GL_ACTION", iGLButtonACTION);
  IupSetCallback(ih, "GL_BUTTON_CB", (Icallback)iGLButtonBUTTON_CB);
  IupSetCallback(ih, "GL_LEAVEWINDOW_CB", iupGLSubCanvasRedraw);
  IupSetCallback(ih, "GL_ENTERWINDOW_CB", iupGLSubCanvasRedraw);

  (void)params; /* label create already parsed title */
  return IUP_NOERROR;
}

static void iGLButtonComputeNaturalSizeMethod(Ihandle* ih, int *w, int *h, int *children_expand)
{
  int fit2backimage = iupAttribGetBoolean(ih, "FITTOBACKIMAGE");
  char* bgimage = iupAttribGet(ih, "BACKIMAGE");

  if (fit2backimage && bgimage)
  {
    iupAttribSet(ih, "BORDERWIDTH", "0");
    iupGLImageGetInfo(bgimage, w, h, NULL);
  }
  else
  {
    float bwidth = iupAttribGetFloat(ih, "BORDERWIDTH");
    int border_width = (int)ceil(bwidth);
    if (border_width == 0)
      return;

    /* add to the label natural size */
    *w += 2 * border_width;
    *h += 2 * border_width;
  }

  (void)children_expand; /* unset if not a container */
}


/******************************************************************************/


Iclass* iupGLButtonNewClass(void)
{
  Iclass* ic = iupClassNew(iupRegisterFindClass("gllabel"));

  ic->name = "glbutton";
  ic->format = "s"; /* one string */
  ic->nativetype = IUP_TYPEVOID;
  ic->childtype = IUP_CHILDNONE;
  ic->is_interactive = 0;

  /* Class functions */
  ic->New = iupGLButtonNewClass;
  ic->Create = iGLButtonCreateMethod;
  ic->ComputeNaturalSize = iGLButtonComputeNaturalSizeMethod;

  iupClassRegisterCallback(ic, "ACTION", "");

  /* disable VALUE inheritance */
  iupClassRegisterAttribute(ic, "VALUE", NULL, NULL, NULL, NULL, IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "BACKIMAGE", NULL, NULL, NULL, NULL, IUPAF_IHANDLENAME | IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "BACKIMAGEPRESS", NULL, NULL, NULL, NULL, IUPAF_IHANDLENAME | IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "BACKIMAGEHIGHLIGHT", NULL, NULL, NULL, NULL, IUPAF_IHANDLENAME | IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "BACKIMAGEINACTIVE", NULL, NULL, NULL, NULL, IUPAF_IHANDLENAME | IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "FITTOBACKIMAGE", NULL, NULL, NULL, NULL, IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "FRONTIMAGE", NULL, NULL, NULL, NULL, IUPAF_IHANDLENAME | IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "FRONTIMAGEPRESS", NULL, NULL, NULL, NULL, IUPAF_IHANDLENAME | IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "FRONTIMAGEHIGHLIGHT", NULL, NULL, NULL, NULL, IUPAF_IHANDLENAME | IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "FRONTIMAGEINACTIVE", NULL, NULL, NULL, NULL, IUPAF_IHANDLENAME | IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);

  return ic;
}

Ihandle* IupGLButton(const char* title)
{
  void *params[2];
  params[0] = (void*)title;
  params[1] = NULL;
  return IupCreatev("glbutton", params);
}

