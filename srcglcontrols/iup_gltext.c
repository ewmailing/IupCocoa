/** \file
 * \brief GLText Control.
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
#include "iup_childtree.h"

#include "iup_glcontrols.h"
#include "iup_gldraw.h"
#include "iup_glicon.h"
#include "iup_glsubcanvas.h"


static int iGLTextACTION(Ihandle* ih)
{
  char* value = IupGetAttribute(ih->firstchild, "VALUE");
  int active = iupAttribGetInt(ih, "ACTIVE");
  int highlight = iupAttribGetInt(ih, "HIGHLIGHT");
  char* fgcolor = iupAttribGetStr(ih, "FGCOLOR");
  char* bgcolor = iupAttribGetStr(ih, "BGCOLOR");
  float bwidth = iupAttribGetFloat(ih, "BORDERWIDTH");
  int border_width = (int)ceil(bwidth);
  char* bordercolor = iupAttribGetStr(ih, "BORDERCOLOR");

  if (highlight)
  {
    char* hlcolor = iupAttribGetStr(ih, "HLCOLOR");
    if (hlcolor)
      bgcolor = hlcolor;
  }

  /* draw border - can still be disabled setting bwidth=0 */
  iupGLDrawRect(ih, 0, ih->currentwidth - 1,
                0, ih->currentheight - 1,
                bwidth, bordercolor, active, 0);

  /* draw background */
  iupGLDrawBox(ih, border_width, ih->currentwidth - 1 - border_width,
               border_width, ih->currentheight - 1 - border_width,
               bgcolor, 1);  /* always active */

  iupGLIconDraw(ih, border_width, border_width,
                ih->currentwidth - 2 * border_width, ih->currentheight - 2 * border_width,
                NULL, NULL, value, fgcolor, active);

  return IUP_DEFAULT;
}

static int iGLTextBUTTON_CB(Ihandle* ih, int button, int pressed, int x, int y, char* status)
{
  if (button == IUP_BUTTON1)
  {
    iupGLSubCanvasRedraw(ih);

    if (!pressed)
    {
      int pos;
      Ihandle* text = ih->firstchild;

      text->x = ih->x;
      text->y = ih->y;

      text->currentwidth = ih->currentwidth;
      text->currentheight = ih->currentheight;

      iupClassObjectLayoutUpdate(text);

      IupSetAttribute(text, "FONT", IupGetAttribute(ih, "FONT"));
      IupSetAttribute(text, "VISIBLE", "YES");
      IupSetAttribute(text, "ACTIVE", "YES");
      IupSetFocus(text);

      pos = IupConvertXYToPos(text, x, y);
      IupSetInt(text, "CARETPOS", pos);
    }
  }

  (void)x;
  (void)y;
  (void)status;
  return IUP_DEFAULT;
}

static int iGLTextEditKILLFOCUS_CB(Ihandle* text)
{
  Ihandle* ih = text->parent;
  Ihandle* gl_parent = (Ihandle*)iupAttribGet(ih, "GL_CANVAS");
  IupSetAttribute(text, "VISIBLE", "NO");
  IupSetAttribute(text, "ACTIVE", "NO");
  IupSetAttribute(gl_parent, "REDRAW", NULL);  /* redraw the whole box */
  return IUP_DEFAULT;
}

static int iGLTextEditKANY_CB(Ihandle* text, int c)
{
  if (c == K_ESC || c == K_CR)
  {
    iGLTextEditKILLFOCUS_CB(text);
    return IUP_IGNORE;  /* always ignore to avoid the defaultenter/defaultesc behavior from here */
  }

  return IUP_CONTINUE;
}

static int iGLTextEditVALUECHANGED_CB(Ihandle* text)
{
  Ihandle* ih = text->parent;
  Icallback cb = IupGetCallback(ih, "VALUECHANGED_CB");
  if (cb)
    cb(ih);
  return IUP_DEFAULT;
}

static int iGLTextSetValueAttrib(Ihandle* ih, const char* value)
{
  IupSetStrAttribute(ih->firstchild, "VALUE", value);
  return 0; /* do not store value in hash table */
}

static char* iGLTextGetValueAttrib(Ihandle* ih)
{
  return IupGetAttribute(ih->firstchild, "VALUE");
}

static char* iGLTextGetTextAttrib(Ihandle* ih)
{
  return IupGetName(ih->firstchild);
}

