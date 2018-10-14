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
static const void* IUP_COCOA_TEXT_DELEGATE_OBJ_KEY = "IUP_COCOA_TEXT_DELEGATE_OBJ_KEY";


#if __APPLE__
	#define USE_NSSTACKVIEW_TEXTFIELD_CONTAINER 1
#else // Intended for GNUStep because it lacks NSStackView
	// USE_CONTAINERVIEW_TEXTFIELD_CONTAINER was intended to try to build an equivalent to the NSStackView approach using the old autoresizingMask, but I can't get it to work correctly in the complicated IupGrid situtation.
	// The default fallback case is actually better than this container approach.
//	#define USE_CONTAINERVIEW_TEXTFIELD_CONTAINER 1
#endif



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

static NSView* cocoaTextGetRootView(Ihandle* ih)
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
#ifdef USE_NSSTACKVIEW_TEXTFIELD_CONTAINER
	NSStackView* root_container_view = (NSStackView*)ih->handle;
	NSTextField* text_field = [[root_container_view views] firstObject];
	NSCAssert([text_field isKindOfClass:[NSTextField class]], @"Expected NSTextField");
	return text_field;
#elif defined(USE_CONTAINERVIEW_TEXTFIELD_CONTAINER)
	NSView* root_container_view = (NSView*)ih->handle;
	NSTextField* text_field = [[root_container_view subviews] firstObject];
	NSCAssert([text_field isKindOfClass:[NSTextField class]], @"Expected NSTextField");
	return text_field;
#else
	NSTextField* root_container_view = (NSTextField*)ih->handle;
	NSCAssert([root_container_view isKindOfClass:[NSTextField class]], @"Expected NSTextField");
	return root_container_view;
#endif

}

