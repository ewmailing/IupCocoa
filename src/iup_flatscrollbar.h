/** \file
 * \brief flat scrollbar utilities
 *
 * See Copyright Notice in "iup.h"
 */
 
#ifndef __IUP_FLATSCROLLBAR_H 
#define __IUP_FLATSCROLLBAR_H

#ifdef __cplusplus
extern "C" {
#endif


int iupFlatScrollBarCreate(Ihandle* ih);
void iupFlatScrollBarRegister(Iclass* ic);

int iupFlatScrollBarGet(Ihandle* ih);
                                               
void iupFlatScrollBarSetChildrenCurrentSize(Ihandle* ih, int shrink);
void iupFlatScrollBarSetChildrenPosition(Ihandle* ih);

void iupFlatScrollBarWheelUpdate(Ihandle* ih, float delta);
void iupFlatScrollBarMotionUpdate(Ihandle* ih, int x, int y);

/* used only in IupFlatScrollBox */
void iupFlatScrollBarSetPos(Ihandle *ih, int posx, int posy);


#ifdef __cplusplus
}
#endif

#endif
