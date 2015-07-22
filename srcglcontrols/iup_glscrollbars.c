/** \file
 * \brief GL Scrollars base functions.
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "iup.h"
#include "iupcbs.h"

#include "iup_object.h"
#include "iup_register.h"
#include "iup_attrib.h"
#include "iup_str.h"
#include "iup_layout.h"

#include "iup_glcontrols.h"
#include "iup_glscrollbars.h"
#include "iup_gldraw.h"
#include "iup_glsubcanvas.h"


enum {
  SB_NONE, 
  SB_INC_X, SB_PAGEINC_X, SB_DRAG_X, SB_PAGEDEC_X, SB_DEC_X,
  SB_INC_Y, SB_PAGEINC_Y, SB_DRAG_Y, SB_PAGEDEC_Y, SB_DEC_Y
};


int iupGLScrollbarsGetSize(Ihandle* ih)
{
  return iupAttribGetInt(ih, "SCROLLBARSIZE");
}

static int iGLScrollbarsIsInsideHandlers(Ihandle* ih, int x, int y)
{
  int has_vert_scroll = 0;
  int has_horiz_scroll = 0;
  int sb_size = iupGLScrollbarsGetSize(ih);
  int width = ih->currentwidth,
    height = ih->currentheight;
  int sb_xmin = iupAttribGetInt(ih, "XMIN");
  int sb_xmax = iupAttribGetInt(ih, "XMAX");
  int sb_ymin = iupAttribGetInt(ih, "YMIN");
  int sb_ymax = iupAttribGetInt(ih, "YMAX");
  int sb_dy = iupAttribGetInt(ih, "DY");
  int sb_dx = iupAttribGetInt(ih, "DX");

  if (sb_xmax - sb_xmin > sb_dx)  /* has horizontal scrollbar */
  {
    has_horiz_scroll = 1;
    height -= sb_size;
  }

  if (sb_ymax - sb_ymin > sb_dy)  /* has vertical scrollbar */
  {
    has_vert_scroll = 1;
    width -= sb_size;
  }

  if (has_vert_scroll)
  {
    if (x >= ih->currentwidth - sb_size && x < ih->currentwidth)
    {
      int sb_posy = iupAttribGetInt(ih, "POSY");
      int ymin = 0;
      int ymax = height - 1;
      int range = ymax - ymin - 2 * sb_size;
      int sb_range = sb_ymax - sb_ymin;
      int dy = (sb_dy * range) / sb_range;
      int posy = ((sb_posy - sb_ymin) * range) / sb_range;
      posy += ymin + sb_size;

      if (y < sb_size)
        return SB_DEC_Y;
      else if (y < posy)
        return SB_PAGEDEC_Y;
      else if (y < posy + dy)
        return SB_DRAG_Y;
      else if (y < height - sb_size)
        return SB_PAGEINC_Y;
      else if (y < height)
        return SB_INC_Y;

      return SB_NONE;
    }
  }

  if (has_horiz_scroll)
  {
    if (y >= ih->currentheight - sb_size && y < ih->currentheight)
    {
      int sb_posx = iupAttribGetInt(ih, "POSX");
      int xmin = 0;
      int xmax = width - 1;
      int sb_range = sb_xmax - sb_xmin;
      int range = xmax - xmin - 2 * sb_size;
      int dx = (sb_dx * range) / sb_range;
      int posx = ((sb_posx - sb_xmin) * range) / sb_range;
      posx += xmin + sb_size;

      if (x < sb_size)
        return SB_DEC_X;
      else if (x < posx)
        return SB_PAGEDEC_X;
      else if (x < posx + dx)
        return SB_DRAG_X;
      else if (x < width - sb_size)
        return SB_PAGEINC_X;
      else if (x < width)
        return SB_INC_X;

      return SB_NONE;
    }
  }

  return SB_NONE;
}

