/** \file
 * \brief List Control
 *
 * See Copyright Notice in "iup.h"
 */

#include <Cocoa/Cocoa.h>
#import <objc/runtime.h>


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

#include "iupcocoa_drv.h"


// the point of this is we have a unique memory address for an identifier
static const void* IUP_COCOA_LIST_POPUPBUTTON_RECEIVER_OBJ_KEY = "IUP_COCOA_LIST_POPUPBUTTON_RECEIVER_OBJ_KEY";
static const void* IUP_COCOA_LIST_COMBOBOX_RECEIVER_OBJ_KEY = "IUP_COCOA_LIST_COMBOBOX_RECEIVER_OBJ_KEY";
static const void* IUP_COCOA_LIST_TABLEVIEW_RECEIVER_OBJ_KEY = "IUP_COCOA_LIST_TABLEVIEW_RECEIVER_OBJ_KEY";


@interface IupCocoaListPopupButtonReceiver : NSObject
- (IBAction) onSelectionChanged:(id)the_sender;
@end

@implementation IupCocoaListPopupButtonReceiver

/*
- (void) dealloc
{
	[super dealloc];
}
*/


- (IBAction) onSelectionChanged:(id)the_sender;
{
//	Icallback callback_function;
	Ihandle* ih = (Ihandle*)objc_getAssociatedObject(the_sender, IHANDLE_ASSOCIATED_OBJ_KEY);


	if(ih->data->has_editbox)
	{

	}

	if(!ih->data->is_multiple)
	{
		IFnsii cb = (IFnsii)IupGetCallback(ih, "ACTION");
		if(cb)
		{
			NSPopUpButton* popup_button = (NSPopUpButton*)ih->handle;
			NSInteger index_of_selected_item = [popup_button indexOfSelectedItem];
			int adjusted_index = (int)(index_of_selected_item+1); /* IUP starts at 1 */
			iupListSingleCallActionCb(ih, cb, adjusted_index);

		}
	}
	else
	{
		IFns multi_cb = (IFns)IupGetCallback(ih, "MULTISELECT_CB");
		IFnsii cb = (IFnsii) IupGetCallback(ih, "ACTION");
		if (multi_cb || cb)
		{
			/*
			GList *il, *list = gtk_tree_selection_get_selected_rows(selection, NULL);
			int i, sel_count = g_list_length(list);
			int* pos = malloc(sizeof(int)*sel_count);
			for (il=list, i=0; il; il=il->next, i++)
			{
			GtkTreePath* path = (GtkTreePath*)il->data;
			int* indices = gtk_tree_path_get_indices(path);
			pos[i] = indices[0];
			gtk_tree_path_free(path);
			}
			g_list_free(list);

			iupListMultipleCallActionCb(ih, cb, multi_cb, pos, sel_count);
			free(pos);
			 */
		}
	}

	if (!ih->data->has_editbox)
	{
		iupBaseCallValueChangedCb(ih);
	}
	
	
	

}

@end


// We are not using NSComboBoxDataSource
@interface IupCocoaListComboBoxReceiver : NSObject <NSComboBoxDelegate>


// NSComboBoxDelegate
/* Notifications */
//- (void)comboBoxWillPopUp:(NSNotification *)notification;
//- (void)comboBoxWillDismiss:(NSNotification *)notification;
- (void) comboBoxSelectionDidChange:(NSNotification*)the_notification;
//- (void)comboBoxSelectionIsChanging:(NSNotification *)notification;


@end

@implementation IupCocoaListComboBoxReceiver

/*
 - (void) dealloc
 {
	[super dealloc];
 }
 */


- (void) comboBoxSelectionDidChange:(NSNotification*)the_notification
{
	NSComboBox* combo_box = [the_notification object];
	Ihandle* ih = (Ihandle*)objc_getAssociatedObject(combo_box, IHANDLE_ASSOCIATED_OBJ_KEY);
	
	IFnsii cb = (IFnsii)IupGetCallback(ih, "ACTION");
	if(cb)
	{

		
		NSInteger index_of_selected_item = [combo_box indexOfSelectedItem];
		int adjusted_index = (int)(index_of_selected_item+1); /* IUP starts at 1 */
		iupListSingleCallActionCb(ih, cb, adjusted_index);

	}
	

	/*
	if (!ih->data->has_editbox)
	{
		iupBaseCallValueChangedCb(ih);
	}
	
	*/
	
	
}

@end


// We are not using NSComboBoxDataSource
@interface IupCocoaListTableViewReceiver : NSObject <NSTableViewDelegate, NSTableViewDataSource>
{
	NSMutableArray* dataArray;
}

