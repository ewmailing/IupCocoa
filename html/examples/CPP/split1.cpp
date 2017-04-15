#include <iup_plus.h>

void SplitTest(void)
{
  Iup::Button bt("Button");
  bt.SetAttribute("EXPAND", "YES");

  Iup::Text ml;
  ml.SetAttribute("MULTILINE", "YES");
  ml.SetAttribute("EXPAND", "YES");
  ml.SetAttribute("VISIBLELINES", "5");
  ml.SetAttribute("VISIBLECOLUMNS", "10");
  
  Iup::Split split(bt, ml);
//  split.SetAttribute("ORIENTATION", "VERTICAL");
  split.SetAttribute("ORIENTATION", "HORIZONTAL");
  split.SetAttribute("COLOR", "127 127 255");

  Iup::Vbox vbox(split);
  vbox.SetAttribute("MARGIN", "10x10");
  vbox.SetAttribute("GAP", "10");

  Iup::Dialog dlg(vbox);
  dlg.SetAttribute("TITLE", "IupSplit Example");

  dlg.Show();
}

#ifndef BIG_TEST
int main(int argc, char* argv[])
{
  Iup::Open(argc, argv);

  SplitTest();

  Iup::MainLoop();

  Iup::Close();

  return 0;
}
#endif
