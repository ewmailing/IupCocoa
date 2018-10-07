#ifndef IUPCOCOAFONT_H
#define IUPCOCOAFONT_H

#import <Cocoa/Cocoa.h>
#include <stdbool.h>

@interface IupCocoaFont : NSObject
@property(nonatomic, retain) NSFont* nativeFont;
@property(nonatomic, copy) NSString* iupFontName;
@property(nonatomic, copy) NSString* typeFace;
@property(nonatomic, retain) NSMutableDictionary* attributeDictionary;
@property(nonatomic, assign) bool usesAttributes;
@property(nonatomic, assign) int fontSize;
@property(nonatomic, assign) int charWidth;
@property(nonatomic, assign) int charHeight;
@end

#endif


