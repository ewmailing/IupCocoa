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

// TODO: API: I think we're going to need a separate start/stop key.
// Cocoa Indeterminate is for progresses you don't know the range for, but are still animated when in progress.

// TODO: FEATURE: Cocoa provides spinner style

#if 0
static int cocoaProgressBarSetValueAttrib(Ihandle* ih, const char* value)
{
	NSProgressIndicator* progress_bar = (NSProgressIndicator*)ih->handle;

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
	
	[progress_bar setMinValue:ih->data->vmin];
	[progress_bar setMinValue:ih->data->vmax];
	[progress_bar setDoubleValue:ih->data->value];
	// Not sure if I really need this, but
	// https://developer.apple.com/library/mac/qa/qa1473/_index.html
	[progress_bar displayIfNeeded];
	
	

//	gtk_progress_bar_set_fraction(pbar, (ih->data->value - ih->data->vmin) / (ih->data->vmax - ih->data->vmin));
	
	return 0;
}

static int cocoaProgressBarSetMarqueeAttrib(Ihandle* ih, const char* value)
{
	if (!ih->data->marquee)
	{
		return 0;
	}
	NSProgressIndicator* progress_bar = (NSProgressIndicator*)ih->handle;

	if (iupStrBoolean(value))
	{
		// FIXME: This feels like a hack
		[progress_bar startAnimation:nil];
		IupSetAttribute(ih->data->timer, "RUN", "YES");
	}
	else
	{
		// FIXME: This feels like a hack
		[progress_bar stopAnimation:nil];
	}
	return 1;
}


static int cocoaProgressBarMapMethod(Ihandle* ih)
{
//	char* value;


	static int woffset = 0;
	static int hoffset = 0;
	
	woffset += 60;
	hoffset += 10;
	//	ih->data->type = 0;

	NSProgressIndicator* progress_indicator = [[NSProgressIndicator alloc] initWithFrame:NSMakeRect(woffset, hoffset, 200, 40)];
	
//	NSProgressIndicator* progress_indicator = [[NSProgressIndicator alloc] initWithFrame:NSZeroRect];
	
	[progress_indicator setUsesThreadedAnimation:YES];
	
	// FIXME: Iup doesn't seem to have explicit start/stop commands.
	// Cocoa Indeterminate is for progresses you don't know the range for, but are still animated when in progress.
	[progress_indicator startAnimation:nil];

	
	if (iupStrEqualNoCase(iupAttribGetStr(ih, "ORIENTATION"), "VERTICAL"))
	{
		// This might require layer-backed views to be active
		[progress_indicator setFrameCenterRotation:M_PI/180.0 * 90.0];

		
		if (ih->userheight < ih->userwidth)
		{
			int tmp = ih->userheight;
			ih->userheight = ih->userwidth;
			ih->userwidth = tmp;
		}
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
	

	

	

	

	
	[progress_indicator sizeToFit];
	
	
	
	ih->handle = progress_indicator;

	
	
	// All Cocoa views shoud call this to add the new view to the parent view.
	iupCocoaAddToParent(ih);
	

	
	
	
	
	
	return IUP_NOERROR;
}

static void cocoaProgressBarUnMapMethod(Ihandle* ih)
{
	id progress_bar = ih->handle;
	[progress_bar release];
	ih->handle = NULL;
	
}
#endif

void iupdrvProgressBarInitClass(Iclass* ic)
{
#if 0
  /* Driver Dependent Class functions */
	ic->Map = cocoaProgressBarMapMethod;
	ic->UnMap = cocoaProgressBarUnMapMethod;

  /* Driver Dependent Attribute functions */
  
  /* Visual */
  iupClassRegisterAttribute(ic, "BGCOLOR", NULL, iupdrvBaseSetBgColorAttrib, IUPAF_SAMEASSYSTEM, "DLGBGCOLOR", IUPAF_DEFAULT);
  
  /* Special */
  iupClassRegisterAttribute(ic, "FGCOLOR", NULL, NULL, NULL, NULL, IUPAF_DEFAULT);
#if 1

  /* IupProgressBar only */
  iupClassRegisterAttribute(ic, "VALUE",  iProgressBarGetValueAttrib,  cocoaProgressBarSetValueAttrib,  NULL, NULL, IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);

	
  iupClassRegisterAttribute(ic, "ORIENTATION", NULL, NULL, IUPAF_SAMEASSYSTEM, "HORIZONTAL", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "MARQUEE",     NULL, cocoaProgressBarSetMarqueeAttrib, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "DASHED",      NULL, NULL, NULL, NULL, IUPAF_NO_INHERIT);
#endif
#endif
}
