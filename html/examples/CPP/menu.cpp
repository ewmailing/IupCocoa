/* Iup::Menu: Example in C 
   Creates a dialog with a menu with two submenus.
*/

#include <iup_plus.h>

static int exit_cb(Ihandle*)
{
  return IUP_CLOSE;
}

int main(int argc, char **argv)
{
  Iup::Open(argc, argv);

  Iup::Item item_open("Open");
  item_open.SetAttribute("KEY", "O");

  Iup::Item item_save("Save");
  item_save.SetAttribute("KEY", "S");

  Iup::Item item_undo("Undo");
  item_undo.SetAttribute("KEY", "U");
  item_undo.SetAttribute("ACTIVE", "NO");

  Iup::Item item_exit("Exit");
  item_exit.SetAttribute("KEY", "x");
  item_exit.SetCallback("ACTION", exit_cb);

  Iup::Menu file_menu(item_open,
                      item_save, 
                      IupSeparator(),
                      item_undo,
                      item_exit);

  Iup::Submenu sub1_menu("File", file_menu);

  Iup::Menu menu(sub1_menu);
  Iup::Canvas cnv;
  Iup::Dialog dlg(cnv);
  dlg.SetAttributeHandle("MENU", menu);
  dlg.SetAttribute("TITLE", "Iup::Menu");
  dlg.SetAttribute("SIZE", "QUARTERxQUARTER");

  dlg.Show();

  Iup::MainLoop();

  Iup::Close();

  return 0;
}
