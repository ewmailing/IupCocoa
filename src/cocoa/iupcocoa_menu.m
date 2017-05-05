/** \file
 * \brief Menu Resources
 *
 * See Copyright Notice in "iup.h"
 */

#import <Cocoa/Cocoa.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <stdarg.h>

#include "iup.h"
#include "iupcbs.h"

#include "iup_object.h"
#include "iup_childtree.h"
#include "iup_attrib.h"
#include "iup_dialog.h"
#include "iup_str.h"
#include "iup_label.h"
#include "iup_drv.h"
#include "iup_drvfont.h"
#include "iup_image.h"
#include "iup_menu.h"

#include "iupcocoa_drv.h"

/*
For a menu bar:

AppleIcon File Edit Window
               ----
			   Copy
 
 In Cocoa:
 NSMenu (menubar)
 -> addItem: NSMenuItem (for Edit, but not named)
    -> setSubmenu: NSMenu ("Edit", this is the thing that sets the name)
       -> addItem: NSMenuItem (for "Paste")
 
 In IUP:
 IupMenu (menubar)
 -> IupSubmenu (this is the part that gets named "Edit")
    -> IupMenu  (for Edit, but this is not named)
       -> IupItem (for "Paste")
 
 Notice that Submenu is an NSMenuItem. And the naming must be done on the NSMenu attached below it, not on the NSMenuItem itself.
 
*/

typedef struct _ImenuPos
{
  int x, y;
} ImenuPos;



int iupdrvMenuPopup(Ihandle* ih, int x, int y)
{
 
	return IUP_NOERROR;
}

int iupdrvMenuGetMenuBarSize(Ihandle* ih)
{
	CGFloat menu_bar_height = [[[NSApplication sharedApplication] mainMenu] menuBarHeight];
	return iupROUND(menu_bar_height);
}

/*
static void cocoaReleaseMenuClass(Iclass* ic)
{
	// Not sure if I should tear this down. Typically apps just quit and leave all this stuff.
	[NSApp setMainMenu:nil];

}
*/



static int cocoaMenuMapMethod(Ihandle* ih)
{
	if(iupMenuIsMenuBar(ih))
	{
		/* top level menu used for MENU attribute in IupDialog (a menu bar) */
		NSLog(@"cocoaMenuMapMethod iupMenuIsMenuBar %@", ih->parent->handle);
#if 0
		NSMenu* the_menu = [[NSMenu alloc] init];
		ih->handle = the_menu;
		
		[NSApp setMainMenu:the_menu];
		NSLog(@"cocoaMenuMapMethod created NSMenu %@", the_menu);
		NSLog(@"cocoaMenuMapMethod setMainMenu");
#else
		NSMenu* main_menu = [NSApp mainMenu];
		
		ih->handle = main_menu;

		// not sure if I should retain it because I don't know if this is going to ever get released, but probably should to obey normal patterns.
		[main_menu retain];
#endif
		


	}
	else
	{
		if(ih->parent)
		{

			NSLog(@"cocoaMenuMapMethod ih->parent %@", ih->parent->handle);
		/* parent is a submenu, it is created here */


			NSMenuItem* parent_menu = (NSMenuItem*)(ih->parent->handle);
			NSString* parent_menu_title = [parent_menu title];
			
			NSMenu* the_menu = [parent_menu submenu];

			// Try searching for an existing menu by this name and only create is not there.
			if(nil == [parent_menu submenu])
			{
				the_menu = [[NSMenu alloc] init];
				ih->handle = the_menu;
				
				[parent_menu setSubmenu:the_menu];
				// In Cocoa, the name (e.g. "Edit") goes on the NSMenu, not the above NSMenuItem.
				// I earlier set the name on the parent (which isn't visible), and now set it on the correct widget.
				// Not sure if I should unset the title on the NSMenuItem afterwards.
				[the_menu setTitle:parent_menu_title];
				NSLog(@"cocoaMenuMapMethod created NSMenu %@", the_menu);
			}
			else
			{
				// Already exists. Let's try reusing the existing one.
				[the_menu retain];
				ih->handle = the_menu;
				
				NSLog(@"cocoaMenuMapMethod reused NSMenu %@", the_menu);

			}
			
			

			NSLog(@"cocoaMenuMapMethod [parent_menu setSubmenu:the_menu]");
		}
		else
		{
			/* top level menu used for IupPopup */

			NSMenu* the_menu = [[NSMenu alloc] init];
			ih->handle = the_menu;

			NSLog(@"else cocoaMenuMapMethod created NSMenu %@", the_menu);

			
			//iupAttribSet(ih, "_IUPWIN_POPUP_MENU", "1");
		}
	}

	
	

	
	return IUP_NOERROR;
}

