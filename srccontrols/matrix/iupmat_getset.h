/** \file
 * \brief iupmatrix control
 * attributes set and get.
 *
 * See Copyright Notice in "iup.h"
 */
 
#ifndef __IUPMAT_GETSET_H 
#define __IUPMAT_GETSET_H

#ifdef __cplusplus
extern "C" {
#endif

/* Used for draw, size computation and edition (display).
   The internal value may have been translated, converted or formatted. */
char* iupMatrixGetValueDisplay(Ihandle* ih, int lin, int col);
/* Used only when sorting numeric values.
   The internal value may have been translated or converted. */
double iupMatrixGetValueNumeric(Ihandle* ih, int lin, int col);
/* Used for getattribute, copy cell and undo (internal),
   also a string with maximum precision when numeric. */
char* iupMatrixGetValue(Ihandle* ih, int lin, int col);  

void  iupMatrixSetValue(Ihandle* ih, int lin, int col, const char* value, int user_edited);
void iupMatrixModifyValue(Ihandle* ih, int lin, int col, const char* value);

void iupMatrixSetCellFlag(Ihandle* ih, int lin, int col, unsigned char attr, int set);
int iupMatrixGetCellOffset(Ihandle* ih, int lin, int col, int *x, int *y);

void  iupMatrixPrepareDrawData(Ihandle* ih);
char* iupMatrixGetFgColorStr(Ihandle* ih, int lin, int col);
char* iupMatrixGetBgColorStr(Ihandle* ih, int lin, int col);
char* iupMatrixGetFont(Ihandle* ih, int lin, int col);
int   iupMatrixGetType(Ihandle* ih, int lin, int col);

void iupMatrixGetBgRGB(Ihandle* ih, int lin, int col, unsigned char *r, unsigned char *g, unsigned char *b, int mark, int active);
void iupMatrixGetFgRGB(Ihandle* ih, int lin, int col, unsigned char *r, unsigned char *g, unsigned char *b, int mark, int active);
void iupMatrixGetTypeRGB(Ihandle* ih, const char* color, unsigned char *r, unsigned char *g, unsigned char *b, int mark, int active);

char* iupMatrixGetMaskStr(Ihandle* ih, const char* name, int lin, int col);

char* iupMatrixGetSize(Ihandle* ih, int index, int m, int pixels_unit);
int   iupMatrixGetVisibleCellDim(Ihandle* ih, int lin, int col, int* x, int* y, int* w, int* h);
int   iupMatrixGetColumnWidth(Ihandle* ih, int col, int use_value);
int   iupMatrixGetLineHeight (Ihandle* ih, int lin, int use_value);
int   iupMatrixGetCellFromXY(Ihandle* ih, int x, int y, int* l, int* c);

int iupMatrixCheckCellPos(Ihandle* ih, int lin, int col);

char* iupMatrixGetNumericFormatDef(Ihandle* ih);


#ifdef __cplusplus
}
#endif

#endif
