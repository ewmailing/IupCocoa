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

#include "iup_childtree.h"

#include "iupcocoa_drv.h"

#import "IUPCocoaVerticalAlignmentTextFieldCell.h"


static int cocoaLabelSetPaddingAttrib(Ihandle* ih, const char* value)
{
	// Our Cocoa iupdrvbaseUpdateLayout contains a special case to handle padding. We just need to make sure the padding values get set here.
	// Other platforms seem to be skipping separators. We could theoretically support this since we are just manually computing offsets in iupdrvbaseUpdateLayout.
	if(ih->handle && ih->data->type != IUP_LABEL_SEP_HORIZ && ih->data->type != IUP_LABEL_SEP_VERT)
	{
		// I believe this sets the internal data structure values.
		iupStrToIntInt(value, &ih->data->horiz_padding, &ih->data->vert_padding, 'x');
		// HACK: I need to force a redraw. iupdrvbaseUpdateLayout queries the PADDING attribute, but it is not immediately set yet. So I'll force it to set now.
		iupAttribSetStr(ih, "PADDING", value);
		
		// Windows always calls iupdrvRedrawNow, and we need to too because the change won't update without it.
		// But this can require a new layout, so we need IupRefresh.
		IupRefresh(ih);
	}
	return 0;
}


static int cocoaLabelSetTitleAttrib(Ihandle* ih, const char* value)
{
	id the_label = ih->handle;
	if(the_label)
	{
		// This could be a NSTextField, some kind of image, or something else.
		
		if([the_label respondsToSelector:@selector(setStringValue:)])
		{
			NSString* ns_string = nil;
			if(value)
			{
				char* stripped_str = iupStrProcessMnemonic(value, NULL, 0);   /* remove & */

				// This will return nil if the string can't be converted.
				ns_string = [NSString stringWithUTF8String:stripped_str];
				
				if(stripped_str && stripped_str != value)
				{
					free(stripped_str);
				}
			}
			else
			{
				ns_string = @"";
			}
			// NSImageCells don't accept a stringValue
			id cell = [the_label respondsToSelector:@selector(cell)] ? [the_label cell] : nil;
			if (![cell isKindOfClass:[NSImageCell class]]) {
                // This will throw an exception for a nil string.
                [the_label setStringValue:ns_string];
 			}
			[the_label setNeedsDisplay:YES];
			[the_label updateCell:[the_label cell]];

			// I think I need to call this. I noticed in another program, when I suddenly set a long string, it seems to use the prior layout. This forces a relayout.
			IupRefresh(ih);
			
		}
	}
	return 1;

}


static char* cocoaLabelGetTitleAttrib(Ihandle* ih)
{
	id the_label = ih->handle;
	if(the_label)
	{
		// This could be a NSTextField, some kind of image, or something else.
		
		if([the_label respondsToSelector:@selector(setStringValue:)])
		{
			NSString* ns_string = [the_label stringValue];
			if(ns_string)
			{
				return iupStrReturnStr([ns_string UTF8String]);
			}
		}
	}
	return NULL;
	
}