static void cocoaMenuUnMapMethod(Ihandle* ih)
{
	NSMenu* the_menu = (NSMenu*)ih->handle;
	// do I need to remove it from the parent???
	ih->handle = NULL;
	[the_menu release];
}

void iupdrvMenuInitClass(Iclass* ic)
{

	
	id app_menu = [[[NSMenu alloc] init] autorelease];
	id app_name = [[NSProcessInfo processInfo] processName];
	id quit_title = [[NSLocalizedString(@"Quit", @"Quit") stringByAppendingString:@" "] stringByAppendingString:app_name];
	id quit_menu_item = [[[NSMenuItem alloc] initWithTitle:quit_title action:@selector(terminate:) keyEquivalent:@"q"] autorelease];
	[app_menu addItem:quit_menu_item];

	id app_menu_category = [[[NSMenuItem alloc] init] autorelease];
	[app_menu_category setSubmenu:app_menu];
	// This is supposed to do nothing. This is a cheat so I can look up this menu item later and try to reuse it.
	[app_menu_category setTitle:@"ApplicationMenu"];
	
	
	
	
	
//	id print_title = [NSLocalizedString(@"Print", @"Print") stringByAppendingString:@"…"];
//	id print_menu_item = [[[NSMenuItem alloc] initWithTitle:print_title action:@selector(print:) keyEquivalent:@"p"] autorelease];
	
	id file_menu = [[[NSMenu alloc] init] autorelease];
	[file_menu setTitle:NSLocalizedString(@"File", @"File")];
	
//	[file_menu addItem:print_menu_item];

	
	
	id file_menu_category = [[[NSMenuItem alloc] init] autorelease];
	[file_menu_category setSubmenu:file_menu];
	// This is supposed to do nothing. This is a cheat so I can look up this menu item later and try to reuse it.
	[file_menu_category setTitle:NSLocalizedString(@"File", @"File")];

	
	
	
	id cut_menu_item = [[[NSMenuItem alloc] initWithTitle:NSLocalizedString(@"Cut", @"Cut") action:@selector(cut:) keyEquivalent:@"x"] autorelease];
	id copy_menu_item = [[[NSMenuItem alloc] initWithTitle:NSLocalizedString(@"Copy", @"Copy") action:@selector(copy:) keyEquivalent:@"c"] autorelease];
	id paste_menu_item = [[[NSMenuItem alloc] initWithTitle:NSLocalizedString(@"Paste", @"Paste") action:@selector(paste:) keyEquivalent:@"v"] autorelease];
	
	id edit_menu = [[[NSMenu alloc] init] autorelease];
	[edit_menu setTitle:NSLocalizedString(@"Edit", @"Edit")];

	[edit_menu addItem:cut_menu_item];
	[edit_menu addItem:copy_menu_item];
	[edit_menu addItem:paste_menu_item];


	id edit_menu_category = [[[NSMenuItem alloc] init] autorelease];
	[edit_menu_category setSubmenu:edit_menu];
	// This is supposed to do nothing. This is a cheat so I can look up this menu item later and try to reuse it.
	[edit_menu_category setTitle:NSLocalizedString(@"Edit", @"Edit")];

	


	
	
	id menu_bar = [[[NSMenu alloc] init] autorelease];
	[NSApp setMainMenu:menu_bar];
	
	[menu_bar addItem:app_menu_category];
	[menu_bar addItem:file_menu_category];
	[menu_bar addItem:edit_menu_category];


	
	
//	ic->Release = cocoaReleaseMenuClass;

	/* Driver Dependent Class functions */
	ic->Map = cocoaMenuMapMethod;
	ic->UnMap = cocoaMenuUnMapMethod;
#if 0

	/* Used by iupdrvMenuGetMenuBarSize */
	iupClassRegisterAttribute(ic, "FONT", NULL, NULL, IUPAF_SAMEASSYSTEM, "DEFAULTFONT", IUPAF_NOT_MAPPED);  /* inherited */
	
	iupClassRegisterAttribute(ic, "BGCOLOR", NULL, iupdrvBaseSetBgColorAttrib, NULL, NULL, IUPAF_DEFAULT);

#endif
}




