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
#include "iup_draw.h"
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

  iupDrawParentBackground(dc, ih);
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

static int iDrawGetLineWidth(Ihandle* ih)
{
  int line_width = IupGetInt(ih, "DRAWLINEWIDTH");
  if (line_width == 0)
    return 1;
  else
    return line_width;
}

void IupDrawLine(Ihandle* ih, int x1, int y1, int x2, int y2)
{
  IdrawCanvas* dc;
  long color = 0;
  int style, line_width;

  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return;

  dc = (IdrawCanvas*)iupAttribGet(ih, "_IUP_DRAW_DC");
  if (!dc)
    return;

  color = iupDrawStrToColor(IupGetAttribute(ih, "DRAWCOLOR"), 0);

  line_width = iDrawGetLineWidth(ih);
  style = iDrawGetStyle(ih);

  iupdrvDrawLine(dc, x1, y1, x2, y2, color, style, line_width);
}

void IupDrawRectangle(Ihandle* ih, int x1, int y1, int x2, int y2)
{
  IdrawCanvas* dc;
  long color;
  int style, line_width;

  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return;

  dc = (IdrawCanvas*)iupAttribGet(ih, "_IUP_DRAW_DC");
  if (!dc)
    return;

  color = iupDrawStrToColor(IupGetAttribute(ih, "DRAWCOLOR"), 0);

  line_width = iDrawGetLineWidth(ih);
  style = iDrawGetStyle(ih);

  iupdrvDrawRectangle(dc, x1, y1, x2, y2, color, style, line_width);
}

void IupDrawArc(Ihandle* ih, int x1, int y1, int x2, int y2, double a1, double a2)
{
  IdrawCanvas* dc;
  long color = 0;
  int style, line_width;

  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return;

  dc = (IdrawCanvas*)iupAttribGet(ih, "_IUP_DRAW_DC");
  if (!dc)
    return;

  color = iupDrawStrToColor(IupGetAttribute(ih, "DRAWCOLOR"), 0);

  line_width = iDrawGetLineWidth(ih);
  style = iDrawGetStyle(ih);

  iupdrvDrawArc(dc, x1, y1, x2, y2, a1, a2, color, style, line_width);
}

void IupDrawPolygon(Ihandle* ih, int* points, int count)
{
  IdrawCanvas* dc;
  long color = 0;
  int style, line_width;

  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return;

  dc = (IdrawCanvas*)iupAttribGet(ih, "_IUP_DRAW_DC");
  if (!dc)
    return;

  color = iupDrawStrToColor(IupGetAttribute(ih, "DRAWCOLOR"), 0);

  line_width = iDrawGetLineWidth(ih);
  style = iDrawGetStyle(ih);

  iupdrvDrawPolygon(dc, points, count, color, style, line_width);
}

void IupDrawText(Ihandle* ih, const char* text, int len, int x, int y)
{
  IdrawCanvas* dc;
  long color = 0;
  char* font;
  int align, w, h;

  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return;

  iupASSERT(text);
  if (!text || text[0] == 0)
    return;

  dc = (IdrawCanvas*)iupAttribGet(ih, "_IUP_DRAW_DC");
  if (!dc)
    return;

  color = iupDrawStrToColor(IupGetAttribute(ih, "DRAWCOLOR"), 0);

  align = iupFlatGetHorizontalAlignment(IupGetAttribute(ih, "DRAWTEXTALIGNMENT"));

  font = iupDrawGetTextSize(ih, text, &w, &h);

  if (len == 0)
    len = (int)strlen(text);

  if (len != 0)
    iupdrvDrawText(dc, text, len, x, y, w, h, color, font, align);
}

void IupDrawGetTextSize(Ihandle* ih, const char* str, int *w, int *h)
{
  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return;

  iupDrawGetTextSize(ih, str, w, h);
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


/************************************************************************************************/


long iupDrawColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
  a = ~a;
  return (((unsigned long)a) << 24) |
    (((unsigned long)r) << 16) |
    (((unsigned long)g) << 8) |
    (((unsigned long)b) << 0);
}

