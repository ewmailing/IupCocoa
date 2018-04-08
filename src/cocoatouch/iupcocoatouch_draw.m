/** \file
 * \brief Draw Functions
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>

#include <UIKit/UIKit.h>

#include "iup.h"

#include "iup_attrib.h"
#include "iup_class.h"
#include "iup_str.h"
#include "iup_object.h"
#include "iup_image.h"
#include "iup_drvdraw.h"



IdrawCanvas* iupDrawCreateCanvas(Ihandle* ih)
{

	return NULL;
}

void iupDrawKillCanvas(IdrawCanvas* dc)
{

	
}

void iupDrawUpdateSize(IdrawCanvas* dc)
{

}

//void iupdrvDrawFocusRect(Ihandle* ih, void* _gc, int x, int y, int w, int h);

void iupDrawFlush(IdrawCanvas* dc)
{

}

void iupDrawGetSize(IdrawCanvas* dc, int *w, int *h)
{
//	if (w) *w = dc->w;
//	if (h) *h = dc->h;
}

void iupdrvDrawRectangle(IdrawCanvas* dc, int x1, int y1, int x2, int y2, long color, int style, int line_width)
{

}

void iupdrvDrawLine(IdrawCanvas* dc, int x1, int y1, int x2, int y2, long color, int style, int line_width)
{
}

void iupdrvDrawArc(IdrawCanvas* dc, int x1, int y1, int x2, int y2, double a1, double a2, long color, int style, int line_width)
{

}

void iupdrvDrawPolygon(IdrawCanvas* dc, int* points, int count, long color, int style, int line_width)
{

}

void iupDrawSetClipRect(IdrawCanvas* dc, int x1, int y1, int x2, int y2)
{

}

void iupDrawResetClip(IdrawCanvas* dc)
{
}

void iupDrawText(IdrawCanvas* dc, const char* text, int len, int x, int y, unsigned char r, unsigned char g, unsigned char b, const char* font)
{

}

void iupDrawImage(IdrawCanvas* dc, const char* name, int make_inactive, int x, int y, int *img_w, int *img_h)
{
}

void iupDrawSelectRect(IdrawCanvas* dc, int x, int y, int w, int h)
{

}

void iupDrawFocusRect(IdrawCanvas* dc, int x, int y, int w, int h)
{
}

void iupdrvDrawFocusRect(IdrawCanvas* dc, int x1, int y1, int x2, int y2)
{
#if 0
	dc->draw_focus = 1;  /* draw focus on the next flush */
	dc->focus_x1 = x1;
	dc->focus_y1 = y1;
	dc->focus_x2 = x2;
	dc->focus_y2 = y2;
#endif
}
void iupdrvDrawResetClip(IdrawCanvas* dc)
{
}

void iupdrvDrawParentBackground(IdrawCanvas* dc)
{
}
void iupdrvDrawText(IdrawCanvas* dc, const char* text, int len, int x, int y, int w, int h, long color, const char* font, int align)
{
}
void iupdrvDrawSelectRect(IdrawCanvas* dc, int x1, int y1, int x2, int y2)
{
}
IdrawCanvas* iupdrvDrawCreateCanvas(Ihandle* ih)
{
#if 0
	IdrawCanvas* dc = calloc(1, sizeof(IdrawCanvas));
	
	dc->ih = ih;
	dc->wnd = (GdkWindow*)IupGetAttribute(ih, "DRAWABLE");
	dc->gc = gdk_gc_new(dc->wnd);
	
	gdk_drawable_get_size(dc->wnd, &dc->w, &dc->h);
	
	dc->pixmap = gdk_pixmap_new(dc->wnd, dc->w, dc->h, gdk_drawable_get_depth(dc->wnd));
	dc->pixmap_gc = gdk_gc_new(dc->pixmap);
	
	return dc;
#endif
	return NULL;
}

void iupdrvDrawSetClipRect(IdrawCanvas* dc, int x1, int y1, int x2, int y2)
{
}
void iupdrvDrawKillCanvas(IdrawCanvas* dc)
{
}

void iupdrvDrawGetSize(IdrawCanvas* dc, int *w, int *h)
{
	/*
  if (w) *w = dc->w;
  if (h) *h = dc->h;
	 */
}
void iupdrvDrawFlush(IdrawCanvas* dc)
{
}

void iupdrvDrawImage(IdrawCanvas* dc, const char* name, int make_inactive, int x, int y)
{
}