- (NSMutableArray*) dataArray;


// NSTableViewDataSource
- (NSInteger) numberOfRowsInTableView:(NSTableView*)table_view;
/* This method is required for the "Cell Based" TableView, and is optional for the "View Based" TableView. If implemented in the latter case, the value will be set to the view at a given row/column if the view responds to -setObjectValue: (such as NSControl and NSTableCellView).
 */
//- (nullable id) tableView:(NSTableView*)table_view objectValueForTableColumn:(nullable NSTableColumn*)table_column row:(NSInteger)the_row;
- (nullable NSView*) tableView:(NSTableView*)table_view viewForTableColumn:(NSTableColumn*)table_column row:(NSInteger)the_row;


// NSTableViewDelegate

- (void) tableViewSelectionDidChange:(NSNotification*)the_notification;

@end

@implementation IupCocoaListTableViewReceiver

- (instancetype) init
{
	self = [super init];
	if(self)
	{
		dataArray = [[NSMutableArray alloc] init];
	}
	return self;
}

- (void) dealloc
{
	[dataArray release];
	[super dealloc];
}

- (NSMutableArray*) dataArray
{
	return dataArray;
}


- (NSInteger) numberOfRowsInTableView:(NSTableView*)table_view
{
	IupCocoaListTableViewReceiver* list_receiver = objc_getAssociatedObject(table_view, IUP_COCOA_LIST_TABLEVIEW_RECEIVER_OBJ_KEY);
	NSMutableArray* data_array = [list_receiver dataArray];
	
	return [data_array count];
	
}
/*
- (NSTableRowView *)tableView:(NSTableView *)tableView rowViewForRow:(NSInteger)row
{
	NSTextField* the_result = [table_view makeViewWithIdentifier:@"IupCocoaListTableViewCell" owner:self];
	return the_result;
//	return nil;
}
 */

- (nullable NSView*) tableView:(NSTableView*)table_view viewForTableColumn:(NSTableColumn*)table_column row:(NSInteger)the_row
{
	IupCocoaListTableViewReceiver* list_receiver = objc_getAssociatedObject(table_view, IUP_COCOA_LIST_TABLEVIEW_RECEIVER_OBJ_KEY);
	NSMutableArray* data_array = [list_receiver dataArray];
	
	NSString* string_item = [data_array objectAtIndex:the_row];
	
	
	
	    // Get an existing cell with the MyView identifier if it exists
    NSTextField* the_result = [table_view makeViewWithIdentifier:@"IupCocoaListTableViewCell" owner:self];
 
    // There is no existing cell to reuse so create a new one
    if(nil == the_result)
	{
 
         // Create the new NSTextField with a frame of the {0,0} with the width of the table.
         // Note that the height of the frame is not really relevant, because the row height will modify the height.
         the_result = [[NSTextField alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];
		[the_result setBezeled:NO];
		[the_result setDrawsBackground:NO];
		[the_result setEditable:NO];
		//			[the_label setSelectable:NO];
		// TODO: FEATURE: I think this is really convenient for users so it should be the default
		[the_result setSelectable:YES];
		
         // The identifier of the NSTextField instance is set to MyView.
         // This allows the cell to be reused.
         the_result.identifier = @"IupCocoaListTableViewCell";
	}
 
      // result is now guaranteed to be valid, either as a reused cell
      // or as a new cell, so set the stringValue of the cell to the
      // nameArray value at row
      the_result.stringValue = string_item;
 
      // Return the result
      return the_result;
 

	
	
	
}


- (void) tableViewSelectionDidChange:(NSNotification*)the_notification
{
	NSTableView* table_view = [the_notification object];
	Ihandle* ih = (Ihandle*)objc_getAssociatedObject(table_view, IHANDLE_ASSOCIATED_OBJ_KEY);
	
	IFnsii cb = (IFnsii)IupGetCallback(ih, "ACTION");
	if(cb)
	{
		
		
		NSInteger index_of_selected_item = [table_view selectedRow];
		int adjusted_index = (int)(index_of_selected_item+1); /* IUP starts at 1 */
		iupListSingleCallActionCb(ih, cb, adjusted_index);
		
	}
	
	
	/*
	 if (!ih->data->has_editbox)
	 {
		iupBaseCallValueChangedCb(ih);
	 }
	 
	 */
	
	
}

@end


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
  int border_size = 2*10;
  (*x) += border_size;
  (*y) += border_size;

  if (ih->data->is_dropdown)
  {
    (*x) += 5; /* extra space for the dropdown button */

    if (ih->data->has_editbox)
	{
      (*x) += 5; /* another extra space for the dropdown button */
		(*y) += 7; /* extra padding space */
	}
    else
    {
      (*y) += 4; /* extra padding space */
      (*x) += 4; /* extra padding space */
    }
  }
  else
  {
    if (ih->data->is_multiple)
	{
		(*x) += 2*80; /* internal border between editbox and list */

      (*y) += 2*40; /* internal border between editbox and list */
		
		(*x) = 200;
		(*y) = 200;
	}
  }
}

