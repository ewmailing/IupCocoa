/** \file
* \brief Tabs Control
*
* See Copyright Notice in "iup.h"
*/

#define UNIMPLEMENTED printf("%s (%s %d) UNIMPLEMENTED\n",__func__,__FILE__,__LINE__);

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <memory.h>
#include <stdarg.h>

#include <TabView.h>

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

#include "iuphaiku_drv.h"


int iupdrvTabsExtraDecor(Ihandle* ih)
{
  (void)ih;
  return 0;
}

int iupdrvTabsGetLineCountAttrib(Ihandle* ih)
{
  (void)ih;
  return 1;
}

void iupdrvTabsSetCurrentTab(Ihandle* ih, int pos)
{
	UNIMPLEMENTED
}

int iupdrvTabsGetCurrentTab(Ihandle* ih)
{
  BTabView* tv = (BTabView*)ih->handle;
  return tv->Selection();
}

static void gtkTabsUpdatePageFont(Ihandle* ih)
{
	UNIMPLEMENTED
}

static void gtkTabsUpdatePageBgColor(Ihandle* ih, unsigned char r, unsigned char g, unsigned char b)
{
	UNIMPLEMENTED
}

static void gtkTabsUpdatePageFgColor(Ihandle* ih, unsigned char r, unsigned char g, unsigned char b)
{
	UNIMPLEMENTED
}

static void gtkTabsUpdatePagePadding(Ihandle* ih)
{
	UNIMPLEMENTED
}

/* ------------------------------------------------------------------------- */
/* gtkTabs - Sets and Gets accessors                                         */
/* ------------------------------------------------------------------------- */

  
static int gtkTabsSetPaddingAttrib(Ihandle* ih, const char* value)
{
  iupStrToIntInt(value, &ih->data->horiz_padding, &ih->data->vert_padding, 'x');

  if (ih->handle)
  {
    gtkTabsUpdatePagePadding(ih);
    return 0;
  }
  else
    return 1; /* store until not mapped, when mapped will be set again */
}

static void gtkTabsUpdateTabType(Ihandle* ih)
{
	UNIMPLEMENTED
}

static int gtkTabsSetTabTypeAttrib(Ihandle* ih, const char* value)
{
  if(iupStrEqualNoCase(value, "BOTTOM"))
    ih->data->type = ITABS_BOTTOM;
  else if(iupStrEqualNoCase(value, "LEFT"))
    ih->data->type = ITABS_LEFT;
  else if(iupStrEqualNoCase(value, "RIGHT"))
    ih->data->type = ITABS_RIGHT;
  else /* "TOP" */
    ih->data->type = ITABS_TOP;

  if (ih->handle)
    gtkTabsUpdateTabType(ih);  /* for this to work must be updated in map */

  return 0;
}

static int gtkTabsSetTabOrientationAttrib(Ihandle* ih, const char* value)
{
  if (ih->handle) /* allow to set only before mapping */
    return 0;

  if(iupStrEqualNoCase(value, "VERTICAL"))
    ih->data->orientation = ITABS_VERTICAL;
  else  /* HORIZONTAL */
    ih->data->orientation = ITABS_HORIZONTAL;

  return 0;
}

static int haikuTabsSetTabTitleAttrib(Ihandle* ih, int pos, const char* value)
{
  if (value)
  {
    BTabView* tv = (BTabView*)ih->handle;
	BTab* tab = tv->TabAt(pos);
	tab->SetLabel(value);
  }
  
  return 1;
}

static int gtkTabsSetTabImageAttrib(Ihandle* ih, int pos, const char* value)
{
	UNIMPLEMENTED
  return 1;
}

static int gtkTabsSetStandardFontAttrib(Ihandle* ih, const char* value)
{
  iupdrvSetStandardFontAttrib(ih, value);
  if (ih->handle)
    gtkTabsUpdatePageFont(ih);
  return 1;
}

static int gtkTabsSetFgColorAttrib(Ihandle* ih, const char* value)
{
  unsigned char r, g, b;
  if (!iupStrToRGB(value, &r, &g, &b))
    return 0;

  iuphaikuBaseSetFgColor(ih->handle, r, g, b);
  gtkTabsUpdatePageFgColor(ih, r, g, b);

  return 1;
}

