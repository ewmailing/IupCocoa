/** \file
 * \brief IupImgLib
 *
 * See Copyright Notice in iup.h
 */

#include <stdlib.h>

#include "iup.h"
#include "iup_image.h"

#include "iup_imglib.h"

void iupImglibIconsGtk48x48Open(void)
{
  iupImageStockSet("IUP_IconMessageError", 0, "gtk-dialog-error");
  iupImageStockSet("IUP_IconMessageHelp", 0, "gtk-dialog-question");
  iupImageStockSet("IUP_IconMessageInfo", 0, "gtk-dialog-info");
  iupImageStockSet("IUP_IconMessageSecurity", 0, "gtk-dialog-authentication");
  iupImageStockSet("IUP_IconMessageWarning", 0, "gtk-dialog-warning");
}
