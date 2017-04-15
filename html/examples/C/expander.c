#include <stdio.h>
#include <iup.h>

void ExpanderTest(void)
{
  Ihandle *dlg, *bt, *exp, *vbox, *bt2;

  bt = IupButton("Button ONE", NULL);
//  IupSetAttribute(bt, "EXPAND", "YES");
//  IupSetAttribute(bt, "MINSIZE", "30x");
//  IupSetAttribute(bt, "RASTERSIZE", "30x");
  
  exp = IupExpander(bt);
//  IupSetAttribute(exp, "BARPOSITION", "LEFT");
//  IupSetAttribute(exp, "BARPOSITION", "BOTTOM");
//  IupSetAttribute(exp, "BARPOSITION", "RIGHT");
//  IupSetAttribute(exp, "FGCOLOR", "0 0 255");
  IupSetAttribute(exp, "TITLE", "IupExpander Title");
//  IupSetAttribute(exp, "AUTOSHOW", "Yes");
//  IupSetAttribute(exp, "STATE", "OPEN");

  bt2 = IupButton("Button TWO", NULL);

  vbox = IupVbox(exp, bt2, NULL);
  IupSetAttribute(vbox, "MARGIN", "10x10");
  IupSetAttribute(vbox, "GAP", "10");

  dlg = IupDialog(vbox);
  IupSetAttribute(dlg, "TITLE", "IupExpander");

  IupShow(dlg);
}

#ifndef BIG_TEST
int main(int argc, char* argv[])
{
  IupOpen(&argc, &argv);

  ExpanderTest();

  IupMainLoop();

  IupClose();

  return EXIT_SUCCESS;
}
#endif
