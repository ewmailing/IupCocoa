/** \file
 * \brief Button Control
 *
 * See Copyright Notice in "iup.h"
 */

#import <UIKit/UIKit.h>
#import <objc/runtime.h>

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

#include "iupcocoatouch_drv.h"

#if 0
// the point of this is we have a unique memory address for an identifier
static const void* IUP_COCOA_BUTTON_RECEIVER_OBJ_KEY = "IUP_COCOA_BUTTON_RECEIVER_OBJ_KEY";


@interface IupCocoaButtonReceiver : NSObject
- (IBAction) myButtonClickAction:(id)the_sender;
@end

@implementation IupCocoaButtonReceiver

/*
- (void) dealloc
{
	[super dealloc];
}
*/


- (IBAction) myButtonClickAction:(id)the_sender;
{
	Icallback callback_function;
	Ihandle* ih = (Ihandle*)objc_getAssociatedObject(the_sender, IHANDLE_ASSOCIATED_OBJ_KEY);

	// CONFLICT: Cocoa buttons don't normally do anything for non-primary click. (Second click is supposed to trigger the contextual menu.)
	// Also Cocoa doesn't normall give callbacks for both down and up
	/*
	callback_function = IupGetCallback(ih, "BUTTON_CB");
	if(callback_function)
	{
		if(callback_function(ih) == IUP_CLOSE)
		{
			IupExitLoop();
		}
		
	}
	 */
	
	callback_function = IupGetCallback(ih, "ACTION");
	if(callback_function)
	{
		if(callback_function(ih) == IUP_CLOSE)
		{
			IupExitLoop();
		}
	}
}

@end


#endif

static int cocoaButtonMapMethod(Ihandle* ih)
{
#if 0
	char* value;
#if 0
	int impress;
	
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
#else

	static int woffset = 0;
	static int hoffset = 0;
	
	woffset += 30;
	hoffset += 30;
//	ih->data->type = 0;
	
//	UIButton* the_button = [[UIButton alloc] initWithFrame:NSZeroRect];
	UIButton* the_button = [[UIButton alloc] initWithFrame:NSMakeRect(woffset, hoffset, 0, 0)];
	
	

	

#if 1
	value = iupAttribGet(ih, "IMAGE");
	if(value && *value!=0)
	{
		ih->data->type |= IUP_BUTTON_IMAGE;
		
		[the_button setButtonType:NSMomentaryChangeButton];

		// I don't know what the style should be for images
		// https://mackuba.eu/2014/10/06/a-guide-to-UIButton-styles/
//		[the_button setBezelStyle:NSRoundedBezelStyle];
		[the_button setBezelStyle:NSThickSquareBezelStyle];
//		[the_button setBezelStyle:NSShadowlessSquareBezelStyle];
//		[the_button setBezelStyle:NSTexturedSquareBezelStyle];
//		[the_button setBezelStyle:NSThickerSquareBezelStyle];

		
		NSImage* the_bitmap;
		int make_inactive = 0;
		

		if(!iupAttribGet(ih, "IMINACTIVE"))
		{
			make_inactive = 1;
		}
			
		the_bitmap = iupImageGetImage(value, ih, make_inactive);
		[the_button setImage:the_bitmap];
		
		
		value = iupAttribGet(ih, "IMPRESS");
		if(value && *value!=0)
		{
			the_bitmap = iupImageGetImage(value, ih, make_inactive);
			[the_button setAlternateImage:the_bitmap];
		}
	}
	else
	{
		[the_button setButtonType:NSMomentaryLightButton];
		[the_button setBezelStyle:NSRoundedBezelStyle];

		

	}
#endif
//	[the_button setButtonType:NSMomentaryLightButton];

#if 1
	value = iupAttribGet(ih, "TITLE");
	if(value && *value!=0)
	{
		ih->data->type |= IUP_BUTTON_TEXT;
		NSString* ns_string = [NSString stringWithUTF8String:value];
		[the_button setTitle:ns_string];
		if(ih->data->type & IUP_BUTTON_IMAGE)
		{
			// TODO: FEATURE: Cocoa allows text to be placed in different positions
			// https://developer.apple.com/library/mac/documentation/Cocoa/Conceptual/Button/Tasks/SettingButtonImage.html
			[the_button setImagePosition:NSImageLeft];
		}
		else
		{
//			[the_button setImagePosition:NSNoImage];
			
		}

	}
#endif
	
	[the_button sizeToFit];
	
	
	
	ih->handle = the_button;
	
	// I'm using objc_setAssociatedObject/objc_getAssociatedObject because it allows me to avoid making subclasses just to hold ivars.
	objc_setAssociatedObject(the_button, IHANDLE_ASSOCIATED_OBJ_KEY, (id)ih, OBJC_ASSOCIATION_ASSIGN);
	// I also need to track the memory of the buttion action receiver.
	// I prefer to keep the Ihandle the actual NSView instead of the receiver because it makes the rest of the implementation easier if the handle is always an NSView (or very small set of things, e.g. NSWindow, NSView, CALayer).
	// So with only one pointer to deal with, this means we need our button to hold a reference to the receiver object.
	// This is generally not good Cocoa as buttons don't retain their receivers, but this seems like the best option.
	// Be careful of retain cycles.
	IupCocoaButtonReceiver* button_receiver = [[IupCocoaButtonReceiver alloc] init];
	[the_button setTarget:button_receiver];
	[the_button setAction:@selector(myButtonClickAction:)];
	// I *think* is we use RETAIN, the object will be released automatically when the button is freed.
	// However, the fact that this is tricky and I had to look up the rules (not to mention worrying about retain cycles)
	// makes me think I should just explicitly manage the memory so everybody is aware of what's going on.
	objc_setAssociatedObject(the_button, IUP_COCOA_BUTTON_RECEIVER_OBJ_KEY, (id)button_receiver, OBJC_ASSOCIATION_ASSIGN);

	
	// All Cocoa views shoud call this to add the new view to the parent view.
	iupCocoaAddToParent(ih);

	
#endif
	
	

	
	
//	gtk_widget_realize(ih->handle);
	
	/* update a mnemonic in a label if necessary */
//	iupgtkUpdateMnemonic(ih);
	
#endif
	return IUP_NOERROR;
}

static void cocoaButtonUnMapMethod(Ihandle* ih)
{
#if 0
	id the_button = ih->handle;

	id butten_receiver = objc_getAssociatedObject(the_button, IUP_COCOA_BUTTON_RECEIVER_OBJ_KEY);
	objc_setAssociatedObject(the_button, IUP_COCOA_BUTTON_RECEIVER_OBJ_KEY, nil, OBJC_ASSOCIATION_ASSIGN);
	[butten_receiver release];
	
	[the_button release];
	ih->handle = NULL;
#endif
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
