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

static UIFont *cocoaFontFromHandle(Ihandle* ih) {
	id native_object = ih->handle;
	if ([native_object respondsToSelector:@selector(titleLabel)]) {
		return [[native_object titleLabel] font];
	} else if ([native_object respondsToSelector:@selector(font)]) {
		return [native_object font];
	} else {
		NSLog(@"cocoaFontFromHandle not implemented for %@", [native_object class]);
	}
	return nil;
}


char* iupdrvGetSystemFont(void)
{
	static char systemfont[200] = "";
	UIFont* font = [UIFont systemFontOfSize:[UIFont labelFontSize]];
	NSLog(@"systemfont: %@", font);
	NSString* nameSize = [NSString stringWithFormat:@"%@,%g", [font familyName], [font pointSize]];
	char* name = [nameSize UTF8String];
	if(*name)
	{
		strlcpy(systemfont, name, sizeof systemfont);
	}
	else
	{
		strlcpy(systemfont, "Helvetica,17", sizeof systemfont);
	}
	return systemfont;
}


int iupdrvSetStandardFontAttrib(Ihandle* ih, const char* value)
{
	id native_object = ih->handle;
	NSString* fontNameSize = [NSString stringWithUTF8String:value];
	NSScanner* scanner = [NSScanner scannerWithString:fontNameSize];
	NSString* fontName = nil;
	float size;
	if ([scanner scanUpToString:@"," intoString:&fontName] &&
		[scanner scanString:@"," intoString:NULL] &&
		[scanner scanFloat:&size])
	{
		UIFont* font = [UIFont fontWithName:fontName size:size];
		if ([native_object respondsToSelector:@selector(setFont:)])
		{
			[native_object setFont:font];
		}
		else if ([native_object respondsToSelector:@selector(titleLabel)])
		{
			[[native_object titleLabel] setFont:font];
		}
		else
		{
			NSLog(@"iupdrvSetStandardFontAttrib not implemented for %@", [ih->handle class]);
		}
	}
	return 1;
}


static void helperFontGetMultiLineStringSize(UIFont* the_font, const char* str, int *w, int *h)
{
	CGRect bounding_rect = CGRectZero;
	if (str)
	{
//		UIFont* the_font = cocoaFontFromHandle(ih);
		if (the_font == nil)
		{
			the_font = [UIFont systemFontOfSize:[UIFont labelFontSize]];
		}
		
		NSString* ns_string = [NSString stringWithUTF8String:str];
		
		CGSize size_bounds = [[UIScreen mainScreen] bounds].size;
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
		bounding_rect = [ns_string boundingRectWithSize:size_bounds
			options:NSStringDrawingUsesLineFragmentOrigin
			attributes:@{NSFontAttributeName:the_font}
			context:nil
		];
	}
	if (w) *w = (int)(bounding_rect.size.width + 0.5f);
	if (h) *h = (int)(bounding_rect.size.height + 0.5f);
}

void iupdrvFontGetMultiLineStringSize(Ihandle* ih, const char* str, int* w, int* h)
{
	id native_object = ih->handle;
	int temp_w = 0;
	int temp_h = 0;
	if(str)
	{
		UIFont* the_font = cocoaFontFromHandle(ih);
		helperFontGetMultiLineStringSize(the_font, str, &temp_w, &temp_h);
	}
	
	CGSize margin = CGSizeZero;
	if ([native_object isKindOfClass:[UIButton class]])
	{
		margin = CGSizeMake(10, 5);
	}
	else if([native_object isKindOfClass:[UILabel class]])
	{
		margin = CGSizeMake(5, 5);
	}
	// Should I always round up or to the nearest?
	if (w) *w = temp_w + (int)(0.5f + margin.width);
	if (h) *h = temp_h + (int)(0.5f + margin.height);

}

int iupdrvFontGetStringWidth(Ihandle* ih, const char* str)
{
	UIFont* the_font = cocoaFontFromHandle(ih);
	if (the_font == nil)
	{
		the_font = [UIFont systemFontOfSize:[UIFont labelFontSize]];
	}
	
	NSString* ns_string = [NSString stringWithUTF8String:str];
	
	CGSize size_bounds = CGSizeMake(CGFLOAT_MAX, CGFLOAT_MAX);
	CGRect bounding_rect = [ns_string boundingRectWithSize:size_bounds
		options:NSStringDrawingUsesLineFragmentOrigin
		attributes:@{NSFontAttributeName:the_font}
		context:nil];
	return bounding_rect.size.width;
	
}

void iupdrvFontGetCharSize(Ihandle* ih, int* charwidth, int* charheight)
{
//	NSLog(@"iupdrvFontGetCharSize not implemented.");
	if (charwidth) *charwidth = 17;
	if (charheight) *charheight = 17;

}

void iupdrvFontGetTextSize(const char* font, const char* str, int *w, int *h)
{
	// FIXME: FindFont was skipped to get things going
//	UIFont* the_font = cocoaTouchFindFont(ih);
	// hardcoded
//	UIFont* the_font = [UIFont fontWithName:[NSString stringWithUTF8String:font] size:0];
	UIFont* the_font = [UIFont systemFontOfSize:[UIFont labelFontSize]];

	if (the_font)
	{
		// FIXME: quick and dirty fix to get around Iup internal API changes
		helperFontGetMultiLineStringSize(the_font, str, w, h);
	}
	
}

void iupdrvFontInit(void)
{
//	NSLog(@"iupdrvFontInit not implemented.");
}

void iupdrvFontFinish(void)
{
//	NSLog(@"iupdrvFontFinish not implemented.");
}


int iupdrvSetFontAttrib(Ihandle* ih, const char* value)
{
	return 1;
}

const char* iupdrvGetFontAttrib(Ihandle* ih)
{
	return NULL;
}