static int cocoaItemSetTitleAttrib(Ihandle* ih, const char* value)
{
//	char *str;
	
	/* check if the submenu handle was created in winSubmenuAddToParent */
/*
	if (ih->handle == (InativeHandle*)-1)
		return 1;
*/
	NSMenuItem* menu_item = (NSMenuItem*)ih->handle;

	NSString* ns_string = nil;
	if(!value)
	{
		ns_string = @"";
	}
	else
	{
		ns_string = [NSString stringWithUTF8String:value];

	}
	
	// Mnemonic is not actually supported on Mac
	[menu_item setTitleWithMnemonic:ns_string];
	
	
	return 1;
}



static int cocoaItemMapMethod(Ihandle* ih)
{

	NSLog(@"cocoaItemMapMethod");

	
	if(!ih->parent)
	{
		NSLog(@"IUP_ERROR cocoaItemMapMethod !ih->parent");
		return IUP_ERROR;
	}
	
	
	

	
	if (iupMenuIsMenuBar(ih))
	{
		/* top level menu used for MENU attribute in IupDialog (a menu bar) */
		
		NSLog(@"cocoaItemMapMethod iupMenuIsMenuBar %@", ih->parent->handle);
		
	}
	else
	{
		if(ih->parent)
		{
			/* parent is a submenu, it is created here */
			NSLog(@"cocoaItemMapMethod ih->parent %@", ih->parent->handle);
			
		}
		else
		{
			NSLog(@"cocoaItemMapMethod else");
		}
	}
	

	NSMenuItem* menu_item = [[NSMenuItem alloc] init];
	ih->handle = menu_item;
	NSMenu* parent_menu = (NSMenu*)(ih->parent->handle);
	[parent_menu addItem:menu_item];
	
	

	return IUP_NOERROR;
}

static void cocoaItemUnMapMethod(Ihandle* ih)
{
	NSMenuItem* menu_item = (NSMenuItem*)ih->handle;
	// do I need to remove it from the parent???
	ih->handle = NULL;
	[menu_item release];
}


void iupdrvItemInitClass(Iclass* ic)
{
  /* Driver Dependent Class functions */
  ic->Map = cocoaItemMapMethod;
  ic->UnMap = cocoaItemUnMapMethod;
#if 0

	/* Common */
	iupClassRegisterAttribute(ic, "FONT", NULL, iupdrvSetFontAttrib, IUPAF_SAMEASSYSTEM, "DEFAULTFONT", IUPAF_NOT_MAPPED);  /* inherited */
	
	/* Visual */
	iupClassRegisterAttribute(ic, "ACTIVE", iupBaseGetActiveAttrib, iupBaseSetActiveAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_DEFAULT);
	iupClassRegisterAttribute(ic, "BGCOLOR", NULL, iupdrvBaseSetBgColorAttrib, IUPAF_SAMEASSYSTEM, "DLGBGCOLOR", IUPAF_DEFAULT);
	
	/* IupItem only */
	iupClassRegisterAttribute(ic, "VALUE", gtkItemGetValueAttrib, gtkItemSetValueAttrib, NULL, NULL, IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);
#endif
	iupClassRegisterAttribute(ic, "TITLE", NULL, cocoaItemSetTitleAttrib, NULL, NULL, IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);
#if 0
	iupClassRegisterAttribute(ic, "TITLEIMAGE", NULL, gtkItemSetTitleImageAttrib, NULL, NULL, IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);
	iupClassRegisterAttribute(ic, "IMAGE", NULL, gtkItemSetImageAttrib, NULL, NULL, IUPAF_IHANDLENAME|IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);
	iupClassRegisterAttribute(ic, "IMPRESS", NULL, gtkItemSetImpressAttrib, NULL, NULL, IUPAF_IHANDLENAME|IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);
	



  /* IupItem GTK and Motif only */
  iupClassRegisterAttribute(ic, "HIDEMARK", NULL, NULL, NULL, NULL, IUPAF_NOT_MAPPED);
#endif
}


