/** \file
 * \brief Text Control
 *
 * See Copyright Notice in "iup.h"
 */

#import <Cocoa/Cocoa.h>
#import <objc/runtime.h>

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
#include "iup_childtree.h"

#include "iupcocoa_drv.h"
#import "IUPTextSpinnerFilesOwner.h"

// the point of this is we have a unique memory address for an identifier
static const void* IUP_COCOA_TEXT_SPINNERCONTAINER_OBJ_KEY = "IUP_COCOA_TEXT_SPINNERCONTAINER_OBJ_KEY";



typedef enum
{
	IupCocoaTextSubType_UNKNOWN = -1,
	IUPCOCOATEXTSUBTYPE_FIELD,
	IUPCOCOATEXTSUBTYPE_VIEW,
	IUPCOCOATEXTSUBTYPE_STEPPER,
} IupCocoaTextSubType;

/*
 Each IUP list subtype requires a completely different Cocoa native widget.
 This function provides a consistent and centralized way to distinguish which subtype we need.
 */
static IupCocoaTextSubType cocoaTextGetSubType(Ihandle* ih)
{
	if(ih->data->is_multiline)
	{
		return IUPCOCOATEXTSUBTYPE_VIEW;
	}
	else if(iupAttribGetBoolean(ih, "SPIN"))
	{
		return IUPCOCOATEXTSUBTYPE_STEPPER;
	}
	else
	{
		return IUPCOCOATEXTSUBTYPE_FIELD;
	}
	return IupCocoaTextSubType_UNKNOWN;
}

static NSView* cocoaProgressBarGetRootView(Ihandle* ih)
{
	IupCocoaTextSubType sub_type = cocoaTextGetSubType(ih);
	switch(sub_type)
	{
		case IUPCOCOATEXTSUBTYPE_VIEW:
		{
			NSView* root_container_view = (NSView*)ih->handle;
			NSCAssert([root_container_view isKindOfClass:[NSView class]], @"Expected NSView");
			return root_container_view;
		}
		case IUPCOCOATEXTSUBTYPE_FIELD:
		{
			NSView* root_container_view = (NSView*)ih->handle;
			NSCAssert([root_container_view isKindOfClass:[NSView class]], @"Expected NSView");
			return root_container_view;
		}
		case IUPCOCOATEXTSUBTYPE_STEPPER:
		{
			NSStackView* root_container_view = (NSView*)ih->handle;
			NSCAssert([root_container_view isKindOfClass:[NSStackView class]], @"Expected NSStackView");
			return root_container_view;
		}
		default:
		{
			break;
		}
	}
	return nil;
}


static NSTextField* cocoaTextGetTextField(Ihandle* ih)
{
	NSTextField* root_container_view = (NSTextField*)ih->handle;
	NSCAssert([root_container_view isKindOfClass:[NSTextField class]], @"Expected NSTextField");
	return root_container_view;
}

static NSTextView* cocoaTextGetTextView(Ihandle* ih)
{
	NSScrollView* root_container_view = (NSScrollView*)cocoaProgressBarGetRootView(ih);
	NSCAssert([root_container_view isKindOfClass:[NSScrollView class]], @"Expected NSScrollView");
	NSTextView* text_view = [root_container_view documentView];
	NSCAssert([text_view isKindOfClass:[NSTextView class]], @"Expected NSTextView");
	return text_view;
}

static NSStepper* cocoaTextGetStepperView(Ihandle* ih)
{
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
}

static NSTextView* cocoaTextGetStepperTextField(Ihandle* ih)
{
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
}



void iupdrvTextAddSpin(int *w, int h)
{

	
}

