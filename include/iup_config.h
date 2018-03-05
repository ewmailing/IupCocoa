/** \file
 * \brief Configuration file Utilities
 *
 * See Copyright Notice in "iup.h"
 */

#ifndef IUP_CONFIG_H
#define IUP_CONFIG_H

#if	defined(__cplusplus)
extern "C" {
#endif

#ifndef DOXYGEN_SHOULD_IGNORE_THIS
/** @cond DOXYGEN_SHOULD_IGNORE_THIS */
#ifndef IUP_EXPORT
	#ifdef IUP_BUILD_LIBRARY
		#ifdef __EMSCRIPTEN__
				#include <emscripten.h>
                #define IUP_EXPORT EMSCRIPTEN_KEEPALIVE
        #elif WIN32
                #define IUP_EXPORT __declspec(dllexport)
        #elif defined(__GNUC__) && __GNUC__ >= 4
                #define IUP_EXPORT __attribute__ ((visibility("default")))
        #else
                #define IUP_EXPORT
        #endif
	#else
        #define IUP_EXPORT
	#endif /* IUP_BUILD_LIBRARY */
#endif /* IUP_EXPORT */
/** @endcond DOXYGEN_SHOULD_IGNORE_THIS */
#endif /* DOXYGEN_SHOULD_IGNORE_THIS */

IUP_EXPORT Ihandle* IupConfig(void);

IUP_EXPORT int IupConfigLoad(Ihandle* ih);
IUP_EXPORT int IupConfigSave(Ihandle* ih);

/****************************************************************/

IUP_EXPORT void IupConfigSetVariableStr(Ihandle* ih, const char* group, const char* key, const char* value);
IUP_EXPORT void IupConfigSetVariableStrId(Ihandle* ih, const char* group, const char* key, int id, const char* value);
IUP_EXPORT void IupConfigSetVariableInt(Ihandle* ih, const char* group, const char* key, int value);
IUP_EXPORT void IupConfigSetVariableIntId(Ihandle* ih, const char* group, const char* key, int id, int value);
IUP_EXPORT void IupConfigSetVariableDouble(Ihandle* ih, const char* group, const char* key, double value);
IUP_EXPORT void IupConfigSetVariableDoubleId(Ihandle* ih, const char* group, const char* key, int id, double value);

IUP_EXPORT const char* IupConfigGetVariableStr(Ihandle* ih, const char* group, const char* key);
IUP_EXPORT const char* IupConfigGetVariableStrId(Ihandle* ih, const char* group, const char* key, int id);
IUP_EXPORT int    IupConfigGetVariableInt(Ihandle* ih, const char* group, const char* key);
IUP_EXPORT int    IupConfigGetVariableIntId(Ihandle* ih, const char* group, const char* key, int id);
IUP_EXPORT double IupConfigGetVariableDouble(Ihandle* ih, const char* group, const char* key);
IUP_EXPORT double IupConfigGetVariableDoubleId(Ihandle* ih, const char* group, const char* key, int id);

IUP_EXPORT const char* IupConfigGetVariableStrDef(Ihandle* ih, const char* group, const char* key, const char* def);
IUP_EXPORT const char* IupConfigGetVariableStrIdDef(Ihandle* ih, const char* group, const char* key, int id, const char* def);
IUP_EXPORT int    IupConfigGetVariableIntDef(Ihandle* ih, const char* group, const char* key, int def);
IUP_EXPORT int    IupConfigGetVariableIntIdDef(Ihandle* ih, const char* group, const char* key, int id, int def);
IUP_EXPORT double IupConfigGetVariableDoubleDef(Ihandle* ih, const char* group, const char* key, double def);
IUP_EXPORT double IupConfigGetVariableDoubleIdDef(Ihandle* ih, const char* group, const char* key, int id, double def);

IUP_EXPORT void IupConfigCopy(Ihandle* ih1, Ihandle* ih2, const char* exclude_prefix);

/****************************************************************/

IUP_EXPORT void IupConfigSetListVariable(Ihandle* ih, const char *group, const char* key, const char* value, int add);

IUP_EXPORT void IupConfigRecentInit(Ihandle* ih, Ihandle* menu, Icallback recent_cb, int max_recent);
IUP_EXPORT void IupConfigRecentUpdate(Ihandle* ih, const char* filename);

IUP_EXPORT void IupConfigDialogShow(Ihandle* ih, Ihandle* dialog, const char* name);
IUP_EXPORT void IupConfigDialogClosed(Ihandle* ih, Ihandle* dialog, const char* name);


#if defined(__cplusplus)
}
#endif

#endif
