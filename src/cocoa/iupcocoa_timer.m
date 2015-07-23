/** \file
 * \brief Timer for the Mac Driver.
 *
 * See Copyright Notice in "iup.h"
 */

#import <Cocoa/Cocoa.h>
 
#include <stdio.h>
#include <stdlib.h>

#include "iup.h"
#include "iupcbs.h"

#include "iup_object.h"
#include "iup_attrib.h"
#include "iup_str.h"
#include "iup_assert.h"
#include "iup_timer.h"

@interface MyTimerController : NSObject {
  NSTimer *timer;
}
@property (retain)NSTimer *timer;
- (void)targetMethod:(NSTimer*)theTimer;
@end
@implementation MyTimerController
@synthesize timer;
- (void)targetMethod:(NSTimer*)theTimer {
  Icallback cb;
  Ihandle* ih = (Ihandle*)[[theTimer userInfo] pointerValue];
  cb = IupGetCallback(ih, "ACTION_CB");
  if(cb)
  {
    if (cb(ih) == IUP_CLOSE)
      IupExitLoop();
  }
}

void iupdrvTimerRun(Ihandle *ih)
{
	// BROKEN: serial is an int which is not large enough to hold a pointer
#if 0
  unsigned int time_ms;

  if (ih->serial != 0) /* timer already started */
    return;

  time_ms = iupAttribGetInt(ih, "TIME");
  if (time_ms > 0)
  {
    MyTimerController *timerController = [MyTimerController new];
    NSTimer *timer = [NSTimer scheduledTimerWithTimeInterval:(time_ms/1000.0) target:timerController 
        selector:@selector(targetMethod:)
        userInfo:(id)[NSValue valueWithPointer:ih] repeats:YES];
    [[NSRunLoop currentRunLoop] addTimer:timer
      forMode:NSModalPanelRunLoopMode];
    timerController.timer = timer;
    ih->serial = (int)timerController;
  }
#endif
}

void iupdrvTimerStop(Ihandle* ih)
{
	// BROKEN: serial is an int which is not large enough to hold a pointer
#if 0
  if (ih->serial != 0)
  {
    MyTimerController *timerController = (MyTimerController*)ih->serial;
    [timerController.timer invalidate];
    ih->serial = 0;
  }
#endif
}

void iupdrvTimerInitClass(Iclass* ic)
{
  (void)ic;
}

@end

