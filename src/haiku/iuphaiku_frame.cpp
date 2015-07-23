/** \file
 * \brief Frame Control
 *
 * See Copyright Notice in "iup.h"
 */

#define UNIMPLEMENTED printf("%s (%s %d) UNIMPLEMENTED\n",__func__,__FILE__,__LINE__);

#include <Box.h>
#include <Rect.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <stdarg.h>

#include "iup.h"

#include "iup_object.h"
#include "iup_layout.h"
#include "iup_attrib.h"
#include "iup_str.h"
#include "iup_dialog.h"
#include "iup_drv.h"
#include "iup_drvfont.h"
#include "iup_stdcontrols.h"

#include "iuphaiku_drv.h"


extern "C" {
void iupdrvFrameGetDecorOffset(int *x, int *y)
{
  *x = 3;
  *y = 1;
}

int iupdrvFrameHasClientOffset(void)
{
  return 1;
}
}

static char* beFrameGetTitleAttrib(Ihandle* ih)
{
	UNIMPLEMENTED
		return NULL;
}

static int beFrameSetTitleAttrib(Ihandle* ih, const char* value)
{
  if (iupAttribGetStr(ih, "_IUPFRAME_HAS_TITLE"))
  {
	BBox* box = (BBox*)ih->handle;
	box->SetLabel(value);
  }
  return 0;
}

static int gtkFrameSetBgColorAttrib(Ihandle* ih, const char* value)
{
	UNIMPLEMENTED
  return 1;
}

static int gtkFrameSetFgColorAttrib(Ihandle* ih, const char* value)
{
	UNIMPLEMENTED
  return 1;
}

/*
static int beFrameSetStandardFontAttrib(Ihandle* ih, const char* value)
{
  iupdrvSetStandardFontAttrib(ih, value);

  if (ih->handle)
  {
    
  }
  return 1;
}

static void* beFrameGetInnerNativeContainerHandleMethod(Ihandle* ih, Ihandle* child)
{
  return ih->handle;
}
*/

static int beFrameMapMethod(Ihandle* ih)
{
  char *value, *title = iupAttribGet(ih, "TITLE");
  BBox* bbox = new BBox(BRect(0, 0, 10, 10), title);
  ih->handle = (InativeHandle*)bbox;
  new BBox(BRect(0, 0, 10, 10), title);

  if(!ih->handle)
	return IUP_ERROR;

  title = iupAttribGet(ih, "TITLE");

  if (title)
    iupAttribSet(ih, "_IUPFRAME_HAS_TITLE", "1");
  else
  {
    value = iupAttribGetStr(ih, "SUNKEN");
    if (iupStrBoolean(value))
	  bbox->SetBorder(B_FANCY_BORDER);
    else
	  bbox->SetBorder(B_PLAIN_BORDER);

    if (iupAttribGet(ih, "BGCOLOR"))
      iupAttribSet(ih, "_IUPFRAME_HAS_BGCOLOR", "1");
  }

  iuphaikuBaseAddToParent(ih);

  return IUP_NOERROR;
}

extern "C" {
void iupdrvFrameInitClass(Iclass* ic)
{
  /* Driver Dependent Class functions */
  ic->Map = beFrameMapMethod;
//  ic->GetInnerNativeContainerHandle = beFrameGetInnerNativeContainerHandleMethod;

  /* Driver Dependent Attribute functions */

  /* Overwrite Common */
  //iupClassRegisterAttribute(ic, "STANDARDFONT", NULL, beFrameSetStandardFontAttrib, IUPAF_SAMEASSYSTEM, "DEFAULTFONT", IUPAF_NO_SAVE|IUPAF_NOT_MAPPED);

  /* Visual */
  iupClassRegisterAttribute(ic, "BGCOLOR", NULL, gtkFrameSetBgColorAttrib, IUPAF_SAMEASSYSTEM, "DLGBGCOLOR", IUPAF_DEFAULT);

  /* Special */
  iupClassRegisterAttribute(ic, "FGCOLOR", NULL, gtkFrameSetFgColorAttrib, IUPAF_SAMEASSYSTEM, "DLGFGCOLOR", IUPAF_DEFAULT);
  iupClassRegisterAttribute(ic, "TITLE", beFrameGetTitleAttrib, beFrameSetTitleAttrib, NULL, NULL, IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);
}
}
