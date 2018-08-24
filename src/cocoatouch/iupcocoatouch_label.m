/** \file
 * \brief Label Control
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
#include "iup_image.h"
#include "iup_label.h"
#include "iup_drv.h"
#include "iup_image.h"
#include "iup_focus.h"

#include "iup_childtree.h"

#include "iupcocoatouch_drv.h"

static UIView* cocoaTouchLabelGetRootView(Ihandle* ih)
{
	UIView* root_container_view = (UIView*)ih->handle;
	return root_container_view;
}

static UIImageView* cocoaTouchLabelGetImageView(Ihandle* ih)
{
	UIView* root_container_view = cocoaTouchLabelGetRootView(ih);
	NSCAssert([root_container_view isKindOfClass:[UIImageView class]], @"Expected UIImageView");
	return (UIImageView*)root_container_view;
}

void iupdrvLabelAddBorders(Ihandle* ih, int *x, int *y)
{
  (void)ih;
  (void)x;
  (void)y;
}

static int cocoaLabelSetTitleAttrib(Ihandle* ih, const char* value)
{
	id the_label = ih->handle;
	if(the_label)
	{
		// This could be a UILabel, some kind of image, or something else.
		
		if([the_label respondsToSelector:@selector(setText:)])
		{
			NSString* ns_string = nil;
			if(value)
			{
				// This will return nil if the string can't be converted.
				ns_string = [NSString stringWithUTF8String:value];
			}
			// It's OK if ns_string is nil.
			[the_label setText:ns_string];
			[the_label sizeToFit];
		}
	}
	return 1;

}

static char* cocoaLabelGetTitleAttrib(Ihandle* ih) {
	UILabel* the_label = ih->handle;
	if([the_label respondsToSelector:@selector(text)])
	{
		NSString *title = the_label.text;
		if (title)
		{
			return iupStrReturnStr([title UTF8String]);
		}
	}
	return NULL;
}



static int cocoaTouchLabelSetImageAttrib(Ihandle* ih, const char* value)
{
	
	if(ih->data->type == IUP_LABEL_IMAGE)
	{
		UIImageView* image_view = cocoaTouchLabelGetImageView(ih);
		if(nil == image_view)
		{
			return 0;
		}
		
		char* name;
		int make_inactive = 0;
		
		if (iupdrvIsActive(ih))
		{
			make_inactive = 0;
		}
		else
		{
			name = iupAttribGet(ih, "IMINACTIVE");
			if (!name)
			{
				make_inactive = 1;
			}
		}
		
		
		UIImage* the_bitmap;
		the_bitmap = iupImageGetImage(value, ih, make_inactive);
		int width;
		int height;
		int bpp;
		
		iupdrvImageGetInfo(the_bitmap, &width, &height, &bpp);
		
		// FIXME: What if the width and height change? Do we change it or leave it alone?
		CGSize new_size = CGSizeMake(width, height);
		CGRect the_frame = [image_view frame];
		the_frame.size = new_size;
		[image_view setFrame:the_frame];

		[image_view setImage:the_bitmap];
		
		return 1;
	}
	else
	{
		return 0;
	}
}

static int cocoaLabelMapMethod(Ihandle* ih)
{
	char* value;
	// using id because we may be using different types depending on the case
	id the_label = nil;
	
	value = iupAttribGet(ih, "SEPARATOR");
	if (value)
	{
		if (iupStrEqualNoCase(value, "HORIZONTAL"))
		{
			ih->data->type = IUP_LABEL_SEP_HORIZ;

//			NSBox* horizontal_separator= [[NSBox alloc] initWithFrame:NSMakeRect(20.0, 20.0, 250.0, 1.0)];
			UIView* horizontal_separator= [[UIView alloc] initWithFrame:CGRectMake(0.0, 0.0, 250.0, 1.0)];
			the_label = horizontal_separator;
			
		}
		else /* "VERTICAL" */
		{
			ih->data->type = IUP_LABEL_SEP_VERT;

//			NSBox* vertical_separator=[[NSBox alloc] initWithFrame:NSMakeRect(20.0, 20.0, 1.0, 250.0)];
			UIView* vertical_separator=[[UIView alloc] initWithFrame:CGRectMake(0.0, 0.0, 1.0, 250.0)];
			the_label = vertical_separator;

		}
	}
	else
	{
		value = iupAttribGet(ih, "IMAGE");
		if (value)
		{
			ih->data->type = IUP_LABEL_IMAGE;
			
			char *name;
			int make_inactive = 0;
			
			if (iupdrvIsActive(ih))
    name = iupAttribGet(ih, "IMAGE");
			else
			{
    name = iupAttribGet(ih, "IMINACTIVE");
    if (!name)
	{
		name = iupAttribGet(ih, "IMAGE");
		make_inactive = 1;
	}
			}
			
			
			id the_bitmap;
			the_bitmap = iupImageGetImage(name, ih, make_inactive);
			int width;
			int height;
			int bpp;
			
			iupdrvImageGetInfo(the_bitmap, &width, &height, &bpp);

//			static int woffset = 0;
//			static int hoffset = 0;
			
			UIImageView* image_view = [[UIImageView alloc] initWithFrame:CGRectMake(0, 0, width, height)];
//			NSImageView* image_view = [[NSImageView alloc] initWithFrame:NSMakeRect(woffset, hoffset, width, height)];
			[image_view setImage:the_bitmap];
			
//			woffset += 30;
//			hoffset += 30;
			
			the_label = image_view;
			
#if 0
			if (!the_bitmap)
					return;
			
			/* must use this info, since image can be a driver image loaded from resources */
			iupdrvImageGetInfo(hBitmap, &width, &height, &bpp);

			
			NSBitmapImageRep* bitmap_image = [[NSBitmapImageRep alloc]
									 initWithBitmapDataPlanes:NULL
									 pixelsWide: width
									 pixelsHigh: height
									 bitsPerSample: 8
									 samplesPerPixel: 4
									 hasAlpha: YES
									 isPlanar: NO
									 colorSpaceName: NSCalibratedRGBColorSpace
									 bytesPerRow: width * 4
									 bitsPerPixel: 32]
#endif

		}
		else
		{
			ih->data->type = IUP_LABEL_TEXT;

			the_label = [[UILabel alloc] initWithFrame:CGRectMake(0, 0, 100, 100)];
			[the_label setNumberOfLines:0];
//			the_label = [[NSTextField alloc] initWithFrame:NSMakeRect(0, 0, 100, 100)];

//			[the_label setSelectable:NO];
			
			UIFont* the_font = [the_label font];
			NSLog(@"font %@", the_font);
		
		
		}
	}
	
	if (!the_label)
	{
		return IUP_ERROR;
	}
	
	
	ih->handle = the_label;

	// All cocoaTouch views should call this to add the new view to the parent view.
	iupCocoaTouchAddToParent(ih);
	
	return IUP_NOERROR;
}


