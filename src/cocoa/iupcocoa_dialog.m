/** \file
 * \brief IupDialog class
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

#include "iupcocoa_drv.h"

/*
@interface NSWindow () 
@property(readwrite, unsafe_unretained) Ihandle* iupIhandle;
@end

@implementation NSWindow
@synthesize iupIhandle = _iupIhandle;
@end
 */
@interface IupCocoaWindowDelegate : NSObject <NSWindowDelegate>
- (BOOL) windowShouldClose:(id)the_sender;
- (NSSize) windowWillResize:(NSWindow*)the_sender toSize:(NSSize)frame_size;
@end

static void cocoaCleanUpWindow(Ihandle* ih)
{
//	NSLog(@"cocoaCleanUpWindow");
	NSWindow* the_window = (__bridge NSWindow*)ih->handle;
	[the_window close];
	// Expecting windowWillClose to run immediately here
	
	IupCocoaWindowDelegate* window_delegate = [the_window delegate];
	[the_window setDelegate:nil];
	[window_delegate release];
	
	[the_window release];
}

static void cocoaDialogRunModalLoop(Ihandle* ih, NSWindow* the_window)
{
	// Gotcha: beginModalSessionForWindow sets [NSApp isRunning] to 1
	NSModalSession the_session = [[NSApplication sharedApplication] beginModalSessionForWindow:the_window];
	
	iupAttribSet(ih, "_COCOA_MODAL", "YES");
	iupAttribSet(ih, "_COCOA_MODAL_SESSION", (const char*)the_session);
	
	
	for(;;)
	{
		if([NSApp runModalSession:the_session] != NSModalResponseContinue)
		{
			break;
		}
	}
	
	// Normally, we would call endModalSession here, but this doesn't seem to quite work for us.
	// But there seems to be an ordering problem.
	// Calling IupDestroy to close the modal window goes through the entire teardown process before this loop can return in the next pump.
	// This seems to be causing bugs where the window doesn't fully get destroyed and a ghost window is stuck on the screen sometimes.
	// The workaround seems to be to call endModalSession directly next to where we call stopModal.
	// I think the teardown then will work because the window is no longer modal.
	//	[NSApp endModalSession:the_session];
	// UPDATE: I now believe the ghost/stuck windows are because of not using the [NSApp run] (real runloop). I now have a special case to handle this.

	iupAttribSet(ih, "_COCOA_MODAL", "NO");
	iupAttribSet(ih, "_COCOA_MODAL_SESSION", NULL);
}



@interface IupNonRunLoopModalAppDelegate : NSObject <NSApplicationDelegate>
{
	Ihandle* _ih;
}
- (instancetype) initWithIhandle:(Ihandle*)ih;
- (Ihandle*) ih;
@end



@implementation IupNonRunLoopModalAppDelegate

- (instancetype) initWithIhandle:(Ihandle*)ih
{
	self = [super init];
	if(nil == self)
	{
		return nil;
	}
	_ih = ih;
	return self;
}

- (Ihandle*) ih
{
	return _ih;
}

- (void) applicationDidFinishLaunching:(NSNotification*)a_notification
{
	Ihandle* ih = _ih;
	
	NSWindow* the_window = (NSWindow*)ih->handle;

	// This blocks until done.
	cocoaDialogRunModalLoop(ih, the_window);

	// Now stop this special case run loop.
	// Ugh. [NSApp stop:nil]; throws a NSApp with wrong _running count exception.
	// I think we need to let this callback finish before we can stop it.
	// Fortunately, now that we have a working runloop, we can try to schedule the stop to happen later.
//	[NSApp stop:nil];
	[NSApp performSelectorOnMainThread:@selector(stop:) withObject:NSApp waitUntilDone:NO];

	
}

- (void) applicationWillTerminate:(NSNotification*)a_notification
{
	// Invoke the IupEntry callback function to start the user code.
	IFentry exit_callback = (IFentry)IupGetFunction("EXIT_CB");
	
	if(NULL != exit_callback)
	{
		exit_callback();
	}
}


@end

