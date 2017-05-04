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
	return (int)(menu_bar_height + 0.5);
}


static void cocoaReleaseMenuClass(Iclass* ic)
{
	// Not sure if I should tear this down. Typically apps just quit and leave all this stuff.
	[NSApp setMainMenu:nil];

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
	
	
	
	
	id print_title = [NSLocalizedString(@"Print", @"Print") stringByAppendingString:@"…"];
	id print_menu_item = [[[NSMenuItem alloc] initWithTitle:print_title action:@selector(print:) keyEquivalent:@"p"] autorelease];
	
	id file_menu = [[[NSMenu alloc] init] autorelease];
	[file_menu setTitle:NSLocalizedString(@"File", @"File")];
	
	[file_menu addItem:print_menu_item];

	
	
	id file_menu_category = [[[NSMenuItem alloc] init] autorelease];
	[file_menu_category setSubmenu:file_menu];

	
	
	
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
	
	


	
	
	id menu_bar = [[[NSMenu alloc] init] autorelease];
	[NSApp setMainMenu:menu_bar];
	
	[menu_bar addItem:app_menu_category];
	[menu_bar addItem:file_menu_category];
	[menu_bar addItem:edit_menu_category];


	
	
	ic->Release = cocoaReleaseMenuClass;
#if 0
  /* Driver Dependent Class functions */
  ic->Map = gtkMenuMapMethod;
  ic->UnMap = gtkMenuUnMapMethod;

  /* Used by iupdrvMenuGetMenuBarSize */
  iupClassRegisterAttribute(ic, "STANDARDFONT", NULL, NULL, IUPAF_SAMEASSYSTEM, "DEFAULTFONT", IUPAF_NO_SAVE|IUPAF_DEFAULT);  /* use inheritance to retrieve standard fonts */

  iupClassRegisterAttribute(ic, "BGCOLOR", NULL, iupdrvBaseSetBgColorAttrib, NULL, NULL, IUPAF_DEFAULT);
#endif
}

void iupdrvItemInitClass(Iclass* ic)
{
#if 0
  /* Driver Dependent Class functions */
  ic->Map = gtkItemMapMethod;
  ic->UnMap = iupdrvBaseUnMapMethod;

  /* Common */
  iupClassRegisterAttribute(ic, "STANDARDFONT", NULL, iupdrvSetStandardFontAttrib, IUPAF_SAMEASSYSTEM, "DEFAULTFONT", IUPAF_NO_SAVE|IUPAF_NOT_MAPPED);  /* use inheritance to retrieve standard fonts */

  /* Visual */
  iupClassRegisterAttribute(ic, "ACTIVE", iupBaseGetActiveAttrib, iupBaseSetActiveAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_DEFAULT);
  iupClassRegisterAttribute(ic, "BGCOLOR", NULL, iupdrvBaseSetBgColorAttrib, IUPAF_SAMEASSYSTEM, "DLGBGCOLOR", IUPAF_DEFAULT);

  /* IupItem only */
  iupClassRegisterAttribute(ic, "VALUE", gtkItemGetValueAttrib, gtkItemSetValueAttrib, NULL, NULL, IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "TITLE", NULL, gtkItemSetTitleAttrib, NULL, NULL, IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "TITLEIMAGE", NULL, gtkItemSetTitleImageAttrib, NULL, NULL, IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "IMAGE", NULL, gtkItemSetImageAttrib, NULL, NULL, IUPAF_IHANDLENAME|IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "IMPRESS", NULL, gtkItemSetImpressAttrib, NULL, NULL, IUPAF_IHANDLENAME|IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);

  /* IupItem GTK and Motif only */
  iupClassRegisterAttribute(ic, "HIDEMARK", NULL, NULL, NULL, NULL, IUPAF_NOT_MAPPED);
#endif
}



void iupdrvSubmenuInitClass(Iclass* ic)
{
#if 0
  /* Driver Dependent Class functions */
  ic->Map = gtkSubmenuMapMethod;
  ic->UnMap = iupdrvBaseUnMapMethod;

  /* Common */
  iupClassRegisterAttribute(ic, "STANDARDFONT", NULL, iupdrvSetStandardFontAttrib, IUPAF_SAMEASSYSTEM, "DEFAULTFONT", IUPAF_NO_SAVE|IUPAF_NOT_MAPPED);  /* use inheritance to retrieve standard fonts */

  /* Visual */
  iupClassRegisterAttribute(ic, "ACTIVE", iupBaseGetActiveAttrib, iupBaseSetActiveAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_DEFAULT);
  iupClassRegisterAttribute(ic, "BGCOLOR", NULL, iupdrvBaseSetBgColorAttrib, IUPAF_SAMEASSYSTEM, "DLGBGCOLOR", IUPAF_DEFAULT);

  /* IupSubmenu only */
  iupClassRegisterAttribute(ic, "TITLE", NULL, gtkItemSetTitleAttrib, NULL, NULL, IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "IMAGE", NULL, gtkSubmenuSetImageAttrib, NULL, NULL, IUPAF_IHANDLENAME|IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);
#endif
}



void iupdrvSeparatorInitClass(Iclass* ic)
{
#if 0
  /* Driver Dependent Class functions */
  ic->Map = gtkSeparatorMapMethod;
  ic->UnMap = iupdrvBaseUnMapMethod;
#endif
	
}
