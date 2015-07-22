/** \file
 * \brief List Control
 *
 * See Copyright Notice in "iup.h"
 */

#define UNIMPLEMENTED printf("%s (%s %d) UNIMPLEMENTED\n",__func__,__FILE__,__LINE__);

#include <ListView.h>
#include <MenuItem.h>
#include <MenuField.h>
#include <ScrollView.h>

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
#include "iup_drv.h"
#include "iup_drvfont.h"
#include "iup_mask.h"
#include "iup_key.h"
#include "iup_image.h"
#include "iup_list.h"

#include "iuphaiku_drv.h"


enum
{
  IUPGTK_LIST_IMAGE,  /* "pixbuf" */
  IUPGTK_LIST_TEXT,  /* "text" */
  IUPGTK_LIST_LAST_DATA  /* used as a count */
};

void iupdrvListAddItemSpace(Ihandle* ih, int *h)
{
  (void)ih;
  *h += 3;
}

void iupdrvListAddBorders(Ihandle* ih, int *x, int *y)
{
  int border_size = 2*5;
  (*x) += border_size;
  (*y) += border_size;

  if (ih->data->is_dropdown)
  {
#ifdef HILDON
    (*x) += 9; /* extra space for the dropdown button */
#else
    (*x) += 5; /* extra space for the dropdown button */
#endif

    if (ih->data->has_editbox)
      (*x) += 5; /* another extra space for the dropdown button */
    else
    {
      (*y) += 4; /* extra padding space */
      (*x) += 4; /* extra padding space */
    }
  }
  else
  {
    if (ih->data->has_editbox)
      (*y) += 2*3; /* internal border between editbox and list */
  }
}

int iupdrvListGetCount(Ihandle* ih)
{
  BListView* listview = (BListView*)ih->handle;
  return listview->CountItems();
}

BListView* iuphaikuGetListView(BView* view)
{
  // TODO maybe it is easier to get ScrollBar->Target ?
  BListView* listview = dynamic_cast<BListView*>(view);
  if(!listview)
  {
	BView* previous = view;
	previous->LockLooper();
	view = view->ChildAt(0);
	previous->UnlockLooper();
	while(view && !listview)
	{
      listview = dynamic_cast<BListView*>(view);
	  view = view->NextSibling();
	}
  }

  return listview;
}

void iupdrvListAppendItem(Ihandle* ih, const char* value)
{
  BView* view = (BView*)ih->handle;

  BMenuField* menu = dynamic_cast<BMenuField*>(view);
  if(menu)
  {
    menu->Menu()->AddItem(new BMenuItem(value, NULL));
	return;
  }

  BListView* listview = iuphaikuGetListView(view);

  if(listview)
  {
    BListItem* item = new BStringItem(value);
	listview->LockLooper();
    listview->AddItem(item);
	listview->UnlockLooper();
  } else {
    fprintf(stderr, "View hierarchy problem\n");
  }

}

void iupdrvListInsertItem(Ihandle* ih, int pos, const char* value)
{
	UNIMPLEMENTED
}

void iupdrvListRemoveItem(Ihandle* ih, int pos)
{
  BView* view = (BView*)ih->handle;

  BMenuField* menu = dynamic_cast<BMenuField*>(view);
  if(menu)
  {
	UNIMPLEMENTED
	return;
  }
  
  BListView* listview = iuphaikuGetListView(view);
  if(listview) {
	BListItem* item = listview->RemoveItem(pos - 1);
	delete item;
  } else {
    fprintf(stderr, "View hierarchy problem\n");
  }
}

void iupdrvListRemoveAllItems(Ihandle* ih)
{
  BView* view = (BView*)ih->handle;

  BMenuField* menu = dynamic_cast<BMenuField*>(view);
  if(menu)
  {
	UNIMPLEMENTED
	return;
  }
  
  BListView* listview = iuphaikuGetListView(view);
  if(listview) {
	while(!listview->IsEmpty()) {
	  BListItem* item = listview->RemoveItem(0L);
	  delete item;
	}
  } else {
    fprintf(stderr, "View hierarchy problem\n");
  }
}


/*********************************************************************************/

