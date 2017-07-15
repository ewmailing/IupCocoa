/** \file
 * \brief Text Control
 *
 * See Copyright Notice in "iup.h"
 */

#define UNIMPLEMENTED printf("%s (%s %d) UNIMPLEMENTED\n",__func__,__FILE__,__LINE__);

#include <TextControl.h>

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
#include "iup_image.h"
#include "iup_mask.h"
#include "iup_drv.h"
#include "iup_drvfont.h"
#include "iup_image.h"
#include "iup_key.h"
#include "iup_array.h"
#include "iup_text.h"

#include "iuphaiku_drv.h"

#ifndef PANGO_WEIGHT_SEMIBOLD
#define PANGO_WEIGHT_SEMIBOLD 600
#endif

void iupdrvTextAddSpin(Ihandle* ih, int *w, int h)
{
  int spin_size = 16;
  *w += spin_size;
  (void)h;
  (void)ih;
}

void iupdrvTextAddBorders(Ihandle* ih, int *x, int *y)
{
  int border_size = 2*5;
  (*x) += border_size;
  (*y) += border_size;
  (void)ih;
}

void iupdrvTextConvertLinColToPos(Ihandle* ih, int lin, int col, int *pos)
{
	UNIMPLEMENTED
}

void iupdrvTextConvertPosToLinCol(Ihandle* ih, int pos, int *lin, int *col)
{
	UNIMPLEMENTED
}

/*******************************************************************************************/


static int gtkTextSetSelectionAttrib(Ihandle* ih, const char* value)
{
	UNIMPLEMENTED
  return 0;
}

static char* gtkTextGetSelectionAttrib(Ihandle* ih)
{
	UNIMPLEMENTED
  return NULL;
}

static int gtkTextSetSelectionPosAttrib(Ihandle* ih, const char* value)
{
	UNIMPLEMENTED
  return 0;
}

static char* gtkTextGetSelectionPosAttrib(Ihandle* ih)
{
	UNIMPLEMENTED
  return NULL;
}

static int gtkTextSetSelectedTextAttrib(Ihandle* ih, const char* value)
{
	UNIMPLEMENTED
  return 0;
}

static char* gtkTextGetCountAttrib(Ihandle* ih)
{
	UNIMPLEMENTED
  return NULL;
}

static char* gtkTextGetLineCountAttrib(Ihandle* ih)
{
	UNIMPLEMENTED
  return NULL;
}

static char* gtkTextGetSelectedTextAttrib(Ihandle* ih)
{
	UNIMPLEMENTED
  return NULL;
}

static int gtkTextSetCaretAttrib(Ihandle* ih, const char* value)
{
	UNIMPLEMENTED
  return 0;
}

static char* gtkTextGetCaretAttrib(Ihandle* ih)
{
	UNIMPLEMENTED
  return NULL;
}

static int gtkTextSetCaretPosAttrib(Ihandle* ih, const char* value)
{
	UNIMPLEMENTED
  return 0;
}

static char* gtkTextGetCaretPosAttrib(Ihandle* ih)
{
	UNIMPLEMENTED
  return NULL;
}

static int gtkTextSetScrollToAttrib(Ihandle* ih, const char* value)
{
	UNIMPLEMENTED
  return 0;
}

static int gtkTextSetScrollToPosAttrib(Ihandle* ih, const char* value)
{
	UNIMPLEMENTED
  return 0;
}

static int gtkTextSetValueAttrib(Ihandle* ih, const char* value)
{
  BView* view = (BView*)ih->handle;

  BTextControl* ctrl = dynamic_cast<BTextControl*>(view);
  if (ctrl) {
	ctrl->SetText(value);
  } else
	UNIMPLEMENTED
  return 0;
}

static char* gtkTextGetValueAttrib(Ihandle* ih)
{
	UNIMPLEMENTED
  return NULL;
}
                       
static char* gtkTextGetLineValueAttrib(Ihandle* ih)
{
	UNIMPLEMENTED
  return NULL;
}

static int gtkTextSetInsertAttrib(Ihandle* ih, const char* value)
{
	UNIMPLEMENTED
  return 0;
}

static int gtkTextSetAppendAttrib(Ihandle* ih, const char* value)
{
	UNIMPLEMENTED
  return 0;
}

static int gtkTextSetAlignmentAttrib(Ihandle* ih, const char* value)
{
	UNIMPLEMENTED
  return 1;
}

static int gtkTextSetPaddingAttrib(Ihandle* ih, const char* value)
{
	UNIMPLEMENTED
    return 1; /* store until not mapped, when mapped will be set again */
}

