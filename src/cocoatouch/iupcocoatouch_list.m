/** \file
 * \brief List Control
 *
 * See Copyright Notice in "iup.h"
 */

#include <UIKit/UIKit.h>


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

#include "iupcocoatouch_drv.h"




void iupdrvListAddItemSpace(Ihandle* ih, int *h)
{
  (void)ih;
  /* FROM:
    gtk_tree_view_column_cell_get_size
      height = text_height + 2*focus_line_width;
    gtk_widget_style_get(ih->handle, "focus-line-width", &focus_line_width, NULL);
      returns always 1
  */
  *h += 2;
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
	return 0;
	
}

void iupdrvListAppendItem(Ihandle* ih, const char* value)
{

	
}

void iupdrvListInsertItem(Ihandle* ih, int pos, const char* value)
{

	
}

void iupdrvListRemoveItem(Ihandle* ih, int pos)
{

}

void iupdrvListRemoveAllItems(Ihandle* ih)
{

	
}


void* iupdrvListGetImageHandle(Ihandle* ih, int id)
{

	
    return NULL;

}

int iupdrvListSetImageHandle(Ihandle* ih, int id, void* hImage)
{

	
  return 0;
}



#if 0
static int cocoaListMapMethod(Ihandle* ih)
{

	UIView* the_view;
	NSPopUpButton* popup_button = [[NSPopUpButton alloc] initWithFrame:NSMakeRect(30, 30, 190, 40)];
	the_view = popup_button;
	
	if (ih->data->is_dropdown)
	{
		if (ih->data->has_editbox)
		{

			
		}
		else
		{
//			ih->handle = gtk_combo_box_new_with_model(GTK_TREE_MODEL(store));
		}
		


		
		
		if(ih->data->show_image)
		{

			
		}
		
		if (ih->data->has_editbox)
		{

			
			if (!iupAttribGetBoolean(ih, "CANFOCUS"))
			{
//				iupgtkSetCanFocus(ih->handle, 0);
			}
			

			
		}
		else
		{

			

			
			
			if (!iupAttribGetBoolean(ih, "CANFOCUS"))
			{

				
			}
			else
			{

				
			}
		}
	
	}
	else
	{

		
		
		if (ih->data->has_editbox)
		{

			
//			iupgtkSetCanFocus(ih->handle, 0);  /* focus goes only to the edit box */
			if (!iupAttribGetBoolean(ih, "CANFOCUS"))
			{
//				iupgtkSetCanFocus(entry, 0);
			}

		}
		else
		{
			
			if (!iupAttribGetBoolean(ih, "CANFOCUS"))
			{
//				iupgtkSetCanFocus(ih->handle, 0);
			}
		}
		
		if (ih->data->show_image)
		{

		}
		
		
		if (ih->data->sb)
		{
			if (iupAttribGetBoolean(ih, "AUTOHIDE"))
			{
//				scrollbar_policy = GTK_POLICY_AUTOMATIC;
			}
			else
			{
//				scrollbar_policy = GTK_POLICY_ALWAYS;
			}
		}
		else
		{
//			scrollbar_policy = GTK_POLICY_NEVER;
		}

		
		
		if (!ih->data->has_editbox && ih->data->is_multiple)
		{

			
		}
		else
		{
		}

	}
	
	/* Enable internal drag and drop support */
	if(ih->data->show_dragdrop && !ih->data->is_dropdown && !ih->data->is_multiple)
	{

	}
	
	if (iupAttribGetBoolean(ih, "SORT"))
	{
//		gtk_tree_sortable_set_sort_column_id(GTK_TREE_SORTABLE(store), IUPGTK_LIST_TEXT, GTK_SORT_ASCENDING);
	}
	/* add to the parent, all GTK controls must call this. */
//	iupgtkAddToParent(ih);
	
	
	ih->handle = the_view;
	
#if 0
	// I'm using objc_setAssociatedObject/objc_getAssociatedObject because it allows me to avoid making subclasses just to hold ivars.
	objc_setAssociatedObject(the_toggle, IHANDLE_ASSOCIATED_OBJ_KEY, (id)ih, OBJC_ASSOCIATION_ASSIGN);
	// I also need to track the memory of the buttion action receiver.
	// I prefer to keep the Ihandle the actual NSView instead of the receiver because it makes the rest of the implementation easier if the handle is always an NSView (or very small set of things, e.g. NSWindow, NSView, CALayer).
	// So with only one pointer to deal with, this means we need our Toggle to hold a reference to the receiver object.
	// This is generally not good Cocoa as Toggles don't retain their receivers, but this seems like the best option.
	// Be careful of retain cycles.
	IupCocoaToggleReceiver* toggle_receiver = [[IupCocoaToggleReceiver alloc] init];
	[the_toggle setTarget:toggle_receiver];
	[the_toggle setAction:@selector(myToggleClickAction:)];
	// I *think* is we use RETAIN, the object will be released automatically when the Toggle is freed.
	// However, the fact that this is tricky and I had to look up the rules (not to mention worrying about retain cycles)
	// makes me think I should just explicitly manage the memory so everybody is aware of what's going on.
	objc_setAssociatedObject(the_toggle, IUP_COCOA_TOGGLE_RECEIVER_OBJ_KEY, (id)toggle_receiver, OBJC_ASSOCIATION_ASSIGN);
	
#endif
	// All Cocoa views shoud call this to add the new view to the parent view.
	iupCocoaAddToParent(ih);

	

	/* configure for DRAG&DROP */
	if (IupGetCallback(ih, "DROPFILES_CB"))
	{
//		iupAttribSet(ih, "DROPFILESTARGET", "YES");
	}
	
//	IupSetCallback(ih, "_IUP_XY2POS_CB", (Icallback)gtkListConvertXYToPos);
	
//	iupListSetInitialItems(ih);
	
	/* update a mnemonic in a label if necessary */
//	iupgtkUpdateMnemonic(ih);
	
	return IUP_NOERROR;
}


