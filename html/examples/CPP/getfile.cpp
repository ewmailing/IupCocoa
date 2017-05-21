/*  IupGetFile: Example in C 
    Shows a typical file-selection dialog.
*/

#include <iup_plus.h>

int main(int argc, char **argv)
{
  char file[10240] = "*.txt";

  Iup::Open(argc, argv);
  Iup::SetLanguage("ENGLISH");

  switch(IupGetFile(file))
  {
    case 1: 
      Iup::Message("New file", file);
      break;
    case 0:
      Iup::Message("File already exists", file);
      break;
    case -1:
      Iup::Message("IupFileDlg", "Operation canceled");
      break;
    case -2:
      Iup::Message("IupFileDlg", "Allocation error");
      break;
    case -3:
      Iup::Message("IupFileDlg", "Invalid parameter");
      break;	    
  }

  Iup::Close();

  return 0;
}
