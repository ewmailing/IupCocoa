/** \file
 * \brief MAC Message Loop
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdio.h>    
#include <string.h>    

#import <UIKit/UIKit.h>

#include "iup.h"
#include "iupcbs.h"

#import "IupAppDelegate.h"

static IFidle s_idleFunction = NULL;
static int ios_main_loop = 0;


void iupdrvSetIdleFunction(Icallback f)
{
	s_idleFunction = (IFidle)f;
}

void IupExitLoop(void)
{
//	[[UIApplication sharedApplication] stop:nil];
}
/*
static int macLoopProcessMessage(UIEvent *event)
{
    [[UIApplication sharedApplication] sendEvent:event];
    return IUP_DEFAULT;
}
 */

int IupMainLoopLevel(void)
{
  return ios_main_loop;
}


int IupMainLoop(void)
{
	@autoreleasepool
	{
		return UIApplicationMain(0, NULL, nil, NSStringFromClass([IupAppDelegate class]));
	}
	return IUP_NOERROR;
}


int IupLoopStepWait(void)
{
/*
  UIEvent *event;
  event = [[UIApplication sharedApplication]
	                nextEventMatchingMask:NSAnyEventMask
	                untilDate:[NSDate distantFuture]
	                inMode:NSDefaultRunLoopMode
	                dequeue:YES];
  if (macLoopProcessMessage(event) == IUP_CLOSE)  
    return IUP_CLOSE;
*/
  return IUP_DEFAULT;
}

int IupLoopStep(void)
{
/*
  UIEvent *event; 
  event = [[UIApplication sharedApplication]
	                nextEventMatchingMask:NSAnyEventMask
	                untilDate:[NSDate dateWithTimeIntervalSinceNow:0.0]
	                inMode:NSDefaultRunLoopMode
	                dequeue:YES];
  if(event != nil)
    return macLoopProcessMessage(event);
*/
  return IUP_DEFAULT;
}

void IupFlush(void)
{
#if 0
  int post_quit = 0;
  UIEvent *event;
  while(1) { 
    event = [[UIApplication sharedApplication]
	                nextEventMatchingMask:NSAnyEventMask
	                untilDate:[NSDate dateWithTimeIntervalSinceNow:0.0]
	                inMode:NSDefaultRunLoopMode
	                dequeue:YES];
	if (event==nil) break;
	if (macLoopProcessMessage(event) == IUP_CLOSE)
    {
      post_quit = 1;
      break;
    }  
  }
  
  /* re post the quit message if still inside MainLoop */
  if (post_quit && mac_main_loop>0)
	[[UIApplication sharedApplication] terminate:nil];
#endif
}

