#include <stdlib.h>
#include <iup.h>

int btn_reset_cb( Ihandle *self )
{
  Ihandle* text = IupGetDialogChild(self, "GAUGE");
  Ihandle* val = IupGetDialogChild(self, "VAL");
  Ihandle* dial = IupGetDialog(self);
  Ihandle* timer = (char *)IupGetAttribute(dial, "TIMER");

  IupSetAttribute(timer, "RUN", "NO");
  IupSetAttribute(timer, "RUN", "YES");

  IupSetAttribute(timer, "TOTALELAPSEDTIME", "0");
  IupSetAttribute(val, "VALUE", "30");

  return IUP_DEFAULT;
}

int val_valuechanged_cb(Ihandle *self)
{
  Ihandle* gauge = IupGetDialogChild(self, "GAUGE");
  Ihandle* dial = IupGetDialog(self);
  Ihandle* timer = (char *) IupGetAttribute(dial, "TIMER");
  double value = IupGetDouble(self, "VALUE");
  double totalTime = IupGetDouble(timer, "TOTALELAPSEDTIME");

  IupSetDouble(gauge, "MAX", value);

  if (totalTime/1000. < value)
    IupSetAttribute(timer, "RUN", "YES");
  else
    IupSetAttribute(gauge, "VALUE", IupGetAttribute(gauge, "MAX"));

  return IUP_DEFAULT;
}

int timer_cb(Ihandle *self)
{
  Ihandle* gauge = (Ihandle *)IupGetAttribute(self, "GAUGE");
  Ihandle* val = (Ihandle *)IupGetAttribute(self, "VAL");
  Ihandle* label = (Ihandle *)IupGetAttribute(self, "LABEL");
  double elapsedTime = IupGetDouble(self, "ELAPSEDTIME");
  double totaltime = IupGetDouble(self, "TOTALELAPSEDTIME");

  if ((totaltime + elapsedTime) / 1000. >= IupGetDouble(val, "VALUE"))
  {
    IupSetAttribute(self, "RUN", "NO");
    IupSetStrf(self, "TOTALELAPSEDTIME", "%.2lf", totaltime + elapsedTime);
    IupSetStrf(label, "TITLE", "%.2fs", IupGetDouble(val, "VALUE"));
    IupSetStrf(gauge, "VALUE", "%.2fs", IupGetDouble(val, "VALUE"));
  }
  else
  {
    IupSetStrf(label, "TITLE", "%.2fs", (totaltime + elapsedTime) / 1000.);
    IupSetStrf(gauge, "VALUE", "%.2fs", (totaltime + elapsedTime) / 1000.);
  }

  return IUP_DEFAULT;
}

int main(int argc, char **argv)
{
  Ihandle *dlg, *gauge, *label, *glabel, *val, *vlable, *button, *vbox, *hbox1, *hbox2, *timer;

  IupOpen(&argc, &argv);

  timer = IupTimer();
  IupSetAttribute(timer, "TIME", "100");
  IupSetAttribute(timer, "TOTALELAPSEDTIME", "0");
  IupSetCallback(timer, "ACTION_CB", (Icallback)timer_cb);

  glabel = IupLabel("Elapsed Time:");
  gauge = IupGauge();
  IupSetAttribute(gauge, "NAME", "GAUGE");
  IupSetAttribute(gauge, "MIN", ".1");
  IupSetAttribute(gauge, "MAX", "30");
  IupSetAttribute(gauge, "VALUE", "30");
  IupSetAttribute(gauge, "EXPAND", "HORIZONTAL");
  IupSetAttribute(gauge, "SHOWTEXT", "NO");
  label = IupLabel("0.0s");
  IupSetAttribute(label, "NAME", "LABEL");
  IupSetAttribute(label, "EXPAND", "HORIZONTAL");
  vlable = IupLabel("Duration:");
  val = IupVal("HORIZONTAL");
  IupSetAttribute(val, "EXPAND", "HORIZONTAL");
  IupSetAttribute(val, "NAME", "VAL");
  IupSetAttribute(val, "MAX", "60.0");
  IupSetAttribute(val, "STEP", ".1");
  IupSetAttribute(val, "VALUE", "30.0");
  IupSetAttribute(val, "PAGESTEP", ".5");
  button = IupButton("Reset", NULL);
  IupSetAttribute(button, "EXPAND", "HORIZONTAL");
  IupSetAttribute(button, "NAME", "BUTTON");
  hbox1 = IupHbox(glabel, gauge, NULL);
  IupSetAttribute(hbox1, "MARGIN", "0x0");
  IupSetAttribute(hbox1, "ALIGNMENT", "ACENTER");
  hbox2 = IupHbox(vlable, val, NULL);
  IupSetAttribute(hbox2, "MARGIN", "0x0");
  IupSetAttribute(hbox2, "ALIGNMENT", "ACENTER");
  vbox = IupVbox(
    hbox1,
    label,
    hbox2,
    button,
    NULL);
  IupSetAttribute(vbox, "MARGIN", "10x10");
  IupSetAttribute(vbox, "GAP", "10");
  dlg = IupDialog(vbox);
  IupSetAttribute(dlg, "TITLE", "Timer");
  IupSetAttribute(dlg, "SIZE", "200");
  IupSetAttribute(dlg, "TIMER", (char *) timer);

  IupSetAttribute(timer, "GAUGE", (char *)gauge);
  IupSetAttribute(timer, "LABEL", (char *)label);
  IupSetAttribute(timer, "VAL", (char *)val);

  /* Registers callbacks */
  IupSetCallback(val, "VALUECHANGED_CB", (Icallback)val_valuechanged_cb);
  IupSetCallback(button, "ACTION", (Icallback)btn_reset_cb);

  IupShowXY(dlg, IUP_CENTER, IUP_CENTER);

  IupMainLoop();

  IupClose();
  return EXIT_SUCCESS;
}
