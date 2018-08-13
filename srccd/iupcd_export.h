#ifndef __IUPCD_EXPORT_H
#define __IUPCD_EXPORT_H

#ifndef DOXYGEN_SHOULD_IGNORE_THIS
        /** @cond DOXYGEN_SHOULD_IGNORE_THIS */
#ifdef IUPCD_BUILD_LIBRARY
		#ifdef __EMSCRIPTEN__
				#include <emscripten.h>
                #define IUPCD_EXPORTI EMSCRIPTEN_KEEPALIVE
        #elif WIN32
                #define IUPCD_EXPORTI __declspec(dllexport)
        #elif defined(__GNUC__) && __GNUC__ >= 4
                #define IUPCD_EXPORTI __attribute__ ((visibility("default")))
        #else
                #define IUPCD_EXPORTI
        #endif
#else
        #define IUPCD_EXPORTI
#endif /* IUPCD_BUILD_LIBRARY */

/** @endcond DOXYGEN_SHOULD_IGNORE_THIS */
#endif /* DOXYGEN_SHOULD_IGNORE_THIS */


#endif /* __IUPCD_EXPORT_H */

