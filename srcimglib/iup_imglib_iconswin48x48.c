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
#include "iup_imglib_iconswin48x48.h"

void iupImglibIconsWin48x48Open(void)
{
#ifdef IUP_IMGLIB_LARGE
  iupImageStockSet("IUP_DeviceCamera", load_image_DeviceCamera, 0);
  iupImageStockSet("IUP_DeviceCD", load_image_DeviceCD, 0);
  iupImageStockSet("IUP_DeviceCellPhone", load_image_DeviceCellPhone, 0);
  iupImageStockSet("IUP_DeviceComputer", load_image_DeviceComputer, 0);
  iupImageStockSet("IUP_DeviceHardDrive", load_image_DeviceHardDrive, 0);
  iupImageStockSet("IUP_DeviceFax", load_image_DeviceFax, 0);
  iupImageStockSet("IUP_DeviceMP3", load_image_DeviceMP3, 0);
  iupImageStockSet("IUP_DeviceNetwork", load_image_DeviceNetwork, 0);
  iupImageStockSet("IUP_DevicePDA", load_image_DevicePDA, 0);
  iupImageStockSet("IUP_DevicePrinter", load_image_DevicePrinter, 0);
  iupImageStockSet("IUP_DeviceScanner", load_image_DeviceScanner, 0);
  iupImageStockSet("IUP_DeviceSound", load_image_DeviceSound, 0);
  iupImageStockSet("IUP_DeviceVideo", load_image_DeviceVideo, 0);

  iupImageStockSet("IUP_IconMessageError", load_image_IconMessageError, 0);
  iupImageStockSet("IUP_IconMessageHelp", load_image_IconMessageHelp, 0);
  iupImageStockSet("IUP_IconMessageInfo", load_image_IconMessageInfo, 0);
  iupImageStockSet("IUP_IconMessageSecurity", load_image_IconMessageSecurity, 0);
  iupImageStockSet("IUP_IconMessageWarning", load_image_IconMessageWarning, 0);
#endif
}
