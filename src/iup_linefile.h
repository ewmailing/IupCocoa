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

/** \defgroup linefile Line Base Text File Load API
* \par
* See \ref iup_linefile.h
* \ingroup util */

typedef struct _IlineFile IlineFile;

/** Opens an existing file. 
* Returns NULL if failed.
* \ingroup linefile */
IlineFile* iupLineFileOpen(const char* filename);

/** Closes the file.
* \ingroup linefile */
void iupLineFileClose(IlineFile* line_file);

/** Reads a line from the file.
* Returns -1 if failed, otherwise returns the line size.
* \ingroup linefile */
int iupLineFileReadLine(IlineFile* line_file);

/** Returns the buffer used to read the line.
* \ingroup linefile */
const char* iupLineFileGetBuffer(IlineFile* line_file);

/** Returns a non zero value if reached the end of the file.
* \ingroup linefile */
int iupLineFileEOF(IlineFile* line_file);


#if defined(__cplusplus)
}
#endif

#endif