static int cocoaSubmenuSetTitleAttrib(Ihandle* ih, const char* value)
{
	//	char *str;
	
	/* check if the submenu handle was created in winSubmenuAddToParent */
	/*
	 if (ih->handle == (InativeHandle*)-1)
		return 1;
	 */
	
#if 0
	NSMenu* the_menu = (NSMenu*)ih->handle;
	
	NSString* ns_string = nil;
	if(!value)
	{
		ns_string = @"";
	}
	else
	{
		ns_string = [NSString stringWithUTF8String:value];
		
	}
	
	[the_menu setTitle:ns_string];
#else

	
	
	NSMenuItem* menu_item = (NSMenuItem*)ih->handle;
	
	
	NSString* ns_string = nil;
	if(!value)
	{
		ns_string = @"";
	}
	else
	{
		ns_string = [NSString stringWithUTF8String:value];
		
	}
	
	[menu_item setTitle:ns_string];
#endif
	
	return 1;
}


static int cocoaSubmenuMapMethod(Ihandle* ih)
{
	if(!ih->parent)
	{
		NSLog(@"IUP_ERROR cocoaSubmenuMapMethod !ih->parent");
		return IUP_ERROR;
	}
	
	
	if (iupMenuIsMenuBar(ih->parent))
	{
		/* top level menu used for MENU attribute in IupDialog (a menu bar) */
		
		NSLog(@"cocoaSubmenuMapMethod iupMenuIsMenuBar %@", ih->parent->handle);
		
	}
	else
	{
		if(ih->parent)
		{
			/* parent is a submenu, it is created here */
			NSLog(@"cocoaSubmenuMapMethod ih->parent %@", ih->parent->handle);
			
		}
		else
		{
			NSLog(@"cocoaSubmenuMapMethod else");
		}
	}
	
	
	
	
	NSObject* parent_object = (NSObject*)ih->parent->handle;
	if([parent_object isKindOfClass:[NSMenuItem class]])
	{
		/* parent is a submenu, it is created here */
		NSMenu* the_menu = [[NSMenu alloc] init];
		ih->handle = the_menu;
		
		NSMenuItem* parent_menu = (NSMenuItem*)(ih->parent->handle);
		[parent_menu setSubmenu:the_menu];
		
		
		NSLog(@"cocoaSubmenuMapMethod iupMenuIsMenuBar %@", ih->parent->handle);
		NSLog(@"cocoaSubmenuMapMethod created NSMenu %@", the_menu);
		NSLog(@"[parent_menu setSubmenu:the_menu]");

		
		
	}
	else if([parent_object isKindOfClass:[NSMenu class]])
	{
		

		
#if 0
		NSMenu* the_menu = [[NSMenu alloc] init];
		ih->handle = the_menu;
		
			NSMenu* parent_menu = (NSMenu*)(ih->parent->handle);
			NSMenuItem* replacement_parent_menu_item = [[NSMenuItem alloc] initWithTitle:[parent_menu title] action:nil keyEquivalent:@""];
			[parent_menu release];
			ih->parent->handle = replacement_parent_menu_item;
#else
		
		NSMenu* parent_menu = (NSMenu*)(ih->parent->handle);
		NSArray* list_of_menu_items = [parent_menu itemArray];
		NSInteger number_of_items = [parent_menu numberOfItems];
		NSMenuItem* found_menu_item = nil;
		
		const char* c_title = IupGetAttribute(ih, "TITLE");
		NSString* ns_string = nil;
		if(!c_title)
		{
			ns_string = @"";
		}
		else
		{
			ns_string = [NSString stringWithUTF8String:c_title];
			
		}
		
		for(NSMenuItem* menu_item in list_of_menu_items)
		{
			NSString* menu_item_title = [menu_item title];
			if([menu_item_title isEqualToString:ns_string])
			{
				found_menu_item = menu_item;
				break;
			}
		}
		
		if(found_menu_item)
		{
			NSLog(@"found menu item for Submenu");
			ih->handle = found_menu_item;
			[found_menu_item retain];
			
		}
		else
		{
			//		NSMenuItem* menu_item_for_submenu = [[NSMenuItem alloc] initWithTitle:[parent_menu title] action:nil keyEquivalent:@""];
			NSMenuItem* menu_item_for_submenu = [[NSMenuItem alloc] initWithTitle:@"" action:nil keyEquivalent:@""];
			[parent_menu addItem:menu_item_for_submenu];
	
			ih->handle = menu_item_for_submenu;
			[menu_item_for_submenu setTitle:ns_string];

			NSLog(@"cocoaSubmenuMapMethod parent_menu %@", parent_menu);
			NSLog(@"cocoaSubmenuMapMethod replacement_parent_menu_item %@", menu_item_for_submenu);
			NSLog(@"[parent_menu setSubmenu:the_menu]");
		
		
		}
		//[replacement_parent_menu_item setSubmenu:the_menu];
		
		
#endif
		
		
		
		NSLog(@"NSMenu swap");
		//NSCAssert(0==1, @"NSMenu");
		
		
//		return iupBaseTypeVoidMapMethod(ih);

		
		return IUP_NOERROR;
	}
	else
	{
		NSLog(@"What menu thing is this?");
		NSCAssert(0==1, @"What is this?");
		return IUP_ERROR;

	}

	
	return IUP_NOERROR;
}

