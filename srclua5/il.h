/** \file
 * \brief IUPLua5 internal Functions
 *
 * See Copyright Notice in "iup.h"
 */
 
#ifndef __IL_H 
#define __IL_H

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

int iuplistlua_open (lua_State * L);
int iuplabellua_open (lua_State * L);
int iupitemlua_open (lua_State * L);
int iupimagelua_open (lua_State * L);
int iuphboxlua_open (lua_State * L);
int iupframelua_open (lua_State * L);
int iupfilllua_open (lua_State * L);
int iupfiledlglua_open (lua_State * L);
int iupdialoglua_open (lua_State * L);
int iupcanvaslua_open (lua_State * L);
int iupbuttonlua_open (lua_State * L);
int iupzboxlua_open (lua_State * L);
int iupvboxlua_open (lua_State * L);
int iuptogglelua_open (lua_State * L);
int iuptimerlua_open (lua_State * L);
int iuptextlua_open (lua_State * L);
int iupsubmenulua_open (lua_State * L);
int iupseparatorlua_open (lua_State * L);
int iupradiolua_open (lua_State * L);
int iupmultilinelua_open (lua_State * L);
int iupmenulua_open (lua_State * L);
int iupcboxlua_open (lua_State * L);
int iupspinboxlua_open (lua_State * L);
int iupspinlua_open (lua_State * L);
int iupsboxlua_open (lua_State * L);
int iupscrollboxlua_open(lua_State * L);
int iupflatscrollboxlua_open(lua_State * L);
int iupgridboxlua_open(lua_State * L);
int iuplinklua_open (lua_State * L);
int iupsplitlua_open (lua_State * L);
int iupgclua_open (lua_State * L);
int iupvallua_open (lua_State * L);
int iuptabslua_open (lua_State * L);
int iupflattabslua_open(lua_State * L);
int iupfontdlglua_open(lua_State * L);
int iupmessagedlglua_open(lua_State * L);
int iupcolordlglua_open(lua_State * L);
int iupimagergbalua_open(lua_State * L);
int iupimagergblua_open(lua_State * L);
int iupprogressbarlua_open(lua_State * L);
int iupnormalizerlua_open(lua_State * L);
int iupuserlua_open(lua_State * L);
int iuptreelua_open(lua_State * L);
int iupclipboardlua_open(lua_State * L);
int iupexpanderlua_open(lua_State * L);
int iupprogressdlglua_open(lua_State * L);
int iupdetachboxlua_open(lua_State * L);
int iupbackgroundboxlua_open(lua_State * L);
int iupflatbuttonlua_open(lua_State * L);
int iupflattogglelua_open(lua_State * L);
int iupflatframelua_open(lua_State * L);
int iupanimatedlabellua_open(lua_State * L);
int iupcalendarlua_open(lua_State * L);
int iupdatepicklua_open(lua_State * L);
int iupgaugelua_open(lua_State * L);
int iupdiallua_open(lua_State * L);
int iupcolorbarlua_open (lua_State * L);
int iupcolorbrowserlua_open (lua_State * L);
int iupflatlabellua_open(lua_State * L);
int iupdropbuttonlua_open(lua_State * L);
int iupflatseparatorlua_open(lua_State * L);

void iupgetparamlua_open (lua_State * L);
void iupconfiglua_open(lua_State * L);
int iupparamlua_open(lua_State * L);
int iupparamboxlua_open(lua_State * L);
void iupluadraw_open(lua_State * L);

int iupluaScanf(lua_State *L);
void iupluaapi_open(lua_State * L);


            /*     Registration    */

/** Gets the global environment iup.
    Used by secondary iuplua modules. */
IUPLUA_EXPORT void iuplua_get_env(lua_State *L);

/** Returns true if IupOpen was called from inside the IupLua initialization function (iuplua_open).
 * All the other C initialization functions are called from the respective IupLua initialization functions. */
IUPLUA_EXPORT int iuplua_opencall_internal(lua_State *L);

/** Register a function. */
IUPLUA_EXPORT void iuplua_register(lua_State *L, lua_CFunction func, const char* name);

/** Register a string. */
IUPLUA_EXPORT void iuplua_regstring(lua_State *L, const char* str, const char* name);