static void cocoaListUnMapMethod(Ihandle* ih)
{
	id the_view = ih->handle;
	/*
	 id text_receiver = objc_getAssociatedObject(the_view, IUP_COCOA_TOGGLE_RECEIVER_OBJ_KEY);
	 objc_setAssociatedObject(the_view, IUP_COCOA_TOGGLE_RECEIVER_OBJ_KEY, nil, OBJC_ASSOCIATION_ASSIGN);
	 [text_receiver release];
	 */
	[the_view release];
	ih->handle = NULL;
	
}
#endif

void iupdrvListInitClass(Iclass* ic)
{
  /* Driver Dependent Class functions */
//	ic->Map = cocoaListMapMethod;
//	ic->UnMap = cocoaListUnMapMethod;
#if 0

  /* Driver Dependent Attribute functions */

  /* Overwrite Common */
  iupClassRegisterAttribute(ic, "STANDARDFONT", NULL, gtkListSetStandardFontAttrib, IUPAF_SAMEASSYSTEM, "DEFAULTFONT", IUPAF_NO_SAVE|IUPAF_NOT_MAPPED);

  /* Visual */
  iupClassRegisterAttribute(ic, "BGCOLOR", NULL, gtkListSetBgColorAttrib, IUPAF_SAMEASSYSTEM, "TXTBGCOLOR", IUPAF_DEFAULT);

  /* Special */
  iupClassRegisterAttribute(ic, "FGCOLOR", NULL, gtkListSetFgColorAttrib, IUPAF_SAMEASSYSTEM, "TXTFGCOLOR", IUPAF_DEFAULT);

  /* IupList only */
  iupClassRegisterAttributeId(ic, "IDVALUE", gtkListGetIdValueAttrib, iupListSetIdValueAttrib, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "VALUE", gtkListGetValueAttrib, gtkListSetValueAttrib, NULL, NULL, IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);
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
#endif
}
