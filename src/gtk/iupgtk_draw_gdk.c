/** \file
 * \brief Draw Functions
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>

#include <gtk/gtk.h>

#include "iup.h"

#include "iup_attrib.h"
#include "iup_class.h"
#include "iup_str.h"
#include "iup_object.h"
#include "iup_image.h"
#include "iup_drvdraw.h"
#include "iup_draw.h"

#include "iupgtk_drv.h"


struct _IdrawCanvas{
  Ihandle* ih;
  int w, h;

  GdkDrawable* wnd;
  GdkPixmap* pixmap;
  GdkGC *gc, *pixmap_gc;

  int draw_focus, 
    focus_x1, focus_y1, focus_x2, focus_y2;
};

static void iupgdkColorSet(GdkColor* c, long color)
{
  c->red = iupCOLOR8TO16(iupDrawRed(color));
  c->green = iupCOLOR8TO16(iupDrawGreen(color));
  c->blue = iupCOLOR8TO16(iupDrawBlue(color));
  c->pixel = 0;
}

IdrawCanvas* iupdrvDrawCreateCanvas(Ihandle* ih)
{
  IdrawCanvas* dc = calloc(1, sizeof(IdrawCanvas));

  dc->ih = ih;
  dc->wnd = (GdkWindow*)IupGetAttribute(ih, "DRAWABLE");
  dc->gc = gdk_gc_new(dc->wnd);

  gdk_drawable_get_size(dc->wnd, &dc->w, &dc->h);

  dc->pixmap = gdk_pixmap_new(dc->wnd, dc->w, dc->h, gdk_drawable_get_depth(dc->wnd));
  dc->pixmap_gc = gdk_gc_new(dc->pixmap);

  return dc;
}

void iupdrvDrawKillCanvas(IdrawCanvas* dc)
{
  g_object_unref(dc->pixmap_gc); 
  g_object_unref(dc->pixmap); 
  g_object_unref(dc->gc); 

  free(dc);
}

void iupdrvDrawUpdateSize(IdrawCanvas* dc)
{
  int w, h;
  gdk_drawable_get_size(dc->wnd, &w, &h);

  if (w != dc->w || h != dc->h)
  {
    dc->w = w;
    dc->h = h;

    g_object_unref(dc->pixmap_gc); 
    g_object_unref(dc->pixmap); 

    dc->pixmap = gdk_pixmap_new(dc->wnd, dc->w, dc->h, gdk_drawable_get_depth(dc->wnd));
    dc->pixmap_gc = gdk_gc_new(dc->pixmap);
  }
}

void iupdrvPaintFocusRect(Ihandle* ih, void* _gc, int x, int y, int w, int h);

void iupdrvDrawFlush(IdrawCanvas* dc)
{
  gdk_draw_drawable(dc->wnd, dc->gc, dc->pixmap, 0, 0, 0, 0, dc->w, dc->h);

  if (dc->draw_focus)
  {
    iupdrvPaintFocusRect(dc->ih, NULL, dc->focus_x1, dc->focus_y1, dc->focus_x2 - dc->focus_x1 + 1, dc->focus_y2 - dc->focus_y1 + 1);
    dc->draw_focus = 0;
  }
}

void iupdrvDrawGetSize(IdrawCanvas* dc, int *w, int *h)
{
  if (w) *w = dc->w;
  if (h) *h = dc->h;
}

static void iDrawSetLineStyle(IdrawCanvas* dc, int style)
{
  GdkGCValues gcval;
  if (style == IUP_DRAW_STROKE || style == IUP_DRAW_FILL)
    gcval.line_style = GDK_LINE_SOLID;
  else
  {
    gint8 dashes[2] = { 6, 2 };
    gint8 dots[2] = { 2, 2 };

    if (style == IUP_DRAW_STROKE_DASH)
      gdk_gc_set_dashes(dc->pixmap_gc, 0, dashes, 2);
    else
      gdk_gc_set_dashes(dc->pixmap_gc, 0, dots, 2);

    gcval.line_style = GDK_LINE_ON_OFF_DASH;
  }

  gdk_gc_set_values(dc->pixmap_gc, &gcval, GDK_GC_LINE_STYLE);
}

static void iDrawSetLineWidth(IdrawCanvas* dc, int line_width)
{
  GdkGCValues gcval;

  if (line_width == 1)
    gcval.line_width = 0;
  else
    gcval.line_width = line_width;

  gdk_gc_set_values(dc->pixmap_gc, &gcval, GDK_GC_LINE_WIDTH);
}

void iupdrvDrawRectangle(IdrawCanvas* dc, int x1, int y1, int x2, int y2, long color, int style, int line_width)
{
  GdkColor c;
  iupgdkColorSet(&c, color);
  gdk_gc_set_rgb_fg_color(dc->pixmap_gc, &c);

  iupDrawCheckSwapCoord(x1, x2);
  iupDrawCheckSwapCoord(y1, y2);

  if (style==IUP_DRAW_FILL)
    gdk_draw_rectangle(dc->pixmap, dc->pixmap_gc, TRUE, x1, y1, x2 - x1 + 1, y2 - y1 + 1);
  else
  {
    iDrawSetLineWidth(dc, line_width);
    iDrawSetLineStyle(dc, style);

    gdk_draw_rectangle(dc->pixmap, dc->pixmap_gc, FALSE, x1, y1, x2 - x1, y2 - y1);  /* outlined rectangle is actually of size w+1,h+1 */
  }
}

