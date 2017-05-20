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

#include "SSYMoreProgressView.h"

/* Like awakeFromNib */
static int cocoaProgressBarMapMethod(Ihandle* ih)
{
	static int woffset = 0;
	static int hoffset = 0;
	
	woffset += 60;
	hoffset += 10;
	//	ih->data->type = 0;

    CGRect frame = CGRectMake(woffset, hoffset, 200, 40);
	SSYMoreProgressView* progress_indicator = [[SSYMoreProgressView alloc] initWithFrame:frame];
	
	if (iupStrEqualNoCase(iupAttribGetStr(ih, "ORIENTATION"), "VERTICAL"))
	{
		[progress_indicator setIsVertical:YES];

        NSLog(@"user size:  %f  x  %f", ih->userwidth, ih->userheight) ;
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
    SSYMoreProgressView* the_bar = ih->handle;
    float progress = [the_bar progress];
    char string[255];
    snprintf(string, 255, "VALUE FROM COCOA: %g", progress);
    return string;
}

static int cocoaProgressBarSetValueAttrib(Ihandle* ih, const char* value)
{
    SSYMoreProgressView* progress_bar = (SSYMoreProgressView*)ih->handle;
    
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
    SSYMoreProgressView* the_bar = ih->handle;
    UIColor *textColor = [the_bar progressTintColor];
    return iupCocoaTouchColorFromNative(textColor);
}

static int cocoaProgressBarSetFgColorAttrib(Ihandle* ih, char *iColor)
{
    UIColor *color = iupCocoaTouchToNativeColor(iColor);
    if (color)
    {
        SSYMoreProgressView* the_bar = ih->handle;
        [the_bar setProgressTintColor:color];
        return IUP_NOERROR;
    }
    return IUP_ERROR;
}


static char* cocoaProgressBarGetBgColorAttrib(Ihandle* ih)
{
    SSYMoreProgressView* the_bar = ih->handle;
    UIColor *textColor = [the_bar trackTintColor];
    return iupCocoaTouchColorFromNative(textColor);
}


static int cocoaProgressBarSetBgColorAttrib(Ihandle* ih, char *iColor)
{
    UIColor *color = iupCocoaTouchToNativeColor(iColor);
    if (color)
    {
        SSYMoreProgressView* the_bar = ih->handle;
        [the_bar setTrackTintColor:color];
        return IUP_NOERROR;
    }
    return IUP_ERROR;
}

static char* cocoaProgressBarGetMarqueeAttrib(Ihandle* ih)
{
    SSYMoreProgressView* the_bar = (SSYMoreProgressView*)ih->handle;
    // Seems like there should be a iup function to convert bool --> "YES" : "NO" ??????
    return [the_bar indeterminate] ? "YES" : "NO";
}

static int cocoaProgressBarSetMarqueeAttrib(Ihandle* ih, const char* value)
{
    if (!ih->data->marquee)
    {
        return 0;
    }
    
    SSYMoreProgressView* progress_bar = (SSYMoreProgressView*)ih->handle;
    [progress_bar setIndeterminate:iupStrBoolean(value)];
    
    return 1;
}

static char* cocoaProgressBarGetOrientationAttrib(Ihandle* ih)
{
    SSYMoreProgressView* the_bar = ih->handle;
    return [the_bar isVertical] ? "VERTICAL" : "HORIZONTAL";
}

static int cocoaProgressBarSetOrientationAttrib(Ihandle* ih, char *orientation)
{
    SSYMoreProgressView* the_bar = ih->handle;
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
