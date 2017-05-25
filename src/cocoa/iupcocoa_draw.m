/** \file
 * \brief Draw Functions
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>

#include <Cocoa/Cocoa.h>

#include "iup.h"

#include "iup_attrib.h"
#include "iup_class.h"
#include "iup_str.h"
#include "iup_object.h"
#include "iup_image.h"
#include "iup_drvdraw.h"




struct _IdrawCanvas
{
	Ihandle* ih;
	
	CGContextRef cgContext;

	
	int w, h;
	int draw_focus;
	int focus_x1;
	int focus_y1;
	int focus_x2;
	int focus_y2;
};

static CGColorRef coregraphicsCreateAutoreleasedColor(unsigned char r, unsigned char g, unsigned char b, unsigned a)
{
	// What color space should I be using?
	//	CGColorSpaceRef color_space = CGColorSpaceCreateWithName(kCGColorSpaceGenericRGB);
	
	CGFloat inv_byte = 1.0/255.0;
	CGColorRef the_color = CGColorCreateGenericRGB(r*inv_byte, g*inv_byte, b*inv_byte, a*inv_byte);
	// Requires 10.9, 7.0
	CFAutorelease(the_color);
	return the_color;
}

static void coregraphicsSetLineStyle(IdrawCanvas* dc, int style)
{
	CGContextRef cg_context = dc->cgContext;

	if((IUP_DRAW_STROKE == style) || (IUP_DRAW_FILL==style))
	{
		CGContextSetLineDash(cg_context, 0, NULL, 0);
	}
	else
	{
		if(IUP_DRAW_STROKE_DASH == style)
		{
			CGFloat dashes[2] = { 6.0, 2.0 };
			CGContextSetLineDash(cg_context, 0, dashes, 2);
		}
		else // DOTS
		{
			CGFloat dots[2] = { 2.0, 2.0 };
			CGContextSetLineDash(cg_context, 0, dots, 2);
		}
	}
}

IdrawCanvas* iupdrvDrawCreateCanvas(Ihandle* ih)
{
	IdrawCanvas* dc = calloc(1, sizeof(IdrawCanvas));
	
	dc->ih = ih;
	dc->cgContext = (CGContextRef)IupGetAttribute(ih, "DRAWABLE");
	NSCAssert(dc->cgContext != NULL, @"CGContextRef should not be NULL");
	// should I retain it?
	CGContextRetain(dc->cgContext);
	
//	gdk_drawable_get_size(dc->wnd, &dc->w, &dc->h);
	
//	dc->pixmap = gdk_pixmap_new(dc->wnd, dc->w, dc->h, gdk_drawable_get_depth(dc->wnd));
	
	return dc;
}

void iupdrvDrawKillCanvas(IdrawCanvas* dc)
{
	CGContextRelease(dc->cgContext);
	free(dc);
}





void iupdrvDrawArc(IdrawCanvas* dc, int x1, int y1, int x2, int y2, double a1, double a2, unsigned char r, unsigned char g, unsigned char b, int style)
{
	CGContextRef cg_context = dc->cgContext;
	
	CGColorRef the_color = coregraphicsCreateAutoreleasedColor(r, g, b, 255);
	
	CGRect the_rectangle = CGRectMake(x1, y1, x2-x1+1, y2-y1+1);
	CGContextAddRect(cg_context, the_rectangle);
	
	if(IUP_DRAW_FILL == style)
	{
		CGContextSetFillColorWithColor(cg_context, the_color);
	}
	else
	{
		CGContextSetStrokeColorWithColor(cg_context, the_color);
		coregraphicsSetLineStyle(dc, style);
	}
	
	

	CGFloat w = x2-x1+1;
	CGFloat h = y2-y1+1;
	CGFloat xc = x1 + w/2;
	CGFloat yc = y1 + h/2;
	
	if (w == h)
	{
		CGContextAddArc(cg_context, xc, yc, 0.5*w, a1, a2, 1);
		if(IUP_DRAW_FILL == style)
		{
			CGContextFillPath(cg_context);
		}
		else
		{
			CGContextStrokePath(cg_context);
		}
	}
	else  /* Ellipse: change the scale to create from the circle */
	{
		/* save to use the local transform */
		CGContextSaveGState(cg_context);
		
		CGContextTranslateCTM(cg_context, xc, yc);
		CGContextScaleCTM(cg_context, w/h, 1.0);
		CGContextTranslateCTM(cg_context, -xc, -yc);
	
		CGContextAddArc(cg_context, xc, yc, 0.5*h, a1, a2, 1);
		
		if(IUP_DRAW_FILL == style)
		{
			CGContextFillPath(cg_context);
		}
		else
		{
			CGContextStrokePath(cg_context);
		}
		
		/* restore from local */
		CGContextRestoreGState(cg_context);
	}
}

