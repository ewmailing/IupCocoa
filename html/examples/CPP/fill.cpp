/* Iup::Fill: Example in C 
   Uses the Fill element to horizontally centralize a button and to justify it to the left and right.
*/

#include <iup_plus.h>

int main(int argc, char **argv)
{
  /* Initializes IUP */
  Iup::Open(argc, argv);
  
  /* Creates frame with left aligned button */
  Iup::Frame frame_left
  (
    Iup::Hbox
    (
      Iup::Button("Ok"),
      Iup::Fill()
    )
  );

  /* Sets frame's title */
  frame_left.SetAttribute("TITLE", "Left aligned");

  /* Creates frame with centered button */
  Iup::Frame frame_center
  (
    Iup::Hbox
    (
      Iup::Fill (),
      Iup::Button("Ok"),
      Iup::Fill ()
    )
  );

  /* Sets frame's title */
  frame_center.SetAttribute("TITLE", "Centered");

  /* Creates frame with right aligned button */
  Iup::Frame frame_right
  (
    Iup::Hbox
    (
      Iup::Fill (),
      Iup::Button ("Ok")
    )
  );

  /* Sets frame's title */
  frame_right.SetAttribute("TITLE", "Right aligned");

  /* Creates dialog with these three frames */
  Iup::Dialog dialog
  (
    Iup::Vbox
    (
      frame_left,
      frame_center,
      frame_right
    )
  );

  /* Sets dialog's size and title */
  dialog.SetAttributes("SIZE=120, TITLE=Iup::Fill");

  dialog.Show();  /* Shows dialog in the center of the screen */

  Iup::MainLoop();      /* Initializes IUP main loop */

  Iup::Close();         /* Finishes IUP */

  return 0;
}