static int gtkTabsSetBgColorAttrib(Ihandle* ih, const char* value)
{
  unsigned char r, g, b;
  if (!iupStrToRGB(value, &r, &g, &b))
    return 0;

  iuphaikuBaseSetBgColor(ih->handle, r, g, b);
  gtkTabsUpdatePageBgColor(ih, r, g, b);

  return 1;
}


/* ------------------------------------------------------------------------- */
/* gtkTabs - Methods and Init Class                                          */
/* ------------------------------------------------------------------------- */

static void haikuTabsChildAddedMethod(Ihandle* ih, Ihandle* child)
{
  if (IupGetName(child) == NULL)
    iupAttribSetHandleName(child);

  if (ih->handle)
  {
	BTabView* tv = (BTabView*)ih->handle;

    const char *tabtitle, *tabimage;
    int pos;

    pos = IupGetChildPos(ih, child);

	BTab* tab = new BTab((BView*)child->handle);

	BView* view = new BView(
		BRect(0, 0, ih->currentwidth, ih->currentheight),
		"IUPTabHolder", B_FOLLOW_ALL_SIDES, 0);

    tabtitle = iupTabsAttribGetStrId(ih, "TABTITLE", pos);
    if (!tabtitle) 
    {
      tabtitle = iupAttribGet(child, "TABTITLE");
      if (tabtitle)
        iupTabsAttribSetStrId(ih, "TABTITLE", pos, tabtitle);
    }

    tabimage = iupTabsAttribGetStrId(ih, "TABIMAGE", pos);
    if (!tabimage) 
    {
      tabimage = iupAttribGet(child, "TABIMAGE");
      if (tabimage)
        iupTabsAttribSetStrId(ih, "TABIMAGE", pos, tabimage);
    }

    if (!tabtitle && !tabimage)
      tabtitle = "     ";

    if (tabtitle)
    {
	  tab->SetLabel(tabtitle);
    }

    if (tabimage)
    {
		UNIMPLEMENTED
      //GdkPixbuf* pixbuf = iupImageGetImage(tabimage, ih, 0);

      //tab_image = gtk_image_new();

      //if (pixbuf)
      //  gtk_image_set_from_pixbuf((GtkImage*)tab_image, pixbuf);
    }

    if (tabimage && tabtitle)
    {
		UNIMPLEMENTED
      //GtkWidget* box;
	  /*
      if (ih->data->orientation == ITABS_VERTICAL)
        box = gtk_vbox_new(FALSE, 2);
      else
        box = gtk_hbox_new(FALSE, 2);
      gtk_widget_show(box);

      gtk_container_add((GtkContainer*)box, tab_image);
      gtk_container_add((GtkContainer*)box, tab_label);

      gtk_notebook_insert_page((GtkNotebook*)ih->handle, tab_page, box, pos);
      gtk_notebook_set_menu_label_text((GtkNotebook*)ih->handle, tab_page, gtk_label_get_text((GtkLabel*)tab_label));
	  */
    }

	// There is some breakage of the view hierarchy here. Haiku does not use the
	// regular AddChild method for adding tabs to a BTabView, and IUP also doesn't
	// use the regular way of looking for the parent native view. We need to add
	// a top view for te whole tab and tell IUP explicitly that this is the
	// parent view to use for the tab contents.
    iupAttribSet(child, "_IUPTAB_CONTAINER", (char*)view);
    iupAttribSet(child, "_IUPTAB_PAGE", (char*)tab);

    unsigned char r, g, b;
    iupStrToRGB(IupGetAttribute(ih, "BGCOLOR"), &r, &g, &b);
    iuphaikuBaseSetBgColor((InativeHandle*)view, r, g, b);

    //iupAttribSet(ih, "_IUPGTK_IGNORE_CHANGE", NULL);

	tv->AddTab(view, tab);
  }
}

static void gtkTabsChildRemovedMethod(Ihandle* ih, Ihandle* child)
{
	UNIMPLEMENTED
}


class IUPTabView: public BTabView
{
	public:
		IUPTabView(Ihandle* handle)
			: BTabView(BRect(0,0,10,10), "iuptabs")
			, ih(handle)
		{}

		void Select(int32 newtab) {
			int32 oldtab = Selection();

			BTabView::Select(newtab);

			IFnnn cb = (IFnnn)IupGetCallback(ih, "TABCHANGE_CB");
			if (cb) {
				Ihandle* child = IupGetChild(ih, newtab);
				Ihandle* prev_child = IupGetChild(ih, oldtab);
				cb(ih, child, prev_child);
			} else
			{
				IFnii cb2 = (IFnii)IupGetCallback(ih, "TABCHANGEPOS_CB");
				if (cb2)
					cb2(ih, newtab, oldtab);
			}
		}
	private:
		Ihandle* ih;
};


