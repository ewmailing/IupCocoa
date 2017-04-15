/*Iup::Frame Example in C 
Draws a frame around a button. Note that "FGCOLOR" is added to the frame but it is inherited by the button. */

#include <iup_plus.h>

int main(int argc, char **argv)
{
  /* Initializes IUP */  
  Iup::Open(argc, argv);

  /* Creates frame with a label */
  Iup::Frame frame
          (
            Iup::Hbox
            (
              Iup::Fill(),
              Iup::Label ("Iup::Frame Attributes:\nFGCOLOR = \"255 0 0\"\nSIZE = \"EIGHTHxEIGHTH\"\nTITLE = \"This is the frame\"\nMARGIN = \"10x10\""),
              Iup::Fill()
            )
          );

  /* Sets frame's attributes */
  frame.SetAttributes("FGCOLOR=\"255 0 0\", SIZE=EIGHTHxEIGHTH, TITLE=\"This is the frame\", MARGIN=10x10");

  /* Creates dialog  */
  Iup::Dialog dialog(frame);

  /* Sets dialog's title */
  dialog.SetAttribute("TITLE", "Iup::Frame");

  dialog.Show();  /* Shows dialog in the center of the screen */
  Iup::MainLoop();      /* Initializes IUP main loop */
  Iup::Close();         /* Finishes IUP */

  return 0;
}
