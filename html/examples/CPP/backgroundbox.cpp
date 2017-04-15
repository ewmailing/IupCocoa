/* IupBackgroundBox Example in C */

#include <iup_plus.h>

int main(int argc, char **argv)
{
  Iup::Open(argc, argv);

  Iup::Dialog dlg(Iup::Vbox(Iup::BackgroundBox(
    Iup::Frame(
      Iup::Vbox(
        Iup::Button("This button does nothing"),
        Iup::Text()
        ).SetAttributes("MARGIN=0x0")
      )
    )));

  dlg.SetAttributes("MARGIN=10x10, GAP=10, TITLE = \"IupBackgroundBox Example\"");

  dlg.ShowXY(IUP_CENTER, IUP_CENTER);

  Iup::MainLoop();
  Iup::Close();
  return 0;
}
