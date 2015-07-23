/** \file
 * \brief GL Icon functions.
 *
 * See Copyright Notice in "iup.h"
 */
 
#ifndef __IUP_GLICON_H 
#define __IUP_GLICON_H


#ifdef __cplusplus
extern "C" {
#endif


void iupGLIconRegisterAttrib(Iclass* ic);
void iupGLIconDraw(Ihandle* ih, int icon_x, int icon_y, int icon_width, int icon_height, const char *baseattrib, const char* imagename, const char* title, const char* fgcolor, int active);
void iupGLIconGetSize(Ihandle* ih, const char* image, const char* title, int *w, int *h);
Ihandle* iupGLIconGetImageHandle(Ihandle* ih, const char* baseattrib, const char* imagename, int active);


#ifdef __cplusplus
}
#endif

#endif
