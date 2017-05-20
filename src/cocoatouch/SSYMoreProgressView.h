#import <UIKit/UIKit.h>

#if __has_feature(objc_arc)
#define WEAK weak
#else
#define WEAK unsafe_unretained
#endif


IB_DESIGNABLE

/*!
 @brief    Replacement for UIProgressView with additional features: 
 *indeterminate* mode, similar to NSProgressIndicator in macOS, adjustable
 thickness, and vertical orientation mode

 @details  The additional features are requirements of using this class in
 [iupCocoaTouch](https://github.com/ewmailing/IupCocoaTouch), the iOS
 port of [IUP](http://webserver2.tecgraf.puc-rio.br/iup/), a
 multi-platformtoolkit for building graphical user interfaces.
*/
@interface SSYMoreProgressView : UIProgressView

/*!
 @brief    Whether or not the receiver is in a special "indeterminate" state
 wherein the progress value is not displayed, presumably because it is unknown
 
 @details  Setting to YES sets the receiver's `progress` property to 0.0,
 draws 5 equally-spaced and equal-sized "slugs", of the receiver's current
 progress tint color (usually blue) in the receiver's track, and begins
 animating these slugs in a continuous motion to the right.  Setting to NO
 stops the animation and removes the slugs.
 */
@property (assign) IBInspectable BOOL indeterminate ;

/*!
 @brief    Factor by which to increase or decrease the thickness of the
 receiver's view (height if horizontal, width if vertical) from the value
 inherited from the base class UIProgressView (2.0 points in iOS 10.2).
 
 @details  Many people think that 2.0 points is too tiny to see, particularly
 on displays with the higher dot densities.
 */
@property (assign) IBInspectable CGFloat thicknessMultiple ;

/*!
 @brief    Indicates whether to flip the receiver's view so that it is
 oriented vertically, with progress flowing from bottom to top, instead
 of horizontally
 */
@property (assign) IBInspectable BOOL isVertical ;

@end
