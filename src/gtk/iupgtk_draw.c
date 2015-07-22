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
#include "iup_draw.h"

#include "iupgtk_drv.h"


struct _IdrawCanvas{
  Ihandle* ih;
  int w, h;

  GdkDrawable* wnd;
  GdkPixmap* pixmap;
  GdkGC *gc, *pixmap_gc;

  int draw_focus, 
    focus_x, focus_y, focus_w, focus_h;
};

IdrawCanvas* iupDrawCreateCanvas(Ihandle* ih)
{
  IdrawCanvas* dc = calloc(1, sizeof(IdrawCanvas));

  dc->ih = ih;
  dc->wnd = iupgtkGetWindow(ih->handle);
  dc->gc = gdk_gc_new(dc->wnd);

  gdk_drawable_get_size(dc->wnd, &dc->w, &dc->h);

  dc->pixmap = gdk_pixmap_new(dc->wnd, dc->w, dc->h, gdk_drawable_get_depth(dc->wnd));
  dc->pixmap_gc = gdk_gc_new(dc->pixmap);

  return dc;
}

void iupDrawKillCanvas(IdrawCanvas* dc)
{
  g_object_unref(dc->pixmap_gc); 
  g_object_unref(dc->pixmap); 
  g_object_unref(dc->gc); 

  free(dc);
}

void iupDrawUpdateSize(IdrawCanvas* dc)
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

void iupdrvDrawFocusRect(Ihandle* ih, void* _gc, int x, int y, int w, int h);

void iupDrawFlush(IdrawCanvas* dc)
{
  gdk_draw_drawable(dc->wnd, dc->gc, dc->pixmap, 0, 0, 0, 0, dc->w, dc->h);

  if (dc->draw_focus)
  {
    iupdrvDrawFocusRect(dc->ih, NULL, dc->focus_x, dc->focus_y, dc->focus_w, dc->focus_h);
    dc->draw_focus = 0;
  }
}

void iupDrawGetSize(IdrawCanvas* dc, int *w, int *h)
{
  if (w) *w = dc->w;
  if (h) *h = dc->h;
}

void iupDrawParentBackground(IdrawCanvas* dc)
{
  unsigned char r=0, g=0, b=0;
  char* color = iupBaseNativeParentGetBgColorAttrib(dc->ih);
  iupStrToRGB(color, &r, &g, &b);
  iupDrawRectangle(dc, 0, 0, dc->w-1, dc->h-1, r, g, b, IUP_DRAW_FILL);
}

void iupDrawRectangle(IdrawCanvas* dc, int x1, int y1, int x2, int y2, unsigned char r, unsigned char g, unsigned char b, int style)
{
  GdkColor color;
  iupgdkColorSet(&color, r, g, b);
  gdk_gc_set_rgb_fg_color(dc->pixmap_gc, &color);

  if (style==IUP_DRAW_FILL)
    gdk_draw_rectangle(dc->pixmap, dc->pixmap_gc, TRUE, x1, y1, x2-x1+1, y2-y1+1);
  else
  {
    GdkGCValues gcval;
    if (style==IUP_DRAW_STROKE_DASH)
      gcval.line_style = GDK_LINE_ON_OFF_DASH;
    else
      gcval.line_style = GDK_LINE_SOLID;
    gdk_gc_set_values(dc->pixmap_gc, &gcval, GDK_GC_LINE_STYLE);

    gdk_draw_rectangle(dc->pixmap, dc->pixmap_gc, FALSE, x1, y1, x2-x1, y2-y1);  /* outlined rectangle is actually of size w+1,h+1 */
  }
}

void iupDrawLine(IdrawCanvas* dc, int x1, int y1, int x2, int y2, unsigned char r, unsigned char g, unsigned char b, int style)
{
  GdkGCValues gcval;
  GdkColor color;
  iupgdkColorSet(&color, r, g, b);
  gdk_gc_set_rgb_fg_color(dc->pixmap_gc, &color);

  if (style==IUP_DRAW_STROKE_DASH)
    gcval.line_style = GDK_LINE_ON_OFF_DASH;
  else
    gcval.line_style = GDK_LINE_SOLID;
  gdk_gc_set_values(dc->pixmap_gc, &gcval, GDK_GC_LINE_STYLE);

  gdk_draw_line(dc->pixmap, dc->pixmap_gc, x1, y1, x2, y2);
}

