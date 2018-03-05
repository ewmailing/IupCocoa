/** \file
 * \brief iupmatrix control
 * draw functions
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "iup.h"
#include "iupcbs.h"

#include <cd.h>
#include <cd_old.h>

#include "iup_object.h"
#include "iup_attrib.h"
#include "iup_str.h"
#include "iup_drv.h"
#include "iup_drvfont.h"
#include "iup_stdcontrols.h"
#include "iup_image.h"

#include "iup_controls.h"
#include "iup_cdutil.h"

#include "iupmat_def.h"
#include "iupmat_cd.h"
#include "iupmat_draw.h"
#include "iupmat_aux.h"
#include "iupmat_getset.h"
#include "iupmat_mark.h"


#define IMAT_FEEDBACK_SIZE 16

static unsigned char imatrix_toggleoff_alpha[IMAT_FEEDBACK_SIZE * IMAT_FEEDBACK_SIZE] =
{
  255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
  255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255,
  255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255,
  255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255,
  255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255,
  255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255,
  255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255,
  255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255,
  255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255,
  255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255,
  255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255,
  255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255,
  255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255,
  255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255,
  255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255,
  255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255
};

static unsigned char imatrix_toggleon_alpha[IMAT_FEEDBACK_SIZE * IMAT_FEEDBACK_SIZE] =
{
  255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
  255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255,
  255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255,
  255, 0, 0, 0, 0, 0, 31, 0, 0, 0, 0, 0, 0, 0, 0, 255,
  255, 0, 0, 0, 0, 89, 252, 89, 0, 0, 0, 0, 0, 0, 0, 255,
  255, 0, 0, 0, 84, 239, 106, 239, 84, 0, 0, 0, 0, 0, 0, 255,
  255, 0, 0, 84, 239, 60, 0, 60, 239, 84, 0, 0, 0, 0, 0, 255,
  255, 0, 84, 239, 60, 0, 0, 0, 60, 239, 84, 0, 0, 0, 0, 255,
  255, 16, 227, 60, 0, 0, 0, 0, 0, 60, 239, 84, 0, 0, 0, 255,
  255, 0, 8, 0, 0, 0, 0, 0, 0, 0, 60, 239, 84, 0, 0, 255,
  255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 60, 239, 84, 0, 255,
  255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 60, 227, 16, 255,
  255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 0, 255,
  255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255,
  255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255,
  255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255
};

static unsigned char imatrix_dropdown_alpha[IMAT_FEEDBACK_SIZE * IMAT_FEEDBACK_SIZE] =
{
  000, 000, 000, 000, 000, 000, 000, 000, 000, 000, 000, 000, 000, 000, 000, 000,
  000, 000, 000, 000, 000, 000, 000, 000, 000, 000, 000, 000, 000, 000, 000, 000,
  000, 000, 000, 000, 000, 000, 000, 000, 000, 000, 000, 000, 000, 000, 000, 000,
  000, 000, 000, 000, 000, 000, 000, 128, 128, 000, 000, 000, 000, 000, 000, 000,
  000, 000, 000, 000, 000, 000, 128, 255, 255, 128, 000, 000, 000, 000, 000, 000,
  000, 000, 000, 000, 000, 128, 255, 255, 255, 255, 128, 000, 000, 000, 000, 000,
  000, 000, 000, 000, 128, 255, 255, 255, 255, 255, 255, 128, 000, 000, 000, 000,
  000, 000, 000, 128, 255, 255, 255, 255, 255, 255, 255, 255, 128, 000, 000, 000,
  000, 000, 128, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 128, 000, 000,
  000, 000, 000, 000, 000, 000, 000, 000, 000, 000, 000, 000, 000, 000, 000, 000,
  000, 000, 000, 000, 000, 000, 000, 000, 000, 000, 000, 000, 000, 000, 000, 000,
  000, 000, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 000, 000,
  000, 000, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 000, 000,
  000, 000, 000, 000, 000, 000, 000, 000, 000, 000, 000, 000, 000, 000, 000, 000,
  000, 000, 000, 000, 000, 000, 000, 000, 000, 000, 000, 000, 000, 000, 000, 000,
  000, 000, 000, 000, 000, 000, 000, 000, 000, 000, 000, 000, 000, 000, 000, 000
};

static unsigned char imatrix_sortup_alpha[IMAT_FEEDBACK_SIZE * IMAT_FEEDBACK_SIZE] =
{
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 32, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 32, 32, 0,
  0, 0, 32, 239, 60, 0, 0, 0, 0, 0, 0, 0, 60, 239, 32, 0,
  0, 0, 0, 84, 239, 60, 0, 0, 0, 0, 0, 60, 239, 84, 0, 0,
  0, 0, 0, 0, 84, 239, 60, 0, 0, 0, 60, 239, 84, 0, 0, 0,
  0, 0, 8, 0, 0, 84, 239, 60, 0, 60, 239, 84, 0, 0, 8, 0,
  0, 0, 32, 32, 0, 0, 84, 239, 106, 239, 84, 0, 0, 32, 32, 0,
  0, 0, 32, 239, 60, 0, 0, 89, 252, 89, 0, 0, 60, 239, 32, 0,
  0, 0, 0, 84, 239, 60, 0, 0, 31, 0, 0, 60, 239, 84, 0, 0,
  0, 0, 0, 0, 84, 239, 60, 0, 0, 0, 60, 239, 84, 0, 0, 0,
  0, 0, 0, 0, 0, 84, 239, 60, 0, 60, 239, 84, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 84, 239, 106, 239, 84, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 89, 252, 89, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 31, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static unsigned char imatrix_sortdown_alpha[IMAT_FEEDBACK_SIZE * IMAT_FEEDBACK_SIZE] =
{
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 31, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 89, 252, 89, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 84, 239, 106, 239, 84, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 84, 239, 60, 0, 60, 239, 84, 0, 0, 0, 0,
  0, 0, 0, 0, 84, 239, 60, 0, 0, 0, 60, 239, 84, 0, 0, 0,
  0, 0, 0, 84, 239, 60, 0, 0, 31, 0, 0, 60, 239, 84, 0, 0,
  0, 0, 32, 239, 60, 0, 0, 89, 252, 89, 0, 0, 60, 239, 32, 0,
  0, 0, 32, 32, 0, 0, 84, 239, 106, 239, 84, 0, 0, 32, 32, 0,
  0, 0, 8, 0, 0, 84, 239, 60, 0, 60, 239, 84, 0, 0, 8, 0,
  0, 0, 0, 0, 84, 239, 60, 0, 0, 0, 60, 239, 84, 0, 0, 0,
  0, 0, 0, 84, 239, 60, 0, 0, 0, 0, 0, 60, 239, 84, 0, 0,
  0, 0, 32, 239, 60, 0, 0, 0, 0, 0, 0, 0, 60, 239, 32, 0,
  0, 0, 32, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 32, 32, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};


/**************************************************************************/
/*  Private functions                                                     */
/**************************************************************************/

static void iMatrixDrawSetCellClipping(Ihandle* ih, int x1, int x2, int y1, int y2)
{
  int old_clip = cdCanvasClip(ih->data->cd_canvas, CD_QUERY);
  if (old_clip == CD_CLIPAREA)
  {
    cdCanvasGetClipArea(ih->data->cd_canvas, &(ih->data->clip_x1), &(ih->data->clip_x2), &(ih->data->clip_y1), &(ih->data->clip_y2));
    y1 = iupMATRIX_INVERTYAXIS(ih, y1);
    y2 = iupMATRIX_INVERTYAXIS(ih, y2);
    if (x1 > x2) { int tmp = x1; x1 = x2; x2 = tmp; }
    if (y1 > y2) { int tmp = y1; y1 = y2; y2 = tmp; }
    if (x1 < ih->data->clip_x1) x1 = ih->data->clip_x1;
    if (x2 > ih->data->clip_x2) x2 = ih->data->clip_x2;
    if (y1 < ih->data->clip_y1) y1 = ih->data->clip_y1;
    if (y2 > ih->data->clip_y2) y2 = ih->data->clip_y2;
    if (x1 > x2) x2 = x1;
    if (y1 > y2) y2 = y1;
    cdCanvasClipArea(ih->data->cd_canvas, x1, x2, y1, y2);
    cdCanvasClip(ih->data->cd_canvas, CD_CLIPAREA);
  }
}