static int haikuTabsMapMethod(Ihandle* ih)
{
  ih->handle = (InativeHandle*)new IUPTabView(ih);
  if (!ih->handle)
    return IUP_ERROR;

  //gtk_notebook_set_scrollable((GtkNotebook*)ih->handle, TRUE);
  //gtk_notebook_popup_enable((GtkNotebook*)ih->handle);

  //gtkTabsUpdateTabType(ih);

  /* add to the parent, all controls must call this. */
  iuphaikuBaseAddToParent(ih);

  /*
  gtk_widget_add_events(ih->handle, GDK_ENTER_NOTIFY_MASK|GDK_LEAVE_NOTIFY_MASK);

  g_signal_connect(G_OBJECT(ih->handle), "enter-notify-event",  G_CALLBACK(iupgtkEnterLeaveEvent), ih);
  g_signal_connect(G_OBJECT(ih->handle), "leave-notify-event",  G_CALLBACK(iupgtkEnterLeaveEvent), ih);
  g_signal_connect(G_OBJECT(ih->handle), "focus-in-event",      G_CALLBACK(iupgtkFocusInOutEvent), ih);
  g_signal_connect(G_OBJECT(ih->handle), "focus-out-event",     G_CALLBACK(iupgtkFocusInOutEvent), ih);
  g_signal_connect(G_OBJECT(ih->handle), "key-press-event",     G_CALLBACK(iupgtkKeyPressEvent),   ih);
  g_signal_connect(G_OBJECT(ih->handle), "show-help",           G_CALLBACK(iupgtkShowHelp),        ih);

  g_signal_connect(G_OBJECT(ih->handle), "switch-page",         G_CALLBACK(gtkTabSwitchPage), ih);

  gtk_widget_realize(ih->handle);
  */

  /* Create pages and tabs */
  if (ih->firstchild)
  {
    Ihandle* child;
    for (child = ih->firstchild; child; child = child->brother)
      haikuTabsChildAddedMethod(ih, child);
  }

  return IUP_NOERROR;
}

void iupdrvTabsInitClass(Iclass* ic)
{
  /* Driver Dependent Class functions */
  ic->Map = haikuTabsMapMethod;
  ic->ChildAdded     = haikuTabsChildAddedMethod;
  ic->ChildRemoved   = gtkTabsChildRemovedMethod;

  /* Driver Dependent Attribute functions */

  /* Common */
  iupClassRegisterAttribute(ic, "STANDARDFONT", NULL, gtkTabsSetStandardFontAttrib,
	IUPAF_SAMEASSYSTEM, "DEFAULTFONT", IUPAF_NO_SAVE|IUPAF_NOT_MAPPED);

  /* Visual */
  iupClassRegisterAttribute(ic, "BGCOLOR", NULL, gtkTabsSetBgColorAttrib,
	IUPAF_SAMEASSYSTEM, "DLGBGCOLOR", IUPAF_DEFAULT);
  iupClassRegisterAttribute(ic, "FGCOLOR", NULL, gtkTabsSetFgColorAttrib,
	IUPAF_SAMEASSYSTEM, "DLGFGCOLOR", IUPAF_DEFAULT);

  /* IupTabs only */
  iupClassRegisterAttribute(ic, "TABTYPE", iupTabsGetTabTypeAttrib,
	gtkTabsSetTabTypeAttrib, IUPAF_SAMEASSYSTEM, "TOP", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "TABORIENTATION", iupTabsGetTabOrientationAttrib,
	gtkTabsSetTabOrientationAttrib, IUPAF_SAMEASSYSTEM, "HORIZONTAL",
	IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "TABTITLE", NULL, haikuTabsSetTabTitleAttrib,
	IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "TABIMAGE", NULL, gtkTabsSetTabImageAttrib,
	IUPAF_IHANDLENAME|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "PADDING", iupTabsGetPaddingAttrib,
	gtkTabsSetPaddingAttrib, IUPAF_SAMEASSYSTEM, "0x0", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);

  /* NOT supported */
  iupClassRegisterAttribute(ic, "MULTILINE", NULL, NULL, NULL, NULL,
	IUPAF_NOT_SUPPORTED|IUPAF_DEFAULT);
}
