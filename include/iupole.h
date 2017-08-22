/** \file
 * \brief Ole control.
 *
 * See Copyright Notice in "iup.h"
 */
 
#ifndef __IUPOLE_H 
#define __IUPOLE_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef DOXYGEN_SHOULD_IGNORE_THIS
/** @cond DOXYGEN_SHOULD_IGNORE_THIS */
#ifndef IUPOLE_EXPORT
	#ifdef IUPOLE_BUILD_LIBRARY
		#ifdef __EMSCRIPTEN__
				#include <emscripten.h>
                #define IUPOLE_EXPORT EMSCRIPTEN_KEEPALIVE
        #elif WIN32
                #define IUPOLE_EXPORT __declspec(dllexport)
        #elif defined(__GNUC__) && __GNUC__ >= 4
                #define IUPOLE_EXPORT __attribute__ ((visibility("default")))
        #else
                #define IUPOLE_EXPORT
        #endif
	#else
        #define IUPOLE_EXPORT
	#endif /* IUP_BUILD_LIBRARY */
#endif /* IUPOLE_EXPORT */
/** @endcond DOXYGEN_SHOULD_IGNORE_THIS */
#endif /* DOXYGEN_SHOULD_IGNORE_THIS */

IUPOLE_EXPORT Ihandle *IupOleControl(const char* progid);

IUPOLE_EXPORT int IupOleControlOpen(void);


#ifdef __cplusplus
}
#endif

#endif
