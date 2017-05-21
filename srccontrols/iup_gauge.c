/** \file
 * \brief Gauge control
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

#include <cd.h>
#include <cdiup.h>
#include <cddbuf.h>

#include "iup_object.h"
#include "iup_attrib.h"
#include "iup_str.h"
#include "iup_drv.h"
#include "iup_stdcontrols.h"
#include "iup_controls.h"
#include "iup_cdutil.h"
#include "iup_register.h"
#include "iup_image.h"


#define IGAUGE_DEFAULTCOLOR "64 96 192"
#define IGAUGE_DEFAULTSIZE  "120x14"

#define IGAUGE_DASHED_GAP     3
#define IGAUGE_DASHED_BLOCKS 20

#define gaugeround(_) ((int)((_)+.5))


struct _IcontrolData
{
  iupCanvas canvas;  /* from IupCanvas (must reserve it) */

  int w;
  int h;

  int flat;
  int show_text;
  int dashed;
  int horiz_padding, vert_padding;  /* internal margin */

  long bgcolor;
  long fgcolor;
  long light_shadow;
  long mid_shadow;
  long dark_shadow; 
  long flatcolor;

  double value;  /* min<=value<max */
  double vmin;
  double vmax;

  char* text;

  cdCanvas *cd_canvas;
};

static void iGaugeDrawText(Ihandle* ih, int xmid)
{
  int x, y, xmin, xmax, ymin, ymax;
  char* text = ih->data->text;
  char buffer[30];

  IupCdSetFont(ih, ih->data->cd_canvas, IupGetAttribute(ih, "FONT"));
  cdCanvasTextAlignment(ih->data->cd_canvas, CD_CENTER);
  cdCanvasBackOpacity(ih->data->cd_canvas, CD_TRANSPARENT);

  x = (int)(0.5 * ih->data->w);
  y = (int)(0.5 * ih->data->h);

  if (text == NULL)
  {
    sprintf(buffer, "%.1f%%", 100 * (ih->data->value - ih->data->vmin) / (ih->data->vmax - ih->data->vmin));
    text = buffer;
  }

  cdCanvasGetTextBox(ih->data->cd_canvas, x, y, text, &xmin, &xmax, &ymin, &ymax);

  if(xmid < xmin)
  {
    cdCanvasForeground(ih->data->cd_canvas, ih->data->fgcolor);
    cdCanvasText(ih->data->cd_canvas, x, y, text);
  }
  else if(xmid > xmax)
  {
    cdCanvasForeground(ih->data->cd_canvas, ih->data->bgcolor);
    cdCanvasText(ih->data->cd_canvas, x, y, text);
  }
  else
  {
    cdCanvasClip(ih->data->cd_canvas, CD_CLIPAREA);
    cdCanvasClipArea(ih->data->cd_canvas, xmin, xmid, ymin, ymax);
    cdCanvasForeground(ih->data->cd_canvas, ih->data->bgcolor);
    cdCanvasText(ih->data->cd_canvas, x, y, text);

    cdCanvasClipArea(ih->data->cd_canvas, xmid, xmax, ymin, ymax);
    cdCanvasForeground(ih->data->cd_canvas, ih->data->fgcolor);
    cdCanvasText(ih->data->cd_canvas, x, y, text);
    cdCanvasClip(ih->data->cd_canvas, CD_CLIPOFF);
  }
}

