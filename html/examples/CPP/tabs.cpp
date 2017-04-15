/* IupTabs: Example in C 
   Creates a IupTabs control.
*/

#include <iup_plus.h>

int main(int argc, char **argv)
{
  Iup::Open(argc, argv);

  Iup::Vbox vbox1(Iup::Label("Inside Tab A"), Iup::Button("Button A"));
  Iup::Vbox vbox2(Iup::Label("Inside Tab B"), Iup::Button("Button B"));

  vbox1.SetAttribute("TABTITLE", "Tab A");
  vbox2.SetAttribute("TABTITLE", "Tab B");

  Iup::Tabs tabs1(vbox1, vbox2);

  Iup::Vbox vbox3(Iup::Label("Inside Tab C"), Iup::Button("Button C"));
  Iup::Vbox vbox4(Iup::Label("Inside Tab D"), Iup::Button("Button D"));

  vbox3.SetAttribute("TABTITLE", "Tab C");
  vbox4.SetAttribute("TABTITLE", "Tab D");

  Iup::Tabs tabs2(vbox3, vbox4);
  tabs2.SetAttribute("TABTYPE", "LEFT");

  Iup::Hbox box(tabs1, tabs2);
  box.SetAttribute("MARGIN", "10x10");
  box.SetAttribute("GAP", "10");

  Iup::Dialog dlg(box);
  dlg.SetAttribute("TITLE", "IupTabs");
  dlg.SetAttribute("SIZE", "200x80");

  dlg.ShowXY(IUP_CENTER, IUP_CENTER);

  Iup::MainLoop();

  Iup::Close();

  return 0;
}