static int cocoaLabelSetAlignmentAttrib(Ihandle* ih, const char* value)
{
	return 0;
	if(ih->data->type != IUP_LABEL_SEP_HORIZ && ih->data->type != IUP_LABEL_SEP_VERT)
	{
		if(ih->data->type == IUP_LABEL_TEXT)
		{
			NSTextField* the_label = (NSTextField*)ih->handle;
			// Note: We might be able to get away with any kind of NSControl
			NSCAssert([the_label isKindOfClass:[NSTextField class]], @"Expected NSTextField");

			char value1[30], value2[30];
			
			iupStrToStrStr(value, value1, value2, ':');

			
			if (iupStrEqualNoCase(value1, "ARIGHT"))
			{
				[the_label setAlignment:NSTextAlignmentRight];
			}
			else if (iupStrEqualNoCase(value1, "ACENTER"))
			{
				[the_label setAlignment:NSTextAlignmentCenter];
			}
			else /* "ALEFT" */
			{
				[the_label setAlignment:NSTextAlignmentLeft];

			}
			
			
			// Vertical alignment is not built into NSTextField.
			// We implemented our own custom NSTextFieldCell subclass to handle this case.
			
			if (iupStrEqualNoCase(value2, "ABOTTOM"))
			{
				NSCAssert([[the_label cell] isKindOfClass:[IUPCocoaVerticalAlignmentTextFieldCell class]], @"Expected IUPCocoaVerticalAlignmentTextFieldCell");
				IUPCocoaVerticalAlignmentTextFieldCell* vertical_alignment_cell = (IUPCocoaVerticalAlignmentTextFieldCell*)[the_label cell];
				[vertical_alignment_cell setAlignmentMode:IUPTextVerticalAlignmentBottom];
			}
			else if (iupStrEqualNoCase(value2, "ATOP"))
			{
				NSCAssert([[the_label cell] isKindOfClass:[IUPCocoaVerticalAlignmentTextFieldCell class]], @"Expected IUPCocoaVerticalAlignmentTextFieldCell");
				IUPCocoaVerticalAlignmentTextFieldCell* vertical_alignment_cell = (IUPCocoaVerticalAlignmentTextFieldCell*)[the_label cell];
				[vertical_alignment_cell setAlignmentMode:IUPTextVerticalAlignmentTop];
			}
			else  /* ACENTER (default) */
			{
				NSCAssert([[the_label cell] isKindOfClass:[IUPCocoaVerticalAlignmentTextFieldCell class]], @"Expected IUPCocoaVerticalAlignmentTextFieldCell");
				IUPCocoaVerticalAlignmentTextFieldCell* vertical_alignment_cell = (IUPCocoaVerticalAlignmentTextFieldCell*)[the_label cell];
				[vertical_alignment_cell setAlignmentMode:IUPTextVerticalAlignmentCenter];

			}
			[the_label setNeedsDisplay:YES];
			[the_label updateCell:[the_label cell]];

			return 1;
		}
		else if(ih->data->type == IUP_LABEL_IMAGE)
		{
			NSImageView* the_label = (NSImageView*)ih->handle;
			// Note: We might be able to get away with any kind of NSControl
			NSCAssert([the_label isKindOfClass:[NSImageView class]], @"Expected NSImageView");
			
			char value1[30], value2[30];
			
			iupStrToStrStr(value, value1, value2, ':');
			
			
			if(iupStrEqualNoCase(value1, "ARIGHT"))
			{
				if(iupStrEqualNoCase(value2, "ABOTTOM"))
				{
					[the_label setImageAlignment:NSImageAlignBottomRight];
				}
				else if (iupStrEqualNoCase(value2, "ATOP"))
				{
					[the_label setImageAlignment:NSImageAlignTopRight];
				}
				else  /* ACENTER */
				{
					[the_label setImageAlignment:NSImageAlignRight];
				}
			}
			else if (iupStrEqualNoCase(value1, "ACENTER"))
			{
				if(iupStrEqualNoCase(value2, "ABOTTOM"))
				{
					[the_label setImageAlignment:NSImageAlignBottom];
				}
				else if (iupStrEqualNoCase(value2, "ATOP"))
				{
					[the_label setImageAlignment:NSImageAlignTop];
				}
				else  /* ACENTER */
				{
					[the_label setImageAlignment:NSImageAlignCenter];
				}
			}
			else /* "ALEFT" */
			{
				if(iupStrEqualNoCase(value2, "ABOTTOM"))
				{
					[the_label setImageAlignment:NSImageAlignBottomLeft];
				}
				else if (iupStrEqualNoCase(value2, "ATOP"))
				{
					[the_label setImageAlignment:NSImageAlignTopLeft];
				}
				else  /* ACENTER */
				{
					[the_label setImageAlignment:NSImageAlignLeft];
				}
			}
			
			
			return 1;
		}
	}
	
	return 0;
}

