#ifndef __IUPCONTROLS_EXPORT_H
#define __IUPCONTROLS_EXPORT_H

#ifndef DOXYGEN_SHOULD_IGNORE_THIS
        /** @cond DOXYGEN_SHOULD_IGNORE_THIS */
#ifdef IUPCONTROLS_BUILD_LIBRARY
		#ifdef __EMSCRIPTEN__
				#include <emscripten.h>
                #define IUPCONTROLS_EXPORTI EMSCRIPTEN_KEEPALIVE
        #elif WIN32
                #define IUPCONTROLS_EXPORTI __declspec(dllexport)
        #elif defined(__GNUC__) && __GNUC__ >= 4
                #define IUPCONTROLS_EXPORTI __attribute__ ((visibility("default")))
        #else
                #define IUPCONTROLS_EXPORTI
        #endif
#else
        #define IUPCONTROLS_EXPORTI
#endif /* IUPCONTROLS_BUILD_LIBRARY */

/** @endcond DOXYGEN_SHOULD_IGNORE_THIS */
#endif /* DOXYGEN_SHOULD_IGNORE_THIS */


#endif /* __IUPCONTROLS_EXPORT_H */