static void iMatrixDrawResetCellClipping(Ihandle* ih)
{
  int old_clip = cdCanvasClip(ih->data->cd_canvas, CD_QUERY);
  if (old_clip == CD_CLIPAREA)
    cdCanvasClipArea(ih->data->cd_canvas, ih->data->clip_x1, ih->data->clip_x2, ih->data->clip_y1, ih->data->clip_y2);
}

static int iMatrixDrawCallDrawCB(Ihandle* ih, int lin, int col, int x1, int x2, int y1, int y2, IFniiiiiiC draw_cb)
{
  int ret;
  cdCanvas* old_cnv;

  iMatrixDrawSetCellClipping(ih, x1, x2, y1, y2);

  old_cnv = cdActiveCanvas();
  if (old_cnv != ih->data->cd_canvas) /* backward compatibility code */
    cdActivate(ih->data->cd_canvas);

  ret = draw_cb(ih, lin, col, x1, x2, iupMATRIX_INVERTYAXIS(ih, y1), iupMATRIX_INVERTYAXIS(ih, y2), ih->data->cd_canvas);

  iMatrixDrawResetCellClipping(ih);

  if (old_cnv && old_cnv != ih->data->cd_canvas) /* backward compatibility code */
  {
    cdActivate(old_cnv);
    cdCanvasActivate(ih->data->cd_canvas);
  }

  if (ret == IUP_DEFAULT)
    return 0;

  return 1;
}

/* Change the CD foreground color, for the selected color to draw a cell with
   its FOREGROUND COLOR. This involves checking if there is a color attribute
   that cell.  If no, uses a color attribute for the line, else if no for the
   column,  else if no  for the entire matrix.   Finally,  if not find any of
   these, use the default color.
   -> lin, col - cell coordinates, in IUP format - i.e., l,l represents the left
   top cell of the matrix; lin and col values = 0 represents the
   title lines and columns.
   -> marked - indicate if a cell is marked. If yes, its color is attenuated.
   */
static unsigned long iMatrixDrawSetFgColor(Ihandle* ih, int lin, int col, int marked, int active)
{
  unsigned char r = 0, g = 0, b = 0;
  iupMatrixGetFgRGB(ih, lin, col, &r, &g, &b, marked, active);
  return cdCanvasForeground(ih->data->cd_canvas, cdEncodeColor(r, g, b));
}

/* Change the CD foreground color, for the selected color to draw a cell with
   its BACKGROUND COLOR. This involves checking if there is a color attribute
   that cell.  If no, uses a color attribute for the line, else if no for the
   column,  else if no  for the entire matrix.   Finally,  if not find any of
   these, use the default color.
   -> lin, col - cell coordinates, in IUP format - i.e., l,l represents the left
   top cell of the matrix; lin and col values = 0 represents the
   title lines and columns.
   -> marked - indicate if a cell is marked. If yes, its color is attenuated.
   */
static unsigned long iMatrixDrawSetBgColor(Ihandle* ih, int lin, int col, int marked, int active)
{
  unsigned char r = 255, g = 255, b = 255;
  iupMatrixGetBgRGB(ih, lin, col, &r, &g, &b, marked, active);
  return cdCanvasForeground(ih->data->cd_canvas, cdEncodeColor(r, g, b));
}

static unsigned long iMatrixDrawSetTypeColor(Ihandle* ih, const char* color, int marked, int active)
{
  unsigned char r = 0, g = 0, b = 0;
  iupMatrixGetTypeRGB(ih, color, &r, &g, &b, marked, active);
  return cdCanvasForeground(ih->data->cd_canvas, cdEncodeColor(r, g, b));
}

static int iMatrixDrawFrameVertLineTitleHighlight(Ihandle* ih, int lin, int col, int x, int y1, int y2, long framecolor)
{
  if (ih->data->flat)
    return 1;

  if (col > 0)
  {
    int transp = iupMatrixGetFrameVertColor(ih, lin, col - 1, &framecolor, 0); /* framecolor is ignored here */
    if (transp)
      return 1;
  }

  cdCanvasForeground(ih->data->cd_canvas, CD_WHITE);
  iupMATRIX_LINE(ih, x, y1, x, y2);

  return 0;
}

static int iMatrixDrawFrameHorizLineTitleHighlight(Ihandle* ih, int lin, int col, int x1, int x2, int y, long framecolor)
{
  if (ih->data->flat)
    return 1;

  if (lin > 0)
  {
    int transp = iupMatrixGetFrameHorizColor(ih, lin - 1, col, &framecolor, 0); /* framecolor is ignored here */
    if (transp)
      return 1;
  }

  cdCanvasForeground(ih->data->cd_canvas, CD_WHITE);
  iupMATRIX_LINE(ih, x1, y, x2, y);

  return 0;
}

static int iMatrixDrawFrameHorizLineCell(Ihandle* ih, int lin, int col, int x1, int x2, int y, long framecolor)
{
  int transp = iupMatrixGetFrameHorizColor(ih, lin, col, &framecolor, 0);
  if (transp)
    return 1;

  cdCanvasForeground(ih->data->cd_canvas, framecolor);
  iupMATRIX_LINE(ih, x1, y, x2, y);   /* horizontal line */
  return 0;
}

static int iMatrixDrawFrameHorizLineTitle(Ihandle* ih, int col, int x1, int x2, int y, long framecolor)
{
  int transp = iupMatrixGetFrameHorizColor(ih, 0, col, &framecolor, 1);
  if (transp)
    return 1;

  cdCanvasForeground(ih->data->cd_canvas, framecolor);
  iupMATRIX_LINE(ih, x1, y, x2, y);   /* horizontal line */
  return 0;
}

static int iMatrixDrawFrameVertLineTitle(Ihandle* ih, int lin, int x, int y1, int y2, long framecolor)
{
  int transp = iupMatrixGetFrameVertColor(ih, lin, 0, &framecolor, 1);
  if (transp)
    return 1;

  cdCanvasForeground(ih->data->cd_canvas, framecolor);
  iupMATRIX_LINE(ih, x, y1, x, y2);    /* vertical line */
  return 0;
}

static int iMatrixDrawFrameVertLineCell(Ihandle* ih, int lin, int col, int x, int y1, int y2, long framecolor)
{
  int transp = iupMatrixGetFrameVertColor(ih, lin, col, &framecolor, 0);
  if (transp)
    return 1;

  cdCanvasForeground(ih->data->cd_canvas, framecolor);
  iupMATRIX_LINE(ih, x, y1, x, y2);    /* vertical line */
  return 0;
}

static void iMatrixDrawFrameRectTitle(Ihandle* ih, int lin, int col, int x1, int x2, int y1, int y2, long framecolor, int framehighlight)
{
  /* avoid drawing over the frame of the next cell */
  x2 -= IMAT_FRAME_W / 2;
  y2 -= IMAT_FRAME_H / 2;


  /********************* VERTICAL *************************/


  /* right vertical line */
  iMatrixDrawFrameVertLineCell(ih, lin, col, x2, y1, y2, framecolor);
  if (col == 0)
  {
    /* left vertical line */
    iMatrixDrawFrameVertLineTitle(ih, lin, x1, y1, y2, framecolor);
    x1++;
  }
  else if (col == 1 && ih->data->columns.dt[0].size == 0)
  {
    /* If does not have line titles then draw the left vertical line */
    iMatrixDrawFrameVertLineCell(ih, lin, col - 1, x1, y1, y2, framecolor);
    x1++;
  }

  /* Titles have a bright vertical line near the frame, at left */
  if (framehighlight)
    iMatrixDrawFrameVertLineTitleHighlight(ih, lin, col, x1, y1 + 1, y2 - 1, framecolor);


  /********************* HORIZONTAL *************************/


  /* bottom horizontal line */
  iMatrixDrawFrameHorizLineCell(ih, lin, col, x1, x2, y2, framecolor);
  if (lin == 0)
  {
    /* top horizontal line */
    iMatrixDrawFrameHorizLineTitle(ih, col, x1, x2, y1, framecolor);
    y1++;
  }
  else if (lin == 1 && ih->data->lines.dt[0].size == 0)
  {
    /* If does not have column titles then draw the top horizontal line */
    iMatrixDrawFrameHorizLineCell(ih, lin - 1, col, x1, x2 - 1, y1, framecolor);
    y1++;
  }

  /* Titles have a bright horizontal line near the frame, at top */
  if (framehighlight)
    iMatrixDrawFrameHorizLineTitleHighlight(ih, lin, col, x1, x2 - 1, y1, framecolor);
}