// Warning: The pre-10.10 behavior never behaved well. Maybe it should be removed.
static int cocoaLabelSetWordWrapAttrib(Ihandle* ih, const char* value)
{
	if (ih->data->type == IUP_LABEL_TEXT)
	{
		NSTextField* the_label = (NSTextField*)ih->handle;
		// Note: We might be able to get away with any kind of NSControl
		NSCAssert([the_label isKindOfClass:[NSTextField class]], @"Expected NSTextField");
		if(iupStrBoolean(value))
		{
			// setLineBreakMode Requires 10.10+. Allows for both word wrapping and different ellipsis behaviors.
			if([the_label respondsToSelector:@selector(setLineBreakMode:)])
			{
				[the_label setLineBreakMode:NSLineBreakByWordWrapping];
			}
			else
			{

				char* ellipsis_state = iupAttribGet(ih, "ELLIPSIS");
				if(iupStrBoolean(ellipsis_state))
				{
					// Ellipsis only seem to appear when multiline is enabled
					[the_label setUsesSingleLineMode:NO];
					[[the_label cell] setScrollable:NO];
					
					[[the_label cell] setWraps:YES];
					[[the_label cell] setLineBreakMode:NSLineBreakByTruncatingTail];
					[[the_label cell] setTruncatesLastVisibleLine:YES];
				}
				else
				{
					[the_label setUsesSingleLineMode:NO];
					[[the_label cell] setScrollable:NO];
					
					[[the_label cell] setWraps:YES];
					[[the_label cell] setLineBreakMode:NSLineBreakByWordWrapping];
					[[the_label cell] setTruncatesLastVisibleLine:NO];
				}
				
			}
			
		}
		else
		{
			// setLineBreakMode Requires 10.10+. Allows for both word wrapping and different ellipsis behaviors.
			if([the_label respondsToSelector:@selector(setLineBreakMode:)])
			{
				// Wrapping and ellipsis are mutually exclusive
				char* ellipsis_state = iupAttribGet(ih, "ELLIPSIS");
				if(iupStrBoolean(ellipsis_state))
				{
					[the_label setLineBreakMode:NSLineBreakByTruncatingTail];
				}
				else
				{
					[the_label setLineBreakMode:NSLineBreakByClipping];
				}
			}
			else
			{
				
				char* ellipsis_state = iupAttribGet(ih, "ELLIPSIS");
				if(iupStrBoolean(ellipsis_state))
				{
					// Ellipsis only seem to appear when multiline is enabled
					[the_label setUsesSingleLineMode:NO];
					[[the_label cell] setScrollable:NO];
					
					[[the_label cell] setWraps:YES];
					[[the_label cell] setLineBreakMode:NSLineBreakByWordWrapping];
					[[the_label cell] setTruncatesLastVisibleLine:YES];
				}
				else
				{
					[the_label setUsesSingleLineMode:YES];
					[[the_label cell] setScrollable:YES];
					
					[[the_label cell] setWraps:NO];
					[[the_label cell] setLineBreakMode:NSLineBreakByClipping];
					[[the_label cell] setTruncatesLastVisibleLine:NO];
				}
				
			}
		}
		[the_label setNeedsDisplay:YES];
		[the_label updateCell:[the_label cell]];
		return 1;
	}
	return 0;
}


/*
This is a terrible workaround for a Mac bug.
In creating our own subclass for NSTextFieldCell to provide vertical alignment, 
we have hit a Mac bug.
When we first create a multi-line word-wrapped label that is top-aligned,
Mac seems to draw it in the wrong place.
Even though our overrides in our subclass are producing the correct values,
on the very initial creation, the cell is positioned too low (about midway).
Calling setNeedsDisplay:YES, updateCell:, and all sorts of things to trigger a redraw/relayout do nothing.
It almost feels like that Mac is caching the image and some other place in the renderer is putting it in the wrong position
because the parts I tried overriding in the cell subclass seem to be producing all the correct values.
The one thing that seems to fix the position is clicking on the label after it has been displayed.
And when I click on it, nothing in I can see in the Cell, e.g. drawInteriorWithFrame get triggered, which is another reason it feels like it is some deferred Mac rendering bug.
So, as a workaround, we need to simulate a mouse-click on the label when first displayed to force Mac to draw it in the correct place.
Simulating the mouse-click caused its own problems.
mouseDown: warns that the parameter needs to be non-nil. However, in my attempts to create a proper NSEvent to pass,
I discovered that I would only sometimes correct the bug, and other times, the call seems to enter an event loop and never returns and never creates a window.
Or sometimes nothing would happen at all.
I discovered calling performSelector:withObject:afterDelay with some delay increased chances of it working.
However, using a debugger/breakpoints would frequently break things.
Eventually, I tried just passing nil as the events, and everything worked, and without needing a delay.
*/
 
 
#import <Carbon/Carbon.h>


