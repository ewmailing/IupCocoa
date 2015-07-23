/** \file
 * \brief Canvas Control
 *
 * See Copyright Notice in "iup.h"
 */

#include <ScrollView.h>
#include <View.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <stdarg.h>
#include <limits.h>

#include "iup.h"
#include "iupcbs.h"

#include "iup_object.h"
#include "iup_layout.h"
#include "iup_attrib.h"
#include "iup_dialog.h"
#include "iup_str.h"
#include "iup_drv.h"
#include "iup_drvinfo.h"
#include "iup_drvfont.h"
#include "iup_canvas.h"
#include "iup_key.h"

#include "iuphaiku_drv.h"

#define UNIMPLEMENTED printf("%s (%s %d) UNIMPLEMENTED\n",__func__,__FILE__,__LINE__);

#define GTK_TYPE_IUPDRAWING_AREA            (iup_gtk_drawing_area_get_type ())
#define GTK_IUPDRAWING_AREA(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GTK_TYPE_IUPDRAWING_AREA, GtkIupDrawingArea))

enum {
   PROP_0,
   PROP_HADJUSTMENT,
   PROP_VADJUSTMENT,
   PROP_HSCROLL_POLICY,
   PROP_VSCROLL_POLICY
};

static int gtkCanvasSetBgColorAttrib(Ihandle* ih, const char* value);

static int gtkCanvasSetXAutoHideAttrib(Ihandle* ih, const char *value)
{
	UNIMPLEMENTED
  return 1;
}

static int gtkCanvasSetYAutoHideAttrib(Ihandle* ih, const char *value)
{
	UNIMPLEMENTED
  return 1;
}

static int gtkCanvasSetDXAttrib(Ihandle* ih, const char *value)
{
	UNIMPLEMENTED
  return 1;
}

static int gtkCanvasSetDYAttrib(Ihandle* ih, const char *value)
{
	UNIMPLEMENTED
  return 1;
}

static BView* iuphaikuGetScrolled(Ihandle* ih)
{
  BScrollView* sv = (BScrollView*)ih->handle;
  BScrollBar* sb = sv->ScrollBar(B_HORIZONTAL);
  if(!sb) sb = sv->ScrollBar(B_VERTICAL);
  return sb->Target();
}

static int gtkCanvasSetPosXAttrib(Ihandle* ih, const char *value)
{
  float posx, xmin, xmax, dx;
  if (!(ih->data->sb & IUP_SB_HORIZ))
	return 1;
  if (!iupStrToFloat(value, &posx))
    return 1;

  xmin = iupAttribGetFloat(ih, "XMIN");
  xmax = iupAttribGetFloat(ih, "XMAX");
  dx = iupAttribGetFloat(ih, "DX");

  if (posx < xmin) posx = xmin;
  if (posx > (xmax - dx)) posx = xmax - dx;
  ih->data->posx = posx;

  BView* canvas = iuphaikuGetScrolled(ih);

  BPoint curPos = canvas->LeftTop();
  curPos.x = posx;
  canvas->ScrollTo(curPos);

  return 1;
}

static int gtkCanvasSetPosYAttrib(Ihandle* ih, const char *value)
{
  float posy, ymin, ymax, dy;
  if (!iupStrToFloat(value, &posy))
    return 1;
  ymin = iupAttribGetFloat(ih, "YMIN");
  ymax = iupAttribGetFloat(ih, "YMAX");
  dy = iupAttribGetFloat(ih, "DY");

  if (posy < ymin) posy = ymin;
  if (posy > (ymax - dy)) posy = ymax - dy;
  ih->data->posy = posy;

  BView* canvas = (BView*)iupAttribGetStr(ih, "DRAWABLE");
  BPoint origin = canvas->LeftTop();
  canvas->ScrollTo(origin.x, posy);
  
  return 1;
}

static int gtkCanvasSetBgColorAttrib(Ihandle* ih, const char* value)
{
  // TODO This will set the color for the BScrollView. Is it ok ?
  return iupdrvBaseSetBgColorAttrib(ih, value);
	/*
  BView* canvas = iuphaikuGetScrolled(ih);
  unsigned char r, g, b;
  
  if (iupStrToRGB(parent_value, &r, &g, &b))
  {
  }
  
  canvas->SetViewColor();
  canvas->SetLowColor();
  return 1;
  */
}

static void haikuCanvasLayoutUpdateMethod(Ihandle *ih)
{
  iupdrvBaseLayoutUpdateMethod(ih);

  // Also resize the child
  BView* canvas = (BView*)iupAttribGetStr(ih, "DRAWABLE");
  // TODO we need to substract the scrollbar size, and maybe the canvas wants to
  // be bigger and have scrollbars...
  canvas->ResizeTo(ih->currentwidth - 2, ih->currentheight - 2);
}

static char* gtkCanvasGetDrawSizeAttrib(Ihandle *ih)
{
	UNIMPLEMENTED
		return NULL;
}

class IUPCanvas: public BView
{
  public:
	IUPCanvas(Ihandle* ih)
	  : BView(BRect(0, 0, 10, 10), "canvas", B_FOLLOW_ALL_SIDES, B_WILL_DRAW | B_FRAME_EVENTS) 
	  , fHandle(ih)
	{}

	void Draw(BRect rect)
	{
      iupAttribSetStrf(fHandle, "CLIPRECT", "%d %d %d %d", rect.left, rect.top, rect.right, rect.bottom);
      IFnff cb = (IFnff)IupGetCallback(fHandle,"ACTION");
      if (cb) {
		LockLooper();
        cb (fHandle, fHandle->data->posx, fHandle->data->posy);
		UnlockLooper();
	  }
	}