void iupDrawArc(IdrawCanvas* dc, int x1, int y1, int x2, int y2, double a1, double a2, unsigned char r, unsigned char g, unsigned char b, int style)
{
  GdkColor color;
  iupgdkColorSet(&color, r, g, b);
  gdk_gc_set_rgb_fg_color(dc->pixmap_gc, &color);

  if (style!=IUP_DRAW_FILL)
  {
    GdkGCValues gcval;
    if (style==IUP_DRAW_STROKE_DASH)
      gcval.line_style = GDK_LINE_ON_OFF_DASH;
    else
      gcval.line_style = GDK_LINE_SOLID;
    gdk_gc_set_values(dc->pixmap_gc, &gcval, GDK_GC_LINE_STYLE);
  }

  gdk_draw_arc(dc->pixmap, dc->pixmap_gc, style == IUP_DRAW_FILL, x1, y1, x2 - x1 + 1, y2 - y1 + 1, iupRound(a1 * 64), iupRound((a2 - a1) * 64));
}

void iupDrawPolygon(IdrawCanvas* dc, int* points, int count, unsigned char r, unsigned char g, unsigned char b, int style)
{
  GdkColor color;
  iupgdkColorSet(&color, r, g, b);
  gdk_gc_set_rgb_fg_color(dc->pixmap_gc, &color);

  if (style!=IUP_DRAW_FILL)
  {
    GdkGCValues gcval;
    if (style==IUP_DRAW_STROKE_DASH)
      gcval.line_style = GDK_LINE_ON_OFF_DASH;
    else
      gcval.line_style = GDK_LINE_SOLID;
    gdk_gc_set_values(dc->pixmap_gc, &gcval, GDK_GC_LINE_STYLE);
  }

  gdk_draw_polygon(dc->pixmap, dc->pixmap_gc, style==IUP_DRAW_FILL, (GdkPoint*)points, count);
}

void iupDrawSetClipRect(IdrawCanvas* dc, int x1, int y1, int x2, int y2)
{
  GdkRectangle rect;
  rect.x      = x1;
  rect.y      = y1;
  rect.width  = x2-x1+1;
  rect.height = y2-y1+1;
  gdk_gc_set_clip_rectangle(dc->pixmap_gc, &rect);
}

void iupDrawResetClip(IdrawCanvas* dc)
{
  gdk_gc_set_clip_region(dc->pixmap_gc, NULL);
}

void iupDrawText(IdrawCanvas* dc, const char* text, int len, int x, int y, unsigned char r, unsigned char g, unsigned char b, const char* font)
{
  PangoLayout* fontlayout = (PangoLayout*)iupgtkGetPangoLayout(font);
  GdkColor color;
  iupgdkColorSet(&color, r, g, b);
  gdk_gc_set_rgb_fg_color(dc->pixmap_gc, &color);
  text = iupgtkStrConvertToSystemLen(text, &len);
  pango_layout_set_text(fontlayout, text, len);
  gdk_draw_layout(dc->pixmap, dc->pixmap_gc, x, y, fontlayout);
}

void iupDrawImage(IdrawCanvas* dc, const char* name, int make_inactive, int x, int y, int *img_w, int *img_h)
{
  int bpp;
  GdkPixbuf* pixbuf = iupImageGetImage(name, dc->ih, make_inactive);
  if (!pixbuf)
    return;

  /* must use this info, since image can be a driver image loaded from resources */
  iupdrvImageGetInfo(pixbuf, img_w, img_h, &bpp);

  gdk_draw_pixbuf(dc->pixmap, dc->pixmap_gc, pixbuf, 0, 0, x, y, *img_w, *img_h, GDK_RGB_DITHER_NORMAL, 0, 0);
}

void iupDrawSelectRect(IdrawCanvas* dc, int x, int y, int w, int h)
{
  GdkColor color;
  iupgdkColorSet(&color, 255, 255, 255);
  gdk_gc_set_rgb_fg_color(dc->pixmap_gc, &color);
  gdk_gc_set_function(dc->pixmap_gc, GDK_XOR);
  gdk_draw_rectangle(dc->pixmap, dc->pixmap_gc, TRUE, x, y, w, h);
  gdk_gc_set_function(dc->pixmap_gc, GDK_COPY);
}

void iupDrawFocusRect(IdrawCanvas* dc, int x, int y, int w, int h)
{
  dc->draw_focus = 1;  /* draw focus on the next flush */
  dc->focus_x = x;
  dc->focus_y = y;
  dc->focus_w = w;
  dc->focus_h = h;
}
