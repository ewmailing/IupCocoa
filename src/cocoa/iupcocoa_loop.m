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
#include "iupcocoa_drv.h"

static IFidle mac_idle_cb = NULL;
static int mac_main_loop = 0;


void iupdrvSetIdleFunction(Icallback f)
{
  mac_idle_cb = (IFidle)f;
}

void IupExitLoop(void)
{
	if([NSApp isRunning])
	{
		// This is trickier than it should be...
		// If IUP triggered the exit by calling IupExitLoop (via IUP_CLOSE directive)
		// then this branch is taken and we need to call [NSApp stop:nil] to quit.
		// But if the user quits naturally, e.g. Quit menu,
		// then applicationWillTerminate is invoked and [NSApp isRunning] is false, and the else case hits instead.
		if(mac_main_loop <= 1)
		{
			//  [NSApp terminate:nil];
			iupCocoaCommonLoopCallExitCb();
			[NSApp stop:nil];
		}
		else
		{
			mac_main_loop--;

		}
	}
	else
	{
		// This path could be invoked when the application is quit normally (e.g. Quit menu).
		mac_main_loop--;

	}
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
	if(![NSApp isRunning])
	{
		mac_main_loop++;
		[NSApp run];
		// [NSApp run] blocks until the event loop is stopped
		mac_main_loop--;

	}
	else
	{
		NSLog(@"IupMainLoop called again");
		mac_main_loop++;
	}
	
	// Unfortunately, applicationWillTerminate: will not always be called depending how the quit was invoked.
	// Let's try to guard against calling the exit callback too often.
	if(0 == mac_main_loop)
	{
		iupCocoaCommonLoopCallExitCb();
	}
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

/* TODO: Make decision on final API. For now, this API is just to get a usable demo. */
void IupPostMessage(Ihandle* ih, char* unusedchar, void* message_data, int unusedint)
{
	dispatch_async(dispatch_get_main_queue(),
		^{
			//NSLog(@"dispatch_async fired on main_queue");
			IFnsVi post_message_callback = (IFnsVi)IupGetCallback(ih, "POSTMESSAGE_CB");
			if (post_message_callback)
			{
				//post_message_callback(ih, NULL, message_data, 0);
				post_message_callback(ih, unusedchar, message_data, unusedint);
			}
		}
	);
}

