#include <stdlib.h>
#include <iup.h>

int bt_book_cb(Ihandle *self)
{
  Ihandle* list = IupGetDialogChild(self, "LIST");
  Ihandle* startDate = IupGetDialogChild(self, "STARTDATE");
  Ihandle* endDate = IupGetDialogChild(self, "ENDDATE");

  int flightType = IupGetInt(list, "VALUE");

  if (flightType == 1)
    IupMessagef("Attention!", "You have booked a one-way flight on %s.", IupGetAttribute(startDate, "VALUE"));
  else
    IupMessagef("Attention!", "You have booked a return flight on %s"
                              " and %s.", IupGetAttribute(startDate, "VALUE"), IupGetAttribute(endDate, "VALUE"));

  return IUP_DEFAULT;
}

int validateDate(int day, int month, int year)
{
  int leapYear = 0;

  if (day < 1)
    return 1;

  if (year % 400 == 0 || year % 4 == 0 && year % 100 != 0)
    leapYear = 1;

  if (month == 2 && leapYear && day > 29)
    return 1;

  if (month == 2 && !leapYear && day > 28)
    return 1;

  if ((month == 4 || month == 6 || month == 9 || month == 11) && day > 30)
    return 1;

  if ((month == 1 || month == 3 || month == 5 || month == 7 ||
    month == 8 || month == 10 || month == 12) && day > 31)
    return 1;

  return 0;
}

int checkDate(Ihandle *self)
{
  int year, month, day;
  char *value = IupGetAttribute(self, "VALUE");
  char *part, *ret;
  int count = 1, notValid = 0;

  part = strtok(value, ".");
  if (part == NULL)
    return 1;
  while (part != NULL)
  {
    int len = strlen(part);
    switch (count)
    {
      case 1:
        if (strlen(part) > 2)
          notValid = 1;
        else
        {
          day = strtol(part, &ret, 10);
          if (*ret != '\0')
            notValid = 1;
        }
        break;
      case 2:
        if (strlen(part) > 2)
          notValid = 1;
        else
        {
          month = strtol(part, &ret, 10);
          if (*ret != '\0')
            notValid = 1;
        }
        break;
      case 3:
        if (strlen(part) != 4)
          notValid = 1;
        else
        {
          year = strtol(part, &ret, 10);
          if (*ret != '\0')
            notValid = 1;
        }
        break;
      default:
        notValid = 1;
    }
    count++;
    part = strtok(NULL, ".");
  }

  if (count < 4 || notValid)
    return 1;

  notValid = validateDate(day, month, year);

  if (notValid)
    IupSetAttribute(self, "VALID", "NO");
  else
  {
    IupSetAttribute(self, "VALID", "YES");
    IupSetInt(self, "DAY", day);
    IupSetInt(self, "MONTH", month);
    IupSetInt(self, "YEAR", year);
  }

  return notValid;
}

int startBeforeEnd(Ihandle *self)
{
  Ihandle* startDate = IupGetDialogChild(self, "STARTDATE");
  Ihandle* endDate = IupGetDialogChild(self, "ENDDATE");
  int startDay, startMonth, startYear;
  int endDay, endMonth, endYear;

  if (!IupGetInt(endDate, "ACTIVE"))
    return 1;

  startYear = IupGetInt(startDate, "YEAR");
  endYear = IupGetInt(endDate, "YEAR");

  if (startYear < endYear)
    return 1;

  startMonth = IupGetInt(startDate, "MONTH");
  endMonth = IupGetInt(endDate, "MONTH");

  if (startYear == endYear && startMonth < endMonth)
    return 1;

  startDay = IupGetInt(startDate, "DAY");
  endDay = IupGetInt(endDate, "DAY");

  if (startYear == endYear && startMonth == endMonth && startDay <= endDay)
    return 1;

  return 0;
}

