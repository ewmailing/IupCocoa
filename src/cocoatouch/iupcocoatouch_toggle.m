/** \file
 * \brief Toggle Control
 *
 * See Copyright Notice in "iup.h"
 */

#import <UIKit/UIKit.h>



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
#include "iup_drv.h"
#include "iup_drvfont.h"
#include "iup_image.h"
#include "iup_key.h"
#include "iup_toggle.h"

#include "iupcocoatouch_drv.h"


/** \file
 * \brief Toggle Control
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
#include "iup_toggle.h"
#include "iup_drv.h"
#include "iup_drvfont.h"
#include "iup_image.h"
#include "iup_key.h"

#include "iupcocoatouch_drv.h"

#if 0
// the point of this is we have a unique memory address for an identifier
static const void* IUP_COCOA_TOGGLE_RECEIVER_OBJ_KEY = "IUP_COCOA_TOGGLE_RECEIVER_OBJ_KEY";


@interface IupCocoaToggleReceiver : NSObject
- (IBAction) myToggleClickAction:(id)the_sender;
@end

@implementation IupCocoaToggleReceiver

/*
 - (void) dealloc
 {
	[super dealloc];
 }
 */


- (IBAction) myToggleClickAction:(id)the_sender;
{
	Icallback callback_function;
	Ihandle* ih = (Ihandle*)objc_getAssociatedObject(the_sender, IHANDLE_ASSOCIATED_OBJ_KEY);
	
	NSControlStateValue new_state = [the_sender state];
	
	
	// CONFLICT: Cocoa Toggles don't normally do anything for non-primary click. (Second click is supposed to trigger the contextual menu.)
	// Also Cocoa doesn't normall give callbacks for both down and up
	/*
	 callback_function = IupGetCallback(ih, "toggle_CB");
	 if(callback_function)
	 {
		if(callback_function(ih) == IUP_CLOSE)
		{
	 IupExitLoop();
		}
	 
	 }
	 */
	
	IFni action_callback_function = (IFni)IupGetCallback(ih, "ACTION");
	if(action_callback_function)
	{
		if(action_callback_function(ih, (int)new_state) == IUP_CLOSE)
		{
			IupExitLoop();
		}
	}
	Icallback valuechanged_callback_function = IupGetCallback(ih, "VALUECHANGED_CB");
	if(valuechanged_callback_function)
	{
		if(valuechanged_callback_function(ih) == IUP_CLOSE)
		{
			IupExitLoop();
		}
	}
}

@end




static int cocoaToggleMapMethod(Ihandle* ih)
{
	char* value;

	
	
	static int woffset = 0;
	static int hoffset = 0;
	
	woffset += 30;
	hoffset += 30;
	//	ih->data->type = 0;
	
	//	NSToggle* the_toggle = [[NSToggle alloc] initWithFrame:NSZeroRect];
	UIButton* the_toggle = [[NSButton alloc] initWithFrame:NSMakeRect(woffset, hoffset, 0, 0)];
	
	[the_toggle setButtonType:NSSwitchButton];

	
	
	

	value = iupAttribGet(ih, "IMAGE");
	if(value && *value!=0)
	{
		ih->data->type = IUP_TOGGLE_IMAGE;
		
		[the_toggle setButtonType:NSSwitchButton];
		
		// I don't know what the style should be for images
		// https://mackuba.eu/2014/10/06/a-guide-to-nsToggle-styles/
		//		[the_toggle setBezelStyle:NSRoundedBezelStyle];
//		[the_toggle setBezelStyle:NSThickSquareBezelStyle];
		//		[the_toggle setBezelStyle:NSShadowlessSquareBezelStyle];
		//		[the_toggle setBezelStyle:NSTexturedSquareBezelStyle];
		//		[the_toggle setBezelStyle:NSThickerSquareBezelStyle];
		
		
		NSImage* the_bitmap;
		int make_inactive = 0;
		
		
		if(!iupAttribGet(ih, "IMINACTIVE"))
		{
			make_inactive = 1;
		}
		
		the_bitmap = iupImageGetImage(value, ih, make_inactive);
		[the_toggle setImage:the_bitmap];
		
		
		value = iupAttribGet(ih, "IMPRESS");
		if(value && *value!=0)
		{
			the_bitmap = iupImageGetImage(value, ih, make_inactive);
			[the_toggle setAlternateImage:the_bitmap];
		}
	}
	else
	{
		[the_toggle setButtonType:NSSwitchButton];
//		[the_toggle setBezelStyle:NSRoundedBezelStyle];
		
		
		
	}

	//	[the_toggle setToggleType:NSMomentaryLightButton];
	

	value = iupAttribGet(ih, "TITLE");
	if(value && *value!=0)
	{
		ih->data->type |= IUP_TOGGLE_TEXT;
		NSString* ns_string = [NSString stringWithUTF8String:value];
		[the_toggle setTitle:ns_string];
		/*
		if(ih->data->type == IUP_TOGGLE_IMAGE)
		{
			// TODO: FEATURE: Cocoa allows text to be placed in different positions
			// https://developer.apple.com/library/mac/documentation/Cocoa/Conceptual/Toggle/Tasks/SettingToggleImage.html
			[the_toggle setImagePosition:NSImageLeft];
		}
		else
		{
			//			[the_toggle setImagePosition:NSNoImage];
			
		}
		*/
	}

	
	[the_toggle sizeToFit];
	
	
	
	ih->handle = the_toggle;
	
	// I'm using objc_setAssociatedObject/objc_getAssociatedObject because it allows me to avoid making subclasses just to hold ivars.
	objc_setAssociatedObject(the_toggle, IHANDLE_ASSOCIATED_OBJ_KEY, (id)ih, OBJC_ASSOCIATION_ASSIGN);
	// I also need to track the memory of the buttion action receiver.
	// I prefer to keep the Ihandle the actual NSView instead of the receiver because it makes the rest of the implementation easier if the handle is always an NSView (or very small set of things, e.g. NSWindow, NSView, CALayer).
	// So with only one pointer to deal with, this means we need our Toggle to hold a reference to the receiver object.
	// This is generally not good Cocoa as Toggles don't retain their receivers, but this seems like the best option.
	// Be careful of retain cycles.
	IupCocoaToggleReceiver* toggle_receiver = [[IupCocoaToggleReceiver alloc] init];
	[the_toggle setTarget:toggle_receiver];
	[the_toggle setAction:@selector(myToggleClickAction:)];
	// I *think* is we use RETAIN, the object will be released automatically when the Toggle is freed.
	// However, the fact that this is tricky and I had to look up the rules (not to mention worrying about retain cycles)
	// makes me think I should just explicitly manage the memory so everybody is aware of what's going on.
	objc_setAssociatedObject(the_toggle, IUP_COCOA_TOGGLE_RECEIVER_OBJ_KEY, (id)toggle_receiver, OBJC_ASSOCIATION_ASSIGN);
	
	
	// All Cocoa views shoud call this to add the new view to the parent view.
	iupCocoaAddToParent(ih);
	
	

	
	
	
	
	
	//	gtk_widget_realize(ih->handle);
	
	/* update a mnemonic in a label if necessary */
	//	iupgtkUpdateMnemonic(ih);
	
	return IUP_NOERROR;
}