static void cocoaDialogStartModal(Ihandle* ih)
{
	NSWindow* the_window = (NSWindow*)ih->handle;
	
	
	
	// HACK:
	// This seems to be another one of those *must use [NSApp run] bugs*.
	// If I create a modal IupPopup as the first dialog, it prevents IupMainLoop() from getting called and thus [NSApp run].
	// The problem seems to come in dismissing the dialog. It sometimes does not go away and becomes a stuck ghost window.
	// Additionally, if I swap spaces, and then click the Dock icon, sometimes the window actually resurrects and becomes a stuck ghost window.
	// The solution seems to be to always run with [NSApp run].
	// So to make this happen in this IupPopup as first dialog (special) case, I need to check if isRunning.
	// If not running, I need to invoke [NSApp run].
	// However, this will transfer control flow to the AppDelegate and not return until that run is stopped.
	// Since this is a special case, I want to make a special AppDelegate for just this case.
	// So we want to save the real app delegate, swap in our new one, let it run the modal stuff until end, stop the run (which returns the control flow), and swap back in the original app delegate.
	
	// WARNING: There is a potential bug in that if the rest of this implementation is depending on something the real AppDelegate provides and we didn't implement it (correctly),
	// functionality will not behave correctly.
	
	if(NO == [[NSApplication sharedApplication] isRunning])
	{
		// Save the original delegate. It is weak ownership so retain counts are not changed.
		id original_delegate = [NSApp delegate];
		// Create our own special case app delegate for just this case.
		IupNonRunLoopModalAppDelegate* temp_delegate = [[IupNonRunLoopModalAppDelegate alloc] initWithIhandle:ih];
		[NSApp setDelegate:temp_delegate];
		
		// Remember this blocks until the run is stopped (presumably when the modal dialog is done)
		[NSApp run];

		// Now swap back the original app delegate
		[NSApp setDelegate:original_delegate];
		// free our temp delegate
		[temp_delegate release];
		return;
	}
	else
	{
		cocoaDialogRunModalLoop(ih, the_window);
	}
	

	
}

static void cocoaDialogEndModal(Ihandle* ih)
{
	
	void* the_session = iupAttribGet(ih, "_COCOA_MODAL_SESSION");
	[NSApp stopModal];
	// We need to call endModalSession now instead of after the return to the infinite-poll-loop because IupDestroy() wasn't fully working and a ghost window was sometimes being left behind/stuck.
	// I was a little worried this entire block needed to be called in Unmap, before cocoaCleanUpWindow(),
	// but this seems to work so far. I like here better because I don't have to handle two separate cases of whether it was called via IupDestroy or the user hit the close button.
	// Note that endModalSession can only be called once per session, or it throws an exception.
	[NSApp endModalSession:(NSModalSession)the_session];
	
	// IupHide seemed like the right thing to call based on all the times I saw it in the source.
	// However, it seems to get called already in the window close so this is redundant.
//	IupHide(ih); /* default: close the window */
	
	iupAttribSet(ih, "_COCOA_MODAL", "NO");
	iupAttribSet(ih, "_COCOA_MODAL_SESSION", NULL);
	
}


@implementation IupCocoaWindowDelegate

- (BOOL) windowShouldClose:(id)the_sender
{
//	NSLog(@"windowShouldClose");
	// I'm using objc_setAssociatedObject/objc_getAssociatedObject because it allows me to avoid making subclasses just to hold ivars. And category extension isn't working for some reason...NSWindow might be too big/complicated and is expecting me to define Apple stuff.
	
	Ihandle* ih = (Ihandle*)objc_getAssociatedObject(the_sender, IHANDLE_ASSOCIATED_OBJ_KEY);
	
	/* even when ACTIVE=NO the dialog gets this evt */
#if 0
	if (!iupdrvIsActive(ih)) // not implemented yet
	{
		return YES;
	}
#endif
	
	Icallback callback_function = IupGetCallback(ih, "CLOSE_CB");
	if(callback_function)
	{
		int ret = callback_function(ih);
		if (ret == IUP_IGNORE)
		{
			return NO;
		}
		if (ret == IUP_CLOSE)
		{
			IupExitLoop();
		}
	}
	
	return YES; /* do not propagate */
	
}


- (void) windowWillClose:(NSNotification*)the_notification
{
	NSWindow* the_window = [the_notification object];
//	NSLog(@"windowWillClose:");
	
	Ihandle* ih = (Ihandle*)objc_getAssociatedObject(the_window, IHANDLE_ASSOCIATED_OBJ_KEY);

	// I think??? we need to hide and not destroy because the user is supposed to call IupDestroy explicitly
	
	//	IupDestroy(ih);
	
	if(iupAttribGetBoolean(ih, "_COCOA_MODAL"))
	{
		cocoaDialogEndModal(ih);
	}
	else
	{
		// This contains a bunch of stuff for modal handling which doesn't work for us.
		IupHide(ih); /* default: close the window */
		
	}

}