int txt_valuechanged_cb(Ihandle *self)
{
  Ihandle *other = (Ihandle *)IupGetAttribute(self, "OTHER");
  Ihandle* button = IupGetDialogChild(self, "BUTTON");
  int notValid = checkDate(self);

  if (!notValid)
    IupSetAttribute(self, "BGCOLOR", "255 255 255");
  else
    IupSetAttribute(self, "BGCOLOR", "255 0 0");

  if (!notValid && (IupGetInt(other, "VALID") || !IupGetInt(other, "ACTIVE")) &&
      startBeforeEnd(self))
    IupSetAttribute(button, "ACTIVE", "YES");
  else
    IupSetAttribute(button, "ACTIVE", "NO");

  return IUP_DEFAULT;
}

int list_action_cb(Ihandle *self, char *text, int item, int state)
{
  Ihandle* startDate = IupGetDialogChild(self, "STARTDATE");
  Ihandle* endDate = IupGetDialogChild(self, "ENDDATE");
  Ihandle* button = IupGetDialogChild(self, "BUTTON");

  if (state == 0)
    return IUP_DEFAULT;

  if (item == 1)
    IupSetAttribute(endDate, "ACTIVE", "NO");
  else
    IupSetAttribute(endDate, "ACTIVE", "YES");

  if (item==2 && !startBeforeEnd(self))
      IupSetAttribute(button, "ACTIVE", "NO");

  return IUP_DEFAULT;
}

int main(int argc, char **argv)
{
  Ihandle *dlg, *vbox, *list, *startDate, *endDate, *button;

  IupOpen(&argc, &argv);

  list = IupList(NULL);

  IupSetAttribute(list, "NAME", "LIST");
  IupSetAttribute(list, "EXPAND", "HORIZONTAL");
  IupSetAttribute(list, "DROPDOWN", "YES");
  IupSetAttribute(list, "1", "one-way flight");
  IupSetAttribute(list, "2", "return flight");
  IupSetAttribute(list, "VALUE", "1");

  startDate = IupText(NULL);
  endDate = IupText(NULL);

  IupSetAttribute(startDate, "EXPAND", "HORIZONTAL");
  IupSetAttribute(startDate, "NAME", "STARTDATE");
  IupSetAttribute(startDate, "OTHER", (char *)endDate);
  IupSetAttribute(startDate, "VALUE", "22.09.1957");
  IupSetAttribute(startDate, "DAY", "22");
  IupSetAttribute(startDate, "MONTH", "09");
  IupSetAttribute(startDate, "YEAR", "1957");
  IupSetAttribute(startDate, "VALID", "YES");
  IupSetAttribute(endDate, "EXPAND", "HORIZONTAL");
  IupSetAttribute(endDate, "NAME", "ENDDATE");
  IupSetAttribute(endDate, "OTHER", (char *)startDate);
  IupSetAttribute(endDate, "VALUE", "22.09.1957");
  IupSetAttribute(endDate, "VALID", "YES");
  IupSetAttribute(endDate, "DAY", "22");
  IupSetAttribute(endDate, "MONTH", "09");
  IupSetAttribute(endDate, "YEAR", "1957");
  IupSetAttribute(endDate, "ACTIVE", "NO");

  button = IupButton("Book", NULL);

  IupSetAttribute(button, "EXPAND", "HORIZONTAL");
  IupSetAttribute(button, "NAME", "BUTTON");

  vbox = IupVbox(
    list,
    startDate,
    endDate,
    button,
    NULL);
  IupSetAttribute(vbox, "MARGIN", "10x10");
  IupSetAttribute(vbox, "GAP", "10");
  dlg = IupDialog(vbox);
  IupSetAttribute(dlg, "TITLE", "Book Flight");
  IupSetAttribute(dlg, "SIZE", "150");

  /* Registers callbacks */
  IupSetCallback(startDate, "VALUECHANGED_CB", (Icallback)txt_valuechanged_cb);
  IupSetCallback(endDate, "VALUECHANGED_CB", (Icallback)txt_valuechanged_cb);
  IupSetCallback(list, "ACTION", (Icallback)list_action_cb);
  IupSetCallback(button, "ACTION", (Icallback)bt_book_cb);

  IupShowXY(dlg, IUP_CENTER, IUP_CENTER);

  IupMainLoop();

  IupClose();
  return EXIT_SUCCESS;
}