/*
static int gtkListSetStandardFontAttrib(Ihandle* ih, const char* value)
{
	UNIMPLEMENTED
  return 1;
}
*/

static char* haikuListGetIdValueAttrib(Ihandle* ih, int id)
{
  BView* view = (BView*)ih->handle;

  BMenuField* menu = dynamic_cast<BMenuField*>(view);
  if(menu)
  {
	UNIMPLEMENTED
	return NULL;
  }

  BListView* listview = iuphaikuGetListView(view);

  if(listview)
  {
	BStringItem* item = (BStringItem*)listview->ItemAt(id - 1);
	return (char*)item->Text();
  } else {
    fprintf(stderr, "View hierarchy problem\n");
  }

  return NULL;
}


static int gtkListSetBgColorAttrib(Ihandle* ih, const char* value)
{
	UNIMPLEMENTED
		return 0;
}

static int gtkListSetFgColorAttrib(Ihandle* ih, const char* value)
{
	UNIMPLEMENTED
  return 1;
}

static char* haikuListGetValueAttrib(Ihandle* ih)
{
  BView* view = (BView*)ih->handle;

  BMenuField* menu = dynamic_cast<BMenuField*>(view);
  if(menu)
  {
	UNIMPLEMENTED
	return NULL;
  }

  BListView* listview = iuphaikuGetListView(view);

  if(listview)
  {
	BStringItem* item = (BStringItem*)
	  listview->ItemAt(listview->CurrentSelection());
	fprintf(stderr, "IEMTXT %s", item->Text());
	return (char*)item->Text();
  } else {
    fprintf(stderr, "View hierarchy problem\n");
  }

  return NULL;
}

static int gtkListSetValueAttrib(Ihandle* ih, const char* value)
{
  if (ih->data->has_editbox)
	UNIMPLEMENTED

  BView* view = (BView*)ih->handle;

  BMenuField* menu = dynamic_cast<BMenuField*>(view);
  if(menu)
  {
	int pos;
    if (iupStrToInt(value, &pos)==1)
	  menu->Menu()->ItemAt(pos - 1)->SetMarked(true);
	else
	  UNIMPLEMENTED
	return 0;
  }

  BListView* listview = iuphaikuGetListView(view);

  if(listview)
  {
    if (!ih->data->is_multiple) {
	  int pos;
      if (iupStrToInt(value, &pos)==1)
	    listview->Select(pos - 1);
	  else
		listview->DeselectAll();
	} else
	  UNIMPLEMENTED
	
  } else {
    fprintf(stderr, "View hierarchy problem\n");
  }

  return 0;
}

static int gtkListSetShowDropdownAttrib(Ihandle* ih, const char* value)
{
	UNIMPLEMENTED
  return 0;
}

static int gtkListSetTopItemAttrib(Ihandle* ih, const char* value)
{
	UNIMPLEMENTED
  return 0;
}

static int gtkListSetSpacingAttrib(Ihandle* ih, const char* value)
{
	UNIMPLEMENTED
    return 1; /* store until not mapped, when mapped will be set again */
}

static int gtkListSetPaddingAttrib(Ihandle* ih, const char* value)
{
  UNIMPLEMENTED
    return 1; /* store until not mapped, when mapped will be set again */
}

static int gtkListSetSelectionAttrib(Ihandle* ih, const char* value)
{
	UNIMPLEMENTED
  return 0;
}

static char* gtkListGetSelectionAttrib(Ihandle* ih)
{
	UNIMPLEMENTED
  return NULL;
}

static int gtkListSetSelectionPosAttrib(Ihandle* ih, const char* value)
{
	UNIMPLEMENTED
  return 0;
}

static char* gtkListGetSelectionPosAttrib(Ihandle* ih)
{
	UNIMPLEMENTED
  return NULL;
}

static int gtkListSetSelectedTextAttrib(Ihandle* ih, const char* value)
{
	UNIMPLEMENTED
  return 0;
}

static char* gtkListGetSelectedTextAttrib(Ihandle* ih)
{
	UNIMPLEMENTED
  return NULL;
}

static int gtkListSetCaretAttrib(Ihandle* ih, const char* value)
{
	UNIMPLEMENTED
  return 0;
}

