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

@implementation IupAppDelegate


- (void) applicationDidFinishLaunching:(NSNotification*)a_notification
{
	// Invoke the IupEntry callback function to start the user code.
	IFentry entry_callback = (IFentry)IupGetFunction("ENTRY_POINT");
	
	// If no entry point has been defined, we can try to fallback and use dsym to look up a hardcoded function name.
	if(NULL == entry_callback)
	{
		entry_callback = (IFentry)dlsym(RTLD_DEFAULT, "IupEntryPoint");
	}
	
	if(NULL != entry_callback)
	{
		entry_callback();
	}
	
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
