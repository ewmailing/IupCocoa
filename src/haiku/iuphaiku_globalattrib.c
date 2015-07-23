/** \file
 * \brief GTK Driver iupdrvSetGlobal
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdio.h>
#include <string.h>

#include "iup.h"
#include "iupcbs.h"

#include "iup_object.h"
#include "iup_str.h"
#include "iup_drv.h"
#include "iup_drvinfo.h"
#include "iup_key.h"

#include "iuphaiku_drv.h"


int iupgtk_utf8autoconvert = 1;
int iupgtk_globalmenu = 0;

#define UNIMPLEMENTED printf("%s (%s %d) UNIMPLEMENTED\n",__func__,__FILE__,__LINE__);

int iupdrvSetGlobal(const char *name, const char *value)
{
  if (iupStrEqual(name, "INPUTCALLBACKS"))
  {
	UNIMPLEMENTED
	return 1; // Return 0 when we handle it properly !
  }
  if (iupStrEqual(name, "SINGLEINSTANCE"))
  {
	UNIMPLEMENTED
	return 1; // Return 0 when we handle it properly !
  }

  // Other attributes are ignored by the driver. They are forwarded to the main
  // implementation of IUP.
  return 1;
}

int iupdrvCheckMainScreen(int *w, int *h)
{
	UNIMPLEMENTED
  return 0;
}

char *iupdrvGetGlobal(const char *name)
{
  // TODO we can return some platform-specific info here
  return NULL;
}
