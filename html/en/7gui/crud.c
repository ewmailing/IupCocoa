#include <stdlib.h>
#include <iup.h>

#define my_tolower(_c)  ((_c >= 'A' && _c <= 'Z')? (_c - 'A') + 'a': _c)

/*********************************** Names List ****************************************/

#define MAX_NAMES 80

typedef struct _Name
{
  char name[80];
  char surname[80];
  int id;
} Name;

static Name namesList[MAX_NAMES];
static int namesCount = 0;

static int crud_create(char *surname, char *name)
{
  static int lastId = 0;

  if (namesCount == MAX_NAMES)
    return -1;

  strcpy(namesList[namesCount].surname, surname);
  strcpy(namesList[namesCount].name, name);
  namesList[namesCount].id = lastId;  /* each circle has an unique id */

  namesCount++;
  lastId++;
  return lastId - 1;
}

static Name crud_read(int index)
{
  return namesList[index];
}

static Name crud_find(int id)
{
  int i;
  for (i = 0; i < namesCount; i++)
  {
    if (namesList[i].id == id)
      break;
  }
  return namesList[i];
}

static void crud_update(int id, char *surname, char *name)
{
  int i;
  for (i = 0; i < namesCount; i++)
  {
    if (namesList[i].id == id)
      break;
  }
  strcpy(namesList[i].surname, surname);
  strcpy(namesList[i].name, name);
}

static void crud_delete(int id)
{
  int i, j;
  for (i = 0; i < namesCount; i++)
  {
    if (namesList[i].id == id)
      break;
  }

  for (j = i; j < namesCount - 1; j++)
    namesList[j] = namesList[j + 1];

  namesCount--;
}

static int crud_length()
{
  return namesCount;
}

/************************************* Operation List ************************************************/

void strLower(char* dstr, char* sstr)
{
  if (!sstr || sstr[0] == 0) return;
  for (; *sstr; sstr++, dstr++)
    *dstr = (char)my_tolower(*sstr);
  *dstr = 0;
}

static char *trim(char *str)
{
  char *last = NULL;
  while (*str==' ') str++;
  last = str + strlen(str);
  while (*--last == ' ');
  *(last + 1) = '\0';
  return str;
}

static int filterName(char *name, char *filter)
{
  char lname[80], lfilter[80];
  strLower(lname, name);
  strLower(lfilter, filter);
  int i = strncmp(lname, lfilter, strlen(filter));
  return (i==0);
}

static void loadList(Ihandle *ih)
{
  Ihandle *txtFilter = IupGetDialogChild(ih, "TXT_FILTER");
  Ihandle *listNames = IupGetDialogChild(ih, "LST_NAMES");
  Name data;
  int count = crud_length();
  char *filterTxt = IupGetAttribute(txtFilter, "VALUE");
  int filter = 1;
  int i, j;

  filterTxt = trim(filterTxt);
  if (*filterTxt == '\0')
    filter = 0;

  IupSetAttribute(listNames, "1", NULL);

  j = 1;
  for (i = 1; i <= count; i++)
  {
    Name name = crud_read(i-1);
    if (!filter || filterName(name.surname, filterTxt))
    {
      IupSetStrfId(listNames, "", j, "%s, %s", name.surname, name.name);
      IupSetIntId(listNames, "ID", j, name.id);
      j++;
    }
  }

  IupSetAttribute(listNames, "VALUE", NULL);
}

static void updateButtonsState(Ihandle *ih)
{
  Ihandle *updateButton = IupGetDialogChild(ih, "BTN_UPDATE");
  Ihandle *deleteButton = IupGetDialogChild(ih, "BTN_DELETE");
  Ihandle *listNames = IupGetDialogChild(ih, "LST_NAMES");

  if (!IupGetInt(listNames, "VALUE"))
  {
    IupSetAttribute(updateButton, "ACTIVE", "NO");
    IupSetAttribute(deleteButton, "ACTIVE", "NO");
  }
  else
  {
    IupSetAttribute(updateButton, "ACTIVE", "YES");
    IupSetAttribute(deleteButton, "ACTIVE", "YES");
  }
}

