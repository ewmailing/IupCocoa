/** \file
 * \brief Label Control
 *
 * See Copyright Notice in "iup.h"
 */

#include <StringView.h>
#include <View.h>

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
#include "iup_image.h"
#include "iup_label.h"
#include "iup_drv.h"
#include "iup_image.h"
#include "iup_focus.h"

#include "iuphaiku_drv.h"

#define UNIMPLEMENTED printf("%s (%s %d) UNIMPLEMENTED\n",__func__,__FILE__,__LINE__);

static int beLabelSetTitleAttrib(Ihandle* ih, const char* value)
{
  if (ih->data->type == IUP_LABEL_TEXT) {
    BStringView* stringView = (BStringView*)ih->handle;
  	stringView->SetText(iuphaikuStrConvertToUTF8(value));

	// TODO handle the mnemonic ?

	return 1;
  }
  return 0;
}

static int gtkLabelSetWordWrapAttrib(Ihandle* ih, const char* value)
{
	UNIMPLEMENTED
  return 0;
}

static int gtkLabelSetEllipsisAttrib(Ihandle* ih, const char* value)
{
	UNIMPLEMENTED
  return 0;
}

static int gtkLabelSetAlignmentAttrib(Ihandle* ih, const char* value)
{
  if (ih->data->type != IUP_LABEL_SEP_HORIZ && ih->data->type != IUP_LABEL_SEP_VERT)
  {
    BStringView* view = (BStringView*)ih->handle;
    char value1[30]="", value2[30]="";

    iupStrToStrStr(value, value1, value2, ':');
    if (iupStrEqualNoCase(value1, "ARIGHT"))
		view->SetAlignment(B_ALIGN_RIGHT);
    else if (iupStrEqualNoCase(value1, "ACENTER"))
		view->SetAlignment(B_ALIGN_CENTER);
    else /* "ALEFT" */
		view->SetAlignment(B_ALIGN_LEFT);

	/* TODO
    if (iupStrEqualNoCase(value2, "ABOTTOM"))
    else if (iupStrEqualNoCase(value2, "ATOP"))
    else  // ACENTER (default)
	*/

	return 1;
  } else {
    // TODO handle image labels as well, but ignore separators
	UNIMPLEMENTED
    return 0;
  }
}

static int gtkLabelSetPaddingAttrib(Ihandle* ih, const char* value)
{
  if (ih->handle && ih->data->type != IUP_LABEL_SEP_HORIZ && ih->data->type != IUP_LABEL_SEP_VERT)
  {
	// There's no way to do that in Haiku BStringView ?
	UNIMPLEMENTED
    return 0;
  } else
    return 1; /* store until not mapped, when mapped will be set again */
}

static char* gtkLabelGetWidgetPangoLayoutAttrib(Ihandle* ih)
{
	UNIMPLEMENTED
    return NULL;
}


static void beLabelSetPixbuf(Ihandle* ih, const char* name, int make_inactive)
{
  BView* widget = (BView*)ih->handle;

  if (name)
  {
    BBitmap* bitmap = (BBitmap*)iupImageGetImage(name, ih, make_inactive);
	// TODO we may need to play with source and destination rects to align the
	// image here ? Or maybe it's simpler to handle that at view level...
	widget->SetViewBitmap(bitmap, B_FOLLOW_NONE, 0);
  } else {
	widget->ClearViewBitmap();
  }
}

static int gtkLabelSetImageAttrib(Ihandle* ih, const char* value)
{
  if (ih->data->type == IUP_LABEL_IMAGE)
  {
    if (iupdrvIsActive(ih))
      beLabelSetPixbuf(ih, value, 0);
    else
    {
      if (!iupAttribGet(ih, "IMINACTIVE"))
      {
        /* if not active and IMINACTIVE is not defined 
           then automaticaly create one based on IMAGE */
        beLabelSetPixbuf(ih, value, 1); /* make_inactive */
      }
    }
    return 1;
  }
  else
    return 0;
}

static int gtkLabelSetImInactiveAttrib(Ihandle* ih, const char* value)
{
  if (ih->data->type == IUP_LABEL_IMAGE)
  {
    if (!iupdrvIsActive(ih))
    {
      if (value)
        beLabelSetPixbuf(ih, value, 0);
      else
      {
        /* if not defined then automaticaly create one based on IMAGE */
        char* name = iupAttribGet(ih, "IMAGE");
        beLabelSetPixbuf(ih, name, 1); /* make_inactive */
      }
    }
    return 1;
  }
  else
    return 0;
}

static int gtkLabelSetActiveAttrib(Ihandle* ih, const char* value)
{
  /* update the inactive image if necessary */
  if (ih->data->type == IUP_LABEL_IMAGE)
  {
    if (!iupStrBoolean(value))
    {
      char* name = iupAttribGet(ih, "IMINACTIVE");
      if (name)
        beLabelSetPixbuf(ih, name, 0);
      else
      {
        /* if not defined then automaticaly create one based on IMAGE */
        name = iupAttribGet(ih, "IMAGE");
        beLabelSetPixbuf(ih, name, 1); /* make_inactive */
      }
    }
    else
    {
      /* must restore the normal image */
      char* name = iupAttribGet(ih, "IMAGE");
      beLabelSetPixbuf(ih, name, 0);
    }
  }

  return iupBaseSetActiveAttrib(ih, value);
}