static void cocoaSubmenuUnMapMethod(Ihandle* ih)
{
	NSMenuItem* menu_item = (NSMenuItem*)ih->handle;
	// do I need to remove it from the parent???
	ih->handle = NULL;
	[menu_item release];
}



void iupdrvSubmenuInitClass(Iclass* ic)
{
  /* Driver Dependent Class functions */
  ic->Map = cocoaSubmenuMapMethod;
  ic->UnMap = cocoaSubmenuUnMapMethod;
#if 0

	/* Common */
	iupClassRegisterAttribute(ic, "FONT", NULL, iupdrvSetFontAttrib, IUPAF_SAMEASSYSTEM, "DEFAULTFONT", IUPAF_NOT_MAPPED);  /* inherited */
	
	/* Visual */
	iupClassRegisterAttribute(ic, "ACTIVE", iupBaseGetActiveAttrib, iupBaseSetActiveAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_DEFAULT);
	iupClassRegisterAttribute(ic, "BGCOLOR", NULL, iupdrvBaseSetBgColorAttrib, IUPAF_SAMEASSYSTEM, "DLGBGCOLOR", IUPAF_DEFAULT);
	
	/* IupSubmenu only */
#endif
	iupClassRegisterAttribute(ic, "TITLE", NULL, cocoaSubmenuSetTitleAttrib, NULL, NULL, IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);
#if 0
	iupClassRegisterAttribute(ic, "IMAGE", NULL, gtkSubmenuSetImageAttrib, NULL, NULL, IUPAF_IHANDLENAME|IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);
#endif
}


static int cocoaSeparatorMapMethod(Ihandle* ih)
{
#if 0
	int pos;
	
	if (!ih->parent)
		return IUP_ERROR;
	
	ih->handle = gtk_separator_menu_item_new();
	if (!ih->handle)
		return IUP_ERROR;
	
	ih->serial = iupMenuGetChildId(ih);
	
	pos = IupGetChildPos(ih->parent, ih);
	gtk_menu_shell_insert((GtkMenuShell*)ih->parent->handle, ih->handle, pos);
	gtk_widget_show(ih->handle);
#endif
	return IUP_NOERROR;
}

void iupdrvSeparatorInitClass(Iclass* ic)
{
#if 1
  /* Driver Dependent Class functions */
  ic->Map = cocoaSeparatorMapMethod;
  ic->UnMap = iupdrvBaseUnMapMethod;
#endif
	
}
