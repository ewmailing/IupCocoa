/** \file
* \brief Tabs Control
*
* See Copyright Notice in "iup.h"
*/

/*
Uses NSTabViewController which requires 10.10.
*/

#import <Cocoa/Cocoa.h>
#import <objc/runtime.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <memory.h>
#include <stdarg.h>

#include "iup.h"
#include "iupcbs.h"

#include "iup_object.h"
#include "iup_layout.h"
#include "iup_attrib.h"
#include "iup_str.h"
#include "iup_dialog.h"
#include "iup_drv.h"
#include "iup_drvfont.h"
#include "iup_stdcontrols.h"
#include "iup_image.h"
#include "iup_tabs.h"

#include "iupcocoa_drv.h"

static NSTabViewController* cocoaGetTabViewController(Ihandle* ih)
{
	if(ih && ih->handle)
	{
		NSTabViewController* tab_view_controller = (NSTabViewController*)ih->handle;
		NSCAssert([tab_view_controller isKindOfClass:[NSTabViewController class]], @"Expected NSTabViewController");
		return tab_view_controller;
	}
	else
	{
		NSCAssert(1, @"Expected ih->handle");
	}
	return nil;
}

int iupdrvTabsExtraDecor(Ihandle* ih)
{
  (void)ih;
  return 8;
}

int iupdrvTabsGetLineCountAttrib(Ihandle* ih)
{
  (void)ih;
  return 1;
}

void iupdrvTabsSetCurrentTab(Ihandle* ih, int pos)
{
	NSTabViewController* tab_view_controller = cocoaGetTabViewController(ih);
	[tab_view_controller setSelectedTabViewItemIndex:pos];
}

int iupdrvTabsGetCurrentTab(Ihandle* ih)
{
	NSTabViewController* tab_view_controller = cocoaGetTabViewController(ih);
	return (int)[tab_view_controller selectedTabViewItemIndex];
}



int iupdrvTabsIsTabVisible(Ihandle* child, int pos)
{
	return 1;
}


static void cocoaTabsChildAddedMethod(Ihandle* ih, Ihandle* child)
{
  /* make sure it has at least one name */
	if (!iupAttribGetHandleName(child))
	{
		iupAttribSetHandleName(child);
	}
	
	if(ih->handle)
	{
		NSTabViewController* tab_view_controller = cocoaGetTabViewController(ih);
		
		int pos = IupGetChildPos(ih, child);
		
		// It appears I am required to setup a NSViewController for each tab.
		// Since I do not have a nib, I am also required to alloc a NSView for it.
		NSViewController* view_controller = [[[NSViewController alloc] initWithNibName:nil bundle:nil] autorelease];
		NSView* content_view = [[[NSView alloc] initWithFrame:NSZeroRect] autorelease];
		[view_controller setView:content_view];
		NSTabViewItem* tab_item = [NSTabViewItem tabViewItemWithViewController:view_controller];
		
		[tab_view_controller insertTabViewItem:tab_item atIndex:pos];
		//	[tab_view_controller addTabViewItem:tab_item];
		//	NSView* content_view = [tab_item view];

		// IMPORTANT: We are putting the content_view in here and not the ViewController or TabViewItem
		// This is because the rest of IUP "just works" when there is a regular view for the map key.
		// And the system will correctly add, layout, and remove this view.
		iupAttribSet(child, "_IUPTAB_CONTAINER", (char*)content_view);
	}
}

static void cocoaTabsChildRemovedMethod(Ihandle* ih, Ihandle* child, int pos)
{
	if(ih->handle)
	{
		NSTabViewController* tab_view_controller = cocoaGetTabViewController(ih);
		
		// I think this will make Iup switch the current tab to something else so when we remove the tab, both the native & IUP will agree which tab number they are on
		iupTabsCheckCurrentTab(ih, pos, 1);
		iupAttribSet(child, "_IUPTAB_CONTAINER", NULL);
		
		NSArray* array_of_items = [tab_view_controller tabViewItems];
		NSTabViewItem* tab_view_item = [array_of_items objectAtIndex:pos];
		[tab_view_controller removeTabViewItem:tab_view_item];
	}
}

