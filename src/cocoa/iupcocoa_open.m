/** \file
 * \brief MAC Driver Core
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdio.h>          
#include <stdlib.h>
#include <string.h>          
#import <Cocoa/Cocoa.h>

#include "iup.h"

#include "iup_str.h"
#include "iup_drv.h"
#include "iup_drvinfo.h"
#include "iup_object.h"
#include "iup_globalattrib.h"

#include "iupcocoa_drv.h"

#import "IupAppDelegate.h"


static NSAutoreleasePool* s_autoreleasePool = nil;
static IupAppDelegate* s_appDelegate = nil;


#if 0
char* iupmacGetNativeWindowHandle(Ihandle* ih)
{
  id window = ih->handle->window;
  if (window)
    return (char*)window;
  else
    return NULL;
}
#endif

void* iupdrvGetDisplay(void)
{
  return NULL;
}

static bool cocoaGetByteRGBAFromNSColor(NSColor* ns_color, unsigned char* red, unsigned char* green, unsigned char* blue, unsigned char* alpha)
{
	NSColor* rgb_color = [ns_color colorUsingColorSpace:[NSColorSpace genericRGBColorSpace]];
	if(rgb_color)
	{
		CGFloat rgba_components[4];
		[rgb_color getComponents:rgba_components];
		*red = iupROUND(rgba_components[0] * 255.0);
		*green = iupROUND(rgba_components[1] * 255.0);
		*blue = iupROUND(rgba_components[2] * 255.0);
		*alpha = iupROUND(rgba_components[3] * 255.0);
		return true;
	}
	else
	{
		NSCAssert(false, @"Color conversion failed");
		return false;
	}
}

void iupmacUpdateGlobalColors(void)
{
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;

	// I don't know what this should be
	//	cocoaGetByteRGBAFromNSColor([NSColor controlBackgroundColor], &r, &g, &b, &a);
	cocoaGetByteRGBAFromNSColor([NSColor windowFrameColor], &r, &g, &b, &a);
	// 	cocoaGetByteRGBAFromNSColor([NSColor windowBackgroundColor], &r, &g, &b, &a);
	iupGlobalSetDefaultColorAttrib("DLGBGCOLOR", r, g, b);
	
	cocoaGetByteRGBAFromNSColor([NSColor windowFrameTextColor], &r, &g, &b, &a);
	
	iupGlobalSetDefaultColorAttrib("DLGFGCOLOR", r, g, b);
	
	
	
	
	cocoaGetByteRGBAFromNSColor([NSColor textBackgroundColor], &r, &g, &b, &a);
	
	iupGlobalSetDefaultColorAttrib("TXTBGCOLOR", r, g, b);
	
	cocoaGetByteRGBAFromNSColor([NSColor controlTextColor], &r, &g, &b, &a);
	
	iupGlobalSetDefaultColorAttrib("TXTFGCOLOR", r, g, b);
	
	// FIXME: I don't know where these values came from
	iupGlobalSetDefaultColorAttrib("MENUBGCOLOR", 183,183,183);
	
	
	cocoaGetByteRGBAFromNSColor([NSColor controlTextColor], &r, &g, &b, &a);
	
	iupGlobalSetDefaultColorAttrib("MENUFGCOLOR", r, g, b);
}

int iupdrvOpen(int *argc, char ***argv)
{                        
  (void)argc; /* unused in the mac driver */
  (void)argv;

	// Assuming we're always on the main thread.
	// This will be using a singleton pattern depending if iupdrvClose drains it or not.
	// Not using dispatch_once thinking about GNUStep
	if(nil == s_autoreleasePool)
	{
		s_autoreleasePool = [[NSAutoreleasePool alloc] init];
	}

	[NSApplication sharedApplication];
//	[NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
	/*
	id menubar = [[NSMenu new] autorelease];
	id appMenuItem = [[NSMenuItem new] autorelease];
	[menubar addItem:appMenuItem];
	[NSApp setMainMenu:menubar];
	id appMenu = [[NSMenu new] autorelease];
	id appName = [[NSProcessInfo processInfo] processName];
	id quitTitle = [@"Quit " stringByAppendingString:appName];
	id quitMenuItem = [[[NSMenuItem alloc] initWithTitle:quitTitle
												  action:@selector(terminate:) keyEquivalent:@"q"] autorelease];
	[appMenu addItem:quitMenuItem];
	[appMenuItem setSubmenu:appMenu];
	 */
	/*
	id window = [[[NSWindow alloc] initWithContentRect:NSMakeRect(0, 0, 200, 200)
											 styleMask:NSTitledWindowMask backing:NSBackingStoreBuffered defer:NO]
				 autorelease];
	[window cascadeTopLeftFromPoint:NSMakePoint(20,20)];
	[window setTitle:appName];
	[window makeKeyAndOrderFront:nil];
	 */
//	[NSApp activateIgnoringOtherApps:YES];
	if(nil == s_appDelegate)
	{
		s_appDelegate = [[IupAppDelegate alloc] init];
	}
	[NSApp setDelegate:s_appDelegate];

	
	// TODO: Is it possible to support 10.12 automatic window tabbing?
	if([NSWindow respondsToSelector:@selector(setAllowsAutomaticWindowTabbing:)])
	{
		[NSWindow setAllowsAutomaticWindowTabbing:NO];
	}
	
	
//  IupSetGlobal("DRIVER", "MAC");
  IupSetGlobal("DRIVER", "Cocoa");


//  IupSetGlobal("SYSTEMLANGUAGE", iupmacGetSystemLanguage());

  iupmacUpdateGlobalColors();

  IupSetGlobal("_IUP_RESET_GLOBALCOLORS", "YES");  /* will update the global colors when the first dialog is mapped */


	// Not sure if this is the correct thing to do, but the IupTests have a check to for UTF8MODE. All the NSStrings in this implementation use UTF8String and the non-UTF8MODE tests crash.
	
	IupSetInt(NULL, "UTF8MODE", 1);

  return IUP_NOERROR;
}

void iupdrvClose(void)
{
	// My current understanding is that IUP will not clean up our application menu Ihandles. So we need to do it ourselves.
	// By this point, IUP has already cleanup up all its pointers and our code is finally running.
	iupCocoaMenuCleanupApplicationMenu();
	

	// I think the NSStatusItems get cleaned up via Iup because they are IupDialogs and Iup should run through the UnMapMethod.


	[s_appDelegate release];
	s_appDelegate = nil;
	
	// Hmmm...there could a problem. Objects might get called to be Destroyed after the close.
	// They shouldn't do this.
	// But if it happens, maybe we either never drain and do a dispatch_once.
	[s_autoreleasePool drain];
	s_autoreleasePool = nil;
	
	
}
