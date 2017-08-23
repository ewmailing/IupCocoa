/** \file
 * \brief Web control.
 *
 * See Copyright Notice in "iup.h"
 */
 
#ifndef __IUPWEB_H 
#define __IUPWEB_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef DOXYGEN_SHOULD_IGNORE_THIS
/** @cond DOXYGEN_SHOULD_IGNORE_THIS */
#ifndef IUPWEB_EXPORT
	#ifdef IUPWEB_BUILD_LIBRARY
		#ifdef __EMSCRIPTEN__
				#include <emscripten.h>
                #define IUPWEB_EXPORT EMSCRIPTEN_KEEPALIVE
        #elif WIN32
                #define IUPWEB_EXPORT __declspec(dllexport)
        #elif defined(__GNUC__) && __GNUC__ >= 4
                #define IUPWEB_EXPORT __attribute__ ((visibility("default")))
        #else
                #define IUPWEB_EXPORT
        #endif
	#else
        #define IUPWEB_EXPORT
	#endif /* IUP_BUILD_LIBRARY */
#endif /* IUPWEB_EXPORT */
/** @endcond DOXYGEN_SHOULD_IGNORE_THIS */
#endif /* DOXYGEN_SHOULD_IGNORE_THIS */

IUPWEB_EXPORT int IupWebBrowserOpen(void);

IUPWEB_EXPORT Ihandle *IupWebBrowser(void);


#ifdef __cplusplus
}
#endif

#endif
