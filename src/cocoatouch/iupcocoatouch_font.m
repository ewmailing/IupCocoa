/** \file
 * \brief MAC Font mapping
 *
 * See Copyright Notice in "iup.h"
 */


#include <stdlib.h>
#include <stdio.h>

#import <UIKit/UIKit.h>

#include "iup.h"

#include "iup_str.h"
#include "iup_array.h"
#include "iup_attrib.h"
#include "iup_object.h"
#include "iup_drv.h"
#include "iup_drvfont.h"
#include "iup_assert.h"


char* iupdrvGetSystemFont(void)
{
	static char systemfont[200] = "";
	UIFont *font = [UIFont systemFontOfSize:[UIFont labelFontSize]];;
	NSLog(@"systemfont: %@", font);
	char* name = [[font familyName] UTF8String];
	if(*name)
	{
		strlcpy(systemfont, name, 200);
	}
	else
	{
		strlcpy(systemfont, "San Francisco, 17", 200);
	}
	return systemfont;
}


int iupdrvSetStandardFontAttrib(Ihandle* ih, const char* value)
{
	NSLog(@"iupdrvSetStandardFontAttrib not implemented");
	return 1;
}

void iupdrvFontGetMultiLineStringSize(Ihandle* ih, const char* str, int* w, int* h)
{
	NSLog(@"iupdrvFontGetMultiLineStringSize only partially implemented. (Needs to support user specified font & size");

	UIFont* the_font = nil;
	// FIXME: Use font set by user
	
	the_font = [UIFont systemFontOfSize:[UIFont labelFontSize]];
	
	NSString* ns_string = [NSString stringWithUTF8String:str];
	
	CGSize size_bounds = [[UIScreen mainScreen] bounds].size;
 	id native_object = ih->handle;
	// This is making the view too small.
	// I already made the view fit for a UIButton.
	// Doing boundingRectWithSize with the new bounds results in a much smaller rect, instead of the same rect.
/*
	if([native_object isKindOfClass:[UIView class]])
	{
		UIView* the_view = (UIView*)native_object;
		size_bounds = [the_view bounds].size;

	}
*/
	// Using mainScreen bounds is still giving a smaller height than when I used MAX for the previous call. Why???
	CGRect bounding_rect = [ns_string boundingRectWithSize:size_bounds
		options:NSStringDrawingUsesLineFragmentOrigin
		attributes:@{NSFontAttributeName:the_font}
		context:nil
	];
	
	// Should I always round up or to the nearest?
	if (w) *w = (int)(bounding_rect.size.width + 0.5f);
	if (h) *h = (int)(bounding_rect.size.height + 0.5f);

}

int iupdrvFontGetStringWidth(Ihandle* ih, const char* str)
{
	NSLog(@"iupdrvFontGetStringWidth only partially implemented. (Needs to support user specified font & size");
	UIFont* the_font = nil;
	// FIXME: Use font set by user
	
	the_font = [UIFont systemFontOfSize:[UIFont labelFontSize]];
	
	NSString* ns_string = [NSString stringWithUTF8String:str];
	
	CGSize size_bounds = CGSizeMake(CGFLOAT_MAX, CGFLOAT_MAX);
	CGRect bounding_rect = [ns_string boundingRectWithSize:size_bounds
		options:NSStringDrawingUsesLineFragmentOrigin
		attributes:@{NSFontAttributeName:the_font}
		context:nil
	];
	return bounding_rect.size.width;
	
}

void iupdrvFontGetCharSize(Ihandle* ih, int* charwidth, int* charheight)
{
	NSLog(@"iupdrvFontGetCharSize not implemented.");

}

void iupdrvFontInit(void)
{
//	NSLog(@"iupdrvFontInit not implemented.");
}

void iupdrvFontFinish(void)
{
//	NSLog(@"iupdrvFontFinish not implemented.");
}