static void iMatrixDrawFrameRectCell(Ihandle* ih, int lin, int col, int x1, int x2, int y1, int y2, long framecolor)
{
  int transp;

  if (col == 1 && ih->data->columns.dt[0].size == 0)
  {
    /* If does not have line titles then draw the >> left line << of the cell frame */
    iMatrixDrawFrameVertLineCell(ih, lin, col - 1, x1, y1, y2 - 1, framecolor);
  }

  if (lin == 1 && ih->data->lines.dt[0].size == 0)
  {
    /* If does not have column titles then draw the >> top line << of the cell frame */
    iMatrixDrawFrameHorizLineCell(ih, lin - 1, col, x1, x2 - 1, y1, framecolor);
  }

  /* bottom line */
  transp = iMatrixDrawFrameHorizLineCell(ih, lin, col, x1, x2 - 1, y2 - 1, framecolor);

  /* right line */
  iMatrixDrawFrameVertLineCell(ih, lin, col, x2 - 1, y1, transp ? y2 - 1 : y2 - 2, framecolor);
}

static void iMatrixDrawFeedbackImage(Ihandle* ih, int x1, int x2, int y1, int y2, int lin, int col, int active, int marked, const char*name, unsigned char* alpha)
{
  int x, y;
  Ihandle* image = iupImageGetHandle(name);
  if (image)
  {
    long bgcolor;
    int image_width = IupGetInt(image, "WIDTH");
    int image_height = IupGetInt(image, "HEIGHT");
    unsigned char r = 255, g = 255, b = 255;
    iupMatrixGetBgRGB(ih, lin, col, &r, &g, &b, marked, active);
    bgcolor = cdEncodeColor(r, g, b);

    y = (y2 + y1 + image_height) / 2;
    x = (x2 + x1 - image_width) / 2;

    cdIupDrawImage(ih->data->cd_canvas, image, x, iupMATRIX_INVERTYAXIS(ih, y), 0, 0, !active, bgcolor);
  }
  else
  {
    static unsigned char red[IMAT_FEEDBACK_SIZE * IMAT_FEEDBACK_SIZE];
    static unsigned char green[IMAT_FEEDBACK_SIZE * IMAT_FEEDBACK_SIZE];
    static unsigned char blue[IMAT_FEEDBACK_SIZE * IMAT_FEEDBACK_SIZE];
    static unsigned char last_r = 0, last_g = 0, last_b = 0;
    static int init = 1;

    unsigned char r = 0, g = 0, b = 0;
    iupMatrixGetFgRGB(ih, lin, col, &r, &g, &b, marked, active);

    if (init || last_r != r || last_g != g || last_b != b)
    {
      int count = IMAT_FEEDBACK_SIZE * IMAT_FEEDBACK_SIZE;

      memset(red, r, count);
      memset(green, g, count);
      memset(blue, b, count);

      last_r = r;
      last_g = g;
      last_b = b;
      init = 0;
    }

    y = (y2 + y1 + IMAT_FEEDBACK_SIZE) / 2;
    x = (x2 + x1 - IMAT_FEEDBACK_SIZE) / 2;

    cdCanvasPutImageRectRGBA(ih->data->cd_canvas, IMAT_FEEDBACK_SIZE, IMAT_FEEDBACK_SIZE, red, green, blue, alpha, x, iupMATRIX_INVERTYAXIS(ih, y), IMAT_FEEDBACK_SIZE, IMAT_FEEDBACK_SIZE, 0, 0, 0, 0);
  }
}

static int iMatrixDrawSortSign(Ihandle* ih, int x2, int y1, int y2, int col, int active)
{
  int x1;

  char* sort = iupAttribGetId(ih, "SORTSIGN", col);
  if (!sort || iupStrEqualNoCase(sort, "NO"))
    return 0;

  /* feedback area */
  iupMatrixDrawSetDropFeedbackArea(&x1, &y1, &x2, &y2);

  if (iupStrEqualNoCase(sort, "DOWN"))
    iMatrixDrawFeedbackImage(ih, x1, x2, y1, y2, 0, col, active, 0, iupAttribGet(ih, "SORTIMAGEDOWN"), imatrix_sortdown_alpha);
  else
    iMatrixDrawFeedbackImage(ih, x1, x2, y1, y2, 0, col, active, 0, iupAttribGet(ih, "SORTIMAGEUP"), imatrix_sortup_alpha);

  return 1;
}

static void iMatrixDrawDropdownButton(Ihandle* ih, int x2, int y1, int y2, int lin, int col, int marked, int active)
{
  int x1;

  /* feedback area */
  iupMatrixDrawSetDropFeedbackArea(&x1, &y1, &x2, &y2);

  iMatrixDrawFeedbackImage(ih, x1, x2, y1, y2, lin, col, active, marked, iupAttribGet(ih, "DROPIMAGE"), imatrix_dropdown_alpha);
}

static void iMatrixDrawToggle(Ihandle* ih, int x1, int x2, int y1, int y2, int lin, int col, int marked, int active, int toggle_centered)
{
  int togglevalue = 0;

  /* toggle area */
  iupMatrixDrawSetToggleFeedbackArea(toggle_centered, &x1, &y1, &x2, &y2);

  if (toggle_centered)
  {
    char* value = iupMatrixGetValueDisplay(ih, lin, col);
    togglevalue = iupStrBoolean(value);
  }
  else
    togglevalue = iupAttribGetIntId2(ih, "TOGGLEVALUE", lin, col);

  /* toggle check */
  if (togglevalue)
    iMatrixDrawFeedbackImage(ih, x1, x2, y1, y2, lin, col, active, marked, iupAttribGet(ih, "TOGGLEIMAGEON"), imatrix_toggleon_alpha);
  else
    iMatrixDrawFeedbackImage(ih, x1, x2, y1, y2, lin, col, active, marked, iupAttribGet(ih, "TOGGLEIMAGEOFF"), imatrix_toggleoff_alpha);
}

static void iMatrixDrawBackground(Ihandle* ih, int x1, int x2, int y1, int y2, int marked, int active, int lin, int col)
{
  /* avoid drawing over the frame of the next cell */
  x2 -= IMAT_FRAME_W / 2;
  y2 -= IMAT_FRAME_H / 2;

  iMatrixDrawSetBgColor(ih, lin, col, marked, active);
  iupMATRIX_BOX(ih, x1, x2, y1, y2);
}

