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

/** Draws a line.
 * \ingroup draw */
void iupdrvDrawLine(IdrawCanvas* dc, int x1, int y1, int x2, int y2, long color, int style, int line_width);

/** Draws a filled/hollow rectangle.
 * \ingroup draw */
void iupdrvDrawRectangle(IdrawCanvas* dc, int x1, int y1, int x2, int y2, long color, int style, int line_width);

/** Draws a filled/hollow arc.
 * \ingroup draw */
void iupdrvDrawArc(IdrawCanvas* dc, int x1, int y1, int x2, int y2, double a1, double a2, long color, int style, int line_width);

/** Draws a filled/hollow polygon.
 * points are arranged xyxyxy...
 * \ingroup draw */
void iupdrvDrawPolygon(IdrawCanvas* dc, int* points, int count, long color, int style, int line_width);

/** Draws a text.
 * x,y is at left,top corner of the text.
 * \ingroup draw */
void iupdrvDrawText(IdrawCanvas* dc, const char* text, int len, int x, int y, int w, int h, long color, const char* font, int align);

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


#define iupDrawCheckSwapCoord(_c1, _c2) { if (_c1 > _c2) { int t = _c2; _c2 = _c1; _c1 = t; } }   /* make sure _c1 is smaller than _c2 */

long iupDrawStrToColor(const char* str, long c_def);
long iupDrawColor(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha);
void iupDrawCalcShadows(long bgcolor, long *light_shadow, long *mid_shadow, long *dark_shadow);
long iupDrawColorMakeInactive(long color, long bgcolor);

#define iupDrawAlpha(_c)    (unsigned char)(~(((_c) >> 24) & 0xFF))   /* 0=transparent, 255=opaque (default is opaque, internally stored as 0) */
#define iupDrawRed(_c)      (unsigned char)(((_c) >> 16) & 0xFF)
#define iupDrawGreen(_c)    (unsigned char)(((_c) >>  8) & 0xFF)
#define iupDrawBlue(_c)     (unsigned char)(((_c) >>  0) & 0xFF)

void iupDrawSetColor(Ihandle *ih, const char* name, long color);
void iupDrawRaiseRect(Ihandle *ih, int x1, int y1, int x2, int y2, long light_shadow, long mid_shadow, long dark_shadow);
void iupDrawVertSunkenMark(Ihandle *ih, int x, int y1, int y2, long light_shadow, long dark_shadow);
void iupDrawHorizSunkenMark(Ihandle *ih, int x1, int x2, int y, long light_shadow, long dark_shadow);
void iupDrawSunkenRect(Ihandle *ih, int x1, int y1, int x2, int y2, long light_shadow, long mid_shadow, long dark_shadow);

void iupDrawParentBackground(IdrawCanvas* dc, Ihandle* ih);
char* iupDrawGetTextSize(Ihandle* ih, const char* str, int *w, int *h);


/**********************************************************************************************************/


enum{ IUP_IMGPOS_LEFT, IUP_IMGPOS_RIGHT, IUP_IMGPOS_TOP, IUP_IMGPOS_BOTTOM };

int iupFlatGetHorizontalAlignment(const char* value);
int iupFlatGetVerticalAlignment(const char* value);
int iupFlatGetImagePosition(const char* value);

const char* iupFlatGetImageName(Ihandle* ih, const char* baseattrib, const char* basevalue, int press, int highlight, int active, int *make_inactive);
const char* iupFlatGetImageNameId(Ihandle* ih, const char* baseattrib, int id, const char* basevalue, int press, int highlight, int active, int *make_inactive);

void iupFlatDrawBorder(IdrawCanvas* dc, int xmin, int xmax, int ymin, int ymax, int border_width, const char* color, const char* bgcolor, int active);

void iupFlatDrawBox(IdrawCanvas* dc, int xmin, int xmax, int ymin, int ymax, const char* color, const char* bgcolor, int active);

void iupFlatDrawIcon(Ihandle* ih, IdrawCanvas* dc, int icon_x, int icon_y, int icon_width, int icon_height,
                     int img_position, int spacing, int horiz_alignment, int vert_alignment, int horiz_padding, int vert_padding,
                     const char* imagename, int make_inactive, const char* title, const char* text_align, const char* fgcolor, const char* bgcolor, int active);

enum { IUPDRAW_ARROW_LEFT, IUPDRAW_ARROW_RIGHT, IUPDRAW_ARROW_TOP, IUPDRAW_ARROW_BOTTOM };
void iupFlatDrawArrow(IdrawCanvas* dc, int x, int y, int size, const char* color, const char* bgcolor, int active, int dir);


#ifdef __cplusplus
}
#endif

#endif