- (NSSize) windowWillResize:(NSWindow*)the_sender toSize:(NSSize)frame_size
{
	// I'm using objc_setAssociatedObject/objc_getAssociatedObject because it allows me to avoid making subclasses just to hold ivars. And category extension isn't working for some reason...NSWindow might be too big/complicated and is expecting me to define Apple stuff.
	
	Ihandle* ih = (Ihandle*)objc_getAssociatedObject(the_sender, IHANDLE_ASSOCIATED_OBJ_KEY);
	
	/* even when ACTIVE=NO the dialog gets this evt */
#if 0
	if (!iupdrvIsActive(ih)) // not implemented yet
	{
		return YES;
	}
#endif
	
	NSLog(@"resize current_ih:<%d,%d>, target:<%f,%f>", ih->currentwidth, ih->currentheight, frame_size.width, frame_size.height);
	NSLog(@"resize current_win:<%f,%f>", [the_sender frame].size.width, [the_sender frame].size.height);

//	iupdrvDialogGetSize(ih, NULL, &(ih->currentwidth), &(ih->currentheight));

	
	
//	ih->currentwidth = frame_size.width;
//	ih->currentheight = frame_size.height;
	
	
	IFnii cb;
	
	
	cb = (IFnii)IupGetCallback(ih, "RESIZE_CB");
	if(!cb || cb(ih, frame_size.width, frame_size.height)!=IUP_IGNORE)  /* width and height here are for the client area */
	{
		iupdrvDialogGetSize(ih, NULL, &(ih->currentwidth), &(ih->currentheight));

//		ih->data->ignore_resize = 1;
		IupRefresh(ih);
//		ih->data->ignore_resize = 0;
		return frame_size;
	}
	else
	{
		// don't allow resize
		return [the_sender frame].size;
	}
	
	
	
}


@end


/* 
 This is a quick and dirty NSToolbar implementation. This was used to help me figure out the "menu" height metrics.
 But we eventually will need to implement toolbar support so this may be useful.
*/
/*
#define FIXME_TOOLBAR_IDENT @"FIXME:Need unique identifier"

@interface IupCocoaToolbarDelegate : NSObject<NSToolbarDelegate>

- (NSArray*) toolbarAllowedItemIdentifiers:(NSToolbar*)tool_bar;
- (NSArray*) toolbarDefaultItemIdentifiers:(NSToolbar*)tool_bar;
- (NSToolbarItem*) toolbar:(NSToolbar*)tool_bar itemForItemIdentifier:(NSString*)the_identifier willBeInsertedIntoToolbar:(BOOL)will_insert;

@end

@implementation IupCocoaToolbarDelegate


- (NSArray*) toolbarSelectableItemIdentifiers:(NSToolbar *)tool_bar
{
	NSLog(@"%s",__func__);
	return [self toolbarDefaultItemIdentifiers:tool_bar];
}

- (void) toolbarWillAddItem:(NSNotification *)tool_bar
{
	NSLog(@"%s",__func__);
}

- (void) toolbarDidRemoveItem:(NSNotification*)the_notification
{
	NSLog(@"%s",__func__);
}


- (NSArray*) toolbarAllowedItemIdentifiers:(NSToolbar*)tool_bar
{
	return @[@"One", @"Two"];
}

- (NSArray*) toolbarDefaultItemIdentifiers:(NSToolbar*)tool_bar
{
	return @[@"One", @"Two"];
}

- (NSToolbarItem*) toolbar:(NSToolbar*)tool_bar itemForItemIdentifier:(NSString*)item_identifier willBeInsertedIntoToolbar:(BOOL)will_insert
{
	NSToolbarItem* return_val = nil;
	NSString* the_label = @"default";
	NSString* toolbar_identifier = [tool_bar identifier];
	
	if([toolbar_identifier isEqualToString:FIXME_TOOLBAR_IDENT])
	{
		if([item_identifier isEqualToString:@"One"])
		{
			return_val = [[NSToolbarItem alloc] initWithItemIdentifier:@"One"];
			the_label = @"Toolbar One";
		}
		else if([item_identifier isEqualToString:@"Two"])
		{
			return_val = [[NSToolbarItem alloc] initWithItemIdentifier:@"Two"];
			the_label = @"Toolbar Two";
		}
	}
	
	[return_val setLabel:the_label];
	[return_val setPaletteLabel:the_label];
	return return_val;
}

@end
*/


