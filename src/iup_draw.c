/** \file
 * \brief Canvas Control.
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "iup.h"
#include "iupdraw.h"

#include "iup_object.h"
#include "iup_attrib.h"
#include "iup_str.h"
#include "iup_drvfont.h"
#include "iup_drvdraw.h"
#include "iup_assert.h"
#include "iup_image.h"



void IupDrawBegin(Ihandle* ih)
{
  IdrawCanvas* dc;

  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return;

  dc = iupdrvDrawCreateCanvas(ih);
  iupAttribSet(ih, "_IUP_DRAW_DC", (char*)dc);
}

void IupDrawEnd(Ihandle* ih)
{
  IdrawCanvas* dc;

  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return;

  dc = (IdrawCanvas*)iupAttribGet(ih, "_IUP_DRAW_DC");
  if (!dc)
    return;

  iupdrvDrawFlush(dc);
  iupdrvDrawKillCanvas(dc);
  iupAttribSet(ih, "_IUP_DRAW_DC", NULL);
}

void IupDrawGetSize(Ihandle* ih, int *w, int *h)
{
  IdrawCanvas* dc;

  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return;

  dc = (IdrawCanvas*)iupAttribGet(ih, "_IUP_DRAW_DC");
  if (!dc)
    return;

  iupdrvDrawGetSize(dc, w, h);
}

void IupDrawParentBackground(Ihandle* ih)
{
  IdrawCanvas* dc;

  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return;

  dc = (IdrawCanvas*)iupAttribGet(ih, "_IUP_DRAW_DC");
  if (!dc)
    return;

  iupdrvDrawParentBackground(dc);
}

static int iDrawGetStyle(Ihandle* ih)
{
  char* style = IupGetAttribute(ih, "DRAWSTYLE");
  if (iupStrEqualNoCase(style, "FILL"))
    return IUP_DRAW_FILL;
  else if (iupStrEqualNoCase(style, "STROKE_DASH"))
    return IUP_DRAW_STROKE_DASH;
  else if (iupStrEqualNoCase(style, "STROKE_DOT"))
    return IUP_DRAW_STROKE_DOT;
  else 
    return IUP_DRAW_STROKE;
}

void IupDrawLine(Ihandle* ih, int x1, int y1, int x2, int y2)
{
  IdrawCanvas* dc;
  unsigned char r = 0, g = 0, b = 0;
  int style;

  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return;

  dc = (IdrawCanvas*)iupAttribGet(ih, "_IUP_DRAW_DC");
  if (!dc)
    return;

  IupGetRGB(ih, "DRAWCOLOR", &r, &g, &b);

  style = iDrawGetStyle(ih);

  iupdrvDrawLine(dc, x1, y1, x2, y2, r, g, b, style);
}

void IupDrawRectangle(Ihandle* ih, int x1, int y1, int x2, int y2)
{
  IdrawCanvas* dc;
  unsigned char r = 0, g = 0, b = 0;
  int style;

  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return;

  dc = (IdrawCanvas*)iupAttribGet(ih, "_IUP_DRAW_DC");
  if (!dc)
    return;

  IupGetRGB(ih, "DRAWCOLOR", &r, &g, &b);

  style = iDrawGetStyle(ih);

  iupdrvDrawRectangle(dc, x1, y1, x2, y2, r, g, b, style);
}

void IupDrawArc(Ihandle* ih, int x1, int y1, int x2, int y2, double a1, double a2)
{
  IdrawCanvas* dc;
  unsigned char r = 0, g = 0, b = 0;
  int style;

  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return;

  dc = (IdrawCanvas*)iupAttribGet(ih, "_IUP_DRAW_DC");
  if (!dc)
    return;

  IupGetRGB(ih, "DRAWCOLOR", &r, &g, &b);

  style = iDrawGetStyle(ih);

  iupdrvDrawArc(dc, x1, y1, x2, y2, a1, a2, r, g, b, style);
}

void IupDrawPolygon(Ihandle* ih, int* points, int count)
{
  IdrawCanvas* dc;
  unsigned char r = 0, g = 0, b = 0;
  int style;

  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return;

  dc = (IdrawCanvas*)iupAttribGet(ih, "_IUP_DRAW_DC");
  if (!dc)
    return;

  IupGetRGB(ih, "DRAWCOLOR", &r, &g, &b);

  style = iDrawGetStyle(ih);

  iupdrvDrawPolygon(dc, points, count, r, g, b, style);
}

void IupDrawText(Ihandle* ih, const char* text, int len, int x, int y)
{
  IdrawCanvas* dc;
  unsigned char r = 0, g = 0, b = 0;
  char* font;

  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return;

  dc = (IdrawCanvas*)iupAttribGet(ih, "_IUP_DRAW_DC");
  if (!dc)
    return;

  font = IupGetAttribute(ih, "FONT");

  IupGetRGB(ih, "DRAWCOLOR", &r, &g, &b);

  iupdrvDrawText(dc, text, len, x, y, r, g, b, font);
}

void IupDrawGetTextSize(Ihandle* ih, const char* str, int *w, int *h)
{
  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return;

  iupdrvFontGetMultiLineStringSize(ih, str, w, h);
}

void IupDrawGetImageInfo(const char* name, int *w, int *h, int *bpp)
{
  iupImageGetInfo(name, w, h, bpp);
}

void IupDrawImage(Ihandle* ih, const char* name, int make_inactive, int x, int y)
{
  IdrawCanvas* dc;

  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return;

  dc = (IdrawCanvas*)iupAttribGet(ih, "_IUP_DRAW_DC");
  if (!dc)
    return;

  iupdrvDrawImage(dc, name, make_inactive, x, y);
}

void IupDrawSetClipRect(Ihandle* ih, int x1, int y1, int x2, int y2)
{
  IdrawCanvas* dc;

  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return;

  dc = (IdrawCanvas*)iupAttribGet(ih, "_IUP_DRAW_DC");
  if (!dc)
    return;

  iupdrvDrawSetClipRect(dc, x1, y1, x2, y2);
}

void IupDrawResetClip(Ihandle* ih)
{
  IdrawCanvas* dc;

  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return;

  dc = (IdrawCanvas*)iupAttribGet(ih, "_IUP_DRAW_DC");
  if (!dc)
    return;

  iupdrvDrawResetClip(dc);
}

void IupDrawSelectRect(Ihandle* ih, int x1, int y1, int x2, int y2)
{
  IdrawCanvas* dc;

  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return;

  dc = (IdrawCanvas*)iupAttribGet(ih, "_IUP_DRAW_DC");
  if (!dc)
    return;

  iupdrvDrawSelectRect(dc, x1, y1, x2, y2);
}

void IupDrawFocusRect(Ihandle* ih, int x1, int y1, int x2, int y2)
{
  IdrawCanvas* dc;

  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return;

  dc = (IdrawCanvas*)iupAttribGet(ih, "_IUP_DRAW_DC");
  if (!dc)
    return;

  iupdrvDrawFocusRect(dc, x1, y1, x2, y2);
}


/***********************************************************************************************/


