/** \file
 * \brief Frame Control
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

#include "iup_object.h"
#include "iup_layout.h"
#include "iup_attrib.h"
#include "iup_str.h"
#include "iup_dialog.h"
#include "iup_drv.h"
#include "iup_drvfont.h"
#include "iup_stdcontrols.h"
#include "iup_frame.h"

#include "iupcocoatouch_drv.h"

@interface IUPFrame : UIView
@property(nonatomic, retain) UILabel* titleLabel;
@property(nonatomic, retain) UIColor* foreColor;
@end
@implementation IUPFrame
- (void)drawRect:(CGRect)rect {
	[super drawRect:rect];
	CGRect bounds = [self bounds];
	[self.foreColor set];
	CGRect frame = CGRectInset(bounds, 0.5, 0.5);
	UIRectFrame(frame);
}
@end


static int cocoaFrameMapMethod(Ihandle* ih)
{

	IUPFrame* the_frame = [[IUPFrame alloc] initWithFrame:CGRectMake(0, 0, 100, 100)];

	{
		the_frame.backgroundColor = [UIColor clearColor];
		the_frame.foreColor = [UIColor blueColor];
		char* title;
		title = iupAttribGet(ih, "TITLE");
		if(title && *title!=0)
		{
			NSString* ns_string = [NSString stringWithUTF8String:title];
			if (the_frame.titleLabel == nil)
			{
				the_frame.titleLabel = [[[UILabel alloc] initWithFrame:CGRectMake(2, 2, 100, 100)] autorelease];
				[the_frame addSubview:the_frame.titleLabel];
			}
			[the_frame.titleLabel setText:ns_string];
			[the_frame.titleLabel sizeToFit];
		}
	}
	
	ih->handle = the_frame;
	
	iupCocoaTouchAddToParent(ih);
	
	return IUP_NOERROR;
}


void iupdrvFrameGetDecorOffset(int *x, int *y)
{
	*x = 2;
	*y = 2;
}

int iupdrvFrameHasClientOffset(void)
{
	return 0;
}


static void cocoaFrameUnMapMethod(Ihandle* ih)
{
	id the_label = ih->handle;
	iupCocoaTouchRemoveFromParent(ih);
	[the_label release];
	ih->handle = nil;
}

static int cocoaFrameSetFGColorAttrib(Ihandle* ih, char *iColor)
{
	UIColor *color = iupCocoaTouchToNativeColor(iColor);
	if (color)
	{
		IUPFrame* the_frame = ih->handle;
		[the_frame setForeColor:color];
		return IUP_NOERROR;
	}
	return IUP_ERROR;
}


static char* cocoaFrameGetFGColorAttrib(Ihandle* ih)
{
	IUPFrame* the_frame = ih->handle;
	UIColor *foreColor = [the_frame foreColor];
	return iupCocoaTouchColorFromNative(foreColor);
}

static char* cocoaFrameGetTitleAttrib(Ihandle* ih)
{
	IUPFrame* the_frame = ih->handle;
	NSString *title = the_frame.titleLabel.text;
	if (title) {
		return iupStrReturnStr([title UTF8String]);
	}
	return NULL;
}

static int cocoaFrameSetTitleAttrib(Ihandle* ih, char *iTitle)
{
	IUPFrame* the_frame = ih->handle;
	NSString *title = [NSString stringWithUTF8String:iTitle];
	if (the_frame.titleLabel == nil)
	{
		the_frame.titleLabel = [[[UILabel alloc] initWithFrame:CGRectMake(2, 2, 100, 100)] autorelease];
		[the_frame addSubview:the_frame.titleLabel];
	}
	the_frame.titleLabel.text = title;
	[the_frame.titleLabel sizeToFit];
	return IUP_NOERROR;
}


void iupdrvFrameInitClass(Iclass* ic)
{
	/* Driver Dependent Class functions */
	ic->Map = cocoaFrameMapMethod;
	ic->UnMap = cocoaFrameUnMapMethod;
#if 0
	ic->GetInnerNativeContainerHandle = gtkFrameGetInnerNativeContainerHandleMethod;
	
	/* Driver Dependent Attribute functions */
	
	/* Overwrite Common */
	iupClassRegisterAttribute(ic, "STANDARDFONT", NULL, gtkFrameSetStandardFontAttrib, IUPAF_SAMEASSYSTEM, "DEFAULTFONT", IUPAF_NO_SAVE|IUPAF_NOT_MAPPED);
#endif
#if 1
	/* Visual */
	iupClassRegisterAttribute(ic, "BGCOLOR", iupBaseNativeParentGetBgColorAttrib, iupCocoaTouchSetBgColorAttrib, IUPAF_SAMEASSYSTEM, "DLGBGCOLOR", IUPAF_DEFAULT);
	
	/* Special */
	iupClassRegisterAttribute(ic, "FGCOLOR", cocoaFrameGetFGColorAttrib, cocoaFrameSetFGColorAttrib, IUPAF_SAMEASSYSTEM, "DLGFGCOLOR", IUPAF_DEFAULT);
#endif
#if 0
	iupClassRegisterAttribute(ic, "SUNKEN", NULL, gtkFrameSetSunkenAttrib, NULL, NULL, IUPAF_NO_INHERIT);
#endif
	iupClassRegisterAttribute(ic, "TITLE", cocoaFrameGetTitleAttrib, cocoaFrameSetTitleAttrib, NULL, NULL, IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);
}
