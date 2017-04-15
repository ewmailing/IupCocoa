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

#include "iupgtk_drv.h"

/* This was build for GTK3 only */
#if !GTK_CHECK_VERSION(3, 0, 0)
#error This module is for GTK 3.x only
#endif

struct _IdrawCanvas
{
  Ihandle* ih;
  int w, h;

  GtkWidget* widget;
  int release_cr;
  cairo_t *cr, *image_cr;
};

IdrawCanvas* iupdrvDrawCreateCanvas(Ihandle* ih)
{
  IdrawCanvas* dc = calloc(1, sizeof(IdrawCanvas));
  cairo_surface_t* surface;

  dc->ih = ih;

  dc->widget = (GtkWidget*)IupGetAttribute(ih, "WID");

  /* valid only inside the ACTION callback of an IupCanvas */
  dc->cr = (cairo_t*)IupGetAttribute(ih, "CAIRO_CR");
  if (!dc->cr)
  {
    GdkWindow* wnd = (GdkWindow*)IupGetAttribute(ih, "DRAWABLE");
    dc->cr = gdk_cairo_create(wnd);
    dc->release_cr = 1;
  }
  dc->w = gtk_widget_get_allocated_width(dc->widget);
  dc->h = gtk_widget_get_allocated_height(dc->widget);

  surface = cairo_surface_create_similar(cairo_get_target(dc->cr), CAIRO_CONTENT_COLOR_ALPHA, dc->w, dc->h);
  dc->image_cr = cairo_create(surface);
  cairo_surface_destroy(surface);

  return dc;
}

void iupdrvDrawKillCanvas(IdrawCanvas* dc)
{
  cairo_destroy(dc->image_cr);
  if (dc->release_cr)
    cairo_destroy(dc->cr);

  free(dc);
}

void iupdrvDrawUpdateSize(IdrawCanvas* dc)
{
  int w = gtk_widget_get_allocated_width(dc->widget);
  int h = gtk_widget_get_allocated_height(dc->widget);

  if (w != dc->w || h != dc->h)
  {
    cairo_surface_t* surface;

    dc->w = w;
    dc->h = h;

    cairo_destroy(dc->image_cr);

    surface = cairo_surface_create_similar(cairo_get_target(dc->cr), CAIRO_CONTENT_COLOR_ALPHA, dc->w, dc->h);
    dc->image_cr = cairo_create(surface);
    cairo_surface_destroy(surface);
  }
}

void iupdrvDrawFlush(IdrawCanvas* dc)
{
  /* flush the writing in the image */
  cairo_show_page(dc->image_cr);

  cairo_rectangle(dc->cr, 0, 0, dc->w, dc->h);
  cairo_clip(dc->cr);

  /* creates a pattern from the image and sets it as source in the canvas. */
  cairo_set_source_surface(dc->cr, cairo_get_target(dc->image_cr), 0, 0);

  cairo_set_operator(dc->cr, CAIRO_OPERATOR_SOURCE);
  cairo_paint(dc->cr);  /* paints the current source everywhere within the current clip region. */
}

void iupdrvDrawGetSize(IdrawCanvas* dc, int *w, int *h)
{
  if (w) *w = dc->w;
  if (h) *h = dc->h;
}

void iupdrvDrawParentBackground(IdrawCanvas* dc)
{
  unsigned char r=0, g=0, b=0;
  char* color = iupBaseNativeParentGetBgColorAttrib(dc->ih);
  iupStrToRGB(color, &r, &g, &b);
  iupdrvDrawRectangle(dc, 0, 0, dc->w-1, dc->h-1, r, g, b, IUP_DRAW_FILL);
}

static void iDrawSetLineStyle(IdrawCanvas* dc, int style)
{
  cairo_set_line_width(dc->image_cr, 1);

  if (style == IUP_DRAW_STROKE || style == IUP_DRAW_FILL)
    cairo_set_dash(dc->image_cr, 0, 0, 0);
  else
  {
    double dashes[2] = { 6.0, 2.0 };
    double dots[2] = { 2.0, 2.0 };

    if (style == IUP_DRAW_STROKE_DASH)
      cairo_set_dash(dc->image_cr, dashes, 2, 0);
    else
      cairo_set_dash(dc->image_cr, dots, 2, 0);
  }
}

void iupdrvDrawRectangle(IdrawCanvas* dc, int x1, int y1, int x2, int y2, unsigned char r, unsigned char g, unsigned char b, int style)
{
  cairo_set_source_rgba(dc->image_cr, iupCOLOR8ToDouble(r),
                                       iupCOLOR8ToDouble(g),
                                       iupCOLOR8ToDouble(b),
                                       1.0);

  if (style==IUP_DRAW_FILL)
  {
    cairo_rectangle(dc->image_cr, x1, y1, x2-x1+1, y2-y1+1);
    cairo_fill(dc->image_cr);
  }
  else
  {
    iDrawSetLineStyle(dc, style);

    cairo_rectangle(dc->image_cr, x1, y1, x2-x1, y2-y1);  /* outlined rectangle is actually of size w+1,h+1 */
    cairo_stroke(dc->image_cr);
  }
}