void iupFlatDrawBorder(IdrawCanvas* dc, int xmin, int xmax, int ymin, int ymax, int border_width, const char* color, char* bgcolor, int active)
{
  unsigned char r = 0, g = 0, b = 0;

  if (!color || border_width == 0 || xmin == xmax || ymin == ymax)
    return;

  if (xmin > xmax) { int _t = xmin; xmin = xmax; xmax = _t; }
  if (ymin > ymax) { int _t = ymin; ymin = ymax; ymax = _t; }

  iupStrToRGB(color, &r, &g, &b);
  if (!active)
  {
    unsigned char bg_r = 0, bg_g = 0, bg_b = 0;
    iupStrToRGB(bgcolor, &bg_r, &bg_g, &bg_b);
    iupImageColorMakeInactive(&r, &g, &b, bg_r, bg_g, bg_b);
  }

  iupdrvDrawRectangle(dc, xmin, ymin, xmax, ymax, r, g, b, IUP_DRAW_STROKE);
  while (border_width > 1)
  {
    border_width--;
    iupdrvDrawRectangle(dc, xmin + border_width,
                        ymin + border_width,
                        xmax - border_width,
                        ymax - border_width, r, g, b, IUP_DRAW_STROKE);
  }
}

void iupFlatDrawBox(IdrawCanvas* dc, int xmin, int xmax, int ymin, int ymax, const char* color, char* bgcolor, int active)
{
  unsigned char r = 0, g = 0, b = 0;

  if (!color || xmin == xmax || ymin == ymax)
    return;

  if (xmin > xmax) { int _t = xmin; xmin = xmax; xmax = _t; }
  if (ymin > ymax) { int _t = ymin; ymin = ymax; ymax = _t; }

  iupStrToRGB(color, &r, &g, &b);
  if (!active)
  {
    unsigned char bg_r = 0, bg_g = 0, bg_b = 0;
    iupStrToRGB(bgcolor, &bg_r, &bg_g, &bg_b);
    iupImageColorMakeInactive(&r, &g, &b, bg_r, bg_g, bg_b);
  }

  iupdrvDrawRectangle(dc, xmin, ymin, xmax, ymax, r, g, b, IUP_DRAW_FILL);
}