void iupdrvTextAddBorders(int *x, int *y)
{
//	*y = *y+10;
//	*x = *x+4;
	// NSTextField is all guess work about how to scale for any font size.
	// Throw away ih->currentheight because it will EXPAND it.
	// But for the standard font, we get fontheight=16 and we want the height to be 22
	int font_height = 16;
//	iupdrvFontGetCharSize(ih, NULL, &font_height);
	const CGFloat HEIGHT_PADDING = 6.0;
	
	*y = font_height + HEIGHT_PADDING;
//	*y = 16;

	const CGFloat WIDTH_PADDING = 12.0;
	
	
	*x = *x - WIDTH_PADDING;

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


static int cocoaTextSetValueAttrib(Ihandle* ih, const char* value)
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
	
	IupCocoaTextSubType sub_type = cocoaTextGetSubType(ih);
	switch(sub_type)
	{
		case IUPCOCOATEXTSUBTYPE_VIEW:
		{
			NSTextView* text_view = cocoaTextGetTextView(ih);
			NSCAssert([text_view isKindOfClass:[NSTextView class]], @"Expected NSTextView");
			
			NSAttributedString* attributed_string = [[NSAttributedString alloc] initWithString:ns_string];
			[[text_view textStorage] setAttributedString:attributed_string];
			[attributed_string release];

			break;
		}
		case IUPCOCOATEXTSUBTYPE_FIELD:
		{
			NSTextField* text_field = cocoaTextGetTextField(ih);
			NSCAssert([text_field isKindOfClass:[NSTextField class]], @"Expected NSTextField");
			[text_field setStringValue:ns_string];

			break;
		}
		case IUPCOCOATEXTSUBTYPE_STEPPER:
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

static char* cocoaTextGetValueAttrib(Ihandle* ih)
{
	char* value;
	
	IupCocoaTextSubType sub_type = cocoaTextGetSubType(ih);
	switch(sub_type)
	{
		case IUPCOCOATEXTSUBTYPE_VIEW:
		{
			NSTextView* text_view = cocoaTextGetTextView(ih);
			NSCAssert([text_view isKindOfClass:[NSTextView class]], @"Expected NSTextView");
			
			NSString* ns_string = [[text_view textStorage] string];
			value = iupStrReturnStr([ns_string UTF8String]);
			
			break;
		}
		case IUPCOCOATEXTSUBTYPE_FIELD:
		{
			NSTextField* text_field = cocoaTextGetTextField(ih);
			NSCAssert([text_field isKindOfClass:[NSTextField class]], @"Expected NSTextField");
			
			NSString* ns_string = [text_field stringValue];
			value = iupStrReturnStr([ns_string UTF8String]);
			
			break;
		}
		case IUPCOCOATEXTSUBTYPE_STEPPER:
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


static int cocoaTextSetCueBannerAttrib(Ihandle *ih, const char *value)
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
	
	
	IupCocoaTextSubType sub_type = cocoaTextGetSubType(ih);
	switch(sub_type)
	{
		case IUPCOCOATEXTSUBTYPE_VIEW:
		{
			
			break;
		}
		case IUPCOCOATEXTSUBTYPE_FIELD:
		{
			NSTextField* text_field = cocoaTextGetTextField(ih);
			NSCAssert([text_field isKindOfClass:[NSTextField class]], @"Expected NSTextField");
			[text_field setPlaceholderString:ns_string];

			return  1;
			
		}
		case IUPCOCOATEXTSUBTYPE_STEPPER:
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

static int cocoaTextSetReadOnlyAttrib(Ihandle* ih, const char* value)
{
	BOOL is_editable = (BOOL)iupStrBoolean(value);
	
	IupCocoaTextSubType sub_type = cocoaTextGetSubType(ih);
	switch(sub_type)
	{
		case IUPCOCOATEXTSUBTYPE_VIEW:
		{
			NSTextView* text_view = cocoaTextGetTextView(ih);
			[text_view setEditable:is_editable];
			
			break;
		}
		case IUPCOCOATEXTSUBTYPE_FIELD:
		{
			NSTextField* text_field = cocoaTextGetTextField(ih);
			[text_field setEditable:is_editable];
			
			break;
		}
		case IUPCOCOATEXTSUBTYPE_STEPPER:
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

static char* cocoaTextGetReadOnlyAttrib(Ihandle* ih)
{
	int is_editable;
	
	IupCocoaTextSubType sub_type = cocoaTextGetSubType(ih);
	switch(sub_type)
	{
		case IUPCOCOATEXTSUBTYPE_VIEW:
		{
			NSTextView* text_view = cocoaTextGetTextView(ih);
			is_editable = [text_view isEditable];
			
			break;
		}
		case IUPCOCOATEXTSUBTYPE_FIELD:
		{
			NSTextField* text_field = cocoaTextGetTextField(ih);
			is_editable = [text_field isEditable];
			
			break;
		}
		case IUPCOCOATEXTSUBTYPE_STEPPER:
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

// Need to override because the position offsets are wrong otherwise.
static void cocoaTextLayoutUpdateMethod(Ihandle* ih)
{
	// This may be called before we have a native parent
	InativeHandle* parent_native_handle_direct = iupChildTreeGetNativeParentHandle(ih);
	NSView* parent_view = nil;
	
	// This may be called before we have a native parent, in which case it returns -1
	if((void*)-1 == parent_native_handle_direct)
	{
		return;
	}
	
	NSObject* parent_native_handle = (NSObject*)parent_native_handle_direct;
	

	
	if([parent_native_handle isKindOfClass:[NSWindow class]])
	{
		NSWindow* parent_window = (NSWindow*)parent_native_handle;
		parent_view = [parent_window contentView];
	}
	/* // Part of NSBox experiment
	 else if([parent_native_handle isKindOfClass:[NSBox class]])
	 {
		NSBox* box_view = (NSBox*)parent_native_handle;
		parent_view = [box_view contentView];
		
		CGFloat diff_width = NSWidth([box_view frame]) - NSWidth([parent_view frame]);
		CGFloat diff_height = NSHeight([box_view frame]) - NSHeight([parent_view frame]);
	 
		current_width = current_width - diff_width;
		current_height = current_height - diff_height;
	 }
	 */
	else if([parent_native_handle isKindOfClass:[NSView class]])
	{
		parent_view = (NSView*)parent_native_handle;
	}
	else
	{
		NSCAssert(1, @"Unexpected type for parent widget");
		@throw @"Unexpected type for parent widget";
	}
	
	NSRect parent_rect = [parent_view frame];

	NSRect the_rect;
	id child_handle = ih->handle;
	NSView* the_view = nil;
	if(!ih->data->is_multiline)
	{
//		the_view = (NSView*)child_handle;
		
//		NSTextField* text_field = (NSTextField*)child_handle;


		CGFloat current_width = (CGFloat)ih->currentwidth;
		CGFloat current_height = (CGFloat)ih->currentheight;
		
		NSLog(@"old ih->currentheight=%d", ih->currentheight);
		CGFloat pos_x = (CGFloat)ih->x;
		// Need to invert y-axis, and also need to shift/account for height of widget because that is also lower-left instead of upper-left.
//		CGFloat pos_y = parent_rect.size.height - (CGFloat)ih->y - (CGFloat)ih->currentheight;
		
		// NSTextField is all guess work about how to scale for any font size.
		// Throw away ih->currentheight because it will EXPAND it.
		// But for the standard font, we get fontheight=16 and we want the height to be 22
		int font_height = 0;
		iupdrvFontGetCharSize(ih, NULL, &font_height);
		const CGFloat HEIGHT_PADDING = 6.0;
		const CGFloat WIDTH_PADDING = 12.0;

		current_height = font_height + HEIGHT_PADDING;

		current_width = current_width - WIDTH_PADDING;

		// Need to invert y-axis, and also need to shift/account for height of widget because that is also lower-left instead of upper-left.
		CGFloat pos_y = parent_rect.size.height - (CGFloat)ih->y - current_height;

		
		pos_x = pos_x + WIDTH_PADDING*0.5;
//		pos_y = pos_y - HEIGHT_PADDING*0.5; // I don't realy understand the logic of this offset, particularly the -1
//		pos_y = pos_y - HEIGHT_PADDING*0.5 - 1; // I don't realy understand the logic of this offset, particularly the -1
//		pos_y = pos_y + HEIGHT_PADDING*0.5 - 1; // I don't realy understand the logic of this offset, particularly the -1
		
		the_rect = NSMakeRect(
									 pos_x,
									 // Need to invert y-axis, and also need to shift/account for height of widget because that is also lower-left instead of upper-left.
									 pos_y,
									 current_width,
									 current_height
									 );
		
//		ih->currentheight = current_height;
//		ih->currentwidth = current_width;

//		ih->x = pos_x;
//		ih->y = pos_y;
		
		// for padding
		// drat, data is private and requires a per-widget header
		{
			
			char* padding_str = iupAttribGet(ih, "PADDING");
			if((NULL != padding_str) && (padding_str[0] != '\0'))
			{
				int horiz_padding = 0;
				int vert_padding = 0;
				iupStrToIntInt(padding_str, &horiz_padding, &vert_padding, 'x');
				
				NSRect old_frame = the_rect;
				NSRect new_frame;
				new_frame.origin.x = old_frame.origin.x + (CGFloat)horiz_padding*0.5;
				new_frame.origin.y = old_frame.origin.y + (CGFloat)vert_padding*0.5;
				new_frame.size.width = old_frame.size.width - (CGFloat)horiz_padding;
				new_frame.size.height = old_frame.size.height - (CGFloat)vert_padding;
				
				the_rect = new_frame;
			}
			
		}
		
	}
	else
	{
//		NSTextView* text_view = (NSTextView*)child_handle;
		
		
		the_rect = NSMakeRect(
									 ih->x,
									 // Need to invert y-axis, and also need to shift/account for height of widget because that is also lower-left instead of upper-left.
									 parent_rect.size.height - ih->y - ih->currentheight,
									 ih->currentwidth,
									 ih->currentheight
									 );
		
		// for padding
		// drat, data is private and requires a per-widget header
		{
			
			char* padding_str = iupAttribGet(ih, "PADDING");
			if((NULL != padding_str) && (padding_str[0] != '\0'))
			{
				int horiz_padding = 0;
				int vert_padding = 0;
				iupStrToIntInt(padding_str, &horiz_padding, &vert_padding, 'x');
				
				NSRect old_frame = the_rect;
				NSRect new_frame;
				new_frame.origin.x = old_frame.origin.x + (CGFloat)horiz_padding*0.5;
				new_frame.origin.y = old_frame.origin.y + (CGFloat)vert_padding*0.5;
				new_frame.size.width = old_frame.size.width - (CGFloat)horiz_padding;
				new_frame.size.height = old_frame.size.height - (CGFloat)vert_padding;
				
				the_rect = new_frame;
			}
			
		}

	}

	
	
	//	iupgtkNativeContainerMove((GtkWidget*)parent, widget, x, y);
	
	//	iupgtkSetPosSize(GTK_CONTAINER(parent), widget, ih->x, ih->y, ih->currentwidth, ih->currentheight);
	
	/*
	 CGSize fitting_size = [the_view fittingSize];
	 ih->currentwidth = fitting_size.width;
	 ih->currentheight = fitting_size.height;
	 */
	
	
#if 0 // experiment to try to handle NSBox directly instead of using cocoaFrameGetInnerNativeContainerHandleMethod. I think cocoaFrameGetInnerNativeContainerHandleMethod is better, but
#else
	

	

	
	
	[the_view setFrame:the_rect];
#endif
	
}

static int cocoaTextMapMethod(Ihandle* ih)
{
	NSView* the_view = nil;
	

	
	if (ih->data->is_multiline)
	{
		// We need to put the textview in a scrollview
		// https://developer.apple.com/library/content/documentation/Cocoa/Conceptual/TextUILayer/Tasks/TextInScrollView.html

		NSScrollView* scroll_view = [[NSScrollView alloc] initWithFrame:NSZeroRect];
		NSSize scrollview_content_size = [scroll_view contentSize];
		
		[scroll_view setBorderType:NSNoBorder];
		[scroll_view setHasVerticalScroller:YES];
		[scroll_view setHasHorizontalScroller:NO];
		[scroll_view setAutoresizingMask:NSViewWidthSizable | NSViewHeightSizable];
		
		
		NSTextView* text_view = [[NSTextView alloc] initWithFrame:NSZeroRect];
//		NSTextView* text_view = [[NSTextView alloc] initWithFrame:NSMakeRect(0, 0, 400, 400)];

		
		text_view = [[NSTextView alloc] initWithFrame:NSMakeRect(0, 0, scrollview_content_size.width, scrollview_content_size.height)];

		[text_view setMinSize:NSMakeSize(0.0, 0.0)];
		[text_view setMaxSize:NSMakeSize(FLT_MAX, FLT_MAX)];
		[text_view setVerticallyResizable:YES];
		[text_view setHorizontallyResizable:NO];
//		[text_view setAutoresizingMask:NSViewWidthSizable];
//		[[text_view textContainer] setContainerSize:NSMakeSize(scrollview_content_size.width, FLT_MAX)];
//		[[text_view textContainer] setWidthTracksTextView:YES];
		
		
		[scroll_view setDocumentView:text_view];
		[text_view release];


		
		
		the_view = scroll_view;
		

		

		int wordwrap = 0;

		
		/* formatting is always supported when MULTILINE=YES */
		ih->data->has_formatting = 1;
		
		if (iupAttribGetBoolean(ih, "WORDWRAP"))
		{
			wordwrap = 1;
			ih->data->sb &= ~IUP_SB_HORIZ;  /* must remove the horizontal scroolbar */
			
			
			[[text_view enclosingScrollView] setHasHorizontalScroller:NO];
			[text_view setHorizontallyResizable:YES];
			[text_view setAutoresizingMask:(NSViewWidthSizable | NSViewHeightSizable)];
			[[text_view textContainer] setContainerSize:NSMakeSize(FLT_MAX, FLT_MAX)];
			[[text_view textContainer] setWidthTracksTextView:YES];

		}
		else
		{
			/*
			NSSize layout_size = [text_view maxSize];
			layout_size.width = layout_size.height;
			[text_view setMaxSize:layout_size];
			[[text_view textContainer] setWidthTracksTextView:NO];
			[[text_view textContainer] setContainerSize:layout_size];
			*/
			
			[[text_view enclosingScrollView] setHasHorizontalScroller:YES];
			[text_view setHorizontallyResizable:NO];
			[text_view setAutoresizingMask:(NSViewWidthSizable | NSViewHeightSizable)];
//			[[text_view textContainer] setContainerSize:NSMakeSize(FLT_MAX, FLT_MAX)];
//			[[text_view textContainer] setWidthTracksTextView:YES];
			
		}
		
	}
	else if(iupAttribGetBoolean(ih, "SPIN"))
	{
		// TODO: NSStepper

#if 0
//		NSView* container_view = [[NSView alloc] initWithFrame:NSZeroRect];
//		NSView* container_view = [[NSView alloc] initWithFrame:NSMakeRect(0, 0, 200, 27)];
		NSStackView* container_view = [[NSStackView alloc] initWithFrame:NSZeroRect];
		[container_view setSpacing:4.0];
		
		NSRect stepper_rect = NSMakeRect(0, 0, 19, 27);
		NSStepper* stepper_view = [[NSStepper alloc] initWithFrame:stepper_rect];
		NSTextField* text_field = [[NSTextField alloc] initWithFrame:NSZeroRect];
//		NSTextField* text_field = [[NSTextField alloc] initWithFrame:NSMakeRect(0, 0, 200-19, 22)];

		
//		[container_view addSubview:text_field];
//		[text_field addSubview:stepper_view];

		[container_view addView:text_field inGravity:NSStackViewGravityLeading];
		[container_view addView:stepper_view inGravity:NSStackViewGravityTrailing];
		[stepper_view release];
		[text_field release];
		
		[container_view setAutoresizingMask:(NSViewWidthSizable | NSViewHeightSizable)];
		[text_field setAutoresizingMask:(NSViewWidthSizable)];
		
		the_view = container_view;

#else

		// The NIB file has setup the NSStackView and also the Cocoa Bindings to connect the textfield and stepper.
		
		NSBundle* framework_bundle = [NSBundle bundleWithIdentifier:@"br.puc-rio.tecgraf.iup"];
		NSNib* text_spinner_nib = [[NSNib alloc] initWithNibNamed:@"IupTextSpinner" bundle:framework_bundle];


		NSArray* top_level_objects = nil;
		IUPTextSpinnerFilesOwner* files_owner = [[IUPTextSpinnerFilesOwner alloc] init];
		IUPTextSpinnerContainer* text_spinner_container = [[IUPTextSpinnerContainer alloc] init];

		// We can either fish out the objects we want (and retain them) in the top_level_objects,
		// or for my convenience, I made a File's Owner object and wired up all the major elements so I can retrive them easily.
		[text_spinner_nib instantiateWithOwner:files_owner topLevelObjects:&top_level_objects];

		
		/*
		Messy: There are multiple objects including an NSObject and NSObjectController in the NIB.
		I originally thought about making a NSObject subclass which holds all the components and make it the ih->handle.
		But a non-NSView as the ih->handle breaks an implicit assumption used by the common core.
		So instead, we'll make the NSStackView the root container object for the ih->handle.
		But we will still make a NSObject subclass that holds the remaining components.
		We will use setAssociatedObject to keep these objects connected, while avoiding the need to subclass NSStackView just to hold this property.
		*/
		NSStackView* stack_view = [files_owner stackView];
		NSStepper* stepper_view = [files_owner stepperView];
		NSTextField* text_field = [files_owner textField];

		[text_spinner_container setStepperView:stepper_view];
		[text_spinner_container setTextField:text_field];
		[text_spinner_container setStepperObject:[files_owner stepperObject]];
		[text_spinner_container setStepperObjectController:[files_owner stepperObjectController]];

		// We're going to use OBJC_ASSOCIATION_RETAIN because I do believe it will do the right thing for us.
		objc_setAssociatedObject(stack_view, IUP_COCOA_TEXT_SPINNERCONTAINER_OBJ_KEY, (id)text_spinner_container, OBJC_ASSOCIATION_RETAIN);

		
		the_view = [stack_view retain];
		
		[text_spinner_container release];
		[files_owner release];
		[top_level_objects release];
		[text_spinner_nib release];
		
		
		
		[stepper_view setValueWraps:(BOOL)iupAttribGetBoolean(ih, "SPINWRAP")];
		
		if (!iupAttribGetBoolean(ih, "SPINAUTO"))
		{
//			g_signal_connect(G_OBJECT(ih->handle), "input", G_CALLBACK(gtkTextSpinInput), ih);
//			iupAttribSet(ih, "_IUPGTK_SPIN_NOAUTO", "1");
		}
#endif
		

		
		

		/*
			gtk_spin_button_set_numeric((GtkSpinButton*)ih->handle, FALSE);
			gtk_spin_button_set_digits((GtkSpinButton*)ih->handle, 0);
			
			gtk_spin_button_set_wrap((GtkSpinButton*)ih->handle, iupAttribGetBoolean(ih, "SPINWRAP"));
			
			g_signal_connect(G_OBJECT(ih->handle), "value-changed", G_CALLBACK(gtkTextSpinValueChanged), ih);
			g_signal_connect(G_OBJECT(ih->handle), "output", G_CALLBACK(gtkTextSpinOutput), ih);
			
			if (!iupAttribGetBoolean(ih, "SPINAUTO"))
			{
		 g_signal_connect(G_OBJECT(ih->handle), "input", G_CALLBACK(gtkTextSpinInput), ih);
		 iupAttribSet(ih, "_IUPGTK_SPIN_NOAUTO", "1");
			}
		 */
		/* formatting is never supported when MULTILINE=NO */
		ih->data->has_formatting = 0;
		
		// We must not allow IUP to EXPAND the height of the NSTextField so unset the bit flag if it is set.
		ih->expand = ih->expand & ~IUP_EXPAND_HEIGHT;
		
	}
	else
	{
		NSTextField* text_field;
		
		
		// IMPORTANT: Secure text fields are not togglable in Cocoa
		// It might be fakeable, however, since this is security related, mucking with it is ill-advised.
		// Also Mac App Store may reject ill-advised things.
		if(iupAttribGetBoolean(ih, "PASSWORD"))
		{

			text_field = [[NSSecureTextField alloc] initWithFrame:NSZeroRect];
			//text_field = [[NSSecureTextField alloc] initWithFrame:NSMakeRect(0, 0, 140, 40)];
			[text_field setFont:[NSFont systemFontOfSize:0.0]];

		}
		else
		{
			
			//text_field = [[NSTextField alloc] initWithFrame:NSZeroRect];
			text_field = [[NSTextField alloc] initWithFrame:NSMakeRect(50, 50, 140, 40)];
			[text_field setFont:[NSFont systemFontOfSize:0.0]];

			
		}
		
		// We must not allow IUP to EXPAND the height of the NSTextField so unset the bit flag if it is set.
		ih->expand = ih->expand & ~IUP_EXPAND_HEIGHT;
		
		the_view = text_field;


		
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
	// This is generally not good Cocoa as Toggles don't retain their receivers, but this seems like the best option.
	// Be careful of retain cycles.
	IupCocoaToggleReceiver* toggle_receiver = [[IupCocoaToggleReceiver alloc] init];
	[the_toggle setTarget:toggle_receiver];
	[the_toggle setAction:@selector(myToggleClickAction:)];
	// I *think* is we use RETAIN, the object will be released automatically when the Toggle is freed.
	// However, the fact that this is tricky and I had to look up the rules (not to mention worrying about retain cycles)
	// makes me think I should just explicitly manage the memory so everybody is aware of what's going on.
	objc_setAssociatedObject(the_toggle, IUP_COCOA_TOGGLE_RECEIVER_OBJ_KEY, (id)toggle_receiver, OBJC_ASSOCIATION_ASSIGN);
	
#endif
	// All Cocoa views shoud call this to add the new view to the parent view.
	iupCocoaAddToParent(ih);
	
	


#if 0
	/* configure for DRAG&DROP */
	if (IupGetCallback(ih, "DROPFILES_CB"))
		iupAttribSet(ih, "DROPFILESTARGET", "YES");
	
	/* update a mnemonic in a label if necessary */
	iupgtkUpdateMnemonic(ih);
	
	if (ih->data->formattags)
		iupTextUpdateFormatTags(ih);
#endif
		
	
	return IUP_NOERROR;
}


static void cocoaTextUnMapMethod(Ihandle* ih)
{
	id the_view = ih->handle;
	/*
	id text_receiver = objc_getAssociatedObject(the_view, IUP_COCOA_TOGGLE_RECEIVER_OBJ_KEY);
	objc_setAssociatedObject(the_view, IUP_COCOA_TOGGLE_RECEIVER_OBJ_KEY, nil, OBJC_ASSOCIATION_ASSIGN);
	[text_receiver release];
	*/
	iupCocoaRemoveFromParent(ih);
	[the_view release];
	ih->handle = NULL;
	
}


static void cocoaTextComputeNaturalSizeMethod(Ihandle* ih, int *w, int *h, int *children_expand)
{
	int natural_w = 0,
	natural_h = 0,
	visiblecolumns = iupAttribGetInt(ih, "VISIBLECOLUMNS"),
	visiblelines = iupAttribGetInt(ih, "VISIBLELINES");
	(void)children_expand; /* unset if not a container */
	
	/* Since the contents can be changed by the user, the size can not be dependent on it. */
	iupdrvFontGetCharSize(ih, NULL, &natural_h);  /* one line height */
	natural_w = iupdrvFontGetStringWidth(ih, "WWWWWWWWWW");
	natural_w = (visiblecolumns*natural_w)/10;
	if (ih->data->is_multiline)
	{
		natural_h = visiblelines*natural_h;
	}
	else
	{
		natural_h = 22;
	}
	
	/* compute the borders space */
	if (iupAttribGetBoolean(ih, "BORDER"))
		iupdrvTextAddBorders(&natural_w, &natural_h);
	
	if (iupAttribGetBoolean(ih, "SPIN"))
		iupdrvTextAddSpin(&natural_w, natural_h);
	
	natural_w += 2*ih->data->horiz_padding;
	natural_h += 2*ih->data->vert_padding;
	
	/* add scrollbar */
	if (ih->data->is_multiline && ih->data->sb)
	{
		int sb_size = iupdrvGetScrollbarSize();
		if (ih->data->sb & IUP_SB_HORIZ)
			natural_h += sb_size;  /* sb horizontal affects vertical size */
		if (ih->data->sb & IUP_SB_VERT)
			natural_w += sb_size;  /* sb vertical affects horizontal size */
	}
	
	*w = natural_w;
	*h = natural_h;
	
	Ihandle* child;

	for (child = ih->firstchild; child; child = child->brother)
	{

		
		/* update child natural size first */
		if (!(child->flags & IUP_FLOATING_IGNORE))
			iupBaseComputeNaturalSize(child);
	}
	
}


void iupdrvTextInitClass(Iclass* ic)
{
  /* Driver Dependent Class functions */
  ic->Map = cocoaTextMapMethod;
	ic->UnMap = cocoaTextUnMapMethod;

//	ic->LayoutUpdate = cocoaTextLayoutUpdateMethod;
//	ic->ComputeNaturalSize = cocoaTextComputeNaturalSizeMethod;

#if 0

  /* Driver Dependent Attribute functions */

  /* Visual */
  iupClassRegisterAttribute(ic, "BGCOLOR", NULL, gtkTextSetBgColorAttrib, IUPAF_SAMEASSYSTEM, "TXTBGCOLOR", IUPAF_DEFAULT);

  /* Special */
  iupClassRegisterAttribute(ic, "FGCOLOR", NULL, iupdrvBaseSetFgColorAttrib, IUPAF_SAMEASSYSTEM, "TXTFGCOLOR", IUPAF_DEFAULT);

  /* IupText only */
  iupClassRegisterAttribute(ic, "PADDING", iupTextGetPaddingAttrib, gtkTextSetPaddingAttrib, IUPAF_SAMEASSYSTEM, "0x0", IUPAF_NOT_MAPPED);
#endif
  iupClassRegisterAttribute(ic, "VALUE", cocoaTextGetValueAttrib, cocoaTextSetValueAttrib, NULL, NULL, IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);
#if 0
  iupClassRegisterAttribute(ic, "LINEVALUE", gtkTextGetLineValueAttrib, NULL, NULL, NULL, IUPAF_READONLY|IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SELECTEDTEXT", gtkTextGetSelectedTextAttrib, gtkTextSetSelectedTextAttrib, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SELECTION", gtkTextGetSelectionAttrib, gtkTextSetSelectionAttrib, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SELECTIONPOS", gtkTextGetSelectionPosAttrib, gtkTextSetSelectionPosAttrib, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "CARET", gtkTextGetCaretAttrib, gtkTextSetCaretAttrib, NULL, NULL, IUPAF_NO_SAVE|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "CARETPOS", gtkTextGetCaretPosAttrib, gtkTextSetCaretPosAttrib, IUPAF_SAMEASSYSTEM, "0", IUPAF_NO_SAVE|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "INSERT", NULL, gtkTextSetInsertAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "APPEND", NULL, gtkTextSetAppendAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
#endif
	
  iupClassRegisterAttribute(ic, "READONLY", cocoaTextGetReadOnlyAttrib, cocoaTextSetReadOnlyAttrib, NULL, NULL, IUPAF_DEFAULT);
#if 0
  iupClassRegisterAttribute(ic, "NC", iupTextGetNCAttrib, gtkTextSetNCAttrib, IUPAF_SAMEASSYSTEM, "0", IUPAF_NOT_MAPPED);
  iupClassRegisterAttribute(ic, "CLIPBOARD", NULL, gtkTextSetClipboardAttrib, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SCROLLTO", NULL, gtkTextSetScrollToAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SCROLLTOPOS", NULL, gtkTextSetScrollToPosAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SPINMIN", NULL, gtkTextSetSpinMinAttrib, IUPAF_SAMEASSYSTEM, "0", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SPINMAX", NULL, gtkTextSetSpinMaxAttrib, IUPAF_SAMEASSYSTEM, "100", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SPININC", NULL, gtkTextSetSpinIncAttrib, IUPAF_SAMEASSYSTEM, "1", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SPINVALUE", gtkTextGetSpinValueAttrib, gtkTextSetSpinValueAttrib, IUPAF_SAMEASSYSTEM, "0", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "COUNT", gtkTextGetCountAttrib, NULL, NULL, NULL, IUPAF_READONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "LINECOUNT", gtkTextGetLineCountAttrib, NULL, NULL, NULL, IUPAF_READONLY|IUPAF_NO_INHERIT);

  /* IupText Windows and GTK only */
  iupClassRegisterAttribute(ic, "ADDFORMATTAG", NULL, iupTextSetAddFormatTagAttrib, NULL, NULL, IUPAF_IHANDLENAME|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "ADDFORMATTAG_HANDLE", NULL, iupTextSetAddFormatTagHandleAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "ALIGNMENT", NULL, gtkTextSetAlignmentAttrib, IUPAF_SAMEASSYSTEM, "ALEFT", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "FORMATTING", iupTextGetFormattingAttrib, iupTextSetFormattingAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "OVERWRITE", gtkTextGetOverwriteAttrib, gtkTextSetOverwriteAttrib, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "REMOVEFORMATTING", NULL, gtkTextSetRemoveFormattingAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "TABSIZE", NULL, gtkTextSetTabSizeAttrib, "8", NULL, IUPAF_DEFAULT);  /* force new default value */
#endif
  iupClassRegisterAttribute(ic, "PASSWORD", NULL, NULL, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "CUEBANNER", NULL, cocoaTextSetCueBannerAttrib, NULL, NULL, IUPAF_NO_INHERIT);

#if 0
  /* Not Supported */
  iupClassRegisterAttribute(ic, "FILTER", NULL, NULL, NULL, NULL, IUPAF_NOT_SUPPORTED|IUPAF_NO_INHERIT);
#endif
	
}
