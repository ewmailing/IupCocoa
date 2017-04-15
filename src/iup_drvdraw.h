/** \file
 * \brief Simple Draw API.
 *
 * See Copyright Notice in "iup.h"
 */
 
#ifndef __IUP_DRAW_H 
#define __IUP_DRAW_H

#ifdef __cplusplus
extern "C"
{
#endif

/** \defgroup draw Simple Draw API
 * \par
 * See \ref iup_drvdraw.h
 * \ingroup util */



struct _IdrawCanvas;
typedef struct _IdrawCanvas IdrawCanvas;

enum{ IUP_DRAW_FILL, IUP_DRAW_STROKE, IUP_DRAW_STROKE_DASH, IUP_DRAW_STROKE_DOT };

/** Creates a draw canvas based on an IupCanvas.
 * This will create an image for offscreen drawing.
 * \ingroup draw */
IdrawCanvas* iupdrvDrawCreateCanvas(Ihandle* ih);

/** Destroys the IdrawCanvas.
 * \ingroup draw */
void iupdrvDrawKillCanvas(IdrawCanvas* dc);

/** Draws the ofscreen image on the screen.
 * \ingroup draw */
void iupdrvDrawFlush(IdrawCanvas* dc);

/** Rebuild the offscreen image if the canvas size has changed.
 * Automatically done in iupdrvDrawCreateCanvas.
 * \ingroup draw */
void iupdrvDrawUpdateSize(IdrawCanvas* dc);

/** Returns the canvas size available for drawing.
 * \ingroup draw */
void iupdrvDrawGetSize(IdrawCanvas* dc, int *w, int *h);

/** Draws the parent background.
 * \ingroup draw */
void iupdrvDrawParentBackground(IdrawCanvas* dc);

/** Draws a line.
 * \ingroup draw */
void iupdrvDrawLine(IdrawCanvas* dc, int x1, int y1, int x2, int y2, unsigned char r, unsigned char g, unsigned char b, int style);

/** Draws a filled/hollow rectangle.
 * \ingroup draw */
void iupdrvDrawRectangle(IdrawCanvas* dc, int x1, int y1, int x2, int y2, unsigned char r, unsigned char g, unsigned char b, int style);

/** Draws a filled/hollow arc.
 * \ingroup draw */
void iupdrvDrawArc(IdrawCanvas* dc, int x1, int y1, int x2, int y2, double a1, double a2, unsigned char r, unsigned char g, unsigned char b, int style);

/** Draws a filled/hollow polygon.
 * points are arranged xyxyxy...
 * \ingroup draw */
void iupdrvDrawPolygon(IdrawCanvas* dc, int* points, int count, unsigned char r, unsigned char g, unsigned char b, int style);

/** Draws a text.
 * x,y is at left,top corner of the text.
 * \ingroup draw */
void iupdrvDrawText(IdrawCanvas* dc, const char* text, int len, int x, int y, unsigned char r, unsigned char g, unsigned char b, const char* font);

/** Draws an image.
 * x,y is at left,top corner of the image.
 * \ingroup draw */
void iupdrvDrawImage(IdrawCanvas* dc, const char* name, int make_inactive, int x, int y);

/** Sets a rectangle clipping area.
 * \ingroup draw */
void iupdrvDrawSetClipRect(IdrawCanvas* dc, int x1, int y1, int x2, int y2);

/** Removes clipping.
 * \ingroup draw */
void iupdrvDrawResetClip(IdrawCanvas* dc);

/** Draws a selection rectangle.
 * \ingroup draw */
void iupdrvDrawSelectRect(IdrawCanvas* dc, int x1, int y1, int x2, int y2);

/** Draws a focus rectangle.
 * \ingroup draw */
void iupdrvDrawFocusRect(IdrawCanvas* dc, int x1, int y1, int x2, int y2);


/**********************************************************************************************************/

enum{ IUP_IMGPOS_LEFT, IUP_IMGPOS_RIGHT, IUP_IMGPOS_TOP, IUP_IMGPOS_BOTTOM };

int iupFlatGetHorizontalAlignment(const char* value);
int iupFlatGetVerticalAlignment(const char* value);
int iupFlatGetImagePosition(const char* value);

void iupFlatDrawBorder(IdrawCanvas* dc, int xmin, int xmax, int ymin, int ymax, int border_width, const char* color, char* bgcolor, int active);

void iupFlatDrawBox(IdrawCanvas* dc, int xmin, int xmax, int ymin, int ymax, const char* color, char* bgcolor, int active);

void iupFlatDrawIcon(Ihandle* ih, IdrawCanvas* dc, int icon_x, int icon_y, int icon_width, int icon_height,
                     int img_position, int spacing, int horiz_alignment, int vert_alignment, int horiz_padding, int vert_padding,
                     const char* imagename, int make_inactive, const char* title, const char* fgcolor, const char* bgcolor, int active);


#ifdef __cplusplus
}
#endif

#endif