static void iFlatDrawText(Ihandle* ih, IdrawCanvas* dc, int x, int y, const char* str, const char* color, const char* bgcolor, int active)
{
  unsigned char r = 0, g = 0, b = 0;

  if (!color || !str || str[0] == 0)
    return;

  iupStrToRGB(color, &r, &g, &b);
  if (!active)
  {
    unsigned char bg_r = 0, bg_g = 0, bg_b = 0;
    iupStrToRGB(bgcolor, &bg_r, &bg_g, &bg_b);
    iupImageColorMakeInactive(&r, &g, &b, bg_r, bg_g, bg_b);
  }

  iupdrvDrawText(dc, str, (int)strlen(str), x, y, r, g, b, IupGetAttribute(ih, "FONT"));
}

static void iFlatGetIconPosition(int icon_width, int icon_height, int *x, int *y, int width, int height, int horiz_alignment, int vert_alignment, int horiz_padding, int vert_padding)
{
  if (horiz_alignment == IUP_ALIGN_ARIGHT)
    *x = icon_width - (width + 2 * horiz_padding);
  else if (horiz_alignment == IUP_ALIGN_ACENTER)
    *x = (icon_width - (width + 2 * horiz_padding)) / 2;
  else  /* ALEFT */
    *x = 0;

  if (vert_alignment == IUP_ALIGN_ABOTTOM)
    *y = icon_height - (height + 2 * vert_padding);
  else if (vert_alignment == IUP_ALIGN_ACENTER)
    *y = (icon_height - (height + 2 * vert_padding)) / 2;
  else  /* ATOP */
    *y = 0;

  *x += horiz_padding;
  *y += vert_padding;
}

static void iFlatGetImageTextPosition(int x, int y, int img_position, int spacing,
                                        int img_width, int img_height, int txt_width, int txt_height,
                                        int *img_x, int *img_y, int *txt_x, int *txt_y)
{
  switch (img_position)
  {
  case IUP_IMGPOS_TOP:
    *img_y = y;
    *txt_y = y + img_height + spacing;
    if (img_width > txt_width)
    {
      *img_x = x;
      *txt_x = x + (img_width - txt_width) / 2;
    }
    else
    {
      *img_x = x + (txt_width - img_width) / 2;
      *txt_x = x;
    }
    break;
  case IUP_IMGPOS_BOTTOM:
    *img_y = y + txt_height + spacing;
    *txt_y = y;
    if (img_width > txt_width)
    {
      *img_x = x;
      *txt_x = x + (img_width - txt_width) / 2;
    }
    else
    {
      *img_x = x + (txt_width - img_width) / 2;
      *txt_x = x;
    }
    break;
  case IUP_IMGPOS_RIGHT:
    *img_x = x + txt_width + spacing;
    *txt_x = x;
    if (img_height > txt_height)
    {
      *img_y = y;
      *txt_y = y + (img_height - txt_height) / 2;
    }
    else
    {
      *img_y = y + (txt_height - img_height) / 2;
      *txt_y = y;
    }
    break;
  default: /* IUP_IMGPOS_LEFT (image at left of text) */
    *img_x = x;
    *txt_x = x + img_width + spacing;
    if (img_height > txt_height)
    {
      *img_y = y;
      *txt_y = y + (img_height - txt_height) / 2;
    }
    else
    {
      *img_y = y + (txt_height - img_height) / 2;
      *txt_y = y;
    }
    break;
  }
}

