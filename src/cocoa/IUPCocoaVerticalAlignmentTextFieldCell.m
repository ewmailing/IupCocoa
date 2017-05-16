//
// See discussion at:
// http://stackoverflow.com/questions/11775128/set-text-vertical-center-in-nstextfield
// But we have to implement a more general version to deal with different vertical alignments.
//

#import "IUPCocoaVerticalAlignmentTextFieldCell.h"

@implementation IUPCocoaVerticalAlignmentTextFieldCell
@synthesize alignmentMode = alignmentMode;

- (NSRect) titleRectForBounds:(NSRect)cell_frame
{
	NSAttributedString* attributed_string = [self attributedStringValue];
	NSSize string_size = [attributed_string size];
	CGFloat string_height = string_size.height;
	
	NSRect title_rect = [super titleRectForBounds:cell_frame];
	
	switch(alignmentMode)
	{
		case IUPTextVerticalAlignmentCenter:
		{
			
		//	CGFloat original_y = cell_frame.origin.y;
			title_rect.origin.y = cell_frame.origin.y + (cell_frame.size.height - string_height) * 0.5;
			
			// There is some discussion to change the height if the text height is greater that the available height.
			// But I think that would screw up IUP's sizing mechanisms.

			/*
			if (textRect.size.height < titleFrame.size.height) {
				titleFrame.origin.y = theRect.origin.y + (theRect.size.height - textRect.size.height) / 2.0;
				titleFrame.size.height = textRect.size.height;
			}
			
			title_rect.size.height = title_rect.size.height - (title_rect.origin.y - original_y);
			 */
			
			break;
		}
		case IUPTextVerticalAlignmentTop:
		{
			// this is the normal Cocoa behavior so we don't have to do anything
			break;
		}
		case IUPTextVerticalAlignmentBottom:
		{
			title_rect.origin.y = cell_frame.origin.y + (cell_frame.size.height - string_height);
			break;
		}
		default:
		{
			break;
		}
	}
	

	return title_rect;
	
	
}

- (void) drawInteriorWithFrame:(NSRect)cell_frame inView:(NSView*)control_view
{
	[super drawInteriorWithFrame:[self titleRectForBounds:cell_frame] inView:control_view];
}

- (void) selectWithFrame:(NSRect)cell_frame inView:(NSView*)control_view editor:(NSText*)text_obj delegate:(nullable id)the_delegate start:(NSInteger)sel_start length:(NSInteger)sel_length
{
	[super selectWithFrame:[self titleRectForBounds:cell_frame] inView:control_view editor:text_obj delegate:the_delegate start:sel_start length:sel_length];
}


#if 0
- (NSRect)drawingRectForBounds:(NSRect)theRect
{
	// Get the parent's idea of where we should draw
	NSRect newRect = [super drawingRectForBounds:theRect];

	// When the text field is being 
	// edited or selected, we have to turn off the magic because it screws up 
	// the configuration of the field editor.  We sneak around this by 
	// intercepting selectWithFrame and editWithFrame and sneaking a 
	// reduced, centered rect in at the last minute.
	if (mIsEditingOrSelecting == NO)
	{
		// Get our ideal size for current text
		NSSize textSize = [self cellSizeForBounds:theRect];

		// Center that in the proposed rect
		float heightDelta = newRect.size.height - textSize.height;	
		if (heightDelta > 0)
		{
			newRect.size.height -= heightDelta;
			newRect.origin.y += (heightDelta / 2);
		}
	}
	
	return newRect;
}

- (void)selectWithFrame:(NSRect)aRect inView:(NSView *)controlView editor:(NSText *)textObj delegate:(id)anObject start:(NSInteger)selStart length:(NSInteger)selLength
{
	aRect = [self drawingRectForBounds:aRect];
	mIsEditingOrSelecting = YES;	
	[super selectWithFrame:aRect inView:controlView editor:textObj delegate:anObject start:selStart length:selLength];
	mIsEditingOrSelecting = NO;
}

- (void)editWithFrame:(NSRect)aRect inView:(NSView *)controlView editor:(NSText *)textObj delegate:(id)anObject event:(NSEvent *)theEvent
{	
	aRect = [self drawingRectForBounds:aRect];
	mIsEditingOrSelecting = YES;
	[super editWithFrame:aRect inView:controlView editor:textObj delegate:anObject event:theEvent];
	mIsEditingOrSelecting = NO;
}
#endif

@end
