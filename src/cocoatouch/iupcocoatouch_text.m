/** \file
 * \brief Text Control
 *
 * See Copyright Notice in "iup.h"
 */

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

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
#include "iup_mask.h"
#include "iup_drv.h"
#include "iup_drvfont.h"
#include "iup_image.h"
#include "iup_key.h"
#include "iup_array.h"
#include "iup_text.h"

#include "iupcocoatouch_drv.h"




typedef enum
{
	IUPCOCOATOUCHTEXTSUBTYPE_UNKNOWN = -1,
	IUPCOCOATOUCHTEXTSUBTYPE_FIELD,
	IUPCOCOATOUCHTEXTSUBTYPE_VIEW,
	IUPCOCOATOUCHTEXTSUBTYPE_STEPPER,
} IupCocoaTouchTextSubType;

/*
 Each IUP list subtype requires a completely different Cocoa native widget.
 This function provides a consistent and centralized way to distinguish which subtype we need.
 */
static IupCocoaTouchTextSubType cocoaTouchTextGetSubType(Ihandle* ih)
{
	if(ih->data->is_multiline)
	{
		return IUPCOCOATOUCHTEXTSUBTYPE_VIEW;
	}
	else if(iupAttribGetBoolean(ih, "SPIN"))
	{
		return IUPCOCOATOUCHTEXTSUBTYPE_STEPPER;
	}
	else
	{
		return IUPCOCOATOUCHTEXTSUBTYPE_FIELD;
	}
	return IUPCOCOATOUCHTEXTSUBTYPE_UNKNOWN;
}

static UIView* cocoaTouchTextGetRootView(Ihandle* ih)
{
	IupCocoaTouchTextSubType sub_type = cocoaTouchTextGetSubType(ih);
	switch(sub_type)
	{
		case IUPCOCOATOUCHTEXTSUBTYPE_VIEW:
		{
			UIView* root_container_view = (UIView*)ih->handle;
			NSCAssert([root_container_view isKindOfClass:[UIView class]], @"Expected UIView");
			return root_container_view;
		}
		case IUPCOCOATOUCHTEXTSUBTYPE_FIELD:
		{
			UIView* root_container_view = (UIView*)ih->handle;
			NSCAssert([root_container_view isKindOfClass:[UIView class]], @"Expected UIView");
			return root_container_view;
		}
		case IUPCOCOATOUCHTEXTSUBTYPE_STEPPER:
		{
			UIStackView* root_container_view = (UIStackView*)ih->handle;
			NSCAssert([root_container_view isKindOfClass:[UIStackView class]], @"Expected UIStackView");
			return root_container_view;
		}
		default:
		{
			break;
		}
	}
	return nil;
}


static UITextField* cocoaTouchTextGetTextField(Ihandle* ih)
{
	/*
	UIStackView* root_container_view = (UIStackView*)ih->handle;
	UITextField* text_field = [[root_container_view views] firstObject];
	NSCAssert([text_field isKindOfClass:[UITextField class]], @"Expected UITextField");
	return text_field;
*/
	UITextField* text_field = (UITextField*)ih->handle;
	NSCAssert([text_field isKindOfClass:[UITextField class]], @"Expected UITextField");
	return text_field;
}

static UITextView* cocoaTouchTextGetTextView(Ihandle* ih)
{
	/*
	UIScrollView* root_container_view = (UIScrollView*)cocoaTextGetRootView(ih);
	NSCAssert([root_container_view isKindOfClass:[UIScrollView class]], @"Expected UIScrollView");
	UITextView* text_view = [root_container_view documentView];
	NSCAssert([text_view isKindOfClass:[UITextView class]], @"Expected UITextView");
	return text_view;
	 */
	UITextView* text_view = (UITextView*)ih->handle;
	NSCAssert([text_view isKindOfClass:[UITextView class]], @"Expected UITextView");
	return text_view;
}

