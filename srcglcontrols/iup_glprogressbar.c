/** \file
 * \brief GLProgressBar control
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


typedef struct _iGLProgressBar
{
  int show_text;
  int horiz_padding, vert_padding;  /* internal margin */

  double value;  /* min<=value<max */
  double vmin;
  double vmax;
} iGLProgressBar;

static void iGLProgressBarDrawText(Ihandle* ih, double percent, 
                                   int x, int y, int width, int height,
                                   int active)
{
  int txt_x, txt_y, txt_width, txt_height;
  char* text = iupAttribGetStr(ih, "TEXT");
  char* txtcolor = iupAttribGetStr(ih, "TXTCOLOR");
  char buffer[50];

  if (text == NULL)
  {
    sprintf(buffer, "%.1f%%", 100 * percent);
    text = buffer;
  }

  iupGLFontGetMultiLineStringSize(ih, text, &txt_width, &txt_height);

  /* centered */
  txt_x = (width - (txt_width)) / 2;
  txt_y = (height - (txt_height)) / 2;

  iupGLDrawMultilineText(ih, x + txt_x, y + txt_y, text, txtcolor, active);
}

static int iGLProgressBarACTION_CB(Ihandle* ih)
{
  iGLProgressBar* pb = (iGLProgressBar*)iupAttribGet(ih, "_IUP_GLPROGRESSBAR");
  float bwidth = iupAttribGetFloat(ih, "BORDERWIDTH");
  char* bordercolor = iupAttribGetStr(ih, "BORDERCOLOR");
  int active = iupAttribGetInt(ih, "ACTIVE");
  char* bgcolor = iupAttribGetStr(ih, "BGCOLOR");
  int border_width = (int)ceil(bwidth);
  char* bgimage = iupAttribGet(ih, "BACKIMAGE");

  /* draw background */
  if (bgimage)
    iupGLDrawIconImageZoom(ih, border_width, ih->currentwidth - 1 - border_width,
                           border_width, ih->currentheight - 1 - border_width,
                           "BACKIMAGE", bgimage, active);
  else
  {
    iupGLDrawBox(ih, border_width, ih->currentwidth - 1 - border_width,
                     border_width, ih->currentheight - 1 - border_width, 
                     bgcolor, 1);  /* always active */

    /* draw border - can be disabled setting bwidth=0
       after the background because of the round rect */
    iupGLDrawRect(ih, 0, ih->currentwidth - 1, 0, ih->currentheight - 1, bwidth, bordercolor, active, 1);
  }

  if (pb->show_text || pb->value != pb->vmin)
  {
    int xmin = pb->horiz_padding + border_width;
    int ymin = pb->vert_padding + border_width;
    int xmax = ih->currentwidth - 1 - (pb->horiz_padding + border_width);
    int ymax = ih->currentheight - 1 - (pb->vert_padding + border_width);
    double percent = (pb->value - pb->vmin) / (pb->vmax - pb->vmin);
    int is_horizontal = iupStrEqualNoCase(iupAttribGetStr(ih, "ORIENTATION"), "HORIZONTAL");

    if (pb->value != pb->vmin)
    {
      char* fgcolor = iupAttribGetStr(ih, "FGCOLOR");
      if (is_horizontal)
      {
        int xmid = xmin + iupRound((xmax - xmin + 1) * percent);
        iupGLDrawBox(ih, xmin, xmid, ymin, ymax, fgcolor, 1);
      }
      else
      {
        int ymid = ymin + iupRound((ymax - ymin + 1) * (1.0 - percent));
        iupGLDrawBox(ih, xmin, xmax, ymid, ymax, fgcolor, 1);
      }
    }

    if (pb->show_text && is_horizontal)
      iGLProgressBarDrawText(ih, percent, xmin, ymin, xmax - xmin + 1, ymax - ymin + 1, active);
  }

  return IUP_DEFAULT;
}