static void iMatrixDrawText(Ihandle* ih, int x1, int x2, int y1, int y2, int col_alignment, int lin_alignment, int marked, int active, int lin, int col, const char* text)
{
  int text_alignment;
  int charheight, x, y, hidden_text_marks = 0;

  iupdrvFontGetCharSize(ih, NULL, &charheight);

  if (lin == 0 || ih->data->hidden_text_marks)
  {
    int text_w;
    iupdrvFontGetMultiLineStringSize(ih, text, &text_w, NULL);
    if (text_w > x2 - x1 + 1 - IMAT_PADDING_W - IMAT_FRAME_W)
    {
      if (lin == 0)
        col_alignment = IMAT_ALIGN_START;

      if (ih->data->hidden_text_marks)
        hidden_text_marks = 1;
    }
  }

  /* Set the color used to draw the text */
  iMatrixDrawSetFgColor(ih, lin, col, marked, active);

  /* Set the clip area to the cell region informed, the text maybe greater than the cell */
  if (hidden_text_marks)
  {
    int crop = iupdrvFontGetStringWidth(ih, "...") + 2;
    iMatrixDrawSetCellClipping(ih, x1, x2 - crop, y1, y2);
  }
  else
  {
    if (lin == 3 && col == 2)
      lin = lin;
    iMatrixDrawSetCellClipping(ih, x1, x2, y1, y2);
  }

  IupCdSetFont(ih, ih->data->cd_canvas, iupMatrixGetFont(ih, lin, col));

  /* Create an space between text and cell frame */
  x1 += IMAT_PADDING_W / 2;       x2 -= IMAT_PADDING_W / 2;
  y1 += IMAT_PADDING_H / 2;       y2 -= IMAT_PADDING_H / 2;

  iupMatrixGetCellAlign(ih, lin, col, &col_alignment, &lin_alignment);

  if (lin_alignment == IMAT_ALIGN_CENTER)
  {
    y = iupROUND((y1 + y2) / 2.0);

    if (col_alignment == IMAT_ALIGN_CENTER)
    {
      x = iupROUND((x1 + x2) / 2.0);
      text_alignment = CD_CENTER;
    }
    else if (col_alignment == IMAT_ALIGN_START)
    {
      x = x1;
      text_alignment = CD_WEST;
    }
    else  /* RIGHT */
    {
      x = x2;
      text_alignment = CD_EAST;
    }
  }
  else if (lin_alignment == IMAT_ALIGN_START)
  {
    y = y1;

    if (col_alignment == IMAT_ALIGN_CENTER)
    {
      x = iupROUND((x1 + x2) / 2.0);
      text_alignment = CD_NORTH;
    }
    else if (col_alignment == IMAT_ALIGN_START)
    {
      x = x1;
      text_alignment = CD_NORTH_WEST;
    }
    else  /* RIGHT */
    {
      x = x2;
      text_alignment = CD_NORTH_EAST;
    }
  }
  else /* lin_alignment == IMAT_ALIGN_END */
  {
    y = y2;

    if (col_alignment == IMAT_ALIGN_CENTER)
    {
      x = iupROUND((x1 + x2) / 2.0);
      text_alignment = CD_SOUTH;
    }
    else if (col_alignment == IMAT_ALIGN_START)
    {
      x = x1;
      text_alignment = CD_SOUTH_WEST;
    }
    else  /* RIGHT */
    {
      x = x2;
      text_alignment = CD_SOUTH_EAST;
    }
  }

  cdCanvasTextAlignment(ih->data->cd_canvas, text_alignment);
  iupMATRIX_TEXT(ih, x, y, text);

  iMatrixDrawResetCellClipping(ih);

  if (hidden_text_marks)
  {
    cdCanvasTextAlignment(ih->data->cd_canvas, CD_EAST);
    y = (int)((y1 + y2) / 2.0 - 0.5);
    x = x2 + IMAT_PADDING_W / 2;
    iupMATRIX_TEXT(ih, x, y, "...");
  }
}

static void iMatrixDrawColor(Ihandle* ih, int x1, int x2, int y1, int y2, int marked, int active, const char* color, long framecolor)
{
  x1 += IMAT_PADDING_W / 2 + IMAT_FRAME_H / 2;
  x2 -= IMAT_PADDING_W / 2 + IMAT_FRAME_W / 2;
  y1 += IMAT_PADDING_H / 2 + IMAT_FRAME_H / 2;
  y2 -= IMAT_PADDING_H / 2 + IMAT_FRAME_H / 2;

  if (!iupAttribGetBoolean(ih, "TYPECOLORINACTIVE"))
    active = 1; /* draw as active */

  /* Fill the box with the color */
  iMatrixDrawSetTypeColor(ih, color, marked, active);
  iupMATRIX_BOX(ih, x1, x2, y1, y2);

  /* Draw the frame */
  cdCanvasForeground(ih->data->cd_canvas, framecolor);
  iupMATRIX_RECT(ih, x1, x2, y1, y2);
}

static void iMatrixDrawFill(Ihandle* ih, int x1, int x2, int y1, int y2, int marked, int active, int lin, int col, const char* value, long framecolor)
{
  int empty, fill = 0;

  iupStrToInt(value, &fill);
  if (fill < 0) fill = 0;
  if (fill > 100) fill = 100;

  /* Create an space between text and cell frame */
  x1 += IMAT_PADDING_W / 2;       x2 -= IMAT_PADDING_W / 2;
  y1 += IMAT_PADDING_H / 2;       y2 -= IMAT_PADDING_H / 2;

  empty = ((x2 - x1)*(100 - fill)) / 100;

  /* Fill the box with the color */
  iMatrixDrawSetFgColor(ih, lin, col, marked, active);
  iupMATRIX_BOX(ih, x1, x2 - empty, y1, y2);

  if (ih->data->show_fill_value)
  {
    int y = (int)((y1 + y2) / 2.0 - 0.5);
    int empty1 = ((x2 - x1)*fill) / 100;
    char text[50];
    sprintf(text, "%d%%", fill);
    IupCdSetFont(ih, ih->data->cd_canvas, iupMatrixGetFont(ih, lin, col));
    cdCanvasTextAlignment(ih->data->cd_canvas, CD_CENTER);

    iMatrixDrawSetCellClipping(ih, x1 + empty1, x2, y1, y2);
    iupMATRIX_TEXT(ih, (x1 + x2) / 2, y, text);
    iMatrixDrawResetCellClipping(ih);

    iMatrixDrawSetBgColor(ih, lin, col, marked, active);
    iMatrixDrawSetCellClipping(ih, x1, x2 - empty, y1, y2);
    iupMATRIX_TEXT(ih, (x1 + x2) / 2, y, text);
    iMatrixDrawResetCellClipping(ih);
  }


  /* Draw the frame */
  cdCanvasForeground(ih->data->cd_canvas, framecolor);
  iupMATRIX_RECT(ih, x1, x2, y1, y2);
}

static void iMatrixDrawImage(Ihandle* ih, int x1, int x2, int y1, int y2, int col_alignment, int lin_alignment, int marked, int active, int lin, int col, const char* name)
{
  long bgcolor;
  int x, y;
  Ihandle* image;

  iMatrixDrawSetCellClipping(ih, x1, x2, y1, y2);

  /* Create an space between image and cell frame */
  x1 += IMAT_PADDING_W / 2;       x2 -= IMAT_PADDING_W / 2;
  y1 += IMAT_PADDING_H / 2;       y2 -= IMAT_PADDING_H / 2;

  image = iupImageGetHandle(name);
  if (image)
  {
    int image_width = IupGetInt(image, "WIDTH");
    int image_height = IupGetInt(image, "HEIGHT");
    unsigned char r = 255, g = 255, b = 255;
    iupMatrixGetBgRGB(ih, lin, col, &r, &g, &b, marked, active);
    bgcolor = cdEncodeColor(r, g, b);

    if (lin_alignment == IMAT_ALIGN_CENTER)
      y = (y2 + y1 + image_height) / 2;
    else if (lin_alignment == IMAT_ALIGN_START)
      y = y1;
    else  /* BOTTOM */
      y = y2 + image_height;

    if (col_alignment == IMAT_ALIGN_CENTER)
      x = (x2 + x1 - image_width) / 2;
    else if (col_alignment == IMAT_ALIGN_START)
      x = x1;
    else  /* RIGHT */
      x = x2 - image_width;

    cdIupDrawImage(ih->data->cd_canvas, image, x, iupMATRIX_INVERTYAXIS(ih, y), 0, 0, !active, bgcolor);
  }

  iMatrixDrawResetCellClipping(ih);
}

/* Put the cell contents in the screen, using the specified color and Alignment.
   -> y1, y2 : vertical limits of the cell
   -> x1, x2 : horizontal limits of the complete cell
   -> col_alignment : Alignment type (horizontal) assigned to the text. The options are:
   [IMAT_ALIGN_CENTER,IMAT_ALIGN_START,IMAT_ALIGN_END]
   -> marked : mark state
   -> lin, col - cell coordinates */
