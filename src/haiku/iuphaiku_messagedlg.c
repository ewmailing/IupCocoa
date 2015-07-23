/** \file
 * \brief GTK IupMessageDlg pre-defined dialog
 *
 * See Copyright Notice in "iup.h"
 */

#include "iup.h"

#include "iup_object.h"
#include "iup_attrib.h"
#include "iup_str.h"
#include "iup_dialog.h"

#include "iuphaiku_drv.h"

#include <stdio.h>
#define UNIMPLEMENTED printf("%s (%s %d) UNIMPLEMENTED\n",__func__,__FILE__,__LINE__);

/* Sometimes GTK decides to invert the buttons position because of the GNOME Guidelines.
   To avoid that we define different Ids for the buttons. */
#define IUP_RESPONSE_1 -100
#define IUP_RESPONSE_2 -200
#define IUP_RESPONSE_HELP -300

#ifndef GTK_MESSAGE_OTHER
#define GTK_MESSAGE_OTHER GTK_MESSAGE_INFO
#endif

static int gtkMessageDlgPopup(Ihandle* ih, int x, int y)
{
	UNIMPLEMENTED
  return IUP_NOERROR;
}

void iupdrvMessageDlgInitClass(Iclass* ic)
{
  ic->DlgPopup = gtkMessageDlgPopup;
}
