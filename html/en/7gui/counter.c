#include <stdlib.h>
#include <iup.h>

int btn_count_cb( Ihandle *self )
{
  Ihandle* text = IupGetDialogChild(self, "TEXT");
  int value = IupGetInt(text, "VALUE");
  IupSetInt(text, "VALUE", ++value);

  return IUP_DEFAULT;
}

int main(int argc, char **argv)
{
  Ihandle *dlg, *button, *text, *hbox;

  IupOpen(&argc, &argv);
  
  button = IupButton("Count", NULL);
  IupSetAttribute(button, "SIZE", "60");
  text = IupText(NULL);
  IupSetAttribute(text, "SIZE", "60");
  IupSetAttribute(text, "NAME", "TEXT");
  IupSetAttribute(text, "READONLY", "YES");
  hbox = IupHbox(
    text,
    button,
    NULL);
  IupSetAttribute(hbox, "MARGIN", "10x10");
  IupSetAttribute(hbox, "GAP", "10");
  dlg = IupDialog(hbox);
  IupSetAttribute(dlg, "TITLE", "Counter");

  IupSetInt(text, "VALUE", 0);

  /* Registers callbacks */
  IupSetCallback(button, "ACTION", (Icallback) btn_count_cb);

  IupShowXY(dlg, IUP_CENTER, IUP_CENTER);

  IupMainLoop();

  IupClose();
  return EXIT_SUCCESS;
}
