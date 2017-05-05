/** \file
 * \brief GTK Base Functions
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdio.h>              
#include <stdlib.h>
#include <string.h>             
#include <limits.h>             

#import <Cocoa/Cocoa.h>

#include "iup.h"
#include "iupcbs.h"
#include "iupkey.h"

#include "iup_object.h"
#include "iup_childtree.h"
#include "iup_key.h"
#include "iup_str.h"
#include "iup_class.h"
#include "iup_attrib.h"
#include "iup_focus.h"
#include "iup_key.h"
#include "iup_image.h"
#include "iup_drv.h"

#include "iupcocoa_drv.h"

const void* IHANDLE_ASSOCIATED_OBJ_KEY = @"IHANDLE_ASSOCIATED_OBJ_KEY"; // the point of this is we have a unique memory address for an identifier


void iupCocoaAddToParent(Ihandle* ih)
{
	id parent_native_handle = iupChildTreeGetNativeParentHandle(ih);
	
	id child_handle = ih->handle;
	if([child_handle isKindOfClass:[NSView class]])
	{
		NSView* the_view = (NSView*)child_handle;
		
		// From David Phillip Oster:
		/* 
		 now, when you resize the window, you see the hidden content. This makes the subviews preserve their relative x,y offset from the top left of the window, instead of the Mac default of the bottom left. It probably isn't the right way to do it, since there's probably some iup property that is specifying somethign more complex.
		 
		 
		 If I had set [the_view setAutoresizingMask:NSViewMinXMargin|NSViewWidthSizable|NSViewMaxXMargin];
		 
		 Then, as the window widens, that view widens along with it.
		 */
		[the_view setAutoresizingMask:NSViewMaxXMargin|NSViewMinYMargin];

		
		if([parent_native_handle isKindOfClass:[NSWindow class]])
		{
			NSWindow* parent_window = (NSWindow*)parent_native_handle;
			NSView* window_view = [parent_window contentView];
			[window_view addSubview:the_view];
		}
		/*
		else if([parent_native_handle isKindOfClass:[NSBox class]])
		{
			NSBox* parent_view = (NSBox*)parent_native_handle;
			[[parent_view contentView] addSubview:the_view];

		}
		 */
		else if([parent_native_handle isKindOfClass:[NSView class]])
		{
			NSView* parent_view = (NSView*)parent_native_handle;
			
			[parent_view addSubview:the_view];
		}
		else
		{
			NSCAssert(1, @"Unexpected type for parent widget");
			@throw @"Unexpected type for parent widget";
		}
	}
	else if([child_handle isKindOfClass:[CALayer class]])
	{
		NSCAssert(1, @"CALayer not implemented");
		@throw @"CALayer not implemented";
	}
	else
	{
		NSCAssert(1, @"Unexpected type for parent widget");
		@throw @"Unexpected type for parent widget";
	}
	
}

void iupCocoaRemoveFromParent(Ihandle* ih)
{
	
	id child_handle = ih->handle;
	if([child_handle isKindOfClass:[NSView class]])
	{
		NSView* the_view = (NSView*)child_handle;
		[the_view removeFromSuperview];
	}
	else if([child_handle isKindOfClass:[CALayer class]])
	{
		CALayer* the_layer = (CALayer*)child_handle;
		[the_layer removeFromSuperlayer];
	}
	else
	{
		NSCAssert(1, @"Unexpected type for widget");
		@throw @"Unexpected type for widget";
	}
}

int iupCocoaComputeCartesianScreenHeightFromIup(int iup_height)
{
	// Do I want the full screen height or the visible height?
	NSRect screen_rect = [[NSScreen mainScreen] frame];
//	NSRect screen_rect = [[NSScreen mainScreen] visibleFrame];
	CGFloat inverted_height = screen_rect.size.height - iup_height;
	return iupROUND(inverted_height);
}

int iupCocoaComputeIupScreenHeightFromCartesian(int cartesian_height)
{
	// Do I want the full screen height or the visible height?
	NSRect screen_rect = [[NSScreen mainScreen] frame];
//	NSRect screen_rect = [[NSScreen mainScreen] visibleFrame];
	CGFloat inverted_height = screen_rect.size.height - cartesian_height;
	return iupROUND(inverted_height);
}