static void iGLProgressBarCropValue(iGLProgressBar* pb)
{
  if(pb->value>pb->vmax)
    pb->value = pb->vmax;
  else if(pb->value<pb->vmin)
    pb->value = pb->vmin;
}

static int iGLProgressBarSetValueAttrib(Ihandle* ih, const char* value)
{
  iGLProgressBar* pb = (iGLProgressBar*)iupAttribGet(ih, "_IUP_GLPROGRESSBAR");
  if (iupStrToDouble(value, &(pb->value)))
    iGLProgressBarCropValue(pb);
  return 0; /* do not store value in hash table */
}

static char* iGLProgressBarGetValueAttrib(Ihandle* ih)
{
  iGLProgressBar* pb = (iGLProgressBar*)iupAttribGet(ih, "_IUP_GLPROGRESSBAR");
  return iupStrReturnDouble(pb->value);
}

static int iGLProgressBarSetMinAttrib(Ihandle* ih, const char* value)
{
  iGLProgressBar* pb = (iGLProgressBar*)iupAttribGet(ih, "_IUP_GLPROGRESSBAR");
  if (iupStrToDouble(value, &(pb->vmin)))
    iGLProgressBarCropValue(pb);
  return 0; /* do not store value in hash table */
}

static char* iGLProgressBarGetMinAttrib(Ihandle* ih)
{
  iGLProgressBar* pb = (iGLProgressBar*)iupAttribGet(ih, "_IUP_GLPROGRESSBAR");
  return iupStrReturnDouble(pb->vmin);
}

static int iGLProgressBarSetMaxAttrib(Ihandle* ih, const char* value)
{
  iGLProgressBar* pb = (iGLProgressBar*)iupAttribGet(ih, "_IUP_GLPROGRESSBAR");
  if (iupStrToDouble(value, &(pb->vmax)))
    iGLProgressBarCropValue(pb);
  return 0; /* do not store value in hash table */
}

static char* iGLProgressBarGetMaxAttrib(Ihandle* ih)
{
  iGLProgressBar* pb = (iGLProgressBar*)iupAttribGet(ih, "_IUP_GLPROGRESSBAR");
  return iupStrReturnDouble(pb->vmax);
}

static int iGLProgressBarSetShowTextAttrib(Ihandle* ih, const char* value)
{
  iGLProgressBar* pb = (iGLProgressBar*)iupAttribGet(ih, "_IUP_GLPROGRESSBAR");
  pb->show_text = iupStrBoolean(value);
  return 0; /* do not store value in hash table */
}

static char* iGLProgressBarGetShowTextAttrib(Ihandle* ih)
{
  iGLProgressBar* pb = (iGLProgressBar*)iupAttribGet(ih, "_IUP_GLPROGRESSBAR");
  return iupStrReturnBoolean(pb->show_text);
}

static int iGLProgressBarSetPaddingAttrib(Ihandle* ih, const char* value)
{
  iGLProgressBar* pb = (iGLProgressBar*)iupAttribGet(ih, "_IUP_GLPROGRESSBAR");
  iupStrToIntInt(value, &pb->horiz_padding, &pb->vert_padding, 'x');
  return 0;
}

static char* iGLProgressBarGetPaddingAttrib(Ihandle* ih)
{
  iGLProgressBar* pb = (iGLProgressBar*)iupAttribGet(ih, "_IUP_GLPROGRESSBAR");
  return iupStrReturnIntInt(pb->horiz_padding, pb->vert_padding, 'x');
}

static void iGLProgressBarComputeNaturalSizeMethod(Ihandle* ih, int *w, int *h, int *children_expand)
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
    iGLProgressBar* pb = (iGLProgressBar*)iupAttribGet(ih, "_IUP_GLPROGRESSBAR");
    int charwidth, charheight;
    float bwidth = iupAttribGetFloat(ih, "BORDERWIDTH");
    int border_width = (int)ceil(bwidth);
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

    natural_w += 2 * (pb->horiz_padding + border_width);
    natural_h += 2 * (pb->vert_padding + border_width);
  }

  *w = natural_w;
  *h = natural_h;

  (void)children_expand; /* unset if not a container */
}

