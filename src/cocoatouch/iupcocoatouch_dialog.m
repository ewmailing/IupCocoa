/** \file
 * \brief IupDialog class
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

#include "iupcocoatouch_drv.h"
#include "IupAppDelegateProtocol.h"
#include "IupViewController.h"
#include "IupNavigationController.h"
#include "IupLaunchViewController.h"

/*
@interface UIWindow () 
@property(readwrite, unsafe_unretained) Ihandle* iupIhandle;
@end

@implementation UIWindow
@synthesize iupIhandle = _iupIhandle;
@end
 */
#if 0
@interface IupcocoaTouchWindowDelegate : NSObject <UIWindowDelegate>
- (BOOL) windowShouldClose:(id)the_sender;
- (NSSize) windowWillResize:(UIWindow*)the_sender toSize:(NSSize)frame_size;
@end

static void cocoaTouchCleanUpWindow(Ihandle* ih)
{
	UIWindow* the_window = (__bridge UIWindow*)ih->handle;
	[the_window close];
	
	IupcocoaTouchWindowDelegate* window_delegate = [the_window delegate];
	[the_window setDelegate:nil];
	[window_delegate release];
	
	[the_window release];
}



@implementation IupcocoaTouchWindowDelegate

- (BOOL) windowShouldClose:(id)the_sender
{
	// I'm using objc_setAssociatedObject/objc_getAssociatedObject because it allows me to avoid making subclasses just to hold ivars. And category extension isn't working for some reason...UIWindow might be too big/complicated and is expecting me to define Apple stuff.
	
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

	// I think??? we need to hide and not destroy because the user is supposed to call IupDestroy explicitly
	IupHide(ih); /* default: close the window */

//	IupDestroy(ih);
	
	return YES; /* do not propagate */
	
}

- (NSSize) windowWillResize:(UIWindow*)the_sender toSize:(NSSize)frame_size
{
	// I'm using objc_setAssociatedObject/objc_getAssociatedObject because it allows me to avoid making subclasses just to hold ivars. And category extension isn't working for some reason...UIWindow might be too big/complicated and is expecting me to define Apple stuff.
	
	Ihandle* ih = (Ihandle*)objc_getAssociatedObject(the_sender, IHANDLE_ASSOCIATED_OBJ_KEY);
	
	/* even when ACTIVE=NO the dialog gets this evt */
#if 0
	if (!iupdrvIsActive(ih)) // not implemented yet
	{
		return YES;
	}
#endif
	
//	iupdrvDialogGetSize(ih, NULL, &(ih->currentwidth), &(ih->currentheight));

	ih->currentwidth = frame_size.width;
	ih->currentheight = frame_size.height;
	
	return frame_size;
	
}


@end

#endif
/****************************************************************
 Utilities
 ****************************************************************/

int iupdrvDialogIsVisible(Ihandle* ih)
{
#if 0
//	return iupdrvIsVisible(ih);
	UIWindow* the_window = (UIWindow*)ih->handle;
	int ret_val = (int)[the_window isVisible];
	return ret_val;
#else
	return 1;
#endif
}


void iupdrvDialogGetSize(Ihandle* ih, InativeHandle* handle, int *w, int *h)
{
	UIWindow* the_window = (UIWindow*)ih->handle;
	CGRect the_rect = [the_window frame];
	
	if (w) *w = the_rect.size.width;
	if (h) *h = the_rect.size.height;
}

void iupdrvDialogSetVisible(Ihandle* ih, int visible)
{
#if 0
	UIWindow* the_window = (UIWindow*)ih->handle;

	if(visible)
	{
		[the_window makeKeyAndOrderFront:nil];

	}
	else
	{

		[the_window orderOut:nil];
	}
#endif
}

void iupdrvDialogGetPosition(Ihandle *ih, InativeHandle* handle, int *x, int *y)
{
#if 0
	UIWindow* the_window = (UIWindow*)ih->handle;
	CGRect the_rect = [the_window frame];
	
	if (x) *x = the_rect.origin.x;
	if (y) *y = iupcocoaTouchComputeIupScreenHeightFromCartesian(the_rect.origin.y);
#endif
}

void iupdrvDialogSetPosition(Ihandle *ih, int x, int y)
{
#if 0
	UIWindow* the_window = (UIWindow*)ih->handle;
	CGRect the_rect = [the_window frame];
	
	int inverted_height = iupcocoaTouchComputeCartesianScreenHeightFromIup(y);

	[the_window setFrame:NSMakeRect(x, inverted_height, the_rect.size.width , the_rect.size.height) display:YES];
#endif
}


void iupdrvDialogGetDecoration(Ihandle* ih, int *border, int *caption, int *menu)
{
	// TODO: these are placeholder values
	if(border) *border = 0;
	if(caption) *caption = 0;
	if(menu) *menu = 0;
}

