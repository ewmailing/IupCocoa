/** \file
 * \brief IUP Binding for Lua.
 *
 * See Copyright Notice in "iup.h"
 */
 
#ifndef __IUPLUA_H 
#define __IUPLUA_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef IUPLUA_BUILD_LIBRARY
		#ifdef __EMSCRIPTEN__
				#include <emscripten.h>
                #define IUPLUA_EXPORT EMSCRIPTEN_KEEPALIVE
        #elif WIN32
                #define IUPLUA_EXPORT __declspec(dllexport)
        #elif defined(__GNUC__) && __GNUC__ >= 4
                #define IUPLUA_EXPORT __attribute__ ((visibility("default")))
        #else
                #define IUPLUA_EXPORT
        #endif
#else
        #define IUPLUA_EXPORT
#endif /* IUPLUA_EXPORT */


IUPLUA_EXPORT int iuplua_open(lua_State *L);
IUPLUA_EXPORT int iupkey_open(lua_State *L);  /* does nothing, kept for backward compatibility */
IUPLUA_EXPORT int iuplua_close(lua_State * L);

/* utilities */
IUPLUA_EXPORT int iuplua_isihandle(lua_State *L, int pos);
IUPLUA_EXPORT Ihandle* iuplua_checkihandle(lua_State *L, int pos);
IUPLUA_EXPORT void iuplua_pushihandle(lua_State *L, Ihandle *n);
IUPLUA_EXPORT int iuplua_dofile(lua_State *L, const char *filename);
IUPLUA_EXPORT int iuplua_dostring(lua_State *L, const char *string, const char *chunk_name);
IUPLUA_EXPORT int iuplua_dobuffer(lua_State *L, const char *buffer, int len, const char *chunk_name);
IUPLUA_EXPORT void iuplua_show_error_message(const char *pname, const char* msg);

#ifdef __cplusplus
}
#endif

#endif
