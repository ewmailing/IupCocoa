/** \file
 * \brief Frame Control (not exported API)
 *
 * See Copyright Notice in "iup.h"
 */
 
#ifndef __IUP_FRAME_H 
#define __IUP_FRAME_H

#ifdef __cplusplus
extern "C" {
#endif

int iupdrvFrameHasClientOffset(Ihandle* ih);
void iupdrvFrameInitClass(Iclass* ic);
void iupdrvFrameGetDecorOffset(Ihandle* ih, int *x, int *y);
int iupFrameGetTitleHeight(Ihandle* ih);
char* iupFrameGetBgColorAttrib(Ihandle* ih);
void iupdrvFrameGetTitleHeight(Ihandle* ih, int *h);
void iupdrvFrameGetDecorSize(Ihandle* ih, int *w, int *h);
	
#ifdef __cplusplus
}
#endif

#endif