int iupdrvDialogSetPlacement(Ihandle* ih)
{
#if 0
	char* placement;
	
	UIWindow* the_window = (UIWindow*)ih->handle;
	CGRect the_rect = [the_window frame];
	
	
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
	

#endif
	return 1;
}

void iupdrvDialogSetParent(Ihandle* ih, InativeHandle* parent)
{
	
}


/****************************************************************
 Callbacks and Events
 ****************************************************************/

static int cocoaTouchDialogSetTitleAttrib(Ihandle* ih, const char* value)
{
	// I'm assuming this is a ViewController, but I guess anything that responds to setTitle is fine.
	id the_dialog = (id)ih->handle;
	if([the_dialog respondsToSelector:@selector(setTitle:)])
	{
		if(value)
		{
			NSString* ns_string = [NSString stringWithUTF8String:value];
			[the_dialog setTitle:ns_string];
		}
		else
		{
			[the_dialog setTitle:nil];
		}
	}
	
	return 1;
}




static int cocoaTouchDialogMapMethod(Ihandle* ih)
{
	// TODO: Allow for iOS specific properties if users want to request other behaviors,
	// e.g. UIWindows instead of navigation controllers.
	
	
	
	UIWindow* the_window = cocoaTouchFindCurrentWindow();
	UIViewController* root_view_controller = [the_window rootViewController];
	

	// If we still have our placeholder class, time to replace it.
	if([root_view_controller isKindOfClass:[IupLaunchViewController class]])
	{
		IupViewController* new_view_controller = [[[IupViewController alloc] init] autorelease];
		CGRect window_bounds = [[UIScreen mainScreen] bounds];
		UIView* root_view = [[[UIView alloc] initWithFrame:window_bounds] autorelease];
//		[root_view setBackgroundColor:[UIColor redColor]];
		[root_view setBackgroundColor:[UIColor whiteColor]];
		[new_view_controller setView:root_view];
		[new_view_controller setIhandle:ih];
		
		
		IupNavigationController* navigation_controller = [[[IupNavigationController alloc] initWithRootViewController:new_view_controller] autorelease];
		[navigation_controller setIhandle:ih];

		[the_window setRootViewController:navigation_controller];
		
		ih->handle = [new_view_controller retain];

		ih->currentwidth = window_bounds.size.width;
		ih->currentheight = window_bounds.size.height;
		ih->naturalwidth = window_bounds.size.width;
		ih->naturalheight = window_bounds.size.height;
		
		// It appears that our layout happenned before things are ready, so in our initial swap, the nav bar is missing which messes up the layout calculation.
		// (EdgeInsets are 0).
		// Try to force a recompute.
		dispatch_async(dispatch_get_main_queue(),
			^{
				IupRefresh(ih);
			}
		);
		
	}
	// This is the expected common case where we have replaced the root view controller with a navigation view controller
	else if([root_view_controller isKindOfClass:[UINavigationController class]])
	{
		IupViewController* new_view_controller = [[[IupViewController alloc] init] autorelease];
		CGRect window_bounds = [[UIScreen mainScreen] bounds];
		UIView* root_view = [[[UIView alloc] initWithFrame:window_bounds] autorelease];
//		[root_view setBackgroundColor:[UIColor greenColor]];
//		[root_view setBackgroundColor:[UIColor groupTableViewBackgroundColor]];
		[root_view setBackgroundColor:[UIColor whiteColor]];
		[new_view_controller setIhandle:ih];

		
		[new_view_controller setView:root_view];
	
		[(UINavigationController*)root_view_controller pushViewController:new_view_controller animated:YES];

		ih->handle = [new_view_controller retain];
		ih->currentwidth = window_bounds.size.width;
		ih->currentheight = window_bounds.size.height;
		ih->naturalwidth = window_bounds.size.width;
		ih->naturalheight = window_bounds.size.height;
	}
	// I'm not sure what to do here. But my thinking is it would be nice to support using Iup in existing iOS apps.
	// So we need a convention where IUP code can be used within somebody else's application structure.
	// My thinking is their application delegate must conform to IupAppDelegateProtocol.
	// But after that, I'm not sure what should happen.
	// Should we swap in a navigation controller?
	// Or should we create a separate UIWindow?
	else
	{
		// swapping in a navigation controller for now
		IupViewController* new_view_controller = [[[IupViewController alloc] init] autorelease];
		CGRect window_bounds = [[UIScreen mainScreen] bounds];
		UIView* root_view = [[[UIView alloc] initWithFrame:window_bounds] autorelease];
		[root_view setBackgroundColor:[UIColor whiteColor]];
		[new_view_controller setView:root_view];
		[new_view_controller setIhandle:ih];

		
		
		IupNavigationController* navigation_controller = [[[IupNavigationController alloc] initWithRootViewController:(UINavigationController*)root_view_controller] autorelease];
		
		[the_window setRootViewController:navigation_controller];
		[navigation_controller setIhandle:ih];

		ih->handle = [new_view_controller retain];

		[(UINavigationController*)root_view_controller pushViewController:new_view_controller animated:YES];
		ih->currentwidth = window_bounds.size.width;
		ih->currentheight = window_bounds.size.height;
		ih->naturalwidth = window_bounds.size.width;
		ih->naturalheight = window_bounds.size.height;
	}
	
#if 0
	iupAttribSet(ih, "RASTERSIZE", "500x400");
	
	UIWindow* the_window = [[UIWindow alloc] initWithContentRect:NSMakeRect(0, 0, 200, 200)
													styleMask:NSTitledWindowMask|NSClosableWindowMask|NSResizableWindowMask|NSMiniaturizableWindowMask backing:NSBackingStoreBuffered defer:NO];

	// We are manually managing the memory, so don't let the window release itself
	[the_window setReleasedWhenClosed:NO];
	
	[the_window cascadeTopLeftFromPoint:NSMakePoint(20,20)];
//	[window setTitle:appName];
	[the_window makeKeyAndOrderFront:nil];
	
	ih->handle = (__unsafe_unretained void*)the_window;
	
	IupcocoaTouchWindowDelegate* window_delegate = [[IupcocoaTouchWindowDelegate alloc] init];
//	[window setIupIhandle:ih];
	
	// I'm using objc_setAssociatedObject/objc_getAssociatedObject because it allows me to avoid making subclasses just to hold ivars. And category extension isn't working for some reason...UIWindow might be too big/complicated and is expecting me to define Apple stuff.
	objc_setAssociatedObject(the_window, IHANDLE_ASSOCIATED_OBJ_KEY, (id)ih, OBJC_ASSOCIATION_ASSIGN);

	
	[the_window setDelegate:window_delegate];
	
	

//	ih->currentwidth = 200;
//	ih->currentheight = 200;
#endif
	return IUP_NOERROR;

}

