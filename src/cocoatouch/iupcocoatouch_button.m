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

#if 1
// the point of this is we have a unique memory address for an identifier
static const void* IUP_COCOATOUCH_BUTTON_RECEIVER_OBJ_KEY = "IUP_COCOATOUCH_BUTTON_RECEIVER_OBJ_KEY";


@interface IupCocoaTouchButtonReceiver : NSObject
- (IBAction) myButtonTouchAction:(id)the_sender;
@end

@implementation IupCocoaTouchButtonReceiver

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

	// CONFLICT: cocoaTouch buttons don't normally do anything for non-primary click. (Second click is supposed to trigger the contextual menu.)
	// Also cocoaTouch doesn't normall give callbacks for both down and up
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

static int cocoaTouchButtonSetTitleAttrib(Ihandle* ih, const char* value)
{
	UIButton *the_button = ih->handle;
	if([the_button respondsToSelector:@selector(titleLabel)])
	{
		NSString* ns_string = nil;
		if(value)
		{
			// This will return nil if the string can't be converted.
			ns_string = [NSString stringWithUTF8String:value];
		}
		// It's OK if ns_string is nil.
		[the_button.titleLabel setText:ns_string];
		[the_button sizeToFit];
	}
	return 1;

}


static int cocoaTouchButtonMapMethod(Ihandle* ih)
{
#if 1
	char* value;
	int impress;

	UIButton* the_button = [UIButton buttonWithType:UIButtonTypeSystem];

	
	

	

#if 0
	value = iupAttribGet(ih, "IMAGE");
	if(value && *value!=0)
	{
		
		ih->data->type |= IUP_BUTTON_IMAGE;
		
		
		// If there is supposed to be both text and an image, we use setBackgroundImage.
		// If just an image, we could use setImage
		
		
		
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

	
#if 1
	value = iupAttribGet(ih, "TITLE");
	if(value && *value!=0)
	{
		ih->data->type |= IUP_BUTTON_TEXT;
		NSString* ns_string = [NSString stringWithUTF8String:value];
		
		[the_button setTitle:ns_string forState:UIControlStateNormal];

			UIFont* the_font = nil;
		// FIXME: Use font set by user
		
		the_font = [UIFont systemFontOfSize:[UIFont labelFontSize]];
		
		
		CGSize size_bounds = [[UIScreen mainScreen] bounds].size;

		CGRect bounding_rect = [ns_string boundingRectWithSize:size_bounds
			options:NSStringDrawingUsesLineFragmentOrigin
			attributes:@{NSFontAttributeName:the_font}
			context:nil
		];
		CGSize fit_size = [the_button sizeThatFits:bounding_rect.size];
		CGRect fit_rect = [the_button frame];
		fit_rect.size = fit_size;
		[the_button setFrame:fit_rect];
	}
#endif
	
	// This is making the button too small. Why???
//	[the_button sizeToFit];

	
	
	
	
	ih->handle = [the_button retain];

#if 1
	// I'm using objc_setAssociatedObject/objc_getAssociatedObject because it allows me to avoid making subclasses just to hold ivars.
	objc_setAssociatedObject(the_button, IHANDLE_ASSOCIATED_OBJ_KEY, (id)ih, OBJC_ASSOCIATION_ASSIGN);
	// I also need to track the memory of the buttion action receiver.
	// I prefer to keep the Ihandle the actual NSView instead of the receiver because it makes the rest of the implementation easier if the handle is always an NSView (or very small set of things, e.g. NSWindow, NSView, CALayer).
	// So with only one pointer to deal with, this means we need our button to hold a reference to the receiver object.
	// This is generally not good cocoaTouch as buttons don't retain their receivers, but this seems like the best option.
	// Be careful of retain cycles.
	IupCocoaTouchButtonReceiver* button_receiver = [[IupCocoaTouchButtonReceiver alloc] init];
	// I *think* is we use RETAIN, the object will be released automatically when the button is freed.
	objc_setAssociatedObject(the_button, IUP_COCOATOUCH_BUTTON_RECEIVER_OBJ_KEY, (id)button_receiver, OBJC_ASSOCIATION_RETAIN);
	[button_receiver autorelease];
	
	[the_button addTarget:button_receiver action:@selector(myButtonClickAction:) forControlEvents:UIControlEventTouchUpInside];

#endif

	

	
	
	// All cocoaTouch views should call this to add the new view to the parent view.
	iupCocoaTouchAddToParent(ih);

	
#endif
	
	

	
	
//	gtk_widget_realize(ih->handle);
	
	/* update a mnemonic in a label if necessary */
//	iupgtkUpdateMnemonic(ih);
	

	return IUP_NOERROR;
}

static void cocoaTouchButtonUnMapMethod(Ihandle* ih)
{
#if 1
	id the_button = ih->handle;

/*
	id butten_receiver = objc_getAssociatedObject(the_button, IUP_cocoaTouch_BUTTON_RECEIVER_OBJ_KEY);
	objc_setAssociatedObject(the_button, IUP_cocoaTouch_BUTTON_RECEIVER_OBJ_KEY, nil, OBJC_ASSOCIATION_ASSIGN);
	[butten_receiver release];
	
	[the_button release];
*/
	iupCocoaTouchRemoveFromParent(ih);
	[the_button release];

	ih->handle = NULL;
#endif
}

void iupdrvButtonAddBorders(Ihandle* ih, int *x, int *y)
{

	
}

static int cocoaButtonSetFgColorAttrib(Ihandle* ih, char *iColor)
{
	UIColor *color = iupCocoaTouchToNativeColor(iColor);
	if (color)
	{
		UIButton* the_button = ih->handle;
		[the_button.titleLabel setTextColor:color];
		return IUP_NOERROR;
	}
	return IUP_ERROR;
}


static char* cocoaButtonGetFGColorAttrib(Ihandle* ih)
{
	UIButton* the_button = ih->handle;
	UIColor *textColor = [the_button.titleLabel textColor];
	return iupCocoaTouchColorFromNative(textColor);
}


void iupdrvButtonInitClass(Iclass* ic)
{
	/* Driver Dependent Class functions */
	ic->Map = cocoaTouchButtonMapMethod;
	ic->UnMap = cocoaTouchButtonUnMapMethod;
	
#if 0

	ic->LayoutUpdate = gtkButtonLayoutUpdateMethod;
	
	/* Driver Dependent Attribute functions */
	
	/* Overwrite Common */
	iupClassRegisterAttribute(ic, "STANDARDFONT", NULL, gtkButtonSetStandardFontAttrib, IUPAF_SAMEASSYSTEM, "DEFAULTFONT", IUPAF_NO_SAVE|IUPAF_NOT_MAPPED);
#endif
	
	/* Overwrite Visual */
	iupClassRegisterAttribute(ic, "ACTIVE", iupBaseGetActiveAttrib, iupCocoaTouchSetActiveAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_DEFAULT);
	/* Visual */
	iupClassRegisterAttribute(ic, "BGCOLOR", iupBaseNativeParentGetBgColorAttrib, iupCocoaTouchSetBgColorAttrib, IUPAF_SAMEASSYSTEM, "DLGBGCOLOR", IUPAF_DEFAULT);
	
	/* Special */
	iupClassRegisterAttribute(ic, "FGCOLOR", cocoaButtonGetFGColorAttrib, cocoaButtonSetFgColorAttrib, IUPAF_SAMEASSYSTEM, "DLGFGCOLOR", IUPAF_DEFAULT);
	iupClassRegisterAttribute(ic, "TITLE", NULL, cocoaTouchButtonSetTitleAttrib, NULL, NULL, IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);
#if 0
	/* IupButton only */
	iupClassRegisterAttribute(ic, "ALIGNMENT", NULL, gtkButtonSetAlignmentAttrib, "ACENTER:ACENTER", NULL, IUPAF_NO_INHERIT);  /* force new default value */
	iupClassRegisterAttribute(ic, "IMAGE", NULL, gtkButtonSetImageAttrib, NULL, NULL, IUPAF_IHANDLENAME|IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);
	iupClassRegisterAttribute(ic, "IMINACTIVE", NULL, gtkButtonSetImInactiveAttrib, NULL, NULL, IUPAF_IHANDLENAME|IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);
	iupClassRegisterAttribute(ic, "IMPRESS", NULL, NULL, NULL, NULL, IUPAF_IHANDLENAME|IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);
	
	iupClassRegisterAttribute(ic, "PADDING", iupButtonGetPaddingAttrib, gtkButtonSetPaddingAttrib, IUPAF_SAMEASSYSTEM, "0x0", IUPAF_NOT_MAPPED);
	iupClassRegisterAttribute(ic, "MARKUP", NULL, NULL, NULL, NULL, IUPAF_DEFAULT);
#endif
	
}
