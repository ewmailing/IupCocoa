/** \file
 * \brief Windows Driver TIPS management
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdio.h>

#include "iup.h" 
#include "iupcbs.h" 

#include "iup_object.h" 
#include "iup_str.h" 
#include "iup_attrib.h" 
#include "iup_image.h" 
#include "iup_drv.h"
#include "iup_drvinfo.h"

#include "iuphaiku_drv.h"
#define UNIMPLEMENTED printf("%s (%s %d) UNIMPLEMENTED\n",__func__,__FILE__,__LINE__);

int iupdrvBaseSetTipAttrib(Ihandle* ih, const char* value)
{
	// FIXME Haiku has no official support for tooltips (there's an internal API)
	UNIMPLEMENTED
  return 1;
}

int iupdrvBaseSetTipVisibleAttrib(Ihandle* ih, const char* value)
{
	UNIMPLEMENTED
  return 0;
}

char* iupdrvBaseGetTipVisibleAttrib(Ihandle* ih)
{
	UNIMPLEMENTED
  return NULL;
}