/****************************************************************
 Utilities
 ****************************************************************/

int iupdrvDialogIsVisible(Ihandle* ih)
{
//	return iupdrvIsVisible(ih);

	// This is a little bit of a hack.
	// iupDialogShowXY needs to increment the number of visible windows.
	// When this window is being created, without this check, Cocoa will return true.
	// But Iup then seems to bypass the initialization routine because it assumes it had already gone through the init process.
	// This hack works because I set first_show to 1 in my Map function.
	// After Iup goes through its initialization, it also will set first_show to 1 again.
	// Without this hack, even if I have a bunch of windows open, IUP thinks I closed the last one
	// and will call IupExitLoop().
	if(ih->data->first_show)
	{
		return 0;
	}

	NSWindow* the_window = (NSWindow*)ih->handle;
	int ret_val = (int)[the_window isVisible];
	return ret_val;
}


void iupdrvDialogGetSize(Ihandle* ih, InativeHandle* handle, int *w, int *h)
{
	NSWindow* the_window = (NSWindow*)ih->handle;
	NSRect the_rect = [the_window frame];
	
	if (w) *w = iupROUND(the_rect.size.width);
	if (h) *h = iupROUND(the_rect.size.height);
}

void iupdrvDialogSetVisible(Ihandle* ih, int visible)
{
	NSWindow* the_window = (NSWindow*)ih->handle;

	if(visible)
	{
		[the_window makeKeyAndOrderFront:nil];

	}
	else
	{

		[the_window orderOut:nil];
	}
}

void iupdrvDialogGetPosition(Ihandle *ih, InativeHandle* handle, int *x, int *y)
{
	NSWindow* the_window = (NSWindow*)ih->handle;
	NSRect the_rect = [the_window frame];
	
	if (x) *x = the_rect.origin.x;
	if (y) *y = iupCocoaComputeIupScreenHeightFromCartesian(the_rect.origin.y);
}

void iupdrvDialogSetPosition(Ihandle *ih, int x, int y)
{
	NSWindow* the_window = (NSWindow*)ih->handle;
	NSRect the_rect = [the_window frame];

	if(ih->data->first_show)
	{
		int is_first_window = IupGetInt(ih, "_FIRST_WINDOW");
		if(is_first_window)
		{
			[the_window center];
			
			
			NSPoint new_pos = [the_window frame].origin;
			
			ih->x = new_pos.x;
			ih->y = iupCocoaComputeIupScreenHeightFromCartesian(new_pos.y);
		}
		else
		{
		
			ih->x = IupGetInt(ih, "CASCADE_X");
			ih->y = IupGetInt(ih, "CASCADE_Y");
		///		int inverted_height = iupCocoaComputeCartesianScreenHeightFromIup(ih->y);
		
		//		[the_window setFrame:NSMakeRect(ih->x, inverted_height, ih->currentwidth , ih->currentheight) display:YES];
		//		[the_window setFrameTopLeftPoint:NSMakePoint(ih->x, ih->y)];
		//	[the_window setFrameTopLeftPoint:NSMakePoint(90, 90)];
		
		}
		ih->data->first_show = 0;
	}
	else
	{
		int inverted_height = iupCocoaComputeCartesianScreenHeightFromIup(ih->y);
		
		[the_window setFrame:NSMakeRect(ih->x, inverted_height, ih->currentwidth , ih->currentheight) display:YES];
	}
	
	
	int inverted_height = iupCocoaComputeCartesianScreenHeightFromIup(y);


}