static void cocoaLabelUnMapMethod(Ihandle* ih)
{
	id the_label = ih->handle;
	iupCocoaTouchRemoveFromParent(ih);
	[the_label release];
	ih->handle = nil;
}

static int cocoaLabelSetBgColorAttrib(Ihandle* ih, char *iColor)
{
	unsigned char r, g, b, a;
	if (iupStrToRGBA(iColor, &r, &g, &b, &a))
	{
		CGFloat red = r/255., green = g/255., blue = b/255., alpha = a/255.;
		UIColor *color = [UIColor colorWithRed:red green:green blue:blue alpha:alpha];
		UILabel* the_label = ih->handle;
		[the_label setBackgroundColor:color];
		return IUP_NOERROR;
	}
	return IUP_ERROR;
}

static int cocoaLabelSetFGColorAttrib(Ihandle* ih, char *iColor)
{
	UILabel* the_label = ih->handle;
	if ([the_label respondsToSelector:@selector(textColor)])
	{
		unsigned char r, g, b, a;
		if (iupStrToRGBA(iColor, &r, &g, &b, &a))
		{
			CGFloat red = r/255., green = g/255., blue = b/255., alpha = a/255.;
			UIColor *color = [UIColor colorWithRed:red green:green blue:blue alpha:alpha];
			[the_label setTextColor:color];
			return IUP_NOERROR;
		}
	}
	return IUP_ERROR;
}

