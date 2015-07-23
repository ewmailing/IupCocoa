/** \file
 * \brief Frame Control
 *
 * See Copyright Notice in "iup.h"
 */

#include <Cocoa/Cocoa.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <stdarg.h>

#include "iup.h"

#include "iup_object.h"
#include "iup_layout.h"
#include "iup_attrib.h"
#include "iup_str.h"
#include "iup_dialog.h"
#include "iup_drv.h"
#include "iup_drvfont.h"
#include "iup_stdcontrols.h"
#include "iup_frame.h"

#include "iupcocoa_drv.h"


void iupdrvFrameGetDecorOffset(int *x, int *y)
{
	*x = 2;
	*y = 2;
}

int iupdrvFrameHasClientOffset(void)
{
	return 0;
}



void iupdrvFrameInitClass(Iclass* ic)
{
#if 0
	/* Driver Dependent Class functions */
	ic->Map = gtkFrameMapMethod;
	ic->GetInnerNativeContainerHandle = gtkFrameGetInnerNativeContainerHandleMethod;
	
	/* Driver Dependent Attribute functions */
	
	/* Overwrite Common */
	iupClassRegisterAttribute(ic, "STANDARDFONT", NULL, gtkFrameSetStandardFontAttrib, IUPAF_SAMEASSYSTEM, "DEFAULTFONT", IUPAF_NO_SAVE|IUPAF_NOT_MAPPED);
	
	/* Visual */
	iupClassRegisterAttribute(ic, "BGCOLOR", iupFrameGetBgColorAttrib, gtkFrameSetBgColorAttrib, IUPAF_SAMEASSYSTEM, "DLGBGCOLOR", IUPAF_DEFAULT);
	iupClassRegisterAttribute(ic, "SUNKEN", NULL, gtkFrameSetSunkenAttrib, NULL, NULL, IUPAF_NO_INHERIT);
	
	/* Special */
	iupClassRegisterAttribute(ic, "FGCOLOR", NULL, gtkFrameSetFgColorAttrib, IUPAF_SAMEASSYSTEM, "DLGFGCOLOR", IUPAF_DEFAULT);
	iupClassRegisterAttribute(ic, "TITLE", NULL, gtkFrameSetTitleAttrib, NULL, NULL, IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);
#endif
}