static void iMatrixDrawCellValue(Ihandle* ih, int x1, int x2, int y1, int y2, int col_alignment, int lin_alignment, int marked, int active, int lin, int col, IFniiiiiiC draw_cb, long framecolor)
{
  char *value;

  /* avoid drawing over the frame of the next cell */
  x2 -= IMAT_FRAME_W / 2;
  y2 -= IMAT_FRAME_H / 2;

  /* avoid drawing over the frame of the cell */
  x2 -= IMAT_FRAME_W / 2;
  y2 -= IMAT_FRAME_H / 2;

  if (lin == 0 || col == 0)
  {
    /* avoid drawing over the frame of the cell */
    x1 += IMAT_FRAME_W / 2;
    y1 += IMAT_FRAME_H / 2;

    if (col == 0) x1 += IMAT_FRAME_W / 2;
    if (lin == 0) y1 += IMAT_FRAME_H / 2;
  }
  else if ((col == 1 && ih->data->columns.dt[0].size == 0) || (lin == 1 && ih->data->lines.dt[0].size == 0))
  {
    /* avoid drawing over the frame of the cell */
    x1 += IMAT_FRAME_W / 2;
    y1 += IMAT_FRAME_H / 2;
  }

  if (draw_cb && !iMatrixDrawCallDrawCB(ih, lin, col, x1, x2, y1, y2, draw_cb))
    return;

  value = iupMatrixGetValueDisplay(ih, lin, col);

  /* Put the text */
  if (value && *value)
  {
    int type = iupMatrixGetType(ih, lin, col);
    if (type == IMAT_TYPE_TEXT)
      iMatrixDrawText(ih, x1, x2, y1, y2, col_alignment, lin_alignment, marked, active, lin, col, value);
    else if (type == IMAT_TYPE_COLOR)
      iMatrixDrawColor(ih, x1, x2, y1, y2, marked, active, value, framecolor);
    else if (type == IMAT_TYPE_FILL)
      iMatrixDrawFill(ih, x1, x2, y1, y2, marked, active, lin, col, value, framecolor);
    else if (type == IMAT_TYPE_IMAGE)
      iMatrixDrawImage(ih, x1, x2, y1, y2, col_alignment, lin_alignment, marked, active, lin, col, value);
  }
}

static void iMatrixDrawTitleCorner(Ihandle* ih)
{
  if (ih->data->lines.dt[0].size && ih->data->columns.dt[0].size)
  {
    long framecolor = cdIupConvertColor(iupAttribGetStr(ih, "FRAMECOLOR"));
    int active = iupdrvIsActive(ih);
    IFniiiiiiC draw_cb = (IFniiiiiiC)IupGetCallback(ih, "DRAW_CB");
    int col_alignment = iupMatrixGetColAlignmentLin0(ih);
    int lin_alignment = iupMatrixGetLinAlignment(ih, 0);
    int framehighlight = iupAttribGetInt(ih, "FRAMETITLEHIGHLIGHT");

    iMatrixDrawBackground(ih, 0, ih->data->columns.dt[0].size, 0, ih->data->lines.dt[0].size, 0, active, 0, 0);

    iMatrixDrawFrameRectTitle(ih, 0, 0, 0, ih->data->columns.dt[0].size, 0, ih->data->lines.dt[0].size, framecolor, framehighlight);

    iMatrixDrawCellValue(ih, 0, ih->data->columns.dt[0].size, 0, ih->data->lines.dt[0].size, col_alignment, lin_alignment, 0, active, 0, 0, draw_cb, framecolor);
  }
}

static void iMatrixDrawFocus(Ihandle* ih)
{
  int x1, y1, x2, y2, dx, dy;

  if (iupAttribGetBoolean(ih, "HIDEFOCUS"))
    return;

  /* there are no cells that can get the focus */
  if (ih->data->columns.num <= 1 || ih->data->lines.num <= 1)
    return;

  if (!iupMatrixAuxIsCellVisible(ih, ih->data->lines.focus_cell, ih->data->columns.focus_cell))
    return;

  iupMatrixGetVisibleCellDim(ih, ih->data->lines.focus_cell, ih->data->columns.focus_cell, &x1, &y1, &dx, &dy);

  x2 = x1 + dx - 1;
  y2 = y1 + dy - 1;

  if (ih->data->columns.focus_cell == 1 && ih->data->columns.dt[0].size == 0)
    x1++;
  if (ih->data->lines.focus_cell == 1 && ih->data->lines.dt[0].size == 0)
    y1++;

  {
    cdCanvas* cd_canvas_front = (cdCanvas*)IupGetAttribute(ih, "_CD_CANVAS");  /* front buffer canvas */
    IupCdDrawFocusRect(ih, cd_canvas_front, x1, iupMATRIX_INVERTYAXIS(ih, y1), x2, iupMATRIX_INVERTYAXIS(ih, y2));
  }
}


/**************************************************************************/
/* Exported functions                                                     */
/**************************************************************************/

/* Color attenuation factor in a marked cell, 20% darker */
#define IMAT_ATENUATION(_x)    ((unsigned char)(((_x)*8)/10))

void iupMatrixAddMarkedAttenuation(Ihandle* ih, unsigned char *r, unsigned char *g, unsigned char *b)
{
  char* hlcolor = iupAttribGetStr(ih, "HLCOLOR");
  unsigned char hl_r, hl_g, hl_b;
  if (iupStrToRGB(hlcolor, &hl_r, &hl_g, &hl_b))
  {
    unsigned char a = (unsigned char)iupAttribGetInt(ih, "HLCOLORALPHA");
    *r = iupALPHABLEND(*r, hl_r, a);
    *g = iupALPHABLEND(*g, hl_g, a);
    *b = iupALPHABLEND(*b, hl_b, a);
  }

  *r = IMAT_ATENUATION(*r);
  *g = IMAT_ATENUATION(*g);
  *b = IMAT_ATENUATION(*b);
}

void iupMatrixDrawSetDropFeedbackArea(int *x1, int *y1, int *x2, int *y2)
{
  *x2 -= IMAT_PADDING_W / 2 + IMAT_FRAME_W / 2;
  *x1 = *x2 - IMAT_FEEDBACK_SIZE - IMAT_PADDING_W / 2;

  *y1 += IMAT_PADDING_H / 2 + IMAT_FRAME_H / 2;
  *y2 -= IMAT_PADDING_H / 2 + IMAT_FRAME_H / 2;
}

void iupMatrixDrawSetToggleFeedbackArea(int toggle_centered, int *x1, int *y1, int *x2, int *y2)
{
  if (toggle_centered)
  {
    *x1 = (*x2 + *x1) / 2 - IMAT_FEEDBACK_SIZE / 2;
    *x2 = *x1 + IMAT_FEEDBACK_SIZE;
  }
  else
  {
    *x2 -= IMAT_PADDING_W / 2 + IMAT_FRAME_W / 2;
    *x1 = *x2 - IMAT_FEEDBACK_SIZE - IMAT_PADDING_W / 2;
  }

  *y1 += IMAT_PADDING_H / 2 + IMAT_FRAME_H / 2;
  *y2 -= IMAT_PADDING_H / 2 + IMAT_FRAME_H / 2;
}

static int iMatrixAdjustVisibleColToMergedCells(Ihandle *ih, int *col1, int lin1, int lin2)
{
  int lin, adjusted = 0, merged;
  int startCol;
  int new_col1 = *col1;

  /* check merged cells at col1 */
  for (lin = lin1; lin <= lin2; lin++)
  {
    merged = iupMatrixGetMerged(ih, lin, *col1);
    if (merged)
    {
      iupMatrixGetMergedRect(ih, merged, NULL, NULL, &startCol, NULL);

      if (startCol < new_col1)
      {
        new_col1 = startCol;
        adjusted = 1;
      }
    }
  }

  if (adjusted)
    *col1 = new_col1;

  return adjusted;
}

static int iMatrixAdjustVisibleLinToMergedCells(Ihandle *ih, int *lin1, int col1, int col2)
{
  int col, adjusted = 0, merged;
  int startLin;
  int new_lin1 = *lin1;

  /* check merged cells at lin1 */
  for (col = col1; col <= col2; col++)
  {
    merged = iupMatrixGetMerged(ih, *lin1, col);
    if (merged)
    {
      iupMatrixGetMergedRect(ih, merged, &startLin, NULL, NULL, NULL);

      if (startLin < new_lin1)
      {
        new_lin1 = startLin;
        adjusted = 1;
      }
    }
  }

  if (adjusted)
    *lin1 = new_lin1;

  return adjusted;
}

/* Draw the line titles, visible, between lin and lastlin, include it.
   Line titles marked will be draw with the appropriate feedback.
   -> lin1 - First line to have its title drawn
   -> lin2 - Last line to have its title drawn */