static UIStepper* cocoaTouchTextGetStepperView(Ihandle* ih)
{
/*
#if 0
	NSStackView* root_container_view = (NSStackView*)ih->handle;
	NSStepper* stepper_view = [[root_container_view views] lastObject];
	NSCAssert([stepper_view isKindOfClass:[NSStepper class]], @"Expected NSStepper");
	return stepper_view;
#else
	IUPTextSpinnerContainer* spinner_container = (IUPTextSpinnerContainer*)objc_getAssociatedObject((id)ih->handle, IUP_COCOA_TEXT_SPINNERCONTAINER_OBJ_KEY);
	NSCAssert([spinner_container isKindOfClass:[IUPTextSpinnerFilesOwner class]], @"Expected IUPTextSpinnerFilesOwner");
	return [spinner_container stepperView];
#endif
 */
	return nil;
}

static UITextView* cocoaTouchTextGetStepperTextField(Ihandle* ih)
{
	/*
#if 0
	NSStackView* root_container_view = (NSStackView*)ih->handle;
	NSTextField* text_field = [[root_container_view views] firstObject];
	NSCAssert([text_field isKindOfClass:[NSTextField class]], @"Expected NSTextField");
	return text_field;
#else
	IUPTextSpinnerContainer* spinner_container = (IUPTextSpinnerContainer*)objc_getAssociatedObject((id)ih->handle, IUP_COCOA_TEXT_SPINNERCONTAINER_OBJ_KEY);
	NSCAssert([spinner_container isKindOfClass:[IUPTextSpinnerFilesOwner class]], @"Expected IUPTextSpinnerFilesOwner");
	return [spinner_container textField];
#endif
	 */
	return nil;

}
/*

static IUPStepperObject* cocoaTouchTextGetStepperObject(Ihandle* ih)
{
	IUPTextSpinnerContainer* spinner_container = (IUPTextSpinnerContainer*)objc_getAssociatedObject((id)ih->handle, IUP_COCOA_TEXT_SPINNERCONTAINER_OBJ_KEY);
	NSCAssert([spinner_container isKindOfClass:[IUPTextSpinnerContainer class]], @"Expected IUPTextSpinnerContainer");
	return [spinner_container stepperObject];
}
 */



void iupdrvTextAddSpin(Ihandle* ih, int *w, int h)
{

	
}

void iupdrvTextAddBorders(Ihandle* ih, int *x, int *y)
{

	
}


void iupdrvTextConvertLinColToPos(Ihandle* ih, int lin, int col, int *pos)
{
 
}

void iupdrvTextConvertPosToLinCol(Ihandle* ih, int pos, int *lin, int *col)
{

	
}



void* iupdrvTextAddFormatTagStartBulk(Ihandle* ih)
{
	
	
  return NULL;
}

void iupdrvTextAddFormatTagStopBulk(Ihandle* ih, void* state)
{

	
}

void iupdrvTextAddFormatTag(Ihandle* ih, Ihandle* formattag, int bulk)
{

	
}


static int cocoaTouchTextSetValueAttrib(Ihandle* ih, const char* value)
{
	NSString* ns_string;
	
	if(NULL == value)
	{
		ns_string = @"";
	}
	else
	{
		ns_string = [NSString stringWithUTF8String:value];
	}
	
	IupCocoaTouchTextSubType sub_type = cocoaTouchTextGetSubType(ih);
	switch(sub_type)
	{
		case IUPCOCOATOUCHTEXTSUBTYPE_VIEW:
		{
			UITextView* text_view = cocoaTouchTextGetTextView(ih);
			NSCAssert([text_view isKindOfClass:[UITextView class]], @"Expected UITextView");
			
			NSAttributedString* attributed_string = [[NSAttributedString alloc] initWithString:ns_string];
			[text_view setAttributedText:attributed_string];
			[attributed_string release];

			break;
		}
		case IUPCOCOATOUCHTEXTSUBTYPE_FIELD:
		{
			UITextField* text_field = cocoaTouchTextGetTextField(ih);
			NSCAssert([text_field isKindOfClass:[UITextField class]], @"Expected UITextField");
			[text_field setText:ns_string];

			break;
		}
		case IUPCOCOATOUCHTEXTSUBTYPE_STEPPER:
		{
			break;
		}
		default:
		{
			break;
		}
	}
	
	return 0;
}