static void cocoaToggleUnMapMethod(Ihandle* ih)
{
	id the_toggle = ih->handle;
	
	id butten_receiver = objc_getAssociatedObject(the_toggle, IUP_COCOA_TOGGLE_RECEIVER_OBJ_KEY);
	objc_setAssociatedObject(the_toggle, IUP_COCOA_TOGGLE_RECEIVER_OBJ_KEY, nil, OBJC_ASSOCIATION_ASSIGN);
	[butten_receiver release];
	
	[the_toggle release];
	ih->handle = NULL;
	
}
#endif
void iupdrvToggleAddBorders(Ihandle* ih, int *x, int *y)
{
	
	
}



void iupdrvToggleAddCheckBox(Ihandle* ih, int *x, int *y, const char* str)
{

	
}


void iupdrvToggleInitClass(Iclass* ic)
{
	/* Driver Dependent Class functions */
//	ic->Map = cocoaToggleMapMethod;
//	ic->UnMap = cocoaToggleUnMapMethod;
	
#if 0

	
  /* Driver Dependent Attribute functions */

  /* Overwrite Common */
  iupClassRegisterAttribute(ic, "STANDARDFONT", NULL, gtkToggleSetStandardFontAttrib, IUPAF_SAMEASSYSTEM, "DEFAULTFONT", IUPAF_NO_SAVE|IUPAF_NOT_MAPPED);

  /* Overwrite Visual */
  iupClassRegisterAttribute(ic, "ACTIVE", iupBaseGetActiveAttrib, gtkToggleSetActiveAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_DEFAULT);

  /* Visual */
  iupClassRegisterAttribute(ic, "BGCOLOR", NULL, iupdrvBaseSetBgColorAttrib, IUPAF_SAMEASSYSTEM, "DLGBGCOLOR", IUPAF_DEFAULT);

  /* Special */
  iupClassRegisterAttribute(ic, "FGCOLOR", NULL, gtkToggleSetFgColorAttrib, IUPAF_SAMEASSYSTEM, "DLGFGCOLOR", IUPAF_DEFAULT);  /* black */
  iupClassRegisterAttribute(ic, "TITLE", NULL, gtkToggleSetTitleAttrib, NULL, NULL, IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);

  /* IupToggle only */
  iupClassRegisterAttribute(ic, "ALIGNMENT", NULL, gtkToggleSetAlignmentAttrib, "ACENTER:ACENTER", NULL, IUPAF_NO_INHERIT); /* force new default value */
  iupClassRegisterAttribute(ic, "IMAGE", NULL, gtkToggleSetImageAttrib, NULL, NULL, IUPAF_IHANDLENAME|IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "IMINACTIVE", NULL, gtkToggleSetImInactiveAttrib, NULL, NULL, IUPAF_IHANDLENAME|IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "IMPRESS", NULL, gtkToggleSetImPressAttrib, NULL, NULL, IUPAF_IHANDLENAME|IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "VALUE", gtkToggleGetValueAttrib, gtkToggleSetValueAttrib, NULL, NULL, IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "PADDING", iupToggleGetPaddingAttrib, gtkToggleSetPaddingAttrib, IUPAF_SAMEASSYSTEM, "0x0", IUPAF_NOT_MAPPED);
  iupClassRegisterAttribute(ic, "MARKUP", NULL, NULL, NULL, NULL, IUPAF_DEFAULT);

  /* NOT supported */
  iupClassRegisterAttribute(ic, "RIGHTToggle", NULL, NULL, NULL, NULL, IUPAF_NOT_SUPPORTED|IUPAF_DEFAULT);
#endif
	
}
