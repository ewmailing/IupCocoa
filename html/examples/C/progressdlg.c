#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include "iup.h"

static int cancel_cb(Ihandle* ih)
{
  int ret = IupAlarm("Warning!", "Interrupt Processing?", "Yes", "No", NULL);
  if (ret == 1) /* Yes Interrupt */
  {
    IupSetAttribute(ih, "SIMULATEMODAL", "No");
    IupHide(ih);
    return IUP_DEFAULT;
  }

  return IUP_CONTINUE;
}

static Ihandle* counter_dlg = NULL;

static void counter_start(int totalcount)
{
  if (!counter_dlg)
  {
    counter_dlg = IupProgressDlg();

    //  IupSetAttribute(counter_dlg, "PARENTDIALOG", "MainWindow");
    IupSetAttribute(counter_dlg, "TITLE", "Long Processing Test");
    IupSetAttribute(counter_dlg, "DESCRIPTION", "Description first line\nSecond Line");
    IupSetCallback(counter_dlg, "CANCEL_CB", cancel_cb);
    IupSetInt(counter_dlg, "TOTALCOUNT", totalcount);
    IupSetAttribute(counter_dlg, "RESIZE", "YES");
//    IupSetAttribute(counter_dlg, "MINCLOCK", "500");
//    IupSetAttribute(counter_dlg, "MINPERCENT", "20");
  }

  IupSetAttribute(counter_dlg, "STATE", NULL);

  IupShow(counter_dlg);

  IupSetAttribute(counter_dlg, "SIMULATEMODAL", "Yes");

//  Ihandle* main_window = IupGetHandle("MainWindow");
//  IupSetAttribute(main_window, "TASKBARPROGRESSSTATE", "NORMAL");
//  IupSetAttribute(main_window, "TASKBARPROGRESSVALUE", "0");

  printf("Start\n");
}

static int counter_inc(void)
{
  IupSetAttribute(counter_dlg, "INC", NULL);
//  IupSetInt(counter_dlg, "COUNT", count);

  if (strcmp(IupGetAttribute(counter_dlg, "STATE"), "ABORTED") == 0)
  {
    printf("Interrupted\n");
    return 0;
  }

  IupLoopStep();

//  Ihandle* main_window = IupGetHandle("MainWindow");
//  IupSetInt(main_window, "TASKBARPROGRESSVALUE", (IupGetInt(counter_dlg, "COUNT") * 100) / IupGetInt(counter_dlg, "TOTALCOUNT"));

  printf("Step\n");
  return 1;
}

static void counter_stop(void)
{
  // force one last update
  IupSetInt(counter_dlg, "COUNT", IupGetInt(counter_dlg, "TOTALCOUNT"));

  IupSetAttribute(counter_dlg, "SIMULATEMODAL", "No");

  IupHide(counter_dlg);

//  Ihandle* main_window = IupGetHandle("MainWindow");
//  IupSetAttribute(main_window, "TASKBARPROGRESSVALUE", "100");
//  IupSetAttribute(main_window, "TASKBARPROGRESSSTATE", "NOPROGRESS");

  printf("Stop\n");
}

static void counter_release(void)
{
  if (counter_dlg)
    IupDestroy(counter_dlg);
  counter_dlg = NULL;
}

void LongProcessing(void)
{
  int i, j;

  counter_start(10000);

  for (i = 0; i < 10000; i++)
  {
    for (j = 0; j < 10000; j++)
    {
      double x = fabs(sin(j * 100.) + cos(j * 100.));
      x = sqrt(x * x);
    }

    if (!counter_inc())
      break;
  }

  counter_stop();
}

int main(int argc, char* argv[])
{
  IupOpen(&argc, &argv);

  // CreateMainDialog();

  LongProcessing();

  IupMainLoop();

  counter_release();

  IupClose();

  return EXIT_SUCCESS;
}