void iupdrvActivate(Ihandle* ih)
{

}

void iupdrvReparent(Ihandle* ih)
{

	
}


void iupdrvBaseLayoutUpdateMethod(Ihandle *ih)
{

	id parent_native_handle = iupChildTreeGetNativeParentHandle(ih);
	NSView* parent_view = nil;
	
	int current_width = ih->currentwidth;
	int current_height = ih->currentheight;
	
	if([parent_native_handle isKindOfClass:[NSWindow class]])
	{
		NSWindow* parent_window = (NSWindow*)parent_native_handle;
		parent_view = [parent_window contentView];
	}
/* // Part of NSBox experiment
	else if([parent_native_handle isKindOfClass:[NSBox class]])
	{
		NSBox* box_view = (NSBox*)parent_native_handle;
		parent_view = [box_view contentView];
		
		CGFloat diff_width = NSWidth([box_view frame]) - NSWidth([parent_view frame]);
		CGFloat diff_height = NSHeight([box_view frame]) - NSHeight([parent_view frame]);

		current_width = current_width - diff_width;
		current_height = current_height - diff_height;
	}
*/
	else if([parent_native_handle isKindOfClass:[NSView class]])
	{
		parent_view = (NSView*)parent_native_handle;
	}
	else
	{
		NSCAssert(1, @"Unexpected type for parent widget");
		@throw @"Unexpected type for parent widget";
	}
	
	
	
	id child_handle = ih->handle;
	NSView* the_view = nil;
	if([child_handle isKindOfClass:[NSView class]])
	{
		the_view = (NSView*)child_handle;
	}
	else if([child_handle isKindOfClass:[CALayer class]])
	{
		NSCAssert(1, @"CALayer not implemented");
		@throw @"CALayer not implemented";
	}
	else
	{
		NSCAssert(1, @"Unexpected type for parent widget");
		@throw @"Unexpected type for parent widget";
	}
	
	
//	iupgtkNativeContainerMove((GtkWidget*)parent, widget, x, y);

//	iupgtkSetPosSize(GTK_CONTAINER(parent), widget, ih->x, ih->y, ih->currentwidth, ih->currentheight);

	/*
	CGSize fitting_size = [the_view fittingSize];
	ih->currentwidth = fitting_size.width;
	ih->currentheight = fitting_size.height;
*/
	
	NSRect parent_rect = [parent_view frame];

#if 0 // experiment to try to handle NSBox directly instead of using cocoaFrameGetInnerNativeContainerHandleMethod. I think cocoaFrameGetInnerNativeContainerHandleMethod is better, but I haven't vetted everything.
	if([parent_native_handle isKindOfClass:[NSBox class]])
	{
		NSBox* box_view = (NSBox*)parent_native_handle;
		NSView* box_content_view = [box_view contentView];
		
		CGFloat diff_width = NSWidth([box_view frame]) - NSWidth([box_content_view frame]);
		CGFloat diff_height = NSHeight([box_view frame]) - NSHeight([box_content_view frame]);

		current_width = current_width - diff_width;
		current_height = current_height - diff_height;
		
		NSRect the_rect = NSMakeRect(
		ih->x,
		// Need to invert y-axis, and also need to shift/account for height of widget because that is also lower-left instead of upper-left.
		parent_rect.size.height - ih->y - ih->currentheight,
		current_width,
		ih->currentheight
	);
	[the_view setFrame:the_rect];
//	[the_view setBounds:the_rect];
	}
	else
	{
		
	NSRect the_rect = NSMakeRect(
		ih->x,
		// Need to invert y-axis, and also need to shift/account for height of widget because that is also lower-left instead of upper-left.
		parent_rect.size.height - ih->y - ih->currentheight,
		ih->currentwidth,
		ih->currentheight
	);
	[the_view setFrame:the_rect];
//	[the_view setBounds:the_rect];
	}
#else
	
			
	NSRect the_rect = NSMakeRect(
		ih->x,
		// Need to invert y-axis, and also need to shift/account for height of widget because that is also lower-left instead of upper-left.
		parent_rect.size.height - ih->y - ih->currentheight,
		ih->currentwidth,
		ih->currentheight
	);
	[the_view setFrame:the_rect];
#endif
	
	
}

