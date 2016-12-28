//
//  IupAppDelegate.m
//  iup
//
//  Created by Eric Wing on 12/14/16.
//  Copyright © 2016 Tecgraf, PUC-Rio, Brazil. All rights reserved.
//

#import <UIKit/UIKit.h>

#import "IupAppDelegate.h"
#import "IupLaunchViewController.h"

#include "iup.h"
#include "iupcbs.h"

//extern void IupEntry();

/*
 ** {========================================================================
 ** This is an implementation to fake dlopen with static libraries for iOS.
 ** It requires static linking of the module (and making sure the symbols are
 ** not stripped.
 ** dlopen is not used to avoid trouble with Apple.
 ** dlsym(RTLD_DEFAULT) is used to find the symbol in the application binary.
 ** To the caller, it looks like require successfully loaded the module.
 ** =========================================================================
 */


#include <dlfcn.h>

/*
 ** Scary: Looks like Apple moved the definition of RTLD_DEFAULT behind #ifdefs
 ** which are not defined in the default case for 8.1.
 ** So I'm copying the definition and hoping Apple didn't cripple the backend.
 */
#ifndef RTLD_DEFAULT
#define RTLD_DEFAULT    ((void *) -2)   /* Use default search algorithm. */
#endif




@implementation IupAppDelegate


- (BOOL) application:(UIApplication*)the_application didFinishLaunchingWithOptions:(NSDictionary*)launch_options
{
	CGRect window_bounds = [[UIScreen mainScreen] bounds];
	UIWindow* the_window = [[UIWindow alloc] initWithFrame:window_bounds];
	
	IupLaunchViewController* view_controller = [[[IupLaunchViewController alloc] init] autorelease];
	[the_window setRootViewController:view_controller];

	[self setWindow:the_window];
	[the_window makeKeyAndVisible];

	
	
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
	
	return YES;
}

- (void) applicationWillResignActive:(UIApplication*)the_application
{
	// Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
	// Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
}

- (void) applicationDidEnterBackground:(UIApplication*)the_application
{
	// Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later.
	// If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
}

- (void) applicationWillEnterForeground:(UIApplication*)the_application
{
	// Called as part of the transition from the background to the inactive state; here you can undo many of the changes made on entering the background.
}

- (void) applicationDidBecomeActive:(UIApplication*)the_application
{
	// Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
}

- (void) applicationWillTerminate:(UIApplication*)the_application
{
	// Called when the application is about to terminate. Save data if appropriate. See also applicationDidEnterBackground:.
}

// This is intended to allow us to use multiple UIWindows in the future.
- (UIWindow*) currentWindow
{
	return [self window];
}

@end
