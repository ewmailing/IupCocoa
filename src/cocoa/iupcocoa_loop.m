/** \file
 * \brief MAC Message Loop
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdio.h>    
#include <string.h>    

#import <Cocoa/Cocoa.h>

#include "iup.h"
#include "iupcbs.h"


static IFidle mac_idle_cb = NULL;
static int mac_main_loop = 0;


void iupdrvSetIdleFunction(Icallback f)
{
  mac_idle_cb = (IFidle)f;
}

void IupExitLoop(void)
{
  [NSApp terminate:nil];
}

static int macLoopProcessMessage(NSEvent *event)
{
    [NSApp sendEvent:event];
    return IUP_DEFAULT;
}

int IupMainLoopLevel(void)
{
  return mac_main_loop;
}

#if 1
int IupMainLoop(void)
{
	[NSApp run];
	return IUP_NOERROR;

}
#else
int IupMainLoop(void)
{
  NSEvent *event;
  int ret;

  mac_main_loop++;

  do 
  {
    if (mac_idle_cb)
    {
      ret = 1;          
	  event = [NSApp
	                nextEventMatchingMask:NSAnyEventMask
	                untilDate:[NSDate dateWithTimeIntervalSinceNow:0.0]
	                inMode:NSDefaultRunLoopMode
	                dequeue:YES];
	  if(event != nil)
	  {
        if (macLoopProcessMessage(event) == IUP_CLOSE)
        {
          mac_main_loop--;
          return IUP_CLOSE;
        }
      }
      else
      {
        int ret = mac_idle_cb();
        if (ret == IUP_CLOSE)
        {
          mac_idle_cb = NULL;
          mac_main_loop--;
          return IUP_CLOSE;
        }
        if (ret == IUP_IGNORE) 
          mac_idle_cb = NULL;
      }
    }
    else
    {          
	 	event = [NSApp
		                nextEventMatchingMask:NSAnyEventMask
		                untilDate:[NSDate distantFuture]
		                inMode:NSDefaultRunLoopMode
		                dequeue:YES];
      if (macLoopProcessMessage(event) == IUP_CLOSE)  
      {
        mac_main_loop--;
        return IUP_NOERROR;
      }
    }
  } while (ret);

  mac_main_loop--;
  return IUP_NOERROR;
}
#endif


int IupLoopStepWait(void)
{
  NSEvent *event; 
  event = [NSApp
	                nextEventMatchingMask:NSAnyEventMask
	                untilDate:[NSDate distantFuture]
	                inMode:NSDefaultRunLoopMode
	                dequeue:YES];
  if (macLoopProcessMessage(event) == IUP_CLOSE)  
    return IUP_CLOSE;
  return IUP_DEFAULT;
}

int IupLoopStep(void)
{
  NSEvent *event; 
  event = [NSApp
	                nextEventMatchingMask:NSAnyEventMask
	                untilDate:[NSDate dateWithTimeIntervalSinceNow:0.0]
	                inMode:NSDefaultRunLoopMode
	                dequeue:YES];
  if(event != nil)
    return macLoopProcessMessage(event);

  return IUP_DEFAULT;
}

void IupFlush(void)
{
  int post_quit = 0;
  NSEvent *event;
  while(1) { 
    event = [NSApp
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
	[NSApp terminate:nil];
}

