/** \file
 * \brief MAC Focus
 *
 * See Copyright Notice in "iup.h"
 */

#include <UIKit/UIKit.h>

#include <stdio.h>

#include "iup.h"

#include "iup_object.h"
#include "iup_focus.h"
#include "iup_attrib.h"
#include "iup_drv.h"
#include "iup_assert.h" 
#include "iup_drv.h" 

#include "iupcocoatouch_drv.h"


void iupdrvSetFocus(Ihandle *ih)
{
#if 0
//  [ih->handle makeFirstResponder: self];
	id the_object = ih->handle;
	if([the_object isKindOfClass:[UIWindow class]])
	{
		[(UIWindow*)the_object makeKeyAndOrderFront:nil];
	}
	else if([the_object isKindOfClass:[UIView class]])
	{
		[[(UIView*)the_object window] makeFirstResponder:the_object];
	}

#endif
	
}

void iupmacFocusInOutEvent(Ihandle *ih)
{
/*
  if (evt->in)
  {
    iupCallGetFocusCb(ih);
  }
  else
    iupCallKillFocusCb(ih);

  return FALSE; 
*/
}
