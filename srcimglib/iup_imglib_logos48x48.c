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
#include "iup_imglib_logos48x48.h"

void iupImglibLogos48x48Open(void)
{
#ifdef IUP_IMGLIB_LARGE
  iupImageStockSet("IUP_LogoTecgraf", load_image_LogoTecgraf, 0);
  iupImageStockSet("IUP_LogoPUC-Rio", load_image_LogoPUC_Rio, 0);
  iupImageStockSet("IUP_LogoBR", load_image_LogoBR, 0);
  iupImageStockSet("IUP_LogoLua", load_image_LogoLua, 0);
  iupImageStockSet("IUP_LogoTecgrafPUC-Rio", load_image_LogoTecgrafPUC_Rio, 0);
  iupImageStockSet("IUP_LogoPetrobras", load_image_LogoPetrobras, 0);
#endif
}
