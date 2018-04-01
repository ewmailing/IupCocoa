/** \file
 * \brief Auxiliary Draw API.
 *
 * See Copyright Notice in "iup.h"
 */
 
#ifndef __IUP_DRAW_H 
#define __IUP_DRAW_H

#ifdef __cplusplus
extern "C"
{
#endif

/** \defgroup auxdraw Auxiliary Draw API
 * \par
 * See \ref iup_draw.h
 * \ingroup util */


/** Swap integer coordinates if c1 > c2.
* \ingroup auxdraw */
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
char* iupFlatGetDarkerBgColor(Ihandle* ih);

const char* iupFlatGetImageName(Ihandle* ih, const char* baseattrib, const char* basevalue, int press, int highlight, int active, int *make_inactive);
const char* iupFlatGetImageNameId(Ihandle* ih, const char* baseattrib, int id, const char* basevalue, int press, int highlight, int active, int *make_inactive);

void iupFlatDrawBorder(IdrawCanvas* dc, int xmin, int xmax, int ymin, int ymax, int border_width, const char* color, const char* bgcolor, int active);

void iupFlatDrawBox(IdrawCanvas* dc, int xmin, int xmax, int ymin, int ymax, const char* color, const char* bgcolor, int active);

void iupFlatDrawIcon(Ihandle* ih, IdrawCanvas* dc, int icon_x, int icon_y, int icon_width, int icon_height,
                     int img_position, int spacing, int horiz_alignment, int vert_alignment, int horiz_padding, int vert_padding,
                     const char* imagename, int make_inactive, const char* title, const char* text_align, const char* fgcolor, const char* bgcolor, int active);

enum { IUPDRAW_ARROW_LEFT, IUPDRAW_ARROW_RIGHT, IUPDRAW_ARROW_TOP, IUPDRAW_ARROW_BOTTOM };
void iupFlatDrawArrow(IdrawCanvas* dc, int x, int y, int size, const char* color, const char* bgcolor, int active, int dir);
void iupFlatDrawCheckMark(IdrawCanvas* dc, int xmin, int xmax, int ymin, int ymax, const char* color_str, const char* bgcolor, int active);
void iupFlatDrawDrawCircle(IdrawCanvas* dc, int xc, int yc, int radius, int fill, int line_width, char *fgcolor, char *bgcolor, int active);

#ifdef __cplusplus
}
#endif

#endif

