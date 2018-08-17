/** \file
 * \brief initializes dial, gauge, colorbrowser, colorbar controls.
 *
 * See Copyright Notice in "iup.h"
 */
 
#ifndef __IUPCONTROLS_H 
#define __IUPCONTROLS_H

#ifdef __cplusplus
extern "C" {
#endif


#ifndef DOXYGEN_SHOULD_IGNORE_THIS
/** @cond DOXYGEN_SHOULD_IGNORE_THIS */
#ifndef IUPCONTROLS_EXPORT
	#ifdef IUPCONTROLS_BUILD_LIBRARY
		#ifdef __EMSCRIPTEN__
				#include <emscripten.h>
                #define IUPCONTROLS_EXPORT EMSCRIPTEN_KEEPALIVE
        #elif WIN32
                #define IUPCONTROLS_EXPORT __declspec(dllexport)
        #elif defined(__GNUC__) && __GNUC__ >= 4
                #define IUPCONTROLS_EXPORT __attribute__ ((visibility("default")))
        #else
                #define IUPCONTROLS_EXPORT
        #endif
	#else
        #define IUPCONTROLS_EXPORT
	#endif /* IUP_BUILD_LIBRARY */
#endif /* IUPCONTROLS_EXPORT */
/** @endcond DOXYGEN_SHOULD_IGNORE_THIS */
#endif /* DOXYGEN_SHOULD_IGNORE_THIS */

IUPCONTROLS_EXPORT int  IupControlsOpen(void);

IUPCONTROLS_EXPORT Ihandle* IupCells(void);
IUPCONTROLS_EXPORT Ihandle* IupMatrix(const char *action);
IUPCONTROLS_EXPORT Ihandle* IupMatrixList(void);
IUPCONTROLS_EXPORT Ihandle* IupMatrixEx(void);

/* available only when linking with "iupluamatrix" */
IUPCONTROLS_EXPORT void IupMatrixSetFormula(Ihandle* ih, int col, const char* formula, const char* init);
IUPCONTROLS_EXPORT void IupMatrixSetDynamic(Ihandle* ih, const char* init);


#ifdef __cplusplus
}
#endif

#endif