void iupMatrixDrawTitleLines(Ihandle* ih, int lin1, int lin2)
{
  int x1, y1, x2, y2, first_lin, adjust_merged_lin = 0;
  int lin, col_alignment, active, framehighlight;
  long framecolor;
  IFniiiiiiC draw_cb;

  /* here col==0 always */

  if (!ih->data->columns.dt[0].size)
    return;

  if (ih->data->merge_info_count)
    adjust_merged_lin = iMatrixAdjustVisibleLinToMergedCells(ih, &lin1, 0, 0);

  if (ih->data->lines.num_noscroll > 1 && lin1 == 1 && lin2 == ih->data->lines.num_noscroll - 1)
  {
    first_lin = 0;
    y1 = 0;
  }
  else
  {
    if (lin1 > ih->data->lines.last ||
        lin2 < ih->data->lines.first)
        return;

    if (!adjust_merged_lin && lin1 < ih->data->lines.first)
      lin1 = ih->data->lines.first;
    if (lin2 > ih->data->lines.last)
      lin2 = ih->data->lines.last;

    first_lin = ih->data->lines.first;
    y1 = 0;
    for (lin = 0; lin < ih->data->lines.num_noscroll; lin++)
      y1 += ih->data->lines.dt[lin].size;
  }

  /* Start the position of the line title */
  x1 = 0;
  x2 = ih->data->columns.dt[0].size;

  iupMATRIX_CLIPAREA(ih, x1, x2, y1, iupMatrixGetHeight(ih) - 1);
  cdCanvasClip(ih->data->cd_canvas, CD_CLIPAREA);

  /* Find the initial position of the first line */
  if (first_lin == ih->data->lines.first)
    y1 -= ih->data->lines.first_offset;
  for (lin = first_lin; lin < lin1; lin++)
    y1 += ih->data->lines.dt[lin].size;
  if (adjust_merged_lin)
  {
    for (lin = first_lin; lin > lin1; lin--)
      y1 -= ih->data->lines.dt[lin].size;
  }

  framecolor = cdIupConvertColor(iupAttribGetStr(ih, "FRAMECOLOR"));
  framehighlight = iupAttribGetInt(ih, "FRAMETITLEHIGHLIGHT");
  active = iupdrvIsActive(ih);
  draw_cb = (IFniiiiiiC)IupGetCallback(ih, "DRAW_CB");

  col_alignment = iupMatrixGetColAlignment(ih, 0);

  /* Draw the titles */
  for (lin = lin1; lin <= lin2; lin++)
  {
    int merged, i;

    /* If it is a hidden line (size = 0), don't draw the title */
    if (ih->data->lines.dt[lin].size == 0)
      continue;

    merged = 0;
    if (ih->data->merge_info_count)
      merged = iupMatrixGetMerged(ih, lin, 0);

    y2 = y1 + ih->data->lines.dt[lin].size;

    if (merged)
    {
      int startLin, endLin, startCol, endCol;
      iupMatrixGetMergedRect(ih, merged, &startLin, &endLin, &startCol, &endCol);

      if (lin == startLin && 0 == startCol)  /* merged start */
      {
        for (i = startLin + 1; i <= endLin; i++)
          y2 += ih->data->lines.dt[i].size;
      }
      else
        continue;  /* ignore internal merged */
    }

    /* If it doesn't have title, the loop just calculate the final position */
    if (ih->data->columns.dt[0].size)
    {
      int marked = iupMatrixLineIsMarked(ih, lin);
      int lin_alignment = iupMatrixGetLinAlignment(ih, lin);

      iMatrixDrawBackground(ih, x1, x2, y1, y2, marked, active, lin, 0);

      iMatrixDrawFrameRectTitle(ih, lin, 0, x1, x2, y1, y2, framecolor, framehighlight);

      iMatrixDrawCellValue(ih, x1, x2, y1, y2, col_alignment, lin_alignment, marked, active, lin, 0, draw_cb, framecolor);
    }

    y1 = y2;
  }

  cdCanvasClip(ih->data->cd_canvas, CD_CLIPOFF);
}

/* Draw the column titles, visible, between col and lastcol, include it.
   Column titles marked will be draw with the appropriate feedback.
   -> col1 - First column to have its title drawn
   -> col2 - Last column to have its title drawn */
void iupMatrixDrawTitleColumns(Ihandle* ih, int col1, int col2)
{
  int x1, y1, x2, y2, first_col, adjust_merged_col = 0;
  int col, active, col_alignment, lin_alignment, framehighlight;
  long framecolor;
  IFniiiiiiC draw_cb;

  /* here lin==0 always */

  if (!ih->data->lines.dt[0].size)
    return;

  if (ih->data->merge_info_count)
    adjust_merged_col = iMatrixAdjustVisibleColToMergedCells(ih, &col1, 0, 0);

  if (ih->data->columns.num_noscroll > 1 && col1 == 1 && col2 == ih->data->columns.num_noscroll - 1)
  {
    first_col = 0;
    x1 = 0;
  }
  else
  {
    if (col1 > ih->data->columns.last ||
        col2 < ih->data->columns.first)
        return;

    if (!adjust_merged_col && col1 < ih->data->columns.first)
      col1 = ih->data->columns.first;
    if (col2 > ih->data->columns.last)
      col2 = ih->data->columns.last;

    first_col = ih->data->columns.first;
    x1 = 0;
    for (col = 0; col < ih->data->columns.num_noscroll; col++)
      x1 += ih->data->columns.dt[col].size;
  }

  /* Start the position of the first column title */
  y1 = 0;
  y2 = ih->data->lines.dt[0].size;

  iupMATRIX_CLIPAREA(ih, x1, iupMatrixGetWidth(ih) - 1, y1, y2);
  cdCanvasClip(ih->data->cd_canvas, CD_CLIPAREA);

  /* Find the initial position of the first column */
  if (first_col == ih->data->columns.first)
    x1 -= ih->data->columns.first_offset;
  for (col = first_col; col < col1; col++)
    x1 += ih->data->columns.dt[col].size;
  if (adjust_merged_col)
  {
    for (col = first_col; col > col1; col--)
      x1 -= ih->data->columns.dt[col].size;
  }

  framecolor = cdIupConvertColor(iupAttribGetStr(ih, "FRAMECOLOR"));
  framehighlight = iupAttribGetInt(ih, "FRAMETITLEHIGHLIGHT");
  active = iupdrvIsActive(ih);
  draw_cb = (IFniiiiiiC)IupGetCallback(ih, "DRAW_CB");
  col_alignment = iupMatrixGetColAlignmentLin0(ih);
  lin_alignment = iupMatrixGetLinAlignment(ih, 0);

  /* Draw the titles */
  for (col = col1; col <= col2; col++)
  {
    int merged, i;

    /* If it is hidden column (size = 0), do not draw the title */
    if (ih->data->columns.dt[col].size == 0)
      continue;

    merged = 0;
    if (ih->data->merge_info_count)
      merged = iupMatrixGetMerged(ih, 0, col);

    x2 = x1 + ih->data->columns.dt[col].size;

    if (merged)
    {
      int startLin, endLin, startCol, endCol;
      iupMatrixGetMergedRect(ih, merged, &startLin, &endLin, &startCol, &endCol);

      if (0 == startLin && col == startCol) /* merged start */
      {
        for (i = startCol + 1; i <= endCol; i++)
          x2 += ih->data->columns.dt[i].size;
      }
      else
        continue;  /* ignore internal merged */
    }

    /* If it doesn't have title, the loop just calculate the final position */
    if (ih->data->lines.dt[0].size)
    {
      int sort = 0;
      int marked = iupMatrixColumnIsMarked(ih, col);

      iMatrixDrawBackground(ih, x1, x2, y1, y2, marked, active, 0, col);

      iMatrixDrawFrameRectTitle(ih, 0, col, x1, x2, y1, y2, framecolor, framehighlight);

      if (iMatrixDrawSortSign(ih, x2, y1, y2, col, active))
        sort = IMAT_PADDING_W / 2 + IMAT_FEEDBACK_SIZE + IMAT_PADDING_W / 2; /* same space is used by the sort sign */

      iMatrixDrawCellValue(ih, x1, x2 - sort, y1, y2, col_alignment, lin_alignment, marked, active, 0, col, draw_cb, framecolor);
    }

    x1 = x2;
  }

  cdCanvasClip(ih->data->cd_canvas, CD_CLIPOFF);
}