static char* cocoaTouchTextGetValueAttrib(Ihandle* ih)
{
	char* value = NULL;
	
	IupCocoaTouchTextSubType sub_type = cocoaTouchTextGetSubType(ih);
	switch(sub_type)
	{
		case IUPCOCOATOUCHTEXTSUBTYPE_VIEW:
		{
			UITextView* text_view = cocoaTouchTextGetTextView(ih);
			NSCAssert([text_view isKindOfClass:[UITextView class]], @"Expected UITextView");
			
			NSString* ns_string = [text_view text];
			value = iupStrReturnStr([ns_string UTF8String]);
			
			break;
		}
		case IUPCOCOATOUCHTEXTSUBTYPE_FIELD:
		{
			UITextField* text_field = cocoaTouchTextGetTextField(ih);
			NSCAssert([text_field isKindOfClass:[UITextField class]], @"Expected UITextField");
			
			NSString* ns_string = [text_field text];
			value = iupStrReturnStr([ns_string UTF8String]);
			
			break;
		}
		case IUPCOCOATOUCHTEXTSUBTYPE_STEPPER:
		{
			break;
		}
		default:
		{
			break;
		}
	}
	
	if(NULL == value)
	{
		value = "";
	}
	
	return value;
}


static int cocoaTouchTextSetCueBannerAttrib(Ihandle *ih, const char *value)
{
	NSString* ns_string;
	
	if(NULL == value)
	{
		ns_string = @"";
	}
	else
	{
		ns_string = [NSString stringWithUTF8String:value];
	}
	
	
	IupCocoaTouchTextSubType sub_type = cocoaTouchTextGetSubType(ih);
	switch(sub_type)
	{
		case IUPCOCOATOUCHTEXTSUBTYPE_VIEW:
		{
			
			break;
		}
		case IUPCOCOATOUCHTEXTSUBTYPE_FIELD:
		{
			UITextField* text_field = cocoaTouchTextGetTextField(ih);
			NSCAssert([text_field isKindOfClass:[UITextField class]], @"Expected UITextField");
			[text_field setPlaceholder:ns_string];

			return  1;
			
		}
		case IUPCOCOATOUCHTEXTSUBTYPE_STEPPER:
		{
			break;
		}
		default:
		{
			break;
		}
	}
	

	return 0;
}

static int cocoaTouchTextSetReadOnlyAttrib(Ihandle* ih, const char* value)
{
	BOOL is_editable = !(BOOL)iupStrBoolean(value);
	
	IupCocoaTouchTextSubType sub_type = cocoaTouchTextGetSubType(ih);
	switch(sub_type)
	{
		case IUPCOCOATOUCHTEXTSUBTYPE_VIEW:
		{
			UITextView* text_view = cocoaTouchTextGetTextView(ih);
			[text_view setEditable:is_editable];
			
			break;
		}
		case IUPCOCOATOUCHTEXTSUBTYPE_FIELD:
		{
			UITextField* text_field = cocoaTouchTextGetTextField(ih);
			[text_field setEnabled:is_editable];
			
			break;
		}
		case IUPCOCOATOUCHTEXTSUBTYPE_STEPPER:
		{
			break;
		}
		default:
		{
			break;
		}
	}
	
	return 0;
}

