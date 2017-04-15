
#include <iup_plus.h>

int main(int argc, char **argv)
{
  Iup::Open(argc, argv);

  Iup::Button bt("Button");
  //bt.SetAttribute("EXPAND", "VERTICAL");  /* This is the only necessary EXPAND */
  bt.SetAttribute("EXPAND", "YES");

  Iup::Sbox box(bt);
  box.SetAttribute("DIRECTION", "SOUTH");  /* place at the bottom of the button */
//  box.SetAttribute("COLOR", "0 255 0");

  Iup::Text ml;
  ml.SetAttribute("MULTILINE", "YES");
  ml.SetAttribute("EXPAND", "YES");
  ml.SetAttribute("VISIBLELINES", "5");
  Iup::Vbox vbox(box, ml);

  Iup::Label lbl("Label");
  lbl.SetAttribute("EXPAND", "VERTICAL");

  Iup::Dialog dlg(Iup::Hbox(vbox, lbl));
  dlg.SetAttribute("TITLE", "IupSbox Example");
  dlg.SetAttribute("MARGIN", "10x10");
  dlg.SetAttribute("GAP", "10");

  dlg.Show();

  Iup::MainLoop();
  Iup::Close();
  return 0;
}
