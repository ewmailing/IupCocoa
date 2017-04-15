/** \file
 * \brief IupImgLib
 *
 * See Copyright Notice in iup.h
 */

#include <stdlib.h>

#include "iup.h"
#include "iup_image.h"

#include "iup_imglib.h"

/* source code, included only here */
#include "iup_imglib_logos32x32.h"

void iupImglibLogos32x32Open(void)
{
  iupImageStockSet("IUP_Tecgraf", load_image_Tecgraf, 0);
  iupImageStockSet("IUP_PUC-Rio", load_image_PUC_Rio, 0);
  iupImageStockSet("IUP_BR", load_image_BR, 0);
  iupImageStockSet("IUP_Lua", load_image_Lua, 0);
  iupImageStockSet("IUP_TecgrafPUC-Rio", load_image_TecgrafPUC_Rio, 0);
  iupImageStockSet("IUP_Petrobras", load_image_Petrobras, 0);
}
