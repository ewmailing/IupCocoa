/*IupFileDlg Example in C 
Shows a typical file-saving dialog. */
#include <iup_plus.h>

int main(int argc, char **argv)
{
  Iup::Open(argc, argv);
  Iup::SetLanguage("ENGLISH");
  
  Iup::FileDlg filedlg;
 
  filedlg.SetAttributes("DIALOGTYPE = SAVE, TITLE = \"File Save\"");
  filedlg.SetAttributes("FILTER = \"*.bmp\", FILTERINFO = \"Bitmap Files\"");

  filedlg.Popup(IUP_CENTER, IUP_CENTER);

  switch (filedlg.GetInteger("STATUS"))
  {
    case 1: 
      Iup::Message("New file", filedlg.GetAttribute("VALUE"));
      break; 

    case 0 : 
      Iup::Message("File already exists", filedlg.GetAttribute("VALUE"));
      break; 

    case -1 : 
      Iup::Message("IupFileDlg", "Operation Canceled");
      break; 
  }

  filedlg.Destroy();

  Iup::Close(); 
  return 0;
}
