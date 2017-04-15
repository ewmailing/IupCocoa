#include <stdlib.h>
#include <stdio.h>

#include <iup.h>
#include <iupcontrols.h>

float increment = 0.01f;
Ihandle *progressbar1;
Ihandle *btn_pause;
Ihandle *timer;

int time_cb(void)
{
  Ihandle *dlg = IupGetHandle("mydialog");
  char str[10];
  float value = IupGetFloat(progressbar1, "VALUE");

  value += increment;
  if (value > 1) value = 0; /* start over */
  IupSetfAttribute(progressbar1, "VALUE", "%g", (double)value);

  IupSetInt(dlg, "TASKBARPROGRESSVALUE", (int)(value * 100));

  return IUP_DEFAULT;
}

int btn_pause_cb(void)
{
  Ihandle *dlg = IupGetHandle("mydialog");
  if (!IupGetInt(timer, "RUN"))
  {
    IupSetAttribute(timer, "RUN", "YES");
    IupSetAttribute(btn_pause, "IMAGE", "IUP_MediaPause");
    IupSetAttribute(dlg, "TASKBARPROGRESSSTATE", "NORMAL");
  }
  else
  {
    IupSetAttribute(timer, "RUN", "NO");
    IupSetAttribute(btn_pause, "IMAGE", "IUP_MediaPlay");
    IupSetAttribute(dlg, "TASKBARPROGRESSSTATE", "PAUSED");
  }
  
  return IUP_DEFAULT;
}

int btn_restart_cb(void)
{
  Ihandle *dlg = IupGetHandle("mydialog");
  IupSetAttribute(progressbar1, "VALUE", "0");
  IupSetAttribute(dlg, "TASKBARPROGRESSSTATE", "NOPROGRESS");
  return IUP_DEFAULT;
}

int btn_accelerate_cb(void)
{
  increment *= 2;
  return IUP_DEFAULT;
}

int btn_decelerate_cb(void)
{
  increment /= 2;
  return IUP_DEFAULT;
}

int btn_indeterminate_cb(void)
{
  Ihandle *dlg = IupGetHandle("mydialog");
  IupSetAttribute(timer, "RUN", "NO");
  IupSetAttribute(btn_pause, "IMAGE", "IUP_MediaPlay");
  IupSetAttribute(dlg, "TASKBARPROGRESSSTATE", "INDETERMINATE");
  return IUP_DEFAULT;
}

int btn_noprogress_cb(void)
{
  Ihandle *dlg = IupGetHandle("mydialog");
  IupSetAttribute(timer, "RUN", "NO");
  IupSetAttribute(btn_pause, "IMAGE", "IUP_MediaPlay");
  IupSetAttribute(dlg, "TASKBARPROGRESSSTATE", "NOPROGRESS");
  return IUP_DEFAULT;
}

int btn_error_cb(void)
{
  Ihandle *dlg = IupGetHandle("mydialog");
  IupSetAttribute(timer, "RUN", "NO");
  IupSetAttribute(btn_pause, "IMAGE", "IUP_MediaPlay");
  IupSetAttribute(dlg, "TASKBARPROGRESSSTATE", "ERROR");
  return IUP_DEFAULT;
}

int main(int argc, char* argv[])
{
  Ihandle *dlg, *vbox, *hbox;
  Ihandle *btn_restart, *btn_accelerate, *btn_decelerate;
  Ihandle *btn_indeterminate, *btn_noprogress, *btn_error;
  
  IupOpen(&argc, &argv);                  /* IUP initialization */

  timer = IupTimer();
  IupSetCallback(timer, "ACTION_CB", (Icallback)time_cb);
  IupSetAttribute(timer, "TIME", "100");
  
  progressbar1 = IupProgressBar();
 
  IupSetAttribute(progressbar1, "EXPAND", "YES");
  IupSetAttribute(progressbar1, "DASHED", "YES");

  btn_restart = IupButton(NULL, NULL);
  btn_pause = IupButton(NULL, NULL);
  btn_accelerate = IupButton(NULL, NULL);
  btn_decelerate = IupButton(NULL, NULL);
  btn_indeterminate = IupButton(NULL, NULL);
  btn_noprogress = IupButton(NULL, NULL);
  btn_error = IupButton(NULL, NULL);

  IupImageLibOpen();

  IupSetAttribute(btn_restart, "IMAGE", "IUP_MediaGotoBegin");
  IupSetAttribute(btn_restart, "TIP", "Restart" );
  IupSetAttribute(btn_pause, "IMAGE", "IUP_MediaPause");
  IupSetAttribute(btn_pause, "TIP", "Play/Pause");
  IupSetAttribute(btn_accelerate, "IMAGE", "IUP_MediaForward");
  IupSetAttribute(btn_accelerate, "TIP", "Accelerate");
  IupSetAttribute(btn_decelerate, "IMAGE", "IUP_MediaRewind");
  IupSetAttribute(btn_decelerate, "TIP", "Decelerate");
  IupSetAttribute(btn_indeterminate, "IMAGE", "IUP_ToolsSettings");
  IupSetAttribute(btn_indeterminate, "TIP", "Indeterminate" );
  IupSetAttribute(btn_noprogress, "IMAGE", "IUP_MessageInfo");
  IupSetAttribute(btn_noprogress, "TIP", "No Progress");
  IupSetAttribute(btn_error, "IMAGE", "IUP_MessageError");
  IupSetAttribute(btn_error, "TIP", "Error");

  hbox = IupHbox
         (
           IupFill(), 
           btn_pause,
           btn_restart,
           btn_decelerate,
           btn_accelerate,
           btn_noprogress,
           btn_indeterminate,
           btn_error,
           IupFill(),
           NULL
         );

  vbox = IupHbox(IupVbox(progressbar1, hbox, NULL), NULL);
  IupSetAttribute(vbox, "MARGIN", "10x10");
  IupSetAttribute(vbox, "GAP", "5");

  dlg = IupDialog(vbox);
  
  IupSetAttribute(dlg, "TITLE", "IupDialog: Progress bar in the Windows 7 Taskbar");

  IupSetCallback(btn_pause, "ACTION", (Icallback) btn_pause_cb);
  IupSetCallback(btn_restart, "ACTION", (Icallback) btn_restart_cb);
  IupSetCallback(btn_accelerate, "ACTION", (Icallback) btn_accelerate_cb);
  IupSetCallback(btn_decelerate, "ACTION", (Icallback) btn_decelerate_cb);
  IupSetCallback(btn_indeterminate, "ACTION", (Icallback) btn_indeterminate_cb);
  IupSetCallback(btn_noprogress, "ACTION", (Icallback) btn_noprogress_cb);
  IupSetCallback(btn_error, "ACTION", (Icallback) btn_error_cb);
  
  IupSetAttribute(dlg, "RASTERSIZE", "420x");
  IupSetAttribute(dlg, "TASKBARPROGRESS", "YES");
  IupSetAttribute(dlg, "TASKBARPROGRESSSTATE", "NORMAL");
  IupSetHandle("mydialog", dlg);

  IupShowXY(dlg, IUP_CENTER, IUP_CENTER);

  IupSetAttribute(timer, "RUN", "YES");

  IupMainLoop();

  IupClose();

  return EXIT_SUCCESS;
}