static void iGaugeDrawGauge(Ihandle* ih)
{
  int border = ih->data->flat? 1: 3;
  int xstart = ih->data->horiz_padding + border;
  int ystart = ih->data->vert_padding + border;
  int xend = ih->data->w - 1 - (ih->data->horiz_padding + border);
  int yend = ih->data->h - 1 - (ih->data->vert_padding + border);

  cdCanvasBackground(ih->data->cd_canvas, ih->data->bgcolor);
  cdCanvasClear(ih->data->cd_canvas);

  if (ih->data->flat)
  {
    cdCanvasForeground(ih->data->cd_canvas, ih->data->flatcolor);
    cdCanvasRect(ih->data->cd_canvas, 0, ih->data->w - 1, 0, ih->data->h - 1);
  }
  else
    cdIupDrawSunkenRect(ih->data->cd_canvas, 0, 0, ih->data->w-1, ih->data->h-1,
                        ih->data->light_shadow, ih->data->mid_shadow, ih->data->dark_shadow);

  cdCanvasForeground(ih->data->cd_canvas, ih->data->fgcolor);

  if (ih->data->dashed)
  {
    float step = (xend - xstart + 1) / (float)IGAUGE_DASHED_BLOCKS;
    float boxw = step - IGAUGE_DASHED_GAP;
    float vx   = (float)((xend-xstart + 1) * (ih->data->value - ih->data->vmin) / (ih->data->vmax - ih->data->vmin));
    int intvx  = (int)(100 * vx);
    float i = 0;

    if(ih->data->value == ih->data->vmin)
      return;

    while(gaugeround(100*(i + boxw)) <= intvx)
    {
      cdCanvasBox(ih->data->cd_canvas, xstart + gaugeround(i),
                                       xstart + gaugeround(i + boxw) - 1, ystart, yend);
      i += step;
    }
  }
  else
  {
    int xmid = xstart + gaugeround((xend-xstart + 1) * (ih->data->value - ih->data->vmin) / (ih->data->vmax - ih->data->vmin));

    if(ih->data->value != ih->data->vmin)
      cdCanvasBox(ih->data->cd_canvas, xstart, xmid, ystart, yend );

    if(ih->data->show_text)
      iGaugeDrawText(ih, xmid);
  }
}

static int iGaugeResize_CB(Ihandle* ih)
{
  /* update size */
  cdCanvasActivate(ih->data->cd_canvas);
  cdCanvasGetSize(ih->data->cd_canvas,&ih->data->w,&ih->data->h,NULL,NULL);

  /* update render */
  iGaugeDrawGauge(ih);

  return IUP_DEFAULT;
}

static void iGaugeRepaint(Ihandle* ih)
{
  if (!ih->data->cd_canvas)
    return;

  cdCanvasActivate(ih->data->cd_canvas);

  /* update render */
  iGaugeDrawGauge(ih);

  /* update display */
  cdCanvasFlush(ih->data->cd_canvas);
}

static int iGaugeRedraw_CB(Ihandle* ih)
{
  if (!ih->data->cd_canvas)
    return IUP_DEFAULT;

  /* update display */
  cdCanvasFlush(ih->data->cd_canvas);
  return IUP_DEFAULT;
}

static void iGaugeCropValue(Ihandle* ih)
{
  if(ih->data->value>ih->data->vmax)
    ih->data->value = ih->data->vmax;
  else if(ih->data->value<ih->data->vmin)
    ih->data->value = ih->data->vmin;
}

static int iGaugeSetFgColorAttrib(Ihandle* ih, const char* value)
{
  ih->data->fgcolor = cdIupConvertColor(value);
  iGaugeRepaint(ih);
  return 1;
}

static int iGaugeSetBgColorAttrib(Ihandle* ih, const char* value)
{
  if (!value)
    value = iupControlBaseGetParentBgColor(ih);

  ih->data->bgcolor = cdIupConvertColor(value);
  cdIupCalcShadows(ih->data->bgcolor, &ih->data->light_shadow, &ih->data->mid_shadow, &ih->data->dark_shadow);

  iGaugeRepaint(ih);
  return 1;
}

static int iGaugeSetValueAttrib(Ihandle* ih, const char* value)
{
  if(value == NULL)
    ih->data->value = 0;
  else
  {
    if (iupStrToDouble(value, &(ih->data->value)))
      iGaugeCropValue(ih);
  }

  iGaugeRepaint(ih);
  return 0; /* do not store value in hash table */
}

static char* iGaugeGetValueAttrib(Ihandle* ih)
{
  return iupStrReturnDouble(ih->data->value);
}

static int iGaugeSetMinAttrib(Ihandle* ih, const char* value)
{
  if (iupStrToDouble(value, &(ih->data->vmin)))
    iGaugeCropValue(ih);

  iGaugeRepaint(ih);
  return 0; /* do not store value in hash table */
}