static char*cocoaLabelGetFGColorAttrib(Ihandle* ih)
{
	UILabel* the_label = ih->handle;
	UIColor *textColor = [the_label textColor];
	CGFloat red, green, blue, alpha;
	if ([textColor getRed:&red green:&green blue:&blue alpha:&alpha])
	{
		unsigned char r = red*255, g = green*255, b = blue*255, a = alpha*255;
		return iupStrReturnRGBA(r, g, b, a);
	}
	return NULL;
}


void iupdrvLabelInitClass(Iclass* ic)
{
  /* Driver Dependent Class functions */
  ic->Map = cocoaLabelMapMethod;
  ic->UnMap = cocoaLabelUnMapMethod;

#if 0

  /* Driver Dependent Attribute functions */

  /* Overwrite Visual */
  iupClassRegisterAttribute(ic, "ACTIVE", iupBaseGetActiveAttrib, gtkLabelSetActiveAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_DEFAULT);
	
#endif
  /* Visual */
  iupClassRegisterAttribute(ic, "BGCOLOR", iupBaseNativeParentGetBgColorAttrib, cocoaLabelSetBgColorAttrib, IUPAF_SAMEASSYSTEM, "DLGBGCOLOR", IUPAF_DEFAULT);

  /* Special */
  iupClassRegisterAttribute(ic, "FGCOLOR", cocoaLabelGetFGColorAttrib, cocoaLabelSetFGColorAttrib, "0 0 0", "DLGFGCOLOR", IUPAF_DEFAULT);
	
  iupClassRegisterAttribute(ic, "TITLE", cocoaLabelGetTitleAttrib, cocoaLabelSetTitleAttrib, NULL, NULL, IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);
#if 0
  /* IupLabel only */
  iupClassRegisterAttribute(ic, "ALIGNMENT", NULL, gtkLabelSetAlignmentAttrib, "ALEFT:ACENTER", NULL, IUPAF_NO_INHERIT);  /* force new default value */
#endif
  iupClassRegisterAttribute(ic, "IMAGE", NULL, cocoaTouchLabelSetImageAttrib, NULL, NULL, IUPAF_IHANDLENAME|IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);
#if 0
  iupClassRegisterAttribute(ic, "PADDING", iupLabelGetPaddingAttrib, gtkLabelSetPaddingAttrib, IUPAF_SAMEASSYSTEM, "0x0", IUPAF_NOT_MAPPED);

  /* IupLabel GTK and Motif only */
  iupClassRegisterAttribute(ic, "IMINACTIVE", NULL, gtkLabelSetImInactiveAttrib, NULL, NULL, IUPAF_IHANDLENAME|IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);

  /* IupLabel Windows and GTK only */
  iupClassRegisterAttribute(ic, "WORDWRAP", NULL, gtkLabelSetWordWrapAttrib, NULL, NULL, IUPAF_DEFAULT);
  iupClassRegisterAttribute(ic, "ELLIPSIS", NULL, gtkLabelSetEllipsisAttrib, NULL, NULL, IUPAF_DEFAULT);

  /* IupLabel GTK only */
  iupClassRegisterAttribute(ic, "MARKUP", NULL, NULL, NULL, NULL, IUPAF_DEFAULT);
#endif
}
