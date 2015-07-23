/** \file
 * \brief MAC Focus
 *
 * See Copyright Notice in "iup.h"
 */

#include <Cocoa/Cocoa.h>

#include <stdio.h>

#include "iup.h"

#include "iup_object.h"
#include "iup_focus.h"
#include "iup_attrib.h"
#include "iup_drv.h"
#include "iup_assert.h" 
#include "iup_drv.h" 

#include "iupcocoa_drv.h"


void iupdrvSetFocus(Ihandle *ih)
{               
//  [ih->handle makeFirstResponder: self];
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
