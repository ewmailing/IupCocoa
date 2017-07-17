//
//  IupAppDelegate.m
//  iup
//
//  Created by Eric Wing on 12/23/16.
//  Copyright © 2016 Tecgraf, PUC-Rio, Brazil. All rights reserved.
//

#import "IupAppDelegate.h"
#include <dlfcn.h>
#include "iup.h"
#include "iupcbs.h"
#include "iup_loop.h"
#include "iupcocoa_drv.h"

@implementation IupAppDelegate


- (void) applicationDidFinishLaunching:(NSNotification*)a_notification
{
	// Invoke the IupEntry callback function to start the user code.
	iupLoopCallEntryCb();
	
}

// Drat: Due to the way IupExitLoop() can be called to quit a program,
// applicationWillTerminate: is not guaranteed to be invoked depending on the quit.
// It seems that this will be called when using Cmd-Q (Menu quit)
// But this will not be called when you use IUP_CLOSE or close the last window (IUP will automatically shutdown. We do not use applicationShouldTerminateAfterLastWindowClosed: because of that.)
- (void) applicationWillTerminate:(NSNotification*)a_notification
{
	// Invoke the IupEntry callback function to start the user code.
	iupCocoaCommonLoopCallExitCb();
}


@end