static void fixInitialVerticalAlignment(Ihandle* ih)
{
	if (ih->data->type == IUP_LABEL_TEXT)
	{
		NSTextField* the_label = (NSTextField*)ih->handle;
		// Note: We might be able to get away with any kind of NSControl
		NSCAssert([the_label isKindOfClass:[NSTextField class]], @"Expected NSTextField");
		
		//		[[the_label cell] performClick:nil];
		//		[the_label  performClick:nil];
		
		NSPoint click_location = [the_label frame].origin;
		
		NSWindow* the_window = [the_label window];
		//		click_location = [the_window convertScreenToBase:click_location];
		click_location = [the_label convertPoint:click_location toView:nil];
		
		NSInteger window_id = [the_window windowNumber];
		NSGraphicsContext* graphics_context = [NSGraphicsContext currentContext];
		NSEvent* mouseDownEvent = [NSEvent mouseEventWithType:NSLeftMouseDown location:click_location modifierFlags:0 timestamp:GetCurrentEventTime() windowNumber:window_id context:graphics_context eventNumber: 0 clickCount:1 pressure:0];
		NSEvent* mouseUpEvent = [NSEvent mouseEventWithType:NSLeftMouseUp location:click_location modifierFlags:0 timestamp:GetCurrentEventTime() windowNumber:window_id context:graphics_context eventNumber: 0 clickCount:1 pressure:0];
		
		/*
		 NSEvent* keyDownEvent = [NSEvent keyEventWithType:NSEventTypeKeyDown location:click_location modifierFlags:0 timestamp:GetCurrentEventTime() windowNumber:window_id context:graphics_context characters:@"a" charactersIgnoringModifiers:@"" isARepeat:NO keyCode:20];
		 NSEvent* keyUpEvent = [NSEvent keyEventWithType:NSEventTypeKeyUp location:click_location modifierFlags:0 timestamp:GetCurrentEventTime() windowNumber:window_id context:graphics_context characters:@"a" charactersIgnoringModifiers:@"" isARepeat:NO keyCode:20];
		 
		 
		 
		 
		 
		 [the_label keyDown:keyDownEvent];
		 [the_label keyUp:keyUpEvent];
		 */
		//	[the_label mouseDown:mouseDownEvent];
		//	[the_label mouseDown:mouseUpEvent];
		[the_label mouseDown:nil];
		[the_label mouseUp:nil];
//		NSRange selected_range = [[the_label currentEditor] selectedRange];
//		[[the_label currentEditor] setSelectedRange:NSMakeRange(selected_range.length, 0)];

	}
	
}


@implementation NSTextField (IupCocoaFixIt)


