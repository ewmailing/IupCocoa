#import "SSYMoreProgressView.h"

@interface MarqueeDelegate : NSObject <CALayerDelegate>

@property (WEAK) UIProgressView* progressView ;
@property (readonly) CGFloat slugWidth ;
@property (readonly) CGFloat animationDistance ;

@end

@implementation MarqueeDelegate

- (CGFloat)slugWidth {
    /* Cosmetic decision: How many slugs do you want in the bar?
     I like 5, plus 5 spaces in between slugs. */
    return self.progressView.bounds.size.width / 10 ;
}

- (CGFloat)animationDistance {
    return 2 * self.slugWidth ;
}

- (NSTimeInterval)animationCycleDuration {
    /* Another cosmetic decision.  Increase if you like slower animation. */
    return 0.5 ;
}

- (void)drawLayer:(CALayer*)layer
        inContext:(CGContextRef)context {
    CGColorRef color = [self.progressView.progressTintColor CGColor] ;
    if (!color) {
        color = [self.progressView.tintColor CGColor] ;
    }
    CGContextSetFillColorWithColor(context, color);
    
    // See Notes : ProgressTrackColor
    CGFloat x = 0.0 ;
    CGFloat overallWidth = self.progressView.bounds.size.width ;
    while (x < overallWidth + self.animationDistance) {
        CGFloat thisSlugWidth = self.slugWidth ;
        CGRect slug = CGRectMake(x, 0.0, thisSlugWidth, self.progressView.bounds.size.height) ;
        CGContextFillRect(context, slug) ;

        x += self.slugWidth ;  // for slug just filled
        x += self.slugWidth ;  // for space between slugs
    }
}

@end


@interface SSYMoreProgressView ()

@property (WEAK) CALayer* marqueeLayer ;
@property (WEAK) NSTimer* animationTimer ;
@property CGFloat baseClassZPositionMin ;
@property CGFloat baseClassZPositionMax ;
@property (strong) MarqueeDelegate* marqueeDelegate ;

@end


@implementation SSYMoreProgressView

@synthesize indeterminate = _indeterminate ;
@synthesize thicknessMultiple = _thicknessMultiple ;
@synthesize isVertical = _isVertical ;

#if !__has_feature(objc_arc)
- (void)dealloc {
    [_marqueeDelegate release] ;
    
    [super dealloc] ;
}
#endif

- (void)recalculateTransform {
    CGAffineTransform transform = CGAffineTransformIdentity ;
    if (self.isVertical) {
        transform = CGAffineTransformConcat(transform, CGAffineTransformMakeRotation(-M_PI/2)) ;
    }
    if (self.thicknessMultiple != 1.0) {
        CGFloat x, y ;
        if (self.isVertical) {
            x = self.thicknessMultiple ;
            y = 1.0 ;
        }
        else {
            x = 1.0 ;
            y = self.thicknessMultiple ;
        }
        transform = CGAffineTransformConcat(transform, CGAffineTransformMakeScale(x, y)) ;
    }

    self.transform = transform;
}


- (BOOL)isVertical {
    return _isVertical ;
}

- (void)setIsVertical:(BOOL)isVertical {
    _isVertical = isVertical ;
    
    [self recalculateTransform] ;
}

- (CGFloat)thicknessMultiple {
    return _thicknessMultiple ;
}

- (void)setThicknessMultiple:(CGFloat)thicknessMultiple {
    _thicknessMultiple = thicknessMultiple ;
    
    [self recalculateTransform] ;
}

