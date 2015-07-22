/** \file
 * \brief Haiku Base Functions
 *
 * See Copyright Notice in "iup.h"
 */

#include <Control.h>
#include <ScrollBar.h>
#include <UTF8.h>
#include <Window.h>

#include <stdio.h>              
#include <stdlib.h>
#include <string.h>             
#include <limits.h>             

#include "iup.h"
#include "iupcbs.h"
#include "iupkey.h"

#include "iup_object.h"
#include "iup_childtree.h"
#include "iup_key.h"
#include "iup_str.h"
#include "iup_class.h"
#include "iup_attrib.h"
#include "iup_focus.h"
#include "iup_key.h"
#include "iup_image.h"
#include "iup_drv.h"

#include "iuphaiku_drv.h"

#define UNIMPLEMENTED printf("%s (%s %d) UNIMPLEMENTED\n",__func__,__FILE__,__LINE__);

void iupgtkUpdateMnemonic(Ihandle* ih)
{
	UNIMPLEMENTED
}

void iupdrvActivate(Ihandle* ih)
{
	UNIMPLEMENTED
}

void iupdrvReparent(Ihandle* ih)
{
	UNIMPLEMENTED
}


BView* viewFromHandler(BHandler* handler)
{
	BWindow* window = dynamic_cast<BWindow*>(handler);
	if (window)
	  return window->ChildAt(0); // Get background view
	else
   	  return dynamic_cast<BView*>(handler);
}


void iuphaikuBaseAddToParent(Ihandle* ih)
{
	BHandler* parent = (BHandler*)iupChildTreeGetNativeParentHandle(ih);
	BView* child = (BView*)ih->handle;

	BView* view = viewFromHandler(parent);
	if(view)
	  view->AddChild(child);
	else
	  debugger("Trying to add a control to a parent that has no BView");
}

void iupdrvBaseLayoutUpdateMethod(Ihandle *ih)
{
	BView* view = (BView*)ih->handle;
	view->MoveTo(ih->x, ih->y);
	view->ResizeTo(ih->currentwidth, ih->currentheight);
}

void iupdrvBaseUnMapMethod(Ihandle* ih)
{
  BHandler* handler = (BHandler*)ih->handle;
  BView* view = dynamic_cast<BView*>(handler);
  BWindow* window = dynamic_cast<BWindow*>(handler);
  if(view) {
	view->RemoveSelf();
    delete view;
  } else if(window) {
    window->Quit();
	// The window will delete itself
  } else if(handler != NULL)
    UNIMPLEMENTED
}

void iupdrvPostRedraw(Ihandle *ih)
{
	UNIMPLEMENTED
}

void iupdrvRedrawNow(Ihandle *ih)
{
	UNIMPLEMENTED
}

void iupdrvScreenToClient(Ihandle* ih, int *x, int *y)
{
	UNIMPLEMENTED
}

void iupdrvClientToScreen(Ihandle* ih, int *x, int *y)
{
	UNIMPLEMENTED
}

int iupdrvBaseSetZorderAttrib(Ihandle* ih, const char* value)
{
	UNIMPLEMENTED
	return 0;
}

void iupdrvSetVisible(Ihandle* ih, int visible)
{
  BView* view = (BView*)ih->handle;
  if(visible && view->IsHidden())
    view->Show();
  else if(!visible && !view->IsHidden())
	view->Hide();
}

int iupdrvIsVisible(Ihandle* ih)
{
  BView* view = (BView*)ih->handle;
  return !view->IsHidden();
}

int iupdrvIsActive(Ihandle *ih)
{
	BHandler* handler = (BHandler*)ih->handle;

	BControl* control = dynamic_cast<BControl*>(handler);
	if (control && !control->IsEnabled())
	  return false;
	return true;
}

void iupdrvSetActive(Ihandle* ih, int enable)
{
	BHandler* handler = (BHandler*)ih->handle;

	BView* view = dynamic_cast<BView*>(handler);
	BWindow* window = dynamic_cast<BWindow*>(handler);
	if (view)
	  view->MakeFocus(enable);
	if (window)
	  window->Activate(enable);
}

