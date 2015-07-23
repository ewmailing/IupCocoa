/** \file
 * \brief GTK Message Loop
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdio.h>    
#include <string.h>    

#include <Application.h>
#include <MessageQueue.h>
#include <Window.h>

#include "iup.h"
#include "iupcbs.h"

#define UNIMPLEMENTED printf("%s (%s %d) UNIMPLEMENTED\n",__func__,__FILE__,__LINE__);

static IFidle haiku_idle_cb = NULL;

extern "C" {
void iupdrvSetIdleFunction(Icallback f)
{
  haiku_idle_cb = (IFidle)f;
  if (f != NULL)
    UNIMPLEMENTED
}
}

void IupExitLoop(void)
{
  be_app->PostMessage(B_QUIT_REQUESTED);
}

int IupMainLoopLevel(void)
{
	UNIMPLEMENTED
		return 0;
}

static bool isRunning = false;

int IupMainLoop(void)
{
  // This is called for the main window (and we start the application), but
  // also for all modal dialogs. We can only start the application once, so
  // when this function gets called again it should instead wait for the
  // last opened dialog to close...
  // TODO how do we manage the callback from setIdleFunction ?
  if(!isRunning)
  {
	isRunning = true;
    be_app->Run();
  } else {
	// FIXME not so good design here. We're locking the calling thread (likely
	// a BWindow) which means it will not be able to receive any messages as
	// long as the modal dialog is open. This includes redraw messages as well
	// so the window will get drawing artifacts...
	// But if we return from this method, the dialog will be closed by IUP.
	// We likely need to do some changes to IUP dialog code to get it right.
    BWindow* win = be_app->WindowAt(be_app->CountWindows() - 1);
	thread_id tid = win->Thread();
	status_t result;
	wait_for_thread(tid, &result);
  }
  return IUP_NOERROR;
}

int IupLoopStepWait(void)
{
	UNIMPLEMENTED
  return IUP_DEFAULT;
}

int32 appLoop(void*)
{
	be_app->LockLooper();
	be_app->Run();
}

int IupLoopStep(void)
{
  if(!isRunning) {
	isRunning = true;
	thread_id id = spawn_thread(appLoop, "IUP Application", B_NORMAL_PRIORITY, NULL);
	resume_thread(id);
	be_app->UnlockLooper();
  }

  BLooper* looper = BLooper::LooperForThread(find_thread(NULL));
  if (looper == NULL)
	  return IUP_DEFAULT;

  BMessage* msg = looper->MessageQueue()->NextMessage();
  if (msg != NULL) {
	msg->PrintToStream();
    looper->MessageReceived(msg);
  } else {
	puts("no messages");
  }
  return IUP_DEFAULT;
}

void IupFlush(void)
{
	// This is meant to flush the event loop. Since the windows each run in
	// their own thread, we can't do much anyway. Let's see later if this is a
	// real problem...
}