/* Redraw a block of cells of the matrix. Handle marked cells, change
   automatically the background color of them.
   - lin1, col1 : cell coordinates that mark the left top corner of the area to be redrawn
   - lin2, col2 : cell coordinates that mark the right bottom corner of the area to be redrawn */
void iupMatrixDrawCells(Ihandle* ih, int lin1, int col1, int lin2, int col2)
{
  int x1, y1, x2, y2, old_x2, old_y1, old_y2, toggle_centered;
  int col_alignment, lin, col, active, first_col, first_lin;
  int i, adjust_merged_col = 0, adjust_merged_lin = 0;
  long framecolor, framehighlight, emptyarea_color = -1;
  IFnii mark_cb;
  IFnii dropcheck_cb;
  IFniiiiiiC draw_cb;

  if (ih->data->merge_info_count)
  {
    adjust_merged_lin = iMatrixAdjustVisibleLinToMergedCells(ih, &lin1, col1, col2);
    adjust_merged_col = iMatrixAdjustVisibleColToMergedCells(ih, &col1, lin1, lin2);
  }

  x2 = iupMatrixGetWidth(ih) - 1;
  y2 = iupMatrixGetHeight(ih) - 1;

  old_x2 = x2;
  old_y1 = 0;
  old_y2 = y2;

  if (ih->data->lines.num <= 1 ||
      ih->data->columns.num <= 1)
      return;

  if (ih->data->columns.num_noscroll > 1 && col1 == 1 && col2 == ih->data->columns.num_noscroll - 1)
  {
    first_col = 0;
    x1 = 0;
  }
  else
  {
    if (col1 > ih->data->columns.last ||
        col2 < ih->data->columns.first)
      return;

    if (!adjust_merged_col && col1 < ih->data->columns.first)
      col1 = ih->data->columns.first;
    if (col2 > ih->data->columns.last)
      col2 = ih->data->columns.last;

    first_col = ih->data->columns.first;
    x1 = 0;
    for (col = 0; col< ih->data->columns.num_noscroll; col++)
      x1 += ih->data->columns.dt[col].size;
  }

  if (ih->data->lines.num_noscroll>1 && lin1 == 1 && lin2 == ih->data->lines.num_noscroll - 1)
  {
    first_lin = 0;
    y1 = 0;
  }
  else
  {
    if (lin1 > ih->data->lines.last ||
        lin2 < ih->data->lines.first)
      return;

    if (!adjust_merged_lin && lin1 < ih->data->lines.first)
      lin1 = ih->data->lines.first;
    if (lin2 > ih->data->lines.last)
      lin2 = ih->data->lines.last;

    first_lin = ih->data->lines.first;
    y1 = 0;
    for (lin = 0; lin < ih->data->lines.num_noscroll; lin++)
      y1 += ih->data->lines.dt[lin].size;
  }

  iupMATRIX_CLIPAREA(ih, x1, x2, y1, y2);
  cdCanvasClip(ih->data->cd_canvas, CD_CLIPOFF);  /* wait for background */

  /* Find the initial position of the first column */
  if (first_col == ih->data->columns.first)
    x1 -= ih->data->columns.first_offset;
  for (col = first_col; col < col1; col++)
    x1 += ih->data->columns.dt[col].size;
  if (adjust_merged_col)
  {
    for (col = first_col; col > col1; col--)
      x1 -= ih->data->columns.dt[col].size;
  }

  /* Find the initial position of the first line */
  if (first_lin == ih->data->lines.first)
    y1 -= ih->data->lines.first_offset;
  for (lin = first_lin; lin < lin1; lin++)
    y1 += ih->data->lines.dt[lin].size;
  if (adjust_merged_lin)
  {
    for (lin = first_lin; lin > lin1; lin--)
      y1 -= ih->data->lines.dt[lin].size;
  }

  /* Find the final position of the last column */
  x2 = x1;
  for (; col <= col2; col++)
    x2 += ih->data->columns.dt[col].size;

  /* Find the final position of the last line */
  y2 = y1;
  for (; lin <= lin2; lin++)
    y2 += ih->data->lines.dt[lin].size;

  if ((col2 == ih->data->columns.num - 1) && (old_x2 > x2))
  {
    emptyarea_color = cdIupConvertColor(ih->data->bgcolor_parent);
    cdCanvasForeground(ih->data->cd_canvas, emptyarea_color);

    /* If it was drawn until the last column and remains space in the right of it,
       then delete this area with the the background color. */
    iupMATRIX_BOX(ih, x2, old_x2, old_y1, old_y2);
  }

  if ((lin2 == ih->data->lines.num - 1) && (old_y2 > y2))
  {
    if (emptyarea_color == -1)
      emptyarea_color = cdIupConvertColor(ih->data->bgcolor_parent);
    cdCanvasForeground(ih->data->cd_canvas, emptyarea_color);

    /* If it was drawn until the last line visible and remains space below it,
       then delete this area with the the background color. */
    iupMATRIX_BOX(ih, 0, old_x2, y2, old_y2);
  }

  /* after the background */
  cdCanvasClip(ih->data->cd_canvas, CD_CLIPAREA);

  /***** Draw the cell values and frame */
  old_y1 = y1;
  framecolor = cdIupConvertColor(iupAttribGetStr(ih, "FRAMECOLOR"));
  framehighlight = iupAttribGetInt(ih, "FRAMETITLEHIGHLIGHT");
  active = iupdrvIsActive(ih);

  mark_cb = (IFnii)IupGetCallback(ih, "MARK_CB");
  dropcheck_cb = (IFnii)IupGetCallback(ih, "DROPCHECK_CB");
  draw_cb = (IFniiiiiiC)IupGetCallback(ih, "DRAW_CB");
  toggle_centered = iupAttribGetBoolean(ih, "TOGGLECENTERED");

  for (col = col1; col <= col2; col++)  /* For all the columns in the region */
  {
    int last_x2, last_y2;

    if (ih->data->columns.dt[col].size == 0)
      continue;

    col_alignment = iupMatrixGetColAlignment(ih, col);

    x2 = x1 + ih->data->columns.dt[col].size;
    last_x2 = x2;

    for (lin = lin1; lin <= lin2; lin++)     /* For all lines in the region */
    {
      int drop = 0;
      int marked = 0;
      int lin_alignment;
      int merged;

      if (ih->data->lines.dt[lin].size == 0)
        continue;

      merged = 0;
      if (ih->data->merge_info_count)
        merged = iupMatrixGetMerged(ih, lin, col);

      y2 = y1 + ih->data->lines.dt[lin].size;
      last_y2 = y2;

      if (merged)
      {
        int startLin, endLin, startCol, endCol;
        iupMatrixGetMergedRect(ih, merged, &startLin, &endLin, &startCol, &endCol);

        if (lin == startLin && col == startCol)  /* merged start */
        {
          for (i = startLin + 1; i <= endLin; i++)
            y2 += ih->data->lines.dt[i].size;
          for (i = startCol + 1; i <= endCol; i++)
            x2 += ih->data->columns.dt[i].size;
        }
        else
        {
          x2 = last_x2;
          y1 = last_y2;
          continue;   /* ignore internal merged */
        }
      }

      lin_alignment = iupMatrixGetLinAlignment(ih, lin);

      /* If the cell is marked, then draw it with attenuation color */
      marked = iupMatrixGetMark(ih, lin, col, mark_cb);

      if (ih->data->noscroll_as_title && lin < ih->data->lines.num_noscroll || col < ih->data->columns.num_noscroll)
      {
        iMatrixDrawBackground(ih, x1, x2, y1, y2, marked, active, lin, 0);
        iMatrixDrawFrameRectTitle(ih, lin, col, x1, x2, y1, y2, framecolor, framehighlight);
      }
      else
      {
        iMatrixDrawBackground(ih, x1, x2, y1, y2, marked, active, lin, col);

        iMatrixDrawFrameRectCell(ih, lin, col, x1, x2, y1, y2, framecolor);

        if (dropcheck_cb)
        {
          int ret = dropcheck_cb(ih, lin, col);
          if (ret == IUP_DEFAULT)
          {
            iMatrixDrawDropdownButton(ih, x2, y1, y2, lin, col, marked, active);

            drop = IMAT_PADDING_W / 2 + IMAT_FEEDBACK_SIZE + IMAT_PADDING_W / 2;
          }
          else if (ret == IUP_CONTINUE)
          {
            iMatrixDrawToggle(ih, x1, x2, y1, y2, lin, col, marked, active, toggle_centered);

            if (toggle_centered)
            {
              y1 = last_y2;
              continue; /* do not draw the cell contents */
            }

            drop = IMAT_PADDING_W / 2 + IMAT_FEEDBACK_SIZE + IMAT_PADDING_W / 2;
          }
        }
      }

      /* draw the cell contents */
      iMatrixDrawCellValue(ih, x1, x2 - drop, y1, y2, col_alignment, lin_alignment, marked, active, lin, col, draw_cb, framecolor);

      x2 = last_x2;
      y1 = last_y2;
    }

    x1 = last_x2;
    y1 = old_y1;  /* must reset also y */
  }

  cdCanvasClip(ih->data->cd_canvas, CD_CLIPOFF);
}

