/** \file
 * \brief Clipboard for the GTK Driver.
 *
 * See Copyright Notice in "iup.h"
 */


#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#include "iup.h"

#include "iup_object.h"
#include "iup_attrib.h"
#include "iup_str.h"
#include "iup_image.h"

#include "iuphaiku_drv.h"
#define UNIMPLEMENTED printf("%s (%s %d) UNIMPLEMENTED\n",__func__,__FILE__,__LINE__);

static int gtkClipboardSetFormatDataAttrib(Ihandle *ih, const char *value)
{
	UNIMPLEMENTED
		return 0;
}

static char* gtkClipboardGetFormatDataAttrib(Ihandle *ih)
{
	UNIMPLEMENTED
		return NULL;
}

static int gtkClipboardSetTextAttrib(Ihandle *ih, const char *value)
{
	UNIMPLEMENTED
  return 0;
}

static char* gtkClipboardGetTextAttrib(Ihandle *ih)
{
	UNIMPLEMENTED
		return NULL;
}

static int gtkClipboardSetImageAttrib(Ihandle *ih, const char *value)
{
	UNIMPLEMENTED
		return 0;
}

static int gtkClipboardSetNativeImageAttrib(Ihandle *ih, const char *value)
{
	UNIMPLEMENTED
		return 0;
}

static char* gtkClipboardGetNativeImageAttrib(Ihandle *ih)
{
	UNIMPLEMENTED
		return NULL;
}

static char* gtkClipboardGetTextAvailableAttrib(Ihandle *ih)
{
	UNIMPLEMENTED
		return NULL;
}

static char* gtkClipboardGetImageAvailableAttrib(Ihandle *ih)
{
	UNIMPLEMENTED
		return NULL;
}

static char* gtkClipboardGetFormatAvailableAttrib(Ihandle *ih)
{
	UNIMPLEMENTED
		return NULL;
}

static int gtkClipboardSetAddFormatAttrib(Ihandle *ih, const char *value)
{
	UNIMPLEMENTED;
  return 0;
}

/******************************************************************************/

Ihandle* IupClipboard(void)
{
  return IupCreate("clipboard");
}

Iclass* iupClipboardNewClass(void)
{
  Iclass* ic = iupClassNew(NULL);

  ic->name = "clipboard";
  ic->format = NULL;  /* no parameters */
  ic->nativetype = IUP_TYPECONTROL;
  ic->childtype = IUP_CHILDNONE;
  ic->is_interactive = 0;

  ic->New = iupClipboardNewClass;

  /* Attribute functions */
  iupClassRegisterAttribute(ic, "TEXT", gtkClipboardGetTextAttrib, gtkClipboardSetTextAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "TEXTAVAILABLE", gtkClipboardGetTextAvailableAttrib, NULL, NULL, NULL, IUPAF_READONLY|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "NATIVEIMAGE", gtkClipboardGetNativeImageAttrib, gtkClipboardSetNativeImageAttrib, NULL, NULL, IUPAF_NO_STRING|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "IMAGE", NULL, gtkClipboardSetImageAttrib, NULL, NULL, IUPAF_IHANDLENAME|IUPAF_WRITEONLY|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "IMAGEAVAILABLE", gtkClipboardGetImageAvailableAttrib, NULL, NULL, NULL, IUPAF_READONLY|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "ADDFORMAT", NULL, gtkClipboardSetAddFormatAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "FORMAT", NULL, NULL, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "FORMATAVAILABLE", gtkClipboardGetFormatAvailableAttrib, NULL, NULL, NULL, IUPAF_READONLY|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "FORMATDATA", gtkClipboardGetFormatDataAttrib, gtkClipboardSetFormatDataAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "FORMATDATASIZE", NULL, NULL, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);

  return ic;
}
