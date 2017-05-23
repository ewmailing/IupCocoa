/** \file
* \brief Progress bar Control
*
* See Copyright Notice in "iup.h"
*/

#include <UIKit/UIKit.h>

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
#include "iup_progressbar.h"
#include "iup_drv.h"

#include "iupcocoatouch_drv.h"

@interface IUPProgressBarMarqueeDelegate : NSObject <CALayerDelegate>

@property (unsafe_unretained) UIProgressView* progressView;
@property (readonly) CGFloat slugWidth;
@property (readonly) CGFloat animationDistance;

@end

@implementation IUPProgressBarMarqueeDelegate

- (CGFloat)slugWidth
{
    /* Cosmetic decision: How many slugs do you want in the bar?
     I like 5, plus 5 spaces in between slugs. */
    return self.progressView.bounds.size.width / 10;
}

- (CGFloat)animationDistance
{
    return 2 * self.slugWidth;
}

- (NSTimeInterval)animationCycleDuration
{
    /* Another cosmetic decision.  Increase if you like slower animation. */
    return 0.5;
}

- (void)drawLayer:(CALayer*)layer
        inContext:(CGContextRef)context
{
    CGColorRef color = [self.progressView.progressTintColor CGColor];
    if (!color)
    {
        color = [self.progressView.tintColor CGColor];
    }
    CGContextSetFillColorWithColor(context, color);

    // See Notes : ProgressTrackColor
    CGFloat x = 0.0;
    CGFloat overallWidth = self.progressView.bounds.size.width;
    while (x < overallWidth + self.animationDistance) {
        CGFloat thisSlugWidth = self.slugWidth;
        CGRect slug = CGRectMake(x, 0.0, thisSlugWidth, self.progressView.bounds.size.height);
        CGContextFillRect(context, slug);

        x += self.slugWidth ;  // for slug just filled
        x += self.slugWidth ;  // for space between slugs
    }
}

@end

/*!
 @brief    Subclass of UIProgressView with additional features:
 *indeterminate* mode (similar to NSProgressIndicator in macOS), adjustable
 thickness, and vertical orientation mode

 This class was forked from SSYMoreProgressView on 2017-05-20.
 */
@interface IUPCocoaTouchProgressView : UIProgressView

/*!
 @brief    Whether or not the receiver is in a special "indeterminate" state
 wherein the progress value is not displayed, presumably because it is unknown

 @details  Setting to YES sets the receiver's `progress` property to 0.0,
 draws 5 equally-spaced and equal-sized "slugs", of the receiver's current
 progress tint color (usually blue) in the receiver's track, and begins
 animating these slugs in a continuous motion to the right.  Setting to NO
 stops the animation and removes the slugs.
 */
@property (assign) IBInspectable BOOL indeterminate;

/*!
 @brief    Factor by which to increase or decrease the thickness of the
 receiver's view (height if horizontal, width if vertical) from the value
 inherited from the base class UIProgressView (2.0 points in iOS 10.2).

 @details  Many people think that 2.0 points is too tiny to see, particularly
 on displays with the higher dot densities.
 */
@property (assign) IBInspectable CGFloat thicknessMultiple;

/*!
 @brief    Indicates whether to flip the receiver's view so that it is
 oriented vertically, with progress flowing from bottom to top, instead
 of horizontally
 */
@property (assign) IBInspectable BOOL isVertical;

@end

@interface IUPCocoaTouchProgressView ()

@property (unsafe_unretained) CALayer* marqueeLayer;
@property (unsafe_unretained) NSTimer* animationTimer;
@property CGFloat baseClassZPositionMin;
@property CGFloat baseClassZPositionMax;
@property (strong) IUPProgressBarMarqueeDelegate* marqueeDelegate;

@end


@implementation IUPCocoaTouchProgressView

@synthesize indeterminate = _indeterminate;
@synthesize thicknessMultiple = _thicknessMultiple;
@synthesize isVertical = _isVertical;

#if !__has_feature(objc_arc)
- (void)dealloc
{
    [_marqueeDelegate release];

    [super dealloc];
}
#endif