static int gtkTextSetNCAttrib(Ihandle* ih, const char* value)
{
	UNIMPLEMENTED
    return 1; /* store until not mapped, when mapped will be set again */
}

static int gtkTextSetClipboardAttrib(Ihandle *ih, const char *value)
{
	UNIMPLEMENTED
  return 0;
}

static int gtkTextSetReadOnlyAttrib(Ihandle* ih, const char* value)
{
	UNIMPLEMENTED
  return 0;
}

static char* gtkTextGetReadOnlyAttrib(Ihandle* ih)
{
	UNIMPLEMENTED
  return NULL;
}

static char* gtkTextGetWidgetPangoLayoutAttrib(Ihandle* ih)
{
  if (ih->data->is_multiline)
    return NULL;
  else
	UNIMPLEMENTED
  return NULL;
}

static int gtkTextSetBgColorAttrib(Ihandle* ih, const char* value)
{
	UNIMPLEMENTED
		return 0;
}

static int gtkTextSetTabSizeAttrib(Ihandle* ih, const char* value)
{
	UNIMPLEMENTED
  return 1;
}

static int gtkTextSetOverwriteAttrib(Ihandle* ih, const char* value)
{
  if (!ih->data->is_multiline)
    return 0;
	UNIMPLEMENTED
  return 0;
}

static char* gtkTextGetOverwriteAttrib(Ihandle* ih)
{
  if (!ih->data->is_multiline)
    return "NO";
	UNIMPLEMENTED
	return NULL;
}

void* iupdrvTextAddFormatTagStartBulk(Ihandle* ih)
{
	UNIMPLEMENTED
  return NULL;
}

void iupdrvTextAddFormatTagStopBulk(Ihandle* ih, void* state)
{
	UNIMPLEMENTED
}

void iupdrvTextAddFormatTag(Ihandle* ih, Ihandle* formattag, int bulk)
{
	UNIMPLEMENTED
}

static int gtkTextSetRemoveFormattingAttrib(Ihandle* ih, const char* value)
{
	UNIMPLEMENTED
  return 0;
}


static int gtkTextSetSpinMinAttrib(Ihandle* ih, const char* value)
{
	UNIMPLEMENTED
  return 1;
}

static int gtkTextSetSpinMaxAttrib(Ihandle* ih, const char* value)
{
	UNIMPLEMENTED
  return 1;
}

static int gtkTextSetSpinIncAttrib(Ihandle* ih, const char* value)
{
	UNIMPLEMENTED
  return 1;
}

static int gtkTextSetSpinValueAttrib(Ihandle* ih, const char* value)
{
	UNIMPLEMENTED
  return 1;
}

static char* gtkTextGetSpinValueAttrib(Ihandle* ih)
{
	UNIMPLEMENTED
  return NULL;
}


/**********************************************************************************************************/


static int gtkTextMapMethod(Ihandle* ih)
{
  static const BRect defaultRect(0, 0, 10, 10);
  if (ih->data->is_multiline) {
	ih->handle = (InativeHandle*)new BTextView(defaultRect, "multiline",
	  defaultRect, B_FOLLOW_ALL_SIDES, B_WILL_DRAW);
  } else if (iupAttribGetBoolean(ih, "SPIN")) {
	UNIMPLEMENTED
	// TODO build an actual spinner control !
	ih->handle = (InativeHandle*)new BTextControl(defaultRect, "text",
	  NULL, NULL, NULL);
  } else {
	ih->handle = (InativeHandle*)new BTextControl(defaultRect, "text",
	  NULL, NULL, NULL);
    if (iupAttribGetBoolean(ih, "PASSWORD"))
	  UNIMPLEMENTED
  }

  iuphaikuBaseAddToParent(ih);

  if (!iupAttribGetBoolean(ih, "CANFOCUS"))
	UNIMPLEMENTED
  
  return IUP_NOERROR;
}