int iupdrvListGetCount(Ihandle* ih)
{
	if(ih->data->is_dropdown)
	{
		if(!ih->data->has_editbox)
		{
			NSPopUpButton* popup_button = (NSPopUpButton*)ih->handle;
			NSInteger number_of_items = [[popup_button menu] numberOfItems];
			return (int)number_of_items;
		}
		else
		{
			NSComboBox* combo_box = (NSComboBox*)ih->handle;
			NSInteger number_of_items = [combo_box numberOfItems];
			return (int)number_of_items;
		}
	}
	else
	{
		if(ih->data->is_multiple)
		{
			NSTableView* table_view = (NSTableView*)ih->handle;
			NSInteger number_of_items = [table_view numberOfRows];
			return (int)number_of_items;
		}
		else
		{
			NSLog(@"iupdrvListGetCount widget subtype not implemented");
		}
	}
	
	return 0;
}

void iupdrvListAppendItem(Ihandle* ih, const char* value)
{
	if(ih->data->is_dropdown)
	{
		if(!ih->data->has_editbox)
		{
			NSPopUpButton* popup_button = (NSPopUpButton*)ih->handle;
			NSString* ns_string = [NSString stringWithUTF8String:value];
			
			[[popup_button menu] addItemWithTitle:ns_string action:NULL keyEquivalent:@""];
		}
		else
		{
			NSComboBox* combo_box = (NSComboBox*)ih->handle;
			NSString* ns_string = [NSString stringWithUTF8String:value];
			[combo_box addItemWithObjectValue:ns_string];
			
		}
	}
	else
	{
		if(ih->data->is_multiple)
		{
			NSTableView* table_view = (NSTableView*)ih->handle;
			IupCocoaListTableViewReceiver* list_receiver = objc_getAssociatedObject(table_view, IUP_COCOA_LIST_TABLEVIEW_RECEIVER_OBJ_KEY);
			NSMutableArray* data_array = [list_receiver dataArray];

			NSString* ns_string = [NSString stringWithUTF8String:value];
			[data_array addObject:ns_string];
			
			[table_view reloadData];
		}
		else
		{
			NSLog(@"iupdrvListAppendItem widget subtype not implemented");
		}
	}


	
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



static char* cocoaListGetIdValueAttrib(Ihandle* ih, int id_value)
{
  int pos = iupListGetPosAttrib(ih, id_value);
  if (pos >= 0)
  {
	  
	  if(ih->data->is_dropdown)
	  {
		  if(!ih->data->has_editbox)
		  {
			  NSPopUpButton* popup_button = (NSPopUpButton*)ih->handle;
			  NSString* ns_string = [popup_button itemTitleAtIndex:pos];
			  
			  const char* c_str = [ns_string UTF8String];
			  // don't use [ns_string length]...counts characters, not bytes
			  size_t str_len = strlen(c_str);
			  
			  char* iup_str = iupStrGetMemory((int)str_len);
			  strlcpy(iup_str, c_str, str_len+1);
			  return iup_str;
		  }
		  else
		  {
			  NSComboBox* popup_button = (NSComboBox*)ih->handle;
			  id object_value = [popup_button itemObjectValueAtIndex:pos];
			  NSString* ns_string = nil;

			  if([object_value isKindOfClass:[NSString class]])
			  {
				  ns_string = (NSString*)object_value;
			  }
			  else
			  {
				  ns_string = [object_value stringValue];

			  }
			  
			  const char* c_str = [ns_string UTF8String];
			  // don't use [ns_string length]...counts characters, not bytes
			  size_t str_len = strlen(c_str);
			  
			  char* iup_str = iupStrGetMemory((int)str_len);
			  strlcpy(iup_str, c_str, str_len+1);
			  return iup_str;
		  }
	  }
	  else
	  {
		  if(ih->data->is_multiple)
		  {
			  NSTableView* table_view = (NSTableView*)ih->handle;
			  IupCocoaListTableViewReceiver* list_receiver = objc_getAssociatedObject(table_view, IUP_COCOA_LIST_TABLEVIEW_RECEIVER_OBJ_KEY);
			  NSMutableArray* data_array = [list_receiver dataArray];
			  id object_value = [data_array objectAtIndex:pos];
			  NSString* ns_string = nil;
			  
			  if([object_value isKindOfClass:[NSString class]])
			  {
				  ns_string = (NSString*)object_value;
			  }
			  else
			  {
				  ns_string = [object_value stringValue];
				  
			  }
			  
			  const char* c_str = [ns_string UTF8String];
			  // don't use [ns_string length]...counts characters, not bytes
			  size_t str_len = strlen(c_str);
			  
			  char* iup_str = iupStrGetMemory((int)str_len);
			  strlcpy(iup_str, c_str, str_len+1);
			  return iup_str;
			  
			  
		  }
		  else
		  {
			  
		  }
	  }
	  

	  
#if 0
    GtkTreeIter iter;
    GtkTreeModel* model = cocoaListGetModel(ih);
    if (gtk_tree_model_iter_nth_child(model, &iter, NULL, pos))
    {
      gchar *text = NULL;
      gtk_tree_model_get(model, &iter, IUPGTK_LIST_TEXT, &text, -1);
      if (text)
      {
        char* ret_str = iupStrReturnStr(iupgtkStrConvertFromSystem(text));
        g_free(text);
        return ret_str;
	  }
    }
#endif
  }
  return NULL;
}


static char* cocoaListGetValueAttrib(Ihandle* ih)
{

	if(ih->data->is_dropdown)
	{
		
		if(!ih->data->has_editbox)
		{
			NSPopUpButton* popup_button = (NSPopUpButton*)ih->handle;
			NSInteger index_of_selected_item = [popup_button indexOfSelectedItem];
			int adjusted_index = (int)(index_of_selected_item+1); /* IUP starts at 1 */
			return iupStrReturnInt(adjusted_index);

			
		}
		else
		{
			NSComboBox* combo_box = (NSComboBox*)ih->handle;
			NSInteger index_of_selected_item = [combo_box indexOfSelectedItem];
			int adjusted_index = (int)(index_of_selected_item+1); /* IUP starts at 1 */
			return iupStrReturnInt(adjusted_index);

		}
	}
	else
	{
		if(ih->data->is_multiple)
		{
			

			
			NSTableView* table_view = (NSTableView*)ih->handle;
			NSInteger index_of_selected_item = [table_view selectedRow];
			int adjusted_index = (int)(index_of_selected_item+1); /* IUP starts at 1 */
			return iupStrReturnInt(adjusted_index);
			
		}
		else
		{
			
			
		}
	}
	
	return NULL;
}

static int cocoaListSetValueAttrib(Ihandle* ih, const char* value)
{
	if (ih->data->has_editbox)
	{
#if 0
		GtkEntry* entry = (GtkEntry*)iupAttribGet(ih, "_IUPGTK_ENTRY");
		if (!value) value = "";
		iupAttribSet(ih, "_IUPGTK_DISABLE_TEXT_CB", "1");
		gtk_entry_set_text(entry, iupgtkStrConvertToSystem(value));
		iupAttribSet(ih, "_IUPGTK_DISABLE_TEXT_CB", NULL);
#endif
	}
	else
	{
		if (ih->data->is_dropdown)
		{
#if 0
			int pos;
			GtkTreeModel *model = cocoaListGetModel(ih);
			g_signal_handlers_block_by_func(G_OBJECT(ih->handle), G_CALLBACK(cocoaListComboBoxChanged), ih);
			if (iupStrToInt(value, &pos)==1 &&
				(pos>0 && pos<=gtk_tree_model_iter_n_children(model, NULL)))
			{
				gtk_combo_box_set_active((GtkComboBox*)ih->handle, pos-1);    /* IUP starts at 1 */
				iupAttribSetInt(ih, "_IUPLIST_OLDVALUE", pos);
			}
			else
			{
				gtk_combo_box_set_active((GtkComboBox*)ih->handle, -1);    /* none */
				iupAttribSet(ih, "_IUPLIST_OLDVALUE", NULL);
			}
			g_signal_handlers_unblock_by_func(G_OBJECT(ih->handle), G_CALLBACK(cocoaListComboBoxChanged), ih);
#endif
		}
		else
		{
#if 0
			GtkTreeSelection* selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(ih->handle));
			if (!ih->data->is_multiple)
			{
				int pos;
				g_signal_handlers_block_by_func(G_OBJECT(selection), G_CALLBACK(cocoaListSelectionChanged), ih);
				if (iupStrToInt(value, &pos)==1)
				{
					GtkTreePath* path = gtk_tree_path_new_from_indices(pos-1, -1);   /* IUP starts at 1 */
					gtk_tree_selection_select_path(selection, path);
					gtk_tree_path_free(path);
					iupAttribSetInt(ih, "_IUPLIST_OLDVALUE", pos);
				}
				else
				{
					gtk_tree_selection_unselect_all(selection);
					iupAttribSet(ih, "_IUPLIST_OLDVALUE", NULL);
				}
				g_signal_handlers_unblock_by_func(G_OBJECT(selection), G_CALLBACK(cocoaListSelectionChanged), ih);
			}
			else
			{
				/* User has changed a multiple selection on a simple list. */
				int i, len, count;
				
				g_signal_handlers_block_by_func(G_OBJECT(selection), G_CALLBACK(cocoaListSelectionChanged), ih);
				
				/* Clear all selections */
				gtk_tree_selection_unselect_all(selection);
				
				if (!value)
				{
					iupAttribSet(ih, "_IUPLIST_OLDVALUE", NULL);
					return 0;
				}
				
				len = (int)strlen(value);
				count = iupdrvListGetCount(ih);
				if (len < count)
					count = len;
				
				/* update selection list */
				for (i = 0; i<count; i++)
				{
					if (value[i]=='+')
					{
						GtkTreePath* path = gtk_tree_path_new_from_indices(i, -1);
						gtk_tree_selection_select_path(selection, path);
						gtk_tree_path_free(path);
					}
				}
				iupAttribSetStr(ih, "_IUPLIST_OLDVALUE", value);
				g_signal_handlers_unblock_by_func(G_OBJECT(selection), G_CALLBACK(cocoaListSelectionChanged), ih);
			}
#endif
		}
	}
	
	return 0;
}