- (void)recalculateTransform
{
    CGAffineTransform transform = CGAffineTransformIdentity;
    if (self.isVertical) {
        transform = CGAffineTransformConcat(transform, CGAffineTransformMakeRotation(-M_PI/2));
    }
    if (self.thicknessMultiple != 1.0) {
        CGFloat x, y;
        if (self.isVertical) {
            x = self.thicknessMultiple;
            y = 1.0;
        }
        else
        {
            x = 1.0;
            y = self.thicknessMultiple;
        }
        transform = CGAffineTransformConcat(transform, CGAffineTransformMakeScale(x, y));
    }

    self.transform = transform;
}


- (BOOL)isVertical
{
    return _isVertical;
}

- (void)setIsVertical:(BOOL)isVertical
{
    _isVertical = isVertical;

    [self recalculateTransform];
}

- (CGFloat)thicknessMultiple
{
    return _thicknessMultiple;
}

- (void)setThicknessMultiple:(CGFloat)thicknessMultiple
{
    _thicknessMultiple = thicknessMultiple;

    [self recalculateTransform];
}

- (void)doInitialSetup
{
    self.clipsToBounds = YES;

    self.progress = 0.0;
    self.thicknessMultiple = 1.0;

    // See Notes : ZPosition for purpose of this section
    self.baseClassZPositionMin = 0.0;
    self.baseClassZPositionMax = 0.0;
    for (CALayer* baseClassSublayer in self.layer.sublayers) {
        if (baseClassSublayer.zPosition < self.baseClassZPositionMin) {
            self.baseClassZPositionMin = baseClassSublayer.zPosition;
        }
        if (baseClassSublayer.zPosition > self.baseClassZPositionMax) {
            self.baseClassZPositionMax = baseClassSublayer.zPosition;
        }
    }

    self.marqueeDelegate = [IUPProgressBarMarqueeDelegate new];
    self.marqueeDelegate.progressView = self;

    CALayer* marqueeLayer = [[CALayer alloc] init];
    self.marqueeLayer = marqueeLayer;
    marqueeLayer.zPosition = self.baseClassZPositionMin - 1 ; // Hide initially
    [self.layer addSublayer:marqueeLayer];
#if !__has_feature(objc_arc)
    [marqueeLayer release];
#endif
    [self reframeMarqueeLayer];
    marqueeLayer.delegate = self.marqueeDelegate;
    [marqueeLayer setNeedsDisplay];
}

- (void)reframeMarqueeLayer
{
    CGRect marqueeLayerFrame = self.bounds;
    CGFloat animationDistance = self.marqueeDelegate.animationDistance;
    marqueeLayerFrame.origin.x -= animationDistance;
    marqueeLayerFrame.size.width += animationDistance;
    self.marqueeLayer.frame = marqueeLayerFrame;
}

- (instancetype)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        [self doInitialSetup];
    }
    return self;
}

- (instancetype)initWithCoder:(NSCoder *)coder
{
    self = [super initWithCoder:coder];
    if (self) {
        [self doInitialSetup];
    }

    return self;
}

- (void)setBounds:(CGRect)bounds
{
    [super setBounds:bounds];

    /* To track size changes after init, in particular by Auto Layout */
    [self reframeMarqueeLayer];
}

- (void)setFrame:(CGRect)frame
{
    [super setFrame:frame];
}

- (void)doOneAnimationCycleAtMediaTime:(NSTimeInterval)mediaTime
                              duration:(NSTimeInterval)duration
{
    CGFloat x = self.marqueeLayer.position.x;
    CABasicAnimation* animation = [CABasicAnimation animationWithKeyPath:@"position.x"];
    animation.beginTime = mediaTime ;  // See Notes : HasNoEffect
    animation.fromValue = [NSNumber numberWithFloat:x];
    CGFloat finalX = x + self.marqueeDelegate.animationDistance;
    animation.toValue = [NSNumber numberWithFloat:finalX];
    animation.duration = duration;
    [self.marqueeLayer addAnimation:animation
                             forKey:nil];
    // See Notes : WhySnapBack
}

