/* Iup Alarm: Example in C 
   Shows a dialog similar to the one shown when you exit a program without saving.
*/

/* IUP libraries include */
#include <iup_plus.h>

/* Main program */
int main(int argc, char **argv)
{
  /* Initializes IUP */
  Iup::Open(argc, argv);

  /* Executes IupAlarm */
  switch (Iup::Alarm("IupAlarm Example", "File not saved! Save it now?", "Yes", "No", "Cancel"))
  {
    /* Shows a message for each selected button */
    case 1:
      Iup::Message("Save file", "File saved successfully - leaving program");
    break;

    case 2:
      Iup::Message("Save file", "File not saved - leaving program anyway");
    break;

    case 3:
      Iup::Message("Save file", "Operation canceled");
    break;
  }
  
  /* Finishes IUP */
  Iup::Close();

  /* Program finished successfully */
  return 0;
}
