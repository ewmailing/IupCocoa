/** \file
 * \brief GL Scrollbars functions.
 *
 * See Copyright Notice in "iup.h"
 */
 
#ifndef __IUP_GLSCROLLBARS_H 
#define __IUP_GLSCROLLBARS_H


#ifdef __cplusplus
extern "C" {
#endif


void iupGLScrollbarsRegisterAttrib(Iclass* ic);
int iupGLScrollbarsButton(Ihandle *ih, int pressed, int x, int y);
int  iupGLScrollbarsMotion(Ihandle *ih, int x, int y);
void iupGLScrollbarsEnterWindow(Ihandle* ih, int x, int y);
void iupGLScrollbarsLeaveWindow(Ihandle* ih);
void iupGLScrollbarsDraw(Ihandle* ih, int active);
int iupGLScrollbarsGetSize(Ihandle* ih);
void iupGLScrollbarsChildLayoutUpdate(Ihandle *ih);
void iupGLScrollbarsCheckPosX(Ihandle *ih);
void iupGLScrollbarsCheckPosY(Ihandle *ih);


#ifdef __cplusplus
}
#endif

#endif
