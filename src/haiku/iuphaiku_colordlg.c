/** \file
 * \brief IupColorDlg pre-defined dialog
 *
 * See Copyright Notice in "iup.h"
 */

#include <string.h>
#include <memory.h>
#include <stdio.h>

#include "iup.h"

#include "iup_object.h"
#include "iup_attrib.h"
#include "iup_str.h"
#include "iup_dialog.h"

#include "iuphaiku_drv.h"

#define UNIMPLEMENTED printf("%s (%s %d) UNIMPLEMENTED\n",__func__,__FILE__,__LINE__);

static int gtkColorDlgPopup(Ihandle* ih, int x, int y)
{
	UNIMPLEMENTED
  return IUP_NOERROR;
}

void iupdrvColorDlgInitClass(Iclass* ic)
{
  ic->DlgPopup = gtkColorDlgPopup;
}