static void iGLScrollbarsDrawVertical(Ihandle* ih, int active, const char* fgcolor, const char* bgcolor, int pressed, 
                                      int highlight, int sb_ymin, int sb_ymax, int sb_dy, int has_horiz_scroll)
{
  int sb_size = iupGLScrollbarsGetSize(ih);
  int height = ih->currentheight;
  int xmin, xmax, ymin, ymax, posy, dy, sb_range, range;
  int sb_posy = iupAttribGetInt(ih, "POSY");
  const char *fgcolor_inc = fgcolor, 
             *fgcolor_dec = fgcolor, 
             *fgcolor_drag = fgcolor;

  if (pressed)
  {
    char* presscolor = iupAttribGetStr(ih, "PRESSCOLOR");
    if (presscolor)
    {
      if (pressed == SB_INC_Y)
        fgcolor_inc = presscolor;
      else if (pressed == SB_DEC_Y)
        fgcolor_dec = presscolor;
      else if (pressed == SB_DRAG_Y)
        fgcolor_drag = presscolor;
    }
  }
  else if (highlight)
  {
    char* hlcolor = iupAttribGetStr(ih, "HIGHCOLOR");
    if (hlcolor)
    {
      if (highlight == SB_INC_Y)
        fgcolor_inc = hlcolor;
      else if (highlight == SB_DEC_Y)
        fgcolor_dec = hlcolor;
      else if (highlight == SB_DRAG_Y)
        fgcolor_drag = hlcolor;
    }
  }

  if (has_horiz_scroll)
    height -= sb_size;

  xmin = ih->currentwidth - sb_size;
  xmax = ih->currentwidth - 1;
  ymin = 0;
  ymax = height - 1;
  range = ymax - ymin - 2 * sb_size;
  sb_range = sb_ymax - sb_ymin;

  dy = (sb_dy * range) / sb_range;
  posy = ((sb_posy - sb_ymin) * range) / sb_range;
  posy += ymin + sb_size;

  /* draw background */
  iupGLDrawBox(ih, xmin, xmax,
                   ymin, ymax, bgcolor, 1);

  /* draw arrows */
  iupGLDrawArrow(ih, xmin + 2, ymin + 2,           sb_size - 5, fgcolor_dec, active, IUPGL_ARROW_TOP);
  iupGLDrawArrow(ih, xmin + 2, ymax - sb_size + 3, sb_size - 5, fgcolor_inc, active, IUPGL_ARROW_BOTTOM);

  /* draw handler */
  iupGLDrawBox(ih, xmin + 2, xmax - 2, posy, posy + dy, fgcolor_drag, active);
}

static void iGLScrollbarsDrawHorizontal(Ihandle* ih, int active, const char* fgcolor, const char* bgcolor, int pressed, 
                                        int highlight, int sb_xmin, int sb_xmax, int sb_dx, int has_vert_scroll)
{
  int sb_size = iupGLScrollbarsGetSize(ih);
  int width = ih->currentwidth;
  int xmin, xmax, ymin, ymax, posx, dx, sb_range, range;
  int sb_posx = iupAttribGetInt(ih, "POSX");
  const char *fgcolor_inc = fgcolor,
    *fgcolor_dec = fgcolor,
    *fgcolor_drag = fgcolor;

  if (pressed)
  {
    char* presscolor = iupAttribGetStr(ih, "PRESSCOLOR");
    if (presscolor)
    {
      if (pressed == SB_INC_X)
        fgcolor_inc = presscolor;
      else if (pressed == SB_DEC_X)
        fgcolor_dec = presscolor;
      else if (pressed == SB_DRAG_X)
        fgcolor_drag = presscolor;
    }
  }
  else if (highlight)
  {
    char* hlcolor = iupAttribGetStr(ih, "HIGHCOLOR");
    if (hlcolor)
    {
      if (highlight == SB_INC_X)
        fgcolor_inc = hlcolor;
      else if (highlight == SB_DEC_X)
        fgcolor_dec = hlcolor;
      else if (highlight == SB_DRAG_X)
        fgcolor_drag = hlcolor;
    }
  }

  if (has_vert_scroll)
    width -= sb_size;

  xmin = 0;
  xmax = width - 1;
  ymin = ih->currentheight - sb_size;
  ymax = ih->currentheight - 1;
  sb_range = sb_xmax - sb_xmin;
  range = xmax - xmin - 2 * sb_size;

  dx = (sb_dx * range) / sb_range;
  posx = ((sb_posx - sb_xmin) * range) / sb_range;
  posx += xmin + sb_size;

  /* draw background */
  iupGLDrawBox(ih, xmin, xmax, ymin, ymax, bgcolor, 1);

  /* draw arrows */
  iupGLDrawArrow(ih, xmin + 2,           ymin + 2, sb_size - 5, fgcolor_dec, active, IUPGL_ARROW_LEFT);
  iupGLDrawArrow(ih, xmax - sb_size + 3, ymin + 2, sb_size - 5, fgcolor_inc, active, IUPGL_ARROW_RIGHT);

  /* draw handler */
  iupGLDrawBox(ih, posx, posx + dx, ymin + 2, ymax - 2, fgcolor_drag, active);
}