- (void)startAnimation
{
    NSTimeInterval duration = 0.5;
    // See Notes : WhyATimer
    self.animationTimer = [NSTimer scheduledTimerWithTimeInterval:duration
                                                          repeats:YES
                                                            block:^(NSTimer* timer) {
                                                                NSTimeInterval currentTime = CACurrentMediaTime();
                                                                [self doOneAnimationCycleAtMediaTime:currentTime
                                                                                            duration:duration];
                                                            }];
}

- (void)stopAnimation
{
    [self.animationTimer invalidate];
    self.animationTimer = nil;
    [self.marqueeLayer removeAllAnimations];
}

- (BOOL)indeterminate
{
    return _indeterminate;
}

- (void)setIndeterminate:(BOOL)indeterminate
{
    _indeterminate = indeterminate;
    if (indeterminate) {
        self.progress = 0.0 ; // See Notes : ProgressTrackColor
        [self startAnimation];
        self.marqueeLayer.zPosition = self.baseClassZPositionMax + 1.0;
    }
    else
    {
        [self stopAnimation];
        self.marqueeLayer.zPosition = self.baseClassZPositionMin - 1.0;
    }

    [self setNeedsDisplay];
}

@end

/*
 # Notes for iupCocoaTouchProgressView


 ## ProgressTrackColor

 To make our marquee, we need to get the two colors which are used in
 UIProgressView, ostensibly progressTintColor and trackTintColor.

 Unfortunately, we cannot simply use UIProgressView.progressTintColor and
 UIProgressView.trackTintColor because, by default, and in most apps, both
 of these are nil, even though UIProgressView displays them as blue and gray,
 respectively.

 For the progressTintColor (blue), we can solve this problem by using the
 UIView.tintColor.

 I could not find a way to get the trackTintColor (gray).  Instead, I set
 the progress to 0.0 (so that the progress bar is all track/gray), and
 in the marqueeLayer only fill every other slug with trackTintColor (blue)
 and let the spaces in between be transparent.

 A side effect of this is that, when switching from indeterminate to
 determinate, the progress is initially 0.0, but I don't see any disadvantage
 to this.


 ## WhySnapBack

 The above code is from Core Animation Programming Guide > Animating Layer
 Content > Animating Simple Changes to a Layer’s Properties > Listing 3-2
 Animating a change explicitly.  The last line in that listing would be this:

 self.marqueeLayer.position = CGPointMake(finalX, y);

 which explicitly sets marqueeLayer.position to its final position, because
 otherwise the position would snap back to the initial position after the
 animation was done.  If we did that, in addition, to disable the implicit
 animation which this would trigger, we would need to implement
 -actionForLayer:forKey: in our delegate, as explained in the answer by skozin:
 http://stackoverflow.com/questions/2244147/disabling-implicit-animations-in-calayer-setneedsdisplayinrect
 (Note there is some contoversy whether or not this is the best method,
 but returning [NSNull null] worked for me in Objective-C.).  Skozin got this
 idea from Apple's  Core Animation Programming Guide > Changing a Layer's
 Default Behavior > Action Objects Must Be Installed On a Layer to Have an
 Effect

 However, in this case, because we want a continuous, cyclical animation, and
 because our animation has been crafted to complete exactly one cycle, we *want*
 our marqueeLayter.position to snap back to the original position (which looks
 the same as the final position by design) in preparation for the next
 animation.  So we just omit that line of code :)


 ## WhyATimer

 One would think that a more rational approach would be to trigger a next
 animation when the the first animation ends.  But Apple documentation
 recommends against this.  From Core Animation Programming Guide > Animating
 Layer Content > Detecting the End of of an Animation,

 ""If you want to chain two animations together so that one starts when the
 other finishes, do not use animation notifications. Instead, use the
 beginTime property of your animation objects to start each one at the desired
 time. To chain two animations together, set the start time of the second
 animation to the end time of the first animation"

 Out of curiousity, I first tried it as they said not to, using
 -[CAAnimationDelegate animationDidStop:finished:].  The result was a little
 choppy, especially when beginning.  So now I did it as Apple recommends.
 Result: About the same, still a little choppy when beginning. Oh, well.

 Now, regarding the timer – Apple does not mention that.  To follow Apple's
 recommendation exactly, one could, in -startAnimation, create a whole bunch
 of animations to start at multiples of animationCycleDuration.  Indeed, this
 works, but of course you have no idea how many you're going to need when
 the animation starts.  So in comes the timer.


 ## HasNoEffect

 This line of code has no effect, because if beginTime is not set it defaults to
 the current animation time (mediaTime).  I just used this to comply with the
 Apple recommendation quoted in Note WhyATimer.

 By the way, the current animation time (mediaTime) is not 0.0.  It appears
 to be about 80% of the time since system boot.  Maybe it does not include
 sleeping?


 ## ZPositions

 In iOS 10, there are two base class sublayers (apparently the "track" and the
 "filled progress", and both have zPosition 0.0.  But this is not documented
 and is therefore a Cocoa implementation detail.  Hence our usage of
 baseClassZPositionMin/Max.

 */