static char* iGLTextGetTextHandleAttrib(Ihandle* ih)
{
  return (char*)ih->firstchild;
}

static int iGLTextCreateMethod(Ihandle* ih, void** params)
{
  Ihandle* text = IupText(NULL);
  text->currentwidth = 20;  /* just to avoid initial size 0x0 */
  text->currentheight = 10;
  iupChildTreeAppend(ih, text);

  IupSetCallback(text, "VALUECHANGED_CB", (Icallback)iGLTextEditVALUECHANGED_CB);
  IupSetCallback(text, "KILLFOCUS_CB", (Icallback)iGLTextEditKILLFOCUS_CB);
  IupSetCallback(text, "K_ANY", (Icallback)iGLTextEditKANY_CB);
  IupSetAttribute(text, "FLOATING", "IGNORE");
  IupSetAttribute(text, "VISIBLE", "NO");
  IupSetAttribute(text, "ACTIVE", "NO");

  IupSetCallback(ih, "GL_ACTION", iGLTextACTION);
  IupSetCallback(ih, "GL_BUTTON_CB", (Icallback)iGLTextBUTTON_CB);
  IupSetCallback(ih, "GL_LEAVEWINDOW_CB", iupGLSubCanvasRedraw);
  IupSetCallback(ih, "GL_ENTERWINDOW_CB", iupGLSubCanvasRedraw);

  (void)params;
  return IUP_NOERROR;
}

static void iGLTextComputeNaturalSizeMethod(Ihandle* ih, int *w, int *h, int *children_expand)
{
  int natural_w = 0,
    natural_h = 0,
    visiblecolumns = iupAttribGetInt(ih, "VISIBLECOLUMNS");
  float bwidth = iupAttribGetFloat(ih, "BORDERWIDTH");
  int border_width = (int)ceil(bwidth);

  /* Since the contents can be changed by the user, the size can not be dependent on it. */
  iupGLIconGetSize(ih, NULL, "WWWWWWWWWW", &natural_w, &natural_h);  /* one line height */
  natural_w = (visiblecolumns*natural_w) / 10;

  /* compute the borders space */
  *w += 2 * border_width;
  *h += 2 * border_width;

  *w = natural_w;
  *h = natural_h;

  (void)children_expand; /* unset if not a container */
}


/******************************************************************************/


Iclass* iupGLTextNewClass(void)
{
  Iclass* ic = iupClassNew(iupRegisterFindClass("glsubcanvas"));

  ic->name = "gltext";
  ic->format = NULL; /* no parameters */
  ic->nativetype = IUP_TYPEVOID;
  ic->childtype = IUP_CHILDNONE;
  ic->is_interactive = 0;

  /* Class functions */
  ic->New = iupGLTextNewClass;
  ic->Create = iGLTextCreateMethod;
  ic->ComputeNaturalSize = iGLTextComputeNaturalSizeMethod;

  iupClassRegisterCallback(ic, "VALUECHANGED_CB", "");

  iupClassRegisterAttribute(ic, "VALUE", iGLTextGetValueAttrib, iGLTextSetValueAttrib, NULL, NULL, IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "FGCOLOR", NULL, NULL, "0 0 0", NULL, IUPAF_DEFAULT);  /* inheritable */
  iupClassRegisterAttribute(ic, "VISIBLECOLUMNS", NULL, NULL, IUPAF_SAMEASSYSTEM, "5", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "TEXT", iGLTextGetTextAttrib, NULL, NULL, NULL, IUPAF_READONLY | IUPAF_IHANDLENAME | IUPAF_NOT_MAPPED | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "TEXT_HANDLE", iGLTextGetTextHandleAttrib, NULL, NULL, NULL, IUPAF_READONLY | IUPAF_NOT_MAPPED | IUPAF_NO_INHERIT | IUPAF_IHANDLE | IUPAF_NO_STRING);

  /* replace default value */
  iupClassRegisterAttribute(ic, "PADDING", NULL, NULL, IUPAF_SAMEASSYSTEM, "2x2", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "ALIGNMENT", NULL, NULL, IUPAF_SAMEASSYSTEM, "ALEFT:ABOTTOM", IUPAF_NO_INHERIT);

  return ic;
}

Ihandle* IupGLText(void)
{
  return IupCreate("gltext");
}

