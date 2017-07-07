//
// See discussion at:
// http://stackoverflow.com/questions/11775128/set-text-vertical-center-in-nstextfield
// But we have to implement a more general version to deal with different vertical alignments.
//

#import "IUPCocoaVerticalAlignmentTextFieldCell.h"

@implementation IUPCocoaVerticalAlignmentTextFieldCell
@synthesize alignmentMode = alignmentMode;
@synthesize useWordWrap = useWordWrap;
@synthesize useEllipsis = useEllipsis;

- (NSRect) titleRectForBounds:(NSRect)cell_frame
{
	NSAttributedString* attributed_string = [self attributedStringValue];
	//	NSSize string_size = [attributed_string size];
	
	NSSize string_bounding_size = cell_frame.size;
	
	NSStringDrawingOptions string_draw_options = NSStringDrawingUsesLineFragmentOrigin;
	
	if(NO == [self useWordWrap])
	{
		string_bounding_size.width = CGFLOAT_MAX;
	}
	if([self useEllipsis])
	{
		string_draw_options |= NSStringDrawingTruncatesLastVisibleLine;
		
	}
	
	// 10.11+
	/*
	CGRect rect = [myLabel.text boundingRectWithSize:CGSizeMake(myLabel.frame.size.width, CGFLOAT_MAX)
											 options:NSStringDrawingUsesLineFragmentOrigin
										  attributes:@{NSFontAttributeName: myLabel.font}
											 context:nil];
	*/
	// Deprecated in 10.11.
	// TODO: Allow for font change (needs 10.11 API)
	NSRect string_rect = [attributed_string boundingRectWithSize:string_bounding_size options:string_draw_options];
	CGFloat string_height = string_rect.size.height;
	
	NSRect title_rect = [super titleRectForBounds:cell_frame];
	
	switch(alignmentMode)
	{
		case IUPTextVerticalAlignmentCenter:
		{
			title_rect.origin.y = cell_frame.origin.y + (cell_frame.size.height - string_height) * 0.5;
			
			// There is some discussion to change the height if the text height is greater that the available height.
			// But I think that would screw up IUP's sizing mechanisms.
			
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

@end
