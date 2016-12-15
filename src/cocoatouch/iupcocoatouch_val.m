/** \file
 * \brief Valuator Control
 *
 * See Copyright Notice in "iup.h"
 */

#import <UIKit/UIKit.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <stdarg.h>

#include "iup.h"
#include "iupcbs.h"

#include "iup_object.h"
#include "iup_layout.h"
#include "iup_attrib.h"
#include "iup_str.h"
#include "iup_val.h"
#include "iup_drv.h"
#include "iup_drvfont.h"
#include "iup_key.h"

#include "iupcocoatouch_drv.h"


void iupdrvValGetMinSize(Ihandle* ih, int *w, int *h)
{
#if 0
  if (ih->data->type == IVAL_HORIZONTAL)
  {
    *w = 20;
    *h = 35;
  }
  else
  {
    *w = 35;
    *h = 20;
  }
#endif
}


void iupdrvValInitClass(Iclass* ic)
{
#if 0
  /* Driver Dependent Class functions */
  ic->Map = macValMapMethod;

  /* Driver Dependent Attribute functions */
  
  /* Visual */
  iupClassRegisterAttribute(ic, "BGCOLOR", NULL, iupdrvBaseSetBgColorAttrib, IUPAF_SAMEASSYSTEM, "DLGBGCOLOR", IUPAF_DEFAULT); 

  /* Special */

  /* IupVal only */
  iupClassRegisterAttribute(ic, "VALUE", iupValGetValueAttrib, macValSetValueAttrib, NULL, NULL, IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "PAGESTEP", iupValGetPageStepAttrib, macValSetPageStepAttrib, IUPAF_SAMEASSYSTEM, "0.1", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "STEP", iupValGetStepAttrib, macValSetStepAttrib, IUPAF_SAMEASSYSTEM, "0.01", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SHOWTICKS", NULL, NULL, NULL, NULL, IUPAF_WRITEONLY|IUPAF_READONLY);  /* showticks is not supported in GTK */
#endif
}