void iupdrvTextInitClass(Iclass* ic)
{
  /* Driver Dependent Class functions */
  ic->Map = gtkTextMapMethod;

  /* Driver Dependent Attribute functions */

  /* Common GTK only (when text is in a secondary element) */
  iupClassRegisterAttribute(ic, "WIDGETPANGOLAYOUT", gtkTextGetWidgetPangoLayoutAttrib, NULL, NULL, NULL, IUPAF_NO_INHERIT);

  /* Visual */
  iupClassRegisterAttribute(ic, "BGCOLOR", NULL, gtkTextSetBgColorAttrib, IUPAF_SAMEASSYSTEM, "TXTBGCOLOR", IUPAF_DEFAULT);

  /* Special */
  iupClassRegisterAttribute(ic, "FGCOLOR", NULL, iupdrvBaseSetFgColorAttrib, IUPAF_SAMEASSYSTEM, "TXTFGCOLOR", IUPAF_DEFAULT);

  /* IupText only */
  iupClassRegisterAttribute(ic, "PADDING", iupTextGetPaddingAttrib, gtkTextSetPaddingAttrib, IUPAF_SAMEASSYSTEM, "0x0", IUPAF_NOT_MAPPED);
  iupClassRegisterAttribute(ic, "VALUE", gtkTextGetValueAttrib, gtkTextSetValueAttrib, NULL, NULL, IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "LINEVALUE", gtkTextGetLineValueAttrib, NULL, NULL, NULL, IUPAF_READONLY|IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SELECTEDTEXT", gtkTextGetSelectedTextAttrib, gtkTextSetSelectedTextAttrib, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SELECTION", gtkTextGetSelectionAttrib, gtkTextSetSelectionAttrib, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SELECTIONPOS", gtkTextGetSelectionPosAttrib, gtkTextSetSelectionPosAttrib, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "CARET", gtkTextGetCaretAttrib, gtkTextSetCaretAttrib, NULL, NULL, IUPAF_NO_SAVE|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "CARETPOS", gtkTextGetCaretPosAttrib, gtkTextSetCaretPosAttrib, IUPAF_SAMEASSYSTEM, "0", IUPAF_NO_SAVE|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "INSERT", NULL, gtkTextSetInsertAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "APPEND", NULL, gtkTextSetAppendAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "READONLY", gtkTextGetReadOnlyAttrib, gtkTextSetReadOnlyAttrib, NULL, NULL, IUPAF_DEFAULT);
  iupClassRegisterAttribute(ic, "NC", iupTextGetNCAttrib, gtkTextSetNCAttrib, IUPAF_SAMEASSYSTEM, "0", IUPAF_NOT_MAPPED);
  iupClassRegisterAttribute(ic, "CLIPBOARD", NULL, gtkTextSetClipboardAttrib, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SCROLLTO", NULL, gtkTextSetScrollToAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SCROLLTOPOS", NULL, gtkTextSetScrollToPosAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SPINMIN", NULL, gtkTextSetSpinMinAttrib, IUPAF_SAMEASSYSTEM, "0", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SPINMAX", NULL, gtkTextSetSpinMaxAttrib, IUPAF_SAMEASSYSTEM, "100", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SPININC", NULL, gtkTextSetSpinIncAttrib, IUPAF_SAMEASSYSTEM, "1", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SPINVALUE", gtkTextGetSpinValueAttrib, gtkTextSetSpinValueAttrib, IUPAF_SAMEASSYSTEM, "0", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "COUNT", gtkTextGetCountAttrib, NULL, NULL, NULL, IUPAF_READONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "LINECOUNT", gtkTextGetLineCountAttrib, NULL, NULL, NULL, IUPAF_READONLY|IUPAF_NO_INHERIT);

  /* IupText Windows and GTK only */
  iupClassRegisterAttribute(ic, "ADDFORMATTAG", NULL, iupTextSetAddFormatTagAttrib, NULL, NULL, IUPAF_IHANDLENAME|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "ADDFORMATTAG_HANDLE", NULL, iupTextSetAddFormatTagHandleAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "ALIGNMENT", NULL, gtkTextSetAlignmentAttrib, IUPAF_SAMEASSYSTEM, "ALEFT", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "FORMATTING", iupTextGetFormattingAttrib, iupTextSetFormattingAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "OVERWRITE", gtkTextGetOverwriteAttrib, gtkTextSetOverwriteAttrib, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "REMOVEFORMATTING", NULL, gtkTextSetRemoveFormattingAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "TABSIZE", NULL, gtkTextSetTabSizeAttrib, "8", NULL, IUPAF_DEFAULT);  /* force new default value */
  iupClassRegisterAttribute(ic, "PASSWORD", NULL, NULL, NULL, NULL, IUPAF_NO_INHERIT);

  /* Not Supported */
  iupClassRegisterAttribute(ic, "CUEBANNER", NULL, NULL, NULL, NULL, IUPAF_NOT_SUPPORTED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "FILTER", NULL, NULL, NULL, NULL, IUPAF_NOT_SUPPORTED|IUPAF_NO_INHERIT);
}
