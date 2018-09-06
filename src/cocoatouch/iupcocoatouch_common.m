/** \file
 * \brief GTK Base FunctioUI
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdio.h>              
#include <stdlib.h>
#include <string.h>             
#include <limits.h>             

#import <UIKit/UIKit.h>

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

#include "iupcocoatouch_drv.h"
#import "IupAppDelegateProtocol.h"

const void* IHANDLE_ASSOCIATED_OBJ_KEY = @"IHANDLE_ASSOCIATED_OBJ_KEY"; // the point of this is we have a unique memory address for an identifier


UIWindow* cocoaTouchFindCurrentWindow()
{
	// I'm expecting the app delegate to conform to IupAppDelegateProtocol,
	// just in case users try to use Iup in existing app infrastructures.
	// They will be required to implement the needed methods.
	// I also don't want to be too rigid and force pedantic protocol conformance.
	// An informal protocol conformance like simply adding a category is sufficient.
	UIResponder<IupAppDelegateProtocol>* app_delegate = (UIResponder<IupAppDelegateProtocol>*)[[UIApplication sharedApplication] delegate];
	
	UIWindow* the_window = nil;
	
	// Common expected case
	if([app_delegate respondsToSelector:@selector(currentWindow)])
	{
		the_window = [app_delegate currentWindow];
	}
	// Not sure what to do here. But the user is probably trying to use Iup in an existing program.
	// We could try to create a new UIWindow, but I don't have any place to store it.
	else
	{
		the_window = [[UIApplication sharedApplication] keyWindow];
	}
	
	return the_window;
}

UIViewController* cocoaTouchFindCurrentRootViewController()
{
	UIWindow* the_window = cocoaTouchFindCurrentWindow();
	UIViewController* root_view_controller = [the_window rootViewController];
	
	return root_view_controller;
}

UINavigationController* cocoaTouchFindCurrentRootNavigationViewController()
{
	UIWindow* the_window = cocoaTouchFindCurrentWindow();
	UIViewController* root_view_controller = [the_window rootViewController];
	
	if([root_view_controller isKindOfClass:[UINavigationController class]])
	{
		return (UINavigationController*)root_view_controller;
	}
	else
	{
		return nil;
	}
}


void iupCocoaTouchAddToParent(Ihandle* ih)
{
	id parent_native_handle = iupChildTreeGetNativeParentHandle(ih);
	
	id child_handle = ih->handle;
	if([child_handle isKindOfClass:[UIView class]])
	{
		UIView* the_view = (UIView*)child_handle;
		
		// From David Phillip Oster:
		/* 
		 now, when you resize the window, you see the hidden content. This makes the subviews preserve their relative x,y offset from the top left of the window, iUItead of the Mac default of the bottom left. It probably isn't the right way to do it, since there's probably some iup property that is specifying somethign more complex.
		 
		 
		 If I had set [the_view setAutoresizingMask:UIViewMinXMargin|UIViewWidthSizable|UIViewMaxXMargin];
		 
		 Then, as the window wideUI, that view wideUI along with it.
		 */
		[the_view setAutoresizingMask:UIViewAutoresizingFlexibleRightMargin|UIViewAutoresizingFlexibleTopMargin];

		
		if([parent_native_handle isKindOfClass:[UIViewController class]])
		{
			UIViewController* parent_view_controller = (UIViewController*)parent_native_handle;
			UIView* root_view = [parent_view_controller view];
			[root_view addSubview:the_view];
		}
		else if([parent_native_handle isKindOfClass:[UIWindow class]])
		{
			UIWindow* parent_window = (UIWindow*)parent_native_handle;
			UIViewController* parent_view_controller = (UIViewController*)parent_native_handle;
			UIView* root_view = [parent_view_controller view];
			[root_view addSubview:the_view];
		}
		else if([parent_native_handle isKindOfClass:[UIView class]])
		{
			UIView* parent_view = (UIView*)parent_native_handle;
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

void iupCocoaTouchRemoveFromParent(Ihandle* ih)
{
	
	id child_handle = ih->handle;
	if([child_handle isKindOfClass:[UIView class]])
	{
		UIView* the_view = (UIView*)child_handle;
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

UIColor *iupCocoaTouchToNativeColor(const char *iColor)
{
	unsigned char r, g, b, a;
	if (iupStrToRGBA(iColor, &r, &g, &b, &a))
	{
		CGFloat red = r/255., green = g/255., blue = b/255., alpha = a/255.;
		return [UIColor colorWithRed:red green:green blue:blue alpha:alpha];
	}
	return nil;
}

char *iupCocoaTouchColorFromNative(UIColor *color)
{
	CGFloat red, green, blue, alpha;
	if ([color getRed:&red green:&green blue:&blue alpha:&alpha])
	{
		unsigned char r = red*255, g = green*255, b = blue*255, a = alpha*255;
		return iupStrReturnRGBA(r, g, b, a);
	}
	return NULL;
}

int iupCocoaTouchSetBgColorAttrib(Ihandle* ih, char *iColor)
{
	UIColor *color = iupCocoaTouchToNativeColor(iColor);
	if (color)
	{
		UIView* the_view = ih->handle;
		[the_view setBackgroundColor:color];
		return IUP_NOERROR;
	}
	return IUP_ERROR;
}

char* iupCocoaTouchGetBGColorAttrib(Ihandle* ih)
{
	UIView* the_view = ih->handle;
	return iupCocoaTouchColorFromNative([the_view backgroundColor]);
}

int iupCocoaTouchSetActiveAttrib(Ihandle* ih, int enabled)
{
	UIButton* the_button = ih->handle;
	if ([the_button respondsToSelector:@selector(setEnabled:)])
	{
		[the_button setEnabled:enabled];
		return IUP_NOERROR;
	}
	return IUP_ERROR;
}


// WARNING: It is impossible to refuse a resize on iOS. This must be documented for IUP.
void iupCocoaTouchDoResize(Ihandle* ih, CGSize to_size, UIViewController* view_controller)
{
//	NSLog(@"resize current_ih:<%d,%d>, target:<%f,%f>", ih->currentwidth, ih->currentheight, frame_size.width, frame_size.height);
//	NSLog(@"resize current_win:<%f,%f>", [the_sender frame].size.width, [the_sender frame].size.height);

//	iupdrvDialogGetSize(ih, NULL, &(ih->currentwidth), &(ih->currentheight));

	
	
//	ih->currentwidth = frame_size.width;
//	ih->currentheight = frame_size.height;
	
	
	IFnii cb;
	cb = (IFnii)IupGetCallback(ih, "RESIZE_CB");
	// FIXME: Are the parameters supposed to be the contentView or the entire window. The Windows code comments make me think contentView, but the actual code makes me think entire window. The latter is way easier to do.
	if(!cb || cb(ih, to_size.width, to_size.height)!=IUP_IGNORE)
	{
		ih->currentwidth = iupROUND(to_size.width);
		ih->currentheight = iupROUND(to_size.height);
		
//		ih->data->ignore_resize = 1;
		IupRefresh(ih);
//		ih->data->ignore_resize = 0;
	}
	else
	{
		// WARNING: It is impossible to refuse a resize on iOS.
		NSLog(@"IUP_IGNORE not supported on iOS for RESIZE_CB");
		ih->currentwidth = iupROUND(to_size.width);
		ih->currentheight = iupROUND(to_size.height);
		
//		ih->data->ignore_resize = 1;
		IupRefresh(ih);
//		ih->data->ignore_resize = 0;
	}
	
}


#if 0
int iupCocoaComputeCartesiaUIcreenHeightFromIup(int iup_height)
{
	// Do I want the full screen height or the visible height?
	UIRect screen_rect = [[UIScreen maiUIcreen] frame];
//	UIRect screen_rect = [[UIScreen maiUIcreen] visibleFrame];
	CGFloat inverted_height = screen_rect.size.height - iup_height;
	return (int)(inverted_height+0.5);
}

int iupCocoaComputeIupScreenHeightFromCartesian(int cartesian_height)
{
	// Do I want the full screen height or the visible height?
	UIRect screen_rect = [[UIScreen maiUIcreen] frame];
//	UIRect screen_rect = [[UIScreen maiUIcreen] visibleFrame];
	CGFloat inverted_height = screen_rect.size.height - cartesian_height;
	return (int)(inverted_height+0.5);
}
#endif



void iupdrvActivate(Ihandle* ih)
{

}

void iupdrvReparent(Ihandle* ih)
{

	
}


void iupdrvBaseLayoutUpdateMethod(Ihandle *ih)
{

	id parent_native_handle = iupChildTreeGetNativeParentHandle(ih);
	UIView* parent_view = nil;
	
	if([parent_native_handle isKindOfClass:[UIViewController class]])
	{
		UIViewController* parent_view_controller = (UIViewController*)parent_native_handle;
		parent_view = [parent_view_controller view];
		
	}
	else if([parent_native_handle isKindOfClass:[UIWindow class]])
	{
		UIWindow* parent_window = (UIWindow*)parent_native_handle;
		parent_view = [[parent_window rootViewController] view];
	}
	else if([parent_native_handle isKindOfClass:[UIView class]])
	{
		parent_view = (UIView*)parent_native_handle;
	}
	else
	{
		NSCAssert(1, @"Unexpected type for parent widget");
		@throw @"Unexpected type for parent widget";
	}
	
	
	
	id child_handle = ih->handle;
	UIView* the_view = nil;
	if([child_handle isKindOfClass:[UIView class]])
	{
		the_view = (UIView*)child_handle;
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
	
	CGRect parent_rect = [parent_view frame];
	CGRect the_rect;
	CGFloat target_x;
	CGFloat target_y;
	CGFloat target_width;
	CGFloat target_height;

	if([parent_view respondsToSelector:@selector(safeAreaInsets)])
	{
		// We need to handle iPhone X and the "safeArea".
		// I think the navigation bar is included in the safeArea calcluation.
		UIEdgeInsets safe_area = [parent_view safeAreaInsets];

		// NOTE: I am not sure if the safe area computation should be done here or if the safe-value should be baked into the view controller's RESIZE_CB and pretend the safe area is the actual size.
		// The advantage of the latter is our currentwidth/height calculations would be correct. I'm worried that other IUP mechanisms may make assumptions and read the currentwidth/height values directly which may result in bad layouts.
		// But the advantage of the former (doing it here), is that if Apple puts safeArea info in any of the views (not just the viewcontroller), then we get this automatically handled.
		// Additionally, we should be able to add an IUP Attribute that allows users to mark specific widgets to not worry about the safe area so they can utilize all the screen space.
		// If we lie about the size of the view controller to only the safe-area, then this attribute feature will be more difficult to pull off.

		the_rect = CGRectMake(
			ih->x + safe_area.left,
			ih->y + safe_area.top,
			ih->currentwidth - safe_area.right - safe_area.left,
			ih->currentheight - safe_area.bottom - safe_area.top
		);
	}
	else
	{
		CGFloat status_bar_height = [[UIApplication sharedApplication] statusBarFrame].size.height;

		UINavigationController* navigation_controller = cocoaTouchFindCurrentRootNavigationViewController();
		UINavigationBar* navigation_bar = [navigation_controller navigationBar];
		CGFloat navigation_bar_height = 0.0;
		if(![navigation_bar isHidden])
		{
			navigation_bar_height = [navigation_bar frame].size.height;
		}
		
		CGFloat total_height = status_bar_height + navigation_bar_height;

		the_rect = CGRectMake(
			ih->x,
			ih->y + total_height,
			ih->currentwidth,
			ih->currentheight
		);
	}
	



	
	
	

	[the_view setFrame:the_rect];
//	[the_view setBounds:the_rect];
	
	
}

void iupdrvBaseUnMapMethod(Ihandle* ih)
{
	// Why do I need this when everything else has its own UnMap method?
	NSLog(@"iupdrvBaseUnMapMethod not implemented. Might be leaking");
}

void iupdrvDisplayUpdate(Ihandle *ih)
{
	id the_handle = ih->handle;
	
	if([the_handle isKindOfClass:[UIView class]])
	{
		UIView* the_view = (UIView*)the_handle;
		[the_view setNeedsDisplay];
	}
	else if([the_handle isKindOfClass:[UIWindow class]])
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
	if([the_object isKindOfClass:[UIWindow class]])
	{
		// NOT IMPLEMENTED

	}
	else if([the_object isKindOfClass:[UIView class]])
	{
		UIView* the_view = (UIView*)the_object;
		bool is_hidden = !(bool)visible;
		[the_view setHidden:is_hidden];
	}
	
	
}

int iupdrvIsVisible(Ihandle* ih)
{
	id the_object = ih->handle;
	if([the_object isKindOfClass:[UIWindow class]])
	{
		// NOT IMPLEMENTED
		return 1;
	}
	else if([the_object isKindOfClass:[UIView class]])
	{
		UIView* the_view = (UIView*)the_object;
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
