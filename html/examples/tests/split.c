#include <stdio.h>
#include "iup.h"

int action(Ihandle* bt)
{
  IupSetAttribute(IupGetParent(bt), "VALUE", "0");
  return IUP_DEFAULT;
}

void SplitTest(void)
{
  Ihandle *dlg, *bt, *split, *ml, *vbox;

  bt = IupButton("Button", NULL);
  IupSetAttribute(bt, "EXPAND", "YES");
//  IupSetAttribute(bt, "MINSIZE", "30x");
//  IupSetAttribute(bt, "RASTERSIZE", "30x");
  IupSetCallback(bt, "ACTION", action);

  ml = IupMultiLine(NULL);
  IupSetAttribute(ml, "EXPAND", "YES");
  IupSetAttribute(ml, "VISIBLELINES", "5");
  IupSetAttribute(ml, "VISIBLECOLUMNS", "10");
  IupSetAttribute(ml, "VALUE", "Multiline");
//  IupSetAttribute(ml, "MINSIZE", "30x");
  
  split = IupSplit(bt, ml);
  //split = IupSplit(NULL, NULL);
  IupSetAttribute(split, "ORIENTATION", "VERTICAL");
//  IupSetAttribute(split, "ORIENTATION", "HORIZONTAL");
//  IupSetAttribute(split, "COLOR", "0 255 0");
//  IupSetAttribute(split, "LAYOUTDRAG", "NO");
  IupSetAttribute(split, "AUTOHIDE", "YES");
//  IupSetAttribute(split, "SHOWGRIP", "NO");
//  IupSetAttribute(split, "MINMAX", "100:800");

  vbox = IupVbox(split, NULL);
  IupSetAttribute(vbox, "MARGIN", "10x10");
  IupSetAttribute(vbox, "GAP", "10");

  dlg = IupDialog(vbox);
  IupSetAttribute(dlg, "TITLE", "IupSplit Example");

  IupShow(dlg);

  //IupAppend(split, bt);
  //IupAppend(split, ml);
  //IupMap(bt);
  //IupMap(ml);
  //IupRefresh(dlg);
}

#ifndef BIG_TEST
int main(int argc, char* argv[])
{
  IupOpen(&argc, &argv);

  SplitTest();

  IupMainLoop();

  IupClose();

  return EXIT_SUCCESS;
}
#endif
