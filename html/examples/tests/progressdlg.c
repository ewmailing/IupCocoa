#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include "iup.h"

//#define TIMER
#ifdef TIMER
static int time_cb(Ihandle* timer)
{
  Ihandle* ih = (Ihandle*)IupGetAttribute(timer, "DIALOG");
  IupSetAttribute(ih, "INC", NULL);
  if (IupGetInt(ih, "COUNT")==IupGetInt(ih, "TOTALCOUNT"))
  {
    IupSetAttribute(timer, "RUN", "NO");
#ifdef TIMER
    IupDestroy(timer);
#endif

    return IUP_DEFAULT;
  }
  return IUP_DEFAULT;
}
#else
static int show_cb(Ihandle* ih, int state)
{
  if (state == IUP_SHOW)
  {
    int i, j;
    printf("Start\n");
    for (i = 0; i < 10000; i++)
    {
      for (j = 0; j < 10000; j++)
      {
        double x = fabs(sin(j * 100.) + cos(j * 100.));
        x = sqrt(x * x);
      }

      IupSetAttribute(ih, "INC", NULL);

      if (strcmp(IupGetAttribute(ih, "STATE"), "ABORTED") == 0)
        return IUP_DEFAULT;

      IupLoopStep();

      printf("Step(%d)\n", i);
    }
    printf("Stop\n");
  }
  return IUP_DEFAULT;
}
#endif

static int cancel_cb(Ihandle* ih)
{
  int ret = 1;
#ifdef TIMER
  Ihandle* timer = (Ihandle*)IupGetAttribute(ih, "TIMER");
  IupSetAttribute(timer, "RUN", "NO");
#endif
  ret = IupAlarm("Warning!", "Interrupt Processing?", "Yes", "No", NULL);
  if (ret == 1) /* Yes Interrupt */
  {
    if (IupGetInt(ih, "COUNT")>IupGetInt(ih, "TOTALCOUNT")/2)
    {
      if (strcmp(IupGetAttribute(ih, "STATE"), "UNDEFINED") == 0)
      {
        // IupHide(ih);
        IupExitLoop();
        return IUP_DEFAULT;
      }

      IupSetAttribute(ih, "STATE", "UNDEFINED");
      return IUP_CONTINUE;
    }

//    IupHide(ih);
    IupExitLoop();
    return IUP_DEFAULT;
  }
  
  IupSetAttribute(ih, "STATE", "PROCESSING");
#ifdef TIMER
  IupSetAttribute(timer, "RUN", "Yes");
#endif
  return IUP_CONTINUE;
}

void ProgressDlgTest(void)
{
#ifdef TIMER
  Ihandle* timer;
#endif
  Ihandle* dlg = IupProgressDlg();

  IupSetAttribute(dlg, "TITLE", "IupProgressDlg Test");
  IupSetAttribute(dlg, "DESCRIPTION", "Description first line\nSecond Line");
  IupSetCallback(dlg, "CANCEL_CB", cancel_cb);
  IupSetAttribute(dlg, "TOTALCOUNT", "10000");
  IupSetAttribute(dlg, "RESIZE", "YES");

#ifdef TIMER
  timer = IupTimer();
  IupSetCallback(timer, "ACTION_CB", (Icallback)time_cb);
  IupSetAttribute(timer, "TIME", "100");
  IupSetAttribute(timer, "RUN", "YES");
  IupSetAttribute(timer, "DIALOG", (char*)dlg);
  IupSetAttribute(dlg, "TIMER", (char*)timer);
#else
  IupSetCallback(dlg, "SHOW_CB", (Icallback)show_cb);
#endif

  //IupShow(dlg);
  IupPopup(dlg, IUP_CENTER, IUP_CENTER);
}

#ifndef BIG_TEST
int main(int argc, char* argv[])
{
  IupOpen(&argc, &argv);
  IupControlsOpen();

  ProgressDlgTest();

  IupMainLoop();

  IupClose();

  return EXIT_SUCCESS;
}
#endif
