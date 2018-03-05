/** \file
 * \brief IupImgLib
 *
 * See Copyright Notice in iup.h
 */

#include <stdlib.h>

#include "iup.h"
#include "iup_image.h"

#include "iup_imglib.h"


void IupImageLibOpen(void)
{
  if (IupGetGlobal("_IUP_IMAGELIB_OPEN"))
    return;

  IupSetGlobal("_IUP_IMAGELIB_OPEN", "1");

  /**************** BaseLib *****************/

#if defined(WIN32)
  /* iupImglibBaseLibWin16x16Open(); */
  iupImglibBaseLibWin32x32Open();
#elif defined(MOTIF)
  iupImglibBaseLibMot16x16Open();
#elif defined(__EMSCRIPTEN__)
#elif defined(__APPLE__)
#elif defined(__ANDROID__)
#elif defined(GTK3)
  iupImglibBaseLibGtk324x24Open();
#else
  iupImglibBaseLibGtk24x24Open();
#endif  

  /***************** Logos *****************/

#if defined(MOTIF)
  iupImglibLogosMot32x32Open();
#elif defined(__EMSCRIPTEN__)
#elif defined(__APPLE__)
#elif defined(__ANDROID__)
#else
  iupImglibLogos32x32Open();
#endif

#if defined(MOTIF)
    iupImglibLogosMot48x48Open();
#elif defined(__EMSCRIPTEN__)
#elif defined(__APPLE__)
#elif defined(__ANDROID__)
#else
    iupImglibLogos48x48Open();
#endif

  /***************** Icons *****************/

#ifdef WIN32
  iupImglibIconsWin48x48Open();
#elif defined(MOTIF)
#elif defined(__EMSCRIPTEN__)
#elif defined(__APPLE__)
#elif defined(__ANDROID__)
#elif defined(GTK3)
  iupImglibIconsGtk348x48Open();
#else
  iupImglibIconsGtk48x48Open();
#endif  

  iupImglibCircleProgress();
}