static char* gtkListGetCaretAttrib(Ihandle* ih)
{
	UNIMPLEMENTED
    return NULL;
}

static int gtkListSetCaretPosAttrib(Ihandle* ih, const char* value)
{
	UNIMPLEMENTED
  return 0;
}

static char* gtkListGetCaretPosAttrib(Ihandle* ih)
{
	UNIMPLEMENTED
    return NULL;
}

static int gtkListSetScrollToAttrib(Ihandle* ih, const char* value)
{
	UNIMPLEMENTED
  return 0;
}

static int gtkListSetScrollToPosAttrib(Ihandle* ih, const char* value)
{
	UNIMPLEMENTED
  return 0;
}

static int gtkListSetInsertAttrib(Ihandle* ih, const char* value)
{
	UNIMPLEMENTED
  return 0;
}

static int gtkListSetAppendAttrib(Ihandle* ih, const char* value)
{
	UNIMPLEMENTED
  return 0;
}

static int gtkListSetNCAttrib(Ihandle* ih, const char* value)
{
	UNIMPLEMENTED
    return 1; /* store until not mapped, when mapped will be set again */
}

static int gtkListSetClipboardAttrib(Ihandle *ih, const char *value)
{
	UNIMPLEMENTED
  return 0;
}

static int gtkListSetReadOnlyAttrib(Ihandle* ih, const char* value)
{
	UNIMPLEMENTED
  return 0;
}

static char* gtkListGetReadOnlyAttrib(Ihandle* ih)
{
	UNIMPLEMENTED
    return "NO";
}

static int gtkListSetImageAttrib(Ihandle* ih, int id, const char* value)
{
	UNIMPLEMENTED
  return 0;
}

void* iupdrvListGetImageHandle(Ihandle* ih, int id)
{
	UNIMPLEMENTED
		return NULL;
}

/*********************************************************************************/

static int beListMapMethod(Ihandle* ih)
{
  if (ih->data->is_dropdown)
  {
	BMenu* menu = new BMenu("");
	menu->SetLabelFromMarked(true);
	ih->handle = (InativeHandle*)new BMenuField(BRect(0,0,10,10), "PopUpMenu", NULL, menu);
  } else {
	BListView* widget = new BListView(BRect(0, 0, 10,10), "ListView",
	  B_SINGLE_SELECTION_LIST, B_FOLLOW_ALL_SIDES);
    ih->handle = (InativeHandle*)widget;

	BMessage* msg;
    msg = new BMessage(listInvoke);
	msg->AddPointer("iHandle", ih);
	widget->SetInvocationMessage(msg);

	msg = new BMessage(listSelect);
	msg->AddPointer("iHandle", ih);
	widget->SetSelectionMessage(msg);

    if (ih->data->sb)
    {
      ih->handle = (InativeHandle*)new BScrollView("scrolllist",
		(BView*)ih->handle, B_FOLLOW_LEFT | B_FOLLOW_TOP, 0, false, true);
    }
  }

  if(ih->data->show_image)
	UNIMPLEMENTED

  if (ih->data->has_editbox)
	UNIMPLEMENTED

  iuphaikuBaseAddToParent(ih);

  iupListSetInitialItems(ih);
  return IUP_NOERROR;
}