void iuphaikuBaseSetBgColor(InativeHandle* handle, unsigned char r, unsigned char g, unsigned char b)
{
  BView* view = viewFromHandler((BHandler*)handle);

  view->SetLowColor(r,g,b);
  view->SetViewColor(r,g,b);
}

void iuphaikuBaseSetFgColor(InativeHandle* handle, unsigned char r, unsigned char g, unsigned char b)
{
  BView* view = viewFromHandler((BHandler*)handle);

  view->SetHighColor(r,g,b);
}

int iupdrvBaseSetBgColorAttrib(Ihandle* ih, const char* value)
{
  unsigned char r, g, b;
  if (!iupStrToRGB(value, &r, &g, &b))
    return 0;

  iuphaikuBaseSetBgColor(ih->handle, r, g, b);

  /* DO NOT NEED TO UPDATE GTK IMAGES SINCE THEY DO NOT DEPEND ON BGCOLOR */

  return 1;
}

int iupdrvBaseSetFgColorAttrib(Ihandle* ih, const char* value)
{
  unsigned char r, g, b;
  if (!iupStrToRGB(value, &r, &g, &b))
    return 0;

  iuphaikuBaseSetFgColor(ih->handle, r, g, b);

  return 1;
}

int iupdrvBaseSetCursorAttrib(Ihandle* ih, const char* value)
{
	UNIMPLEMENTED
  return 0;
}

int iupdrvGetScrollbarSize(void)
{
  return (int)B_V_SCROLL_BAR_WIDTH;
}

void iupdrvDrawFocusRect(Ihandle* ih, void* _gc, int x, int y, int w, int h)
{
	UNIMPLEMENTED
}

void iupdrvBaseRegisterCommonAttrib(Iclass* ic)
{
  // Other drivers seems to register some info about fonts and tooltips here.
  // I'm not sure what is needed and there is no documentation...
  iupClassRegisterAttribute(ic, "TIPMARKUP", NULL, NULL, IUPAF_SAMEASSYSTEM, NULL, IUPAF_NOT_MAPPED);
  iupClassRegisterAttribute(ic, "TIPICON", NULL, NULL, IUPAF_SAMEASSYSTEM, NULL, IUPAF_NOT_MAPPED);
}

void iupgtkReleaseConvertUTF8(void)
{
	UNIMPLEMENTED
}

char* iuphaikuStrConvertToUTF8(const char* str)  /* From IUP to GTK */
{
  if (!str || *str == 0)
    return (char*)str;

  if (iupgtk_utf8autoconvert)  /* this means str is in current locale */
  {
    static char* buffer = NULL;
    int32 len = strlen(str);
	int32 len2 = 2 * len;
	int32 cookie;
	free(buffer);
	buffer = (char*)malloc(len * 2); // Should be enough in most cases ?
    convert_to_utf8(B_ISO9_CONVERSION, str, &len, buffer, &len2, &cookie);
    return buffer;
  }

  return (char*)str;
}

char* iuphaikuStrConvertFromUTF8(const char* str)  /* From GTK to IUP */
{
  if (!str || *str == 0)
    return (char*)str;

  if (iupgtk_utf8autoconvert)  /* this means str is in current locale */
  {
    static char* buffer = NULL;
    int32 len = strlen(str);
	int32 len2 = 2 * len;
	int32 cookie;
	free(buffer);
	buffer = (char*)malloc(len * 2); // Should be enough in most cases ?
    convert_from_utf8(B_ISO9_CONVERSION, str, &len, buffer, &len2, &cookie);
    return buffer;
  }

  return (char*)str;
}

char* iupgtkStrConvertFromFilename(const char* str)   /* From Filename to IUP */
{
	UNIMPLEMENTED
  return (char*)str;
}

void iupdrvSendKey(int key, int press)
{
	UNIMPLEMENTED
}

void iupdrvWarpPointer(int x, int y)
{
	UNIMPLEMENTED
}

void iupdrvSendMouse(int x, int y, int bt, int status)
{
	UNIMPLEMENTED
}

void iupdrvSleep(int time)
{
	UNIMPLEMENTED
}