void iupdrvDrawPolygon(IdrawCanvas* dc, int* points, int count, unsigned char r, unsigned char g, unsigned char b, int style)
{
	CGContextRef cg_context = dc->cgContext;
	
	CGColorRef the_color = coregraphicsCreateAutoreleasedColor(r, g, b, 255);
	
	if(IUP_DRAW_FILL == style)
	{
		CGContextSetFillColorWithColor(cg_context, the_color);
	}
	else
	{
		CGContextSetStrokeColorWithColor(cg_context, the_color);
		coregraphicsSetLineStyle(dc, style);
	}
	
	CGContextMoveToPoint(cg_context, (CGFloat)points[0], (CGFloat)points[1]);
	
	for(int i=0; i<count; i++)
	{
		CGContextAddLineToPoint(cg_context, (CGFloat)points[2*i], (CGFloat)points[2*i+1]);
	}
	
	if(IUP_DRAW_FILL == style)
	{
		CGContextFillPath(cg_context);
	}
	else
	{
		CGContextStrokePath(cg_context);
	}
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
void iupdrvDrawText(IdrawCanvas* dc, const char* text, int len, int x, int y, unsigned char r, unsigned char g, unsigned char b, const char* font)
{
}
void iupdrvDrawSelectRect(IdrawCanvas* dc, int x1, int y1, int x2, int y2)
{
}

void iupdrvDrawLine(IdrawCanvas* dc, int x1, int y1, int x2, int y2, unsigned char r, unsigned char g, unsigned char b, int style)
{
	CGContextRef cg_context = dc->cgContext;
	
	CGColorRef the_color = coregraphicsCreateAutoreleasedColor(r, g, b, 255);
	CGContextSetStrokeColorWithColor(cg_context, the_color);
	coregraphicsSetLineStyle(dc, style);

	CGContextMoveToPoint(cg_context, (CGFloat)x1, (CGFloat)y1);
	CGContextAddLineToPoint(cg_context, (CGFloat)x2, (CGFloat)y2);
	CGContextStrokePath(cg_context);
}

void iupdrvDrawSetClipRect(IdrawCanvas* dc, int x1, int y1, int x2, int y2)
{
}

void iupdrvDrawRectangle(IdrawCanvas* dc, int x1, int y1, int x2, int y2, unsigned char r, unsigned char g, unsigned char b, int style)
{
	CGContextRef cg_context = dc->cgContext;
	
	CGColorRef the_color = coregraphicsCreateAutoreleasedColor(r, g, b, 255);
	
	if(IUP_DRAW_FILL == style)
	{
		CGRect the_rectangle = CGRectMake(x1, y1, x2-x1, y2-y1);
		CGContextAddRect(cg_context, the_rectangle);
		
		CGContextSetFillColorWithColor(cg_context, the_color);
		CGContextFillRect(cg_context, the_rectangle);
	}
	else
	{
		// The other implementations make the line rect +1
		CGRect the_rectangle = CGRectMake(x1, y1, x2-x1+1, y2-y1+1);
		CGContextAddRect(cg_context, the_rectangle);
		
		CGContextSetStrokeColorWithColor(cg_context, the_color);
		coregraphicsSetLineStyle(dc, style);
		CGContextStrokePath(cg_context);
	}
}

void iupdrvDrawGetSize(IdrawCanvas* dc, int *w, int *h)
{
	char* draw_size_str = IupGetAttribute(dc->ih, "DRAWSIZE");

	
	iupStrToIntInt(draw_size_str, w, h, 'x');

	
	/*
  if (w) *w = dc->w;
  if (h) *h = dc->h;
	 */
	
}

void iupdrvDrawUpdateSize(IdrawCanvas* dc)
{
/*
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
*/
}

void iupdrvDrawFlush(IdrawCanvas* dc)
{
}

void iupdrvDrawImage(IdrawCanvas* dc, const char* name, int make_inactive, int x, int y)
{
}