static int iGLProgressBarCreateMethod(Ihandle* ih, void **params)
{
  iGLProgressBar* pb = (iGLProgressBar*)malloc(sizeof(iGLProgressBar));
  iupAttribSet(ih, "_IUP_GLPROGRESSBAR", (char*)pb);

  memset(pb, 0, sizeof(iGLProgressBar));

  /* default values */
  pb->vmax = 1;
  pb->show_text = 1;
  pb->horiz_padding = 1;
  pb->vert_padding = 1;

  IupSetCallback(ih, "GL_ACTION", (Icallback)iGLProgressBarACTION_CB);

  (void)params;
  return IUP_NOERROR;
}

static void iGLProgressBarDestroyMethod(Ihandle* ih)
{
  iGLProgressBar* pb = (iGLProgressBar*)iupAttribGet(ih, "_IUP_GLPROGRESSBAR");
  free(pb);
}

Iclass* iupGLProgressBarNewClass(void)
{
  Iclass* ic = iupClassNew(iupRegisterFindClass("glsubcanvas"));

  ic->name = "glprogressbar";
  ic->format = NULL; /* no parameters */
  ic->nativetype = IUP_TYPEVOID;
  ic->childtype   = IUP_CHILDNONE;
  ic->is_interactive = 0;

  /* Class functions */
  ic->New = iupGLProgressBarNewClass;
  ic->Create  = iGLProgressBarCreateMethod;
  ic->Destroy = iGLProgressBarDestroyMethod;
  ic->ComputeNaturalSize = iGLProgressBarComputeNaturalSizeMethod;

  /* IupGLProgressBar only */
  iupClassRegisterAttribute(ic, "MIN", iGLProgressBarGetMinAttrib, iGLProgressBarSetMinAttrib, IUPAF_SAMEASSYSTEM, "0", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "MAX", iGLProgressBarGetMaxAttrib, iGLProgressBarSetMaxAttrib, IUPAF_SAMEASSYSTEM, "1", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "VALUE", iGLProgressBarGetValueAttrib, iGLProgressBarSetValueAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "ORIENTATION", NULL, NULL, IUPAF_SAMEASSYSTEM, "HORIZONTAL", IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "PADDING", iGLProgressBarGetPaddingAttrib, iGLProgressBarSetPaddingAttrib, IUPAF_SAMEASSYSTEM, "1x1", IUPAF_NOT_MAPPED | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "TEXT", NULL, NULL, NULL, NULL, IUPAF_NO_INHERIT);
  /*OLD*/iupClassRegisterAttribute(ic, "SHOW_TEXT", iGLProgressBarGetShowTextAttrib, iGLProgressBarSetShowTextAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NOT_MAPPED | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SHOWTEXT", iGLProgressBarGetShowTextAttrib, iGLProgressBarSetShowTextAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NOT_MAPPED | IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "FGCOLOR", NULL, NULL, IUPAF_SAMEASSYSTEM, "200 225 245", IUPAF_DEFAULT);  /* inheritable */
  iupClassRegisterAttribute(ic, "TXTCOLOR", NULL, NULL, "0 0 0", NULL, IUPAF_DEFAULT);  /* inheritable */

  iupClassRegisterAttribute(ic, "BACKIMAGE", NULL, NULL, NULL, NULL, IUPAF_IHANDLENAME | IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "BACKIMAGEPRESS", NULL, NULL, NULL, NULL, IUPAF_IHANDLENAME | IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "BACKIMAGEHIGHLIGHT", NULL, NULL, NULL, NULL, IUPAF_IHANDLENAME | IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "BACKIMAGEINACTIVE", NULL, NULL, NULL, NULL, IUPAF_IHANDLENAME | IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "FITTOBACKIMAGE", NULL, NULL, NULL, NULL, IUPAF_NO_INHERIT);

  return ic;
}

Ihandle* IupGLProgressBar(void)
{
  return IupCreate("glprogressbar");
}
