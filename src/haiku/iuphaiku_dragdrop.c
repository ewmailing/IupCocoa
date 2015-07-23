/** \file
 * \brief GTK Drag&Drop Functions
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdio.h>              
#include <stdlib.h>
#include <string.h>             
#include <limits.h>             

#include "iup.h"
#include "iupcbs.h"

#include "iup_object.h"
#include "iup_str.h"
#include "iup_class.h"
#include "iup_attrib.h"
#include "iup_drv.h"
#include "iup_key.h"

#include "iuphaiku_drv.h"

#define UNIMPLEMENTED printf("%s (%s %d) UNIMPLEMENTED\n",__func__,__FILE__,__LINE__);

/******************************************************************************************/


static int gtkSetDropTypesAttrib(Ihandle* ih, const char* value)
{
	UNIMPLEMENTED
  return 1;
}

static int gtkSetDropTargetAttrib(Ihandle* ih, const char* value)
{
	UNIMPLEMENTED
  return 1;
}

static int gtkSetDragTypesAttrib(Ihandle* ih, const char* value)
{
	UNIMPLEMENTED
  return 1;
}

static int gtkSetDragSourceAttrib(Ihandle* ih, const char* value)
{
	UNIMPLEMENTED
  return 1;
}


/******************************************************************************************/


static int gtkSetDropFilesTargetAttrib(Ihandle* ih, const char* value)
{
	UNIMPLEMENTED
  return 1;
}


/******************************************************************************************/


void iupdrvRegisterDragDropAttrib(Iclass* ic)
{
  iupClassRegisterCallback(ic, "DROPFILES_CB", "siii");

  iupClassRegisterCallback(ic, "DRAGBEGIN_CB", "ii");
  iupClassRegisterCallback(ic, "DRAGDATASIZE_CB", "s");
  iupClassRegisterCallback(ic, "DRAGDATA_CB", "sCi");
  iupClassRegisterCallback(ic, "DRAGEND_CB", "i");
  iupClassRegisterCallback(ic, "DROPDATA_CB", "sCiii");
  iupClassRegisterCallback(ic, "DROPMOTION_CB", "iis");

  iupClassRegisterAttribute(ic, "DRAGTYPES",  NULL, gtkSetDragTypesAttrib,  NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "DROPTYPES",  NULL, gtkSetDropTypesAttrib,  NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "DRAGSOURCE", NULL, gtkSetDragSourceAttrib, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "DROPTARGET", NULL, gtkSetDropTargetAttrib, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "DRAGSOURCEMOVE", NULL, NULL, NULL, NULL, IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "DRAGDROP", NULL, gtkSetDropFilesTargetAttrib, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "DROPFILESTARGET", NULL, gtkSetDropFilesTargetAttrib, NULL, NULL, IUPAF_NO_INHERIT);
}

/* TODO:
  Could not find a way to disable the internal DND support 
  in IupText(GtkTextView or GtkEntry).
  Mixing the generic support from here and the internal gives weird results.
  So the application should use only the internal in this case.
  The edit box in a IupList has the same problem.
*/
