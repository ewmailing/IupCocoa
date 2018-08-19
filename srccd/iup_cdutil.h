/** \file
 * \brief cdiuputil. Utilities for IUP using CD.
 *
 * See Copyright Notice in "iup.h"
 */
 
#ifndef __IUP_CDUTIL_H 
#define __IUP_CDUTIL_H 

#ifdef __cplusplus
extern "C" {
#endif

#include "iupcd_export.h"

#define cdIupInvertYAxis(_y, _h) ((_h) - (_y) - 1)

/* For drawing custom controls */
IUPCD_EXPORTI void cdIupCalcShadows(long bgcolor, long *light_shadow, long *mid_shadow, long *dark_shadow);
IUPCD_EXPORTI long cdIupConvertColor(const char *color);
IUPCD_EXPORTI void cdIupDrawSunkenRect(cdCanvas *canvas, int x1, int y1, int x2, int y2,  
                         long light_shadow, long mid_shadow, long dark_shadow);
IUPCD_EXPORTI void cdIupDrawRaiseRect(cdCanvas *canvas, int x1, int y1, int x2, int y2,  
                         long light_shadow, long mid_shadow, long dark_shadow);
IUPCD_EXPORTI void cdIupDrawVertSunkenMark(cdCanvas *canvas, int x, int y1, int y2, long light_shadow, long dark_shadow);
IUPCD_EXPORTI void cdIupDrawHorizSunkenMark(cdCanvas *canvas, int x1, int x2, int y, long light_shadow, long dark_shadow);

/* for drawing an IupImage on a CD canvas */
IUPCD_EXPORTI void cdIupDrawImage(cdCanvas *canvas, Ihandle *image, int x, int y, int w, int h, int make_inactive, long bgcolor);

/* for drawing in CD but using IUP native functions */
IUPCD_EXPORTI void IupCdDrawFocusRect(Ihandle* ih, cdCanvas *canvas, int x1, int y1, int x2, int y2);
IUPCD_EXPORTI void IupCdSetFont(Ihandle* ih, cdCanvas *canvas, const char* font);


#ifdef __cplusplus
}
#endif

#endif