void iupGLScrollbarsDraw(Ihandle* ih, int active)
{
  int has_vert_scroll = 0;
  int has_horiz_scroll = 0;
  int sb_xmin = iupAttribGetInt(ih, "XMIN");
  int sb_xmax = iupAttribGetInt(ih, "XMAX");
  int sb_ymin = iupAttribGetInt(ih, "YMIN");
  int sb_ymax = iupAttribGetInt(ih, "YMAX");
  int sb_dx = iupAttribGetInt(ih, "DX");
  int sb_dy = iupAttribGetInt(ih, "DY");
  char* fgcolor = iupAttribGetStr(ih, "FORECOLOR");
  char* bgcolor = iupAttribGetStr(ih, "BACKCOLOR");
  int highlight = iupAttribGetInt(ih, "_IUP_HIGHLIGHT_HANDLER");
  int pressed = iupAttribGetInt(ih, "_IUP_PRESSED_HANDLER");

  if (sb_xmax - sb_xmin > sb_dx)  /* has horizontal scrollbar */
    has_horiz_scroll = 1;

  if (sb_ymax - sb_ymin > sb_dy)  /* has vertical scrollbar */
    has_vert_scroll = 1;

  if (has_vert_scroll)
    iGLScrollbarsDrawVertical(ih, active, fgcolor, bgcolor, pressed, highlight, sb_ymin, sb_ymax, sb_dy, has_horiz_scroll);

  if (has_horiz_scroll)
    iGLScrollbarsDrawHorizontal(ih, active, fgcolor, bgcolor, pressed, highlight, sb_xmin, sb_xmax, sb_dx, has_vert_scroll);
}

void iupGLScrollbarsChildLayoutUpdate(Ihandle *ih)
{
  if (ih->firstchild)
  {
    int sb_posx = iupAttribGetInt(ih, "POSX");
    int sb_posy = iupAttribGetInt(ih, "POSY");

    iupBaseSetPosition(ih->firstchild, ih->x - sb_posx, ih->y - sb_posy);
    iupLayoutUpdate(ih->firstchild);
  }
}

static void iGLScrollbarsPressX(Ihandle* ih, int handler)
{
  int sb_xmax = iupAttribGetInt(ih, "XMAX");
  int sb_xmin = iupAttribGetInt(ih, "XMIN");
  int sb_dx = iupAttribGetInt(ih, "DX");
  int sb_posx = iupAttribGetInt(ih, "POSX");

  if (handler == SB_INC_X)
  {
    int sb_linex = sb_dx / 10;
    char* linex = iupAttribGet(ih, "LINEX");
    if (linex) iupStrToInt(linex, &sb_linex);
    sb_posx += sb_linex;
  }
  else if (handler == SB_PAGEINC_X)
    sb_posx += sb_dx;
  if (handler == SB_DEC_X)
  {
    int sb_linex = sb_dx / 10;
    char* linex = iupAttribGet(ih, "LINEX");
    if (linex) iupStrToInt(linex, &sb_linex);
    sb_posx -= sb_linex;
  }
  else if (handler == SB_PAGEDEC_X)
    sb_posx -= sb_dx;

  if (sb_posx < sb_xmin)
    sb_posx = sb_xmin;
  if (sb_posx > sb_xmax - sb_dx)
    sb_posx = sb_xmax - sb_dx;

  iupAttribSetInt(ih, "POSX", sb_posx);
}

