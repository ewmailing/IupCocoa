#include <stdlib.h>
#include <stdio.h>

#include "iup.h"


static int action(Ihandle *ih, const char* url)
{
  printf("ACTION (%s)\n", url);
  (void)ih;
  return IUP_DEFAULT;
}

void LinkTest(void)
{
  Ihandle *dlg, *linkEmpty, *linkButtonLabel, *linkButton;

  linkEmpty = IupLink(NULL, "Nothing");
  linkButton = IupLink("http://www.tecgraf.puc-rio.br/iup", "IUP Toolkit");
  linkButtonLabel = IupLink("http://www.upf.br/", "Universidade de Passo Fundo");
  
  dlg = IupDialog(IupVbox(IupHbox(linkEmpty, NULL), IupHbox(linkButton, NULL), IupHbox(linkButtonLabel, NULL), NULL));
  IupSetAttribute(dlg, "TITLE", "IupLink Test");
  IupSetAttribute(dlg, "MARGIN", "10x10");
  IupSetAttribute(dlg, "RASTERSIZE", "500x500");
  
  IupSetCallback(linkEmpty, "ACTION", (Icallback)action);
  IupSetCallback(linkButton, "ACTION", (Icallback)action);
  IupSetCallback(linkButtonLabel, "ACTION", (Icallback)action);

  IupShowXY(dlg,IUP_CENTER,IUP_CENTER);
}

#ifndef BIG_TEST
int main(int argc, char* argv[])
{
  IupOpen(&argc, &argv);

  LinkTest();

  IupMainLoop();

  IupClose();

  return EXIT_SUCCESS;
}
#endif