static void cocoaTouchDialogUnMapMethod(Ihandle* ih)
{
	id the_dialog = ih->handle;
	if([the_dialog isKindOfClass:[UIViewController class]])
	{
		UIViewController* view_controller = (UIViewController*)the_dialog;
		
		UINavigationController* root_navigation_controller = cocoaTouchFindCurrentRootNavigationViewController();
		if(root_navigation_controller)
		{
			// This view controller may or may not be in the navigation stack depending if the programmer is trying to remove it explicitly or if this function is being called because the user popped it and we're doing clean up.
			NSMutableArray* view_controller_stack = [[root_navigation_controller viewControllers] mutableCopy];
			
			[view_controller_stack removeObject:view_controller];
			
			// If the view controller was removed, then make that the new view controller array
			if([view_controller_stack count] != [[root_navigation_controller viewControllers] count])
			{
				[root_navigation_controller setViewControllers:view_controller_stack];
			}
		}
	}
	//TODO?: else if([the_dialog isKindOfClass:[UIWindow class]])
	
	
	[the_dialog release];
	ih->handle = NULL;
}

static void cocoaTouchDialogLayoutUpdateMethod(Ihandle* ih)
{
#if 0
#if 0
	if (ih->data->ignore_resize)
		return;
	
	ih->data->ignore_resize = 1;
	
	/* for dialogs the position is not updated here */
	SetWindowPos(ih->handle, 0, 0, 0, ih->currentwidth, ih->currentheight,
				 SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOOWNERZORDER|SWP_NOSENDCHANGING);
	
	ih->data->ignore_resize = 0;
#endif
	
	UIWindow* the_window = (UIWindow*)ih->handle;
//	CGRect the_rect = [the_window frame];
	
	int inverted_height = iupcocoaTouchComputeCartesianScreenHeightFromIup(ih->y);

	[the_window setFrame:NSMakeRect(ih->x, inverted_height, ih->currentwidth , ih->currentheight) display:YES];
#endif
}



void iupdrvDialogInitClass(Iclass* ic)
{
	/* Driver Dependent Class methods */
	ic->Map = cocoaTouchDialogMapMethod;
	ic->UnMap = cocoaTouchDialogUnMapMethod;
	ic->LayoutUpdate = cocoaTouchDialogLayoutUpdateMethod;

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
	iupClassRegisterAttribute(ic, "TITLE", NULL, cocoaTouchDialogSetTitleAttrib, NULL, NULL, IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);
	
	
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