static void iMatrixDrawMatrix(Ihandle* ih)
{
  iupMatrixPrepareDrawData(ih);

  /* fill the background because there will be empty cells */
  if ((ih->data->lines.num == 1) || (ih->data->columns.num == 1))
  {
    cdCanvasBackground(ih->data->cd_canvas, cdIupConvertColor(ih->data->bgcolor_parent));
    cdCanvasClear(ih->data->cd_canvas);
  }

  /* Draw the corner between line and column titles, if necessary */
  iMatrixDrawTitleCorner(ih);

  /* If there are columns, then draw their titles */
  if (ih->data->columns.num_noscroll > 1)
    iupMatrixDrawTitleColumns(ih, 1, ih->data->columns.num_noscroll - 1);
  iupMatrixDrawTitleColumns(ih, ih->data->columns.first, ih->data->columns.last);

  /* If there are lines, then draw their titles */
  if (ih->data->lines.num_noscroll > 1)
    iupMatrixDrawTitleLines(ih, 1, ih->data->lines.num_noscroll - 1);
  iupMatrixDrawTitleLines(ih, ih->data->lines.first, ih->data->lines.last);

  /* If there are ordinary cells, then draw them */
  if (ih->data->columns.num_noscroll > 1 && ih->data->lines.num_noscroll > 1)
    iupMatrixDrawCells(ih, 1, 1,
    ih->data->lines.num_noscroll - 1, ih->data->columns.num_noscroll - 1);
  if (ih->data->columns.num_noscroll > 1)
    iupMatrixDrawCells(ih, ih->data->lines.first, 1,
    ih->data->lines.last, ih->data->columns.num_noscroll - 1);
  if (ih->data->lines.num_noscroll > 1)
    iupMatrixDrawCells(ih, 1, ih->data->columns.first,
    ih->data->lines.num_noscroll - 1, ih->data->columns.last);
  iupMatrixDrawCells(ih, ih->data->lines.first, ih->data->columns.first,
                     ih->data->lines.last, ih->data->columns.last);

  if (iupAttribGetBoolean(ih, "FRAMEBORDER"))
  {
    long framecolor = cdIupConvertColor(iupAttribGetStr(ih, "FRAMECOLOR"));
    cdCanvasForeground(ih->data->cd_canvas, framecolor);

    /* if vertical scrollbar is visible */
    if (!iupAttribGetBoolean(ih, "YHIDDEN"))
    {
      int posy = IupGetInt(ih, "POSY");
      int dy = IupGetInt(ih, "DY");
      int ymax = IupGetInt(ih, "YMAX");
      int height = iupMatrixGetHeight(ih);

      int width = iupMatrixGetWidth(ih);
      if (width > ih->data->columns.total_size)
        width = ih->data->columns.total_size;

      /* if scrollbar at top, top line is not necessary */
      if (posy > 0)
        iupMATRIX_LINE(ih, 0, 0, width - 1, 0);  /* top horizontal line */

      /* if scrollbar at bottom, bottom line is not necessary */
      if (posy < ymax - dy)
        iupMATRIX_LINE(ih, 0, height - 1, width - 1, height - 1);  /* bottom horizontal line */
    }

    /* if horizontal scrollbar is visible */
    if (!iupAttribGetBoolean(ih, "XHIDDEN"))
    {
      int posx = IupGetInt(ih, "POSX");
      int dx = IupGetInt(ih, "DX");
      int xmax = IupGetInt(ih, "XMAX");
      int width = iupMatrixGetWidth(ih);

      int height = iupMatrixGetHeight(ih);
      if (height > ih->data->lines.total_size)
        height = ih->data->lines.total_size;

      /* if scrollbar at left, left line is not necessary */
      if (posx > 0)
        iupMATRIX_LINE(ih, 0, 0, 0, height - 1);  /* left vertical line */

      /* if scrollbar at right, right line is not necessary */
      if (posx < xmax - dx)
        iupMATRIX_LINE(ih, width - 1, 0, width - 1, height - 1);  /* right vertical line */
    }
  }
}

void iupMatrixDraw(Ihandle* ih, int update)
{
  cdCanvasActivate(ih->data->cd_canvas);

  if (ih->data->need_calcsize)
    iupMatrixAuxCalcSizes(ih);

  iMatrixDrawMatrix(ih);

  ih->data->need_redraw = 0;

  if (update)
    iupMatrixDrawUpdate(ih);
}

void iupMatrixDrawUpdate(Ihandle* ih)
{
  cdCanvasFlush(ih->data->cd_canvas);

  if (ih->data->has_focus)
    iMatrixDrawFocus(ih);

  if (!ih->data->edit_hide_onfocus && ih->data->editing)
    IupUpdate(ih->data->datah);
}

int iupMatrixDrawSetRedrawAttrib(Ihandle* ih, const char* value)
{
  int type;

  if (value == NULL)
    type = 0;
  else if (value[0] == 'L' || value[0] == 'l')
    type = IMAT_PROCESS_LIN;
  else if (value[0] == 'C' || value[0] == 'c')
    type = IMAT_PROCESS_COL;
  else
    type = 0;

  if (type)  /* lines or columns, including their titles */
  {
    int min = 0, max = 0;
    value++;

    /* compatibility code */
    if (strchr(value, ':') != NULL)
    {
      if (iupStrToIntInt(value, &min, &max, ':') != 2)
        max = min;
    }
    else
    {
      if (iupStrToIntInt(value, &min, &max, '-') != 2)
        max = min;
    }

    if (min > max)
      return 0;

    cdCanvasActivate(ih->data->cd_canvas);

    iupMatrixPrepareDrawData(ih);

    if (ih->data->need_calcsize)
      iupMatrixAuxCalcSizes(ih);

    /* ignore empty area, draw only cells */

    iMatrixDrawTitleCorner(ih);

    if (type == IMAT_PROCESS_LIN)
    {
      if (min == 0)
      {
        if (ih->data->columns.num_noscroll > 1)
          iupMatrixDrawTitleColumns(ih, 1, ih->data->columns.num_noscroll - 1);
        iupMatrixDrawTitleColumns(ih, ih->data->columns.first, ih->data->columns.last);
      }

      iupMatrixDrawTitleLines(ih, min, max);
      if (ih->data->columns.num_noscroll > 1)
        iupMatrixDrawCells(ih, min, 1, max, ih->data->columns.num_noscroll - 1);
      iupMatrixDrawCells(ih, min, ih->data->columns.first, max, ih->data->columns.last);
    }
    else
    {
      if (min == 0)
      {
        if (ih->data->lines.num_noscroll > 1)
          iupMatrixDrawTitleLines(ih, 1, ih->data->lines.num_noscroll - 1);
        iupMatrixDrawTitleLines(ih, ih->data->lines.first, ih->data->lines.last);
      }

      iupMatrixDrawTitleColumns(ih, min, max);
      if (ih->data->lines.num_noscroll > 1)
        iupMatrixDrawCells(ih, 1, min, ih->data->lines.num_noscroll - 1, max);
      iupMatrixDrawCells(ih, ih->data->lines.first, min, ih->data->lines.last, max);
    }
  }
  else
  {
    cdCanvasActivate(ih->data->cd_canvas);

    /* Force CalcSize */
    iupMatrixAuxCalcSizes(ih);

    iMatrixDrawMatrix(ih);
  }

  ih->data->need_redraw = 0;
  iupMatrixDrawUpdate(ih);
  return 0;
}
