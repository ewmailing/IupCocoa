/** \file
 * \brief Clipboard for the GTK Driver.
 *
 * See Copyright Notice in "iup.h"
 */


#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#import <Cocoa/Cocoa.h>

#include "iup.h"

#include "iup_object.h"
#include "iup_attrib.h"
#include "iup_str.h"
#include "iup_image.h"

#include "iupcocoa_drv.h"


static char* cocoaClipboardGetTextAvailableAttrib(Ihandle* ih)
{
	(void)ih;
	
	NSPasteboard* paste_board;
	NSString* type_available;

	paste_board = [NSPasteboard generalPasteboard];
	/* Ask if this kind of data is available on the paste board? */
	type_available = [paste_board availableTypeFromArray:[NSArray arrayWithObject:NSStringPboardType]];
	return iupStrReturnBoolean((int)type_available);

}

static int cocoaClipboardSetTextAttrib(Ihandle* ih, const char* value)
{
	NSPasteboard* paste_board;
	NSString* type_available;
	
	
	NSString* ns_string = nil;
	if(value)
	{
		// This will return nil if the string can't be converted.
		ns_string = [NSString stringWithUTF8String:value];
	}
	else
	{
		ns_string = @"";
	}
	
	paste_board = [NSPasteboard generalPasteboard];
	[paste_board declareTypes:[NSArray arrayWithObject:NSStringPboardType] owner:nil];
	
	/* Note: Assuming null terminated C string here. src_len is ignored. */
	[paste_board setString:ns_string forType:NSStringPboardType];
	
	return 0;
}

static char* cocoaClipboardGetTextAttrib(Ihandle* ih)
{
	NSPasteboard* paste_board;
	NSString* type_available;
	NSString* pasteboard_string = nil;

	paste_board = [NSPasteboard generalPasteboard];
	/* Ask if this kind of data is available on the paste board? */
	type_available = [paste_board availableTypeFromArray:[NSArray arrayWithObject:NSStringPboardType]];
	if(type_available)
//	if([type_available isEqualToString:NSStringPboardType])
	{
		/* Get the text string from the pasteboard */
		pasteboard_string = [paste_board stringForType:NSStringPboardType];
	}
	return iupStrReturnStr([pasteboard_string UTF8String]);
}


/******************************************************************************/

Ihandle* IupClipboard(void)
{
  return IupCreate("clipboard");
}

Iclass* iupClipboardNewClass(void)
{
  Iclass* ic = iupClassNew(NULL);

  ic->name = "clipboard";
  ic->format = NULL;  /* no parameters */
  ic->nativetype = IUP_TYPECONTROL;
  ic->childtype = IUP_CHILDNONE;
  ic->is_interactive = 0;

  ic->New = iupClipboardNewClass;

  /* Attribute functions */
  iupClassRegisterAttribute(ic, "TEXT", cocoaClipboardGetTextAttrib, cocoaClipboardSetTextAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "TEXTAVAILABLE", cocoaClipboardGetTextAvailableAttrib, NULL, NULL, NULL, IUPAF_READONLY|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
#if 0

  iupClassRegisterAttribute(ic, "NATIVEIMAGE", gtkClipboardGetNativeImageAttrib, gtkClipboardSetNativeImageAttrib, NULL, NULL, IUPAF_NO_STRING|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "IMAGE", NULL, gtkClipboardSetImageAttrib, NULL, NULL, IUPAF_IHANDLENAME|IUPAF_WRITEONLY|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "IMAGEAVAILABLE", gtkClipboardGetImageAvailableAttrib, NULL, NULL, NULL, IUPAF_READONLY|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "ADDFORMAT", NULL, gtkClipboardSetAddFormatAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "FORMAT", NULL, NULL, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "FORMATAVAILABLE", gtkClipboardGetFormatAvailableAttrib, NULL, NULL, NULL, IUPAF_READONLY|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "FORMATDATA", gtkClipboardGetFormatDataAttrib, gtkClipboardSetFormatDataAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "FORMATDATASIZE", NULL, NULL, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
#endif
	
  return ic;
}
