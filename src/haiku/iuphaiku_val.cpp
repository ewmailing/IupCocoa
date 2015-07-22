/** \file
 * \brief Valuator Control
 *
 * See Copyright Notice in "iup.h"
 */

#define UNIMPLEMENTED printf("%s (%s %d) UNIMPLEMENTED\n",__func__,__FILE__,__LINE__);

#include <Slider.h>

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

#include "iuphaiku_drv.h"


void iupdrvValGetMinSize(Ihandle* ih, int *w, int *h)
{
  if (ih->data->orientation == IVAL_HORIZONTAL)
  {
    *w = 20;
    *h = 35;
  }
  else
  {
    *w = 35;
    *h = 20;
  }
}

static int gtkValSetStepAttrib(Ihandle* ih, const char* value)
{
  ih->data->step = atof(value);
  BSlider* slider = (BSlider*)ih->handle;
  slider->SetKeyIncrementValue(ih->data->step);
  return 0; /* do not store value in hash table */
}

static int gtkValSetPageStepAttrib(Ihandle* ih, const char* value)
{
	UNIMPLEMENTED
  return 0; /* do not store value in hash table */
}

static int gtkValSetValueAttrib(Ihandle* ih, const char* value)
{
	UNIMPLEMENTED
  return 0; /* do not store value in hash table */
}


/*****************************************************************************/



static int gtkValMapMethod(Ihandle* ih)
{
  BSlider* slider = new BSlider(BRect(0,0,10,10), NULL, NULL, NULL, 0, 100);
  ih->handle = (InativeHandle*)slider;
  if (ih->data->orientation == IVAL_HORIZONTAL)
    slider->SetOrientation(B_HORIZONTAL);
  else
    slider->SetOrientation(B_VERTICAL);
  
  // TODO handle min/max value at construction (can't change it later...)

  iuphaikuBaseAddToParent(ih);
  return IUP_NOERROR;
}

void iupdrvValInitClass(Iclass* ic)
{
  /* Driver Dependent Class functions */
  ic->Map = gtkValMapMethod;

  /* Driver Dependent Attribute functions */
  
  /* Visual */
  iupClassRegisterAttribute(ic, "BGCOLOR", NULL, iupdrvBaseSetBgColorAttrib, IUPAF_SAMEASSYSTEM, "DLGBGCOLOR", IUPAF_DEFAULT); 

  /* IupVal only */
  iupClassRegisterAttribute(ic, "VALUE", iupValGetValueAttrib, gtkValSetValueAttrib, IUPAF_SAMEASSYSTEM, "0", IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "PAGESTEP", iupValGetPageStepAttrib, gtkValSetPageStepAttrib, IUPAF_SAMEASSYSTEM, "0.1", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "STEP", iupValGetStepAttrib, gtkValSetStepAttrib, IUPAF_SAMEASSYSTEM, "0.01", IUPAF_NO_INHERIT);

  /* NOT supported */
  // TODO we actually can support these on Haiku, but we can't support PAGESTEP
  iupClassRegisterAttribute(ic, "TICKSPOS", NULL, NULL, NULL, NULL, IUPAF_NOT_SUPPORTED|IUPAF_NOT_MAPPED);
  iupClassRegisterAttribute(ic, "SHOWTICKS", NULL, NULL, NULL, NULL, IUPAF_NOT_SUPPORTED);
}