/* Like awakeFromNib */
static int cocoaProgressBarMapMethod(Ihandle* ih)
{
	static int woffset = 0;
	static int hoffset = 0;
	
	woffset += 60;
	hoffset += 10;
	//	ih->data->type = 0;

    CGRect frame = CGRectMake(woffset, hoffset, 200, 40);
	IUPCocoaTouchProgressView* progress_indicator = [[IUPCocoaTouchProgressView alloc] initWithFrame:frame];
	
	if (iupStrEqualNoCase(iupAttribGetStr(ih, "ORIENTATION"), "VERTICAL"))
	{
		[progress_indicator setIsVertical:YES];

        NSLog(@"user size:  %d  x  %d", ih->userwidth, ih->userheight) ;
        /* This is IUP's internal layout system */
//		if (ih->userheight < ih->userwidth)
//		{
//			int tmp = ih->userheight;
//			ih->userheight = ih->userwidth;
//			ih->userwidth = tmp;
//		}
	}
	else
	{

		
	}
	
	if (iupAttribGetBoolean(ih, "MARQUEE"))
	{
		ih->data->marquee = 1;
		[progress_indicator setIndeterminate:YES];
	}
	else
	{
		ih->data->marquee = 0;
		[progress_indicator setIndeterminate:NO];
	}
	
	ih->handle = progress_indicator;
	
	// All Cocoa views shoud call this to add the new view to the parent view.
	iupCocoaTouchAddToParent(ih);
	
	return IUP_NOERROR;
}

static void cocoaProgressBarUnMapMethod(Ihandle* ih)
{
	id progress_bar = ih->handle;
	[progress_bar release];
	ih->handle = NULL;
}

//  ?????  These getters are not called.  IupGetAttribute() invokes iupTableGet()
//  ?????  Also, there are "i" prefix getters, for example iProgressBarGetValueAttrib()
static char* cocoaProgressBarGetValueAttrib(Ihandle* ih)
{
    IUPCocoaTouchProgressView* the_bar = ih->handle;
    float progress = [the_bar progress];
    char string[255];
    return iupStrReturnStrf(string, 255, "VALUE FROM COCOA: %g", progress);
}

static int cocoaProgressBarSetValueAttrib(Ihandle* ih, const char* value)
{
    IUPCocoaTouchProgressView* progress_bar = (IUPCocoaTouchProgressView*)ih->handle;
    
    if (ih->data->marquee)
    {
        return 0;
    }
    
    if(!value)
    {
        ih->data->value = 0;
    }
    else
    {
        iupStrToDouble(value, &(ih->data->value));
    }
    
    iProgressBarCropValue(ih);
    
    float range = (ih->data->vmax - ih->data->vmin);
    if (range > 0.0)
    {
        float progress = (ih->data->value - ih->data->vmin) / range;
        [progress_bar setProgress:progress];
    }
    
    return 0;
}