static char* iGaugeGetMinAttrib(Ihandle* ih)
{
  return iupStrReturnDouble(ih->data->vmin);
}

static int iGaugeSetMaxAttrib(Ihandle* ih, const char* value)
{
  if (iupStrToDouble(value, &(ih->data->vmax)))
    iGaugeCropValue(ih);

  iGaugeRepaint(ih);
  return 0; /* do not store value in hash table */
}

static char* iGaugeGetMaxAttrib(Ihandle* ih)
{
  return iupStrReturnDouble(ih->data->vmax);
}

static int iGaugeSetShowTextAttrib(Ihandle* ih, const char* value)
{
  if (iupStrBoolean(value))
    ih->data->show_text = 1;
  else 
    ih->data->show_text = 0;

  iGaugeRepaint(ih);
  return 0; /* do not store value in hash table */
}

static char* iGaugeGetShowTextAttrib(Ihandle* ih)
{
  return iupStrReturnBoolean(ih->data->show_text);
}

static int iGaugeSetFlatAttrib(Ihandle* ih, const char* value)
{
  if (iupStrBoolean(value))
    ih->data->flat = 1;
  else
    ih->data->flat = 0;

  iGaugeRepaint(ih);
  return 0; /* do not store value in hash table */
}

static char* iGaugeGetFlatAttrib(Ihandle* ih)
{
  return iupStrReturnBoolean(ih->data->flat);
}

static int iGaugeSetPaddingAttrib(Ihandle* ih, const char* value)
{
  iupStrToIntInt(value, &ih->data->horiz_padding, &ih->data->vert_padding, 'x');
  iGaugeRepaint(ih);
  return 0;
}

static char* iGaugeGetPaddingAttrib(Ihandle* ih)
{
  return iupStrReturnIntInt(ih->data->horiz_padding, ih->data->vert_padding, 'x');
}

static int iGaugeSetFlatColorAttrib(Ihandle* ih, const char* value)
{
  unsigned char r, g, b;
  if (iupStrToRGB(value, &r, &g, &b))
  {
    ih->data->flatcolor = cdEncodeColor(r, g, b);
    iGaugeRepaint(ih);
  }
  return 0;
}

static char* iGaugeGetFlatColorAttrib(Ihandle* ih)
{
  return iupStrReturnRGB(cdRed(ih->data->flatcolor), cdGreen(ih->data->flatcolor), cdBlue(ih->data->flatcolor));
}

static int iGaugeSetDashedAttrib(Ihandle* ih, const char* value)
{
  if (iupStrBoolean(value))
    ih->data->dashed = 1;
  else 
    ih->data->dashed = 0;

  iGaugeRepaint(ih);
  return 0; /* do not store value in hash table */
}

static char* iGaugeGetDashedAttrib(Ihandle* ih)
{
  return iupStrReturnBoolean(ih->data->dashed);
}

static int iGaugeSetTextAttrib(Ihandle* ih, const char* value)
{
  if (ih->data->text)
    free(ih->data->text);

  ih->data->text = iupStrDup(value);

  iGaugeRepaint(ih);
  return 0; /* do not store value in hash table */
}

static char* iGaugeGetTextAttrib(Ihandle* ih)
{
  return ih->data->text;
}

static void iGaugeUnMapMethod(Ihandle* ih)
{
  if (ih->data->cd_canvas)
  {
    cdKillCanvas(ih->data->cd_canvas);
    ih->data->cd_canvas = NULL;
  }
}

static int iGaugeMapMethod(Ihandle* ih)
{
  ih->data->cd_canvas = cdCreateCanvas(CD_IUPDBUFFER, ih);
  if (!ih->data->cd_canvas)
    return IUP_ERROR;

  return IUP_NOERROR;
}