/* Register a library. */
IUPLUA_EXPORT void iuplua_register_lib(lua_State *L, const luaL_Reg* funcs);

/* Register several functions. */
IUPLUA_EXPORT void iuplua_register_funcs(lua_State *L, const luaL_Reg* funcs);



          /*              Callbacks             */

/** Stores the Lua State inside the IUP element,
 * so it can be retrieved from inside a callback.
 * Used in the creation of the element.
 * Only called before iuplua_pushihandle_raw.
 */
IUPLUA_EXPORT void iuplua_plugstate(lua_State *L, Ihandle *ih);

/** Retrieve the Lua State. Used inside a callback.
 */
IUPLUA_EXPORT lua_State* iuplua_getstate(Ihandle *ih);

/* Push iup[name] in the stack */
IUPLUA_EXPORT void iuplua_push_name(lua_State *L, const char* name);

/** First function called inside a callback. 
 * It prepares the stack for the call and returns the Lua state.
 */
IUPLUA_EXPORT lua_State * iuplua_call_start(Ihandle *ih, const char* name);

/** Called when a callback in Lua should be called. 
 * nargs is the number of arguments after the first Ihandle*.
 * Returns the callback return value. If nil returns IUP_DEFAULT.
 * At the end clears the Lua stack. 
 */
IUPLUA_EXPORT int iuplua_call(lua_State *L, int nargs);

/** Same as /ref iuplua_call, but returns a string.
 */
IUPLUA_EXPORT char* iuplua_call_ret_s(lua_State *L, int nargs);

/** Same as /ref iuplua_call, but returns a double.
 */
IUPLUA_EXPORT double iuplua_call_ret_d(lua_State *L, int nargs);

/** Same as lua_pcall, but if an error occour then
 * _ERRORMESSAGE is called. 
 */
IUPLUA_EXPORT int iuplua_call_raw(lua_State* L, int nargs, int nresults);

/** Register the callback in the iup.callbaks table.
 * If type is not NULL, register in a sub table with the controls class name.
 */
IUPLUA_EXPORT void iuplua_register_cb(lua_State *L, const char* name, lua_CFunction func, const char* type);


            /*     Functions                     */

/** Pushes a raw Ihandle into the stack (an userdata).
 * Used ONLY by the control creation functions.
 */
IUPLUA_EXPORT void iuplua_pushihandle_raw(lua_State *L, Ihandle *h);

/** Returns an array of string stored in a Lua table. 
 If count is 0, table size is used. Else table size must match count. */
IUPLUA_EXPORT char** iuplua_checkstring_array(lua_State *L, int pos, int count);

/** Returns an array of int stored in a Lua table.
 If count is 0, table size is used. Else table size must match count. */
IUPLUA_EXPORT int* iuplua_checkint_array(lua_State *L, int pos, int count);

/** Returns an array of float stored in a Lua table.
 If count is 0, table size is used. Else table size must match count. */
IUPLUA_EXPORT float* iuplua_checkfloat_array(lua_State *L, int pos, int count);

/** Returns an array of double stored in a Lua table.
If count is 0, table size is used. Else table size must match count. */
IUPLUA_EXPORT double* iuplua_checkdouble_array(lua_State *L, int pos, int count);

/** Returns an array of unsigned char stored in a Lua table.
 If count is 0, table size is used. Else table size must match count. */
IUPLUA_EXPORT unsigned char* iuplua_checkuchar_array(lua_State *L, int pos, int count);

/** Returns an array of Ihandle* stored in a Lua table.
 If count is 0, table size is used. Else table size must match count. */
IUPLUA_EXPORT Ihandle** iuplua_checkihandle_array(lua_State *L, int pos, int count);

/** Same as iuplua_checkihandle but also accepts nil. */
IUPLUA_EXPORT Ihandle *iuplua_checkihandleornil(lua_State *L, int pos);

#if LUA_VERSION_NUM > 501
#define iuplua_getn(L,i)          ((int)lua_rawlen(L, i))
#else
#define LUA_OK		0
#define iuplua_getn(L,i)          ((int)lua_objlen(L, i))
#endif


#ifdef __cplusplus
}
#endif

#endif