class IUPSeparator: public BView
{
  public:
	IUPSeparator(/*orientation dir*/)
	  : BView(BRect(0,0,1,1), "Separator", B_FOLLOW_NONE, B_WILL_DRAW)
	{
	  SetHighColor(tint_color(ui_color(B_PANEL_BACKGROUND_COLOR),
	    B_DISABLED_LABEL_TINT));

	}

	void Draw(BRect updateRect)
	{
      BRect bounds = Bounds();
	  if (bounds.Width() > bounds.Height())
	  {
	    StrokeLine(bounds.LeftTop(), bounds.RightTop());
	  } else {
	    StrokeLine(bounds.LeftTop(), bounds.LeftBottom());
	  }
	}
};

static int beLabelMapMethod(Ihandle* ih)
{
  char* value = iupAttribGet(ih, "SEPARATOR");
  BView* label = NULL;
  
  if (value)
  {
	// TODO maybe we need to hint the separator direction here ?
	label = new IUPSeparator();
    if (iupStrEqualNoCase(value, "HORIZONTAL"))
    {
      ih->data->type = IUP_LABEL_SEP_HORIZ;
	} else {
      ih->data->type = IUP_LABEL_SEP_VERT;
	}
  } else {
    value = iupAttribGet(ih, "IMAGE");
    if (value)
	{
      ih->data->type = IUP_LABEL_IMAGE;
	  // FIXME We rely on SetViewBitmap to put the image on screen. But this
	  // doesn't work too well when the image needs an alpha channel
	  label = new BView(BRect(0,0,10,10),"Label/Image", B_FOLLOW_NONE, 0);
	} else {
      ih->data->type = IUP_LABEL_TEXT;
	  label = new BStringView(BRect(0, 0, 10, 10),"StringView", "");
	}
  }

  if (!label)
    return IUP_ERROR;

  label->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));

  ih->handle = (InativeHandle*)label;

  iuphaikuBaseAddToParent(ih);

  return IUP_NOERROR;
}

void iupdrvLabelInitClass(Iclass* ic)
{
  /* Driver Dependent Class functions */
  ic->Map = beLabelMapMethod;

  /* Driver Dependent Attribute functions */

  /* Common GTK only (when text is in a secondary element) */
  iupClassRegisterAttribute(ic, "WIDGETPANGOLAYOUT", gtkLabelGetWidgetPangoLayoutAttrib, NULL, NULL, NULL, IUPAF_NO_INHERIT);

  /* Overwrite Visual */
  iupClassRegisterAttribute(ic, "ACTIVE", iupBaseGetActiveAttrib, gtkLabelSetActiveAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_DEFAULT);

  /* Visual */
  iupClassRegisterAttribute(ic, "BGCOLOR", iupBaseNativeParentGetBgColorAttrib, NULL, IUPAF_SAMEASSYSTEM, "DLGBGCOLOR", IUPAF_DEFAULT);

  /* Special */
  iupClassRegisterAttribute(ic, "FGCOLOR", NULL, iupdrvBaseSetFgColorAttrib, IUPAF_SAMEASSYSTEM, "DLGFGCOLOR", IUPAF_DEFAULT);
  iupClassRegisterAttribute(ic, "TITLE", NULL, beLabelSetTitleAttrib, NULL, NULL, IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);

  /* IupLabel only */
  iupClassRegisterAttribute(ic, "ALIGNMENT", NULL, gtkLabelSetAlignmentAttrib, "ALEFT:ACENTER", NULL, IUPAF_NO_INHERIT);  /* force new default value */
  iupClassRegisterAttribute(ic, "IMAGE", NULL, gtkLabelSetImageAttrib, NULL, NULL, IUPAF_IHANDLENAME|IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "PADDING", iupLabelGetPaddingAttrib, gtkLabelSetPaddingAttrib, IUPAF_SAMEASSYSTEM, "0x0", IUPAF_NOT_MAPPED);

  /* IupLabel GTK and Motif only */
  iupClassRegisterAttribute(ic, "IMINACTIVE", NULL, gtkLabelSetImInactiveAttrib, NULL, NULL, IUPAF_IHANDLENAME|IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);

  /* IupLabel Windows and GTK only */
  iupClassRegisterAttribute(ic, "WORDWRAP", NULL, gtkLabelSetWordWrapAttrib, NULL, NULL, IUPAF_DEFAULT);
  iupClassRegisterAttribute(ic, "ELLIPSIS", NULL, gtkLabelSetEllipsisAttrib, NULL, NULL, IUPAF_DEFAULT);

  /* IupLabel GTK only */
  iupClassRegisterAttribute(ic, "MARKUP", NULL, NULL, NULL, NULL, IUPAF_DEFAULT);
}
