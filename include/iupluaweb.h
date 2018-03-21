/** \file
 * \brief Binding of iupwebbrowser to Lua.
 *
 * See Copyright Notice in "iup.h"
 */
 
#ifndef __IUPLUAWEB_H 
#define __IUPLUAWEB_H

#ifdef __cplusplus
extern "C" {
#endif

#include "iupweb.h" /* needed to get around code generation limitation of only one include file for il_webbrowser.c */

#ifdef IUPLUAWEB_BUILD_LIBRARY
		#ifdef __EMSCRIPTEN__
				#include <emscripten.h>
                #define IUPLUAWEB_EXPORT EMSCRIPTEN_KEEPALIVE
        #elif WIN32
                #define IUPLUAWEB_EXPORT __declspec(dllexport)
        #elif defined(__GNUC__) && __GNUC__ >= 4
                #define IUPLUAWEB_EXPORT __attribute__ ((visibility("default")))
        #else
                #define IUPLUAWEB_EXPORT
        #endif
#else
        #define IUPLUAWEB_EXPORT
#endif /* IUPLUA_EXPORT */


IUPLUAWEB_EXPORT int iupweblua_open (lua_State * L);
IUPLUAWEB_EXPORT int luaopen_iupluaweb (lua_State * L);

#ifdef __cplusplus
}
#endif

#endif
