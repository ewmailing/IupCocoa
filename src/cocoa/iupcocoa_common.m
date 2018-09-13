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
#include "iup_loop.h"
#include "iup_drv.h"

#include "iupcocoa_drv.h"


#if __has_feature(objc_arc)
#error "Cocoa backend for IUP does not use ARC. Compile with -fno-objc-arc"
#endif


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
		NSAutoresizingMaskOptions current_mask = [the_view autoresizingMask];
		current_mask = current_mask & (NSViewWidthSizable | NSViewHeightSizable); // Preserve Sizable flags if set
		[the_view setAutoresizingMask:current_mask|NSViewMaxXMargin|NSViewMinYMargin];
//		[the_view setAutoresizingMask:NSViewMaxXMargin|NSViewMinYMargin];

		
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

void iupCocoaCommonLoopCallExitCb()
{
	static BOOL has_called_exit_cb = NO;
	if(NO == has_called_exit_cb)
	{
		has_called_exit_cb = YES;
		iupLoopCallExitCb();
	}
	else
	{
		NSLog(@"Warning: iupCocoaCommonLoopCallExitCb has been called too many times");
	}
}


void iupdrvActivate(Ihandle* ih)
{

}

void iupdrvReparent(Ihandle* ih)
{

	
}


// If you change these, make sure to update any overrides that copy/paste this pattern (e.g. cocoaButtonLayoutUpdateMethod)
// TODO: Consider CALayer
NSView* iupCocoaCommonBaseLayoutGetParentView(Ihandle* ih)
{
	id parent_native_handle = iupChildTreeGetNativeParentHandle(ih);
	NSView* parent_view = nil;
	
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
	
	return parent_view;
}

// If you change these, make sure to update any overrides that copy/paste this pattern (e.g. cocoaButtonLayoutUpdateMethod)
// TODO: Consider CALayer
NSView* iupCocoaCommonBaseLayoutGetChildView(Ihandle* ih)
{
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
		NSCAssert(1, @"Unexpected type for child widget");
		@throw @"Unexpected type for child widget";
	}
	return the_view;
}

// If you change these, make sure to update any overrides that copy/paste this pattern (e.g. cocoaButtonLayoutUpdateMethod)
NSRect iupCocoaCommonBaseLayoutComputeChildFrameRectFromParentRect(Ihandle* ih, NSRect parent_rect)
{

	NSRect the_rect = NSMakeRect(
		ih->x,
		// Need to invert y-axis, and also need to shift/account for height of widget because that is also lower-left instead of upper-left.
		parent_rect.size.height - ih->y - ih->currentheight,
		ih->currentwidth,
		ih->currentheight
	);
//	the_rect.origin.y = the_rect.origin.y - 16.0*0.5; // I don't realy understand the logic of this offset, particularly the -1


	// for padding
	// drat, data is private and requires a per-widget header
	{

		char* padding_str = iupAttribGet(ih, "PADDING");
		if((NULL != padding_str) && (padding_str[0] != '\0'))
		{
			int horiz_padding = 0;
			int vert_padding = 0;
			iupStrToIntInt(padding_str, &horiz_padding, &vert_padding, 'x');
			
			NSRect old_frame = the_rect;
			NSRect new_frame;
			new_frame.origin.x = old_frame.origin.x + (CGFloat)horiz_padding*0.5;
			new_frame.origin.y = old_frame.origin.y + (CGFloat)vert_padding*0.5;
			new_frame.size.width = old_frame.size.width - (CGFloat)horiz_padding;
			new_frame.size.height = old_frame.size.height - (CGFloat)vert_padding;
			
			the_rect = new_frame;
		}
		
	}

//	NSLog(@"view %@, rect:%@", the_view, NSStringFromRect(the_rect));
	return the_rect;

}

// If you change these, make sure to update any overrides that copy/paste this pattern (e.g. cocoaButtonLayoutUpdateMethod)
void iupdrvBaseLayoutUpdateMethod(Ihandle *ih)
{

	NSView* parent_view = nil;
	NSView* child_view = nil;
	
	parent_view = iupCocoaCommonBaseLayoutGetParentView(ih);
	child_view = iupCocoaCommonBaseLayoutGetChildView(ih);
	
	
	
	
	NSRect parent_rect = [parent_view frame];

	
/*
#if 0 // experiment to try to handle NSBox directly instead of using cocoaFrameGetInnerNativeContainerHandleMethod. I think cocoaFrameGetInnerNativeContainerHandleMethod is better, but I haven't vetted everything.
	int current_width = ih->currentwidth;
	int current_height = ih->currentheight;

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
//	the_rect.origin.y = the_rect.origin.y - 16.0*0.5; // I don't realy understand the logic of this offset, particularly the -1


	// for padding
	// drat, data is private and requires a per-widget header
	{

		char* padding_str = iupAttribGet(ih, "PADDING");
		if((NULL != padding_str) && (padding_str[0] != '\0'))
		{
			int horiz_padding = 0;
			int vert_padding = 0;
			iupStrToIntInt(padding_str, &horiz_padding, &vert_padding, 'x');
			
			NSRect old_frame = the_rect;
			NSRect new_frame;
			new_frame.origin.x = old_frame.origin.x + (CGFloat)horiz_padding*0.5;
			new_frame.origin.y = old_frame.origin.y + (CGFloat)vert_padding*0.5;
			new_frame.size.width = old_frame.size.width - (CGFloat)horiz_padding;
			new_frame.size.height = old_frame.size.height - (CGFloat)vert_padding;
			
			the_rect = new_frame;
		}
		
	}

//	NSLog(@"view %@, rect:%@", the_view, NSStringFromRect(the_rect));
	[child_view setFrame:the_rect];
#endif
*/
	
	NSRect child_rect = iupCocoaCommonBaseLayoutComputeChildFrameRectFromParentRect(ih, parent_rect);
	[child_view setFrame:child_rect];
	

	
}