static char* cocoaTouchTextGetReadOnlyAttrib(Ihandle* ih)
{
	int is_editable = 1;
	
	IupCocoaTouchTextSubType sub_type = cocoaTouchTextGetSubType(ih);
	switch(sub_type)
	{
		case IUPCOCOATOUCHTEXTSUBTYPE_VIEW:
		{
			UITextView* text_view = cocoaTouchTextGetTextView(ih);
			is_editable = [text_view isEditable];
			
			break;
		}
		case IUPCOCOATOUCHTEXTSUBTYPE_FIELD:
		{
			UITextField* text_field = cocoaTouchTextGetTextField(ih);
			is_editable = [text_field isEnabled];
			
			break;
		}
		case IUPCOCOATOUCHTEXTSUBTYPE_STEPPER:
		{
			break;
		}
		default:
		{
			break;
		}
	}

	return iupStrReturnBoolean(!is_editable);
}



static int cocoaTouchTextMapMethod(Ihandle* ih)
{
#if 1
	UIView* the_view;
	

	
	if (ih->data->is_multiline)
	{
//		NSTextView* text_view = [[NSTextView alloc] initWithFrame:NSZeroRect];
		UITextView* text_view = [[UITextView alloc] initWithFrame:CGRectMake(0, 0, 400, 400)];
		the_view = text_view;
		

		int wordwrap = 0;
		


		
		/* formatting is always supported when MULTILINE=YES */
		ih->data->has_formatting = 1;
		
		if (iupAttribGetBoolean(ih, "WORDWRAP"))
		{
			wordwrap = 1;
			ih->data->sb &= ~IUP_SB_HORIZ;  /* must remove the horizontal scroolbar */
			
			

		}
		else
		{
			/*
			CGSize layout_size = [text_view maxSize];
			layout_size.width = layout_size.height;
			[text_view setMaxSize:layout_size];
			[[text_view textContainer] setWidthTracksTextView:NO];
			[[text_view textContainer] setContainerSize:layout_size];
			*/
			
		}
		
	}
	else
	{
		UITextField* text_field;
		
		text_field = [[UITextField alloc] initWithFrame:CGRectMake(0, 0, 0, 0)];

		// IMPORTANT: Secure text fields are not togglable in cocoaTouch
		// It might be fakeable, however, since this is security related, mucking with it is ill-advised.
		// Also Mac App Store may reject ill-advised things.
		if(iupAttribGetBoolean(ih, "PASSWORD"))
		{

			[text_field setSecureTextEntry:YES];
		}

		the_view = text_field;


//		[text_field setPlaceholderString:@"Placeholder Text"];
		
		if(iupAttribGetBoolean(ih, "SPIN"))
		{
			// TODO: NSStepper
			
			/*
			cocoaTouch_spin_button_set_numeric((cocoaTouchSpinButton*)ih->handle, FALSE);
			cocoaTouch_spin_button_set_digits((cocoaTouchSpinButton*)ih->handle, 0);
			
			cocoaTouch_spin_button_set_wrap((cocoaTouchSpinButton*)ih->handle, iupAttribGetBoolean(ih, "SPINWRAP"));
			
			g_signal_connect(G_OBJECT(ih->handle), "value-changed", G_CALLBACK(cocoaTouchTextSpinValueChanged), ih);
			g_signal_connect(G_OBJECT(ih->handle), "output", G_CALLBACK(cocoaTouchTextSpinOutput), ih);
			
			if (!iupAttribGetBoolean(ih, "SPINAUTO"))
			{
				g_signal_connect(G_OBJECT(ih->handle), "input", G_CALLBACK(cocoaTouchTextSpinInput), ih);
				iupAttribSet(ih, "_IUPcocoaTouch_SPIN_NOAUTO", "1");
			}
			 */
		}
		else
		{
			
		}
		

		/* formatting is never supported when MULTILINE=NO */
		ih->data->has_formatting = 0;
		
		
//		[text_field sizeToFit];


	}
	
	
	
	
	
	
	ih->handle = the_view;
	
#if 0
	// I'm using objc_setAssociatedObject/objc_getAssociatedObject because it allows me to avoid making subclasses just to hold ivars.
	objc_setAssociatedObject(the_toggle, IHANDLE_ASSOCIATED_OBJ_KEY, (id)ih, OBJC_ASSOCIATION_ASSIGN);
	// I also need to track the memory of the buttion action receiver.
	// I prefer to keep the Ihandle the actual NSView instead of the receiver because it makes the rest of the implementation easier if the handle is always an NSView (or very small set of things, e.g. NSWindow, NSView, CALayer).
	// So with only one pointer to deal with, this means we need our Toggle to hold a reference to the receiver object.
	// This is generally not good cocoaTouch as Toggles don't retain their receivers, but this seems like the best option.
	// Be careful of retain cycles.
	IupcocoaTouchToggleReceiver* toggle_receiver = [[IupcocoaTouchToggleReceiver alloc] init];
	[the_toggle setTarget:toggle_receiver];
	[the_toggle setAction:@selector(myToggleClickAction:)];
	// I *think* is we use RETAIN, the object will be released automatically when the Toggle is freed.
	// However, the fact that this is tricky and I had to look up the rules (not to mention worrying about retain cycles)
	// makes me think I should just explicitly manage the memory so everybody is aware of what's going on.
	objc_setAssociatedObject(the_toggle, IUP_cocoaTouch_TOGGLE_RECEIVER_OBJ_KEY, (id)toggle_receiver, OBJC_ASSOCIATION_ASSIGN);
	
#endif
	// All cocoaTouch views shoud call this to add the new view to the parent view.
	iupCocoaTouchAddToParent(ih);
	
	


#if 0
	/* configure for DRAG&DROP */
	if (IupGetCallback(ih, "DROPFILES_CB"))
		iupAttribSet(ih, "DROPFILESTARGET", "YES");
	
	/* update a mnemonic in a label if necessary */
	iupcocoaTouchUpdateMnemonic(ih);
	
	if (ih->data->formattags)
		iupTextUpdateFormatTags(ih);
#endif
	
#endif
	
	return IUP_NOERROR;
}


