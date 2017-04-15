#include <iup_plus.h>

int main(int argc, char* argv[])
{
  Iup::Open(argc, argv);

  Iup::Button bt("Button ONE");
  //  bt.SetAttribute("EXPAND", "YES");
  //  bt.SetAttribute("MINSIZE", "30x");
  //  bt.SetAttribute("RASTERSIZE", "30x");

  Iup::Expander exp(bt);
  //  exp.SetAttribute("BARPOSITION", "LEFT");
  //  exp.SetAttribute("BARPOSITION", "BOTTOM");
  //  exp.SetAttribute("BARPOSITION", "RIGHT");
  //  exp.SetAttribute("FGCOLOR", "0 0 255");
  exp.SetAttribute("TITLE", "IupExpander Title");
  //  exp.SetAttribute("AUTOSHOW", "Yes");
  //  exp.SetAttribute("STATE", "OPEN");

  Iup::Button bt2("Button TWO");

  Iup::Vbox vbox(exp, bt2);
  vbox.SetAttribute("MARGIN", "10x10");
  vbox.SetAttribute("GAP", "10");

  Iup::Dialog dlg(vbox);
  dlg.SetAttribute("TITLE", "IupExpander");

  dlg.Show();

  Iup::MainLoop();

  Iup::Close();

  return 0;
}