static void iGLScrollbarsPressY(Ihandle* ih, int handler)
{
  int sb_ymax = iupAttribGetInt(ih, "YMAX");
  int sb_ymin = iupAttribGetInt(ih, "YMIN");
  int sb_dy = iupAttribGetInt(ih, "DY");
  int sb_posy = iupAttribGetInt(ih, "POSY");

  if (handler == SB_INC_Y)
  {
    int sb_liney = sb_dy / 10;
    char* liney = iupAttribGet(ih, "LINEY");
    if (liney) iupStrToInt(liney, &sb_liney);
    sb_posy += sb_liney;
  }
  else if (handler == SB_PAGEINC_Y)
    sb_posy += sb_dy;
  if (handler == SB_DEC_Y)
  {
    int sb_liney = sb_dy / 10;
    char* liney = iupAttribGet(ih, "LINEY");
    if (liney) iupStrToInt(liney, &sb_liney);
    sb_posy -= sb_liney;
  }
  else if (handler == SB_PAGEDEC_Y)
    sb_posy -= sb_dy;

  if (sb_posy < sb_ymin)
    sb_posy = sb_ymin;
  if (sb_posy > sb_ymax - sb_dy)
    sb_posy = sb_ymax - sb_dy;

  iupAttribSetInt(ih, "POSY", sb_posy);
}

static int iGLScrollbarsMove(Ihandle* ih, int diff_x, int diff_y, int start_pos, int handler)
{
  int has_vert_scroll = 0;
  int has_horiz_scroll = 0;
  int sb_xmin = iupAttribGetInt(ih, "XMIN");
  int sb_xmax = iupAttribGetInt(ih, "XMAX");
  int sb_ymin = iupAttribGetInt(ih, "YMIN");
  int sb_ymax = iupAttribGetInt(ih, "YMAX");
  int sb_dx = iupAttribGetInt(ih, "DX");
  int sb_dy = iupAttribGetInt(ih, "DY");
  int sb_size = iupGLScrollbarsGetSize(ih);
  int range, sb_range;

  if (sb_xmax - sb_xmin > sb_dx)  /* has horizontal scrollbar */
    has_horiz_scroll = 1;

  if (sb_ymax - sb_ymin > sb_dy)  /* has vertical scrollbar */
    has_vert_scroll = 1;

  if (handler == SB_DRAG_Y)
  {
    int ymin, ymax, posy, sb_posy;
    int height = ih->currentheight;
    if (has_horiz_scroll)
      height -= sb_size;

    if (diff_y == 0)
      return 0;

    ymin = 0;
    ymax = height - 1;
    range = ymax - ymin - 2 * sb_size;
    sb_range = sb_ymax - sb_ymin;

    posy = ((start_pos - sb_ymin) * range) / sb_range;
    posy += ymin + sb_size;

    posy += diff_y;

    posy -= ymin + sb_size;
    sb_posy = (posy * sb_range) / range + sb_ymin;

    if (sb_posy < sb_ymin)
      sb_posy = sb_ymin;
    if (sb_posy > sb_ymax - sb_dy)
      sb_posy = sb_ymax - sb_dy;

    if (sb_posy != start_pos)
    {
      iupAttribSetInt(ih, "POSY", sb_posy);
      return 1;
    }
  }
  else if (handler == SB_DRAG_X)
  {
    int xmin, xmax, posx, sb_posx;
    int width = ih->currentwidth;
    if (has_vert_scroll)
      width -= sb_size;

    if (diff_x == 0)
      return 0;

    xmin = 0;
    xmax = width - 1;
    range = xmax - xmin - 2 * sb_size;
    sb_range = sb_xmax - sb_xmin;

    posx = ((start_pos - sb_xmin) * range) / sb_range;
    posx += xmin + sb_size;

    posx += diff_x;

    posx -= xmin + sb_size;
    sb_posx = (posx * sb_range) / range + sb_xmin;

    if (sb_posx < sb_xmin)
      sb_posx = sb_xmin;
    if (sb_posx > sb_xmax - sb_dx)
      sb_posx = sb_xmax - sb_dx;

    if (sb_posx != start_pos)
    {
      iupAttribSetInt(ih, "POSX", sb_posx);
      return 1;
    }
  }

  return 0;
}

