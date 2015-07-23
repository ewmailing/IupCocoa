/** \file
 * \brief IupDialog class
 *
 * See Copyright Notice in "iup.h"
 */

#include <Rect.h>
#include <Screen.h>
#include <Window.h>
                                         
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <stdarg.h>
#include <limits.h>
#include <time.h>

#include "iup.h"
#include "iupcbs.h"

#include "iup_class.h"
#include "iup_object.h"
#include "iup_layout.h"
#include "iup_dlglist.h"
#include "iup_attrib.h"
#include "iup_drv.h"
#include "iup_drvfont.h"
#include "iup_drvinfo.h"
#include "iup_focus.h"
#include "iup_str.h"
#define _IUPDLG_PRIVATE
#include "iup_dialog.h"
#include "iup_image.h"

#include "iuphaiku_drv.h"

#define UNIMPLEMENTED printf("%s (%s %d) UNIMPLEMENTED\n",__func__,__FILE__,__LINE__);

/****************************************************************
                     Utilities
****************************************************************/

int iupdrvDialogIsVisible(Ihandle* ih)
{
  BWindow* win = (BWindow*)ih->handle;
  return !win->IsHidden();
}

void iupdrvDialogGetSize(Ihandle* ih, InativeHandle* handle, int *w, int *h)
{
  BWindow* window = (BWindow*)ih->handle;
  BRect bounds = window->Bounds();

  if(w) *w = (int)bounds.right;
  if(h) *h = (int)bounds.bottom;
}

void iupdrvDialogSetVisible(Ihandle* ih, int visible)
{
  BHandler* handler = (BHandler*)ih->handle;
  BWindow* window = dynamic_cast<BWindow*>(handler);
  if(window) {
    if(visible)
      window->Show();
    else
	  window->Hide();
	return;
  }

  // TODO We can get there even though the window is already destroyed...
}

void iupdrvDialogGetPosition(Ihandle *ih, InativeHandle* handle, int *x, int *y)
{
	UNIMPLEMENTED
}

void iupdrvDialogSetPosition(Ihandle *ih, int x, int y)
{
	BWindow* window = (BWindow*)ih->handle;
	window->MoveTo(x, y);
}

static int gtkDialogGetMenuSize(Ihandle* ih)
{
  if (ih->data->menu && !iupgtk_globalmenu)
    return iupdrvMenuGetMenuBarSize(ih->data->menu);
  else
    return 0;
}

static void gtkDialogGetWindowDecor(Ihandle* ih, int *win_border, int *win_caption)
{
	UNIMPLEMENTED
}

void iupdrvDialogGetDecoration(Ihandle* ih, int *border, int *caption, int *menu)
{
  static int native_border = 0;
  static int native_caption = 0;

  int has_titlebar = iupAttribGetBoolean(ih, "RESIZE")  || /* GTK and Motif only */
                     iupAttribGetBoolean(ih, "MAXBOX")  ||
                     iupAttribGetBoolean(ih, "MINBOX")  ||
                     iupAttribGetBoolean(ih, "MENUBOX") || 
                     IupGetAttribute(ih, "TITLE"); /* must use IupGetAttribute to check from the native implementation */

  int has_border = has_titlebar ||
                   iupAttribGetBoolean(ih, "RESIZE") ||
                   iupAttribGetBoolean(ih, "BORDER");

  *menu = gtkDialogGetMenuSize(ih);

  if (ih->handle && iupdrvIsVisible(ih))
  {
    int win_border, win_caption;
    gtkDialogGetWindowDecor(ih, &win_border, &win_caption);

#ifdef WIN32
    if (*menu)
      win_caption -= *menu;
#endif

    if (!native_border && *border)
      native_border = win_border;

    if (!native_caption && *caption)
      native_caption = win_caption;
  }

  // In Haiku, we don't need to worry about the borders when sizing a window.
  // They are not part of the stuff we resize. So just tell IUP that the
  // border uses no space at all.
  *border = 0;
  *caption = 0;
}

int iupdrvDialogSetPlacement(Ihandle* ih)
{
  char* placement;
  int old_state = ih->data->show_state;
  ih->data->show_state = IUP_SHOW;

  BWindow* window = (BWindow*)ih->handle;

  if (iupAttribGetBoolean(ih, "FULLSCREEN"))
  {
	  UNIMPLEMENTED
    return 1;
  }
  
  placement = iupAttribGet(ih, "PLACEMENT");
  if (!placement)
  {
    if (old_state == IUP_MAXIMIZE || old_state == IUP_MINIMIZE)
      ih->data->show_state = IUP_RESTORE;

	window->ResizeTo(ih->naturalwidth, ih->naturalheight);
    return 0;
  }

  if (iupStrEqualNoCase(placement, "MINIMIZED"))
  {
    ih->data->show_state = IUP_MINIMIZE;
	UNIMPLEMENTED
  }
  else if (iupStrEqualNoCase(placement, "MAXIMIZED"))
  {
    int width, height, x, y;
    int border, caption, menu;
    iupdrvDialogGetDecoration(ih, &border, &caption, &menu);

    x = (border);
    y = (border+caption+menu);

    iupdrvGetFullSize(&width, &height);
    height += menu; /* menu is inside the client area. */

	height -= y + border;
	width -= border * 2;

    ih->data->show_state = IUP_MAXIMIZE;

	window->ResizeTo(width, height);
	window->MoveTo(x, y);
  }
  else if (iupStrEqualNoCase(placement, "FULL"))
  {
	BScreen scr(window);
	BRect frame = scr.Frame();

	window->ResizeTo(frame.Width(), frame.Height());
	window->MoveTo(frame.left, frame.top);

    if (old_state == IUP_MAXIMIZE || old_state == IUP_MINIMIZE)
      ih->data->show_state = IUP_RESTORE;
  }

  iupAttribSet(ih, "PLACEMENT", NULL); /* reset to NORMAL */

  return 1;
}


