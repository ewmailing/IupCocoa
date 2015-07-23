/** \file
 * \brief IupDialog class
 *
 * See Copyright Notice in "iup.h"
 */

#import <Cocoa/Cocoa.h>

#ifdef HILDON
#include <hildon/hildon-program.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <stdarg.h>
#include <limits.h>
#include <time.h>

#include "iup.h"
#include "iupcbs.h"

#include "iup_class.h"
#include "iup_object.h"
#include "iup_layout.h"
#include "iup_dlglist.h"
#include "iup_attrib.h"
#include "iup_drv.h"
#include "iup_drvfont.h"
#include "iup_drvinfo.h"
#include "iup_focus.h"
#include "iup_str.h"
#define _IUPDLG_PRIVATE
#include "iup_dialog.h"
#include "iup_image.h"
#include "iup_assert.h"




/****************************************************************
 Utilities
 ****************************************************************/

int iupdrvDialogIsVisible(Ihandle* ih)
{
	return iupdrvIsVisible(ih);
}

void iupdrvDialogGetSize(Ihandle* ih, InativeHandle* handle, int *w, int *h)
{

	if (w) *w = 640;
	if (h) *h = 480;
}

void iupdrvDialogSetVisible(Ihandle* ih, int visible)
{

}

void iupdrvDialogGetPosition(Ihandle *ih, InativeHandle* handle, int *x, int *y)
{
}

void iupdrvDialogSetPosition(Ihandle *ih, int x, int y)
{
}


void iupdrvDialogGetDecoration(Ihandle* ih, int *border, int *caption, int *menu)
{
}

int iupdrvDialogSetPlacement(Ihandle* ih)
{

	return 1;
}


/****************************************************************
 Callbacks and Events
 ****************************************************************/



