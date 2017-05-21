/** \file
 * \brief GL Draw functions.
 *
 * See Copyright Notice in "iup.h"
 */
 
#ifndef __IUP_GLDRAW_H 
#define __IUP_GLDRAW_H


#ifdef __cplusplus
extern "C" {
#endif


void iupGLDrawMultilineText(Ihandle* ih, int x, int y, const char* str, const char* color, int active);
void iupGLDrawIconImage(Ihandle* ih, int x, int y, const char* baseattrib, const char* imagename, int active);
void iupGLDrawIconImageZoom(Ihandle *ih, int xmin, int xmax, int ymin, int ymax, const char* baseattrib, const char* imagename, int active);
void iupGLDrawLine(Ihandle* ih, int x1, int y1, int x2, int y2, float linewidth, const char* color, int active);
void iupGLDrawPolygon(Ihandle* ih, const int* points, int count, const char* color, int active);
void iupGLDrawPolyline(Ihandle* ih, const int* points, int count, float linewidth, const char* color, int active, int loop);
void iupGLDrawRect(Ihandle* ih, int xmin, int xmax, int ymin, int ymax, float linewidth, const char* color, int active, int round);
void iupGLDrawBox(Ihandle* ih, int xmin, int xmax, int ymin, int ymax, const char* color, int active);
void iupGLDrawFrameRect(Ihandle* ih, int xmin, int xmax, int ymin, int ymax, float linewidth, const char* color, int active, int title_x, int title_width, int title_height);
enum { IUPGL_ARROW_LEFT, IUPGL_ARROW_RIGHT, IUPGL_ARROW_TOP, IUPGL_ARROW_BOTTOM };
void iupGLDrawArrow(Ihandle *ih, int x, int y, int size, const char* color, int active, int dir);
void iupGLDrawSmallCircle(Ihandle* ih, int cx, int cy, int r, float linewidth, const char* color, int active);
void iupGLDrawSmallDisc(Ihandle* ih, int cx, int cy, int rd, const char* color, int active);


#ifdef __cplusplus
}
#endif

#endif