void iupGLScrollbarsCheckPosX(Ihandle *ih)
{
  int xmin = iupAttribGetInt(ih, "XMIN");
  int xmax = iupAttribGetInt(ih, "XMAX");
  int dx = iupAttribGetInt(ih, "DX");
  int posx = iupAttribGetInt(ih, "POSX");

  if (xmax == xmin || dx >= xmax-xmin)
    iupAttribSet(ih, "POSX", "0");
  else if (posx < xmin)
    iupAttribSetInt(ih, "POSX", xmin);
  else if (posx > xmax - dx)
    iupAttribSetInt(ih, "POSX", xmax - dx);
}

void iupGLScrollbarsCheckPosY(Ihandle *ih)
{
  int ymin = iupAttribGetInt(ih, "YMIN");
  int ymax = iupAttribGetInt(ih, "YMAX");
  int dy = iupAttribGetInt(ih, "DY");
  int posy = iupAttribGetInt(ih, "POSY");

  if (ymax == ymin || dy >= ymax - ymin)
    iupAttribSet(ih, "POSX", "0");
  else if (posy < ymin)
    iupAttribSetInt(ih, "POSY", ymin);
  else if (posy > ymax - dy) 
    iupAttribSetInt(ih, "POSY", ymax - dy);
}

static int iGLScrollbarsSetPosXAttrib(Ihandle *ih, const char *value)
{
  int xmin, xmax, dx;
  int posx;

  if (!iupStrToInt(value, &posx))
    return 0;

  xmin = iupAttribGetInt(ih, "XMIN");
  xmax = iupAttribGetInt(ih, "XMAX");
  dx = iupAttribGetInt(ih, "DX");

  if (posx < xmin) posx = xmin;
  if (posx > xmax - dx) posx = xmax - dx;

  iupAttribSetInt(ih, "POSX", posx);

  iupGLScrollbarsChildLayoutUpdate(ih);
  return 0;
}

static int iGLScrollbarsSetPosYAttrib(Ihandle *ih, const char *value)
{
  int ymin, ymax, dy;
  int posy;

  if (!iupStrToInt(value, &posy))
    return 0;

  ymin = iupAttribGetInt(ih, "YMIN");
  ymax = iupAttribGetInt(ih, "YMAY");
  dy = iupAttribGetInt(ih, "DY");

  if (posy < ymin) posy = ymin;
  if (posy > ymax - dy) posy = ymax - dy;

  iupAttribSetInt(ih, "POSY", posy);

  iupGLScrollbarsChildLayoutUpdate(ih);
  return 0;
}

int iupGLScrollbarsButton(Ihandle *ih, int pressed, int x, int y)
{
  if (pressed)
  {
    int handler = iGLScrollbarsIsInsideHandlers(ih, x, y);
    iupAttribSetInt(ih, "_IUP_PRESSED_HANDLER", handler);

    if (handler == SB_DRAG_X || handler == SB_DRAG_Y)
    {
      iupAttribSetInt(ih, "_IUP_START_X", x);
      iupAttribSetInt(ih, "_IUP_START_Y", y);

      if (handler == SB_DRAG_X)
        iupAttribSetStr(ih, "_IUP_START_POS", iupAttribGet(ih, "POSX"));
      else
        iupAttribSetStr(ih, "_IUP_START_POS", iupAttribGet(ih, "POSY"));
    }

    if (handler != SB_NONE)
      return 1;
  }
  else 
  {
    int press_handler = iupAttribGetInt(ih, "_IUP_PRESSED_HANDLER");
    int handler = iGLScrollbarsIsInsideHandlers(ih, x, y);
    if (handler != SB_NONE && handler != SB_DRAG_X && handler != SB_DRAG_Y &&
        handler == press_handler)
    {
      if (handler == SB_INC_X || handler == SB_PAGEINC_X ||
          handler == SB_DEC_X || handler == SB_PAGEDEC_X)
        iGLScrollbarsPressX(ih, handler);

      if (handler == SB_INC_Y || handler == SB_PAGEINC_Y ||
          handler == SB_DEC_Y || handler == SB_PAGEDEC_Y)
        iGLScrollbarsPressY(ih, handler);

      iupGLScrollbarsChildLayoutUpdate(ih);
    }
    iupAttribSet(ih, "_IUP_PRESSED_HANDLER", NULL);

    if (handler != SB_NONE)
      return 1;
  }

  return 0;
}

