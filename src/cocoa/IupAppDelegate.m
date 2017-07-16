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

@implementation IupAppDelegate


- (void) applicationDidFinishLaunching:(NSNotification*)a_notification
{
	// Invoke the IupEntry callback function to start the user code.
	iupLoopCallEntryCb();
	
}

// Drat: Due to the way IupExitLoop() can be called to quit a program,
// applicationWillTerminate: is not guaranteed to be invoked depending on the quit.
// Put common shutdown stuff in IupMainLoop() instead.
/*
- (void) applicationWillTerminate:(NSNotification*)a_notification
{
	// Invoke the IupEntry callback function to start the user code.
	iupLoopCallExitCb();
}
*/

@end
