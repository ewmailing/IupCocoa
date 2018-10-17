/** \file
 * \brief Valuator Control
 *
 * See Copyright Notice in "iup.h"
 */

#import <Cocoa/Cocoa.h>
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
#include "iup_val.h"
#include "iup_drv.h"
#include "iup_drvfont.h"
#include "iup_key.h"

#include "iupcocoa_drv.h"

// the point of this is we have a unique memory address for an identifier
static const void* IUP_COCOA_SLIDER_RECEIVER_OBJ_KEY = "IUP_COCOA_SLIDER_RECEIVER_OBJ_KEY";

@interface IupCocoaSlider : NSSlider
@end

@implementation IupCocoaSlider

/*
I was mistaken about what acceptsFirstMouse does and what CANFOCUS is supposed to do.
This controls whether the widget will immediately respond to a click when the app/window is in the background.
The default is NO.

So for example, Xcode is in the foreground, and my slider app is in the background.
I click on my slider app to bring it forward. I happen to click on the slider in my app.
acceptsFirstMouse==YES would cause both my application to come forward and also move the slider position.
acceptsFirstMouse==NO would cause just the application to come forward, but the slider position will be ignored. This is a safety/focus behavior by default to avoid accidentally changing things.

I suppose a reason to make it YES is if you have a multiple window app, and you want to manipulate the slider in a second window, without losing focus on the main window.

However, this is not what the CANFOCUS feature is about.
CANFOCUS seems to mostly be about getting the focus ring and being able to get keyboard control of it.

*/
/*
- (BOOL) acceptsFirstMouse:(NSEvent*)the_event
{
	Ihandle* ih = (Ihandle*)objc_getAssociatedObject(self, IHANDLE_ASSOCIATED_OBJ_KEY);
  	if(iupAttribGetBoolean(ih, "CANFOCUS"))
  	{
  		return YES;
	}
	else
	{
		return NO;
	}
}
*/

@end

@interface IupCocoaSliderReceiver : NSObject
- (IBAction) mySliderDidMove:(id)the_sender;
@end

@implementation IupCocoaSliderReceiver

/*
- (void) dealloc
{
	[super dealloc];
}
*/


- (IBAction) mySliderDidMove:(id)the_sender;
{
	Icallback callback_function;
	Ihandle* ih = (Ihandle*)objc_getAssociatedObject(the_sender, IHANDLE_ASSOCIATED_OBJ_KEY);

	NSSlider* the_slider = (NSSlider*)the_sender;
	double new_value = [the_slider doubleValue];;
	if(ih->data->val == new_value)
	{
		// no change
		return;
	}
	ih->data->val = new_value;
	
	callback_function = IupGetCallback(ih, "VALUECHANGED_CB");
	if(callback_function)
	{
		int ret_val = callback_function(ih);
		// don't need to do anything with return value
		(void)(ret_val);
	}
}

@end




void iupdrvValGetMinSize(Ihandle* ih, int *w, int *h)
{
#if 1
  if(ih->data->orientation == IVAL_HORIZONTAL)
  {
    *w = 20;
    *h = 35;
  }
  else
  {
    *w = 35;
    *h = 20;
  }
#endif
}

static int cocoaValSetValueAttrib(Ihandle* ih, const char* value)
{
	double new_value = 0;
	if(iupStrToDouble(value, &new_value))
	{
		NSSlider* the_slider = ih->handle;
		
		if(new_value < ih->data->vmin)
		{
			new_value = ih->data->vmin;
		}
		if(new_value > ih->data->vmax)
		{
			new_value = ih->data->vmax;
		}
		
		ih->data->val = new_value;
		
		[the_slider setDoubleValue:new_value];
	}
	return 0; /* do not store value in hash table */
}