void iupdrvBaseUnMapMethod(Ihandle* ih)
{
	// Why do I need this when everything else has its own UnMap method?
	NSLog(@"iupdrvBaseUnMapMethod not implemented. Might be leaking");
	id the_handle = ih->handle;
	[the_handle release];
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
		CALayer* the_layer = (CALayer*)the_handle;
		[the_layer setNeedsDisplay];
	}
	else
	{
		NSCAssert(1, @"Unexpected type for widget");
		@throw @"Unexpected type for widget";
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
		NSWindow* the_window = (NSWindow*)ih->handle;
		int ret_val = (int)[the_window isVisible];
		return ret_val;
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
	id the_object = ih->handle;
	if([the_object isKindOfClass:[NSControl class]])
	{
		NSControl* the_control = (NSControl*)the_object;
		return [the_control isEnabled];
	}
	return 1;
}

void iupdrvSetActive(Ihandle* ih, int enable)
{
	id the_object = ih->handle;
	if([the_object isKindOfClass:[NSControl class]])
	{
		NSControl* the_control = (NSControl*)the_object;
		[the_control setEnabled:enable];
	}
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
	iupdrvDisplayUpdate(ih);
}

void iupdrvRedrawNow(Ihandle *ih)
{
	iupdrvDisplayUpdate(ih);
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

// This will copy all the menu items provided by src_menu and append them into dst_menu with a separator.
void iupCocoaCommonBaseAppendMenuItems(NSMenu* dst_menu, NSMenu* src_menu)
{
	if((src_menu != nil) && ([src_menu numberOfItems] > 0))
	{
		// Add a separator to separate the user's items from the default items
		[dst_menu addItem:[NSMenuItem separatorItem]];
		
		NSArray<NSMenuItem*>* item_array = [src_menu itemArray];
		for(NSMenuItem* a_default_item in item_array)
		{
			// We have to copy the item otherwise Cocoa will complain that the same menu item is used in multiple places.
			NSMenuItem* item_copy = [a_default_item copy];
			[dst_menu addItem:item_copy];
			[item_copy release];
		}
	}
}

// This will copy all the menu items provided by the class's defaultMenu and append them into dst_menu with a separator.
void iupCocoaCommonBaseAppendDefaultMenuItemsForClassType(NSMenu* dst_menu, Class class_of_widget)
{
	// If the class provides a defaultMenu, we should merge those items with our menu
	if([class_of_widget respondsToSelector:@selector(defaultMenu)])
	{
		NSMenu* default_menu = [class_of_widget defaultMenu];
		iupCocoaCommonBaseAppendMenuItems(dst_menu, default_menu);
	}
}

// Because we often have container views wrapping our core objects (e.g. scrollview wraps canvas, stackview wraps NSTextField)
// this helper function lets us split out the ih from the widget, so we don't have to assume the widget is ih->handle.
// So provide the ih, and provide the real core widget that provides [NSResponder setMenu:] that we should set.
// The menu should be in menu_ih.
void iupCocoaCommonBaseSetContextMenuForWidget(Ihandle* ih, id ih_widget_to_attach_menu_to, Ihandle* menu_ih)
{
	// Save the menu Ihandle in this widget's Ihandle so we can GetContextMenuAttrib
	iupAttribSet(ih, "_COCOA_CONTEXT_MENU_IH", (const char*)menu_ih);


	// Unset the existing menu
	if(NULL == menu_ih)
	{
		if([ih_widget_to_attach_menu_to respondsToSelector:@selector(setMenu:)])
		{
			[ih_widget_to_attach_menu_to setMenu:nil];
		}
		return;
	}

	// FIXME: The Menu might not be IupMap'd yet. (Presumably because we do not attach it directly to a dialog in this case.)
	// I think calling IupMap() is the correct thing to do and fixes the problem.
	// But this should be reviewed.
	if(NULL == menu_ih->handle)
	{
		IupMap(menu_ih);
	}
	
	// Make sure we have an IupMenu
	if(menu_ih->iclass->nativetype != IUP_TYPEMENU)
	{
		// call IUPASSERT?
		return;
	}
	// Make sure we have a NSMenu
	if(![(id)menu_ih->handle isKindOfClass:[NSMenu class]])
	{
		// call IUPASSERT?
		return;
	}


	NSMenu* the_menu = (NSMenu*)menu_ih->handle;
	if([ih_widget_to_attach_menu_to respondsToSelector:@selector(setMenu:)])
	{
		iupCocoaCommonBaseAppendDefaultMenuItemsForClassType(the_menu, [ih_widget_to_attach_menu_to class]);
		[ih_widget_to_attach_menu_to setMenu:the_menu];
	}

}


int iupCocoaCommonBaseIupButtonForCocoaButton(NSInteger which_cocoa_button)
{
	if(0 == which_cocoa_button)
	{
		return IUP_BUTTON1;
	}
	else if(1 == which_cocoa_button) // right
	{
		return IUP_BUTTON3;
	}
	else if(2 == which_cocoa_button) // middle
	{
		return IUP_BUTTON2;
	}
	else
	{
		// NOTE: IUP_BUTTON are ASCII values.
		return (int)(which_cocoa_button + 0x30);
	}
}

void iupCocoaCommonBaseHandleMouseButtonCallback(Ihandle* ih, NSEvent* the_event, NSView* represented_view, bool is_pressed)
{
	IFniiiis callback_function;


	callback_function = (IFniiiis)IupGetCallback(ih, "BUTTON_CB");
	if(callback_function)
	{
	    // We must convert the mouse event locations from the window coordinate system to the
		// local view coordinate system.
		NSPoint the_point = [the_event locationInWindow];
		NSPoint converted_point = [represented_view convertPoint:the_point fromView:nil];

		// We must flip the y to go from Cartesian to IUP
		NSRect view_frame = [represented_view frame];
		CGFloat inverted_y = view_frame.size.height - converted_point.y;

		// Button 0 is left
		// Button 1 is right
		// Button 2 is middle
		// Button 3 keeps going
		NSInteger which_cocoa_button = [the_event buttonNumber];
		char mod_status = 0; // FIXME: Implement this!

		if([the_event modifierFlags] & NSControlKeyMask)
		{
			// Should Ctrl-Left-click be a right click?
			if(0 == which_cocoa_button)
			{
				which_cocoa_button = 1; // make right button
			}
		}
		else if([the_event modifierFlags] & NSAlternateKeyMask)
		{

		}
		else if([the_event modifierFlags] & NSCommandKeyMask)
		{
		}
		else
		{
		}
		
		int which_iup_button = iupCocoaCommonBaseIupButtonForCocoaButton(which_cocoa_button);
	
		NSLog(@"Iup mouse button callback: <x,y>=<%f, %f, %f>, is_pressed=%d, button_num:%d", converted_point.x, converted_point.y, inverted_y, is_pressed, which_iup_button);

	
		int callback_result = callback_function(ih, which_iup_button, is_pressed, iupROUND(converted_point.x), iupROUND(inverted_y), &mod_status);
		if(IUP_CLOSE == callback_result)
		{
			IupExitLoop();
		}
	}
}
void iupCocoaCommonBaseHandleMouseMotionCallback(Ihandle* ih, NSEvent* the_event, NSView* represented_view)
{
	IFniis callback_function;
	callback_function = (IFniis)IupGetCallback(ih, "MOTION_CB");
	if(callback_function)
	{
	    // We must convert the mouse event locations from the window coordinate system to the
		// local view coordinate system.
		NSPoint the_point = [the_event locationInWindow];
		NSPoint converted_point = [represented_view convertPoint:the_point fromView:nil];
		
		// We must flip the y to go from Cartesian to IUP
		NSRect view_frame = [represented_view frame];
		CGFloat inverted_y = view_frame.size.height - converted_point.y;
		char mod_status = 0; // FIXME: Implement this!

	
		int callback_result = callback_function(ih,  iupROUND(converted_point.x), iupROUND(inverted_y), &mod_status);
		if(IUP_CLOSE == callback_result)
		{
			IupExitLoop();
		}
	}
}


int iupCocoaCommonBaseSetContextMenuAttrib(Ihandle* ih, const char* value)
{
	Ihandle* menu_ih = (Ihandle*)value;
	id ih_widget_to_attach_menu_to = ih->handle;
	iupCocoaCommonBaseSetContextMenuForWidget(ih, ih_widget_to_attach_menu_to, menu_ih);
	
	return 1;
}

char* iupCocoaCommonBaseGetContextMenuAttrib(Ihandle* ih)
{
	return (char*)iupAttribGet(ih, "_COCOA_CONTEXT_MENU_IH");
}