void iupdrvBaseUnMapMethod(Ihandle* ih)
{
	// Why do I need this when everything else has its own UnMap method?
	NSLog(@"iupdrvBaseUnMapMethod not implemented. Might be leaking");
	[ih->handle release];
}

void iupdrvDisplayUpdate(Ihandle *ih)
{
	id the_handle = ih->handle;
	
	if([the_handle isKindOfClass:[NSView class]])
	{
		NSView* the_view = (NSView*)the_handle;
		[the_view setNeedsDisplay:YES];
	}
	else if([the_handle isKindOfClass:[NSWindow class]])
	{
		// Cocoa generally does this automatically
//		[the_handle display];
	}
	else if([the_handle isKindOfClass:[CALayer class]])
	{
		NSCAssert(1, @"CALayer not implemented");
		@throw @"CALayer not implemented";
	}
	else
	{
		NSCAssert(1, @"Unexpected type for parent widget");
		@throw @"Unexpected type for parent widget";
	}

}

void iupdrvDisplayRedraw(Ihandle *ih)
{
	iupdrvDisplayUpdate(ih);
}

void iupdrvScreenToClient(Ihandle* ih, int *x, int *y)
{
}



int iupdrvBaseSetZorderAttrib(Ihandle* ih, const char* value)
{
  return 0;
}

void iupdrvSetVisible(Ihandle* ih, int visible)
{
	id the_object = ih->handle;
	if([the_object isKindOfClass:[NSWindow class]])
	{
		// NOT IMPLEMENTED

	}
	else if([the_object isKindOfClass:[NSView class]])
	{
		NSView* the_view = (NSView*)the_object;
		bool is_hidden = !(bool)visible;
		[the_view setHidden:is_hidden];
	}
	
	
}

int iupdrvIsVisible(Ihandle* ih)
{
	id the_object = ih->handle;
	if([the_object isKindOfClass:[NSWindow class]])
	{
		// NOT IMPLEMENTED
		return 1;
	}
	else if([the_object isKindOfClass:[NSView class]])
	{
		NSView* the_view = (NSView*)the_object;
		return [the_view isHidden];
	}
	else
	{
		return 1;
	}
}

int iupdrvIsActive(Ihandle *ih)
{
  return 1;
}

void iupdrvSetActive(Ihandle* ih, int enable)
{
}

char* iupdrvBaseGetXAttrib(Ihandle *ih)
{
  return NULL;
}

char* iupdrvBaseGetYAttrib(Ihandle *ih)
{

  return NULL;
}

/*
char* iupdrvBaseGetClientSizeAttrib(Ihandle *ih)
{

    return NULL;

}
 */

int iupdrvBaseSetBgColorAttrib(Ihandle* ih, const char* value)
{

	

  /* DO NOT NEED TO UPDATE GTK IMAGES SINCE THEY DO NOT DEPEND ON BGCOLOR */

  return 1;
}

int iupdrvBaseSetCursorAttrib(Ihandle* ih, const char* value)
{

  return 0;
}


int iupdrvGetScrollbarSize(void)
{

  return 0;
}


void iupdrvBaseRegisterCommonAttrib(Iclass* ic)
{
	/*
#ifndef GTK_MAC
  #ifdef WIN32                                 
    iupClassRegisterAttribute(ic, "HFONT", iupgtkGetFontIdAttrib, NULL, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT|IUPAF_NO_STRING);
  #else
    iupClassRegisterAttribute(ic, "XFONTID", iupgtkGetFontIdAttrib, NULL, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT|IUPAF_NO_STRING);
  #endif
#endif
  iupClassRegisterAttribute(ic, "PANGOFONTDESC", iupgtkGetPangoFontDescAttrib, NULL, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT|IUPAF_NO_STRING);
*/
}

void iupdrvBaseRegisterVisualAttrib(Iclass* ic)
{
	
}

void iupdrvClientToScreen(Ihandle* ih, int *x, int *y)
{
	
}

void iupdrvPostRedraw(Ihandle *ih)
{

}

void iupdrvRedrawNow(Ihandle *ih)
{

}
void iupdrvSendKey(int key, int press)
{
	
}
void iupdrvSendMouse(int x, int y, int bt, int status)
{
	
}
void iupdrvSleep(int time)
{
	
}
void iupdrvWarpPointer(int x, int y)
{
	
}
