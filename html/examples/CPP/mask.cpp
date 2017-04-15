/*  IupText MASK: Example in C 
    Creates an IupText that accepts only numbers.
*/

#include <iup_plus.h>

int main(int argc, char **argv)
{
  Iup::Open(argc, argv);

  Iup::Text text;
  text.SetAttribute("MASK", "/d*");

  text.SetAttribute("SIZE", "100x");
  text.SetAttribute("EXPAND", "HORIZONTAL");

  Iup::Dialog dlg(text);
  dlg.SetAttribute("TITLE", "IupText MASK");

  dlg.Show();

  Iup::MainLoop();

  Iup::Close();

  return 0;
}
