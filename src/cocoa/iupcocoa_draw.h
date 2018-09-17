#ifndef __IUPCOCOA_DRAWCANVAS_H 
#define __IUPCOCOA_DRAWCANVAS_H

struct _IdrawCanvas
{
	Ihandle* ih;
	
	CGContextRef cgContext;

	
	CGFloat w, h;
/*
	int draw_focus;
	int focus_x1;
	int focus_y1;
	int focus_x2;
	int focus_y2;
*/
};

#endif /* __IUPCOCOA_DRAWCANVAS_H */