static int cocoaTabsMapMethod(Ihandle* ih)
{
//	NSTabView* tab_control = [[NSTabView alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];
	NSTabViewController* tab_view_controller = [[NSTabViewController alloc] init];


	// I'm using objc_setAssociatedObject/objc_getAssociatedObject because it allows me to avoid making subclasses just to hold ivars.
	objc_setAssociatedObject(tab_view_controller, IHANDLE_ASSOCIATED_OBJ_KEY, (id)ih, OBJC_ASSOCIATION_ASSIGN);
//	objc_setAssociatedObject(tab_control, IHANDLE_ASSOCIATED_OBJ_KEY, (id)ih, OBJC_ASSOCIATION_ASSIGN);
	// I also need to track the memory of the buttion action receiver.
	// I prefer to keep the Ihandle the actual NSView instead of the receiver because it makes the rest of the implementation easier if the handle is always an NSView (or very small set of things, e.g. NSWindow, NSView, CALayer).
	// So with only one pointer to deal with, this means we need our button to hold a reference to the receiver object.
	// This is generally not good Cocoa as buttons don't retain their receivers, but this seems like the best option.
	// Be careful of retain cycles.
//	IupCocoaSliderReceiver* slider_receiver = [[IupCocoaSliderReceiver alloc] init];
//	[the_slider setTarget:slider_receiver];
//	[the_slider setAction:@selector(mySliderDidMove:)];

	// We're going to use OBJC_ASSOCIATION_RETAIN because I do believe it will do the right thing for us.
	// Just be very careful to not have anything in the delegate retain main widget, or we get a retain cycle.
//	objc_setAssociatedObject(the_slider, IUP_COCOA_SLIDER_RECEIVER_OBJ_KEY, (id)slider_receiver, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
//	[slider_receiver release];
	
	

	

//	ih->handle = tab_control;
	ih->handle = tab_view_controller;
	
	// All Cocoa views shoud call this to add the new view to the parent view.
	iupCocoaAddToParent(ih);
	

	
	/* Create pages and tabs */
	if(ih->firstchild)
	{
		Ihandle* child;
		Ihandle* current_child = (Ihandle*)iupAttribGet(ih, "_IUPTABS_VALUE_HANDLE");
		
		for(child = ih->firstchild; child; child = child->brother)
		{
			cocoaTabsChildAddedMethod(ih, child);
		}
		
		if(current_child)
		{
			IupSetAttribute(ih, "VALUE_HANDLE", (char*)current_child);
			
			/* current value is now given by the native system */
			iupAttribSet(ih, "_IUPTABS_VALUE_HANDLE", NULL);
		}
	}
	
	
	
	return IUP_NOERROR;
}

static void cocoaTabsUnMapMethod(Ihandle* ih)
{
	id tab_control = ih->handle;

	// Destroy the context menu ih it exists
	{
		Ihandle* context_menu_ih = (Ihandle*)iupCocoaCommonBaseGetContextMenuAttrib(ih);
		if(NULL != context_menu_ih)
		{
			IupDestroy(context_menu_ih);
		}
		iupCocoaCommonBaseSetContextMenuAttrib(ih, NULL);
	}

	iupCocoaRemoveFromParent(ih);
	[tab_control release];
	ih->handle = NULL;
	
}


void iupdrvTabsInitClass(Iclass* ic)
{

  /* Driver Dependent Class functions */
  ic->Map = cocoaTabsMapMethod;
    ic->UnMap = cocoaTabsUnMapMethod;
  ic->ChildAdded     = cocoaTabsChildAddedMethod;
  ic->ChildRemoved   = cocoaTabsChildRemovedMethod;
#if 0

  /* Driver Dependent Attribute functions */

  iupClassRegisterCallback(ic, "TABCLOSE_CB", "i");

  /* Common */
  iupClassRegisterAttribute(ic, "STANDARDFONT", NULL, gtkTabsSetStandardFontAttrib, IUPAF_SAMEASSYSTEM, "DEFAULTFONT", IUPAF_NO_SAVE|IUPAF_NOT_MAPPED);

  /* Visual */
  iupClassRegisterAttribute(ic, "BGCOLOR", NULL, gtkTabsSetBgColorAttrib, IUPAF_SAMEASSYSTEM, "DLGBGCOLOR", IUPAF_DEFAULT);
  iupClassRegisterAttribute(ic, "FGCOLOR", NULL, gtkTabsSetFgColorAttrib, IUPAF_SAMEASSYSTEM, "DLGFGCOLOR", IUPAF_DEFAULT);

  /* IupTabs only */
  iupClassRegisterAttribute(ic, "TABTYPE", iupTabsGetTabTypeAttrib, gtkTabsSetTabTypeAttrib, IUPAF_SAMEASSYSTEM, "TOP", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "TABORIENTATION", iupTabsGetTabOrientationAttrib, gtkTabsSetTabOrientationAttrib, IUPAF_SAMEASSYSTEM, "HORIZONTAL", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "TABTITLE", iupTabsGetTitleAttrib, gtkTabsSetTabTitleAttrib, IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "TABIMAGE", NULL, gtkTabsSetTabImageAttrib, IUPAF_IHANDLENAME|IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "TABVISIBLE", iupTabsGetTabVisibleAttrib, gtkTabsSetTabVisibleAttrib, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "PADDING", iupTabsGetPaddingAttrib, gtkTabsSetPaddingAttrib, IUPAF_SAMEASSYSTEM, "0x0", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);

  /* NOT supported */
  iupClassRegisterAttribute(ic, "MULTILINE", NULL, NULL, NULL, NULL, IUPAF_NOT_SUPPORTED|IUPAF_DEFAULT);
#endif
	
}