- (void)doInitialSetup {
    self.clipsToBounds = YES ;
    
    self.progress = 0.0 ;
    self.thicknessMultiple = 1.0 ;
    
    // See Notes : ZPosition for purpose of this section
    self.baseClassZPositionMin = 0.0 ;
    self.baseClassZPositionMax = 0.0 ;
    for (CALayer* baseClassSublayer in self.layer.sublayers) {
        if (baseClassSublayer.zPosition < self.baseClassZPositionMin) {
            self.baseClassZPositionMin = baseClassSublayer.zPosition ;
        }
        if (baseClassSublayer.zPosition > self.baseClassZPositionMax) {
            self.baseClassZPositionMax = baseClassSublayer.zPosition ;
        }
    }
    
    self.marqueeDelegate = [MarqueeDelegate new] ;
    self.marqueeDelegate.progressView = self ;

    CALayer* marqueeLayer = [[CALayer alloc] init] ;
    self.marqueeLayer = marqueeLayer ;
    marqueeLayer.zPosition = self.baseClassZPositionMin - 1 ; // Hide initially
    [self.layer addSublayer:marqueeLayer] ;
#if !__has_feature(objc_arc)
    [marqueeLayer release] ;
#endif
    [self reframeMarqueeLayer] ;
    marqueeLayer.delegate = self.marqueeDelegate ;
    [marqueeLayer setNeedsDisplay] ;
}

- (void)reframeMarqueeLayer {
    CGRect marqueeLayerFrame = self.bounds ;
    CGFloat animationDistance = self.marqueeDelegate.animationDistance ;
    marqueeLayerFrame.origin.x -= animationDistance ;
    marqueeLayerFrame.size.width += animationDistance ;
    self.marqueeLayer.frame = marqueeLayerFrame ;
}

- (instancetype)initWithFrame:(CGRect)frame {
    self = [super initWithFrame:frame] ;
    if (self) {
        [self doInitialSetup] ;
    }
    return self;
}

- (instancetype)initWithCoder:(NSCoder *)coder {
    self = [super initWithCoder:coder];
    if (self) {
        [self doInitialSetup] ;
    }
    
    return self;
}

- (void)setBounds:(CGRect)bounds {
    [super setBounds:bounds] ;

    /* To track size changes after init, in particular by Auto Layout */
    [self reframeMarqueeLayer] ;
}

- (void)setFrame:(CGRect)frame {
    [super setFrame:frame] ;
    /*SSYDBL*/ NSLog(@"frame of %p is %@", self, NSStringFromCGRect(self.frame)) ;
}

- (void)doOneAnimationCycleAtMediaTime:(NSTimeInterval)mediaTime
                              duration:(NSTimeInterval)duration {
    CGFloat x = self.marqueeLayer.position.x ;
    CABasicAnimation* animation = [CABasicAnimation animationWithKeyPath:@"position.x"];
    animation.beginTime = mediaTime ;  // See Notes : HasNoEffect
    animation.fromValue = [NSNumber numberWithFloat:x] ;
    CGFloat finalX = x + self.marqueeDelegate.animationDistance ;
    animation.toValue = [NSNumber numberWithFloat:finalX] ;
    animation.duration = duration ;
    [self.marqueeLayer addAnimation:animation
                             forKey:nil] ;
    // See Notes : WhySnapBack
}

- (void)startAnimation {
    NSTimeInterval duration = 0.5 ;
    // See Notes : WhyATimer
    self.animationTimer = [NSTimer scheduledTimerWithTimeInterval:duration
                                                          repeats:YES
                                                            block:^(NSTimer* timer) {
                                                                NSTimeInterval currentTime = CACurrentMediaTime() ;
                                                                [self doOneAnimationCycleAtMediaTime:currentTime
                                                                                            duration:duration] ;
                                                            }] ;
}

- (void)stopAnimation {
    [self.animationTimer invalidate] ;
    self.animationTimer = nil ;
    [self.marqueeLayer removeAllAnimations] ;
}

- (BOOL)indeterminate {
    return _indeterminate ;
}

- (void)setIndeterminate:(BOOL)indeterminate {
    _indeterminate = indeterminate ;
    if (indeterminate) {
        self.progress = 0.0 ; // See Notes : ProgressTrackColor
        [self startAnimation] ;
        self.marqueeLayer.zPosition = self.baseClassZPositionMax + 1.0 ;
    }
    else {
        [self stopAnimation] ;
        self.marqueeLayer.zPosition = self.baseClassZPositionMin - 1.0 ;
    }
    
    [self setNeedsDisplay] ;
}

@end

/* 
 # Notes
 
 
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
 
 self.marqueeLayer.position = CGPointMake(finalX, y) ;
 
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

