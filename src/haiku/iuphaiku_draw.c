/** \file
 * \brief Draw Functions
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>

#define UNIMPLEMENTED printf("%s (%s %d) UNIMPLEMENTED\n",__func__,__FILE__,__LINE__);

#include "iup.h"

#include "iup_attrib.h"
#include "iup_class.h"
#include "iup_str.h"
#include "iup_object.h"
#include "iup_image.h"
#include "iup_draw.h"

#include "iuphaiku_drv.h"


struct _IdrawCanvas{
  Ihandle* ih;
  int w, h;

  /*
  GdkDrawable* wnd;
  GdkPixmap* pixmap;
  GdkGC *gc, *pixmap_gc;
  */
};

IdrawCanvas* iupDrawCreateCanvas(Ihandle* ih)
{
  IdrawCanvas* dc = calloc(1, sizeof(IdrawCanvas));

  dc->ih = ih;

UNIMPLEMENTED

  return dc;
}

void iupDrawKillCanvas(IdrawCanvas* dc)
{
UNIMPLEMENTED

  free(dc);
}

void iupDrawUpdateSize(IdrawCanvas* dc)
{
UNIMPLEMENTED
}

void iupDrawFlush(IdrawCanvas* dc)
{
UNIMPLEMENTED
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
	UNIMPLEMENTED
}

void iupDrawLine(IdrawCanvas* dc, int x1, int y1, int x2, int y2, unsigned char r, unsigned char g, unsigned char b, int style)
{
	UNIMPLEMENTED
}

void iupDrawArc(IdrawCanvas* dc, int x1, int y1, int x2, int y2, double a1, double a2, unsigned char r, unsigned char g, unsigned char b, int style)
{
	UNIMPLEMENTED
}

void iupDrawPolygon(IdrawCanvas* dc, int* points, int count, unsigned char r, unsigned char g, unsigned char b, int style)
{
	UNIMPLEMENTED
}

void iupDrawSetClipRect(IdrawCanvas* dc, int x1, int y1, int x2, int y2)
{
	UNIMPLEMENTED
}

void iupDrawResetClip(IdrawCanvas* dc)
{
	UNIMPLEMENTED
}

void iupDrawText(IdrawCanvas* dc, const char* text, int len, int x, int y, unsigned char r, unsigned char g, unsigned char b, const char* font)
{
	UNIMPLEMENTED
}

void iupDrawImage(IdrawCanvas* dc, const char* name, int make_inactive, int x, int y, int *img_w, int *img_h)
{
	UNIMPLEMENTED
}

void iupDrawSelectRect(IdrawCanvas* dc, int x, int y, int w, int h)
{
	UNIMPLEMENTED
}

void iupDrawFocusRect(IdrawCanvas* dc, int x, int y, int w, int h)
{
	UNIMPLEMENTED
}
