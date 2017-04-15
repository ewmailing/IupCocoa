/* IupMessage Example */

/* IUP libraries include */
#include <iup_plus.h>

/* Main program */
int main(int argc, char **argv)
{
  /* Initializes IUP */
  Iup::Open(argc, argv);
  
  /* Executes IupMessage */
  Iup::Message("IupMessage Example", "Press the button");

  /* Finishes IUP */
  Iup::Close();

  /* Program finished successfully */
  return 0;
}
