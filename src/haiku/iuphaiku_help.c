/** \file
 * \brief GTK Driver IupHelp for non Windows systems
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "iup.h"

#include "iup_str.h"

#define UNIMPLEMENTED printf("%s (%s %d) UNIMPLEMENTED\n",__func__,__FILE__,__LINE__);

int IupHelp(const char *url)
{
  // TODO open the html page in web browser
  UNIMPLEMENTED

  return -1;
}
