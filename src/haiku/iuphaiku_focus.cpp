/** \file
 * \brief Haiku Focus
 *
 * See Copyright Notice in "iup.h"
 */

#include <View.h>

#include "iup.h"

#include "iup_object.h"
#include "iup_focus.h"
#include "iup_attrib.h"
#include "iup_drv.h"
#include "iup_assert.h" 
#include "iup_drv.h" 

#include "iuphaiku_drv.h"


void iupdrvSetFocus(Ihandle *ih)
{
  BView* view = (BView*)ih->handle;
  view->LockLooper();
  view->MakeFocus(true);
  view->UnlockLooper();
}

