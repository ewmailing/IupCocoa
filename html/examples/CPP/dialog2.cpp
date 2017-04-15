#include <iup_plus.h>

static int quit_cb(void)
{
  return IUP_CLOSE;
}

int main(int argc, char **argv)
{
  Iup::Open(argc, argv);

  /* Creating the button */ 
  Iup::Button quit_bt("Quit");
  quit_bt.SetCallback("ACTION", (Icallback)quit_cb);

  /* the container with a label and the button */
  Iup::Vbox vbox(
    Iup::Label("Very Long Text Label").SetAttributes("EXPAND=YES, ALIGNMENT=ACENTER"),
    quit_bt);
  vbox.SetAttribute("ALIGNMENT", "ACENTER");
  vbox.SetAttribute("MARGIN", "10x10");
  vbox.SetAttribute("GAP", "5");

  /* Creating the dialog */ 
  Iup::Dialog dialog(vbox);
  dialog.SetAttribute("TITLE", "Dialog Title");
  dialog.SetAttributeHandle("DEFAULTESC", quit_bt);

  dialog.Show();

  Iup::MainLoop();
  
  dialog.Destroy();

  Iup::Close();

  return 0;
}
