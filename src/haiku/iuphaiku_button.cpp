/** \file
 * \brief Button Control
 *
 * See Copyright Notice in "iup.h"
 */

#define UNIMPLEMENTED printf("%s (%s %d) UNIMPLEMENTED\n",__func__,__FILE__,__LINE__);

#include <Button.h>

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
#include "iup_button.h"
#include "iup_drv.h"
#include "iup_drvfont.h"
#include "iup_image.h"
#include "iup_key.h"

#include "iuphaiku_drv.h"

class IUPButton: public BButton
{
  public:
	IUPButton(BMessage* msg)
	  : BButton(BRect(0,0,10,10), "button", NULL, msg)
	  , fBitmap(NULL)
	{
	}

	void Draw(BRect updateRect)
	{
	  BButton::Draw(updateRect);
	  if (fBitmap) {
		MovePenTo(6, 6);
		SetDrawingMode(B_OP_ALPHA);
	    DrawBitmapAsync(fBitmap);
	  }
	}

	void SetBitmap(BBitmap* bitmap)
	{
      fBitmap = bitmap;
	}

  private:
	BBitmap* fBitmap;
};

void iupdrvButtonAddBorders(Ihandle* ih, int *x, int *y)
{
  // TODO can we use BControlLook here ?
  int border_size = 2*7+1; /* borders are not symetric */
  (void)ih;  
  (*x) += border_size;
  (*y) += border_size;
}

static int beButtonSetTitleAttrib(Ihandle* ih, const char* value)
{
  BButton* button = (BButton*)ih->handle;
  button->SetLabel(value);
  return 1;
}

static int haikuButtonSetAlignmentAttrib(Ihandle* ih, const char* value)
{
  char value1[30]="", value2[30]="";
  iupStrToStrStr(value, value1, value2, ':');
  
  if (iupStrEqualNoCase(value1, "ARIGHT"))
	UNIMPLEMENTED
  else if (iupStrEqualNoCase(value1, "ACENTER"))
  {
	// This is the default behavior in HAIKU
	// TODO actually restore it when the others are handled
  }
  else /* "ALEFT" */
	UNIMPLEMENTED

  if (iupStrEqualNoCase(value2, "ABOTTOM"))
	UNIMPLEMENTED
  else if (iupStrEqualNoCase(value2, "ATOP"))
	UNIMPLEMENTED
  else  /* ACENTER (default) */
  {
	// This is the default behavior in HAIKU
	// TODO actually restore it when the others are handled
  }

  return 1;
}

static int haikuButtonSetPaddingAttrib(Ihandle* ih, const char* value)
{
  iupStrToIntInt(value, &ih->data->horiz_padding, &ih->data->vert_padding, 'x');
  if (ih->handle)
  {
    if (ih->data->type == IUP_BUTTON_TEXT)   /* text only */
    {
	  UNIMPLEMENTED
    }
    else
    {
	  UNIMPLEMENTED
    }
    return 0;
  }
  else
    return 1; /* store until not mapped, when mapped will be set again */
}

static int gtkButtonSetFgColorAttrib(Ihandle* ih, const char* value)
{
	UNIMPLEMENTED
  return 0;
}

/*
static int gtkButtonSetStandardFontAttrib(Ihandle* ih, const char* value)
{
	UNIMPLEMENTED
  return 0;
}
*/

/* FIXME this is copypasted from the same method for Toggle. we should share it. */
static void gtkButtonSetPixbuf(Ihandle* ih, const char* name, int make_inactive)
{
  IUPButton* button = (IUPButton*)ih->handle;

  if (name)
  {
	BBitmap* bitmap = (BBitmap*)iupImageGetImage(name, ih, make_inactive);
	button->SetBitmap(bitmap);
  }
}

static int gtkButtonSetImageAttrib(Ihandle* ih, const char* value)
{
  if (ih->data->type & IUP_BUTTON_IMAGE)
  {
    if (iupdrvIsActive(ih))
      gtkButtonSetPixbuf(ih, value, 0);
    else
    {
      if (!iupAttribGet(ih, "IMINACTIVE"))
      {
        /* if not active and IMINACTIVE is not defined 
           then automaticaly create one based on IMAGE */
        gtkButtonSetPixbuf(ih, value, 1); /* make_inactive */
      }
    }
    return 1;
  }
  else
    return 0;
}