void iupdrvDrawLine(IdrawCanvas* dc, int x1, int y1, int x2, int y2, long color, int style, int line_width)
{
  GdkColor c;
  iupgdkColorSet(&c, color);
  gdk_gc_set_rgb_fg_color(dc->pixmap_gc, &c);

  iDrawSetLineWidth(dc, line_width);
  iDrawSetLineStyle(dc, style);

  gdk_draw_line(dc->pixmap, dc->pixmap_gc, x1, y1, x2, y2);
}

void iupdrvDrawArc(IdrawCanvas* dc, int x1, int y1, int x2, int y2, double a1, double a2, long color, int style, int line_width)
{
  GdkColor c;
  iupgdkColorSet(&c, color);
  gdk_gc_set_rgb_fg_color(dc->pixmap_gc, &c);

  iupDrawCheckSwapCoord(x1, x2);
  iupDrawCheckSwapCoord(y1, y2);

  if (style != IUP_DRAW_FILL)
  {
    iDrawSetLineWidth(dc, line_width);
    iDrawSetLineStyle(dc, style);
  }

  /* using x2-x1+1 was resulting in a pixel larger arc */
  gdk_draw_arc(dc->pixmap, dc->pixmap_gc, style == IUP_DRAW_FILL, x1, y1, x2 - x1, y2 - y1, iupRound(a1 * 64), iupRound((a2 - a1) * 64));    /* angle = 1/64ths of a degree */
}

void iupdrvDrawPolygon(IdrawCanvas* dc, int* points, int count, long color, int style, int line_width)
{
  GdkColor c;
  iupgdkColorSet(&c, color);
  gdk_gc_set_rgb_fg_color(dc->pixmap_gc, &c);

  if (style != IUP_DRAW_FILL)
  {
    iDrawSetLineWidth(dc, line_width);
    iDrawSetLineStyle(dc, style);
  }

  gdk_draw_polygon(dc->pixmap, dc->pixmap_gc, style==IUP_DRAW_FILL, (GdkPoint*)points, count);
}

void iupdrvDrawSetClipRect(IdrawCanvas* dc, int x1, int y1, int x2, int y2)
{
  GdkRectangle rect;

  iupDrawCheckSwapCoord(x1, x2);
  iupDrawCheckSwapCoord(y1, y2);

  rect.x = x1;
  rect.y = y1;
  rect.width = x2 - x1 + 1;
  rect.height = y2 - y1 + 1;

  gdk_gc_set_clip_rectangle(dc->pixmap_gc, &rect);
}

void iupdrvDrawResetClip(IdrawCanvas* dc)
{
  gdk_gc_set_clip_region(dc->pixmap_gc, NULL);
}

void iupdrvDrawText(IdrawCanvas* dc, const char* text, int len, int x, int y, int w, int h, long color, const char* font, int align)
{
  PangoLayout* fontlayout = (PangoLayout*)iupgtkGetPangoLayout(font);
  PangoAlignment alignment = PANGO_ALIGN_LEFT;
  GdkColor c;
  (void)w; /* unused */
  (void)h; /* unused */

  iupgdkColorSet(&c, color);
  gdk_gc_set_rgb_fg_color(dc->pixmap_gc, &c);

  text = iupgtkStrConvertToSystemLen(text, &len);
  pango_layout_set_text(fontlayout, text, len);

  if (align == IUP_ALIGN_ARIGHT)
    alignment = PANGO_ALIGN_RIGHT;
  else if (align == IUP_ALIGN_ACENTER)
    alignment = PANGO_ALIGN_CENTER;

  pango_layout_set_alignment(fontlayout, alignment);

  gdk_draw_layout(dc->pixmap, dc->pixmap_gc, x, y, fontlayout);
}

void iupdrvDrawImage(IdrawCanvas* dc, const char* name, int make_inactive, int x, int y)
{
  int bpp, img_w, img_h;
  GdkPixbuf* pixbuf = iupImageGetImage(name, dc->ih, make_inactive);
  if (!pixbuf)
    return;

  /* must use this info, since image can be a driver image loaded from resources */
  iupdrvImageGetInfo(pixbuf, &img_w, &img_h, &bpp);

  gdk_draw_pixbuf(dc->pixmap, dc->pixmap_gc, pixbuf, 0, 0, x, y, img_w, img_h, GDK_RGB_DITHER_NORMAL, 0, 0);
}

void iupdrvDrawSelectRect(IdrawCanvas* dc, int x1, int y1, int x2, int y2)
{
  GdkColor c;
  iupgdkColorSetRGB(&c, 255, 255, 255);
  gdk_gc_set_rgb_fg_color(dc->pixmap_gc, &c);

  iupDrawCheckSwapCoord(x1, x2);
  iupDrawCheckSwapCoord(y1, y2);

  gdk_gc_set_function(dc->pixmap_gc, GDK_XOR);
  gdk_draw_rectangle(dc->pixmap, dc->pixmap_gc, TRUE, x1, y1, x2 - x1 + 1, y2 - y1 + 1);
  gdk_gc_set_function(dc->pixmap_gc, GDK_COPY);
}

void iupdrvDrawFocusRect(IdrawCanvas* dc, int x1, int y1, int x2, int y2)
{
  iupDrawCheckSwapCoord(x1, x2);
  iupDrawCheckSwapCoord(y1, y2);

  dc->draw_focus = 1;  /* draw focus on the next flush */
  dc->focus_x1 = x1;
  dc->focus_y1 = y1;
  dc->focus_x2 = x2;
  dc->focus_y2 = y2;
}