- (void) fixInitialVerticalAlignment:(id)the_object
{
	NSTextField* the_label = (NSTextField*)the_object;
	// Note: We might be able to get away with any kind of NSControl
	NSCAssert([the_label isKindOfClass:[NSTextField class]], @"Expected NSTextField");
	
	//		[[the_label cell] performClick:nil];
	//		[the_label  performClick:nil];
	
	NSPoint click_location = [the_label frame].origin;
	
	NSWindow* the_window = [the_label window];
	//		click_location = [the_window convertScreenToBase:click_location];
	click_location = [the_label convertPoint:click_location toView:nil];
	
	NSInteger window_id = [the_window windowNumber];
	NSGraphicsContext* graphics_context = [NSGraphicsContext currentContext];
	NSEvent* mouseDownEvent = [NSEvent mouseEventWithType:NSLeftMouseDown location:click_location modifierFlags:0 timestamp:GetCurrentEventTime() windowNumber:window_id context:graphics_context eventNumber: 0 clickCount:1 pressure:0];
	NSEvent* mouseUpEvent = [NSEvent mouseEventWithType:NSLeftMouseUp location:click_location modifierFlags:0 timestamp:GetCurrentEventTime() windowNumber:window_id context:graphics_context eventNumber: 0 clickCount:1 pressure:0];
	
	/*
	NSEvent* keyDownEvent = [NSEvent keyEventWithType:NSEventTypeKeyDown location:click_location modifierFlags:0 timestamp:GetCurrentEventTime() windowNumber:window_id context:graphics_context characters:@"a" charactersIgnoringModifiers:@"" isARepeat:NO keyCode:20];
	NSEvent* keyUpEvent = [NSEvent keyEventWithType:NSEventTypeKeyUp location:click_location modifierFlags:0 timestamp:GetCurrentEventTime() windowNumber:window_id context:graphics_context characters:@"a" charactersIgnoringModifiers:@"" isARepeat:NO keyCode:20];




	
	[the_label keyDown:keyDownEvent];
	[the_label keyUp:keyUpEvent];
	 */
	// Ugh: Sending generated events only some times works for me. I think it only works if the window is fully created and active.
	// Otherwise the process seems to get stuck never creating the window.
//	[the_label mouseDown:mouseDownEvent];
//	[the_label mouseUp:mouseUpEvent];
	// I know the API says non-nil, but this is the only thing that seems to work.
	[the_label mouseDown:nil];
	[the_label mouseUp:nil];
	//		NSRange selected_range = [[the_label currentEditor] selectedRange];
	//		[[the_label currentEditor] setSelectedRange:NSMakeRange(selected_range.length, 0)];
}

@end

// Warning: The pre-10.10 behavior never behaved well. Maybe it should be removed.
static int cocoaLabelSetEllipsisAttrib(Ihandle* ih, const char* value)
{
	if (ih->data->type == IUP_LABEL_TEXT)
	{
		NSTextField* the_label = (NSTextField*)ih->handle;
		// Note: We might be able to get away with any kind of NSControl
		NSCAssert([the_label isKindOfClass:[NSTextField class]], @"Expected NSTextField");

		
		/*
		[[the_label cell] setScrollable:YES];
		[[the_label cell] setWraps:NO];
		
		[[the_label cell] setScrollable:NO];
		[[the_label cell] setWraps:YES];
		
		[the_label setSelectable:NO];
		[the_label setSelectable:YES];
		
		[the_label setBezeled:YES];
		[the_label setBordered:YES];
		//			[the_label setDrawsBackground:NO];
		[the_label setDrawsBackground:NO]; // sometimes helpful for debugging layout issues
		NSString* str_value = [the_label stringValue];
		[the_label setStringValue:@"blah"]; // sometimes helpful for debugging layout issues

		[the_label setStringValue:str_value];
		 */
	//	[the_label selectText:nil];
//		[the_label mouseDown:nil];
		
		return 1;
		

		if(iupStrBoolean(value))
		{
			// setLineBreakMode Requires 10.10+. Allows for both word wrapping and different ellipsis behaviors.
			if([the_label respondsToSelector:@selector(setLineBreakMode:)])
			{
				// Wrapping and ellipsis are mutually exclusive
				// TODO: Expose different ellipsis modes to public API
				[the_label setLineBreakMode:NSLineBreakByTruncatingTail];

			}
			else
			{
				// Ellipsis only seem to appear when multiline is enabled
				[the_label setUsesSingleLineMode:NO];
				[[the_label cell] setScrollable:NO];
				
				[[the_label cell] setWraps:YES];
				[[the_label cell] setLineBreakMode:NSLineBreakByWordWrapping];
				[[the_label cell] setTruncatesLastVisibleLine:YES];
				
			}
		}
		else
		{
			// setLineBreakMode Requires 10.10+. Allows for both word wrapping and different ellipsis behaviors.
			if([the_label respondsToSelector:@selector(setLineBreakMode:)])
			{
				// Wrapping and ellipsis are mutually exclusive

				char* wordwrap_state = iupAttribGet(ih, "WORDWRAP");
				if(iupStrBoolean(wordwrap_state))
				{
					[the_label setLineBreakMode:NSLineBreakByWordWrapping];
				}
				else
				{
					[the_label setLineBreakMode:NSLineBreakByClipping];
				}
				
			}
			else
			{
				
				char* wordwrap_state = iupAttribGet(ih, "WORDWRAP");
				if(iupStrBoolean(wordwrap_state))
				{
					[the_label setUsesSingleLineMode:NO];
					[[the_label cell] setScrollable:NO];
					
					[[the_label cell] setWraps:YES];
					[[the_label cell] setLineBreakMode:NSLineBreakByWordWrapping];
					[[the_label cell] setTruncatesLastVisibleLine:YES];
				}
				else
				{
					[the_label setUsesSingleLineMode:YES];
					[[the_label cell] setScrollable:YES];
					
					[[the_label cell] setWraps:NO];
					[[the_label cell] setLineBreakMode:NSLineBreakByClipping];
					[[the_label cell] setTruncatesLastVisibleLine:NO];
				}
								
			}

		
		}
		return 1;
	}
	return 0;
}


