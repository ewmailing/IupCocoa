#include <stdlib.h>
#include <iup.h>

int txt_celcius_cb(Ihandle *self)
{
  Ihandle* fahrenheit = IupGetDialogChild(self, "FAHRENHEIT");
  double value = IupGetDouble(self, "VALUE");;

  value = value * (9./5.) + 32;

  IupSetStrf(fahrenheit, "VALUE", "%.2lf", value);

  return IUP_DEFAULT;
}

int txt_fahrenheit_cb(Ihandle *self)
{
  Ihandle* celcius = IupGetDialogChild(self, "CELCIUS");
  double value = IupGetDouble(self, "VALUE");;

  value = (value - 32) * (5./9.);

  IupSetStrf(celcius, "VALUE", "%.2lf", value);

  return IUP_DEFAULT;
}

int main(int argc, char **argv)
{
  Ihandle *dlg, *labelC, *labelF, *tempC, *tempF, *hbox;

  IupOpen(&argc, &argv);
  
  labelC = IupLabel("Celsius =");
  labelF = IupLabel("Fahrenheit");

  tempC = IupText(NULL);
  IupSetAttribute(tempC, "SIZE", "50");
  IupSetAttribute(tempC, "NAME", "CELCIUS");
  IupSetAttribute(tempC, "MASK", IUP_MASK_FLOAT);
  tempF = IupText(NULL);
  IupSetAttribute(tempF, "SIZE", "50");
  IupSetAttribute(tempF, "NAME", "FAHRENHEIT");
  IupSetAttribute(tempF, "MASK", IUP_MASK_FLOAT);
  hbox = IupHbox(
    tempC,
    labelC,
    tempF,
    labelF,
    NULL);
  IupSetAttribute(hbox, "MARGIN", "10x10");
  IupSetAttribute(hbox, "GAP", "10");
  IupSetAttribute(hbox, "ALIGNMENT", "ACENTER");
  dlg = IupDialog(hbox);
  IupSetAttribute(dlg, "TITLE", "TempConv");

  IupSetAttribute(tempC, "VALUE", "");
  IupSetAttribute(tempF, "VALUE", "");

  /* Registers callbacks */
  IupSetCallback(tempC, "VALUECHANGED_CB", (Icallback)txt_celcius_cb);
  IupSetCallback(tempF, "VALUECHANGED_CB", (Icallback)txt_fahrenheit_cb);

  IupShowXY(dlg, IUP_CENTER, IUP_CENTER);

  IupMainLoop();

  IupClose();
  return EXIT_SUCCESS;
}
