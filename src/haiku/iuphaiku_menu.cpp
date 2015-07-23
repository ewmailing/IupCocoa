/** \file
 * \brief Menu Resources
 *
 * See Copyright Notice in "iup.h"
 */

#define UNIMPLEMENTED printf("%s (%s %d) UNIMPLEMENTED\n",__func__,__FILE__,__LINE__);

#include <MenuBar.h>
#include <MenuItem.h>

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

#include "iuphaiku_drv.h"


typedef struct _ImenuPos
{
  int x, y;
} ImenuPos;

int iupdrvMenuPopup(Ihandle* ih, int x, int y)
{
	UNIMPLEMENTED
  return IUP_NOERROR;
}

int iupdrvMenuGetMenuBarSize(Ihandle* ih)
{
  int ch;
  iupdrvFontGetCharSize(ih, NULL, &ch);
  return 4 + ch + 4;
}

/*******************************************************************************************/


static int haikuMenuMapMethod(Ihandle* ih)
{
  if (iupMenuIsMenuBar(ih))
  {
    /* top level menu used for MENU attribute in IupDialog (a menu bar) */
    ih->handle = (InativeHandle*)new BMenuBar(BRect(0, 0, 10, 10), "IUPMenuBar");
    if (!ih->handle)
      return IUP_ERROR;

    iuphaikuBaseAddToParent(ih);
  }
  else
  {
    if (ih->parent)
    {
	  // Menu was already created with the SubMenu. Just set the handle now.
	  BMenuItem* item = (BMenuItem*)ih->parent->handle;
      ih->handle = (InativeHandle*)(item->Submenu());
      if (!ih->handle)
        return IUP_ERROR;
    }
    else
    {
      /* top level menu used for IupPopup */
	  UNIMPLEMENTED;
    }
  }

  //gtk_widget_add_events(ih->handle, GDK_KEY_PRESS_MASK);
  //g_signal_connect(G_OBJECT(ih->handle), "key-press-event", G_CALLBACK(gtkMenuKeyPressEvent), ih);

  ih->serial = iupMenuGetChildId(ih); 
  //gtk_widget_show(ih->handle);

  return IUP_NOERROR;
}

static void gtkMenuUnMapMethod(Ihandle* ih)
{
  if (iupMenuIsMenuBar(ih))
    ih->parent = NULL;

  iupdrvBaseUnMapMethod(ih);
}

void iupdrvMenuInitClass(Iclass* ic)
{
  /* Driver Dependent Class functions */
  ic->Map = haikuMenuMapMethod;
  ic->UnMap = gtkMenuUnMapMethod;

  /* Used by iupdrvMenuGetMenuBarSize */
  iupClassRegisterAttribute(ic, "STANDARDFONT", NULL, NULL, IUPAF_SAMEASSYSTEM, "DEFAULTFONT", IUPAF_NO_SAVE|IUPAF_DEFAULT);  /* use inheritance to retrieve standard fonts */

  iupClassRegisterAttribute(ic, "BGCOLOR", NULL, iupdrvBaseSetBgColorAttrib, NULL, NULL, IUPAF_DEFAULT);
}


/*******************************************************************************************/

static int gtkItemSetTitleImageAttrib(Ihandle* ih, const char* value)
{
	UNIMPLEMENTED
    return 0;
}

static int gtkItemSetImageAttrib(Ihandle* ih, const char* value)
{
	UNIMPLEMENTED
    return 0;
}

static int gtkItemSetImpressAttrib(Ihandle* ih, const char* value)
{
	UNIMPLEMENTED
    return 0;
}

static int gtkItemSetTitleAttrib(Ihandle* ih, const char* value)
{
  BMenuItem* item = (BMenuItem*)ih->handle;
  item->SetLabel(value);
  return 1;
}

