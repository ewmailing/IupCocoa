/* IupBackgroundBox Example in C */

#include <stdlib.h>
#include <stdio.h>
#include <iup.h>

int main(int argc, char **argv)
{
  Ihandle *dlg;
  Ihandle *btn;

  IupOpen(&argc, &argv);

  /* Creates a backgroundbox */
  btn = IupBackgroundBox(IupFrame(IupSetAttributes (IupVbox(
    IupButton("This button does nothing", ""), 
    IupText(""), 
    NULL), "MARGIN=0x0")));
//  IupSetAttribute(btn, "BGCOLOR", "0 128 0");
//  IupSetAttribute(btn, "BORDER", "Yes");

  /* Creates dialog */
  dlg = IupDialog
  (
    IupVbox
    (
      btn,
      NULL
    )
  );

  IupSetAttributes (dlg, "MARGIN=10x10, GAP=10, TITLE = \"IupBackgroundBox Example\"");

  IupShowXY (dlg, IUP_CENTER, IUP_CENTER );

  IupMainLoop ();
  IupClose ();
  return EXIT_SUCCESS;
}