void iupdrvDialogGetDecoration(Ihandle* ih, int *border, int *caption, int *menu)
{
	NSLog(@"border=%d, caption%d, menu=%d", *border, *caption, *menu);
	NSWindow* the_window = (NSWindow*)ih->handle;

	
	
	CGFloat title_bar_height = 0.0;
	// What is "menu"? Is this supposed to encompass things like toolbars?
	CGFloat menu_bar_height = 0.0;
	CGFloat window_border_thickness = 0.0;
	
	NSRect window_frame = [the_window frame];

	// Now get the window contents. Assuming this includes the toolbar if any
	NSRect content_frame = [the_window contentRectForFrameRect:window_frame];
	
	
	// TODO: Test borderless windows, fullscreen windows
	if([the_window styleMask] & NSTitledWindowMask)
	{

		// Use the class method to avoid getting a window with a toolbar?
		NSRect plain_window_frame = [NSWindow frameRectForContentRect:content_frame styleMask:NSTitledWindowMask];
		CGFloat content_diff_height = plain_window_frame.size.height - content_frame.size.height;
		title_bar_height = content_diff_height;

		
	}
	else
	{
		title_bar_height = 0;
	}
	


	NSToolbar* the_toolbar = [the_window toolbar];
	if((nil != the_toolbar) && [the_toolbar isVisible])
	{
		CGFloat content_diff_height = window_frame.size.height - content_frame.size.height;
		menu_bar_height = content_diff_height - title_bar_height;
	}
	else
	{
		menu_bar_height = 0.0;
	}
	
	

	if([the_window styleMask] == NSWindowStyleMaskBorderless)
	{
		// assume window_frame - content_frame is the border thickness? Do width because height has title bars and toolbars.
		window_border_thickness = window_frame.size.width - content_frame.size.width;
		NSCAssert(window_border_thickness == 0.0, @"Expected border width to be 0");
		
	}
	else if(([the_window styleMask] & NSWindowStyleMaskFullSizeContentView) || ([the_window styleMask] & NSWindowStyleMaskFullScreen))
	{
		// assume window_frame - content_frame is the border thickness? Do width because height has title bars and toolbars.
		window_border_thickness = window_frame.size.width - content_frame.size.width;
		NSCAssert(window_border_thickness == 0.0, @"Expected border width to be 0");
		
	}
	else
	{
		// assume window_frame - content_frame is the border thickness? Do width because height has title bars and toolbars.
		window_border_thickness = window_frame.size.width - content_frame.size.width;
		
		CGFloat left_edge = [the_window contentBorderThicknessForEdge:NSRectEdgeMinX];
		CGFloat right_edge = [the_window contentBorderThicknessForEdge:NSRectEdgeMaxX];

		NSCAssert((left_edge+right_edge) == window_border_thickness, @"border width not what I expected");
	}
 
	
	*border = window_border_thickness;
	*menu = menu_bar_height;
	*caption = title_bar_height;

}

int iupdrvDialogSetPlacement(Ihandle* ih)
{
	
	char* placement;
	
	NSWindow* the_window = (NSWindow*)ih->handle;
	NSRect the_rect = [the_window frame];
	
	
	int old_state = ih->data->show_state;
	ih->data->show_state = IUP_SHOW;
	
	if (iupAttribGetBoolean(ih, "FULLSCREEN"))
	{

		NSUInteger masks = [the_window styleMask];
		if ( masks & NSFullScreenWindowMask)
		{
			// Do something
		}
		else
		{
			[the_window toggleFullScreen:nil];
		}
		
		
		return 1;
	}
	
	placement = iupAttribGet(ih, "PLACEMENT");
	if (!placement)
	{
		if (old_state == IUP_MAXIMIZE || old_state == IUP_MINIMIZE)
			ih->data->show_state = IUP_RESTORE;
		
//		gtk_window_unmaximize((GtkWindow*)ih->handle);
//		gtk_window_deiconify((GtkWindow*)ih->handle);
		return 0;
	}
	
	if (iupStrEqualNoCase(placement, "MINIMIZED"))
	{
//		ih->data->show_state = IUP_MINIMIZE;
//		gtk_window_iconify((GtkWindow*)ih->handle);
	}
	else if (iupStrEqualNoCase(placement, "MAXIMIZED"))
	{
//		ih->data->show_state = IUP_MAXIMIZE;
//		gtk_window_maximize((GtkWindow*)ih->handle);
	}
	else if (iupStrEqualNoCase(placement, "FULL"))
	{
#if 0
		int width, height, x, y;
		int border, caption, menu;
		iupdrvDialogGetDecoration(ih, &border, &caption, &menu);
		
		/* position the decoration outside the screen */
		x = -(border);
		y = -(border+caption+menu);
		
		/* the dialog client area will cover the task bar */
		iupdrvGetFullSize(&width, &height);
		
		height += menu; /* menu is inside the client area. */
		
		/* set the new size and position */
		/* The resize evt will update the layout */
		gtk_window_move((GtkWindow*)ih->handle, x, y);
		gtk_window_resize((GtkWindow*)ih->handle, width, height);
		
		if (old_state == IUP_MAXIMIZE || old_state == IUP_MINIMIZE)
			ih->data->show_state = IUP_RESTORE;
#endif
	}
	
	iupAttribSet(ih, "PLACEMENT", NULL); /* reset to NORMAL */
	


	return 1;
}