static int cocoaLabelMapMethod(Ihandle* ih)
{
	char* value;
	// using id because we may be using different types depending on the case
	id the_label = nil;
	
	value = iupAttribGet(ih, "SEPARATOR");
	if (value)
	{
		if (iupStrEqualNoCase(value, "HORIZONTAL"))
		{
			ih->data->type = IUP_LABEL_SEP_HORIZ;

//			NSBox* horizontal_separator= [[NSBox alloc] initWithFrame:NSMakeRect(20.0, 20.0, 250.0, 1.0)];
			NSBox* horizontal_separator= [[NSBox alloc] initWithFrame:NSMakeRect(0.0, 0.0, 250.0, 1.0)];
			[horizontal_separator setBoxType:NSBoxSeparator];
			the_label = horizontal_separator;
			
		}
		else /* "VERTICAL" */
		{
			ih->data->type = IUP_LABEL_SEP_VERT;

//			NSBox* vertical_separator=[[NSBox alloc] initWithFrame:NSMakeRect(20.0, 20.0, 1.0, 250.0)];
			NSBox* vertical_separator=[[NSBox alloc] initWithFrame:NSMakeRect(0.0, 0.0, 1.0, 250.0)];
			[vertical_separator setBoxType:NSBoxSeparator];
			the_label = vertical_separator;

		}
	}
	else
	{
		value = iupAttribGet(ih, "IMAGE");
		if (value)
		{
			ih->data->type = IUP_LABEL_IMAGE;
			
			char *name;
			int make_inactive = 0;
			
			if (iupdrvIsActive(ih))
    name = iupAttribGet(ih, "IMAGE");
			else
			{
    name = iupAttribGet(ih, "IMINACTIVE");
    if (!name)
	{
		name = iupAttribGet(ih, "IMAGE");
		make_inactive = 1;
	}
			}
			
			
			id the_bitmap;
			the_bitmap = iupImageGetImage(name, ih, make_inactive);
			int width;
			int height;
			int bpp;
			
			iupdrvImageGetInfo(the_bitmap, &width, &height, &bpp);

//			static int woffset = 0;
//			static int hoffset = 0;
			
			NSImageView* image_view = [[NSImageView alloc] initWithFrame:NSMakeRect(0, 0, width, height)];
//			NSImageView* image_view = [[NSImageView alloc] initWithFrame:NSMakeRect(woffset, hoffset, width, height)];
			[image_view setImage:the_bitmap];
			
//			woffset += 30;
//			hoffset += 30;
			
			the_label = image_view;
			
#if 0
			if (!the_bitmap)
					return;
			
			/* must use this info, since image can be a driver image loaded from resources */
			iupdrvImageGetInfo(hBitmap, &width, &height, &bpp);

			
			NSBitmapImageRep* bitmap_image = [[NSBitmapImageRep alloc]
									 initWithBitmapDataPlanes:NULL
									 pixelsWide: width
									 pixelsHigh: height
									 bitsPerSample: 8
									 samplesPerPixel: 4
									 hasAlpha: YES
									 isPlanar: NO
									 colorSpaceName: NSCalibratedRGBColorSpace
									 bytesPerRow: width * 4
									 bitsPerPixel: 32]
#endif

		}
		else
		{
			ih->data->type = IUP_LABEL_TEXT;

			the_label = [[NSTextField alloc] initWithFrame:NSZeroRect];
//			the_label = [[NSTextField alloc] initWithFrame:NSMakeRect(0, 0, 100, 100)];

#if 1
			IUPCocoaVerticalAlignmentTextFieldCell* textfield_cell = [[IUPCocoaVerticalAlignmentTextFieldCell alloc] initTextCell:@""];
			[the_label setCell:textfield_cell];
			[textfield_cell release];
			//[textfield_cell setScrollable:NO];
			
//			[textfield_cell performClick:nil];
			
//			[textfield_cell setAlignmentMode:IUPTextVerticalAlignmentTop];
			
#endif
			
			
			[the_label setBezeled:NO];
//			[the_label setDrawsBackground:NO];
			[the_label setDrawsBackground:YES]; // sometimes helpful for debugging layout issues
			[the_label setEditable:NO];
//			[the_label setSelectable:NO];
			// TODO: FEATURE: I think this is really convenient for users so it should be the default
			[the_label setSelectable:YES];
			
//			NSFont* the_font = [the_label font];
//			NSLog(@"font %@", the_font);
			[the_label setFont:[NSFont systemFontOfSize:0.0]];

			
			
#if 1
			if([the_label respondsToSelector:@selector(setLineBreakMode:)])
			{
				[the_label setLineBreakMode:NSLineBreakByClipping];
				
			}
			else
			{
				
				[[the_label cell] setTruncatesLastVisibleLine:NO];


				
				
				[the_label setUsesSingleLineMode:YES];
				[[the_label cell] setScrollable:YES];
				
				[[the_label cell] setWraps:NO];
				[[the_label cell] setLineBreakMode:NSLineBreakByClipping];
				[[the_label cell] setTruncatesLastVisibleLine:NO];
				
			}
			
			
	
#else
			
			
			[the_label setUsesSingleLineMode:NO];
			[[the_label cell] setWraps:YES];
			[[the_label cell] setScrollable:NO];
			
//			[[the_label cell] setTruncatesLastVisibleLine:YES];

			// setLineBreakMode Requires 10.10+. Allows for both word wrapping and different ellipsis behaviors.
//			[the_label setLineBreakMode:NSLineBreakByWordWrapping];
#endif
			
			[the_label setNeedsDisplay:YES];
			[the_label updateCell:[the_label cell]];
			
			IupSetInt(ih, "_FIRST_SHOW_FIX", 1);

			[the_label performSelector:@selector(fixInitialVerticalAlignment:) withObject:the_label afterDelay:0.0];

		}
	}
	
	if (!the_label)
	{
		return IUP_ERROR;
	}
	
	
	ih->handle = the_label;

	
	
	/* add to the parent, all GTK controls must call this. */
//	iupgtkAddToParent(ih);
	
	
//	Ihandle* ih_parent = ih->parent;
//	id parent_native_handle = ih_parent->handle;
	
	iupCocoaAddToParent(ih);
	
	
	/* configure for DRAG&DROP of files */
	if (IupGetCallback(ih, "DROPFILES_CB"))
	{
		iupAttribSet(ih, "DROPFILESTARGET", "YES");
	}
	
//	fixInitialVerticalAlignment(ih);
	
	return IUP_NOERROR;
}