long iupDrawColorMakeInactive(long color, long bgcolor)
{
  unsigned char r = iupDrawRed(color), g = iupDrawGreen(color), b = iupDrawBlue(color), a = iupDrawAlpha(color);
  unsigned char bg_r = iupDrawRed(bgcolor), bg_g = iupDrawGreen(bgcolor), bg_b = iupDrawBlue(bgcolor);
  iupImageColorMakeInactive(&r, &g, &b, bg_r, bg_g, bg_b);
  return iupDrawColor(r, g, b, a);
}

long iupDrawStrToColor(const char* str, long c_def)
{
  unsigned char r, g, b, a;
  if (iupStrToRGBA(str, &r, &g, &b, &a))
    return iupDrawColor(r, g, b, a);
  else
    return c_def;
}

void iupDrawSetColor(Ihandle *ih, const char* name, long color)
{
  char value[60];
  sprintf(value, "%d %d %d", (int)iupDrawRed(color), (int)iupDrawGreen(color), (int)iupDrawBlue(color));
  iupAttribSetStr(ih, name, value);
}

void iupDrawRaiseRect(Ihandle *ih, int x1, int y1, int x2, int y2, long light_shadow, long mid_shadow, long dark_shadow)
{
  iupDrawSetColor(ih, "DRAWCOLOR", light_shadow);
  IupDrawLine(ih, x1, y1, x1, y2);
  IupDrawLine(ih, x1, y1, x2, y1);

  iupDrawSetColor(ih, "DRAWCOLOR", mid_shadow);
  IupDrawLine(ih, x1 + 1, y2 - 1, x2 - 1, y2 - 1);
  IupDrawLine(ih, x2 - 1, y1 + 1, x2 - 1, y2 - 1);

  iupDrawSetColor(ih, "DRAWCOLOR", dark_shadow);
  IupDrawLine(ih, x1, y2, x2, y2);
  IupDrawLine(ih, x2, y1, x2, y2);
}

void iupDrawVertSunkenMark(Ihandle *ih, int x, int y1, int y2, long light_shadow, long dark_shadow)
{
  iupDrawSetColor(ih, "DRAWCOLOR", dark_shadow);
  IupDrawLine(ih, x - 1, y1, x - 1, y2);
  iupDrawSetColor(ih, "DRAWCOLOR", light_shadow);
  IupDrawLine(ih, x, y1, x, y2);
}

void iupDrawHorizSunkenMark(Ihandle *ih, int x1, int x2, int y, long light_shadow, long dark_shadow)
{
  iupDrawSetColor(ih, "DRAWCOLOR", dark_shadow);
  IupDrawLine(ih, x1, y - 1, x2, y - 1);
  iupDrawSetColor(ih, "DRAWCOLOR", light_shadow);
  IupDrawLine(ih, x1, y, x2, y);
}

void iupDrawSunkenRect(Ihandle *ih, int x1, int y1, int x2, int y2, long light_shadow, long mid_shadow, long dark_shadow)
{
  iupDrawSetColor(ih, "DRAWCOLOR", mid_shadow);
  IupDrawLine(ih, x1, y1, x1, y2);
  IupDrawLine(ih, x1, y1, x2, y1);

  iupDrawSetColor(ih, "DRAWCOLOR", dark_shadow);
  IupDrawLine(ih, x1 + 1, y1 + 1, x1 + 1, y2 - 1);
  IupDrawLine(ih, x1 + 1, y1 + 1, x2 - 1, y1 + 1);

  iupDrawSetColor(ih, "DRAWCOLOR", light_shadow);
  IupDrawLine(ih, x1, y2, x2, y2);
  IupDrawLine(ih, x2, y1, x2, y2);
}

