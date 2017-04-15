#include <stdlib.h>
#include <stdio.h>

#include "iup.h"


static int value_cb(Ihandle* ih)
{
  printf("VALUE_CB(%s)\n", IupGetAttribute(ih, "VALUE"));
  return IUP_DEFAULT;
}

static int set_value(Ihandle* ih)
{
  IupSetAttribute(IupGetBrother(ih), "VALUE", "1970/07/11");
  IupSetAttribute(IupGetBrother(IupGetBrother(ih)), "VALUE", "1970/07/11");
  return IUP_DEFAULT;
}

void CalendarTest(void)
{
  Ihandle *dlg, *box, *date;

  date = IupDatePick();
//  IupSetAttribute(date, "FORMAT", "ddd', 'd' of 'MMMM' ('yy')'");
  IupSetAttribute(date, "ZEROPRECED", "Yes");
  IupSetAttribute(date, "MONTHSHORTNAMES", "Yes");
  IupSetAttribute(date, "VALUE", "2016/09/01");
//  IupSetAttribute(date, "VALUE", "TODAY");

  box = IupVbox(
    IupSetCallbacks(IupButton("Set Value", NULL), "ACTION", set_value, NULL),
    IupSetCallbacks(IupCalendar(), "VALUECHANGED_CB", value_cb, NULL),
    IupSetCallbacks(date, "VALUECHANGED_CB", value_cb, NULL),
    NULL);
    IupSetAttribute(box, "MARGIN", "10x10");
  IupSetAttribute(box, "GAP", "10");

  dlg = IupDialog(box);
//  IupSetAttribute(box, "FONTSIZE", "36");

  IupSetAttribute(dlg, "TITLE", "IupCalendar Test");
  IupShow(dlg);
}

#ifndef BIG_TEST
int main(int argc, char* argv[])
{
  IupOpen(&argc, &argv);

  CalendarTest();

  IupMainLoop();

  IupClose();

  return EXIT_SUCCESS;
}
#endif
