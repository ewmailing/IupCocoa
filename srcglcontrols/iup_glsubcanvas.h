/** \file
 * \brief GLSubCanvas Control.
 *
 * See Copyright Notice in "iup.h"
 */
 
#ifndef __IUP_GLSUBCANVAS_H 
#define __IUP_GLSUBCANVAS_H


#ifdef __cplusplus
extern "C" {
#endif


int  iupGLSubCanvasRedraw(Ihandle* ih);

int  iupGLSubCanvasSetTransform(Ihandle* ih, Ihandle* gl_parent);
void iupGLSubCanvasSaveState(Ihandle* gl_parent);
void iupGLSubCanvasRestoreState(Ihandle* gl_parent);

void iupGLSubCanvasStartMoving(Ihandle* ih, int x, int y);
int iupGLSubCanvasMove(Ihandle* ih, int x, int y);


#ifdef __cplusplus
}
#endif

#endif
