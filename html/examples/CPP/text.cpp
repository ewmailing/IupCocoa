/*IupText Example in C 
Creates a IupText that shows asterisks instead of characters (keys-like).*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iup_plus.h>


static int k_any(Ihandle* ih, int c)
{
  switch (c)
  {
  case K_BS:
    {
      Iup::Text text(ih);
      Iup::Text keys = text.GetDialogChild("KEYS");
      char* value = keys.GetAttribute("VALUE");
      char str[30];
      strncpy(str, value, 30);
      int size = (int)strlen(str);
      if (size==0)
        return IUP_IGNORE;
      str[size-1] = 0;
      keys.SetString("VALUE", str);
      return IUP_DEFAULT;
    }
  case K_CR:
  case K_SP:
  case K_ESC:
  case K_INS:
  case K_DEL:
  case K_TAB:
  case K_HOME:
  case K_UP:
  case K_PGUP:
  case K_LEFT:
  case K_MIDDLE:
  case K_RIGHT:
  case K_END:
  case K_DOWN:
  case K_PGDN:
    return IUP_IGNORE;
  default:
    return IUP_DEFAULT;
  }
}

static int action(Ihandle* ih, int c, char*)
{
  if (c)
  {
    Iup::Text text(ih);
    Iup::Text keys = text.GetDialogChild("KEYS");
    char* value = keys.GetAttribute("VALUE");
    char str[30];
    strncpy(str, value, 30);
    int size = (int)strlen(str);
    str[size] = (char)c;
    str[size + 1] = 0;
    keys.SetString("VALUE", str);
  }
  return K_asterisk;
}

int main(int argc, char **argv)
{
  Iup::Open(argc, argv);

  Iup::Text text;
  text.SetAttribute("SIZE", "200x");
  text.SetCallback("ACTION", (Icallback)action);
  text.SetCallback("K_ANY", (Icallback)k_any);

  Iup::Text keys;
  keys.SetAttribute("READONLY", "YES");
  keys.SetAttribute("SIZE", "200x");
  keys.SetAttribute("NAME", "KEYS");

  Iup::Dialog dlg(Iup::Vbox(text, keys));
  dlg.SetAttribute("TITLE", "IupText");

  dlg.ShowXY(IUP_CENTER, IUP_CENTER);

  Iup::MainLoop();

  Iup::Close();

  return 0;
}
