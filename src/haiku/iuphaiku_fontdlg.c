/** \file
 * \brief IupFontDlg pre-defined dialog
 *
 * See Copyright Notice in "iup.h"
 */

#define UNIMPLEMENTED printf("%s (%s %d) UNIMPLEMENTED\n",__func__,__FILE__,__LINE__);
#include <stdio.h>

#include <string.h>
#include <memory.h>

#include "iup.h"

#include "iup_object.h"
#include "iup_attrib.h"
#include "iup_str.h"
#include "iup_dialog.h"

#include "iuphaiku_drv.h"


static int gtkFontDlgPopup(Ihandle* ih, int x, int y)
{
	UNIMPLEMENTED
  return IUP_NOERROR;
}

void iupdrvFontDlgInitClass(Iclass* ic)
{
  ic->DlgPopup = gtkFontDlgPopup;

  /* IupFontDialog GTK Only */
  iupClassRegisterAttribute(ic, "PREVIEWTEXT", NULL, NULL, NULL, NULL, IUPAF_NO_INHERIT);
}
