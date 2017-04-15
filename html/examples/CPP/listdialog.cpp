/* IupListDialog: Example in C 
   Shows a color-selection dialog.
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iup_plus.h>

int main(int argc, char **argv)
{
  Iup::Open(argc, argv);
  Iup::SetLanguage("ENGLISH");
  
  int size = 8;
  int marks[8] = { 0, 0, 0, 0, 1, 1, 0, 0 };
  char *options[] = {
    "Blue",
    "Red",
    "Green",
    "Yellow",
    "Black",
    "White",
    "Gray",
    "Brown" };
  int error = Iup::ListDialog(2, "Color Selection", size, (const char**)options, 0, 16, 5, marks);

  if(error == -1)
  {
    Iup::Message("IupListDialog", "Operation canceled");
  }
  else
  {
    int i;
    char selection[80] = "";
	  
    for(i = 0; i < size; i++)
    {
      if(marks[i])
      {
        char temp[10];	      
        sprintf(temp,"%s\n",options[i]);      
        strcat(selection,temp);
      }
    }
    
    if (selection[0] != 0)
      Iup::Message("Options selected", selection);
    else
      Iup::Message("IupListDialog","No option selected");
  }
  
  Iup::Close();

  return 0;
}
