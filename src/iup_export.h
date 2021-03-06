#ifndef __IUP_EXPORT_H
#define __IUP_EXPORT_H

#ifndef DOXYGEN_SHOULD_IGNORE_THIS
        /** @cond DOXYGEN_SHOULD_IGNORE_THIS */
#ifdef IUP_BUILD_LIBRARY
		#ifdef __EMSCRIPTEN__
				#include <emscripten.h>
                #define IUP_EXPORTI EMSCRIPTEN_KEEPALIVE
        #elif WIN32
                #define IUP_EXPORTI __declspec(dllexport)
        #elif defined(__GNUC__) && __GNUC__ >= 4
                #define IUP_EXPORTI __attribute__ ((visibility("default")))
        #else
                #define IUP_EXPORTI
        #endif
#else
        #define IUP_EXPORTI
#endif /* IUP_BUILD_LIBRARY */

/** @endcond DOXYGEN_SHOULD_IGNORE_THIS */
#endif /* DOXYGEN_SHOULD_IGNORE_THIS */


#endif /* __IUP_EXPORT_H */