void iupdrvDrawLine(IdrawCanvas* dc, int x1, int y1, int x2, int y2, unsigned char r, unsigned char g, unsigned char b, int style)
{
  cairo_set_source_rgba(dc->image_cr, iupCOLOR8ToDouble(r),
                                       iupCOLOR8ToDouble(g),
                                       iupCOLOR8ToDouble(b),
                                       1.0);

  iDrawSetLineStyle(dc, style);

  cairo_move_to(dc->image_cr, x1, y1);
  cairo_line_to(dc->image_cr, x2, y2);
  cairo_stroke(dc->image_cr);
}

void iupdrvDrawArc(IdrawCanvas* dc, int x1, int y1, int x2, int y2, double a1, double a2, unsigned char r, unsigned char g, unsigned char b, int style)
{
  int xc, yc, w, h;

  cairo_set_source_rgba(dc->image_cr, iupCOLOR8ToDouble(r),
                                       iupCOLOR8ToDouble(g),
                                       iupCOLOR8ToDouble(b),
                                       1.0);

  if (style!=IUP_DRAW_FILL)
    iDrawSetLineStyle(dc, style);

  w = x2-x1+1;
  h = y2-y1+1;
  xc = x1 + w/2;
  yc = y1 + h/2;

  if (w == h)
  {
    cairo_arc(dc->image_cr, xc, yc, 0.5*w, a1, a2);

    if (style==IUP_DRAW_FILL)
      cairo_fill(dc->image_cr);
    else
      cairo_stroke(dc->image_cr);
  }
  else  /* Ellipse: change the scale to create from the circle */
  {
    cairo_save(dc->image_cr);  /* save to use the local transform */

    cairo_translate(dc->image_cr, xc, yc);
    cairo_scale(dc->image_cr, w/h, 1.0);
    cairo_translate(dc->image_cr, -xc, -yc);

    cairo_arc(dc->image_cr, xc, yc, 0.5*h, a1, a2);

    if (style==IUP_DRAW_FILL)
      cairo_fill(dc->image_cr);
    else
      cairo_stroke(dc->image_cr);

    cairo_restore(dc->image_cr);  /* restore from local */
  }
}

void iupdrvDrawPolygon(IdrawCanvas* dc, int* points, int count, unsigned char r, unsigned char g, unsigned char b, int style)
{
  int i;

  cairo_set_source_rgba(dc->image_cr, iupCOLOR8ToDouble(r),
                                       iupCOLOR8ToDouble(g),
                                       iupCOLOR8ToDouble(b),
                                       1.0);

  if (style!=IUP_DRAW_FILL)
    iDrawSetLineStyle(dc, style);

  cairo_move_to(dc->image_cr, points[0], points[1]);
  for (i=0; i<count; i++)
    cairo_line_to(dc->image_cr, points[2*i], points[2*i+1]);

  if (style==IUP_DRAW_FILL)
    cairo_fill(dc->image_cr);
  else
    cairo_stroke(dc->image_cr);
}

void iupdrvDrawSetClipRect(IdrawCanvas* dc, int x1, int y1, int x2, int y2)
{
  cairo_rectangle(dc->image_cr, x1, y1, x2-x1+1, y2-y1+1);
  cairo_clip(dc->image_cr);
}

void iupdrvDrawResetClip(IdrawCanvas* dc)
{
  cairo_reset_clip(dc->image_cr);
}

void iupdrvDrawText(IdrawCanvas* dc, const char* text, int len, int x, int y, unsigned char r, unsigned char g, unsigned char b, const char* font)
{
  PangoLayout* fontlayout = (PangoLayout*)iupgtkGetPangoLayout(font);

  text = iupgtkStrConvertToSystemLen(text, &len);
  pango_layout_set_text(fontlayout, text, len);

  cairo_set_source_rgba(dc->image_cr, iupCOLOR8ToDouble(r),
                                       iupCOLOR8ToDouble(g),
                                       iupCOLOR8ToDouble(b),
                                       1.0);

  pango_cairo_update_layout(dc->image_cr, fontlayout);

  cairo_move_to(dc->image_cr, x, y);
  pango_cairo_show_layout(dc->image_cr, fontlayout);
}

void iupdrvDrawImage(IdrawCanvas* dc, const char* name, int make_inactive, int x, int y)
{
  int bpp, img_w, img_h;
  GdkPixbuf* pixbuf = iupImageGetImage(name, dc->ih, make_inactive);
  if (!pixbuf)
    return;

  /* must use this info, since image can be a driver image loaded from resources */
  iupdrvImageGetInfo(pixbuf, &img_w, &img_h, &bpp);

  cairo_save (dc->image_cr);

  cairo_rectangle(dc->image_cr, x, y, img_w, img_h);
  cairo_clip(dc->image_cr);

  gdk_cairo_set_source_pixbuf(dc->image_cr, pixbuf, x, y);
  cairo_paint(dc->image_cr);  /* paints the current source everywhere within the current clip region. */

  /* must restore clipping */
  cairo_restore (dc->image_cr);
}

void iupdrvDrawSelectRect(IdrawCanvas* dc, int x1, int y1, int x2, int y2)
{
  cairo_set_source_rgba(dc->image_cr, 0, 0, 1, 0.60);
  cairo_rectangle(dc->image_cr, x1, y1, x2 - x1 + 1, y2 - y1 + 1);
  cairo_fill(dc->image_cr);
}

void iupdrvDrawFocusRect(IdrawCanvas* dc, int x1, int y1, int x2, int y2)
{
  GtkStyleContext* context = gtk_widget_get_style_context(dc->widget);
  gtk_render_focus(context, dc->image_cr, x1, y1, x2 - x1 + 1, y2 - y1 + 1);
}
