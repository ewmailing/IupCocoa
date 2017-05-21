/** \file
 * \brief Calendar Control
 *
 * See Copyright Notice in "iup.h"
 */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <memory.h>
#include <stdarg.h>
#include <limits.h>

#include "iup.h"
#include "iupcbs.h"

#include "iup_object.h"
#include "iup_attrib.h"
#include "iup_str.h"
#include "iup_drv.h"
#include "iup_drvfont.h"



Iclass* iupCalendarNewClass(void)
{
  Iclass* ic = iupClassNew(NULL);

  ic->name = "calendar";
  ic->format = NULL; /* none */
  ic->nativetype = IUP_TYPECONTROL;
  ic->childtype = IUP_CHILDNONE;
  ic->is_interactive = 1;

  /* Class functions */
  ic->New = iupCalendarNewClass;

#if 0
  ic->Map = gtkCalendarMapMethod;
  ic->ComputeNaturalSize = gtkCalendarComputeNaturalSizeMethod;

  ic->LayoutUpdate = iupdrvBaseLayoutUpdateMethod;
  ic->UnMap = iupdrvBaseUnMapMethod;

  /* Callbacks */
  iupClassRegisterCallback(ic, "VALUECHANGED_CB", "");

  /* Common Callbacks */
  iupBaseRegisterCommonCallbacks(ic);

  /* Common */
  iupBaseRegisterCommonAttrib(ic);

  /* Visual */
  iupBaseRegisterVisualAttrib(ic);

  /* IupCalendar only */
  iupClassRegisterAttribute(ic, "VALUE", gtkCalendarGetValueAttrib, gtkCalendarSetValueAttrib, NULL, "TODAY", IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "WEEKNUMBERS", NULL, gtkCalendarSetWeekNumbersAttrib, NULL, NULL, IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "TODAY", gtkCalendarGetTodayAttrib, NULL, NULL, NULL, IUPAF_READONLY | IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);
#endif

  return ic;
}

Ihandle *IupCalendar(void)
{
  return IupCreate("calendar");
}
