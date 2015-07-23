/** \file
 * \brief iupLua internal declarations
 *
 * See Copyright Notice in "iup.h"
 */
 
#ifndef __IL_H 
#define __IL_H

#ifdef __cplusplus
extern "C" {
#endif

int iupluaapi_open(void);
int iupluawidgets_open(int tag);

int sboxlua_open(void);
int splitlua_open(void);
int spinlua_open(void);
int cboxlua_open(void);
int gclua_open (void);
int vallua_open (void);
int tabslua_open (void);
int treelua_open (void);

void iupluaScanf(void);
void getparamlua_open (void);

/** First function called inside a callback. 
 * It prepares the stack for the call and returns the Lua state.
 */
void iuplua_call_start(Ihandle* handle, char* name);

/** Called when a callback in Lua should be called. 
 * Returns the callback return value. If nil returns IUP_DEFAULT.
 * At the end clears the Lua stack. 
 */
int iuplua_call(void);

/** Same as /ref iuplua_call, but returns a string.
 */
char* iuplua_call_rs(void);

/** Register a function.
 */
void iuplua_register(char* name, lua_CFunction func);

/** Register a function. But consider no prefix.
 */
void iuplua_register_macro(char* name, lua_CFunction func);

/** Register a number.
 */
void iuplua_regnumber(double num, char* name);

/** Register a string.
 */
void iuplua_regstring(char* str, char* name);

/** Register a string, but consider only 3 character as prefix.
 */
void iuplua_regstring_mask(char* str, char* name);

/** Returns an array of ihandle stored in a Lua table.
 */
Ihandle** iuplua_checkihandle_array(int numArg);

/** Pushes an ihandle inside a callback. 
 * If ihandle is from C must call iuplua_regihandle before iuplua_call_start.
 */
void iuplua_pushihandle_cb(Ihandle *n);

/** Register an ihandle created in C in Lua.
 */
void iuplua_regihandle(Ihandle *n);


#ifdef __cplusplus
}
#endif

#endif
