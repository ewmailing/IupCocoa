/** \file
 * \brief iupLua Controls initialization
 *
 * See Copyright Notice in "iup.h"
 */
 
#ifndef __IL_CONTROLS_H 
#define __IL_CONTROLS_H

#ifdef __cplusplus
extern "C" {
#endif

int diallua_open (void);
int gaugelua_open (void);
int masklua_open (void);
int matrixlua_open (void);
int cblua_open (void);
int colorbarlua_open (void);
int cellslua_open(void);

#ifdef __cplusplus
}
#endif

#endif