static void cocoaTouchTextUnMapMethod(Ihandle* ih)
{
	id the_view = ih->handle;
	/*
	id text_receiver = objc_getAssociatedObject(the_view, IUP_cocoaTouch_TOGGLE_RECEIVER_OBJ_KEY);
	objc_setAssociatedObject(the_view, IUP_cocoaTouch_TOGGLE_RECEIVER_OBJ_KEY, nil, OBJC_ASSOCIATION_ASSIGN);
	[text_receiver release];
	*/
	[the_view release];
	ih->handle = NULL;
	
}


void iupdrvTextInitClass(Iclass* ic)
{
  /* Driver Dependent Class functions */
  ic->Map = cocoaTouchTextMapMethod;
	ic->UnMap = cocoaTouchTextUnMapMethod;

#if 0

  /* Driver Dependent Attribute functions */

  /* Visual */
  iupClassRegisterAttribute(ic, "BGCOLOR", NULL, cocoaTouchTextSetBgColorAttrib, IUPAF_SAMEASSYSTEM, "TXTBGCOLOR", IUPAF_DEFAULT);

  /* Special */
  iupClassRegisterAttribute(ic, "FGCOLOR", NULL, iupdrvBaseSetFgColorAttrib, IUPAF_SAMEASSYSTEM, "TXTFGCOLOR", IUPAF_DEFAULT);

  /* IupText only */
  iupClassRegisterAttribute(ic, "PADDING", iupTextGetPaddingAttrib, cocoaTouchTextSetPaddingAttrib, IUPAF_SAMEASSYSTEM, "0x0", IUPAF_NOT_MAPPED);
#endif
  iupClassRegisterAttribute(ic, "VALUE", cocoaTouchTextGetValueAttrib, cocoaTouchTextSetValueAttrib, NULL, NULL, IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);
#if 0
  iupClassRegisterAttribute(ic, "LINEVALUE", cocoaTouchTextGetLineValueAttrib, NULL, NULL, NULL, IUPAF_READONLY|IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SELECTEDTEXT", cocoaTouchTextGetSelectedTextAttrib, cocoaTouchTextSetSelectedTextAttrib, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SELECTION", cocoaTouchTextGetSelectionAttrib, cocoaTouchTextSetSelectionAttrib, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SELECTIONPOS", cocoaTouchTextGetSelectionPosAttrib, cocoaTouchTextSetSelectionPosAttrib, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "CARET", cocoaTouchTextGetCaretAttrib, cocoaTouchTextSetCaretAttrib, NULL, NULL, IUPAF_NO_SAVE|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "CARETPOS", cocoaTouchTextGetCaretPosAttrib, cocoaTouchTextSetCaretPosAttrib, IUPAF_SAMEASSYSTEM, "0", IUPAF_NO_SAVE|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "INSERT", NULL, cocoaTouchTextSetInsertAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "APPEND", NULL, cocoaTouchTextSetAppendAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
#endif
  iupClassRegisterAttribute(ic, "READONLY", cocoaTouchTextGetReadOnlyAttrib, cocoaTouchTextSetReadOnlyAttrib, NULL, NULL, IUPAF_DEFAULT);
#if 0
  iupClassRegisterAttribute(ic, "NC", iupTextGetNCAttrib, cocoaTouchTextSetNCAttrib, IUPAF_SAMEASSYSTEM, "0", IUPAF_NOT_MAPPED);
  iupClassRegisterAttribute(ic, "CLIPBOARD", NULL, cocoaTouchTextSetClipboardAttrib, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SCROLLTO", NULL, cocoaTouchTextSetScrollToAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SCROLLTOPOS", NULL, cocoaTouchTextSetScrollToPosAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SPINMIN", NULL, cocoaTouchTextSetSpinMinAttrib, IUPAF_SAMEASSYSTEM, "0", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SPINMAX", NULL, cocoaTouchTextSetSpinMaxAttrib, IUPAF_SAMEASSYSTEM, "100", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SPININC", NULL, cocoaTouchTextSetSpinIncAttrib, IUPAF_SAMEASSYSTEM, "1", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SPINVALUE", cocoaTouchTextGetSpinValueAttrib, cocoaTouchTextSetSpinValueAttrib, IUPAF_SAMEASSYSTEM, "0", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "COUNT", cocoaTouchTextGetCountAttrib, NULL, NULL, NULL, IUPAF_READONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "LINECOUNT", cocoaTouchTextGetLineCountAttrib, NULL, NULL, NULL, IUPAF_READONLY|IUPAF_NO_INHERIT);

  /* IupText Windows and cocoaTouch only */
  iupClassRegisterAttribute(ic, "ADDFORMATTAG", NULL, iupTextSetAddFormatTagAttrib, NULL, NULL, IUPAF_IHANDLENAME|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "ADDFORMATTAG_HANDLE", NULL, iupTextSetAddFormatTagHandleAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "ALIGNMENT", NULL, cocoaTouchTextSetAlignmentAttrib, IUPAF_SAMEASSYSTEM, "ALEFT", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "FORMATTING", iupTextGetFormattingAttrib, iupTextSetFormattingAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "OVERWRITE", cocoaTouchTextGetOverwriteAttrib, cocoaTouchTextSetOverwriteAttrib, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "REMOVEFORMATTING", NULL, cocoaTouchTextSetRemoveFormattingAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "TABSIZE", NULL, cocoaTouchTextSetTabSizeAttrib, "8", NULL, IUPAF_DEFAULT);  /* force new default value */
  iupClassRegisterAttribute(ic, "PASSWORD", NULL, NULL, NULL, NULL, IUPAF_NO_INHERIT);
#endif
	iupClassRegisterAttribute(ic, "CUEBANNER", NULL, cocoaTouchTextSetCueBannerAttrib, NULL, NULL, IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);
#if 0

	/* Not Supported */
  iupClassRegisterAttribute(ic, "FILTER", NULL, NULL, NULL, NULL, IUPAF_NOT_SUPPORTED|IUPAF_NO_INHERIT);
#endif
	
}
