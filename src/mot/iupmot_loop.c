/** \file
 * \brief Motif Message Loop
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdio.h>    

#include <Xm/Xm.h>

#include "iup.h"
#include "iupcbs.h"
#include "iup_loop.h"

#include "iupmot_drv.h"


/* local variables */
static int mot_mainloop = 0;
static int mot_exitmainloop = 0;
static IFidle mot_idle_cb = NULL;
static XtWorkProcId mot_idle_id;


static Boolean motIdlecbWorkProc(XtPointer client_data)
{
  (void)client_data;
  if (mot_idle_cb)
  {
    int ret = mot_idle_cb();
    if (ret == IUP_CLOSE)
    {
      mot_idle_cb = NULL;
      IupExitLoop();
      return True; /* removes the working procedure */
    }
    if (ret == IUP_IGNORE)
    {
      mot_idle_cb = NULL;
      return True; /* removes the working procedure */
    }

    return False; /* keeps the working procedure */
  }

  return True; /* removes the working procedure */
}

void iupdrvSetIdleFunction(Icallback f)
{
  if (mot_idle_cb)
    XtRemoveWorkProc(mot_idle_id);

  mot_idle_cb = (IFidle)f;

  if (mot_idle_cb)
    mot_idle_id = XtAppAddWorkProc(iupmot_appcontext, motIdlecbWorkProc, NULL);
}

static int motLoopProcessEvent(void)
{
  XtAppProcessEvent(iupmot_appcontext, XtIMAll);
  return (mot_exitmainloop)? IUP_CLOSE : IUP_DEFAULT;
}

void IupExitLoop(void)
{
  mot_exitmainloop = 1;
}

int IupMainLoopLevel(void)
{
  return mot_mainloop;
}

int IupMainLoop(void)
{
  static int has_done_entry = 0;
  if (0 == has_done_entry)
  {
    iupLoopCallEntryCb();
    has_done_entry = 1;
  }

  mot_mainloop++;
  mot_exitmainloop = 0;

  while (!mot_exitmainloop)
  {
    if (motLoopProcessEvent() == IUP_CLOSE)
      break;
  }

  mot_exitmainloop = 0;
  mot_mainloop--;

  if(0 == mot_mainloop)
  {
    iupLoopCallExitCb();
  }
  return IUP_NOERROR;
}

int IupLoopStepWait(void)
{
  while(!XtAppPending(iupmot_appcontext));

  return motLoopProcessEvent();
}

int IupLoopStep(void)
{
  if (!XtAppPending(iupmot_appcontext)) 
    return IUP_DEFAULT;

  return motLoopProcessEvent();
}

void IupFlush(void)
{
  int count = 0;

  while (count<100 && XtAppPending(iupmot_appcontext) != 0)
  {
    if (motLoopProcessEvent() == IUP_CLOSE)
      break;

    count++;
  }
}