void iupdrvDialogInitClass(Iclass* ic)
{
#if 0
	/* Driver Dependent Class methods */
	ic->Map = gtkDialogMapMethod;
	ic->UnMap = gtkDialogUnMapMethod;
	ic->LayoutUpdate = gtkDialogLayoutUpdateMethod;
	ic->GetInnerNativeContainerHandle = gtkDialogGetInnerNativeContainerHandleMethod;
	ic->SetChildrenPosition = gtkDialogSetChildrenPositionMethod;
	
	/* Callback Windows and GTK Only */
	iupClassRegisterCallback(ic, "TRAYCLICK_CB", "iii");
	
	/* Driver Dependent Attribute functions */
#ifndef GTK_MAC
#ifdef WIN32
	iupClassRegisterAttribute(ic, "HWND", iupgtkGetNativeWindowHandle, NULL, NULL, NULL, IUPAF_NO_STRING|IUPAF_NO_INHERIT);
#else
	iupClassRegisterAttribute(ic, "XWINDOW", iupgtkGetNativeWindowHandle, NULL, NULL, NULL, IUPAF_NO_INHERIT|IUPAF_NO_STRING);
#endif
#endif
	
	/* Visual */
	iupClassRegisterAttribute(ic, "BGCOLOR", NULL, iupdrvBaseSetBgColorAttrib, "DLGBGCOLOR", NULL, IUPAF_DEFAULT);  /* force new default value */
	
	/* Base Container */
	iupClassRegisterAttribute(ic, "CLIENTSIZE", gtkDialogGetClientSizeAttrib, iupDialogSetClientSizeAttrib, NULL, NULL, IUPAF_NO_SAVE|IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);  /* dialog is the only not read-only */
	iupClassRegisterAttribute(ic, "CLIENTOFFSET", gtkDialogGetClientOffsetAttrib, NULL, NULL, NULL, IUPAF_NO_DEFAULTVALUE|IUPAF_READONLY|IUPAF_NO_INHERIT);
	
	/* Special */
	iupClassRegisterAttribute(ic, "TITLE", NULL, gtkDialogSetTitleAttrib, NULL, NULL, IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);
	
	/* IupDialog only */
	iupClassRegisterAttribute(ic, "BACKGROUND", NULL, gtkDialogSetBackgroundAttrib, IUPAF_SAMEASSYSTEM, "DLGBGCOLOR", IUPAF_NO_INHERIT);
	iupClassRegisterAttribute(ic, "ICON", NULL, gtkDialogSetIconAttrib, NULL, NULL, IUPAF_IHANDLENAME|IUPAF_NO_INHERIT);
	iupClassRegisterAttribute(ic, "FULLSCREEN", NULL, gtkDialogSetFullScreenAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
	iupClassRegisterAttribute(ic, "MINSIZE", NULL, gtkDialogSetMinSizeAttrib, IUPAF_SAMEASSYSTEM, "1x1", IUPAF_NO_INHERIT);
	iupClassRegisterAttribute(ic, "MAXSIZE", NULL, gtkDialogSetMaxSizeAttrib, IUPAF_SAMEASSYSTEM, "65535x65535", IUPAF_NO_INHERIT);
	iupClassRegisterAttribute(ic, "SAVEUNDER", NULL, NULL, NULL, NULL, IUPAF_NOT_SUPPORTED|IUPAF_NO_INHERIT);  /* saveunder not supported in GTK */
	
	/* IupDialog Windows and GTK Only */
	iupClassRegisterAttribute(ic, "ACTIVEWINDOW", gtkDialogGetActiveWindowAttrib, NULL, NULL, NULL, IUPAF_READONLY|IUPAF_NO_INHERIT);
	iupClassRegisterAttribute(ic, "TOPMOST", NULL, gtkDialogSetTopMostAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
	iupClassRegisterAttribute(ic, "DIALOGHINT", NULL, NULL, NULL, NULL, IUPAF_NO_INHERIT);
#if GTK_CHECK_VERSION(2, 12, 0)
	iupClassRegisterAttribute(ic, "OPACITY", NULL, gtkDialogSetOpacityAttrib, NULL, NULL, IUPAF_NO_INHERIT);
	iupClassRegisterAttribute(ic, "OPACITYIMAGE", NULL, gtkDialogSetOpacityImageAttrib, NULL, NULL, IUPAF_NO_INHERIT);
#endif
#if GTK_CHECK_VERSION(2, 10, 0)
	iupClassRegisterAttribute(ic, "TRAY", NULL, gtkDialogSetTrayAttrib, NULL, NULL, IUPAF_NO_INHERIT);
	iupClassRegisterAttribute(ic, "TRAYIMAGE", NULL, gtkDialogSetTrayImageAttrib, NULL, NULL, IUPAF_NO_INHERIT);
	iupClassRegisterAttribute(ic, "TRAYTIP", NULL, gtkDialogSetTrayTipAttrib, NULL, NULL, IUPAF_NO_INHERIT);
	iupClassRegisterAttribute(ic, "TRAYTIPMARKUP", NULL, NULL, IUPAF_SAMEASSYSTEM, NULL, IUPAF_NOT_MAPPED);
#endif
	
	/* Not Supported */
	iupClassRegisterAttribute(ic, "BRINGFRONT", NULL, NULL, NULL, NULL, IUPAF_NOT_SUPPORTED|IUPAF_NO_INHERIT);
	iupClassRegisterAttribute(ic, "COMPOSITED", NULL, NULL, NULL, NULL, IUPAF_NOT_SUPPORTED|IUPAF_NOT_MAPPED);
	iupClassRegisterAttribute(ic, "CONTROL", NULL, NULL, NULL, NULL, IUPAF_NOT_SUPPORTED|IUPAF_NO_INHERIT);
	iupClassRegisterAttribute(ic, "HELPBUTTON", NULL, NULL, NULL, NULL, IUPAF_NOT_SUPPORTED|IUPAF_NO_INHERIT);
	iupClassRegisterAttribute(ic, "TOOLBOX", NULL, NULL, NULL, NULL, IUPAF_NOT_SUPPORTED|IUPAF_NO_INHERIT);
	iupClassRegisterAttribute(ic, "MDIFRAME", NULL, NULL, NULL, NULL, IUPAF_NOT_SUPPORTED|IUPAF_NO_INHERIT);
	iupClassRegisterAttribute(ic, "MDICLIENT", NULL, NULL, NULL, NULL, IUPAF_NOT_SUPPORTED|IUPAF_NO_INHERIT);
	iupClassRegisterAttribute(ic, "MDIMENU", NULL, NULL, NULL, NULL, IUPAF_NOT_SUPPORTED|IUPAF_NO_INHERIT);
	iupClassRegisterAttribute(ic, "MDICHILD", NULL, NULL, NULL, NULL, IUPAF_NOT_SUPPORTED|IUPAF_NO_INHERIT);
#endif
	
}
