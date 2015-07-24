/** \file
 * \brief Timer for the Mac Driver.
 *
 * See Copyright Notice in "iup.h"
 */


// TODO: FEATURE: Support Apple 'tolerance' property which controls battery vs. accuracy.


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

@interface IupCocoaTimerController : NSObject
// CFTimeInterval is a double
@property (assign) CFTimeInterval startTime;
@property (retain) NSTimer* theTimer;
- (void) onTimerCallback:(NSTimer*)theTimer;
@end

@implementation IupCocoaTimerController
@synthesize theTimer;

- (void) onTimerCallback:(NSTimer*)theTimer
{
  Icallback callback_function;
  Ihandle* ih = (Ihandle*)[[[self theTimer] userInfo] pointerValue];
  callback_function = IupGetCallback(ih, "ACTION_CB");
	
  if(callback_function)
  {
	  CFTimeInterval start_time = [self startTime];
	  double current_time = CACurrentMediaTime();
	  NSUInteger elapsed_time = (NSUInteger)(((current_time - start_time) * 1000.0) + 0.5);
	  iupAttribSetInt(ih, "ELAPSEDTIME", (int)elapsed_time);
	  
    if(callback_function(ih) == IUP_CLOSE)
	{
		IupExitLoop();
	}
  }
}

@end


void iupdrvTimerRun(Ihandle* ih)
{
  unsigned int time_ms;

  if (ih->handle != nil) /* timer already started */
  {
	  return;
  }
  time_ms = iupAttribGetInt(ih, "TIME");
  if(time_ms > 0)
  {
	  IupCocoaTimerController* timer_controller = [[IupCocoaTimerController alloc] init];
	  // CACurrentMediaTime is tied to a real time clock. It uses mach_absolute_time() under the hood.
	  // GNUStep: Neither of these is likely directly portable (CACurrentMediaTime more likely), so we may need an #ifdef here.
	  // [[NSDate date] timeIntervalSince1970]; isn't so great because it is affected by network clock changes and so forth.
	  double start_time = CACurrentMediaTime();

	  NSTimer* the_timer = [NSTimer scheduledTimerWithTimeInterval:(time_ms/1000.0)
		target:timer_controller
        selector:@selector(onTimerCallback:)
        userInfo:(id)[NSValue valueWithPointer:ih]
		repeats:YES
	];
	  

	[[NSRunLoop currentRunLoop] addTimer:the_timer
      forMode:NSModalPanelRunLoopMode];

	[timer_controller setTheTimer:the_timer];
	  [timer_controller setStartTime:start_time];

	  ih->handle = (__unsafe_unretained void*)timer_controller;
  }
	
}

void iupdrvTimerStop(Ihandle* ih)
{


	if(nil != ih->handle)
	{
		IupCocoaTimerController* timer_controller = (IupCocoaTimerController*)ih->handle;
		NSTimer* the_timer = [timer_controller theTimer];
	  
		[the_timer invalidate];
		
		// This will also release the timer instance
		[timer_controller release];
		
		ih->handle = nil;
	}

}

void iupdrvTimerInitClass(Iclass* ic)
{
	(void)ic;
}


