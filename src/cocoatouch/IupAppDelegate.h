//
//  IupAppDelegate.h
//  iup
//
//  Created by Eric Wing on 12/14/16.
//  Copyright © 2016 Tecgraf, PUC-Rio, Brazil. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "IupAppDelegateProtocol.h"

typedef void (^IUPClosure)();
typedef struct Ihandle_ Ihandle;

// If the app is initialized execute the closure immediately, else add it to an array to do
// when the app is initialized.
void IupAppExecuteWhenReady(IUPClosure closure);

@interface IupAppDelegate : UIResponder <UIApplicationDelegate, IupAppDelegateProtocol>

// TODO: Consider advanced APIs that allow multiple UIWindows instead of ViewControllers
@property (strong, nonatomic) UIWindow* window;

- (UIWindow*) currentWindow;

@end
