#include <stdio.h>
#include "iup.h"

int main(int argc, char **argv)
{
  char *error;
  Ihandle *dlg; 

  IupOpen(&argc, &argv);
  
  /* Loads LED file */
  error = IupLoad("filedlg.led");
  if (error)
  {
    IupMessage("LED error", error);
    return 1 ;
  }

  dlg = IupGetHandle("dlg");  
  IupPopup(dlg, IUP_CENTER, IUP_CENTER); 

  switch(IupGetInt(dlg, "STATUS"))
  {
    case 1: 
      IupMessage("New file",IupGetAttribute(dlg, "VALUE"));	    
    break ;	    
    
    case 0 : 
      IupMessage("File already exists.",IupGetAttribute(dlg, "VALUE"));
    break ;	    
    
    case -1 : 
      IupMessage("IupFileDlg","Operation Canceled");
      return 1;
    break ;	    
  }

  IupDestroy(dlg);
  IupClose();

  return 0;
}