static int cocoaListSetShowDropdownAttrib(Ihandle* ih, const char* value)
{
	if (ih->data->is_dropdown)
	{
#if 0
		if (iupStrBoolean(value))
			gtk_combo_box_popup((GtkComboBox*)ih->handle);
		else
			gtk_combo_box_popdown((GtkComboBox*)ih->handle);
#endif
	}
	return 0;
}



static int cocoaListSetInsertAttrib(Ihandle* ih, const char* value)
{
//	gint pos;
//	GtkEntry* entry;
	if (!ih->data->has_editbox)
		return 0;
	if (!value)
		return 0;

#if 0
	iupAttribSet(ih, "_IUPGTK_DISABLE_TEXT_CB", "1");  /* disable callbacks */
	entry = (GtkEntry*)iupAttribGet(ih, "_IUPGTK_ENTRY");
	pos = gtk_editable_get_position(GTK_EDITABLE(entry));
	gtk_editable_insert_text(GTK_EDITABLE(entry), iupgtkStrConvertToSystem(value), -1, &pos);
	iupAttribSet(ih, "_IUPGTK_DISABLE_TEXT_CB", NULL);
#endif
	
	return 0;
}

static int cocoaListSetAppendAttrib(Ihandle* ih, const char* value)
{
#if 0
	if (ih->data->has_editbox)
	{
		GtkEntry* entry = (GtkEntry*)iupAttribGet(ih, "_IUPGTK_ENTRY");
		gint pos = (gint)strlen(gtk_entry_get_text(entry))+1;
		iupAttribSet(ih, "_IUPGTK_DISABLE_TEXT_CB", "1"); /* disable callbacks */
		gtk_editable_insert_text(GTK_EDITABLE(entry), iupgtkStrConvertToSystem(value), -1, &pos);
		iupAttribSet(ih, "_IUPGTK_DISABLE_TEXT_CB", NULL);
	}
#endif
	return 0;
}



