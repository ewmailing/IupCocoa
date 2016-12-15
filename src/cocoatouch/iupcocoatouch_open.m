/** \file
 * \brief MAC Driver Core
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdio.h>          
#include <stdlib.h>
#include <string.h>          
#import <UIKit/UIKit.h>

#include "iup.h"

#include "iup_str.h"
#include "iup_drv.h"
#include "iup_drvinfo.h"
#include "iup_object.h"
#include "iup_globalattrib.h"

#include "iupcocoatouch_drv.h"

static NSAutoreleasePool* s_autoreleasePool = nil;

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


void iupmacUpdateGlobalColors(void)
{
  iupGlobalSetDefaultColorAttrib("DLGBGCOLOR", 237,237,237);

  iupGlobalSetDefaultColorAttrib("DLGFGCOLOR", 0,0,0);

  iupGlobalSetDefaultColorAttrib("TXTBGCOLOR", 255,255,255);

  iupGlobalSetDefaultColorAttrib("TXTFGCOLOR", 0,0,0);

  iupGlobalSetDefaultColorAttrib("MENUBGCOLOR", 183,183,183);

  iupGlobalSetDefaultColorAttrib("MENUFGCOLOR", 0,0,0);
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

	[UIApplication sharedApplication];
//	[[UIApplication sharedApplication] setActivationPolicy:[UIApplication sharedApplication]licationActivationPolicyRegular];
	/*
	id menubar = [[NSMenu new] autorelease];
	id appMenuItem = [[NSMenuItem new] autorelease];
	[menubar addItem:appMenuItem];
	[[UIApplication sharedApplication] setMainMenu:menubar];
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
//	[[UIApplication sharedApplication] activateIgnoringOtherApps:YES];
	
	
	
  IupSetGlobal("DRIVER", "MAC");

//  IupSetGlobal("SYSTEMLANGUAGE", iupmacGetSystemLanguage());

  iupmacUpdateGlobalColors();

  IupSetGlobal("_IUP_RESET_GLOBALCOLORS", "YES");  /* will update the global colors when the first dialog is mapped */

  return IUP_NOERROR;
}

void iupdrvClose(void)
{

	// Hmmm...there could a problem. Objects might get called to be Destroyed after the close.
	// They shouldn't do this.
	// But if it happens, maybe we either never drain and do a dispatch_once.
	[s_autoreleasePool drain];
	s_autoreleasePool = nil;
	
	
}
