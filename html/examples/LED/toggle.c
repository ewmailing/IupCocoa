#include <stdio.h>
#include "iup.h"          

int toggle1cb(Ihandle *self, int v)
{
  if (v == 1)
    IupMessage("Toggle 1","pressed"); 
  else 
    IupMessage("Toggle 1","released"); 

  return IUP_DEFAULT;
}

int main(int argc, char **argv)
{ 
  char *error;
  Ihandle *dlg;

  IupOpen(&argc, &argv);

  /* Loads LED */
  error = IupLoad("toggle.led");
  if (error)
  {
    IupMessage("LED error", error);
    return 1 ;
  }

  /* Associates the C dialog to the dialog defined in LED */
  dlg = IupGetHandle("dialog_name");

  /* For the other callbacks to work,
  * IupSetFunction must also be called */
  IupSetFunction("toggle1cb", (Icallback) toggle1cb);

  /* Associates a menu to the dialog */
  IupShowXY(dlg, IUP_CENTER, IUP_CENTER); 
  IupMainLoop();
  IupDestroy(dlg);
  IupClose();

  return 0 ;
}
