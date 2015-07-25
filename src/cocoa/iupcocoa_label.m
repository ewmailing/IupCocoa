/** \file
 * \brief Label Control
 *
 * See Copyright Notice in "iup.h"
 */

#include <Cocoa/Cocoa.h>

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

#include "iupcocoa_drv.h"

static int cocoaLabelSetTitleAttrib(Ihandle* ih, const char* value)
{
	id the_label = ih->handle;
	if(the_label)
	{
		// This could be a NSTextField, some kind of image, or something else.
		
		if([the_label respondsToSelector:@selector(setStringValue:)])
		{
			NSString* ns_string = [NSString stringWithUTF8String:value];
			[the_label setStringValue:ns_string];
			[the_label sizeToFit];
		}
	}
	return 1;

}

static int cocoaLabelMapMethod(Ihandle* ih)
{
	char* value;
	// using id because we may be using different types depending on the case
	id the_label;
	
	value = iupAttribGet(ih, "SEPARATOR");
	if (value)
	{
		if (iupStrEqualNoCase(value, "HORIZONTAL"))
		{
			ih->data->type = IUP_LABEL_SEP_HORIZ;

			
		}
		else /* "VERTICAL" */
		{
			ih->data->type = IUP_LABEL_SEP_VERT;

			
		}
	}
	else
	{
		value = iupAttribGet(ih, "IMAGE");
		if (value)
		{
			ih->data->type = IUP_LABEL_IMAGE;

		}
		else
		{
			ih->data->type = IUP_LABEL_TEXT;

			the_label = [[NSTextField alloc] initWithFrame:NSZeroRect];
//			the_label = [[NSTextField alloc] initWithFrame:NSMakeRect(0, 0, 100, 100)];

			[the_label setBezeled:NO];
			[the_label setDrawsBackground:NO];
			[the_label setEditable:NO];
			[the_label setSelectable:NO];
			
		
		
		}
	}
	
	if (!the_label)
	{
		return IUP_ERROR;
	}
	
	
	ih->handle = the_label;

	
	
	/* add to the parent, all GTK controls must call this. */
//	iupgtkAddToParent(ih);
	
	Ihandle* ih_parent = ih->parent;
	id parent_native_handle = ih_parent->handle;
	
	if([parent_native_handle isKindOfClass:[NSWindow class]])
	{
		NSWindow* parent_window = (NSWindow*)parent_native_handle;
		NSView* window_view = [parent_window contentView];
		[window_view addSubview:the_label];
	}
	else if([parent_native_handle isKindOfClass:[NSWindow class]])
	{
		NSView* parent_view = (NSView*)parent_native_handle;
	
		[parent_view addSubview:the_label];
	}
	else
	{
		NSCAssert(1, @"Unexpected type for parent widget");
	}
		
	/* configure for DRAG&DROP of files */
	if (IupGetCallback(ih, "DROPFILES_CB"))
	{
		iupAttribSet(ih, "DROPFILESTARGET", "YES");
	}
	
	return IUP_NOERROR;
}


static void cocoaLabelUnMapMethod(Ihandle* ih)
{
	id the_label = ih->handle;
	[the_label release];
	ih->handle = nil;

}

void iupdrvLabelInitClass(Iclass* ic)
{
  /* Driver Dependent Class functions */
  ic->Map = cocoaLabelMapMethod;
	ic->UnMap = cocoaLabelUnMapMethod;

#if 0

  /* Driver Dependent Attribute functions */

  /* Overwrite Visual */
  iupClassRegisterAttribute(ic, "ACTIVE", iupBaseGetActiveAttrib, gtkLabelSetActiveAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_DEFAULT);

  /* Visual */
  iupClassRegisterAttribute(ic, "BGCOLOR", iupBaseNativeParentGetBgColorAttrib, gtkLabelSetBgColorAttrib, IUPAF_SAMEASSYSTEM, "DLGBGCOLOR", IUPAF_DEFAULT);

  /* Special */
  iupClassRegisterAttribute(ic, "FGCOLOR", NULL, iupdrvBaseSetFgColorAttrib, IUPAF_SAMEASSYSTEM, "DLGFGCOLOR", IUPAF_DEFAULT);
	
#endif
	
  iupClassRegisterAttribute(ic, "TITLE", NULL, cocoaLabelSetTitleAttrib, NULL, NULL, IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);
#if 0
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
#endif
}
