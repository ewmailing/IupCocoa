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
#include "iup_draw.h"



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

void iupDrawParentBackground(IdrawCanvas* dc)
{

}

void iupDrawRectangle(IdrawCanvas* dc, int x1, int y1, int x2, int y2, unsigned char r, unsigned char g, unsigned char b, int style)
{

}

void iupDrawLine(IdrawCanvas* dc, int x1, int y1, int x2, int y2, unsigned char r, unsigned char g, unsigned char b, int style)
{
}

void iupDrawArc(IdrawCanvas* dc, int x1, int y1, int x2, int y2, double a1, double a2, unsigned char r, unsigned char g, unsigned char b, int style)
{

}

void iupDrawPolygon(IdrawCanvas* dc, int* points, int count, unsigned char r, unsigned char g, unsigned char b, int style)
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


