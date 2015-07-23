/** \file
* \brief Progress bar Control
*
* See Copyright Notice in "iup.h"
*/

#define UNIMPLEMENTED printf("%s (%s %d) UNIMPLEMENTED\n",__func__,__FILE__,__LINE__);

#include <StatusBar.h>

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
#include "iup_progressbar.h"
#include "iup_drv.h"

#include "iuphaiku_drv.h"

static int haikuProgressBarSetValueAttrib(Ihandle* ih, const char* value)
{
  if (ih->data->marquee)
    return 0;

  if (!value)
    ih->data->value = 0;
  else
    ih->data->value = atof(value);
  iProgressBarCropValue(ih);

  BStatusBar* bar = (BStatusBar*)ih->handle;
  float val = bar->CurrentValue();
  bar->Update(ih->data->value - val);

  return 1;
}

static int haikuProgressBarSetMaxAttrib(Ihandle* ih, const char* value)
{
  if (ih->data->marquee)
    return 0;

  if (!value)
    ih->data->vmax = 1;
  else
    ih->data->vmax = atof(value);
  iProgressBarCropValue(ih);

  BStatusBar* bar = (BStatusBar*)ih->handle;
  bar->SetMaxValue(ih->data->vmax);

  return 1;
}

static int haikuProgressBarMapMethod(Ihandle* ih)
{
  BStatusBar* bar = new BStatusBar(BRect(0,0,200,30), "progress");
  bar->SetFlags(bar->Flags() | B_SUPPORTS_LAYOUT);
  	// We need this to allow the bar to have no labels

  bar->SetMaxValue(1.0f);

  ih->handle = (InativeHandle*)bar;
  
  if (iupStrEqualNoCase(iupAttribGetStr(ih, "ORIENTATION"), "VERTICAL"))
	UNIMPLEMENTED

  if (iupAttribGetBoolean(ih, "MARQUEE"))
	UNIMPLEMENTED

  iuphaikuBaseAddToParent(ih);
  return IUP_NOERROR;
}


static void haikuProgressLayoutUpdateMethod(Ihandle *ih)
{
  iupdrvBaseLayoutUpdateMethod(ih);

  BStatusBar* bar = (BStatusBar*)ih->handle;
  bar->SetBarHeight(ih->currentheight - 1);
}


void iupdrvProgressBarInitClass(Iclass* ic)
{
  /* Driver Dependent Class functions */
  ic->Map = haikuProgressBarMapMethod;
  ic->LayoutUpdate = haikuProgressLayoutUpdateMethod;
  
  // TODO Callbacks : BGCOLOR, FGCOLOR, DASHED, ORIENTATION, MARQUEE

  /* IupProgressBar only */
  iupClassRegisterAttribute(ic, "VALUE",  iProgressBarGetValueAttrib,  haikuProgressBarSetValueAttrib,  NULL, NULL, IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "MAX",  NULL,  haikuProgressBarSetMaxAttrib,  NULL, NULL, IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);
}