void iupDrawCalcShadows(long bgcolor, long *light_shadow, long *mid_shadow, long *dark_shadow)
{
  int r, bg_r = iupDrawRed(bgcolor);
  int g, bg_g = iupDrawGreen(bgcolor);
  int b, bg_b = iupDrawBlue(bgcolor);

  /* light_shadow */

  int max = bg_r;
  if (bg_g > max) max = bg_g;
  if (bg_b > max) max = bg_b;

  if (255 - max < 64)
  {
    r = 255;
    g = 255;
    b = 255;
  }
  else
  {
    /* preserve some color information */
    if (bg_r == max) r = 255;
    else             r = bg_r + (255 - max);
    if (bg_g == max) g = 255;
    else             g = bg_g + (255 - max);
    if (bg_b == max) b = 255;
    else             b = bg_b + (255 - max);
  }

  if (light_shadow) *light_shadow = iupDrawColor((unsigned char)r, (unsigned char)g, (unsigned char)b, 0);

  /* dark_shadow */
  r = bg_r - 128;
  g = bg_g - 128;
  b = bg_b - 128;
  if (r < 0) r = 0;
  if (g < 0) g = 0;
  if (b < 0) b = 0;

  if (dark_shadow) *dark_shadow = iupDrawColor((unsigned char)r, (unsigned char)g, (unsigned char)b, 0);

  /*   mid_shadow = (dark_shadow+bgcolor)/2    */
  if (mid_shadow) *mid_shadow = iupDrawColor((unsigned char)((bg_r + r) / 2), (unsigned char)((bg_g + g) / 2), (unsigned char)((bg_b + b) / 2), 0);
}

void iupDrawParentBackground(IdrawCanvas* dc, Ihandle* ih)
{
  long color;
  int w, h;
  char* color_str = iupBaseNativeParentGetBgColorAttrib(ih);
  color = iupDrawStrToColor(color_str, 0);
  iupdrvDrawGetSize(dc, &w, &h);
  iupdrvDrawRectangle(dc, 0, 0, w - 1, h - 1, color, IUP_DRAW_FILL, 1);
}

char* iupDrawGetTextSize(Ihandle* ih, const char* str, int *w, int *h)
{
  char*font = IupGetAttribute(ih, "DRAWFONT");
  if (!font)
    font = IupGetAttribute(ih, "FONT");

  iupdrvFontGetTextSize(font, str, w, h);

  return font;
}


/***********************************************************************************************/

static long iFlatDrawColorMakeInactive(long color, const char* bgcolor)
{
  unsigned char bg_r = 0, bg_g = 0, bg_b = 0;
  unsigned char r = iupDrawRed(color), g = iupDrawGreen(color), b = iupDrawBlue(color), a = iupDrawAlpha(color);
  iupStrToRGB(bgcolor, &bg_r, &bg_g, &bg_b);
  iupImageColorMakeInactive(&r, &g, &b, bg_r, bg_g, bg_b);
  return iupDrawColor(r, g, b, a);
}

void iupFlatDrawBorder(IdrawCanvas* dc, int xmin, int xmax, int ymin, int ymax, int border_width, const char* fgcolor, const char* bgcolor, int active)
{
  long color = 0;

  if (!fgcolor || border_width == 0 || xmin == xmax || ymin == ymax)
    return;

  iupDrawCheckSwapCoord(xmin, xmax);
  iupDrawCheckSwapCoord(ymin, ymax);

  color = iupDrawStrToColor(fgcolor, 0);
  if (!active)
    color = iFlatDrawColorMakeInactive(color, bgcolor);

  iupdrvDrawRectangle(dc, xmin, ymin, xmax, ymax, color, IUP_DRAW_STROKE, 1);
  while (border_width > 1)
  {
    border_width--;
    iupdrvDrawRectangle(dc, xmin + border_width,
                        ymin + border_width,
                        xmax - border_width,
                        ymax - border_width, color, IUP_DRAW_STROKE, 1);
  }
}

void iupFlatDrawBox(IdrawCanvas* dc, int xmin, int xmax, int ymin, int ymax, const char* fgcolor, const char* bgcolor, int active)
{
  long color;

  if (!fgcolor || xmin == xmax || ymin == ymax)
    return;

  iupDrawCheckSwapCoord(xmin, xmax);
  iupDrawCheckSwapCoord(ymin, ymax);

  color = iupDrawStrToColor(fgcolor, 0);
  if (!active)
    color = iFlatDrawColorMakeInactive(color, bgcolor);

  iupdrvDrawRectangle(dc, xmin, ymin, xmax, ymax, color, IUP_DRAW_FILL, 1);
}

