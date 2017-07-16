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
#include "iup_loop.h"


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
	iupLoopCallEntryCb();
	
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

// Note: This will almost never be called
- (void) applicationWillTerminate:(UIApplication*)the_application
{
	// Called when the application is about to terminate. Save data if appropriate. See also applicationDidEnterBackground:.

	// Reminder: Users should not rely on this being called
	iupLoopCallExitCb();
}

// This is intended to allow us to use multiple UIWindows in the future.
- (UIWindow*) currentWindow
{
	return [self window];
}

@end