static int gtkItemSetValueAttrib(Ihandle* ih, const char* value)
{
    if (iupAttribGetBoolean(ih->parent, "RADIO"))
      value = "ON";

	BMenuItem* item = (BMenuItem*)ih->handle;
	item->SetMarked(iupStrBoolean(value));

	// TODO handle images items

    return 0;
}

static char* gtkItemGetValueAttrib(Ihandle* ih)
{
	BMenuItem* item = (BMenuItem*)ih->handle;
	return (char*)(item->IsMarked() ? "ON":"OFF");
}

static int gtkItemMapMethod(Ihandle* ih)
{
  if (!ih->parent)
    return IUP_ERROR;

  BMenu* parent = dynamic_cast<BMenu*>((BView*)ih->parent->handle);
  char* title = iupAttribGet(ih, "TITLE");

  BMessage* msg = new BMessage(menuInvoke);
  msg->AddPointer("iHandle", ih);
  BMenuItem* child = new BMenuItem(title, msg);
  ih->handle = (InativeHandle*)child;

  int pos = IupGetChildPos(ih->parent, ih);
  parent->AddItem(child, pos);

  return IUP_NOERROR;
}

void iupdrvItemInitClass(Iclass* ic)
{
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
}


/*******************************************************************************************/


static int gtkSubmenuSetImageAttrib(Ihandle* ih, const char* value)
{
	UNIMPLEMENTED
    return 0;
}

static int haikuSubmenuMapMethod(Ihandle* ih)
{
  if (!ih->parent)
    return IUP_ERROR;

  char* title = iupAttribGet(ih, "TITLE");
  BMenuItem* item = new BMenuItem(new BMenu(title), NULL);
  ih->handle = (InativeHandle*)item;

  BMenu* topmenu = (BMenu*)ih->parent->handle;
  int pos = IupGetChildPos(ih->parent, ih);
  topmenu->AddItem(item, pos);
  
  //iupUpdateStandardFontAttrib(ih);

  return IUP_NOERROR;
}

void iupdrvSubmenuInitClass(Iclass* ic)
{
  /* Driver Dependent Class functions */
  ic->Map = haikuSubmenuMapMethod;
  ic->UnMap = iupdrvBaseUnMapMethod;

  /* Common */
  iupClassRegisterAttribute(ic, "STANDARDFONT", NULL, iupdrvSetStandardFontAttrib, IUPAF_SAMEASSYSTEM, "DEFAULTFONT", IUPAF_NO_SAVE|IUPAF_NOT_MAPPED);  /* use inheritance to retrieve standard fonts */

  /* Visual */
  iupClassRegisterAttribute(ic, "ACTIVE", iupBaseGetActiveAttrib, iupBaseSetActiveAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_DEFAULT);
  iupClassRegisterAttribute(ic, "BGCOLOR", NULL, iupdrvBaseSetBgColorAttrib, IUPAF_SAMEASSYSTEM, "DLGBGCOLOR", IUPAF_DEFAULT);

  /* IupSubmenu only */
  iupClassRegisterAttribute(ic, "TITLE", NULL, gtkItemSetTitleAttrib, NULL, NULL, IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "IMAGE", NULL, gtkSubmenuSetImageAttrib, NULL, NULL, IUPAF_IHANDLENAME|IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);
}


/*******************************************************************************************/


static int gtkSeparatorMapMethod(Ihandle* ih)
{

  if (!ih->parent)
    return IUP_ERROR;

  ih->handle = (InativeHandle*)new BSeparatorItem();

  BMenu* parent = dynamic_cast<BMenu*>((BView*)ih->parent->handle);
  int pos = IupGetChildPos(ih->parent, ih);
  parent->AddItem((BSeparatorItem*)ih->handle, pos);
  
  return IUP_NOERROR;
}

void iupdrvSeparatorInitClass(Iclass* ic)
{
  /* Driver Dependent Class functions */
  ic->Map = gtkSeparatorMapMethod;
  ic->UnMap = iupdrvBaseUnMapMethod;
}