static void cocoaLabelUnMapMethod(Ihandle* ih)
{
	id the_label = ih->handle;
	iupCocoaRemoveFromParent(ih);
	[the_label release];
	ih->handle = nil;

}

int cocoaLabelSetRasterSizeAttrib(Ihandle* ih, const char* value)
{
	iupBaseSetRasterSizeAttrib(ih, value);
	if (ih->data->type == IUP_LABEL_TEXT)
	{
		NSTextField* the_label = (NSTextField*)ih->handle;
		// Note: We might be able to get away with any kind of NSControl
		NSCAssert([the_label isKindOfClass:[NSTextField class]], @"Expected NSTextField");
		

		[the_label setNeedsDisplay:YES];
		[the_label updateCell:[the_label cell]];
	}
	return 0;
}

#include "iup_classbase.h"

void cocoaLabelLayoutUpdate(Ihandle *ih)
{
	iupdrvBaseLayoutUpdateMethod(ih);
	
	int needs_first_shown_vertical_align_fix = IupGetInt(ih, "_FIRST_SHOW_FIX");

	if(needs_first_shown_vertical_align_fix)
	{
		NSTextField* the_label = (NSTextField*)ih->handle;
		// Note: We might be able to get away with any kind of NSControl
		NSCAssert([the_label isKindOfClass:[NSTextField class]], @"Expected NSTextField");
		

//		[the_label performSelector:@selector(fixInitialVerticalAlignment:) withObject:the_label afterDelay:0.0];

		
		//fixInitialVerticalAlignment(ih);
		IupSetInt(ih, "_FIRST_SHOW_FIX", 0);

	}
}

