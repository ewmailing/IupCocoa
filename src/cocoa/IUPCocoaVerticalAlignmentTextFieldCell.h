
#import <Cocoa/Cocoa.h>

typedef NS_ENUM(NSUInteger, IUPTextVerticalAlignment)
{
	IUPTextVerticalAlignmentCenter = 0,
	IUPTextVerticalAlignmentTop = 1,
	IUPTextVerticalAlignmentBottom = 2
};

@interface IUPCocoaVerticalAlignmentTextFieldCell : NSTextFieldCell

@property(assign, nonatomic) IUPTextVerticalAlignment alignmentMode;

@end