static int beDialogSetTitleAttrib(Ihandle* ih, const char* value)
{
  if (!value)
    value = "";

  BWindow* window = (BWindow*)ih->handle;
  window->SetTitle(value);

  return 0;
}

static char* beDialogGetTitleAttrib(Ihandle* ih)
{
  BWindow* window = (BWindow*)ih->handle;
  const char* title = window->Title();

  if (!title || title[0] == 0)
    return NULL;
  else
    return iupStrGetMemoryCopy(title);
}    

/****************************************************************
                     Idialog Methods
****************************************************************/

// TODO these could be IUPWindow static private methods ?
extern void iuphaikuListInvoked(Ihandle* ih, int item);
extern void iuphaikuToggleInvoked(Ihandle* ih, int value);
extern void iuphaikuButtonInvoked(Ihandle* ih);
extern void iuphaikuListSelected(Ihandle* ih, int item);

// This one also used at BApplication level
extern void iuphaikuTimerFired(Ihandle* ih);

class IUPWindow: public BWindow
{
  private:
	  Ihandle* myHandle;
  public:
	
  IUPWindow(Ihandle* ih)
	: BWindow(BRect(100,100,200,200), "", B_TITLED_WINDOW, 0)
  {
	this->myHandle = ih;
	BView* view = new BView(Bounds(), "background",B_FOLLOW_ALL_SIDES, 0);
	view->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	AddChild(view);
  }

  void MessageReceived(BMessage* message)
  {
	Ihandle* ih = NULL;
	int32 index = 0;

	message->FindPointer("iHandle", (void**)&ih);

	switch(message->what)
	{
	  case checkboxInvoke:
		message->FindInt32("be:value", &index);
		iuphaikuToggleInvoked(ih, index);
		break;
	  case listInvoke:
		message->FindInt32("index", &index);
		iuphaikuListInvoked(ih, index);
		break;
	  case listSelect:
		message->FindInt32("index", &index);
		iuphaikuListSelected(ih, index);
		break;
	  case buttonInvoke:
	  case menuInvoke:
		iuphaikuButtonInvoked(ih);
		break;
	  case timerFire:
		iuphaikuTimerFired(ih);
		break;
      default:
		BWindow::MessageReceived(message);
	}
  }

  bool QuitRequested()
  {
    Icallback cb = IupGetCallback(myHandle, "CLOSE_CB");
    if (cb)
    {
      int ret = cb(myHandle);
      if (ret == IUP_IGNORE)
        return false;
      if (ret == IUP_CLOSE)
        return true;
	}

	return true;
  }
};


static int beDialogMapMethod(Ihandle* ih)
{
  ih->handle = (InativeHandle*)new IUPWindow(ih);
  return IUP_NOERROR;
}


/* replace the common dialog SetChildrenPosition method because of 
   the menu that it is inside the dialog. */
static void beDialogSetChildrenPositionMethod(Ihandle* ih, int x, int y)
{
  int menu_h = gtkDialogGetMenuSize(ih);
  (void)x;
  (void)y;

  /* Child coordinates are relative to client left-top corner. */
  iupBaseSetPosition(ih->firstchild, 0, menu_h);
}

/****************************************************************************
                                   Attributes
****************************************************************************/

void iupdrvDialogInitClass(Iclass* ic)
{
	ic->Map = beDialogMapMethod;
	/*
	ic->UnMap = beDialogUnMapMethod;
	ic->LayoutUpdate = beDialogLayoutUpdateMethod;
	ic->GetInnerNativeContainerHandle = beDialogGetInnerNativeContainerHandleMethod;
	*/
	ic->SetChildrenPosition = beDialogSetChildrenPositionMethod;
	
	// Register TRAYCLICK_CB callback
	
	// Set platform-specific handlers
	// HWND/XWINDOW > Get native BWindow pointer
	// CLIENTSIZE > window size
	// CLIENTOFFSET > window position
	
  /* Visual */
  iupClassRegisterAttribute(ic, "BGCOLOR", NULL, iupdrvBaseSetBgColorAttrib, "DLGBGCOLOR", NULL, IUPAF_DEFAULT);  /* force new default value */

  /* Special */
  iupClassRegisterAttribute(ic, "TITLE", beDialogGetTitleAttrib, beDialogSetTitleAttrib,
	NULL, NULL, IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);

	// BACKGROUND
	// ICON
	// FULLSCREEN
	// MINSIZE
	// MAXSIZE
	// SAVEUNDER ?
	// ACTIVEWINDOW
	// TOPMOST
	// DIALOGHINT
	// OPACITY
	// TRAY
	// TRAYIMAGE
	// TRAYTIP
	// TRAYTIPMARKUP
}
