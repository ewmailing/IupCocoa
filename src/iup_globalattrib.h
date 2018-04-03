/** \file
 * \brief global attributes environment (not exported API)
 *
 * See Copyright Notice in "iup.h"
 */
 
#ifndef __IUP_GLOBALATTRIB_H 
#define __IUP_GLOBALATTRIB_H

#ifdef __cplusplus
extern "C" {
#endif

#include "iup_export.h"

/* called only in IupOpen and IupClose */
void iupGlobalAttribInit(void);
void iupGlobalAttribFinish(void);

IUP_EXPORTI int iupGlobalIsPointer(const char* name);

int iupGlobalDefaultColorChanged(const char *name);    /* check if user changed */
void iupGlobalSetDefaultColorAttrib(const char* name, int r, int g, int b);  /* internal change method */

/* Other functions declared in <iup.h> and implemented here. 
IupSetGlobal
IupStoreGlobal
IupGetGlobal
*/

#ifdef __cplusplus
}
#endif

#endif