void iupdrvListInitClass(Iclass* ic)
{
  /* Driver Dependent Class functions */
  ic->Map = beListMapMethod;

  /* Driver Dependent Attribute functions */

  /* Overwrite Common */
// TODO does the common code actually works for lists ? We may need to iterate
// over the stringitems and set the font for each of them ?
//  iupClassRegisterAttribute(ic, "STANDARDFONT", NULL, gtkListSetStandardFontAttrib, IUPAF_SAMEASSYSTEM, "DEFAULTFONT", IUPAF_NO_SAVE|IUPAF_NOT_MAPPED);

  /* Visual */
  iupClassRegisterAttribute(ic, "BGCOLOR", NULL, gtkListSetBgColorAttrib, IUPAF_SAMEASSYSTEM, "TXTBGCOLOR", IUPAF_DEFAULT);

  /* Special */
  iupClassRegisterAttribute(ic, "FGCOLOR", NULL, gtkListSetFgColorAttrib, IUPAF_SAMEASSYSTEM, "TXTFGCOLOR", IUPAF_DEFAULT);

  /* IupList only */
  iupClassRegisterAttributeId(ic, "IDVALUE", haikuListGetIdValueAttrib, iupListSetIdValueAttrib, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "VALUE", haikuListGetValueAttrib, gtkListSetValueAttrib, NULL, NULL, IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SHOWDROPDOWN", NULL, gtkListSetShowDropdownAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "TOPITEM", NULL, gtkListSetTopItemAttrib, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SPACING", iupListGetSpacingAttrib, gtkListSetSpacingAttrib, IUPAF_SAMEASSYSTEM, "0", IUPAF_NOT_MAPPED);

  iupClassRegisterAttribute(ic, "PADDING", iupListGetPaddingAttrib, gtkListSetPaddingAttrib, IUPAF_SAMEASSYSTEM, "0x0", IUPAF_NOT_MAPPED);
  iupClassRegisterAttribute(ic, "SELECTEDTEXT", gtkListGetSelectedTextAttrib, gtkListSetSelectedTextAttrib, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SELECTION", gtkListGetSelectionAttrib, gtkListSetSelectionAttrib, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SELECTIONPOS", gtkListGetSelectionPosAttrib, gtkListSetSelectionPosAttrib, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "CARET", gtkListGetCaretAttrib, gtkListSetCaretAttrib, NULL, NULL, IUPAF_NO_SAVE|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "CARETPOS", gtkListGetCaretPosAttrib, gtkListSetCaretPosAttrib, IUPAF_SAMEASSYSTEM, "0", IUPAF_NO_SAVE|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "INSERT", NULL, gtkListSetInsertAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "APPEND", NULL, gtkListSetAppendAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "READONLY", gtkListGetReadOnlyAttrib, gtkListSetReadOnlyAttrib, NULL, NULL, IUPAF_DEFAULT);
  iupClassRegisterAttribute(ic, "NC", iupListGetNCAttrib, gtkListSetNCAttrib, NULL, NULL, IUPAF_NOT_MAPPED);
  iupClassRegisterAttribute(ic, "CLIPBOARD", NULL, gtkListSetClipboardAttrib, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SCROLLTO", NULL, gtkListSetScrollToAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SCROLLTOPOS", NULL, gtkListSetScrollToPosAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);

  iupClassRegisterAttributeId(ic, "IMAGE", NULL, gtkListSetImageAttrib, IUPAF_IHANDLENAME|IUPAF_WRITEONLY|IUPAF_NO_INHERIT);

  /* Not Supported */
  iupClassRegisterAttribute(ic, "VISIBLE_ITEMS", NULL, NULL, IUPAF_SAMEASSYSTEM, "5", IUPAF_NOT_SUPPORTED);
  iupClassRegisterAttribute(ic, "DROPEXPAND", NULL, NULL, IUPAF_SAMEASSYSTEM, "Yes", IUPAF_NOT_SUPPORTED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AUTOREDRAW", NULL, NULL, IUPAF_SAMEASSYSTEM, "Yes", IUPAF_NOT_SUPPORTED|IUPAF_NO_INHERIT);
}

// Event handlers

void iuphaikuListInvoked(Ihandle* ih, int item)
{
  IFnis cb = (IFnis) IupGetCallback(ih, "DBLCLICK_CB");
  if (cb)
  {
    iupListSingleCallDblClickCallback(ih, cb, item+1);  /* IUP starts at 1 */
  }
}

void iuphaikuListSelected(Ihandle* ih, int item)
{
  if (ih->data->has_editbox)
  {
    /* must manually update its contents */
	UNIMPLEMENTED
  }

  if (!ih->data->is_multiple)
  {
    IFnsii cb = (IFnsii) IupGetCallback(ih, "ACTION");
    if (cb)
    {
      iupListSingleCallActionCallback(ih, cb, item+1);  /* IUP starts at 1 */
    }
  } else {
    IFns multi_cb = (IFns)IupGetCallback(ih, "MULTISELECT_CB");
    IFnsii cb = (IFnsii) IupGetCallback(ih, "ACTION");
    if (multi_cb || cb)
    {
	  UNIMPLEMENTED
	}
  }
}