	// TODO handle ScrollBy/ScrollTo (manage X/YMIN/MAX, DX,DY, call SCROLL_CB
	// TODO handle size changes and call iupdrvBaseLayoutUpdateMethod(ih);
	// TODO handle events (focus, key press/release, enter, leave, ...)
	void FrameResized(float w, float h)
	{
      IFnii cb = (IFnii)IupGetCallback(fHandle, "RESIZE_CB");
      if (cb) {
		if (!LockLooper()) debugger("Failed to lock view from FrameResized");
        cb(fHandle, (int)w, (int)h);
		UnlockLooper();
	  }
	}
  private:
	Ihandle* fHandle;
};

static int haikuCanvasMapMethod(Ihandle* ih)
{
  if (!ih->parent)
    return IUP_ERROR;

  bool scrollh = false;
  bool scrollv = false;
  border_style border = B_NO_BORDER; 

  ih->data->sb = iupBaseGetScrollbar(ih);
  if (ih->data->sb & IUP_SB_HORIZ)
    scrollh = true;
  if (ih->data->sb & IUP_SB_VERT)
	scrollv = true;

  if (iupAttribGetBoolean(ih, "BORDER"))
  {
    iupAttribSetInt(ih, "BORDERWIDTH", 1);
    border = B_FANCY_BORDER;
  }

  int flags = 0; 

  /* To receive keyboard events, you will need to set the GTK_CAN_FOCUS flag on
   * the drawing area. */
  if (ih->iclass->is_interactive)
  {
    if (iupAttribGetBoolean(ih, "CANFOCUS"))
	  flags |= B_NAVIGABLE;
  }

  BView* canvas = new IUPCanvas(ih);

  ih->handle = (InativeHandle*)new BScrollView("scrollcanvas",
	canvas, B_FOLLOW_LEFT | B_FOLLOW_TOP, flags, scrollh, scrollv, border);

  /* configure for DRAG&DROP */
  if (IupGetCallback(ih, "DROPFILES_CB"))
    iupAttribSet(ih, "DROPFILESTARGET", "YES");

  iuphaikuBaseAddToParent(ih);

  iupAttribSet(ih, "DRAWABLE", (char*)canvas);

  return IUP_NOERROR;
}

void iupdrvCanvasInitClass(Iclass* ic)
{
  /* Driver Dependent Class functions */
  ic->Map = haikuCanvasMapMethod;
  ic->LayoutUpdate = haikuCanvasLayoutUpdateMethod;

  /* Driver Dependent Attribute functions */

  /* Visual */
  iupClassRegisterAttribute(ic, "BGCOLOR", NULL, gtkCanvasSetBgColorAttrib, "255 255 255", NULL, IUPAF_DEFAULT);  /* force new default value */

  /* IupCanvas only */
  iupClassRegisterAttribute(ic, "CURSOR", NULL, iupdrvBaseSetCursorAttrib, IUPAF_SAMEASSYSTEM, "ARROW", IUPAF_IHANDLENAME|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "DRAWSIZE", gtkCanvasGetDrawSizeAttrib, NULL, NULL, NULL, IUPAF_READONLY|IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "DX", NULL, gtkCanvasSetDXAttrib, "0.1", NULL, IUPAF_NO_INHERIT);  /* force new default value */
  iupClassRegisterAttribute(ic, "DY", NULL, gtkCanvasSetDYAttrib, "0.1", NULL, IUPAF_NO_INHERIT);  /* force new default value */
  iupClassRegisterAttribute(ic, "POSX", iupCanvasGetPosXAttrib, gtkCanvasSetPosXAttrib, "0", NULL, IUPAF_NO_INHERIT);  /* force new default value */
  iupClassRegisterAttribute(ic, "POSY", iupCanvasGetPosYAttrib, gtkCanvasSetPosYAttrib, "0", NULL, IUPAF_NO_INHERIT);  /* force new default value */
  iupClassRegisterAttribute(ic, "XAUTOHIDE", NULL, gtkCanvasSetXAutoHideAttrib, "YES", NULL, IUPAF_DEFAULT);  /* force new default value */
  iupClassRegisterAttribute(ic, "YAUTOHIDE", NULL, gtkCanvasSetYAutoHideAttrib, "YES", NULL, IUPAF_DEFAULT);  /* force new default value */

  /* IupCanvas Windows or X only */
#ifndef GTK_MAC
  #ifdef WIN32                                 
    iupClassRegisterAttribute(ic, "HWND", iupgtkGetNativeWindowHandle, NULL, NULL, NULL, IUPAF_NO_STRING|IUPAF_NO_INHERIT);
  #else
    iupClassRegisterAttribute(ic, "XWINDOW", iupgtkGetNativeWindowHandle, NULL, NULL, NULL, IUPAF_NO_INHERIT|IUPAF_NO_STRING);
    iupClassRegisterAttribute(ic, "XDISPLAY", (IattribGetFunc)iupdrvGetDisplay, NULL, NULL, NULL, IUPAF_READONLY|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT|IUPAF_NO_STRING);
  #endif
#endif

  /* Not Supported */
  iupClassRegisterAttribute(ic, "BACKINGSTORE", NULL, NULL, "YES", NULL, IUPAF_NOT_SUPPORTED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "TOUCH", NULL, NULL, NULL, NULL, IUPAF_NOT_SUPPORTED|IUPAF_NO_INHERIT);
}
