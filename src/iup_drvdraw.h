/** \file
 * \brief Driver Draw API.
 *
 * See Copyright Notice in "iup.h"
 */
 
#ifndef __IUP_DRVDRAW_H 
#define __IUP_DRVDRAW_H

#ifdef __cplusplus
extern "C"
{
#endif

/** \defgroup drvdraw Driver Draw API
 * \par
 * See \ref iup_drvdraw.h
 * \ingroup util */



struct _IdrawCanvas;
typedef struct _IdrawCanvas IdrawCanvas;

enum{ IUP_DRAW_FILL, IUP_DRAW_STROKE, IUP_DRAW_STROKE_DASH, IUP_DRAW_STROKE_DOT };

/** Creates a draw canvas based on an IupCanvas.
 * This will create an image for offscreen drawing.
 * \ingroup drvdraw */
IdrawCanvas* iupdrvDrawCreateCanvas(Ihandle* ih);

/** Destroys the IdrawCanvas.
 * \ingroup drvdraw */
void iupdrvDrawKillCanvas(IdrawCanvas* dc);

/** Draws the ofscreen image on the screen.
 * \ingroup drvdraw */
void iupdrvDrawFlush(IdrawCanvas* dc);

/** Rebuild the offscreen image if the canvas size has changed.
 * Automatically done in iupdrvDrawCreateCanvas.
 * \ingroup drvdraw */
void iupdrvDrawUpdateSize(IdrawCanvas* dc);

/** Returns the canvas size available for drawing.
 * \ingroup drvdraw */
void iupdrvDrawGetSize(IdrawCanvas* dc, int *w, int *h);

/** Draws a line.
 * \ingroup drvdraw */
void iupdrvDrawLine(IdrawCanvas* dc, int x1, int y1, int x2, int y2, long color, int style, int line_width);

/** Draws a filled/hollow rectangle.
 * \ingroup drvdraw */
void iupdrvDrawRectangle(IdrawCanvas* dc, int x1, int y1, int x2, int y2, long color, int style, int line_width);

/** Draws a filled/hollow arc.
 * \ingroup drvdraw */
void iupdrvDrawArc(IdrawCanvas* dc, int x1, int y1, int x2, int y2, double a1, double a2, long color, int style, int line_width);

/** Draws a filled/hollow polygon.
 * points are arranged xyxyxy...
 * \ingroup drvdraw */
void iupdrvDrawPolygon(IdrawCanvas* dc, int* points, int count, long color, int style, int line_width);

/** Draws a text.
 * x,y is at left,top corner of the text.
 * \ingroup drvdraw */
void iupdrvDrawText(IdrawCanvas* dc, const char* text, int len, int x, int y, int w, int h, long color, const char* font, int align);

/** Draws an image.
 * x,y is at left,top corner of the image.
 * \ingroup drvdraw */
void iupdrvDrawImage(IdrawCanvas* dc, const char* name, int make_inactive, int x, int y);

/** Sets a rectangle clipping area.
 * \ingroup drvdraw */
void iupdrvDrawSetClipRect(IdrawCanvas* dc, int x1, int y1, int x2, int y2);

/** Removes clipping.
 * \ingroup drvdraw */
void iupdrvDrawResetClip(IdrawCanvas* dc);

/** Draws a selection rectangle.
 * \ingroup drvdraw */
void iupdrvDrawSelectRect(IdrawCanvas* dc, int x1, int y1, int x2, int y2);

/** Draws a focus rectangle.
 * \ingroup drvdraw */
void iupdrvDrawFocusRect(IdrawCanvas* dc, int x1, int y1, int x2, int y2);


#ifdef __cplusplus
}
#endif

#endif

