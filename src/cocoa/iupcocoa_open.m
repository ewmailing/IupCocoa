/** \file
 * \brief MAC Driver Core
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdio.h>          
#include <stdlib.h>
#include <string.h>          

#include "iup.h"

#include "iup_str.h"
#include "iup_drv.h"
#include "iup_drvinfo.h"
#include "iup_object.h"
#include "iup_globalattrib.h"

#include "iupcocoa_drv.h"

#if 0
char* iupmacGetNativeWindowHandle(Ihandle* ih)
{
  id window = ih->handle->window;
  if (window)
    return (char*)window;
  else
    return NULL;
}
#endif

void* iupdrvGetDisplay(void)
{
  return NULL;
}


void iupmacUpdateGlobalColors(void)
{
  iupGlobalSetDefaultColorAttrib("DLGBGCOLOR", 237,237,237);

  iupGlobalSetDefaultColorAttrib("DLGFGCOLOR", 0,0,0);

  iupGlobalSetDefaultColorAttrib("TXTBGCOLOR", 255,255,255);

  iupGlobalSetDefaultColorAttrib("TXTFGCOLOR", 0,0,0);

  iupGlobalSetDefaultColorAttrib("MENUBGCOLOR", 183,183,183);

  iupGlobalSetDefaultColorAttrib("MENUFGCOLOR", 0,0,0);
}

int iupdrvOpen(int *argc, char ***argv)
{                        
  (void)argc; /* unused in the mac driver */
  (void)argv;

  IupSetGlobal("DRIVER", "MAC");

//  IupSetGlobal("SYSTEMLANGUAGE", iupmacGetSystemLanguage());

  iupmacUpdateGlobalColors();

  IupSetGlobal("_IUP_RESET_GLOBALCOLORS", "YES");  /* will update the global colors when the first dialog is mapped */

  return IUP_NOERROR;
}

void iupdrvClose(void)
{
//  iupmacReleaseConvertUTF8();
}