static int cocoaValMapMethod(Ihandle* ih)
{
//	char* value;




	IupCocoaSlider* the_slider = [[IupCocoaSlider alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];

	[the_slider setMinValue:0.0];
	[the_slider setMaxValue:1.0];
	// PAGESTEP cannot be supported. Only STEP.
	[the_slider setAltIncrementValue:0.01];
	[the_slider setNumberOfTickMarks:0];


	// I'm using objc_setAssociatedObject/objc_getAssociatedObject because it allows me to avoid making subclasses just to hold ivars.
	objc_setAssociatedObject(the_slider, IHANDLE_ASSOCIATED_OBJ_KEY, (id)ih, OBJC_ASSOCIATION_ASSIGN);
	// I also need to track the memory of the buttion action receiver.
	// I prefer to keep the Ihandle the actual NSView instead of the receiver because it makes the rest of the implementation easier if the handle is always an NSView (or very small set of things, e.g. NSWindow, NSView, CALayer).
	// So with only one pointer to deal with, this means we need our button to hold a reference to the receiver object.
	// This is generally not good Cocoa as buttons don't retain their receivers, but this seems like the best option.
	// Be careful of retain cycles.
	IupCocoaSliderReceiver* slider_receiver = [[IupCocoaSliderReceiver alloc] init];
	[the_slider setTarget:slider_receiver];
	[the_slider setAction:@selector(mySliderDidMove:)];

	// We're going to use OBJC_ASSOCIATION_RETAIN because I do believe it will do the right thing for us.
	// Just be very careful to not have anything in the delegate retain main widget, or we get a retain cycle.
	objc_setAssociatedObject(the_slider, IUP_COCOA_SLIDER_RECEIVER_OBJ_KEY, (id)slider_receiver, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
	[slider_receiver release];
	
	

	

	ih->handle = the_slider;

	
	// All Cocoa views shoud call this to add the new view to the parent view.
	iupCocoaAddToParent(ih);
	

	
	
	
	
	
	return IUP_NOERROR;
}

static void cocoaValUnMapMethod(Ihandle* ih)
{
	id the_slider = ih->handle;

	// Destroy the context menu ih it exists
	{
		Ihandle* context_menu_ih = (Ihandle*)iupCocoaCommonBaseGetContextMenuAttrib(ih);
		if(NULL != context_menu_ih)
		{
			IupDestroy(context_menu_ih);
		}
		iupCocoaCommonBaseSetContextMenuAttrib(ih, NULL);
	}

	iupCocoaRemoveFromParent(ih);
	[the_slider release];
	ih->handle = NULL;
	
}


void iupdrvValInitClass(Iclass* ic)
{
  /* Driver Dependent Class functions */
  ic->Map = cocoaValMapMethod;
  ic->UnMap = cocoaValUnMapMethod;


  /* Driver Dependent Attribute functions */
  
  /* Visual */
  iupClassRegisterAttribute(ic, "BGCOLOR", NULL, iupdrvBaseSetBgColorAttrib, IUPAF_SAMEASSYSTEM, "DLGBGCOLOR", IUPAF_DEFAULT); 

  /* Special */

  /* IupVal only */
  iupClassRegisterAttribute(ic, "VALUE", iupValGetValueAttrib, cocoaValSetValueAttrib, NULL, NULL, IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);
#if 0
  iupClassRegisterAttribute(ic, "PAGESTEP", iupValGetPageStepAttrib, macValSetPageStepAttrib, IUPAF_SAMEASSYSTEM, "0.1", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "STEP", iupValGetStepAttrib, macValSetStepAttrib, IUPAF_SAMEASSYSTEM, "0.01", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SHOWTICKS", NULL, NULL, NULL, NULL, IUPAF_WRITEONLY|IUPAF_READONLY);  /* showticks is not supported in GTK */
#endif

	/* New API for view specific contextual menus (Mac only) */
	iupClassRegisterAttribute(ic, "CONTEXTMENU", iupCocoaCommonBaseGetContextMenuAttrib, iupCocoaCommonBaseSetContextMenuAttrib, NULL, NULL, IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);

}