static void iFlatDrawText(IdrawCanvas* dc, int x, int y, int w, int h, const char* str, const char* font, const char* text_align, const char* fgcolor, const char* bgcolor, int active)
{
  long color;
  int align = iupFlatGetHorizontalAlignment(text_align);

  if (!fgcolor || !str || str[0] == 0)
    return;

  color = iupDrawStrToColor(fgcolor, 0);
  if (!active)
    color = iFlatDrawColorMakeInactive(color, bgcolor);

  iupdrvDrawText(dc, str, (int)strlen(str), x, y, w, h, color, font, align);
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
                     const char* imagename, int make_inactive, const char* title, const char* text_align, const char* fgcolor, const char* bgcolor, int active)
{
  int x, y, width, height;
  char* font;

  iupdrvDrawSetClipRect(dc, icon_x, icon_y, icon_x + icon_width, icon_y + icon_height);

  if (imagename)
  {
    if (title)
    {
      int img_x, img_y, txt_x, txt_y;
      int txt_width, txt_height;
      int img_width, img_height;

      font = iupDrawGetTextSize(ih, title, &txt_width, &txt_height);

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
      iFlatDrawText(dc, txt_x + icon_x, txt_y + icon_y, txt_width, txt_height, title, font, text_align, fgcolor, bgcolor, active);
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
    font = iupDrawGetTextSize(ih, title, &width, &height);

    iFlatGetIconPosition(icon_width, icon_height, &x, &y, width, height, horiz_alignment, vert_alignment, horiz_padding, vert_padding);

    iFlatDrawText(dc, x + icon_x, y + icon_y, width, height, title, font, text_align, fgcolor, bgcolor, active);
  }

  iupdrvDrawResetClip(dc);
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

void iupFlatDrawArrow(IdrawCanvas* dc, int x, int y, int size, const char* color_str, const char* bgcolor, int active, int dir)
{
  int points[6];

  int off1 = iupRound((double)size * 0.13);
  int off2 = iupRound((double)size * 0.87);
  int half = size / 2;

  long color = iupDrawStrToColor(color_str, 0);
  if (!active)
    color = iFlatDrawColorMakeInactive(color, bgcolor);

  switch (dir)
  {
  case IUPDRAW_ARROW_LEFT:  /* arrow points left */
    points[0] = x + off2;
    points[1] = y;
    points[2] = x + off2;
    points[3] = y + size;
    points[4] = x + off1;
    points[5] = y + half;
    break;
  case IUPDRAW_ARROW_TOP:    /* arrow points top */
    points[0] = x;
    points[1] = y + off2;
    points[2] = x + size;
    points[3] = y + off2;
    points[4] = x + half;
    points[5] = y + off1;
    break;
  case IUPDRAW_ARROW_RIGHT:  /* arrow points right */
    points[0] = x + off1;
    points[1] = y;
    points[2] = x + off1;
    points[3] = y + size;
    points[4] = x + size - off1;
    points[5] = y + half;
    break;
  case IUPDRAW_ARROW_BOTTOM:  /* arrow points bottom */
    points[0] = x;
    points[1] = y + off1;
    points[2] = x + size;
    points[3] = y + off1;
    points[4] = x + half;
    points[5] = y + size - off1;
    break;
  }

  iupdrvDrawPolygon(dc, points, 3, color, IUP_DRAW_FILL, 1);
  iupdrvDrawPolygon(dc, points, 3, color, IUP_DRAW_STROKE, 1);
}

void iupFlatDrawCheckMark(IdrawCanvas* dc, int xmin, int xmax, int ymin, int ymax, const char* color_str, const char* bgcolor, int active)
{
  int points[6];

  long color = iupDrawStrToColor(color_str, 0);
  if (!active)
    color = iFlatDrawColorMakeInactive(color, bgcolor);

    points[0] = xmin;
    points[1] = (ymax + ymin) / 2;
    points[2] = (xmax + xmin) / 2;
    points[3] = ymax;
    points[4] = xmax;
    points[5] = ymin;

  iupdrvDrawPolygon(dc, points, 3, color, IUP_DRAW_STROKE, 2);
}

void iupFlatDrawDrawCircle(IdrawCanvas* dc, int xc, int yc, int radius, int fill, int line_width, char *fgcolor, char *bgcolor, int active)
{
  int x1, y1, x2, y2;
  int style = (fill) ? IUP_DRAW_FILL : IUP_DRAW_STROKE;

  long color = iupDrawStrToColor(fgcolor, 0);
  if (!active)
    color = iFlatDrawColorMakeInactive(color, bgcolor);

  x1 = xc - radius;
  y1 = yc - radius;
  x2 = xc + radius;
  y2 = yc + radius;

  iupdrvDrawArc(dc, x1, y1, x2, y2, 0.0, 360, color, style, line_width);
}

static char* iFlatDrawGetImageName(Ihandle* ih, const char* baseattrib, const char* state)
{
  char attrib[1024];
  strcpy(attrib, baseattrib);
  strcat(attrib, state);
  return iupAttribGetStr(ih, attrib);
}

const char* iupFlatGetImageName(Ihandle* ih, const char* baseattrib, const char* basevalue, int press, int highlight, int active, int *make_inactive)
{
  const char* imagename = NULL;

  *make_inactive = 0;

  if (active)
  {
    if (press)
      imagename = iFlatDrawGetImageName(ih, baseattrib, "PRESS");
    else
    {
      if (highlight)
        imagename = iFlatDrawGetImageName(ih, baseattrib, "HIGHLIGHT");
    }
  }
  else
  {
    imagename = iFlatDrawGetImageName(ih, baseattrib, "INACTIVE");
    if (!imagename)
      *make_inactive = 1;
  }

  if (!imagename)
  {
    if (!basevalue)
      basevalue = iupAttribGetStr(ih, baseattrib);

    imagename = basevalue;
  }

  return imagename;
}

static char* iFlatDrawGetImageNameId(Ihandle* ih, const char* baseattrib, const char* state, int id)
{
  char attrib[1024];
  strcpy(attrib, baseattrib);
  strcat(attrib, state);
  return iupAttribGetId(ih, attrib, id);
}

const char* iupFlatGetImageNameId(Ihandle* ih, const char* baseattrib, int id, const char* basevalue, int press, int highlight, int active, int *make_inactive)
{
  const char* imagename = NULL;

  *make_inactive = 0;

  if (active)
  {
    if (press == id)
      imagename = iFlatDrawGetImageNameId(ih, baseattrib, "PRESS", id);
    else
    {
      if (highlight == id)
        imagename = iFlatDrawGetImageNameId(ih, baseattrib, "HIGHLIGHT", id);
    }
  }
  else
  {
    imagename = iFlatDrawGetImageNameId(ih, baseattrib, "INACTIVE", id);
    if (!imagename)
      *make_inactive = 1;
  }

  if (!imagename)
  {
    if (!basevalue)
      basevalue = iupAttribGetId(ih, baseattrib, id);

    imagename = basevalue;
  }

  return imagename;
}

char* iupFlatGetDarkerBgColor(Ihandle* ih)
{
  char* value = iupAttribGet(ih, "DARK_DLGBGCOLOR");
  if (!value)
  {
    unsigned char r, g, b;
    iupStrToRGB(IupGetGlobal("DLGBGCOLOR"), &r, &g, &b);
    r = (r * 90) / 100;
    g = (g * 90) / 100;
    b = (b * 90) / 100;
    iupAttribSetStrf(ih, "DARK_DLGBGCOLOR", "%d %d %d", r, g, b);
    return iupAttribGet(ih, "DARK_DLGBGCOLOR");
  }
  else
    return value;
}
