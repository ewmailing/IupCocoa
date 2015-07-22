/** \file
 * \brief GL Image functions.
 *
 * See Copyright Notice in "iup.h"
 */
 
#ifndef __IUP_GLIMAGE_H 
#define __IUP_GLIMAGE_H


#ifdef __cplusplus
extern "C" {
#endif


void iupGLImageGetInfo(const char* name, int *w, int *h, int *bpp);
unsigned char* iupGLImageGetData(Ihandle* ih, int active);
void iupGLColorMakeInactive(unsigned char *r, unsigned char *g, unsigned char *b);


#ifdef __cplusplus
}
#endif

#endif