static int gtkButtonSetImInactiveAttrib(Ihandle* ih, const char* value)
{
  if (ih->data->type & IUP_BUTTON_IMAGE)
  {
    if (!iupdrvIsActive(ih))
    {
      if (value)
        gtkButtonSetPixbuf(ih, value, 0);
      else
      {
        /* if not defined then automaticaly create one based on IMAGE */
        char* name = iupAttribGet(ih, "IMAGE");
        gtkButtonSetPixbuf(ih, name, 1); /* make_inactive */
      }
    }
    return 1;
  }
  else
    return 0;
}

static int gtkButtonSetActiveAttrib(Ihandle* ih, const char* value)
{
  /* update the inactive image if necessary */
  if (ih->data->type & IUP_BUTTON_IMAGE)
  {
    if (!iupStrBoolean(value))
    {
      char* name = iupAttribGet(ih, "IMINACTIVE");
      if (name)
        gtkButtonSetPixbuf(ih, name, 0);
      else
      {
        /* if not defined then automaticaly create one based on IMAGE */
        name = iupAttribGet(ih, "IMAGE");
        gtkButtonSetPixbuf(ih, name, 1); /* make_inactive */
      }
    }
    else
    {
      /* must restore the normal image */
      char* name = iupAttribGet(ih, "IMAGE");
      gtkButtonSetPixbuf(ih, name, 0);
    }
  }

  return iupBaseSetActiveAttrib(ih, value);
}

static int gtkButtonMapMethod(Ihandle* ih)
{
  char* value;
  value = iupAttribGet(ih, "IMAGE");
  char* title = iupAttribGet(ih, "TITLE");

  BMessage* msg = new BMessage(buttonInvoke);
  msg->AddPointer("iHandle", ih);

  BButton* button = new IUPButton(msg);

  if (value)
  {
    ih->data->type = IUP_BUTTON_IMAGE;

    if (title && *title!=0) {
      ih->data->type |= IUP_BUTTON_TEXT;
	} 
  } else {
    ih->data->type = IUP_BUTTON_TEXT;
	button->SetLabel(title);
  }

  ih->handle = (InativeHandle*)button;

  iuphaikuBaseAddToParent(ih);
  return IUP_NOERROR;
}

void iupdrvButtonInitClass(Iclass* ic)
{
  /* Driver Dependent Class functions */
  ic->Map = gtkButtonMapMethod;

  /* Driver Dependent Attribute functions */

  /* Overwrite Common */
//  iupClassRegisterAttribute(ic, "STANDARDFONT", NULL, gtkButtonSetStandardFontAttrib, IUPAF_SAMEASSYSTEM, "DEFAULTFONT", IUPAF_NO_SAVE|IUPAF_NOT_MAPPED);

  /* Overwrite Visual */
  iupClassRegisterAttribute(ic, "ACTIVE", iupBaseGetActiveAttrib, gtkButtonSetActiveAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_DEFAULT);

  /* Visual */
  iupClassRegisterAttribute(ic, "BGCOLOR", NULL, iupdrvBaseSetBgColorAttrib, IUPAF_SAMEASSYSTEM, "DLGBGCOLOR", IUPAF_DEFAULT);

  /* Special */
  iupClassRegisterAttribute(ic, "FGCOLOR", NULL, gtkButtonSetFgColorAttrib, IUPAF_SAMEASSYSTEM, "DLGFGCOLOR", IUPAF_DEFAULT);
  iupClassRegisterAttribute(ic, "TITLE", NULL, beButtonSetTitleAttrib, NULL, NULL, IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);

  /* IupButton only */
  iupClassRegisterAttribute(ic, "ALIGNMENT", NULL, haikuButtonSetAlignmentAttrib, "ACENTER:ACENTER", NULL, IUPAF_NO_INHERIT);  /* force new default value */
  iupClassRegisterAttribute(ic, "IMAGE", NULL, gtkButtonSetImageAttrib, NULL, NULL, IUPAF_IHANDLENAME|IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "IMINACTIVE", NULL, gtkButtonSetImInactiveAttrib, NULL, NULL, IUPAF_IHANDLENAME|IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "IMPRESS", NULL, NULL, NULL, NULL, IUPAF_IHANDLENAME|IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "PADDING", iupButtonGetPaddingAttrib, haikuButtonSetPaddingAttrib, IUPAF_SAMEASSYSTEM, "0x0", IUPAF_NOT_MAPPED);
  iupClassRegisterAttribute(ic, "MARKUP", NULL, NULL, NULL, NULL, IUPAF_DEFAULT);
}

void iuphaikuButtonInvoked(Ihandle* ih)
{
  Icallback cb = IupGetCallback(ih, "ACTION");
  if (cb)
  {
    if (cb(ih) == IUP_CLOSE) 
      IupExitLoop();
  }
}