void iupdrvLabelInitClass(Iclass* ic)
{
  /* Driver Dependent Class functions */
  ic->Map = cocoaLabelMapMethod;
	ic->UnMap = cocoaLabelUnMapMethod;
	
	ic->LayoutUpdate = cocoaLabelLayoutUpdate;

#if 0

  /* Driver Dependent Attribute functions */

  /* Overwrite Visual */
  iupClassRegisterAttribute(ic, "ACTIVE", iupBaseGetActiveAttrib, gtkLabelSetActiveAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_DEFAULT);

  /* Visual */
  iupClassRegisterAttribute(ic, "BGCOLOR", iupBaseNativeParentGetBgColorAttrib, gtkLabelSetBgColorAttrib, IUPAF_SAMEASSYSTEM, "DLGBGCOLOR", IUPAF_DEFAULT);

  /* Special */
  iupClassRegisterAttribute(ic, "FGCOLOR", NULL, iupdrvBaseSetFgColorAttrib, IUPAF_SAMEASSYSTEM, "DLGFGCOLOR", IUPAF_DEFAULT);
	
#endif
	
  iupClassRegisterAttribute(ic, "TITLE", cocoaLabelGetTitleAttrib, cocoaLabelSetTitleAttrib, NULL, NULL, IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);
  /* IupLabel only */
  iupClassRegisterAttribute(ic, "ALIGNMENT", NULL, cocoaLabelSetAlignmentAttrib, "ALEFT:ACENTER", NULL, IUPAF_NO_INHERIT);  /* force new default value */
#if 0
  iupClassRegisterAttribute(ic, "IMAGE", NULL, gtkLabelSetImageAttrib, NULL, NULL, IUPAF_IHANDLENAME|IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);
#endif
  iupClassRegisterAttribute(ic, "PADDING", iupLabelGetPaddingAttrib, cocoaLabelSetPaddingAttrib, IUPAF_SAMEASSYSTEM, "0x0", IUPAF_NOT_MAPPED);
#if 0
  /* IupLabel GTK and Motif only */
  iupClassRegisterAttribute(ic, "IMINACTIVE", NULL, gtkLabelSetImInactiveAttrib, NULL, NULL, IUPAF_IHANDLENAME|IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);
#endif
	
  /* IupLabel Windows and GTK only */
  iupClassRegisterAttribute(ic, "WORDWRAP", NULL, cocoaLabelSetWordWrapAttrib, NULL, NULL, IUPAF_DEFAULT);
  iupClassRegisterAttribute(ic, "ELLIPSIS", NULL, cocoaLabelSetEllipsisAttrib, NULL, NULL, IUPAF_DEFAULT);

#if 0
  /* IupLabel GTK only */
  iupClassRegisterAttribute(ic, "MARKUP", NULL, NULL, NULL, NULL, IUPAF_DEFAULT);
#endif
	
//	iupClassRegisterAttribute(ic, "SIZE", iupBaseGetSizeAttrib, iupBaseSetSizeAttrib, NULL, NULL, IUPAF_NO_SAVE|IUPAF_NO_DEFAULTVALUE|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
	iupClassRegisterAttribute(ic, "RASTERSIZE", iupBaseGetRasterSizeAttrib, cocoaLabelSetRasterSizeAttrib, NULL, NULL, IUPAF_NO_SAVE|IUPAF_NO_DEFAULTVALUE|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);


	

}
