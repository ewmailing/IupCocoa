/** \file
 * \brief Button Control
 *
 * See Copyright Notice in "iup.h"
 */

#import <Cocoa/Cocoa.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <stdarg.h>

#include "iup.h"
#include "iupcbs.h"

#include "iup_object.h"
#include "iup_layout.h"
#include "iup_attrib.h"
#include "iup_str.h"
#include "iup_image.h"
#include "iup_button.h"
#include "iup_drv.h"
#include "iup_drvfont.h"
#include "iup_image.h"
#include "iup_key.h"



static int cocoaButtonMapMethod(Ihandle* ih)
{
	int impress;
	char* value;
	
	value = iupAttribGet(ih, "IMAGE");
	if (value)
	{
		ih->data->type = IUP_BUTTON_IMAGE;
		
		value = iupAttribGet(ih, "TITLE");
		if (value && *value!=0)
		{
			ih->data->type |= IUP_BUTTON_TEXT;
		}
	}
	else
	{
		ih->data->type = IUP_BUTTON_TEXT;
	}
		
	if (ih->data->type == IUP_BUTTON_IMAGE &&
		iupAttribGet(ih, "IMPRESS") &&
		!iupAttribGetBoolean(ih, "IMPRESSBORDER"))
	{

		
	}
	else
	{
//		ih->handle = gtk_button_new();
	}
	
	
	
	if (!ih->handle)
	{
		return IUP_ERROR;
	}
	
	if (ih->data->type & IUP_BUTTON_IMAGE)
	{
	/*
		if (!iupAttribGet(ih, "_IUPGTK_EVENTBOX"))
		{
			gtk_button_set_image((GtkButton*)ih->handle, gtk_image_new());
			
			if (ih->data->type & IUP_BUTTON_TEXT)
			{
				GtkSettings* settings = gtk_widget_get_settings(ih->handle);
				g_object_set(settings, "gtk-button-images", (int)TRUE, NULL);
				
				gtk_button_set_label((GtkButton*)ih->handle, iupgtkStrConvertToSystem(iupAttribGet(ih, "TITLE")));
				

	 
			}
		}
*/
	}
	else
	{
		char* title = iupAttribGet(ih, "TITLE");
		if (!title)
		{
			if (iupAttribGet(ih, "BGCOLOR"))
			{
				
			}
			else
			{
//				gtk_button_set_label((GtkButton*)ih->handle, "");
			}
		}
		else
		{
//			gtk_button_set_label((GtkButton*)ih->handle, iupgtkStrConvertToSystem(title));
		}
	}
	
	/* add to the parent, all GTK controls must call this. */
//	iupgtkAddToParent(ih);
	
	if (!iupAttribGetBoolean(ih, "CANFOCUS"))
	{
//		iupgtkSetCanFocus(ih->handle, 0);
	}
	
	value = iupAttribGet(ih, "IMPRESS");
	impress = (ih->data->type & IUP_BUTTON_IMAGE && value)? 1: 0;
	if (!impress && iupAttribGetBoolean(ih, "FLAT"))
	{

	}
	else
	{

	}
	
	

	
	
//	gtk_widget_realize(ih->handle);
	
	/* update a mnemonic in a label if necessary */
//	iupgtkUpdateMnemonic(ih);
	
	return IUP_NOERROR;
}

static void cocoaButtonUnMapMethod(Ihandle* ih)
{
	id the_button = ih->handle;
	[the_button release];
	ih->handle = NULL;
	
}

void iupdrvButtonAddBorders(int *x, int *y)
{

	
}

void iupdrvButtonInitClass(Iclass* ic)
{
	/* Driver Dependent Class functions */
	ic->Map = cocoaButtonMapMethod;
	ic->UnMap = cocoaButtonUnMapMethod;
	
#if 0

	ic->LayoutUpdate = gtkButtonLayoutUpdateMethod;
	
	/* Driver Dependent Attribute functions */
	
	/* Overwrite Common */
	iupClassRegisterAttribute(ic, "STANDARDFONT", NULL, gtkButtonSetStandardFontAttrib, IUPAF_SAMEASSYSTEM, "DEFAULTFONT", IUPAF_NO_SAVE|IUPAF_NOT_MAPPED);
	
	/* Overwrite Visual */
	iupClassRegisterAttribute(ic, "ACTIVE", iupBaseGetActiveAttrib, gtkButtonSetActiveAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_DEFAULT);
	
	/* Visual */
	iupClassRegisterAttribute(ic, "BGCOLOR", NULL, gtkButtonSetBgColorAttrib, IUPAF_SAMEASSYSTEM, "DLGBGCOLOR", IUPAF_DEFAULT);
	
	/* Special */
	iupClassRegisterAttribute(ic, "FGCOLOR", NULL, gtkButtonSetFgColorAttrib, IUPAF_SAMEASSYSTEM, "DLGFGCOLOR", IUPAF_DEFAULT);
	iupClassRegisterAttribute(ic, "TITLE", NULL, gtkButtonSetTitleAttrib, NULL, NULL, IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);
	
	/* IupButton only */
	iupClassRegisterAttribute(ic, "ALIGNMENT", NULL, gtkButtonSetAlignmentAttrib, "ACENTER:ACENTER", NULL, IUPAF_NO_INHERIT);  /* force new default value */
	iupClassRegisterAttribute(ic, "IMAGE", NULL, gtkButtonSetImageAttrib, NULL, NULL, IUPAF_IHANDLENAME|IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);
	iupClassRegisterAttribute(ic, "IMINACTIVE", NULL, gtkButtonSetImInactiveAttrib, NULL, NULL, IUPAF_IHANDLENAME|IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);
	iupClassRegisterAttribute(ic, "IMPRESS", NULL, NULL, NULL, NULL, IUPAF_IHANDLENAME|IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);
	
	iupClassRegisterAttribute(ic, "PADDING", iupButtonGetPaddingAttrib, gtkButtonSetPaddingAttrib, IUPAF_SAMEASSYSTEM, "0x0", IUPAF_NOT_MAPPED);
	iupClassRegisterAttribute(ic, "MARKUP", NULL, NULL, NULL, NULL, IUPAF_DEFAULT);
#endif
	
}
