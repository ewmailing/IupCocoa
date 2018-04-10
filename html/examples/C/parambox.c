/* IupGetParam: Example in C 
   Shows a dialog with all the possible fields. 
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <iup.h>

static int param_action(Ihandle* dialog, int param_index, void* user_data)
{
  switch (param_index)
  {
  case IUP_GETPARAM_BUTTON1:
    printf("Button1 (OK)\n");
    break;
  case IUP_GETPARAM_MAP:
    printf("Map\n");
    break;
  case IUP_GETPARAM_INIT:  /* not called for IupParamBox */
    printf("Init\n");
    break;
  case IUP_GETPARAM_BUTTON2:
    printf("Button2 (Cancel)\n");
    break;
  case IUP_GETPARAM_BUTTON3:
    printf("Button3 (Help)\n");
    break;
  case IUP_GETPARAM_CLOSE:  /* not called for IupParamBox */
    printf("Close\n");
    break;
  default:
    {
      Ihandle* param;
      char param_str[50];
      sprintf(param_str, "PARAM%d", param_index);
      param = (Ihandle*)IupGetAttribute(dialog, param_str);
      printf("%s = %s\n", param_str, IupGetAttribute(param, "VALUE"));
      break;
    }
  }
  return 1;
}

int main(int argc, char **argv)
{
  int pboolean = 1;
  int pinteger = 3456;
  float preal = 3.543f;
  int pinteger2 = 192;
  float preal2 = 0.5f;
  float pangle = 90;
  char pstring[100] = "string text";
  char pcolor[100] = "255 0 128";
  int plist = 2;
  char pfile_name[500] = "test.jpg";
  Ihandle *dlg, *box, *hbox2, *param1, *param2, *param3, *param4, *param5, *param6, *param7, *param8, *param9, *param10, *param11, *param12, *parambox;
  
  IupOpen(&argc, &argv);
  IupSetLanguage("ENGLISH");

  param1 = IupParam("Boolean: %b[No,Yes]{Boolean Tip}\n");
  param2 = IupParam("Integer: %i{Integer Tip}\n");
  param3 = IupParam("Real 1: %r{Real Tip}\n");
  param4 = IupParam("Sep1 %t\n");
  param5 = IupParam("Integer: %i[0,255]{Integer Tip 2}\n");
  param6 = IupParam("Real 2: %r[-1.5,1.5]{Real Tip 2}\n");
  param7 = IupParam("Sep2 %t\n");
  param8 = IupParam("Angle: %a[0,360]{Angle Tip}\n");
  param9 = IupParam("String: %s{String Tip}\n");
  param10 = IupParam("List: %l|item1|item2|item3|{List Tip}\n");
  param11 = IupParam("File: %f[OPEN|*.bmp;*.jpg|CURRENT|NO|NO]{File Tip}\n");
  param12 = IupParam("Color: %c{Color Tip}\n");

  IupSetInt(param1, "VALUE", pboolean);
  IupSetInt(param2, "VALUE", pinteger);
  IupSetFloat(param3, "VALUE", preal);
  IupSetInt(param5, "VALUE", pinteger2);
  IupSetFloat(param6, "VALUE", preal2);
  IupSetFloat(param8, "VALUE", pangle);
  IupSetAttribute(param9, "VALUE", pstring);
  IupSetInt(param10, "VALUE", plist);
  IupSetAttribute(param11, "VALUE", pfile_name);
  IupSetAttribute(param12, "VALUE", pcolor);
 
  hbox2 = IupHbox(IupLabel("Settings"), IupButton("Extra Button", NULL), NULL);

  box = IupVbox(
    parambox = IupParamBox(param1, param2, param3, param4, param5, param6, param7, param8, param9, param10, param11, param12, NULL),
    hbox2,
    NULL);

  IupSetCallback(parambox, "PARAM_CB", (Icallback)param_action);

  dlg = IupDialog(box);
  IupSetAttribute(dlg, "TITLE", "IupParamBox Example");

  IupPopup(dlg, IUP_CENTER, IUP_CENTER);

  if (IupGetInt(parambox, "STATUS") == 1)
  {
    pboolean = IupGetInt(param1, "VALUE");
    pinteger = IupGetInt(param2, "VALUE");
    preal = IupGetFloat(param3, "VALUE");
    pinteger2 = IupGetInt(param5, "VALUE");
    preal2 = IupGetFloat(param6, "VALUE");
    pangle = IupGetFloat(param8, "VALUE");
    strcpy(pstring, IupGetAttribute(param9, "VALUE"));
    plist = IupGetInt(param10, "VALUE");
    strcpy(pfile_name, IupGetAttribute(param11, "VALUE"));
    strcpy(pcolor, IupGetAttribute(param12, "VALUE"));

    IupMessagef("IupGetParam",
                "Boolean Value: %d\n"
                "Integer: %d\n"
                "Real 1: %g\n"
                "Integer: %d\n"
                "Real 2: %g\n"
                "Angle: %g\n"
                "String: %s\n"
                "List Index: %d\n"
                "FileName: %s\n"
                "Color: %s\n",
                pboolean, pinteger, (double)preal, pinteger2, (double)preal2, (double)pangle, pstring, plist, pfile_name, pcolor);
  }

  IupMainLoop();
  IupClose();

  return EXIT_SUCCESS;
}