static int iGaugeCreateMethod(Ihandle* ih, void **params)
{
  (void)params;

  /* free the data allocated by IupCanvas */
  free(ih->data);
  ih->data = iupALLOCCTRLDATA();

  /* change the IupCanvas default values */
  iupAttribSet(ih, "BORDER", "NO");
  IupSetAttribute(ih, "SIZE", IGAUGE_DEFAULTSIZE);
  IupSetAttribute(ih, "EXPAND", "NO");

  /* default values */
  iupAttribSet(ih, "FGCOLOR", IGAUGE_DEFAULTCOLOR);
  ih->data->fgcolor = cdIupConvertColor(IGAUGE_DEFAULTCOLOR);
  ih->data->vmax         = 1;
  ih->data->bgcolor      = CD_GRAY;
  ih->data->light_shadow = CD_WHITE;
  ih->data->mid_shadow   = CD_GRAY;
  ih->data->dark_shadow  = CD_DARK_GRAY;
  ih->data->flatcolor = cdEncodeColor(164, 164, 164);
  ih->data->show_text = 1;

  /* IupCanvas callbacks */
  IupSetCallback(ih, "RESIZE_CB", (Icallback)iGaugeResize_CB);
  IupSetCallback(ih, "ACTION",    (Icallback)iGaugeRedraw_CB);

  return IUP_NOERROR;
}

Iclass* iupGaugeNewClass(void)
{
  Iclass* ic = iupClassNew(iupRegisterFindClass("canvas"));

  ic->name = "gauge";
  ic->format = NULL; /* no parameters */
  ic->nativetype  = IUP_TYPECANVAS;
  ic->childtype   = IUP_CHILDNONE;
  ic->is_interactive = 0;

  /* Class functions */
  ic->New = iupGaugeNewClass;
  ic->Create  = iGaugeCreateMethod;
  ic->Map     = iGaugeMapMethod;
  ic->UnMap   = iGaugeUnMapMethod;

  /* Do not need to set base attributes because they are inherited from IupCanvas */

  /* IupGauge only */
  iupClassRegisterAttribute(ic, "MIN", iGaugeGetMinAttrib, iGaugeSetMinAttrib, IUPAF_SAMEASSYSTEM, "0", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "MAX", iGaugeGetMaxAttrib, iGaugeSetMaxAttrib, IUPAF_SAMEASSYSTEM, "1", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "VALUE", iGaugeGetValueAttrib, iGaugeSetValueAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "DASHED", iGaugeGetDashedAttrib, iGaugeSetDashedAttrib, NULL, NULL, IUPAF_NOT_MAPPED);
  iupClassRegisterAttribute(ic, "PADDING", iGaugeGetPaddingAttrib, iGaugeSetPaddingAttrib, IUPAF_SAMEASSYSTEM, "0x0", IUPAF_NOT_MAPPED);
  iupClassRegisterAttribute(ic, "TEXT", iGaugeGetTextAttrib, iGaugeSetTextAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  /*OLD*/iupClassRegisterAttribute(ic, "SHOW_TEXT", iGaugeGetShowTextAttrib, iGaugeSetShowTextAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NOT_MAPPED);
  iupClassRegisterAttribute(ic, "SHOWTEXT", iGaugeGetShowTextAttrib, iGaugeSetShowTextAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NOT_MAPPED);
  iupClassRegisterAttribute(ic, "FGCOLOR", NULL, iGaugeSetFgColorAttrib, IGAUGE_DEFAULTCOLOR, NULL, IUPAF_NOT_MAPPED);
  iupClassRegisterAttribute(ic, "FLAT", iGaugeGetFlatAttrib, iGaugeSetFlatAttrib, NULL, NULL, IUPAF_NOT_MAPPED);
  iupClassRegisterAttribute(ic, "FLATCOLOR", iGaugeGetFlatColorAttrib, iGaugeSetFlatColorAttrib, IUPAF_SAMEASSYSTEM, "164 164 164", IUPAF_NOT_MAPPED);

  /* Overwrite IupCanvas Attributes */
  iupClassRegisterAttribute(ic, "BGCOLOR", iupControlBaseGetBgColorAttrib, iGaugeSetBgColorAttrib, NULL, "255 255 255", IUPAF_NO_INHERIT);    /* overwrite canvas implementation, set a system default to force a new default */

  return ic;
}

Ihandle *IupGauge(void)
{
  return IupCreate("gauge");
}
