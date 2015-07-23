#include <stdlib.h>
#include <stdio.h>

#include "iup.h"


static int time_cb(Ihandle* timer)
{
  Ihandle* ih = (Ihandle*)IupGetAttribute(timer, "DIALOG");
  IupSetAttribute(ih, "INC", NULL);
  if (IupGetInt(ih, "COUNT")==IupGetInt(ih, "TOTALCOUNT"))
  {
    IupSetAttribute(timer, "RUN", "NO");
    IupExitLoop();
    return IUP_DEFAULT;
  }
  return IUP_DEFAULT;
}

static int cancel_cb(Ihandle* ih)
{
  int ret;
  Ihandle* timer = (Ihandle*)IupGetAttribute(ih, "TIMER");
  IupSetAttribute(timer, "RUN", "NO");
  ret = IupAlarm("Warning!", "Interrupt Processing?", "Yes", "No", NULL);
  if (ret == 1) /* Yes Interrupt */
  {
    if (IupGetInt(ih, "COUNT")<IupGetInt(ih, "TOTALCOUNT")/2)
    {
//      IupSetAttribute(ih, "STATE", "UNDEFINED");
  //    return IUP_CONTINUE;
      IupExitLoop();
      return IUP_DEFAULT;
    }
    else
    {
      IupExitLoop();
      return IUP_DEFAULT;
    }
  }
  
  IupSetAttribute(ih, "STATE", "PROCESSING");
  IupSetAttribute(timer, "RUN", "Yes");
  return IUP_CONTINUE;
}

void ProgressDlgTest(void)
{
  Ihandle* timer;
  Ihandle* dlg = IupProgressDlg();
  
  IupSetAttribute(dlg, "TITLE", "IupProgressDlg Test");
  IupSetAttribute(dlg, "DESCRIPTION", "Description first line\nSecond Line");
  IupSetCallback(dlg, "CANCEL_CB", cancel_cb);
  IupSetAttribute(dlg, "TOTALCOUNT", "300");

  timer = IupTimer();
  IupSetCallback(timer, "ACTION_CB", (Icallback)time_cb);
  IupSetAttribute(timer, "TIME", "100");
  IupSetAttribute(timer, "RUN", "YES");
  IupSetAttribute(timer, "DIALOG", (char*)dlg);
  IupSetAttribute(dlg, "TIMER", (char*)timer);

  IupPopup(dlg, IUP_CENTER, IUP_CENTER);

  IupDestroy(timer);
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
