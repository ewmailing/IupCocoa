/** \file
 * \brief Draw Functions
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>

#include <Xm/Xm.h>
#include <X11/Xlib.h>

#include "iup.h"

#include "iup_attrib.h"
#include "iup_class.h"
#include "iup_str.h"
#include "iup_object.h"
#include "iup_image.h"
#include "iup_drvdraw.h"

#include "iupmot_drv.h"
#include "iupmot_color.h"


struct _IdrawCanvas{
  Ihandle* ih;
  int w, h;

  Window wnd;
  Pixmap pixmap;
  GC pixmap_gc, gc;
};

static void motDrawGetGeometry(Display *dpy, Drawable wnd, int *_w, int *_h, int *_d)
{
  Window root;
  int x, y;
  unsigned int w, h, b, d;
  XGetGeometry(dpy, wnd, &root, &x, &y, &w, &h, &b, &d);
  *_w = w;
  *_h = h;
  *_d = d;
}

IdrawCanvas* iupdrvDrawCreateCanvas(Ihandle* ih)
{
  IdrawCanvas* dc = calloc(1, sizeof(IdrawCanvas));
  int depth;

  dc->ih = ih;
  dc->wnd = (Window)IupGetAttribute(ih, "DRAWABLE");

  dc->gc = XCreateGC(iupmot_display, dc->wnd, 0, NULL);

  motDrawGetGeometry(iupmot_display, dc->wnd, &dc->w, &dc->h, &depth);

  dc->pixmap = XCreatePixmap(iupmot_display, dc->wnd, dc->w, dc->h, depth);
  dc->pixmap_gc = XCreateGC(iupmot_display, dc->pixmap, 0, NULL);

  return dc;
}

void iupdrvDrawKillCanvas(IdrawCanvas* dc)
{
  XFreeGC(iupmot_display, dc->pixmap_gc);
  XFreePixmap(iupmot_display, dc->pixmap);
  XFreeGC(iupmot_display, dc->gc);

  free(dc);
}

void iupdrvDrawUpdateSize(IdrawCanvas* dc)
{
  int w, h, depth;

  motDrawGetGeometry(iupmot_display, dc->wnd, &w, &h, &depth);

  if (w != dc->w || h != dc->h)
  {
    dc->w = w;
    dc->h = h;

    XFreeGC(iupmot_display, dc->pixmap_gc);
    XFreePixmap(iupmot_display, dc->pixmap);

    dc->pixmap = XCreatePixmap(iupmot_display, dc->wnd, dc->w, dc->h, depth);
    dc->pixmap_gc = XCreateGC(iupmot_display, dc->pixmap, 0, NULL);
  }
}

void iupdrvDrawFlush(IdrawCanvas* dc)
{
  XCopyArea(iupmot_display, dc->pixmap, dc->wnd, dc->gc, 0, 0, dc->w, dc->h, 0, 0);
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
  XGCValues gcval;
  if (style == IUP_DRAW_STROKE || style == IUP_DRAW_FILL)
    gcval.line_style = LineSolid;
  else
  {
    char dashes[2] = { 6, 2 };
    char dots[2] = { 2, 2 };

    if (style == IUP_DRAW_STROKE_DASH)
      XSetDashes(iupmot_display, dc->pixmap_gc, 0, dashes, 2);
    else
      XSetDashes(iupmot_display, dc->pixmap_gc, 0, dots, 2);

    gcval.line_style = LineOnOffDash;
  }

  XChangeGC(iupmot_display, dc->pixmap_gc, GCLineStyle, &gcval);
}

void iupdrvDrawRectangle(IdrawCanvas* dc, int x1, int y1, int x2, int y2, unsigned char r, unsigned char g, unsigned char b, int style)
{
  XSetForeground(iupmot_display, dc->pixmap_gc, iupmotColorGetPixel(r, g, b));

  if (style==IUP_DRAW_FILL)
    XFillRectangle(iupmot_display, dc->pixmap, dc->pixmap_gc, x1, y1, x2-x1+1, y2-y1+1);
  else
  {
    iDrawSetLineStyle(dc, style);

    XDrawRectangle(iupmot_display, dc->pixmap, dc->pixmap_gc, x1, y1, x2-x1, y2-y1);
  }
}

void iupdrvDrawLine(IdrawCanvas* dc, int x1, int y1, int x2, int y2, unsigned char r, unsigned char g, unsigned char b, int style)
{
  XSetForeground(iupmot_display, dc->pixmap_gc, iupmotColorGetPixel(r, g, b));

  iDrawSetLineStyle(dc, style);

  XDrawLine(iupmot_display, dc->pixmap, dc->pixmap_gc, x1, y1, x2, y2);
}

void iupdrvDrawArc(IdrawCanvas* dc, int x1, int y1, int x2, int y2, double a1, double a2, unsigned char r, unsigned char g, unsigned char b, int style)
{
  XSetForeground(iupmot_display, dc->pixmap_gc, iupmotColorGetPixel(r, g, b));

  if (style==IUP_DRAW_FILL)
  {
    XSetArcMode(iupmot_display, dc->pixmap_gc, ArcPieSlice);
    XFillArc(iupmot_display, dc->pixmap, dc->pixmap_gc, x1, y1, x2 - x1 + 1, y2 - y1 + 1, iupRound(a1 * 64), iupRound((a2 - a1) * 64));
  }
  else
  {
    iDrawSetLineStyle(dc, style);

    XDrawArc(iupmot_display, dc->pixmap, dc->pixmap_gc, x1, y1, x2 - x1 + 1, y2 - y1 + 1, iupRound(a1 * 64), iupRound((a2 - a1) * 64));
  }
}

void iupdrvDrawPolygon(IdrawCanvas* dc, int* points, int count, unsigned char r, unsigned char g, unsigned char b, int style)
{
  int i;
  XPoint* pnt = (XPoint*)malloc(count*sizeof(XPoint)); /* XPoint uses short for coordinates */

  for (i = 0; i < count; i++)
  {
    pnt[i].x = (short)points[2*i];
    pnt[i].y = (short)points[2*i+1];
  }

  XSetForeground(iupmot_display, dc->pixmap_gc, iupmotColorGetPixel(r, g, b));

  if (style==IUP_DRAW_FILL)
    XFillPolygon(iupmot_display, dc->pixmap, dc->pixmap_gc, pnt, count, Complex, CoordModeOrigin);
  else
  {
    iDrawSetLineStyle(dc, style);

    XDrawLines(iupmot_display, dc->pixmap, dc->pixmap_gc, pnt, count, CoordModeOrigin);
  }

  free(pnt);
}

