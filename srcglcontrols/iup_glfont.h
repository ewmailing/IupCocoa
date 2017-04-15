/** \file
 * \brief GL Font functions.
 *
 * See Copyright Notice in "iup.h"
 */
 
#ifndef __IUP_GLFONT_H 
#define __IUP_GLFONT_H


#ifdef __cplusplus
extern "C" {
#endif


int iupGLFontGetStringWidth(Ihandle* ih, const char* str, int len);
void iupGLFontGetMultiLineStringSize(Ihandle* ih, const char* str, int *w, int *h);
void iupGLFontGetCharSize(Ihandle* ih, int *charwidth, int *charheight);
void iupGLFontGetDim(Ihandle* ih, int *maxwidth, int *height, int *ascent, int *descent);

int iupGLFontSetFontAttrib(Ihandle* ih, const char* value);
void iupGLFontRenderString(Ihandle* ih, const char* str, int len);
void iupGLFontInit(void);
void iupGLFontFinish(void);
void iupGLFontRelease(Ihandle* gl_parent);


#ifdef __cplusplus
}
#endif

#endif