int btn_create_cb(Ihandle *ih)
{
  Ihandle *txtName = IupGetDialogChild(ih, "TXT_NAME");
  Ihandle *txtSurname = IupGetDialogChild(ih, "TXT_SURNAME");
  char *name = trim(IupGetAttribute(txtName, "VALUE"));
  char *surname = trim(IupGetAttribute(txtSurname, "VALUE"));
  crud_create(surname, name);
  IupSetAttribute(txtName, "VALUE", NULL);
  IupSetAttribute(txtSurname, "VALUE", NULL);
  loadList(ih);
  updateButtonsState(ih);
  return IUP_DEFAULT;
}

int btn_update_cb(Ihandle *ih)
{
  Ihandle *txtName = IupGetDialogChild(ih, "TXT_NAME");
  Ihandle *txtSurname = IupGetDialogChild(ih, "TXT_SURNAME");
  Ihandle *listNames = IupGetDialogChild(ih, "LST_NAMES");
  int index = IupGetInt(listNames, "VALUE");
  int id = IupGetIntId(listNames, "ID", index);
  char *name = trim(IupGetAttribute(txtName, "VALUE"));
  char *surname = trim(IupGetAttribute(txtSurname, "VALUE"));
  crud_update(id, surname, name);
  IupSetAttribute(txtName, "VALUE", NULL);
  IupSetAttribute(txtSurname, "VALUE", NULL);
  loadList(ih);
  updateButtonsState(ih);
  return IUP_DEFAULT;
}

int btn_delete_cb(Ihandle *ih)
{
  Ihandle *txtName = IupGetDialogChild(ih, "TXT_NAME");
  Ihandle *txtSurname = IupGetDialogChild(ih, "TXT_SURNAME");
  Ihandle *listNames = IupGetDialogChild(ih, "LST_NAMES");
  int index = IupGetAttribute(listNames, "VALUE");
  int id = IupGetIntId(listNames, "ID", index);
  char *name = trim(IupGetAttribute(txtName, "VALUE"));
  char *surname = trim(IupGetAttribute(txtSurname, "VALUE"));
  crud_delete(id, surname, name);
  IupSetAttribute(txtName, "VALUE", NULL);
  IupSetAttribute(txtSurname, "VALUE", NULL);
  loadList(ih);
  updateButtonsState(ih);
  return IUP_DEFAULT;
}

int lst_valuechanged_cb(Ihandle *ih)
{
  Ihandle *txtName = IupGetDialogChild(ih, "TXT_NAME");
  Ihandle *txtSurname = IupGetDialogChild(ih, "TXT_SURNAME");
  int sel = IupGetInt(ih, "VALUE");
  int id = IupGetIntId(ih, "ID", sel);
  Name name = crud_find(id);
  IupSetAttribute(txtName, "VALUE", name.name);
  IupSetAttribute(txtSurname, "VALUE", name.surname);
  updateButtonsState(ih);
  return IUP_DEFAULT;
}

int txt_valuechanged_cb(Ihandle *ih)
{
  loadList(ih);
  return IUP_DEFAULT;
}