void iupFlatDrawIcon(Ihandle* ih, IdrawCanvas* dc, int icon_x, int icon_y, int icon_width, int icon_height,
                     int img_position, int spacing, int horiz_alignment, int vert_alignment, int horiz_padding, int vert_padding,
                     const char* imagename, int make_inactive, const char* title, const char* fgcolor, const char* bgcolor, int active)
{
  int x, y, width, height;

  if (imagename)
  {
    if (title)
    {
      int img_x, img_y, txt_x, txt_y;
      int txt_width, txt_height;
      int img_width, img_height;

      iupdrvFontGetMultiLineStringSize(ih, title, &txt_width, &txt_height);
      iupImageGetInfo(imagename, &img_width, &img_height, NULL);

      if (img_position == IUP_IMGPOS_RIGHT || img_position == IUP_IMGPOS_LEFT)
      {
        width = img_width + txt_width + spacing;
        height = iupMAX(img_height, txt_height);
      }
      else
      {
        width = iupMAX(img_width, txt_width);
        height = img_height + txt_height + spacing;
      }

      iFlatGetIconPosition(icon_width, icon_height, &x, &y, width, height, horiz_alignment, vert_alignment, horiz_padding, vert_padding);

      iFlatGetImageTextPosition(x, y, img_position, spacing,
                                  img_width, img_height, txt_width, txt_height,
                                  &img_x, &img_y, &txt_x, &txt_y);

      iupdrvDrawImage(dc, imagename, make_inactive, img_x + icon_x, img_y + icon_y);
      iFlatDrawText(ih, dc, txt_x + icon_x, txt_y + icon_y, title, fgcolor, bgcolor, active);
    }
    else
    {
      iupImageGetInfo(imagename, &width, &height, NULL);

      iFlatGetIconPosition(icon_width, icon_height, &x, &y, width, height, horiz_alignment, vert_alignment, horiz_padding, vert_padding);

      iupdrvDrawImage(dc, imagename, make_inactive, x + icon_x, y + icon_y);
    }
  }
  else if (title)
  {
    iupdrvFontGetMultiLineStringSize(ih, title, &width, &height);

    iFlatGetIconPosition(icon_width, icon_height, &x, &y, width, height, horiz_alignment, vert_alignment, horiz_padding, vert_padding);

    iFlatDrawText(ih, dc, x + icon_x, y + icon_y, title, fgcolor, bgcolor, active);
  }
}

int iupFlatGetHorizontalAlignment(const char* value)
{
  int horiz_alignment = IUP_ALIGN_ACENTER;  /* default always "ACENTER" */
  if (iupStrEqualNoCase(value, "ARIGHT"))
    horiz_alignment = IUP_ALIGN_ARIGHT;
  else if (iupStrEqualNoCase(value, "ALEFT"))
    horiz_alignment = IUP_ALIGN_ALEFT;
  return horiz_alignment;
}

int iupFlatGetVerticalAlignment(const char* value)
{
  int vert_alignment = IUP_ALIGN_ACENTER;  /* default always "ACENTER" */
  if (iupStrEqualNoCase(value, "ABOTTOM"))
    vert_alignment = IUP_ALIGN_ABOTTOM;
  else if (iupStrEqualNoCase(value, "ATOP"))
    vert_alignment = IUP_ALIGN_ATOP;
  return vert_alignment;
}

int iupFlatGetImagePosition(const char* value)
{
  int img_position = IUP_IMGPOS_LEFT; /* default always "LEFT" */
  if (iupStrEqualNoCase(value, "RIGHT"))
    img_position = IUP_IMGPOS_RIGHT;
  else if (iupStrEqualNoCase(value, "BOTTOM"))
    img_position = IUP_IMGPOS_BOTTOM;
  else if (iupStrEqualNoCase(value, "TOP"))
    img_position = IUP_IMGPOS_TOP;
  return img_position;
}