static char* cocoaProgressBarGetFgColorAttrib(Ihandle* ih)
{
    IUPCocoaTouchProgressView* the_bar = ih->handle;
    UIColor *textColor = [the_bar progressTintColor];
    return iupCocoaTouchColorFromNative(textColor);
}

static int cocoaProgressBarSetFgColorAttrib(Ihandle* ih, const char *iColor)
{
    UIColor *color = iupCocoaTouchToNativeColor(iColor);
    if (color)
    {
        IUPCocoaTouchProgressView* the_bar = ih->handle;
        [the_bar setProgressTintColor:color];
        return IUP_NOERROR;
    }
    return IUP_ERROR;
}


static char* cocoaProgressBarGetBgColorAttrib(Ihandle* ih)
{
    IUPCocoaTouchProgressView* the_bar = ih->handle;
    UIColor *textColor = [the_bar trackTintColor];
    return iupCocoaTouchColorFromNative(textColor);
}


static int cocoaProgressBarSetBgColorAttrib(Ihandle* ih, const char *iColor)
{
    UIColor *color = iupCocoaTouchToNativeColor(iColor);
    if (color)
    {
        IUPCocoaTouchProgressView* the_bar = ih->handle;
        [the_bar setTrackTintColor:color];
        return IUP_NOERROR;
    }
    return IUP_ERROR;
}

static char* cocoaProgressBarGetMarqueeAttrib(Ihandle* ih)
{
    IUPCocoaTouchProgressView* the_bar = (IUPCocoaTouchProgressView*)ih->handle;
    // Seems like there should be a iup function to convert bool --> "YES" : "NO" ??????
    return [the_bar indeterminate] ? "YES" : "NO";
}

static int cocoaProgressBarSetMarqueeAttrib(Ihandle* ih, const char* value)
{
    if (!ih->data->marquee)
    {
        return 0;
    }
    
    IUPCocoaTouchProgressView* progress_bar = (IUPCocoaTouchProgressView*)ih->handle;
    [progress_bar setIndeterminate:iupStrBoolean(value)];
    
    return 1;
}

static char* cocoaProgressBarGetOrientationAttrib(Ihandle* ih)
{
    IUPCocoaTouchProgressView* the_bar = ih->handle;
    return [the_bar isVertical] ? "VERTICAL" : "HORIZONTAL";
}

static int cocoaProgressBarSetOrientationAttrib(Ihandle* ih, const char *orientation)
{
    IUPCocoaTouchProgressView* the_bar = ih->handle;
    [the_bar setIsVertical:(strcmp(orientation, "VERTICAL") == 0)];

    return IUP_NOERROR;
}

void iupdrvProgressBarInitClass(Iclass* ic)
{
  /* Driver Dependent Class functions */
	ic->Map = cocoaProgressBarMapMethod;
	ic->UnMap = cocoaProgressBarUnMapMethod;

  iupClassRegisterAttribute(ic, "BGCOLOR", cocoaProgressBarGetBgColorAttrib, cocoaProgressBarSetBgColorAttrib, IUPAF_SAMEASSYSTEM, "DLGBGCOLOR", IUPAF_DEFAULT);
  iupClassRegisterAttribute(ic, "FGCOLOR", cocoaProgressBarGetFgColorAttrib, cocoaProgressBarSetFgColorAttrib, IUPAF_SAMEASSYSTEM, "DLGFGCOLOR", IUPAF_DEFAULT);
  iupClassRegisterAttribute(ic, "VALUE",  cocoaProgressBarGetValueAttrib,  cocoaProgressBarSetValueAttrib,  NULL, NULL, IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "ORIENTATION", cocoaProgressBarGetOrientationAttrib, cocoaProgressBarSetOrientationAttrib, IUPAF_SAMEASSYSTEM, "HORIZONTAL", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "MARQUEE",     cocoaProgressBarGetMarqueeAttrib, cocoaProgressBarSetMarqueeAttrib, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "DASHED",      NULL, NULL, NULL, NULL, IUPAF_NO_INHERIT);
}