static int cocoaListSetSelectionAttrib(Ihandle* ih, const char* value)
{
#if 0
  int start=1, end=1;
  GtkEntry* entry;
  if (!ih->data->has_editbox)
    return 0;
  if (!value)
    return 0;

  entry = (GtkEntry*)iupAttribGet(ih, "_IUPGTK_ENTRY");
  if (!value || iupStrEqualNoCase(value, "NONE"))
  {
    gtk_editable_select_region(GTK_EDITABLE(entry), 0, 0);
    return 0;
  }

  if (iupStrEqualNoCase(value, "ALL"))
  {
    gtk_editable_select_region(GTK_EDITABLE(entry), 0, -1);
    return 0;
  }

  if (iupStrToIntInt(value, &start, &end, ':')!=2) 
    return 0;

  if(start<1 || end<1) 
    return 0;

  start--; /* IUP starts at 1 */
  end--;

  gtk_editable_select_region(GTK_EDITABLE(entry), start, end);
#endif
  return 0;
}

static char* cocoaListGetSelectionAttrib(Ihandle* ih)
{
#if 0
int start, end;
  GtkEntry* entry;
  if (!ih->data->has_editbox)
    return NULL;

  entry = (GtkEntry*)iupAttribGet(ih, "_IUPGTK_ENTRY");
  if (gtk_editable_get_selection_bounds(GTK_EDITABLE(entry), &start, &end))
  {
    start++; /* IUP starts at 1 */
    end++;
    return iupStrReturnIntInt((int)start, (int)end, ':');
  }
#endif
  return NULL;
}



