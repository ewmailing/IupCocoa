/** \file
 * \brief iupmatrix control
 * draw functions.
 *
 * See Copyright Notice in "iup.h"
 */
 
#ifndef __IUPMAT_DRAW_H 
#define __IUPMAT_DRAW_H

#ifdef __cplusplus
extern "C" {
#endif

void iupMatrixDrawCells(Ihandle* ih, int lin1, int col1, int lin2, int col2);
void iupMatrixDrawTitleColumns(Ihandle* ih, int col1, int col2);
void iupMatrixDrawTitleLines(Ihandle* ih, int lin1, int lin2);

void iupMatrixDrawSetDropFeedbackArea(int *x1, int *y1, int *x2, int *y2);
void iupMatrixDrawSetToggleFeedbackArea(int toggle_centered, int *x1, int *y1, int *x2, int *y2);

/* Render the visible cells and update display */
void iupMatrixDraw(Ihandle* ih, int update);

/* Update the display only */
void iupMatrixDrawUpdate(Ihandle* ih);

int iupMatrixDrawSetRedrawAttrib(Ihandle* ih, const char* value);

void iupMatrixAddMarkedAttenuation(Ihandle* ih, unsigned char *r, unsigned char *g, unsigned char *b);


#ifdef __cplusplus
}
#endif

#endif