void iupdrvDialogSetParent(Ihandle* ih, InativeHandle* parent)
{
	
}

/****************************************************************
 Callbacks and Events
 ****************************************************************/

static int cocoaDialogSetTitleAttrib(Ihandle* ih, const char* value)
{
	NSWindow* the_window = (NSWindow*)ih->handle;

	if(value)
	{
		NSString* ns_string = [NSString stringWithUTF8String:value];

		[the_window setTitle:ns_string];

	}
	else
	{
		[the_window setTitle:nil];

	}

	
	return 1;
}





static int cocoaDialogModalPopupMethod(Ihandle* ih, int x, int y)
{
	cocoaDialogStartModal(ih);
	return IUP_NOERROR;
	
}


static int cocoaDialogMapMethod(Ihandle* ih)
{
	
//	iupAttribSet(ih, "RASTERSIZE", "500x400");
	
	
	// Warning: Don't make the initial window too big. There is code in the IUP core that does a MAX(current_size, needed_size)
	// which is intended to make the window grow to fit.
	// I made the mistake of making the initial window too big and didn't understand why I could never get a window that perfectly fit the contents.
	// I think the other implementations start with 100x100.
	NSWindow* the_window = [[NSWindow alloc] initWithContentRect:NSMakeRect(0, 0, 100, 100)
													styleMask:NSTitledWindowMask|NSClosableWindowMask|NSResizableWindowMask|NSMiniaturizableWindowMask backing:NSBackingStoreBuffered defer:NO];

/*
	NSToolbar* the_toolbar = [[NSToolbar alloc] initWithIdentifier:FIXME_TOOLBAR_IDENT];
	IupCocoaToolbarDelegate* toolbar_delegate = [[IupCocoaToolbarDelegate alloc] init];
	[the_toolbar setDelegate:toolbar_delegate];
	[the_toolbar setDisplayMode:NSToolbarDisplayModeIconAndLabel];
	[the_toolbar setAllowsUserCustomization:YES];
	[the_toolbar setAutosavesConfiguration:YES];
	[the_window setToolbar:the_toolbar];
*/
	
	// We are manually managing the memory, so don't let the window release itself
	[the_window setReleasedWhenClosed:NO];

	static _Bool s_isFirstWindow = true;

	static NSPoint last_cascade_point = {0, 0};
	NSPoint new_cascade_point = {0, 0};
	
	
	if(s_isFirstWindow)
	{
		// I would like to detect if this is the very first window created and center the window in this case.
		// TODO: Save window locations between runs
		[the_window center];
		last_cascade_point = [the_window cascadeTopLeftFromPoint:NSZeroPoint];
		IupSetInt(ih, "_FIRST_WINDOW", 1);
		s_isFirstWindow = false;

	}
	else
	{
		
		NSWindow* key_window = [[NSApplication sharedApplication] keyWindow];
		if(nil != key_window)
		{
			//		last_cascade_point = [key_window frame].origin;
			//		last_cascade_point.y = iupCocoaComputeCartesianScreenHeightFromIup(last_cascade_point.y);
			
			// Just in case the user moved the window from the last time we saved the variable
			last_cascade_point = [key_window cascadeTopLeftFromPoint:NSZeroPoint];
			
			
			//   new_cascade_point = [the_window cascadeTopLeftFromPoint:last_cascade_point];
			new_cascade_point = [the_window cascadeTopLeftFromPoint:last_cascade_point];
			IupSetInt(ih, "_FIRST_WINDOW", 0);
			
			new_cascade_point = [the_window cascadeTopLeftFromPoint:last_cascade_point];
			//ih->x = cascade_point.x;
			//ih->y = iupCocoaComputeIupScreenHeightFromCartesian(cascade_point.y);
			IupSetInt(ih, "CASCADE_X", last_cascade_point.x);
			//	IupSetInt(ih, "CASCADE_Y", iupCocoaComputeIupScreenHeightFromCartesian(last_cascade_point.y));
			IupSetInt(ih, "CASCADE_Y", last_cascade_point.y);
			last_cascade_point = new_cascade_point;
			
		}
		
	}
	
    
	ih->data->first_show = 1;


//	[the_window setTitle:@"First Window"];
	
	ih->handle = (__unsafe_unretained void*)the_window;
	
	IupCocoaWindowDelegate* window_delegate = [[IupCocoaWindowDelegate alloc] init];
	[the_window setDelegate:window_delegate];
//	[window setIupIhandle:ih];
	
	// I'm using objc_setAssociatedObject/objc_getAssociatedObject because it allows me to avoid making subclasses just to hold ivars. And category extension isn't working for some reason...NSWindow might be too big/complicated and is expecting me to define Apple stuff.
	objc_setAssociatedObject(the_window, IHANDLE_ASSOCIATED_OBJ_KEY, (id)ih, OBJC_ASSOCIATION_ASSIGN);

	
	
	[the_window makeKeyAndOrderFront:nil];

//	ih->currentwidth = 200;
//	ih->currentheight = 200;
	
	return IUP_NOERROR;

}