int main(int argc, char **argv)
{
  Ihandle *dlg, *lbl_filter, *txt_filter, *lst_names, *lbl_name, *txt_name, *lbl_surname, *txt_surname;
  Ihandle *btn_create, *btn_update, *btn_delete;
  Ihandle *hbox, *box, *hbx_filter, *hbx_top, *hbx_name, *hbx_surname, *hbx_buttons, *vbx_input;

  IupOpen(&argc, &argv);

  lbl_filter = IupLabel("Filter prefix:");
  IupSetAttribute(lbl_filter, "NAME", "LBL_FILTER");
  txt_filter = IupText(NULL);
  IupSetAttribute(txt_filter, "NAME", "TXT_FILTER");
  IupSetAttribute(txt_filter, "EXPAND", "HORIZONTAL");
  IupSetAttribute(txt_filter, "SIZE", "60x");
  lst_names = IupList(NULL);
  IupSetAttribute(lst_names, "NAME", "LST_NAMES");
  IupSetAttribute(lst_names, "EXPAND", "YES");
  IupSetAttribute(lst_names, "SIZE", "60x");
  IupSetAttribute(lst_names, "VISIBLELINES", "6");
  lbl_name = IupLabel("Name:");
  IupSetAttribute(lbl_name, "NAME", "LBL_NAME");
  IupSetAttribute(lbl_name, "SIZE", "35");
  txt_name = IupText(NULL);
  IupSetAttribute(txt_name, "NAME", "TXT_NAME");
  IupSetAttribute(txt_name, "SIZE", "60");
  IupSetAttribute(txt_name, "NC", "80");
  IupSetAttribute(txt_name, "EXPAND", "HORIZONTAL");
  lbl_surname = IupLabel("Surname:");
  IupSetAttribute(lbl_surname, "NAME", "LBL_SURNAME");
  IupSetAttribute(lbl_surname, "SIZE", "35");
  txt_surname = IupText(NULL);
  IupSetAttribute(txt_surname, "NAME", "TXT_SURNAME");
  IupSetAttribute(txt_surname, "SIZE", "60");
  IupSetAttribute(txt_surname, "NC", "80");
  IupSetAttribute(txt_surname, "EXPAND", "HORIZONTAL");

  btn_create = IupButton("Create", NULL); 
  IupSetAttribute(btn_create, "NAME", "BTN_CREATE");
  IupSetAttribute(btn_create, "SIZE", "30");
  btn_update = IupButton("Update", NULL);
  IupSetAttribute(btn_update, "NAME", "BTN_UPDATE");
  IupSetAttribute(btn_update, "SIZE", "30");
  IupSetAttribute(btn_update, "ACTIVE", "NO");
  btn_delete = IupButton("Delete", NULL);
  IupSetAttribute(btn_delete, "NAME", "BTN_DELETE");
  IupSetAttribute(btn_delete, "SIZE", "30");
  IupSetAttribute(btn_delete, "ACTIVE", "NO");

  hbx_filter = IupHbox(lbl_filter, txt_filter, NULL);
  IupSetAttribute(hbx_filter, "ALIGNMENT", "ACENTER");
  hbx_top = IupHbox(hbx_filter, IupFill(), NULL);
  IupSetAttribute(hbx_top, "HOMOGENEOUS", "YES");
  hbx_name = IupHbox(lbl_name, txt_name, NULL); 
  IupSetAttribute(hbx_name, "ALIGNMENT", "ACENTER");
  hbx_surname = IupHbox(lbl_surname, txt_surname, NULL);
  IupSetAttribute(hbx_surname, "alignment", "ACENTER");
  hbx_buttons = IupHbox(btn_create, btn_update, btn_delete, NULL);

  vbx_input = IupVbox(hbx_name, hbx_surname, NULL);

  hbox = IupHbox(lst_names, vbx_input, NULL);
  IupSetAttribute(hbox, "HOMOGENEOUS", "YES");
  box = IupVbox(hbx_top, hbox, hbx_buttons, NULL);
  IupSetAttribute(box, "NMARGIN", "10x10");

  dlg = IupDialog(box);
  IupSetAttribute(dlg, "TITLE", "CRUD");
  IupSetAttribute(dlg, "GAP", "10");

  /* Registers callbacks */
  IupSetCallback(btn_create, "ACTION", (Icallback)btn_create_cb);
  IupSetCallback(btn_update, "ACTION", (Icallback)btn_update_cb);
  IupSetCallback(btn_delete, "ACTION", (Icallback)btn_delete_cb);
  IupSetCallback(lst_names, "VALUECHANGED_CB", (Icallback)lst_valuechanged_cb);
  IupSetCallback(txt_filter, "VALUECHANGED_CB", (Icallback)txt_valuechanged_cb);

  IupShowXY(dlg, IUP_CENTER, IUP_CENTER);

  IupMainLoop();

  IupClose();
  return EXIT_SUCCESS;
}