static int cocoaListMapMethod(Ihandle* ih)
{

	/*
	Iup’s "list" has 4 different modes.
	
	On Mac, these will need to be 4 completely different widgets.
	
	Dropdown:			NSPopupButton
	Editbox+Dropdown:	NSComboBox
	Multiple:			NSTableView
	Editbox:			No obvious native widget
	*/

	
	NSView* the_view = nil;
	
	if (ih->data->is_dropdown)
	{
		if (ih->data->has_editbox)
		{
			// NSComboBox height is very specific. This number (30) plus the stuff going on in iupdrvListAddBorders affects the final height.
			NSComboBox* combo_box = [[NSComboBox alloc] initWithFrame:NSMakeRect(0, 0, 122, 30)];
			the_view = combo_box;
			
			// I'm using objc_setAssociatedObject/objc_getAssociatedObject because it allows me to avoid making subclasses just to hold ivars.
			objc_setAssociatedObject(combo_box, IHANDLE_ASSOCIATED_OBJ_KEY, (id)ih, OBJC_ASSOCIATION_ASSIGN);
			// I also need to track the memory of the buttion action receiver.
			// I prefer to keep the Ihandle the actual NSView instead of the receiver because it makes the rest of the implementation easier if the handle is always an NSView (or very small set of things, e.g. NSWindow, NSView, CALayer).
			// So with only one pointer to deal with, this means we need our Toggle to hold a reference to the receiver object.
			// This is generally not good Cocoa as Toggles don't retain their receivers, but this seems like the best option.
			// Be careful of retain cycles.
			IupCocoaListComboBoxReceiver* list_receiver = [[IupCocoaListComboBoxReceiver alloc] init];
			[combo_box setDelegate:list_receiver];
			
			// I *think* is we use RETAIN, the object will be released automatically when the Toggle is freed.
			// However, the fact that this is tricky and I had to look up the rules (not to mention worrying about retain cycles)
			// makes me think I should just explicitly manage the memory so everybody is aware of what's going on.
			objc_setAssociatedObject(combo_box, IUP_COCOA_LIST_COMBOBOX_RECEIVER_OBJ_KEY, (id)list_receiver, OBJC_ASSOCIATION_ASSIGN);
			
		}
		else
		{
			//	NSPopUpButton* popup_button = [[NSPopUpButton alloc] initWithFrame:NSMakeRect(30, 30, 190, 40) pullsDown:NO];
			NSPopUpButton* popup_button = [[NSPopUpButton alloc] initWithFrame:NSMakeRect(110, 110, 333, 32) pullsDown:NO];
			the_view = popup_button;
			
#if 1
			// I'm using objc_setAssociatedObject/objc_getAssociatedObject because it allows me to avoid making subclasses just to hold ivars.
			objc_setAssociatedObject(popup_button, IHANDLE_ASSOCIATED_OBJ_KEY, (id)ih, OBJC_ASSOCIATION_ASSIGN);
			// I also need to track the memory of the buttion action receiver.
			// I prefer to keep the Ihandle the actual NSView instead of the receiver because it makes the rest of the implementation easier if the handle is always an NSView (or very small set of things, e.g. NSWindow, NSView, CALayer).
			// So with only one pointer to deal with, this means we need our Toggle to hold a reference to the receiver object.
			// This is generally not good Cocoa as Toggles don't retain their receivers, but this seems like the best option.
			// Be careful of retain cycles.
			IupCocoaListPopupButtonReceiver* list_receiver = [[IupCocoaListPopupButtonReceiver alloc] init];
			[popup_button setTarget:list_receiver];
			[popup_button setAction:@selector(onSelectionChanged:)];
			// I *think* is we use RETAIN, the object will be released automatically when the Toggle is freed.
			// However, the fact that this is tricky and I had to look up the rules (not to mention worrying about retain cycles)
			// makes me think I should just explicitly manage the memory so everybody is aware of what's going on.
			objc_setAssociatedObject(popup_button, IUP_COCOA_LIST_POPUPBUTTON_RECEIVER_OBJ_KEY, (id)list_receiver, OBJC_ASSOCIATION_ASSIGN);
			
#endif
			
			
		}
	}
	else
	{
		if(ih->data->is_multiple)
		{
			NSTableView* table_view = [[NSTableView alloc] initWithFrame:NSMakeRect(10, 10, 100, 100)];
			the_view = table_view;
			NSTableColumn* first_column = [[NSTableColumn alloc] initWithIdentifier:@"IupList"];
			[table_view addTableColumn:first_column];

			[table_view setHeaderView:nil];
			
			// I'm using objc_setAssociatedObject/objc_getAssociatedObject because it allows me to avoid making subclasses just to hold ivars.
			objc_setAssociatedObject(table_view, IHANDLE_ASSOCIATED_OBJ_KEY, (id)ih, OBJC_ASSOCIATION_ASSIGN);
			// I also need to track the memory of the buttion action receiver.
			// I prefer to keep the Ihandle the actual NSView instead of the receiver because it makes the rest of the implementation easier if the handle is always an NSView (or very small set of things, e.g. NSWindow, NSView, CALayer).
			// So with only one pointer to deal with, this means we need our Toggle to hold a reference to the receiver object.
			// This is generally not good Cocoa as Toggles don't retain their receivers, but this seems like the best option.
			// Be careful of retain cycles.
			IupCocoaListTableViewReceiver* list_receiver = [[IupCocoaListTableViewReceiver alloc] init];
			[table_view setDataSource:list_receiver];
			[table_view setDelegate:list_receiver];
			
			// I *think* is we use RETAIN, the object will be released automatically when the Toggle is freed.
			// However, the fact that this is tricky and I had to look up the rules (not to mention worrying about retain cycles)
			// makes me think I should just explicitly manage the memory so everybody is aware of what's going on.
			objc_setAssociatedObject(table_view, IUP_COCOA_LIST_TABLEVIEW_RECEIVER_OBJ_KEY, (id)list_receiver, OBJC_ASSOCIATION_ASSIGN);
			
			
			NSScrollView* scroll_view = [[NSScrollView alloc] initWithFrame:NSMakeRect(10, 10, 100, 100)];
			[scroll_view setDocumentView:table_view];
			[scroll_view setHasVerticalScroller:YES];
			
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
	

	// All Cocoa views shoud call this to add the new view to the parent view.
	iupCocoaAddToParent(ih);

	

	/* configure for DRAG&DROP */
	if (IupGetCallback(ih, "DROPFILES_CB"))
	{
//		iupAttribSet(ih, "DROPFILESTARGET", "YES");
	}
	
//	IupSetCallback(ih, "_IUP_XY2POS_CB", (Icallback)cocoaListConvertXYToPos);
	
	iupListSetInitialItems(ih);
	
	/* update a mnemonic in a label if necessary */
//	iupgtkUpdateMnemonic(ih);
	
	return IUP_NOERROR;
}


static void cocoaListUnMapMethod(Ihandle* ih)
{
	id the_view = ih->handle;
	id list_receiver = nil;

	if(ih->data->is_dropdown)
	{
		if(!ih->data->has_editbox)
		{
			list_receiver = objc_getAssociatedObject(the_view, IUP_COCOA_LIST_POPUPBUTTON_RECEIVER_OBJ_KEY);
			objc_setAssociatedObject(the_view, IUP_COCOA_LIST_POPUPBUTTON_RECEIVER_OBJ_KEY, nil, OBJC_ASSOCIATION_ASSIGN);

		}
		else
		{
			list_receiver = objc_getAssociatedObject(the_view, IUP_COCOA_LIST_COMBOBOX_RECEIVER_OBJ_KEY);
			objc_setAssociatedObject(the_view, IUP_COCOA_LIST_COMBOBOX_RECEIVER_OBJ_KEY, nil, OBJC_ASSOCIATION_ASSIGN);

		}
	}
	else
	{
		if(ih->data->is_multiple)
		{
			list_receiver = objc_getAssociatedObject(the_view, IUP_COCOA_LIST_TABLEVIEW_RECEIVER_OBJ_KEY);
			objc_setAssociatedObject(the_view, IUP_COCOA_LIST_TABLEVIEW_RECEIVER_OBJ_KEY, nil, OBJC_ASSOCIATION_ASSIGN);

		}
		else
		{
			NSLog(@"Warning: List subtype not implemented");
		}
	}

	[list_receiver release];

	iupCocoaRemoveFromParent(ih);
	[the_view release];
	ih->handle = NULL;
	
}


void iupdrvListInitClass(Iclass* ic)
{
  /* Driver Dependent Class functions */
	ic->Map = cocoaListMapMethod;
	ic->UnMap = cocoaListUnMapMethod;
#if 0

  /* Driver Dependent Attribute functions */

  /* Overwrite Common */
  iupClassRegisterAttribute(ic, "STANDARDFONT", NULL, cocoaListSetStandardFontAttrib, IUPAF_SAMEASSYSTEM, "DEFAULTFONT", IUPAF_NO_SAVE|IUPAF_NOT_MAPPED);

  /* Visual */
  iupClassRegisterAttribute(ic, "BGCOLOR", NULL, cocoaListSetBgColorAttrib, IUPAF_SAMEASSYSTEM, "TXTBGCOLOR", IUPAF_DEFAULT);

  /* Special */
  iupClassRegisterAttribute(ic, "FGCOLOR", NULL, cocoaListSetFgColorAttrib, IUPAF_SAMEASSYSTEM, "TXTFGCOLOR", IUPAF_DEFAULT);
#endif
  /* IupList only */
  iupClassRegisterAttributeId(ic, "IDVALUE", cocoaListGetIdValueAttrib, iupListSetIdValueAttrib, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "VALUE", cocoaListGetValueAttrib, cocoaListSetValueAttrib, NULL, NULL, IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SHOWDROPDOWN", NULL, cocoaListSetShowDropdownAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
#if 0
iupClassRegisterAttribute(ic, "TOPITEM", NULL, cocoaListSetTopItemAttrib, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SPACING", iupListGetSpacingAttrib, cocoaListSetSpacingAttrib, IUPAF_SAMEASSYSTEM, "0", IUPAF_NOT_MAPPED);

  iupClassRegisterAttribute(ic, "PADDING", iupListGetPaddingAttrib, cocoaListSetPaddingAttrib, IUPAF_SAMEASSYSTEM, "0x0", IUPAF_NOT_MAPPED);
  iupClassRegisterAttribute(ic, "SELECTEDTEXT", cocoaListGetSelectedTextAttrib, cocoaListSetSelectedTextAttrib, NULL, NULL, IUPAF_NO_INHERIT);
#endif
	
  iupClassRegisterAttribute(ic, "SELECTION", cocoaListGetSelectionAttrib, cocoaListSetSelectionAttrib, NULL, NULL, IUPAF_NO_INHERIT);
#if 0
  iupClassRegisterAttribute(ic, "SELECTIONPOS", cocoaListGetSelectionPosAttrib, cocoaListSetSelectionPosAttrib, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "CARET", cocoaListGetCaretAttrib, cocoaListSetCaretAttrib, NULL, NULL, IUPAF_NO_SAVE|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "CARETPOS", cocoaListGetCaretPosAttrib, cocoaListSetCaretPosAttrib, IUPAF_SAMEASSYSTEM, "0", IUPAF_NO_SAVE|IUPAF_NO_INHERIT);
#endif
	
  iupClassRegisterAttribute(ic, "INSERT", NULL, cocoaListSetInsertAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "APPEND", NULL, cocoaListSetAppendAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
#if 0
  iupClassRegisterAttribute(ic, "READONLY", cocoaListGetReadOnlyAttrib, cocoaListSetReadOnlyAttrib, NULL, NULL, IUPAF_DEFAULT);
  iupClassRegisterAttribute(ic, "NC", iupListGetNCAttrib, cocoaListSetNCAttrib, NULL, NULL, IUPAF_NOT_MAPPED);
  iupClassRegisterAttribute(ic, "CLIPBOARD", NULL, cocoaListSetClipboardAttrib, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SCROLLTO", NULL, cocoaListSetScrollToAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SCROLLTOPOS", NULL, cocoaListSetScrollToPosAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);

  iupClassRegisterAttributeId(ic, "IMAGE", NULL, cocoaListSetImageAttrib, IUPAF_IHANDLENAME|IUPAF_WRITEONLY|IUPAF_NO_INHERIT);

  /* Not Supported */
  iupClassRegisterAttribute(ic, "VISIBLE_ITEMS", NULL, NULL, IUPAF_SAMEASSYSTEM, "5", IUPAF_NOT_SUPPORTED);
  iupClassRegisterAttribute(ic, "DROPEXPAND", NULL, NULL, IUPAF_SAMEASSYSTEM, "Yes", IUPAF_NOT_SUPPORTED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AUTOREDRAW", NULL, NULL, IUPAF_SAMEASSYSTEM, "Yes", IUPAF_NOT_SUPPORTED|IUPAF_NO_INHERIT);
#endif
}