static void cocoaDialogUnMapMethod(Ihandle* ih)
{
	// I am having problems with stuck ghost windows. I don't know what's causing it, but one theory I have is that tearing down a window while modal is bad.
	// So this is my attempt to stop the modal stuff before we tear down the window.
	// This means I need this code here and possilby if the user just closes the window first instead of triggered by IupDestroy.
	// Unfortunately, this doesn't seem to actually fix the problem. I still see it sometimes, especially with breakpoints on.
	// However, it seems to help a little. It seems like a race condiition.
	// My test creates the popup before [NSApp run] is started. So I hope maybe this is just another one of those bugs, and one that is rare.
	// Another theory is I need to wait for the loop control to return to the infinite-loop. But I don't know if it is possible to wait.
	// The problem is that IUP wants to destroy the IH now. If I try to defer the Cocoa teardown somehow, the ih handle may be gone which contains data I need.
	// UPDATE: I now believe the ghost/stuck windows are because of not using the [NSApp run] (real runloop). I now have a special case to handle this.
	if(iupAttribGetBoolean(ih, "_COCOA_MODAL"))
	{
		cocoaDialogEndModal(ih);
	}
	cocoaCleanUpWindow(ih);
}

static void cocoaDialogLayoutUpdateMethod(Ihandle* ih)
{
#if 1
	if(ih->data->ignore_resize)
	{
		return;
	}
	
	ih->data->ignore_resize = 1;
	
	/* for dialogs the position is not updated here */
	
	NSWindow* the_window = (NSWindow*)ih->handle;
	NSRect the_frame = [the_window frame];
	the_frame.size.width = ih->currentwidth;
	the_frame.size.height = ih->currentheight;
	
//	[the_window setFrame:the_frame display:YES animate:YES];
	[the_window setFrame:the_frame display:YES animate:NO];
	
	ih->data->ignore_resize = 0;
#endif
	

	
//	NSRect the_rect = [the_window frame];



}


void iupdrvDialogInitClass(Iclass* ic)
{
	/* Driver Dependent Class methods */
	ic->Map = cocoaDialogMapMethod;
	ic->UnMap = cocoaDialogUnMapMethod;
	ic->LayoutUpdate = cocoaDialogLayoutUpdateMethod;
	

	// Setting DlgPopup is not typical, but IUP's modal dialog system for IupPopup doesn't work the way Cocoa does.
	// IUP wants to control the modality and the event loop pumping with IupMainLoop(). This doesn't work so well for us.
	// So we set this callback to notify IUP that we are using the native platform's modal stuff.
	// IUP seemed to intend it to be for specific dialogs like the FileDialog, but Antonio Scuri thinks we can use it here too.
	ic->DlgPopup = cocoaDialogModalPopupMethod;


#if 0
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
#endif
	
	
	/* Special */
	iupClassRegisterAttribute(ic, "TITLE", NULL, cocoaDialogSetTitleAttrib, NULL, NULL, IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);
	
	
#if 0
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