void iupdrvDrawSetClipRect(IdrawCanvas* dc, int x1, int y1, int x2, int y2)
{
  XRectangle rect;
  rect.x      = (short)x1;
  rect.y      = (short)y1;
  rect.width  = (unsigned short)(x2-x1+1);
  rect.height = (unsigned short)(y2-y1+1);
  XSetClipRectangles(iupmot_display, dc->pixmap_gc, 0, 0, &rect, 1, Unsorted);
}

void iupdrvDrawResetClip(IdrawCanvas* dc)
{
  XSetClipMask(iupmot_display, dc->pixmap_gc, None);
}

void iupdrvDrawText(IdrawCanvas* dc, const char* text, int len, int x, int y, unsigned char r, unsigned char g, unsigned char b, const char* font)
{
  int num_line;
  XFontStruct* xfont = (XFontStruct*)iupmotGetFontStruct(font);
  XSetForeground(iupmot_display, dc->pixmap_gc, iupmotColorGetPixel(r, g, b));
  XSetFont(iupmot_display, dc->pixmap_gc, xfont->fid);

  num_line = iupStrLineCount(text);

  if (num_line == 1)
    XDrawString(iupmot_display, dc->pixmap, dc->pixmap_gc, x, y+xfont->ascent, text, len);
  else
  {
    int i, line_height, len;
    const char *p, *q;

    line_height = xfont->ascent + xfont->descent;

    p = text;
    for (i = 0; i < num_line; i++)
    {
      q = strchr(p, '\n');
      if (q)
        len = (int)(q - p);  /* Cut the string to contain only one line */
      else
        len = (int)strlen(p);  /* use the remaining characters */

      /* Draw the line */
      XDrawString(iupmot_display, dc->pixmap, dc->pixmap_gc, x, y + xfont->ascent, p, len);

      /* Advance the string */
      if (q)
        p = q + 1;

      /* Advance a line */
      y += line_height;
    }
  }
}

void iupdrvDrawImage(IdrawCanvas* dc, const char* name, int make_inactive, int x, int y)
{
  int img_w, img_h;
  int bpp;
  Pixmap pixmap = (Pixmap)iupImageGetImage(name, dc->ih, make_inactive);
  if (!pixmap)
    return;

  /* must use this info, since image can be a driver image loaded from resources */
  iupdrvImageGetInfo((void*)pixmap, &img_w, &img_h, &bpp);

  XCopyArea(iupmot_display, pixmap, dc->pixmap, dc->pixmap_gc, 0, 0, img_w, img_h, x, y);
}

void iupdrvDrawSelectRect(IdrawCanvas* dc, int x1, int y1, int x2, int y2)
{
  XSetFunction(iupmot_display, dc->pixmap_gc, GXxor);
  XSetForeground(iupmot_display, dc->pixmap_gc, iupmotColorGetPixel(255, 255, 255));
  XFillRectangle(iupmot_display, dc->pixmap, dc->pixmap_gc, x1, y1, x2 - x1 + 1, y2 - y1 + 1);
  XSetFunction(iupmot_display, dc->pixmap_gc, GXcopy);
}

#include <Xm/XmP.h>
#include <Xm/DrawP.h>

void iupdrvDrawFocusRect(IdrawCanvas* dc, int x1, int y1, int x2, int y2)
{
  XmeDrawHighlight(iupmot_display, dc->pixmap, dc->pixmap_gc, x1, y1, x2 - x1 + 1, y2 - y1 + 1, 1);
}
