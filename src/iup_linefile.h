/** \file
* \brief Line Base Text File Load
*
* See Copyright Notice in "iup.h"
*/

#ifndef __IUP_LINEFILE_H 
#define __IUP_LINEFILE_H

#if	defined(__cplusplus)
extern "C" {
#endif

typedef struct _IlineFile IlineFile;

IlineFile* iupLineFileOpen(const char* filename);
int iupLineFileReadLine(IlineFile* line_file);
const char* iupLineFileGetBuffer(IlineFile* line_file);
int iupLineFileEOF(IlineFile* line_file);
void iupLineFileClose(IlineFile* line_file);

#if defined(__cplusplus)
}
#endif

#endif