int iupGLScrollbarsMotion(Ihandle *ih, int x, int y)
{
  int redraw = 0;
  int handler = iGLScrollbarsIsInsideHandlers(ih, x, y);

  /* special highlight processing for scrollbar area */
  int old_handler = iupAttribGetInt(ih, "_IUP_HIGHLIGHT_HANDLER");
  if (old_handler != handler)
  {
    redraw = 1;
    iupAttribSetInt(ih, "_IUP_HIGHLIGHT_HANDLER", handler);
  }

  handler = iupAttribGetInt(ih, "_IUP_PRESSED_HANDLER");
  if (handler == SB_DRAG_X || handler == SB_DRAG_Y)
  {
    int start_x = iupAttribGetInt(ih, "_IUP_START_X");
    int start_y = iupAttribGetInt(ih, "_IUP_START_Y");
    int start_pos = iupAttribGetInt(ih, "_IUP_START_POS");

    if (iGLScrollbarsMove(ih, x - start_x, y - start_y, start_pos, handler))
    {
      iupGLScrollbarsChildLayoutUpdate(ih);
      redraw = 1;
    }
  }

  return redraw;
}

void iupGLScrollbarsEnterWindow(Ihandle* ih, int x, int y)
{
  /* special highlight processing for scrollbar area */
  int handler = iGLScrollbarsIsInsideHandlers(ih, x, y);
  iupAttribSetInt(ih, "_IUP_HIGHLIGHT_HANDLER", handler);
}

void iupGLScrollbarsLeaveWindow(Ihandle* ih)
{
  iupAttribSet(ih, "_IUP_HIGHLIGHT_HANDLER", NULL);
  iupAttribSet(ih, "_IUP_PRESSED_HANDLER", NULL);
}

void iupGLScrollbarsRegisterAttrib(Iclass* ic)
{
  iupClassRegisterAttribute(ic, "POSX", NULL, iGLScrollbarsSetPosXAttrib, "0", NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "POSY", NULL, iGLScrollbarsSetPosYAttrib, "0", NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "XMIN", NULL, NULL, IUPAF_SAMEASSYSTEM, "0", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "XMAX", NULL, NULL, IUPAF_SAMEASSYSTEM, "100", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "YMIN", NULL, NULL, IUPAF_SAMEASSYSTEM, "0", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "YMAX", NULL, NULL, IUPAF_SAMEASSYSTEM, "100", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "LINEX", NULL, NULL, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "LINEY", NULL, NULL, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "DX", NULL, NULL, "10", NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "DY", NULL, NULL, "10", NULL, IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "SCROLLBARSIZE", NULL, NULL, IUPAF_SAMEASSYSTEM, "11", IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "HIGHCOLOR", NULL, NULL, IUPAF_SAMEASSYSTEM, "90 190 255", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "FORECOLOR", NULL, NULL, IUPAF_SAMEASSYSTEM, "110 210 230", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "PRESSCOLOR", NULL, NULL, IUPAF_SAMEASSYSTEM, "50 150 255", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "BACKCOLOR", NULL, NULL, IUPAF_SAMEASSYSTEM, "200 225 245", IUPAF_NO_INHERIT);
}