static NSTextView* cocoaTextGetTextView(Ihandle* ih)
{
	NSScrollView* root_container_view = (NSScrollView*)cocoaTextGetRootView(ih);
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

static IUPStepperObject* cocoaTextGetStepperObject(Ihandle* ih)
{

	IUPTextSpinnerContainer* spinner_container = (IUPTextSpinnerContainer*)objc_getAssociatedObject((id)ih->handle, IUP_COCOA_TEXT_SPINNERCONTAINER_OBJ_KEY);
	NSCAssert([spinner_container isKindOfClass:[IUPTextSpinnerContainer class]], @"Expected IUPTextSpinnerContainer");
	return [spinner_container stepperObject];
}



// This is shared between IupCocoaTextField & IupCocoaSecureTextField
static BOOL IupCocoaTextFieldActionCallbackHelper(Ihandle* ih, NSRange change_range, NSString* replacement_string)
{
	if(ih->data->disable_callbacks)
	{
		return YES;
	}
	
	
	IFnis action_cb = (IFnis)IupGetCallback(ih, "ACTION");
	int ret_val;
	
	if(NULL != action_cb)
	{
		
		// FIXME: Converting to UTF8String may break the start/end ranges
		int start_pos = (int)change_range.location;
		// FIXME: I don't know what end_pos is. I thought I needed to subtract 1, but not doing so makes things work
//		int end_pos = (int)(start_pos + change_range.length - 1);
		int end_pos = (int)(start_pos + change_range.length);
		
		if(end_pos < start_pos)
		{
			end_pos = start_pos;
		}
		
		// Note: I'm assuming we're always in UTF8 mode
		
		//int iupEditCallActionCb(Ihandle* ih, IFnis cb, const char* insert_value, int start, int end, void *mask, int nc, int remove_dir, int utf8)
		// FIXME: remove_direction???: 1 backwards, 0 forwards, -1???. I don't know what this means.
		const char* c_str = [replacement_string UTF8String];
		
		// I think iupEditCallActionCb assumes a delete is NULL for the insert_value and "" will break it.
		if(0 == strlen(c_str))
		{
			c_str = NULL;
		}
		
		ret_val = iupEditCallActionCb(ih, action_cb, c_str, start_pos, end_pos, ih->data->mask, ih->data->nc, 1, YES);

		// FIXME: I don't understand the documentation return value rules.
		if(0 == ret_val)
		{
			return YES;
		}
		else if(-1 != ret_val)
		{
			return NO;
		}
		
	}
	return YES;
}


// Helper function for NSTextField and NSSecureTextField overrides of textView:menu:forEvent:atIndex: to inject user menu items into the context menu.
static void cocoaTextFieldOverrideContextMenuHelper(Ihandle* ih, NSTextView* text_view, NSMenu* the_menu)
{
	Ihandle* menu_ih = (Ihandle*)iupAttribGet(ih, "_COCOA_CONTEXT_MENU_IH");
	if(NULL != menu_ih)
	{
		NSMenu* user_menu = (NSMenu*)menu_ih->handle;
		iupCocoaCommonBaseAppendMenuItems(the_menu, user_menu);
		// It appears that Cocoa may append "Services" after our entries if something is selected, so we want another separator.
		NSRange selected_range = [text_view selectedRange];
		if(selected_range.length > 0)
		{
			[the_menu addItem:[NSMenuItem separatorItem]];
		}
	}
}

@interface IupCocoaTextField : NSTextField
@end

@implementation IupCocoaTextField

/*
- (void) controlTextDidBeginEditing:(NSNotification*)notification
{
}

- (void) controlTextDidEndEditing:(NSNotification*)notification
{
}
*/

// I'm in dangerous territory here and could break things like undo
// http://www.cocoabuilder.com/archive/cocoa/309009-equivalent-of-uitextfield-textfield-shouldchangecharactersinrange-replacementstring-for-nstextfield.html
// Also, I'm not sure if this is public or private API. If the latter, we need to remove it.
- (BOOL) textView:(NSTextView*)text_view shouldChangeTextInRange:(NSRange)change_range replacementString:(NSString*)replacement_string
{
//	NSLog(@"textView shouldChangeTextInRange replacement: %@", replacement_string);
	
	BOOL ret_flag;
	if([super textView:text_view shouldChangeTextInRange:change_range replacementString:replacement_string])
	{
		id the_delegate = [self delegate];
		if([the_delegate respondsToSelector:@selector(textView:shouldChangeTextInRange:replacementString:)])
		{
			ret_flag = [the_delegate textView:text_view shouldChangeTextInRange:change_range replacementString:replacement_string];
			if(NO == ret_flag)
			{
				// If something returned NO, I don't think a user callback can do anything.
				return NO;
			}
		}
	}
	else
	{
		// If something returned NO, I don't think a user callback can do anything.
		return NO;
	}
	
	
	NSTextField* text_field = self;
	Ihandle* ih = (Ihandle*)objc_getAssociatedObject(text_field, IHANDLE_ASSOCIATED_OBJ_KEY);
	
	ret_flag = IupCocoaTextFieldActionCallbackHelper(ih, change_range, replacement_string);
	if(YES == ret_flag)
	{
	    [text_view didChangeText];
	}
	return ret_flag;
}

// WARNING: This was the only way I could figure out how to correctly override the contextual menu for NSTextField.
// I tried setting the menu (on demand) of the field editor, but I ended up getting lots of missing items.
// I tried creating my own field editor and setting a custom delegate for this method on it, but it never got invoked. I think Cocoa may ignore my delegate when used as a field editor.
// I found a mention on the internet that vaguely suggested subclassing NSTextField and implementing this method.
// I have not found documentation about this method in NSTextField, only as a delegate protocol for NSTextView.
// This might mean this is private API so this may have to be disabled.
// This seems to work though.
- (NSMenu *)textView:(NSTextView *)text_view menu:(NSMenu *)the_menu forEvent:(NSEvent *)the_event atIndex:(NSUInteger)char_index
{
	Ihandle* ih = (Ihandle*)objc_getAssociatedObject(self, IHANDLE_ASSOCIATED_OBJ_KEY);
	cocoaTextFieldOverrideContextMenuHelper(ih, text_view, the_menu);
	return the_menu;
}
@end


@interface IupCocoaSecureTextField : NSSecureTextField
@end

@implementation IupCocoaSecureTextField

/*
 - (void) controlTextDidBeginEditing:(NSNotification*)notification
 {
 }
 
 - (void) controlTextDidEndEditing:(NSNotification*)notification
 {
 }
 */

// I'm in dangerous territory here and could break things like undo
// http://www.cocoabuilder.com/archive/cocoa/309009-equivalent-of-uitextfield-textfield-shouldchangecharactersinrange-replacementstring-for-nstextfield.html
// Also, I'm not sure if this is public or private API. If the latter, we need to remove it.
- (BOOL) textView:(NSTextView*)text_view shouldChangeTextInRange:(NSRange)change_range replacementString:(NSString*)replacement_string
{
	//	NSLog(@"textView shouldChangeTextInRange replacement: %@", replacement_string);
	
	BOOL ret_flag;
	if([super textView:text_view shouldChangeTextInRange:change_range replacementString:replacement_string])
	{
		id the_delegate = [self delegate];
		if([the_delegate respondsToSelector:@selector(textView:shouldChangeTextInRange:replacementString:)])
		{
			ret_flag = [the_delegate textView:text_view shouldChangeTextInRange:change_range replacementString:replacement_string];
			if(NO == ret_flag)
			{
				// If something returned NO, I don't think a user callback can do anything.
				return NO;
			}
		}
	}
	else
	{
		// If something returned NO, I don't think a user callback can do anything.
		return NO;
	}
	
	
	NSTextField* text_field = self;
	Ihandle* ih = (Ihandle*)objc_getAssociatedObject(text_field, IHANDLE_ASSOCIATED_OBJ_KEY);
	
	ret_flag = IupCocoaTextFieldActionCallbackHelper(ih, change_range, replacement_string);
	if(YES == ret_flag)
	{
	    [text_view didChangeText];
	}
	return ret_flag;
}

// WARNING: This was the only way I could figure out how to correctly override the contextual menu for NSTextField.
// I tried setting the menu (on demand) of the field editor, but I ended up getting lots of missing items.
// I tried creating my own field editor and setting a custom delegate for this method on it, but it never got invoked. I think Cocoa may ignore my delegate when used as a field editor.
// I found a mention on the internet that vaguely suggested subclassing NSTextField and implementing this method.
// I have not found documentation about this method in NSTextField, only as a delegate protocol for NSTextView.
// This might mean this is private API so this may have to be disabled.
// This seems to work though.
- (NSMenu *)textView:(NSTextView *)text_view menu:(NSMenu *)the_menu forEvent:(NSEvent *)the_event atIndex:(NSUInteger)char_index
{
	Ihandle* ih = (Ihandle*)objc_getAssociatedObject(self, IHANDLE_ASSOCIATED_OBJ_KEY);
	cocoaTextFieldOverrideContextMenuHelper(ih, text_view, the_menu);
	return the_menu;
}

@end

@interface IupCocoaTextFieldDelegate : NSObject <NSControlTextEditingDelegate>
@end

@implementation IupCocoaTextFieldDelegate
// We could have reused textView:shouldChangeTextInRange:replacementString:, but I have multiple concerns we may not be able to keep it.
// So we use this for the VALUECHANGED_CB
- (void) controlTextDidChange:(NSNotification*)the_notification
{
	NSTextField* text_field = [the_notification object];
	Ihandle* ih = (Ihandle*)objc_getAssociatedObject(text_field, IHANDLE_ASSOCIATED_OBJ_KEY);
//	NSLog(@"controlTextDidChange: stringValue == %@", [text_field stringValue]);

	if(ih->data->disable_callbacks)
	{
		return;
	}
	
	IFn value_changed_callback = (IFn)IupGetCallback(ih, "VALUECHANGED_CB");
	if(NULL != value_changed_callback)
	{
		int ret_val = value_changed_callback(ih);
		(void)ret_val;
	}
}

@end

// NOTE: This callback is unfinished. Need to understand SPIN_CB rules better.
// Inherit from IupCocoaTextFieldDelegate to get VALUECHANGED_CB
@interface IupCocoaTextSpinnerDelegate : IupCocoaTextFieldDelegate
@end

@implementation IupCocoaTextSpinnerDelegate
- (void) controlTextDidEndEditing:(NSNotification*)the_notification
{
	IFni callback_function;
	NSTextField* text_field = [the_notification object];
	Ihandle* ih = (Ihandle*)objc_getAssociatedObject(text_field, IHANDLE_ASSOCIATED_OBJ_KEY);
	
	NSLog(@"controlTextDidEndEditing: %@", [the_notification userInfo]);
	
	callback_function = IupGetCallback(ih, "SPIN_CB");
	if(callback_function)
	{
		int current_value = [text_field intValue];
		
		int ret_val = callback_function(ih, current_value);
		
//		if(IUP_IGNORE == ret_val)
		{
			IUPStepperObject* stepper_object = cocoaTextGetStepperObject(ih);
			NSInteger old_value = [stepper_object stepperValue];
			NSLog(@"old_value: %d", old_value);

		}
//		else
		{
			
		}
	}
	
	
}
- (IBAction) mySpinnerClickAction:(id)the_sender
{
	NSLog(@"mySpinnerClickAction");
}
@end


// Inherit from IupCocoaTextFieldDelegate to get VALUECHANGED_CB
@interface IupCocoaTextViewDelegate : IupCocoaTextFieldDelegate <NSTextViewDelegate>
{
	NSUndoManager* undoManager;
}
@end

@implementation IupCocoaTextViewDelegate

- (instancetype) init
{
	self = [super init];
	if(nil != self)
	{
		undoManager = [[NSUndoManager alloc] init];
	}
	return self;
}

- (void) dealloc
{
	[undoManager release];
	[super dealloc];
}

- (NSUndoManager*) undoManagerForTextView:(NSTextView*)text_view
{
	return undoManager;
}

- (BOOL) textView:(NSTextView*)text_view shouldChangeTextInRange:(NSRange)change_range replacementString:(NSString*)replacement_string
{
	Ihandle* ih = (Ihandle*)objc_getAssociatedObject(text_view, IHANDLE_ASSOCIATED_OBJ_KEY);
	BOOL ret_flag = IupCocoaTextFieldActionCallbackHelper(ih, change_range, replacement_string);
	if(YES == ret_flag)
	{
	    [text_view didChangeText];
	}
	return ret_flag;
}

@end


void iupdrvTextAddSpin(Ihandle* ih, int *w, int h)
{

	
}

void iupdrvTextAddBorders(Ihandle* ih, int *x, int *y)
{
	// TODO: Originally, we wanted to lock down the height for NSTextFields so Iup wouldn't try to grow it.
	// This broke multiline NSTextView's.
	// We needed the Ihandle* to distinguish, but the API did not provide it.
	// We've since changed the API to get this.
	// However, we also employed NSStackView to prevent the NSTextField from increasing its height.
	// This code seems to work for now, but probably should be heavily tested under more conditions.
	
	// if(ih->data->is_multiline)



	// WARNING: This is still a hack. SCROLLBAR_PADDING ws randomly guessed to get by for now.
	// BEWARE: The Cocoa widget may not exist yet when this is called. We will get an ih, but ih->handle is not established. This will make things harder.
	IupCocoaTextSubType sub_type = cocoaTextGetSubType(ih);
	switch(sub_type)
	{
		case IUPCOCOATEXTSUBTYPE_VIEW:
		{
//			NSTextView* text_view = cocoaTextGetTextView(ih);

	
			//	*y = *y+10;
			//	*x = *x+4;
				// NSTextField is all guess work about how to scale for any font size.
				// Throw away ih->currentheight because it will EXPAND it.
				// But for the standard font, we get fontheight=16 and we want the height to be 22

			
			//	int font_height = 16;
				int font_height = *y;
			
			//	iupdrvFontGetCharSize(ih, NULL, &font_height);
				const CGFloat HEIGHT_PADDING = 6.0;
				const CGFloat SCROLLBAR_PADDING = 8.0;

				*y = font_height + HEIGHT_PADDING + SCROLLBAR_PADDING;
			//	*y = 16;

				const CGFloat WIDTH_PADDING = 12.0;
			
			
				*x = *x - WIDTH_PADDING;

			break;
		}
		case IUPCOCOATEXTSUBTYPE_FIELD:
		{
			// Neither the field nor the cell work. I think I must change the field editor.
//			NSTextField* text_field = cocoaTextGetTextField(ih);


			
			//	*y = *y+10;
			//	*x = *x+4;
				// NSTextField is all guess work about how to scale for any font size.
				// Throw away ih->currentheight because it will EXPAND it.
				// But for the standard font, we get fontheight=16 and we want the height to be 22

			
			//	int font_height = 16;
				int font_height = *y;
			
			//	iupdrvFontGetCharSize(ih, NULL, &font_height);
				const CGFloat HEIGHT_PADDING = 6.0;
			
				*y = font_height + HEIGHT_PADDING;
			//	*y = 16;

				const CGFloat WIDTH_PADDING = 12.0;
			
			
				*x = *x - WIDTH_PADDING;

			break;
		}
		case IUPCOCOATEXTSUBTYPE_STEPPER:
		{
		
		
	
			//	*y = *y+10;
			//	*x = *x+4;
				// NSTextField is all guess work about how to scale for any font size.
				// Throw away ih->currentheight because it will EXPAND it.
				// But for the standard font, we get fontheight=16 and we want the height to be 22

			
			//	int font_height = 16;
				int font_height = *y;
			
			//	iupdrvFontGetCharSize(ih, NULL, &font_height);
				const CGFloat HEIGHT_PADDING = 6.0;
			
				*y = font_height + HEIGHT_PADDING;
			//	*y = 16;

				const CGFloat WIDTH_PADDING = 12.0;
			
			
				*x = *x - WIDTH_PADDING;
	
			break;
		}
		default:
		{
			break;
		}
	}
	
	
	

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
			IupCocoaFont* iup_font = iupCocoaGetFont(ih);
			NSAttributedString* attributed_string = [[NSAttributedString alloc] initWithString:ns_string attributes:[iup_font attributeDictionary]];
			[[text_view textStorage] setAttributedString:attributed_string];
			[attributed_string release];

			break;
		}
		case IUPCOCOATEXTSUBTYPE_FIELD:
		{
			NSTextField* text_field = cocoaTextGetTextField(ih);
			NSCAssert([text_field isKindOfClass:[NSTextField class]], @"Expected NSTextField");
			IupCocoaFont* iup_font = iupCocoaGetFont(ih);
			if([iup_font usesAttributes])
			{
				NSAttributedString* attributed_string = [[NSAttributedString alloc] initWithString:ns_string attributes:[iup_font attributeDictionary]];
				[text_field setAttributedStringValue:attributed_string];
				[attributed_string release];
			
			}
			else
			{
				[text_field setStringValue:ns_string];
			}
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
			
			NSString* ns_string = [[text_view textStorage] string];
			value = iupStrReturnStr([ns_string UTF8String]);
			
			break;
		}
		case IUPCOCOATEXTSUBTYPE_FIELD:
		{
			NSTextField* text_field = cocoaTextGetTextField(ih);
			
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


/// For the provided start_line, start_column, end_line, end_column, get the native NSRange for the selection.
static bool cocoaTextComputeRangeFromLineColumnForTextView(NSTextView* text_view, NSUInteger start_line, NSUInteger start_column, NSUInteger end_line, NSUInteger end_column, NSRange* out_range)
{
	*out_range = NSMakeRange(0, 0);

	if(end_line < start_line)
	{
		return false;
	}
	else if((end_line == start_line) && (end_column < start_column))
	{
		return false;
	}

	NSLayoutManager* layout_manager = [text_view layoutManager];
	NSUInteger number_of_lines;
	NSUInteger index;
	
	NSUInteger number_of_glyphs = [layout_manager numberOfGlyphs];
	NSRange line_range = NSMakeRange(0, 0);

	bool found_start_line = false;
	bool found_start_column = false;
	NSUInteger selection_start_index = 0;

	for(number_of_lines = 0, index = 0; index < number_of_glyphs;)
	{
		NSUInteger last_index = index; // this is for a workaround where end_line==start_line
		NSRange last_range = line_range; // this is for a workaround where end_line==start_line
		
		(void) [layout_manager lineFragmentRectForGlyphAtIndex:index
			effectiveRange:&line_range];
		index = NSMaxRange(line_range);
		number_of_lines++;
		if(number_of_lines == start_line)
		{
			found_start_line = true;
			// line_range.length will tell us how manay glyphs there are on this line
			if(line_range.length >= start_column)
			{
				found_start_column = true;
				// The line_range.length may have overshot, so we need to subtract out the extra glyphs
				NSUInteger overshot_count = line_range.length - (start_column-1);
	
				// index contains our total running count of all glyphs, which is what we need to specify the range for text selection
				selection_start_index = index - overshot_count;
			}
			else
			{
				// Decision: If there are not enough columns on this line,
				// do we start the selection at the end of the line
				// or do we abort?
				found_start_column = false;
				selection_start_index = index;
			}
			
			// Edge case: end_line is the same line as start_line
			// The problem is we handle the end positions in the next block, but advance to the next line earlier in this loop.
			// The easiest workaround to do is back up the counters and re-do the next line in the next block if this is the case.
			if(start_line == end_line)
			{
				number_of_lines--;
				line_range = last_range;
				index = last_index;
			}
			
			break;
		}
	}

	if(!found_start_line)
	{
		NSLog(@"Failed to find startLine of %lu. Number of lines is %lu", start_line, number_of_lines);
		return false;
	}

	// If we comment out this block, we're going to allow being too short on columns.
/*
	if(!found_start_column)
	{
		NSLog(@"Failed to find startColumn line of %lu. Number of columns at line %lu is %lu", start_column, number_of_lines, line_range.location);
		return false;
	}
*/

	// Do end
	bool found_end_line = false;
	bool found_end_column = false;
	NSUInteger selection_end_index = 0;

	for(; index < number_of_glyphs;)
	{
		(void) [layout_manager lineFragmentRectForGlyphAtIndex:index
			effectiveRange:&line_range];
		index = NSMaxRange(line_range);
		number_of_lines++;
		if(number_of_lines == end_line)
		{
			found_end_line = true;
			// line_range.length will tell us how manay glyphs there are on this line
			if(line_range.length >= end_column)
			{
				found_start_column = true;
				// The line_range.length may have overshot, so we need to subtract out the extra glyphs
				NSUInteger overshot_count = line_range.length - (end_column-1);
	
				// index contains our total running count of all glyphs, which is what we need to specify the range for text selection
				selection_end_index = index - overshot_count;
			}
			else
			{
				// Decision: If there are not enough columns on this line,
				// do we start the selection at the end of the line
				// or do we abort?
				found_end_column = false;
				selection_end_index = index;
			}
			break;
		}
	}

	// Decision: If there are not enough lines,
	// do we select up to the end,
	// or do we abort?
	if(!found_end_line)
	{
#if 0
		NSLog(@"Failed to find startLine of %lu. Number of lines is %lu", start_line, number_of_lines);
		return false;
#else
		selection_end_index = index;
#endif
	}

	// If we comment out this block, we're going to allow being too short on columns.
/*
	if(!found_start_column)
	{
		NSLog(@"Failed to find startColumn line of %lu. Number of columns at line %lu is %lu", start_column, number_of_lines, line_range.length);
		return false;
	}
*/

	NSRange selection_range = NSMakeRange(selection_start_index, selection_end_index-selection_start_index);
//	[textView setSelectedRange:selection_range];
	*out_range = selection_range;

	return true;
}

/// For a provided native_selection_range, get the start_line, start_column, end_line, end_column
/// @param native_selection_range This is a range that something like [text_view selectedRange] would return
static bool cocoaTextComputeLineColumnFromRangeForTextView(NSTextView* text_view, NSRange native_selection_range, NSUInteger* out_start_line, NSUInteger* out_start_column, NSUInteger* out_end_line, NSUInteger* out_end_column)
{
	*out_start_line = 0;
	*out_start_column = 0;
	*out_end_line = 0;
	*out_end_column = 0;

	NSUInteger start_line = 1;
	NSUInteger start_column = 1;
	NSUInteger end_line = 1;
	NSUInteger end_column = 1;


	NSLayoutManager* layout_manager = [text_view layoutManager];
	NSUInteger number_of_lines;
	NSUInteger index;
	
	NSUInteger number_of_glyphs = [layout_manager numberOfGlyphs];
	NSRange line_range = NSMakeRange(0, 0);



	bool found_start_line = false;
	bool found_start_column = false;
	NSUInteger selection_start_index = native_selection_range.location;
	NSUInteger selection_end_index = native_selection_range.location + native_selection_range.length;

	for(number_of_lines = 0, index = 0; index < number_of_glyphs;)
	{
//		NSUInteger last_index = index; // this is for a workaround where end_line==start_line
//		NSRange last_range = line_range; // this is for a workaround where end_line==start_line
		
		(void) [layout_manager lineFragmentRectForGlyphAtIndex:index
			effectiveRange:&line_range];
		index = NSMaxRange(line_range);
		number_of_lines++;
		if(index >= selection_start_index)
		{
			found_start_line = true;
			start_line = number_of_lines;
			
			found_start_column = true;
			// Getting a full line at a time may have overshot, so we need to subtract out the extra glyphs
			NSUInteger overshot_count = index - (selection_start_index);

			// line_range.length will tell us how manay glyphs there are on this lin
			start_column = line_range.length - overshot_count + 1;


			// Edge case: end_line is the same line as start_line
			// The problem is we handle the end positions in the next block, but advance to the next line earlier in this loop.
			// So just handle it here and return immediately.
			if(index >= selection_end_index)
			{
				NSUInteger overshot_end_count = index - (selection_end_index);
				end_column = line_range.length - overshot_end_count + 1;
				end_line = start_line;
				
				*out_start_line = start_line;
				*out_start_column = start_column;
				*out_end_line = end_line;
				*out_end_column = end_column;
				return true;
			}
			break;
		}
	}

	if(!found_start_line)
	{
		NSLog(@"Failed to find start position at %lu. Number of glyphs is %lu", selection_start_index, number_of_glyphs);
		return false;
	}

	// If we comment out this block, we're going to allow being too short on columns.
/*
	if(!found_start_column)
	{
		NSLog(@"Failed to find startColumn line of %lu. Number of columns at line %lu is %lu", start_column, number_of_lines, line_range.location);
		return false;
	}
*/

	// Do end
	bool found_end_line = false;
	bool found_end_column = false;

	for(; index < number_of_glyphs;)
	{
		(void) [layout_manager lineFragmentRectForGlyphAtIndex:index
			effectiveRange:&line_range];
		index = NSMaxRange(line_range);
		number_of_lines++;
		if(index >= selection_end_index)
		{
			found_end_line = true;
			end_line = number_of_lines;
			
			found_end_column = true;

			// Getting a full line at a time may have overshot, so we need to subtract out the extra glyphs
			NSUInteger overshot_count = index - (selection_end_index);

			// line_range.length will tell us how manay glyphs there are on this lin
			end_column = line_range.length - overshot_count + 1;
			break;
		}
	}

	// Decision: If there are not enough lines,
	// do we select up to the end,
	// or do we abort?
	if(!found_end_line)
	{
#if 0
		NSLog(@"Failed to find end position of %lu. Number of gylphs is %lu", selection_end_index, number_of_glyphs);
		return false;
#else
		end_line = number_of_lines;
		end_column = line_range.length;
#endif
	}



	*out_start_line = start_line;
	*out_start_column = start_column;
	*out_end_line = end_line;
	*out_end_column = end_column;
	return true;
}

void iupdrvTextConvertLinColToPos(Ihandle* ih, int lin, int col, int *pos)
{
		NSLog(@"iupdrvTextConvertLinColToPos");

}

void iupdrvTextConvertPosToLinCol(Ihandle* ih, int pos, int *lin, int *col)
{
	NSLog(@"iupdrvTextConvertPosToLinCol");

	
}



void* iupdrvTextAddFormatTagStartBulk(Ihandle* ih)
{
	NSLog(@"iupdrvTextAddFormatTagStartBulk");
	  NSTextView* text_view = cocoaTextGetTextView(ih);
	NSUndoManager* undo_manager = [[text_view delegate] undoManagerForTextView:text_view];
	[undo_manager beginUndoGrouping];
	
	  NSTextStorage* text_storage = [text_view textStorage];
	  // I'm not sure if this is safe. The LayoutManager will throw an exception if a layout is done while beginEditing
	  [text_storage beginEditing];
	return NULL;
}

void iupdrvTextAddFormatTagStopBulk(Ihandle* ih, void* state)
{
	NSLog(@"iupdrvTextAddFormatTagStopBulk");

	  NSTextView* text_view = cocoaTextGetTextView(ih);
	NSUndoManager* undo_manager = [[text_view delegate] undoManagerForTextView:text_view];
	
	  NSTextStorage* text_storage = [text_view textStorage];
	  [text_storage endEditing];
//	  [text_view didChangeText];
	[undo_manager endUndoGrouping];

}


static bool cocoaTextParseParagraphFormat(Ihandle* formattag, NSMutableDictionary* attribute_dict)
{
	bool needs_paragraph_style = false;
	char* format;

	NSMutableParagraphStyle* paragraph_style = nil;
	paragraph_style = [[attribute_dict objectForKey:NSParagraphStyleAttributeName] mutableCopy];
	if(nil == paragraph_style)
	{
		paragraph_style = [[NSMutableParagraphStyle defaultParagraphStyle] mutableCopy];
	}
	
	[paragraph_style autorelease];
	
	

	
	format = iupAttribGet(formattag, "ALIGNMENT");
	if(format)
	{
		if(iupStrEqualNoCase(format, "JUSTIFY"))
		{
			[paragraph_style setAlignment:NSTextAlignmentJustified];
		}
		else if(iupStrEqualNoCase(format, "RIGHT"))
		{
			[paragraph_style setAlignment:NSTextAlignmentRight];
		}
		else if(iupStrEqualNoCase(format, "CENTER"))
		{
			[paragraph_style setAlignment:NSTextAlignmentCenter];
		}
		else /* "LEFT" */
		{
			[paragraph_style setAlignment:NSTextAlignmentLeft];
		}
		needs_paragraph_style = true;
	}
	
	format = iupAttribGet(formattag, "INDENT");
	if(format)
	{
		int val = 0;
		if(iupStrToInt(format, &val))
		{
			[paragraph_style setFirstLineHeadIndent:(CGFloat)val];
			needs_paragraph_style = true;
		}
		
		format = iupAttribGet(formattag, "INDENTRIGHT");
		if(format && iupStrToInt(format, &val))
		{
			// The Iup/Windows implementation seems to use the value relative to the right margin.
			// In Cocoa, positive values are the distance from the left margin, and negative values are from the right.
			// NOTE: Cocoa uses head/tail, not left/right because it supports right-to-left languages.
			[paragraph_style setTailIndent:(CGFloat)(-val)];
			needs_paragraph_style = true;
		}
		
		format = iupAttribGet(formattag, "INDENTOFFSET");
		if(format && iupStrToInt(format, &val))
		{
			[paragraph_style setHeadIndent:(CGFloat)(val)];
			needs_paragraph_style = true;
		}
	}


	format = iupAttribGet(formattag, "LINESPACING");
	if(format)
	{
		double val = 0;
		
		if(iupStrEqualNoCase(format, "SINGLE"))
		{
			[paragraph_style setLineSpacing:1.0];
		}
		else if(iupStrEqualNoCase(format, "ONEHALF"))
		{
			[paragraph_style setLineSpacing:1.5];
		}
		else if(iupStrEqualNoCase(format, "DOUBLE"))
		{
			[paragraph_style setLineSpacing:2.0];
		}
		else if(iupStrToDouble(format, &val))
		{
			[paragraph_style setLineSpacing:val];
		}
		needs_paragraph_style = true;
	}
	
	format = iupAttribGet(formattag, "SPACEBEFORE");
	if(format)
	{
		double val = 0;
		if(iupStrToDouble(format, &val))
		{
			[paragraph_style setParagraphSpacingBefore:val];
			needs_paragraph_style = true;
		}
	}
	
	format = iupAttribGet(formattag, "SPACEAFTER");
	if(format)
	{
		double val = 0;
		if(iupStrToDouble(format, &val))
		{
			[paragraph_style setParagraphSpacing:val];
			needs_paragraph_style = true;
		}
	}

	format = iupAttribGet(formattag, "TABSARRAY");
	if(format)
	{
		int pos = 0;
		char* str;
		NSMutableArray* tab_array = [NSMutableArray array];
		
		while(format)
		{
			str = iupStrDupUntil((char**)&format, ' ');
			if (!str) break;
  		    pos = atoi(str);
			free(str);
			
			str = iupStrDupUntil((char**)&format, ' ');
			if (!str) break;
			
			NSTextTab* text_tab = nil;
			// NOTE: DECIMAL is not supported.
			// Cocoa additioanlly supports natural and justified.
			if(iupStrEqualNoCase(str, "LEFT"))
			{
				text_tab = [[NSTextTab alloc] initWithTextAlignment:NSTextAlignmentLeft location:(CGFloat)pos options:[NSDictionary dictionary]];
			}
			else if(iupStrEqualNoCase(str, "RIGHT"))
			{
				text_tab = [[NSTextTab alloc] initWithTextAlignment:NSTextAlignmentRight location:(CGFloat)pos options:[NSDictionary dictionary]];
			}
			else if(iupStrEqualNoCase(str, "CENTER"))
			{
				text_tab = [[NSTextTab alloc] initWithTextAlignment:NSTextAlignmentCenter location:(CGFloat)pos options:[NSDictionary dictionary]];
			}
			else /* fallback for unsupported options */
			{
				text_tab = [[NSTextTab alloc] initWithTextAlignment:NSTextAlignmentLeft location:(CGFloat)pos options:[NSDictionary dictionary]];
			}
			free(str);
			
			[tab_array addObject:text_tab];
			[text_tab release];
		}
		
		[paragraph_style setTabStops:tab_array];
		needs_paragraph_style = true;
	}
	

	if(needs_paragraph_style)
	{
		[attribute_dict setValue:paragraph_style
			forKey:NSParagraphStyleAttributeName];
	}
	return needs_paragraph_style;
}

/*
NOTES: Not supporting NUMBERINGTAB because following TextEdit.app conventions, we use two tabs (before & after the marker), so the API doesn't fit.
Using TABSARRAY seems to be the correct way to control indentation for this (looking at TextEdit.app).
Additionally, because we must manually inject the markers into the text, a variable number of tabs makes it harder to remove formatting because we already must use regex to find patterns since we don't necessarily know the marker type to remove.
Long term, we might like to support nested lists, which NUMBERINGTAB will make harder to detect since it could confuse the number of tabs to look for.
*/
static bool cocoaTextParseBulletNumberListFormat(Ihandle* ih, Ihandle* formattag, NSTextView* text_view, NSRange selection_range)
{
	// Apple doesn't provide these string constants until 10.13, so we provide our own copy.
	static NSString* IupNSTextListMarkerBox = @"{box}";
	static NSString* IupNSTextListMarkerCheck = @"{check}";
	static NSString* IupNSTextListMarkerCircle = @"{circle}";
	static NSString* IupNSTextListMarkerDiamond = @"{diamond}";
	static NSString* IupNSTextListMarkerDisc = @"{disc}";
	static NSString* IupNSTextListMarkerHyphen = @"{hyphen}";
	static NSString* IupNSTextListMarkerSquare = @"{square}";
	static NSString* IupNSTextListMarkerLowercaseHexadecimal = @"{lower-hexadecimal}";
	static NSString* IupNSTextListMarkerUppercaseHexadecimal = @"{upper-hexadecimal}";
	static NSString* IupNSTextListMarkerOctal = @"{octal}";
	static NSString* IupNSTextListMarkerLowercaseAlpha = @"{lower-alpha}";
	static NSString* IupNSTextListMarkerUppercaseAlpha = @"{upper-alpha}";
	static NSString* IupNSTextListMarkerLowercaseLatin = @"{lower-latin}";
	static NSString* IupNSTextListMarkerUppercaseLatin = @"{upper-latin}";
	static NSString* IupNSTextListMarkerLowercaseRoman = @"{lower-roman}";
	static NSString* IupNSTextListMarkerUppercaseRoman = @"{upper-roman}";
	static NSString* IupNSTextListMarkerDecimal = @"{decimal}";
	
	char* format;
	bool use_list = false;
	NSString* which_list_marker = nil;

	format = iupAttribGet(formattag, "NUMBERING");
	if(format)
	{
		use_list = true; // will unset in NONE case
		if(iupStrEqualNoCase(format, "BULLET"))
		{
			which_list_marker = IupNSTextListMarkerDisc;
		}
		else if(iupStrEqualNoCase(format, "ARABIC"))
		{
			which_list_marker = IupNSTextListMarkerDecimal;
		}
		else if(iupStrEqualNoCase(format, "LCLETTER"))
		{
			which_list_marker = IupNSTextListMarkerLowercaseAlpha;
		}
		else if(iupStrEqualNoCase(format, "UCLETTER"))
		{
			which_list_marker = IupNSTextListMarkerUppercaseAlpha;
		}
		else if(iupStrEqualNoCase(format, "LCROMAN"))
		{
			which_list_marker = IupNSTextListMarkerLowercaseRoman;
		}
		else if(iupStrEqualNoCase(format, "UCROMAN"))
		{
			which_list_marker = IupNSTextListMarkerUppercaseRoman;
		}
		else if(iupStrEqualNoCase(format, "NONE"))
		{
			use_list = false;
		}
		else
		{
			use_list = false;
		}
	
		if(use_list)
		{
			const NSUInteger kIupNumberingStyleNone = 0;
			const NSUInteger kIupNumberingStyleRightParenthesis = 1;
			const NSUInteger kIupNumberingStyleParenthesis = 2;
			const NSUInteger kIupNumberingStylePeriod = 3;
			const NSUInteger kIupNumberingStyleNonNumber = 4;

			NSUInteger which_number_style = kIupNumberingStyleNone;
			
			/* TODO: WARNING: If the user hits return in the middle of a list, the custom NUMBERINGSTYLE disappears.
			 	According to this blog:
				https://meandmark.com/blog/2016/12/adding-markers-to-text-lists-when-pressing-the-return-key/
				1. Create a subclass of NSTextView.
				2. Override the function insertNewline in your NSTextView subclass. This function gets called when someone presses the Return key.
				3. Use the text view’s selectedRange property to determine if you’re inside a list. The selectedRange property’s location field is the text view’s insertion point.
				4. If you’re inside a list, add the marker to the text view’s text storage at the insertion point.

				I'm suspicious that this may not fully work.
				I noticed that we lose all the existing NUMBERINGSTYLEs, but they make no mention of that.
				This makes me think they didn't set up the textLists correctly in the paragraph attributes.
				Ironically, if we don't set up the TextLists/paragraph attributes correctly,
				the NUMBERINGSTYLE won't revert.
				But we lose other properties such as the numbers automatically re-ordering themselves
				and it being aware that it is a list (tab and shift-tab can change the nesting levels).
			 
				This may be worth a DTS incident to fix.
				Or if somebody can find a modern version of the TextEdit.app source code.
			*/
			format = iupAttribGet(formattag, "NUMBERINGSTYLE");
			if(format)
			{
				if(iupStrEqualNoCase(format, "RIGHTPARENTHESIS"))
				{
					which_number_style = kIupNumberingStyleRightParenthesis;
				}
				else if(iupStrEqualNoCase(format, "PARENTHESIS"))
				{
					which_number_style = kIupNumberingStyleParenthesis;
				}
				else if(iupStrEqualNoCase(format, "PERIOD"))
				{
					which_number_style = kIupNumberingStylePeriod;
				}
				else if(iupStrEqualNoCase(format, "NONUMBER"))
				{
					which_number_style = kIupNumberingStyleNonNumber;
				}
				else if(iupStrEqualNoCase(format, "NONE"))
				{
					which_number_style = kIupNumberingStyleNone;
				}
				else
				{
					which_number_style = kIupNumberingStyleNone;
				}
			}



			NSTextStorage* text_storage = [text_view textStorage];
			NSString* all_string = [text_storage string];

			// The problem seems to be that in order to change the TextLines attribute for the entire block, I need to re-set the attribute.
			// So I have to be very careful about applying attributes and not clobbering them.
			
			[text_storage beginEditing];

			
			__block NSUInteger applied_paragraph_start = NSUIntegerMax;
			__block NSUInteger applied_paragraph_end = 0;
			__block NSMutableArray<NSTextList*>* array_of_text_lists = [NSMutableArray array];
			__block NSUInteger item_count = 1;
			[all_string enumerateSubstringsInRange:selection_range options:NSStringEnumerationByParagraphs usingBlock:
				^(NSString * _Nullable substring, NSRange substring_range, NSRange enclosing_range, BOOL * _Nonnull stop)
				{
					*stop = NO;
	//				NSLog(@"substring:%@", substring);
	//				NSLog(@"substringRange:%@", NSStringFromRange(substring_range));
	//				NSLog(@"enclosingRange:%@", NSStringFromRange(enclosing_range));

					// If the selection block starts in the middle of the paragraph instead of the beginning,
					// we have to decide whether we should start as-is,
					// or back-up to the beginning.
					// TextEdit.app backs up to the beginning, so we will use getParagraphStart to back up to the beginning.
					NSUInteger start_paragraph_index;
					NSUInteger end_paragraph_index;
					NSUInteger contents_end_paragraph_index;
					[all_string getParagraphStart:&start_paragraph_index end:&end_paragraph_index contentsEnd:&contents_end_paragraph_index forRange:enclosing_range];

	//				NSLog(@"start_paragraph_index:%lu", start_paragraph_index);
	//				NSLog(@"end_paragraph_index:%lu", end_paragraph_index);
	//				NSLog(@"contents_end_paragraph_index:%lu", contents_end_paragraph_index);

					if(applied_paragraph_start > start_paragraph_index)
					{
						applied_paragraph_start = start_paragraph_index;
					}
					if(applied_paragraph_end < end_paragraph_index)
					{
						applied_paragraph_end = end_paragraph_index;
					}

					// end_paragraph_index seems to include the newline, which we want
					NSRange paragraph_range = NSMakeRange(start_paragraph_index, end_paragraph_index-start_paragraph_index);
//					NSLog(@"paragraph_range:%@", NSStringFromRange(paragraph_range));


					NSTextList* text_list = [[NSTextList alloc] initWithMarkerFormat:which_list_marker options:0];
					[text_list autorelease];
					[array_of_text_lists addObject:text_list];
					
					NSMutableAttributedString* current_line = [[text_storage attributedSubstringFromRange:paragraph_range] mutableCopy];
					[current_line autorelease];
//					NSLog(@"current_line: %@", current_line);
					NSDictionary<NSAttributedStringKey, id>* current_line_attributes = [current_line attributesAtIndex:0 effectiveRange:NULL];

//					NSLog(@"current_line_attributes: %@", current_line_attributes);

					NSString* marker_with_style_prefix = nil;
					switch(which_number_style)
					{
						case kIupNumberingStyleRightParenthesis:
						{
							marker_with_style_prefix = [NSString stringWithFormat:@"\t%@)\t", [text_list markerForItemNumber:item_count]];
							break;
						}
						case kIupNumberingStyleParenthesis:
						{
							marker_with_style_prefix = [NSString stringWithFormat:@"\t(%@)\t", [text_list markerForItemNumber:item_count]];
							break;
						}
						case kIupNumberingStylePeriod:
						{
							marker_with_style_prefix = [NSString stringWithFormat:@"\t%@.\t", [text_list markerForItemNumber:item_count]];
							break;
						}
						case kIupNumberingStyleNonNumber:
						{
							marker_with_style_prefix = @"\t\t";
							break;
						}
						default:
						{
							marker_with_style_prefix = [NSString stringWithFormat:@"\t%@\t", [text_list markerForItemNumber:item_count]];
							break;
						}
					}
					
					
					NSAttributedString* attribued_prefix = [[NSAttributedString alloc] initWithString:marker_with_style_prefix attributes:current_line_attributes];
					[attribued_prefix autorelease];
					[current_line insertAttributedString:attribued_prefix atIndex:0];
//					[current_line fixAttributesInRange:NSMakeRange(0, [current_line length])];

					ih->data->disable_callbacks = 1;
					// For undo manager
					[text_view shouldChangeTextInRange:paragraph_range replacementString:[current_line string]];
					ih->data->disable_callbacks = 0;
					[text_storage replaceCharactersInRange:paragraph_range withAttributedString:current_line];



					item_count++;
				
				}
			];


			if(applied_paragraph_start == NSUIntegerMax)
			{
				applied_paragraph_start = selection_range.location;
			}
			if(applied_paragraph_end == 0)
			{
				applied_paragraph_end = selection_range.location + selection_range.length;
			}

			
			NSRange applied_paragraph_range = NSMakeRange(applied_paragraph_start, applied_paragraph_end-applied_paragraph_start);
			NSMutableDictionary<NSAttributedStringKey, id>* text_storage_attributes = [[[text_storage attributedSubstringFromRange:applied_paragraph_range] attributesAtIndex:0 effectiveRange:NULL] mutableCopy];
			[text_storage_attributes autorelease];
			NSMutableParagraphStyle* paragraph_style = [[text_storage_attributes objectForKey:NSParagraphStyleAttributeName] mutableCopy];
			[paragraph_style autorelease];
			if(nil == paragraph_style)
			{
//				NSLog(@"nil == paragraph_style");
				paragraph_style = [[NSMutableParagraphStyle defaultParagraphStyle] mutableCopy];
				[paragraph_style autorelease];
			}
			
			// Be careful here:
			// It seems I must do this last, otherwise the list doesn't take effect.
			// (You should be able to add a new line in-between the lines, and it should automatically
			// insert a new bullet and renumber all the lines accordingly.)
			// But applying this last seems to blow away some attributes.
			// Use addAttribute and avoid setAttributes
			// In the text.c test,
			// I lose the strike-through and italics on "Second Line"
			// I lose the alignment on "Third Line" (this is not completely unreasonable)
			[paragraph_style setTextLists:array_of_text_lists];
			[text_storage addAttribute:NSParagraphStyleAttributeName value:paragraph_style range:applied_paragraph_range];


			[text_storage endEditing];
			[text_view didChangeText];

			
		}
		else // we attempt to remove list formatting
		{
			NSTextStorage* text_storage = [text_view textStorage];
			NSString* all_string = [text_storage string];

			// The problem seems to be that in order to remove the TextLines attribute for the entire block, I need to re-set the attribute.
			// So I have to be very careful about applying attributes and not clobbering them.

			
			[text_storage beginEditing];

			__block NSUInteger applied_paragraph_start = NSUIntegerMax;
			__block NSUInteger applied_paragraph_end = 0;
			[all_string enumerateSubstringsInRange:selection_range options:NSStringEnumerationByParagraphs usingBlock:
				^(NSString * _Nullable substring, NSRange substring_range, NSRange enclosing_range, BOOL * _Nonnull stop)
				{
					*stop = NO;
	//				NSLog(@"substring:%@", substring);
	//				NSLog(@"substringRange:%@", NSStringFromRange(substring_range));
	//				NSLog(@"enclosingRange:%@", NSStringFromRange(enclosing_range));

					// If the selection block starts in the middle of the paragraph instead of the beginning,
					// we have to decide whether we should start as-is,
					// or back-up to the beginning.
					// TextEdit.app backs up to the beginning, so we will use getParagraphStart to back up to the beginning.
					NSUInteger start_paragraph_index;
					NSUInteger end_paragraph_index;
					NSUInteger contents_end_paragraph_index;
					[all_string getParagraphStart:&start_paragraph_index end:&end_paragraph_index contentsEnd:&contents_end_paragraph_index forRange:enclosing_range];
					if(applied_paragraph_start > start_paragraph_index)
					{
						applied_paragraph_start = start_paragraph_index;
					}
					if(applied_paragraph_end < end_paragraph_index)
					{
						applied_paragraph_end = end_paragraph_index;
					}
	//				NSLog(@"start_paragraph_index:%lu", start_paragraph_index);
	//				NSLog(@"end_paragraph_index:%lu", end_paragraph_index);
	//				NSLog(@"contents_end_paragraph_index:%lu", contents_end_paragraph_index);

					// end_paragraph_index seems to include the newline, which we want
					NSRange paragraph_range = NSMakeRange(start_paragraph_index, end_paragraph_index-start_paragraph_index);
				//	NSRange paragraph_range = NSMakeRange(start_paragraph_index, contents_end_paragraph_index-start_paragraph_index);
	//				NSLog(@"paragraph_range:%@", NSStringFromRange(paragraph_range));


					NSMutableAttributedString* current_line = [[text_storage attributedSubstringFromRange:paragraph_range] mutableCopy];
					[current_line autorelease];
					NSString* current_line_nsstr = [current_line string];
	//				NSLog(@"current_line bef:%@", current_line);

					NSString* marker_prefix_pattern = @"^\t.*?\t";
					NSError* ns_error = nil;
					NSRegularExpression* reg_ex = [NSRegularExpression
						regularExpressionWithPattern:marker_prefix_pattern
						options:NSRegularExpressionAnchorsMatchLines
						error:&ns_error
					];
					
					NSArray<NSTextCheckingResult*>* regex_matches = [reg_ex matchesInString:current_line_nsstr
						options:kNilOptions
						range:NSMakeRange(0, [current_line_nsstr length])
					];
					
					// There should only be 1 match right now since we anchor with ^
					// But maybe in the future we can leverage the loop to look at nested lists
					for(NSTextCheckingResult* match in regex_matches)
					{
	//					NSLog(@"match:%@", match);
	//					NSLog(@"match range:%@", NSStringFromRange([match range]));
						[current_line deleteCharactersInRange:[match range]];
	//					NSLog(@"current_line aft:%@", current_line);
					}

					// Must be [current_line string] and not current_line_nsstr because we mutated current_line and current_line_nsstr may be out of date
					ih->data->disable_callbacks = 1;
					[text_view shouldChangeTextInRange:paragraph_range replacementString:[current_line string]];
					ih->data->disable_callbacks = 0;
					[text_storage replaceCharactersInRange:paragraph_range withAttributedString:current_line];


				}
			];
			NSRange applied_paragraph_range = NSMakeRange(applied_paragraph_start, applied_paragraph_end-applied_paragraph_start);
			NSMutableDictionary<NSAttributedStringKey, id>* text_storage_attributes = [[[text_storage attributedSubstringFromRange:applied_paragraph_range] attributesAtIndex:0 effectiveRange:NULL] mutableCopy];
			[text_storage_attributes autorelease];
			NSMutableParagraphStyle* paragraph_style = [[text_storage_attributes objectForKey:NSParagraphStyleAttributeName] mutableCopy];
			[paragraph_style autorelease];
			if(nil == paragraph_style)
			{
				// There was no TextList, so there is nothing to remove.
//				NSLog(@"Nothing to remove");
			}
			else
			{
				// Can't set nil, so set empty array
				[paragraph_style setTextLists:[NSArray array]];
				/*
				// Be careful here:
				// Use removeAttribute and not setAttributes
				*/
//				NSLog(@"removed textlist");

				[text_storage removeAttribute:NSParagraphStyleAttributeName range:applied_paragraph_range];

			}

			[text_storage endEditing];
			[text_view didChangeText];
		}
		return true;
	} // end if NUMBERING
	return false;
}

static bool cocoaTextParseCharacterFormat(Ihandle* formattag, NSMutableDictionary* attribute_dict, IupCocoaFont* iup_font)
{
	NSFontManager* font_manager = [NSFontManager sharedFontManager];
	
	NSFont* target_font = nil;
	
	target_font = [attribute_dict objectForKey:NSFontAttributeName];
	if(nil == target_font)
	{
		target_font = [iup_font nativeFont];
	}
	
	NSFontTraitMask trait_mask = [iup_font traitMask];
	
	CGFloat font_size = [target_font pointSize];
//	NSString* font_family = [target_font familyName];
	
	char* format;
	bool did_change_attribute = false;

	format = iupAttribGet(formattag, "FONTSIZE");
	if(format)
	{
		int font_size_int = 0;
		iupStrToInt(format, &font_size_int);
		// less than 0 is in "pixels",
		// but Apple platforms don't really support this because of all the things they do for resolution independent scaling (e.g. retina)
		// All things must be in points.
		if(font_size_int < 0)
		{
		
		}
		else
		{
			font_size = (CGFloat)font_size_int;
		}
		did_change_attribute = true;
	}
	
	format = iupAttribGet(formattag, "FONTFACE");
	if(format)
	{
		/* Map standard names to native names */
		const char* mapped_name = iupFontGetMacName(format);
		NSString* ns_family_name = nil;
		if(mapped_name)
		{
			ns_family_name = [NSString stringWithUTF8String:mapped_name];
		}
		else
		{
			ns_family_name = [NSString stringWithUTF8String:format];
		}
		
		target_font = [font_manager convertFont:target_font toFamily:ns_family_name];
		did_change_attribute = true;
	}
	
	// TODO: Do we reset the traits or are we expected to inherit it from the original font?
	// And does our NSFont carry these set traits, or do we need to save these traits when they were first set?
	format = iupAttribGet(formattag, "ITALIC");
	if(format)
	{
		if(iupStrBoolean(format))
		{
			trait_mask |= NSItalicFontMask;
		}
		else
		{
			trait_mask = trait_mask & (~NSItalicFontMask); // To unset the flag, invert the bits, then AND
		}
		did_change_attribute = true;
	}


	format = iupAttribGet(formattag, "UNDERLINE");
	if(format)
	{
		// TODO: Apple supports DOTTED (and DASHED) independently of SINGLE/DOUBLE.
		if(iupStrEqualNoCase(format, "SINGLE"))
		{
			[attribute_dict setValue:[NSNumber numberWithInt:NSUnderlinePatternSolid|NSUnderlineStyleSingle]
				forKey:NSUnderlineStyleAttributeName];
		}
		else if(iupStrEqualNoCase(format, "DOUBLE"))
		{
			[attribute_dict setValue:[NSNumber numberWithInt:NSUnderlinePatternSolid|NSUnderlineStyleDouble]
				forKey:NSUnderlineStyleAttributeName];
		}
		else if(iupStrEqualNoCase(format, "DOTTED"))
		{
			[attribute_dict setValue:[NSNumber numberWithInt:NSUnderlineStylePatternDot|NSUnderlineStyleSingle]
				forKey:NSUnderlineStyleAttributeName];
		}
		else /* "NONE" */
		{
			[attribute_dict setValue:[NSNumber numberWithInt:NSUnderlineStyleNone]
				forKey:NSUnderlineStyleAttributeName];
		}
		did_change_attribute = true;
	}


	format = iupAttribGet(formattag, "STRIKEOUT");
	if(format)
	{
		if(iupStrBoolean(format))
		{
	  		[attribute_dict setValue:[NSNumber numberWithInt:YES]
				forKey:NSStrikethroughStyleAttributeName];
		}
		else
		{
			[attribute_dict setValue:[NSNumber numberWithInt:NO]
				forKey:NSStrikethroughStyleAttributeName];
		}
		did_change_attribute = true;
	}
	format = iupAttribGet(formattag, "RISE");
	if(format)
	{
		// For Mac, NSSuperscriptAttributeName
		// iOS kCTSuperscriptAttributeName
		// Pass in a negative value for subscript.
    	if(iupStrEqualNoCase(format, "SUPERSCRIPT"))
		{
	  		[attribute_dict setValue:[NSNumber numberWithInt:1]
				forKey:NSSuperscriptAttributeName];
			// I don't know how small to make the font. IUP uses X-SMALL on GTK.
			font_size = font_size * 0.6444444444444;
		}
		else if(iupStrEqualNoCase(format, "SUBSCRIPT"))
		{
	  		[attribute_dict setValue:[NSNumber numberWithInt:-1]
				forKey:NSSuperscriptAttributeName];
			// I don't know how small to make the font. IUP uses X-SMALL on GTK.
			font_size = font_size * 0.6444444444444;
		}
		else
		{
			int offset_val = 0;
			iupStrToInt(format, &offset_val);
			[attribute_dict setValue:[NSNumber numberWithDouble:(double)offset_val]
				forKey:NSBaselineOffsetAttributeName];
			// user is expected to set the font size for this case
		}
		did_change_attribute = true;
	}


	format = iupAttribGet(formattag, "FONTSCALE");
	if(format)
	{
		double fval = 0;
		if (iupStrEqualNoCase(format, "XX-SMALL"))
			fval = 0.5787037037037;
		else if (iupStrEqualNoCase(format, "X-SMALL"))
			fval = 0.6444444444444;
		else if (iupStrEqualNoCase(format, "SMALL"))
			fval = 0.8333333333333;
		else if (iupStrEqualNoCase(format, "MEDIUM"))
			fval = 1.0;
		else if (iupStrEqualNoCase(format, "LARGE"))
			fval = 1.2;
		else if (iupStrEqualNoCase(format, "X-LARGE"))
			fval = 1.4399999999999;
		else if (iupStrEqualNoCase(format, "XX-LARGE"))
			fval = 1.728;
		else
			iupStrToDouble(format, &fval);
		
		if(fval > 0)
		{
			font_size = font_size * fval;
		}
		did_change_attribute = true;
	}

	format = iupAttribGet(formattag, "WEIGHT");
	if(format)
	{
		/*
		Apple Terminology						ISO Equivalent
		1. ultralight
		2. thin W1. ultralight
		3. light, extralight 					W2. extralight
		4. book									W3. light
		5. regular, plain, display, roman		W4. semilight
		6. medium								W5. medium
		7. demi, demibold
		8. semi, semibold						W6. semibold
		9. bold									W7. bold
		10. extra, extrabold					W8. extrabold
		11. heavy, heavyface
		12. black, super						W9. ultrabold
		13. ultra, ultrablack, fat
		14. extrablack, obese, nord
		
		The NSFontManager implementation of this method refuses to convert a font’s weight
		if it can’t maintain all other traits, such as italic and condensed.
		You might wish to override this method to allow a looser interpretation of weight conversion
		*/
		// NSFontWeightTrait: The valid value range is from -1.0 to 1.0. The value of 0.0 corresponds to the regular or medium font weight.
//		CGFloat weight_scale = 0;
		// weightOfFont: An approximation of the weight of the given font, where 0 indicates the lightest possible weight, 5 indicates a normal or book weight, and 9 or more indicates a bold or heavier weight.
		int target_weight = 5;
		
		if(iupStrEqualNoCase(format, "EXTRALIGHT"))
		{
//			weight_scale = -1.0;
			target_weight = 0;
		}
		else if(iupStrEqualNoCase(format, "LIGHT"))
		{
//			weight_scale = -0.5;
			target_weight = 3;
		}
		else if(iupStrEqualNoCase(format, "SEMIBOLD"))
		{
//			weight_scale = 0.25;
			target_weight = 6;
		}
		else if(iupStrEqualNoCase(format, "BOLD"))
		{
//			weight_scale = 0.50;
			target_weight = 8;
		}
		else if (iupStrEqualNoCase(format, "EXTRABOLD"))
		{
//			weight_scale = 0.75;
			target_weight = 10;
		}
		else if(iupStrEqualNoCase(format, "HEAVY"))
		{
//			weight_scale = 1.0;
			target_weight = 11;
		}
		else /* "NORMAL" */
		{
//			weight_scale = 0.0;
			target_weight = 5;
		}
		
		// Ugh: This has no effect.
		// [attribute_dict setValue:[NSNumber numberWithFloat:weight_scale] forKey:NSFontWeightTrait];

		// I don't want to create a font from scratch because there are warnings about using FamilyName for the default/system font.
		// And I just don't want to accidentally miss any properties.
		// But this leaves us only with this curious API which will attempt to bump up/down the weight by 1 level at a time.
		// If it can't go any higher/lower, it returns back the original font.
		// The levels are not well defined, so this is very imprecise.
		NSInteger current_weight = [font_manager weightOfFont:target_font];
		if(current_weight < target_weight)
		{
			// need to go up
			while(current_weight < target_weight)
			{
				NSFont* result_font;
				result_font = [font_manager convertWeight:YES ofFont:target_font];
				current_weight = [font_manager weightOfFont:target_font];
				
				if(result_font == target_font)
				{
					// unable to convert more
					break;
				}
				else
				{
					target_font = result_font;
				}
			}
		}
	   else if(current_weight > target_weight)
	   {
			// need to go down
			while(current_weight > target_weight)
			{
				NSFont* result_font;
				result_font = [font_manager convertWeight:NO ofFont:target_font];
				current_weight = [font_manager weightOfFont:target_font];
				
				if(result_font == target_font)
				{
					// unable to convert more
					break;
				}
				else
				{
					target_font = result_font;
				}
			}
	   }
	   else
	   {
		   // already at target weight
	   }

 
		did_change_attribute = true;

	}
	
	format = iupAttribGet(formattag, "FGCOLOR");
	if(format)
	{
		unsigned char r, g, b;
		if(iupStrToRGB(format, &r, &g, &b))
		{
			CGFloat red = r/255.0;
			CGFloat green = g/255.0;
			CGFloat blue = b/255.0;

			NSColor* the_color = [NSColor colorWithCalibratedRed:red green:green blue:blue alpha:1.0];
			[attribute_dict setValue:the_color
				forKey:NSForegroundColorAttributeName];
		}
		else
		{
			[attribute_dict removeObjectForKey:NSForegroundColorAttributeName];
		}
		did_change_attribute = true;
	}
	
	format = iupAttribGet(formattag, "BGCOLOR");
	if(format)
	{
		unsigned char r, g, b;
		if(iupStrToRGB(format, &r, &g, &b))
		{
			CGFloat red = r/255.0;
			CGFloat green = g/255.0;
			CGFloat blue = b/255.0;

			NSColor* the_color = [NSColor colorWithCalibratedRed:red green:green blue:blue alpha:1.0];
			[attribute_dict setValue:the_color
				forKey:NSBackgroundColorAttributeName];
		}
		else
		{
			[attribute_dict removeObjectForKey:NSBackgroundColorAttributeName];
		}
		did_change_attribute = true;
	}
	
	// Disabled color (not actually disabled)
	// Watch-out, FGCOLOR can conflict with this because they both use NSForegroundColorAttributeName
	format = iupAttribGet(formattag, "DISABLED");
	if(format)
	{
		if(iupStrBoolean(format))
		{
			NSColor* the_color = [NSColor disabledControlTextColor];
			[attribute_dict setValue:the_color
				forKey:NSForegroundColorAttributeName];
		}
		else
		{
			// If the user specified FGCOLOR, then it was already set above.
			if(iupAttribGet(formattag, "FGCOLOR"))
			{
			
			}
			else
			{
				[attribute_dict removeObjectForKey:NSForegroundColorAttributeName];
			}

		}
		did_change_attribute = true;
	}
	
	
	if(did_change_attribute)
	{
		target_font = [font_manager convertFont:target_font toSize:font_size];
		target_font = [font_manager convertFont:target_font toHaveTrait:trait_mask];
		[attribute_dict setValue:target_font forKey:NSFontAttributeName];
	}
	return did_change_attribute;
}

void iupdrvTextAddFormatTag(Ihandle* ih, Ihandle* formattag, int bulk)
{
	if(!ih->data->is_multiline)
	{
		return;
	}
	NSTextView* text_view = cocoaTextGetTextView(ih);


	char* iup_selection = NULL;
	NSRange native_selection_range = {0, 0};
	iup_selection = iupAttribGet(formattag, "SELECTION");
	if(iup_selection)
	{
		int ret_val;
		NSUInteger lin_start=1;
		NSUInteger col_start=1;
		NSUInteger lin_end=1;
		NSUInteger col_end=1;

		ret_val = sscanf(iup_selection, "%lu,%lu:%lu,%lu", &lin_start, &col_start, &lin_end, &col_end);
		if(ret_val != 4)
		{
			return;
		}
		if(lin_start<1 || col_start<1 || lin_end<1 || col_end<1)
		{
			return;
		}
		
		bool did_find_range = cocoaTextComputeRangeFromLineColumnForTextView(text_view, lin_start, col_start, lin_end, col_end, &native_selection_range);
		if(!did_find_range)
		{
			return;
		}
	}
	else
	{
		char* iup_selection_pos = iupAttribGet(formattag, "SELECTIONPOS");
		if(iup_selection_pos)
		{
			// FIXME: Iup is artificially constraining us to 32-bit by not supporting 64-bit variants.
			int start_int = 0;
			int end_int = 0;

			if(iupStrToIntInt(iup_selection_pos, &start_int, &end_int, ':')!=2)
			{
				return;
			}
      		if(start_int<0 || end_int<0)
      		{
		        return;
			}
			NSUInteger start = (NSUInteger)start_int;
			NSUInteger end = (NSUInteger)end_int;
			native_selection_range = NSMakeRange(start, end-start);
		}
		else
		{
			
			IupCocoaFont* iup_font = iupCocoaGetFont(ih);

			// Use the current set font as the baseline. We will modify a local copy of its attributes from there.
			NSMutableDictionary* attribute_dict = [[iup_font attributeDictionary] mutableCopy];
			[attribute_dict autorelease];
			NSTextStorage* text_storage = [text_view textStorage];
			// Set the selection range to be at the very last character.
			// Get the attributes (if any) for the selected range, and merge it into our copy of the font's attributes.
			// This will overwrite/merge the current attributes into our font copy attributes.
			NSDictionary<NSAttributedStringKey, id>* text_storage_attributes = [[text_storage attributedSubstringFromRange:NSMakeRange([text_storage length]-1, 1)] attributesAtIndex:0 effectiveRange:NULL];
			[attribute_dict addEntriesFromDictionary:text_storage_attributes];
			
			
			NSUndoManager* undo_manager = [[text_view delegate] undoManagerForTextView:text_view];
			[undo_manager beginUndoGrouping];
			
			
			bool needs_paragraph_style_change = cocoaTextParseParagraphFormat(formattag, attribute_dict);
			bool needs_character_style_change = cocoaTextParseCharacterFormat(formattag, attribute_dict, iup_font);
			
			if(needs_character_style_change || needs_paragraph_style_change)
			{
				[text_view setTypingAttributes:attribute_dict];
			}
			

			// Append a newline so we have a fresh line to enable bullet lists
			// If we don't, the previous line gets converted into a list which is often not what I think people expect.
			NSAttributedString* attributed_append_string = [[NSAttributedString alloc] initWithString:@"\n" attributes:attribute_dict];
			[attributed_append_string autorelease];
			
			// We need to mark the proposed change range (before we change it) in order to call shouldChangeTextInRange:
			NSRange change_range = NSMakeRange([text_storage length], 0);
			ih->data->disable_callbacks = 1;
			[text_view shouldChangeTextInRange:change_range replacementString:[attributed_append_string string]];
			[text_storage beginEditing];
			[text_storage appendAttributedString:attributed_append_string];
			  ih->data->disable_callbacks = 0;
			[text_storage endEditing];
		
		
			// We need a length of least 1 or an exception gets thrown inside cocoaTextParseBulletNumberListFormat
			// Hence why we append a newline.
			native_selection_range = NSMakeRange([text_storage length]-1, 1);
//			native_selection_range = NSMakeRange(change_range.location, 1);
			cocoaTextParseBulletNumberListFormat(ih, formattag, text_view, native_selection_range);
			
			// The result of the above yields an extra blank line after the bullet.
			// So another hack is the delete the final newline
			change_range = NSMakeRange([text_storage length]-1, 1);
			ih->data->disable_callbacks = 1;
			[text_view shouldChangeTextInRange:change_range replacementString:@""];
			[text_storage beginEditing];
			[text_storage deleteCharactersInRange:change_range];
			  ih->data->disable_callbacks = 0;
			[text_storage endEditing];
		
		
			
			[undo_manager endUndoGrouping];
			
			// Return immediately. The fall-through code is for selection-only
			return;
			
		}
	}
	
	
	IupCocoaFont* iup_font = iupCocoaGetFont(ih);

	// Use the current set font as the baseline. We will modify a local copy of its attributes from there.
	NSMutableDictionary* attribute_dict = [[iup_font attributeDictionary] mutableCopy];
	[attribute_dict autorelease];
	NSTextStorage* text_storage = [text_view textStorage];
	// Get the attributes (if any) for the selected range, and merge it into our copy of the font's attributes.
	// This will overwrite/merge the current attributes into our font copy attributes.
	NSDictionary<NSAttributedStringKey, id>* text_storage_attributes = [[text_storage attributedSubstringFromRange:native_selection_range] attributesAtIndex:0 effectiveRange:NULL];
	[attribute_dict addEntriesFromDictionary:text_storage_attributes];


	
//	NSLog(@"iupdrvTextAddFormatTag: %@", NSStringFromRange(native_selection_range));
	NSUndoManager* undo_manager = [[text_view delegate] undoManagerForTextView:text_view];
	[undo_manager beginUndoGrouping];
	
//	[text_storage beginEditing];
	// Only the bullet/number feature changes the text. nil should express attribute-only change
	[text_view shouldChangeTextInRange:native_selection_range replacementString:nil];


	// cocoaTextParseBulletNumberListFormat actually re-adjusts its ranges to fit the list block.
	// So it doesn't use the attribute_dict for our current range.
	// I originally had a lot of problems with attributes clobbering each other, so order used to matter.
	// I think some of those issues are improved, so order may not be as important now.
	cocoaTextParseBulletNumberListFormat(ih, formattag, text_view, native_selection_range);

	bool needs_paragraph_style_change = cocoaTextParseParagraphFormat(formattag, attribute_dict);
	bool needs_character_style_change = cocoaTextParseCharacterFormat(formattag, attribute_dict, iup_font);

	if(needs_character_style_change || needs_paragraph_style_change)
	{
		[text_storage setAttributes:attribute_dict range:native_selection_range];
	}

	
//	[text_storage endEditing];

	[text_view didChangeText];
	[undo_manager endUndoGrouping];
	

}

static int cocoaTextSetSelectionAttrib(Ihandle* ih, const char* value)
{
	if(ih->data->is_multiline)
	{
		NSUInteger start;
		NSUInteger end;
		NSTextView* text_view = cocoaTextGetTextView(ih);
		NSRange selection_range = NSMakeRange(0, 0);


		if (!value || iupStrEqualNoCase(value, "NONE"))
		{
//			start = 0;
//			end = 0;
		}
		else if (iupStrEqualNoCase(value, "ALL"))
		{
			NSTextStorage* text_storage = [text_view textStorage];
			
			start = 0;
			end = [text_storage length];
			selection_range = NSMakeRange(start, end);
		}
		else
		{
			int ret_val;
			NSUInteger lin_start=1;
			NSUInteger col_start=1;
			NSUInteger lin_end=1;
			NSUInteger col_end=1;

			ret_val = sscanf(value, "%lu,%lu:%lu,%lu", &lin_start, &col_start, &lin_end, &col_end);
		    if(ret_val != 4)
		    {
		    	return 0;
			}
			if(lin_start<1 || col_start<1 || lin_end<1 || col_end<1)
			{
				return 0;
			}

			bool did_find_range = cocoaTextComputeRangeFromLineColumnForTextView(text_view, lin_start, col_start, lin_end, col_end, &selection_range);

		}
		
		[text_view setSelectedRange:selection_range affinity:NSSelectionAffinityDownstream stillSelecting:NO];

		
	}
	else
	{
		NSUInteger start;
		NSUInteger end;
		NSRange selection_range = NSMakeRange(start, end);

		if (!value || iupStrEqualNoCase(value, "NONE"))
		{
			start = 0;
			end = 0;
		}
		else if (iupStrEqualNoCase(value, "ALL"))
		{
			start = 0;
			end = -1;  /* a negative value means all */
		}
		else
		{
			// FIXME: Iup is artificially constraining us to 32-bit by not supporting 64-bit variants.
			int start_int = 0;
			int end_int = 0;

			if(iupStrToIntInt(value, &start_int, &end_int, ':')!=2)
			{
				return 0;
			}
      		if(start_int<0 || end_int<0)
      		{
		        return 0;
			}
			start = (NSUInteger)start_int;
			end = (NSUInteger)end_int;
			selection_range = NSMakeRange(start, end-start);
		}
		NSLog(@"cocoaTextSetSelectionAttrib not implemented");

	}

	return 0;
}

static char* cocoaTextGetSelectionAttrib(Ihandle* ih)
{
#if 1
	if(ih->data->is_multiline)
	{
		NSTextView* text_view = cocoaTextGetTextView(ih);
		// Use selectedRanges to get an array of multiple selections if we ever have to handle that
		NSRange selected_range = [text_view selectedRange];
		if(NSNotFound == selected_range.location)
		{
			return NULL;
		}
		
		NSUInteger lin_start=1;
		NSUInteger col_start=1;
		NSUInteger lin_end=1;
		NSUInteger col_end=1;
		bool did_find_range = cocoaTextComputeLineColumnFromRangeForTextView(text_view, selected_range, &lin_start, &col_start, &lin_end, &col_end);
		if(did_find_range)
		{
			return iupStrReturnStrf("%lu,%lu:%lu,%lu", lin_start, col_start, lin_end, col_end);
		}
	}
	else
	{
		int start, end;
#if 0
		if (gtk_editable_get_selection_bounds(GTK_EDITABLE(ih->handle), &start, &end))
		{
			start++; /* IUP starts at 1 */
			end++;
			return iupStrReturnIntInt((int)start, (int)end, ':');
		}
#endif
		NSLog(@"cocoaTextGetSelectionAttrib not implemented");
	}
#endif

	return NULL;
}


static int cocoaTextSetSelectionPosAttrib(Ihandle* ih, const char* value)
{
	if(ih->data->is_multiline)
	{
		NSUInteger start;
		NSUInteger end;
		NSTextView* text_view = cocoaTextGetTextView(ih);
		NSRange selection_range = NSMakeRange(0, 0);


		if (!value || iupStrEqualNoCase(value, "NONE"))
		{
//			start = 0;
//			end = 0;
		}
		else if (iupStrEqualNoCase(value, "ALL"))
		{
			NSTextStorage* text_storage = [text_view textStorage];
			
			start = 0;
			end = [text_storage length];
			selection_range = NSMakeRange(start, end);
		}
		else
		{
			// FIXME: Iup is artificially constraining us to 32-bit by not supporting 64-bit variants.
			int start_int = 0;
			int end_int = 0;

			if(iupStrToIntInt(value, &start_int, &end_int, ':')!=2)
			{
				return 0;
			}
      		if(start_int<0 || end_int<0)
      		{
		        return 0;
			}
			start = (NSUInteger)start_int;
			end = (NSUInteger)end_int;
			selection_range = NSMakeRange(start, end-start);
		}
		
		[text_view setSelectedRange:selection_range affinity:NSSelectionAffinityDownstream stillSelecting:NO];

		
	}
	else
	{
		NSUInteger start;
		NSUInteger end;
		NSRange selection_range = NSMakeRange(start, end);

		if (!value || iupStrEqualNoCase(value, "NONE"))
		{
			start = 0;
			end = 0;
		}
		else if (iupStrEqualNoCase(value, "ALL"))
		{
			start = 0;
			end = -1;  /* a negative value means all */
		}
		else
		{
			// FIXME: Iup is artificially constraining us to 32-bit by not supporting 64-bit variants.
			int start_int = 0;
			int end_int = 0;

			if(iupStrToIntInt(value, &start_int, &end_int, ':')!=2)
			{
				return 0;
			}
      		if(start_int<0 || end_int<0)
      		{
		        return 0;
			}
			start = (NSUInteger)start_int;
			end = (NSUInteger)end_int;
			selection_range = NSMakeRange(start, end-start);
		}
		NSLog(@"cocoaTextSetSelectionAttrib not implemented");

	}

	return 0;
}

static char* cocoaTextGetSelectionPosAttrib(Ihandle* ih)
{
#if 1
	if(ih->data->is_multiline)
	{
		NSTextView* text_view = cocoaTextGetTextView(ih);
		// Use selectedRanges to get an array of multiple selections if we ever have to handle that
		NSRange selected_range = [text_view selectedRange];
		if(NSNotFound == selected_range.location)
		{
			return NULL;
		}
		NSUInteger start = selected_range.location;
		NSUInteger end = selected_range.location + selected_range.length;
		// FIXME: Iup is artificially constraining us to 32-bit by not supporting 64-bit variants.
		return iupStrReturnIntInt((int)start, (int)end, ':');

	}
	else
	{
		int start, end;
#if 0
		if (gtk_editable_get_selection_bounds(GTK_EDITABLE(ih->handle), &start, &end))
		{
			start++; /* IUP starts at 1 */
			end++;
			return iupStrReturnIntInt((int)start, (int)end, ':');
		}
#endif
		NSLog(@"cocoaTextGetSelectionAttrib not implemented");
	}
#endif

	return NULL;
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

// NSControl's setEnabled didn't seem to do anything on NSTextField. Also, NSTextView isn't an NSControl.
static int cocoaTextSetActiveAttrib(Ihandle* ih, const char* value)
{
	BOOL is_active = (BOOL)iupStrBoolean(value);
	
	IupCocoaTextSubType sub_type = cocoaTextGetSubType(ih);
	switch(sub_type)
	{
		case IUPCOCOATEXTSUBTYPE_VIEW:
		{
			NSTextView* text_view = cocoaTextGetTextView(ih);
			[text_view setSelectable:is_active];

			break;
		}
		case IUPCOCOATEXTSUBTYPE_FIELD:
		{
			NSTextField* text_field = cocoaTextGetTextField(ih);
			[text_field setSelectable:is_active];
			// This doesn't seem to have any visible effect
			[text_field setEnabled:is_active];

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

static char* cocoaTextGetActiveAttrib(Ihandle* ih)
{
	int is_active = true;
	
	IupCocoaTextSubType sub_type = cocoaTextGetSubType(ih);
	switch(sub_type)
	{
		case IUPCOCOATEXTSUBTYPE_VIEW:
		{
			NSTextView* text_view = cocoaTextGetTextView(ih);
			is_active = [text_view isSelectable];
			
			break;
		}
		case IUPCOCOATEXTSUBTYPE_FIELD:
		{
			NSTextField* text_field = cocoaTextGetTextField(ih);
//			is_active = [text_field isEditable];
			is_active = [text_field isSelectable];

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

	return iupStrReturnBoolean(is_active);
}

static int cocoaTextSetReadOnlyAttrib(Ihandle* ih, const char* value)
{
	BOOL is_editable = !(BOOL)iupStrBoolean(value);
	
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
	int is_editable = true;
	
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

static int cocoaTextSetAppendAttrib(Ihandle* ih, const char* value)
{
  if (!ih->handle)  /* do not do the action before map */
  {
    	return 0;
  }
	
	if(value == NULL)
	{
		return 0;
	}
	
  ih->data->disable_callbacks = 1;
  if(ih->data->is_multiline)
  {
	  NSTextView* text_view = cocoaTextGetTextView(ih);

	  NSTextStorage* text_storage = [text_view textStorage];

	  
	  NSString* ns_append_string = nil;
		IupCocoaFont* iup_font = iupCocoaGetFont(ih);

		// NSTextStorage is a NSMutableAttributedString
//	  NSMutableString* mutable_string = [text_storage mutableString];
	  
	  if(ih->data->append_newline && ([text_storage length] > 0))
	  {
		  ns_append_string = [NSString stringWithFormat:@"\n%s", value];


	  }
	  else
	  {
		  ns_append_string = [NSString stringWithUTF8String:value];

	  }
	  

	  
	  
	  NSAttributedString* attributed_append_string = [[NSAttributedString alloc] initWithString:ns_append_string attributes:[iup_font attributeDictionary]];
	  [attributed_append_string autorelease];

	  // We need to mark the proposed change range (before we change it) in order to call shouldChangeTextInRange:
	  NSRange change_range = NSMakeRange([text_storage length], 0);
	  ih->data->disable_callbacks = 1;
  	  [text_view shouldChangeTextInRange:change_range replacementString:[attributed_append_string string]];
	  [text_storage beginEditing];

	  [text_storage appendAttributedString:attributed_append_string];

	  ih->data->disable_callbacks = 0;

	  [text_storage endEditing];

		// We must call both shouldChangeTextInRange and didChangeText to keep the undo manager consistent
	  [text_view didChangeText];

  }
  else
  {
	  
	  IupCocoaTextSubType sub_type = cocoaTextGetSubType(ih);
	  switch(sub_type)
	  {
		  case IUPCOCOATEXTSUBTYPE_FIELD:
		  {
			  NSTextField* text_field = cocoaTextGetTextField(ih);
			  IupCocoaFont* iup_font = iupCocoaGetFont(ih);
			  // I don't know if the old string is attributed or not, so I need to assume it is
			  NSMutableAttributedString* old_string_value = [[[text_field attributedStringValue] mutableCopy] autorelease];
			  
			  NSString* ns_append_string = [NSString stringWithUTF8String:value];
			  
			  
			  NSAttributedString* attributed_append_string = [[NSAttributedString alloc] initWithString:ns_append_string attributes:[iup_font attributeDictionary]];
			  [attributed_append_string autorelease];
			  
			  [old_string_value appendAttributedString:attributed_append_string];
			  
			  
			  
			  break;
		  }
		  default:
		  {
			  break;
		  }
	  }
  }
	
  ih->data->disable_callbacks = 0;
  return 0;
}

static char* cocoaTextGetCountAttrib(Ihandle* ih)
{
	NSString* text_string = nil;

	IupCocoaTextSubType sub_type = cocoaTextGetSubType(ih);
	switch(sub_type)
	{
		case IUPCOCOATEXTSUBTYPE_VIEW:
		{
			NSTextView* text_view = cocoaTextGetTextView(ih);
			NSTextStorage* text_storage = [text_view textStorage];
			text_string = [text_storage string];
			

			
			break;
		}
		case IUPCOCOATEXTSUBTYPE_FIELD:
		{
			NSTextField* text_field = cocoaTextGetTextField(ih);
			text_string = [text_field stringValue];

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
	
	if((nil == text_string) || ([text_string length] == 0))
	{
		return iupStrReturnInt(0);
	}
	
	// GOTCHA: Modern characters may be multiple bytes (e.g. emoji characters).
	// Because of this, [string length] isn't correct, because it tells us the number of bytes, not characters.
	// The correct thing to do is to iterate through the string and count the glyphs.
	// But it probably is more expensive than what people think when they call this routine.
	// See https://www.objc.io/issues/9-strings/unicode/
	// NSString *s = @"The weather on \U0001F30D is \U0001F31E today.";
	// The weather on 🌍 is 🌞 today.

//	NSLog(@"length: %zu\n%@", [text_string length], text_string);

	NSRange full_range = NSMakeRange(0, [text_string length]);
	// Remember __block let's us modify this outside variable inside the block
	// https://developer.apple.com/library/content/documentation/Cocoa/Conceptual/Blocks/Articles/bxVariables.html#//apple_ref/doc/uid/TP40007502-CH6-SW1
	__block int character_count = 0;
	[text_string enumerateSubstringsInRange:full_range
		options:NSStringEnumerationByComposedCharacterSequences
		usingBlock:^(NSString* substring, NSRange substring_range,
		NSRange enclosing_range, BOOL* stop)
		{
			character_count++;
		}
	 ];
//	NSLog(@"final count: %d", character_count);
	return iupStrReturnInt(character_count);
}

static int cocoaTextSetScrollToPosAttrib(Ihandle* ih, const char* value)
{
  int pos = 0;

  if (!value)
    return 0;

  iupStrToInt(value, &pos);
  if (pos < 0) pos = 0;

  if (ih->data->is_multiline)
  {
	  NSTextView* text_view = cocoaTextGetTextView(ih);
	  
	  [text_view scrollRangeToVisible:NSMakeRange(pos, 0)];
  }
  else
  {
	  // I don't think this makes any sense to scroll other widgets
  }

  return 0;
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


static int cocoaTextSetContextMenuAttrib(Ihandle* ih, const char* value)
{
	Ihandle* menu_ih = (Ihandle*)value;
	
	IupCocoaTextSubType sub_type = cocoaTextGetSubType(ih);
	switch(sub_type)
	{
		case IUPCOCOATEXTSUBTYPE_VIEW:
		{
			NSTextView* text_view = cocoaTextGetTextView(ih);
			
			iupCocoaCommonBaseSetContextMenuForWidget(ih, text_view, menu_ih);


			break;
		}
		case IUPCOCOATEXTSUBTYPE_FIELD:
		{
			// Neither the field nor the cell work. I think I must change the field editor.
			NSTextField* text_field = cocoaTextGetTextField(ih);

			// We can't use iupCocoaCommonBaseSetContextMenuForWidget() because field editors are shared.
			// We will override textView:menu:forEvent:atIndex: and inject the menu options there.
			if(NULL != menu_ih)
			{
				// FIXME: The Menu might not be IupMap'd yet. (Presumably because we do not attach it directly to a dialog in this case.)
				// I think calling IupMap() is the correct thing to do and fixes the problem.
				// But this should be reviewed.
				if(NULL == menu_ih->handle)
				{
					IupMap(menu_ih);
				}
			}
			// Save the menu_ih so we can access it in the callback
			iupAttribSet(ih, "_COCOA_CONTEXT_MENU_IH", (const char*)menu_ih);

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
	
	
	
	
	return 1;
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
//		[scroll_view setAutoresizingMask:NSViewWidthSizable | NSViewHeightSizable];
		
		
		NSTextView* text_view = [[NSTextView alloc] initWithFrame:NSZeroRect];
//		NSTextView* text_view = [[NSTextView alloc] initWithFrame:NSMakeRect(0, 0, 400, 400)];

		
		text_view = [[NSTextView alloc] initWithFrame:NSMakeRect(0, 0, scrollview_content_size.width, scrollview_content_size.height)];

		[text_view setMinSize:NSMakeSize(0.0, 0.0)];
		[text_view setMaxSize:NSMakeSize(FLT_MAX, FLT_MAX)];
		[text_view setVerticallyResizable:YES];
		[text_view setHorizontallyResizable:NO];
		// This NSViewWidthSizable I still need despite disabling the others. Otherwise using SHRINK with horizontal scrollbars continues to grow and never shrink.
		[text_view setAutoresizingMask:NSViewWidthSizable];
//		[[text_view textContainer] setContainerSize:NSMakeSize(scrollview_content_size.width, FLT_MAX)];
//		[[text_view textContainer] setWidthTracksTextView:YES];
		// Needed to allow things like Cmd-E (put in search buffer), Cmd-G (find next), and the standard Find panel. Even if you don't want the standard find panel, a broken cmd-e/cmd-g is bad.
		[text_view setUsesFindPanel:YES];
		[text_view setAllowsUndo:YES];

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
			
			
//			[[text_view enclosingScrollView] setHasHorizontalScroller:YES];
			[[text_view enclosingScrollView] setHasHorizontalScroller:NO];
			[text_view setHorizontallyResizable:YES];
//			[text_view setAutoresizingMask:(NSViewWidthSizable | NSViewHeightSizable)];
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
//			[text_view setAutoresizingMask:(NSViewWidthSizable | NSViewHeightSizable)];
//			[[text_view textContainer] setContainerSize:NSMakeSize(FLT_MAX, FLT_MAX)];
//			[[text_view textContainer] setWidthTracksTextView:YES];
			
		}
		
		
		IupCocoaTextViewDelegate* text_view_delegate = [[IupCocoaTextViewDelegate alloc] init];
		[text_view setDelegate:text_view_delegate];
		objc_setAssociatedObject(text_view, IHANDLE_ASSOCIATED_OBJ_KEY, (id)ih, OBJC_ASSOCIATION_ASSIGN);
		// putting on the text_field just for storage. This is to make it consistent with the regular NSTextField (non-spinner) case.
		objc_setAssociatedObject(text_view, IUP_COCOA_TEXT_DELEGATE_OBJ_KEY, (id)text_view_delegate, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
		[text_view_delegate release];
		
		
		
		
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
		IupCocoaTextField* text_field = [[IupCocoaTextField alloc] initWithFrame:NSZeroRect];
//		NSTextField* text_field = [[NSTextField alloc] initWithFrame:NSMakeRect(0, 0, 200-19, 22)];

		
//		[container_view addSubview:text_field];
//		[text_field addSubview:stepper_view];

		[container_view addView:text_field inGravity:NSStackViewGravityLeading];
		[container_view addView:stepper_view inGravity:NSStackViewGravityTrailing];
		[stepper_view release];
		[text_field release];
		
//		[container_view setAutoresizingMask:(NSViewWidthSizable | NSViewHeightSizable)];
//		[text_field setAutoresizingMask:(NSViewWidthSizable)];
		
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
		// Note: top_level_objects must be explicitly retained if I want to keep it around. (I don't need to keep it around so I don't retain it.))
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
		
		// can't turn off decimals in IB, so do it here.
		{
			NSNumberFormatter* number_formatter = [text_field formatter];
			[number_formatter setMaximumFractionDigits:0];
		}

		// We're going to use OBJC_ASSOCIATION_RETAIN because I do believe it will do the right thing for us.
		objc_setAssociatedObject(stack_view, IUP_COCOA_TEXT_SPINNERCONTAINER_OBJ_KEY, (id)text_spinner_container, OBJC_ASSOCIATION_RETAIN_NONATOMIC);

		
		the_view = [stack_view retain];
		
		[text_spinner_container release];
		[files_owner release];
		// do not release top_level_objects because instantiateWithOwner did not retain (is is essentially autoreleased);
		[text_spinner_nib release];
		
		
		
		[stepper_view setValueWraps:(BOOL)iupAttribGetBoolean(ih, "SPINWRAP")];
		
		
		/*
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
*/
		IupCocoaTextSpinnerDelegate* text_spinner_delegate = [[IupCocoaTextSpinnerDelegate alloc] init];
//		[stepper_view setDelegate:text_spinner_delegate];
		[text_field setDelegate:text_spinner_delegate];

		[stepper_view setTarget:text_spinner_delegate];
		[stepper_view setAction:@selector(mySpinnerClickAction:)];
		

		objc_setAssociatedObject(stepper_view, IHANDLE_ASSOCIATED_OBJ_KEY, (id)ih, OBJC_ASSOCIATION_ASSIGN);
		objc_setAssociatedObject(text_field, IHANDLE_ASSOCIATED_OBJ_KEY, (id)ih, OBJC_ASSOCIATION_ASSIGN);
		// putting on the text_field just for storage. This is to make it consistent with the regular NSTextField (non-spinner) case.
		objc_setAssociatedObject(text_field, IUP_COCOA_TEXT_DELEGATE_OBJ_KEY, (id)text_spinner_delegate, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
		[text_spinner_delegate release];
		
		if(!iupAttribGetBoolean(ih, "SPINAUTO"))
		{
			// disconnect the bindings
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

			text_field = [[IupCocoaSecureTextField alloc] initWithFrame:NSZeroRect];
			//text_field = [[NSSecureTextField alloc] initWithFrame:NSMakeRect(0, 0, 140, 40)];
			[text_field setFont:[NSFont systemFontOfSize:0.0]];

		}
		else
		{
			
			//text_field = [[NSTextField alloc] initWithFrame:NSZeroRect];
			text_field = [[IupCocoaTextField alloc] initWithFrame:NSMakeRect(50, 50, 140, 40)];
			[text_field setFont:[NSFont systemFontOfSize:0.0]];

			
		}
		
		IupCocoaTextFieldDelegate* text_field_delegate = [[IupCocoaTextFieldDelegate alloc] init];
		[text_field setDelegate:text_field_delegate];
		objc_setAssociatedObject(text_field, IHANDLE_ASSOCIATED_OBJ_KEY, (id)ih, OBJC_ASSOCIATION_ASSIGN);
		objc_setAssociatedObject(text_field, IUP_COCOA_TEXT_DELEGATE_OBJ_KEY, (id)text_field_delegate, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
		[text_field_delegate release];

		
		// We must not allow IUP to EXPAND the height of the NSTextField so unset the bit flag if it is set.
		ih->expand = ih->expand & ~IUP_EXPAND_HEIGHT;
		
		/* formatting is never supported when MULTILINE=NO */
		ih->data->has_formatting = 0;
		
		
#ifdef USE_NSSTACKVIEW_TEXTFIELD_CONTAINER
		NSStackView* stack_view = [[NSStackView alloc] initWithFrame:NSZeroRect];
		[stack_view setOrientation:NSUserInterfaceLayoutOrientationVertical];
		[stack_view setAlignment:NSLayoutAttributeCenterX];
		[stack_view setDistribution:NSStackViewDistributionEqualCentering]; // requires 10.11. In a complicated IB test, I needed this to keep the widget vertically centerd. But in practice, I'm not sure if this is needed. Maybe we can use respondsToSelector if 10.9/10.10 compat is needed.
		[stack_view addView:text_field inGravity:NSStackViewGravityCenter];
		[text_field release];
		the_view = stack_view;
#elif defined(USE_CONTAINERVIEW_TEXTFIELD_CONTAINER)
		NSView* container_view = [[NSView alloc] initWithFrame:NSZeroRect];
		// Warning: This gets clobbered by AddParent
		[container_view setAutoresizingMask:NSViewMinXMargin|NSViewMaxXMargin|NSViewMinYMargin|NSViewMaxYMargin|NSViewWidthSizable|NSViewHeightSizable];
		[text_field setAutoresizingMask:NSViewMinXMargin|NSViewMaxXMargin|NSViewMinYMargin|NSViewMaxYMargin|NSViewWidthSizable];

		[container_view addSubview:text_field];
		[text_field release];
		the_view = container_view;
#else
		the_view = text_field;

#endif
		

	}
	
	
	
	
	
	
	ih->handle = the_view;
	

	// All Cocoa views shoud call this to add the new view to the parent view.
	iupCocoaAddToParent(ih);
	
	


#if 0
	/* configure for DRAG&DROP */
	if (IupGetCallback(ih, "DROPFILES_CB"))
		iupAttribSet(ih, "DROPFILESTARGET", "YES");
	
	/* update a mnemonic in a label if necessary */
	iupgtkUpdateMnemonic(ih);
#endif

	if(ih->data->formattags)
	{
		iupTextUpdateFormatTags(ih);
	}
	
	if(ih->data->is_multiline)
	{
		// Reset the undo manager so the user can't undo the initial setup.
		NSTextView* text_view = cocoaTextGetTextView(ih);
		NSUndoManager* undo_manager = [[text_view delegate] undoManagerForTextView:text_view];
		[undo_manager removeAllActions];
	}
	
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
	
	// Destroy the context menu ih it exists
	{
		Ihandle* context_menu_ih = (Ihandle*)iupCocoaCommonBaseGetContextMenuAttrib(ih);
		if(NULL != context_menu_ih)
		{
			IupDestroy(context_menu_ih);
		}
		iupCocoaCommonBaseSetContextMenuAttrib(ih, NULL);
	}
	
	// Because we used retain for the delegates, they should automatically release
	
	iupCocoaRemoveFromParent(ih);
	[the_view release];
	ih->handle = NULL;
}


// We need to override iTextComputeNaturalSizeMethod because the metrics only compute the raw string size and not the textbox area around it.
// On Mac, using the standard computation, we only see 3 'WWWW' instead of 5, presumably because the widget is including whitespace area for the surrounding box.
static void cocoaTextComputeNaturalSizeMethod(Ihandle* ih, int *w, int *h, int *children_expand)
{
	int natural_w = 0,
	natural_h = 0,
	visiblecolumns = iupAttribGetInt(ih, "VISIBLECOLUMNS"),
	visiblelines = iupAttribGetInt(ih, "VISIBLELINES");
	(void)children_expand; /* unset if not a container */
	int single_char_width;
	
	/* Since the contents can be changed by the user, the size can not be dependent on it. */
	iupdrvFontGetCharSize(ih, NULL, &natural_h);  /* one line height */
	natural_w = iupdrvFontGetStringWidth(ih, "WWWWWWWWWW");

	// Cocoa adjustment here: Let's make the width slightly bigger.
	// Since we see 3 out of 5 WWW's, let's pad with two more character widths.
	single_char_width = natural_w / 10;

	
	natural_w = (visiblecolumns*natural_w)/10;
	
	
	// Cocoa adjustment here
	natural_w += (single_char_width * 2);
	
	
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
		iupdrvTextAddBorders(ih, &natural_w, &natural_h);
	
	if (iupAttribGetBoolean(ih, "SPIN"))
		iupdrvTextAddSpin(ih, &natural_w, natural_h);
	
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
	ic->ComputeNaturalSize = cocoaTextComputeNaturalSizeMethod;

#if 0

  /* Driver Dependent Attribute functions */

  /* Visual */
  iupClassRegisterAttribute(ic, "BGCOLOR", NULL, gtkTextSetBgColorAttrib, IUPAF_SAMEASSYSTEM, "TXTBGCOLOR", IUPAF_DEFAULT);
#endif
  // need to override active behavior for text
  iupClassRegisterAttribute(ic, "ACTIVE", cocoaTextGetActiveAttrib, cocoaTextSetActiveAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_DEFAULT);

#if 0
  /* Special */
  iupClassRegisterAttribute(ic, "FGCOLOR", NULL, iupdrvBaseSetFgColorAttrib, IUPAF_SAMEASSYSTEM, "TXTFGCOLOR", IUPAF_DEFAULT);

  /* IupText only */
  iupClassRegisterAttribute(ic, "PADDING", iupTextGetPaddingAttrib, gtkTextSetPaddingAttrib, IUPAF_SAMEASSYSTEM, "0x0", IUPAF_NOT_MAPPED);
#endif
  iupClassRegisterAttribute(ic, "VALUE", cocoaTextGetValueAttrib, cocoaTextSetValueAttrib, NULL, NULL, IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);
#if 0
  iupClassRegisterAttribute(ic, "LINEVALUE", gtkTextGetLineValueAttrib, NULL, NULL, NULL, IUPAF_READONLY|IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SELECTEDTEXT", gtkTextGetSelectedTextAttrib, gtkTextSetSelectedTextAttrib, NULL, NULL, IUPAF_NO_INHERIT);
#endif
  iupClassRegisterAttribute(ic, "SELECTION", cocoaTextGetSelectionAttrib, cocoaTextSetSelectionAttrib, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SELECTIONPOS", cocoaTextGetSelectionPosAttrib, cocoaTextSetSelectionPosAttrib, NULL, NULL, IUPAF_NO_INHERIT);
#if 0
  iupClassRegisterAttribute(ic, "CARET", gtkTextGetCaretAttrib, gtkTextSetCaretAttrib, NULL, NULL, IUPAF_NO_SAVE|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "CARETPOS", gtkTextGetCaretPosAttrib, gtkTextSetCaretPosAttrib, IUPAF_SAMEASSYSTEM, "0", IUPAF_NO_SAVE|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "INSERT", NULL, gtkTextSetInsertAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
#endif
	
  iupClassRegisterAttribute(ic, "APPEND", NULL, cocoaTextSetAppendAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_WRITEONLY|IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "READONLY", cocoaTextGetReadOnlyAttrib, cocoaTextSetReadOnlyAttrib, NULL, NULL, IUPAF_DEFAULT);
#if 0
  iupClassRegisterAttribute(ic, "NC", iupTextGetNCAttrib, gtkTextSetNCAttrib, IUPAF_SAMEASSYSTEM, "0", IUPAF_NOT_MAPPED);
  iupClassRegisterAttribute(ic, "CLIPBOARD", NULL, gtkTextSetClipboardAttrib, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SCROLLTO", NULL, gtkTextSetScrollToAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
#endif
  iupClassRegisterAttribute(ic, "SCROLLTOPOS", NULL, cocoaTextSetScrollToPosAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
#if 0
  iupClassRegisterAttribute(ic, "SPINMIN", NULL, gtkTextSetSpinMinAttrib, IUPAF_SAMEASSYSTEM, "0", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SPINMAX", NULL, gtkTextSetSpinMaxAttrib, IUPAF_SAMEASSYSTEM, "100", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SPININC", NULL, gtkTextSetSpinIncAttrib, IUPAF_SAMEASSYSTEM, "1", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SPINVALUE", gtkTextGetSpinValueAttrib, gtkTextSetSpinValueAttrib, IUPAF_SAMEASSYSTEM, "0", IUPAF_NO_INHERIT);
#endif

  iupClassRegisterAttribute(ic, "COUNT", cocoaTextGetCountAttrib, NULL, NULL, NULL, IUPAF_READONLY|IUPAF_NO_INHERIT);
#if 0
	// TODO: What is a "line", e.g. does it include lines caused by wrapping?
	// https://developer.apple.com/library/content/documentation/Cocoa/Conceptual/TextLayout/Tasks/CountLines.html
  iupClassRegisterAttribute(ic, "LINECOUNT", gtkTextGetLineCountAttrib, NULL, NULL, NULL, IUPAF_READONLY|IUPAF_NO_INHERIT);
#endif

  /* IupText Windows and GTK only */
  iupClassRegisterAttribute(ic, "ADDFORMATTAG", NULL, iupTextSetAddFormatTagAttrib, NULL, NULL, IUPAF_IHANDLENAME|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "ADDFORMATTAG_HANDLE", NULL, iupTextSetAddFormatTagHandleAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "FORMATTING", iupTextGetFormattingAttrib, iupTextSetFormattingAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
#if 0
  iupClassRegisterAttribute(ic, "ALIGNMENT", NULL, gtkTextSetAlignmentAttrib, IUPAF_SAMEASSYSTEM, "ALEFT", IUPAF_NO_INHERIT);
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

	/* New API for view specific contextual menus (Mac only) */
	iupClassRegisterAttribute(ic, "CONTEXTMENU", iupCocoaCommonBaseGetContextMenuAttrib, cocoaTextSetContextMenuAttrib, NULL, NULL, IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);

}
