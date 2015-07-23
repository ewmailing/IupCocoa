/** \file
 * \brief Timer for the GTK Driver.
 *
 * See Copyright Notice in "iup.h"
 */

#include <Application.h>
#include <Message.h>
#include <MessageRunner.h>
#include <Window.h>

#include <stdio.h>
#include <stdlib.h>

#include "iup.h"

#include "iup_object.h"
#include "iup_attrib.h"
#include "iup_str.h"
#include "iup_assert.h"
#include "iup_timer.h"

#include "iuphaiku_drv.h"


void iuphaikuTimerFired(Ihandle* ih)
{
  Icallback cb;

  if (!iupObjectCheck(ih)) {
	/* control was destroyed before timer callback */
    return;
  }

  cb = IupGetCallback(ih, "ACTION_CB");
  if (cb && cb(ih)==IUP_CLOSE)
    IupExitLoop();
}

void iupdrvTimerRun(Ihandle *ih)
{
  unsigned int time_ms;

  if (ih->serial > 0) /* timer already started */
    return;
  
  time_ms = iupAttribGetInt(ih, "TIME");
  if (time_ms > 0) {
	BMessage* msg = new BMessage(timerFire);
	msg->AddPointer("iHandle", ih);

    BLooper* looper = be_app->WindowAt(be_app->CountWindows() - 1);
	if (looper == NULL) {
		looper = be_app;
	}

    BMessenger messenger(looper);

	ih->handle = (InativeHandle*)new BMessageRunner(messenger, msg, time_ms * 1000);
	// Just use something "reasonably unique" as the serial...
	ih->serial = (int)ih->handle;
	if (ih->serial < 0) ih->serial = -ih->serial;
  }
}

void iupdrvTimerStop(Ihandle* ih)
{
  if (ih->serial > 0)
  {
	BMessageRunner* runner = (BMessageRunner*)ih->handle;
	delete runner;
    ih->serial = -1;
  }
}

void iupdrvTimerInitClass(Iclass* ic)
{
  (void)ic;
}
