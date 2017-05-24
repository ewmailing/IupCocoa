/** \file
 * \brief Canvas Draw API
 *
 * See Copyright Notice in "iup.h"
 */
 
#ifndef __IUPDRAW_H 
#define __IUPDRAW_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef DOXYGEN_SHOULD_IGNORE_THIS
/** @cond DOXYGEN_SHOULD_IGNORE_THIS */
#ifndef IUP_EXPORT
	#ifdef IUP_BUILD_LIBRARY
		#ifdef __EMSCRIPTEN__
				#include <emscripten.h>
                #define IUP_EXPORT EMSCRIPTEN_KEEPALIVE
        #elif WIN32
                #define IUP_EXPORT __declspec(dllexport)
        #elif defined(__GNUC__) && __GNUC__ >= 4
                #define IUP_EXPORT __attribute__ ((visibility("default")))
        #else
                #define IUP_EXPORT
        #endif
	#else
        #define IUP_EXPORT
	#endif /* IUP_BUILD_LIBRARY */
#endif /* IUP_EXPORT */
/** @endcond DOXYGEN_SHOULD_IGNORE_THIS */
#endif /* DOXYGEN_SHOULD_IGNORE_THIS */

/* all functions can be used only in IUP canvas and inside the ACTION callback */

IUP_EXPORT void IupDrawBegin(Ihandle* ih);
IUP_EXPORT void IupDrawEnd(Ihandle* ih);

/* all primitives can be called only between calls to Begin and End */

IUP_EXPORT void IupDrawSetClipRect(Ihandle* ih, int x1, int y1, int x2, int y2);
IUP_EXPORT void IupDrawResetClip(Ihandle* ih);

/* color controlled by the attribute DRAWCOLOR */
/* line style or fill controlled by the attribute DRAWSTYLE */

IUP_EXPORT void IupDrawParentBackground(Ihandle* ih);
IUP_EXPORT void IupDrawLine(Ihandle* ih, int x1, int y1, int x2, int y2);
IUP_EXPORT void IupDrawRectangle(Ihandle* ih, int x1, int y1, int x2, int y2);
IUP_EXPORT void IupDrawArc(Ihandle* ih, int x1, int y1, int x2, int y2, double a1, double a2);
IUP_EXPORT void IupDrawPolygon(Ihandle* ih, int* points, int count);
IUP_EXPORT void IupDrawText(Ihandle* ih, const char* text, int len, int x, int y);
IUP_EXPORT void IupDrawImage(Ihandle* ih, const char* name, int make_inactive, int x, int y);
IUP_EXPORT void IupDrawSelectRect(Ihandle* ih, int x1, int y1, int x2, int y2);
IUP_EXPORT void IupDrawFocusRect(Ihandle* ih, int x1, int y1, int x2, int y2);

IUP_EXPORT void IupDrawGetSize(Ihandle* ih, int *w, int *h);
IUP_EXPORT void IupDrawGetTextSize(Ihandle* ih, const char* str, int *w, int *h);
IUP_EXPORT void IupDrawGetImageInfo(const char* name, int *w, int *h, int *bpp);


#ifdef __cplusplus
}
#endif

#endif
