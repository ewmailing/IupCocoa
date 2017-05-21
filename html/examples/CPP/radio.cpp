/*IupRadio Example in C 
Creates a dialog for the user to select his/her gender. 
In this case, the radio element is essential to prevent the user from selecting both options. */

#include <iup_plus.h>

int main(int argc, char **argv)
{
  Iup::Open(argc, argv);

  Iup::Toggle male("Male");
  Iup::Toggle female("Female");

  Iup::Radio exclusive
  (
    Iup::Vbox
    (
      male,
      female
    )
  );
  exclusive.SetAttributeHandle("VALUE", female);
  male.SetAttribute("TIP", "Two state button - Exclusive - RADIO");
  female.SetAttribute("TIP", "Two state button - Exclusive - RADIO");

  Iup::Frame frame
  (
    exclusive
  );
  frame.SetAttribute("TITLE", "Gender");

  Iup::Dialog dialog
  (
    Iup::Hbox
    (
      Iup::Fill(),
      frame,
      Iup::Fill()
    )
  );

  dialog.SetAttributes("SIZE=140, TITLE=IupRadio, RESIZE=NO, MINBOX=NO, MAXBOX=NO");

  dialog.Show();

  Iup::MainLoop();

  Iup::Close();

  return 0;
}
