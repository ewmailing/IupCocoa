/** \file
 * \brief iuptabs control
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>

#include "iup.h"
#include "iupcbs.h"
#include "iupkey.h"

#include <cd.h>
#include <cdiup.h>
#include <cddbuf.h>

#include "iup_object.h"
#include "iup_attrib.h"
#include "iup_str.h"
#include "iup_drv.h"
#include "iup_drvfont.h"
#include "iup_stdcontrols.h"
#include "iup_register.h"
#include "iup_layout.h"
#include "iup_controls.h"
#include "iup_cdutil.h"


/* Constants */
static const int ITABS_BORDER  =  1;
static const int ITABS_MARGIN  = 10;
static const int ITABS_SPACING =  2;
static const int ITABS_SCROLL_LENGTH  = 51; /* the length of the scroll control in any direction */
static const int ITABS_SCROLL_THICK   = 17;
static const int ITABS_SCROLL_SPACING =  7;
static const int ITABS_BROKEN_TAB     =  8;
static const int ITABS_CURRENT_EXTRA_PIXELS = 1;

/* Enum's */
typedef enum
{
  ITABS_TOP, ITABS_BOTTOM, ITABS_LEFT, ITABS_RIGHT
} ItabsType;

typedef enum
{
  ITABS_FALSE, ITABS_TRUE
} ItabsBool;

typedef enum
{
  ITABS_BROKEN_NONE,
  ITABS_BROKEN_START,
  ITABS_BROKEN_END,
  ITABS_BROKEN_CENTER
} ItabsBrokenType;

typedef enum
{
  ITABS_HORIZONTAL, ITABS_VERTICAL
} ItabsOrientation;

typedef enum
{
  ITABS_BUTTON_NONE, 
  ITABS_BUTTONPRESS_FORWARD, ITABS_BUTTONPRESS_MENU, ITABS_BUTTONPRESS_BACKWARD,   /* can not change order because of "+= 3" */
  ITABS_BUTTONRELEASE_FORWARD, ITABS_BUTTONRELEASE_MENU, ITABS_BUTTONRELEASE_BACKWARD  
} ItabsButtonState;

typedef enum
{
  ITABS_RENDER_PLANE_ALIGN, ITABS_RENDER_PLANE_NORMAL
} ItabsTextRender;

/* Info about how to draw each tab */
typedef struct _tagTabsDrawInfo
{
  Ihandle* ihandle;    /* child handle */
  int text_w,         /* width of the text of this Tab */
      tab_len;        /* len */
} ItabsDrawInfo;

typedef struct _tagTabsLine
{
  /* Visible tabs (start and end interval) */
  int start_tab, end_tab;

  /* Shows if any button is pressed */
  ItabsButtonState button;

  /* Info about how to draw each tab */
  ItabsDrawInfo* tabs_info;

  /* Shows if all the tabs are visible */
  ItabsBool scroll_visible, 
      broken_start_visible, 
        broken_end_visible;

  /* Shows if the current tab is visible, when
     there are other tabs to the left or right  */
  ItabsBool broken_center_visible;

  /* Available space to the broken tabs */
  int broken_tab_space,
      broken_space_start,
      broken_space_end;

  /* Tabs line size */
  int line_thick,
      line_length;
  
  /* Point to thick and length according to render */
  int* line_w;
  int* line_h;

  /* Tabs scroll size */
  int scroll_len,  /* the scroll space in the tabs line length direction */
      scroll_thick,
      scroll_x,
      scroll_y,
      scroll_w,
      scroll_h;

  int text_h; /* height of the text (all texts have the same height) */
} ItabsLine;

/* Control context */
struct _IcontrolData
{
  iupCanvas canvas;  /* from IupCanvas (must reserve it) */

  /* tabs type */
  ItabsType tabs_type;
  ItabsOrientation tabs_orientation;
  ItabsTextRender tabs_render;   /* tabs line render (depends on
                                    tabs type and orientation)        */

  cdCanvas* cdcanvas;        /* CD canvas for drawing                 */
  cdCanvas* cddbuffer;       /* image canvas for double buffering     */
  Ihandle*  zbox;            /* Handle "zbox" is a child of Tabs      */

  int number_of_tabs;        /* number of tabs                        */
  int current_tab;           /* current tab                           */
  ItabsLine tabs_line;       /* data from a queue of tabs             */

  /* data from width and weight */
  int w, h;
  int* max_line_length;     /* pointer to w or h                     */

  /* Used colors */
  long bgcolor;
  long fgcolor;
  long light_shadow;
  long mid_shadow;
  long dark_shadow;

  int has_focus, 
      focus_x1,
      focus_x2,
      focus_y1,
      focus_y2;

  char* font_active;
  char* font_inactive;
};

static void iTabsUpdateRender(Ihandle* ih)
{
  if (ih->data->tabs_orientation == ITABS_VERTICAL)
  {
    if (ih->data->tabs_type == ITABS_LEFT || ih->data->tabs_type == ITABS_RIGHT)
      ih->data->tabs_render = ITABS_RENDER_PLANE_ALIGN;
    else
      ih->data->tabs_render = ITABS_RENDER_PLANE_NORMAL;
  }
  else
  {
    if (ih->data->tabs_type == ITABS_LEFT || ih->data->tabs_type == ITABS_RIGHT)
      ih->data->tabs_render = ITABS_RENDER_PLANE_NORMAL;
    else
      ih->data->tabs_render = ITABS_RENDER_PLANE_ALIGN;
  }

  if (ih->data->tabs_type == ITABS_LEFT || ih->data->tabs_type == ITABS_RIGHT)
  {
    ih->data->tabs_line.line_w = &ih->data->tabs_line.line_thick;
    ih->data->tabs_line.line_h = &ih->data->tabs_line.line_length;
    ih->data->max_line_length  = &ih->data->h;
  }
  else
  {
    ih->data->tabs_line.line_w = &ih->data->tabs_line.line_length;
    ih->data->tabs_line.line_h = &ih->data->tabs_line.line_thick;
    ih->data->max_line_length  = &ih->data->w;
  }

  if (ih->data->tabs_render == ITABS_RENDER_PLANE_NORMAL)
  {
    ih->data->tabs_line.scroll_len   = ITABS_SCROLL_THICK;
    ih->data->tabs_line.scroll_thick = ITABS_SCROLL_LENGTH;
  }
  else
  {
    ih->data->tabs_line.scroll_len   = ITABS_SCROLL_LENGTH;
    ih->data->tabs_line.scroll_thick = ITABS_SCROLL_THICK;
  }

  if (ih->data->tabs_orientation == ITABS_HORIZONTAL)
  {
    ih->data->tabs_line.scroll_w = ITABS_SCROLL_LENGTH;
    ih->data->tabs_line.scroll_h = ITABS_SCROLL_THICK;
  }
  else
  {
    ih->data->tabs_line.scroll_w = ITABS_SCROLL_THICK;
    ih->data->tabs_line.scroll_h = ITABS_SCROLL_LENGTH;
  }
}

static void iTabsSetCDFont(Ihandle* ih, char* tab_font, int bold)
{
  if (tab_font) 
    cdCanvasNativeFont(ih->data->cddbuffer, tab_font);
  else 
    cdCanvasNativeFont(ih->data->cddbuffer, IupGetAttribute(ih, "FONT"));

  if (bold) 
    cdCanvasFont(ih->data->cddbuffer, NULL, CD_BOLD, 0);
}

/* Gets the associated name of the tab */
static Ihandle* iTabsGetTabIhandle(Ihandle* ih, int number)
{
  return ih->data->tabs_line.tabs_info[number].ihandle;
}

/* ========================================================================= */
/* Drawing functions                                                         */
/* ========================================================================= */

/* Fills the draw area */
static void iTabsFillArea(cdCanvas* canvas, int x1, int y1, int x2, int y2,
                                            int x3, int y3, int x4, int y4)
{
  cdCanvasBegin(canvas, CD_FILL);
    cdCanvasVertex(canvas, x1, y1);
    cdCanvasVertex(canvas, x2, y2);
    cdCanvasVertex(canvas, x3, y3);
    cdCanvasVertex(canvas, x4, y4);
  cdCanvasEnd(canvas);
  
  cdCanvasBegin(canvas, CD_CLOSED_LINES);
    cdCanvasVertex(canvas, x1, y1);
    cdCanvasVertex(canvas, x2, y2);
    cdCanvasVertex(canvas, x3, y3);
    cdCanvasVertex(canvas, x4, y4);
  cdCanvasEnd(canvas);
}

/* Draws the broken borders */
static void iTabsDrawBrokenBorder(Ihandle* ih, ItabsType type, int x1, int y1, int x2, int y2)
{
  cdCanvas* canvas = ih->data->cddbuffer;
  double  step_x = (x2 - x1) / 5;
  double  step_y = (y2 - y1) / 5;
  int old_color = cdCanvasForeground(canvas, CD_QUERY);

  switch(type)
  {
    case ITABS_TOP:
      cdCanvasForeground(canvas, ih->data->mid_shadow);
      cdCanvasLine(canvas, (int)(x1 + step_x * 3.), y1, (int)(x1 + 4. * step_x), y2);
      cdCanvasLine(canvas, (int)(x1 + step_x), y1, (int)(x1 + 2. * step_x), y2);

      cdCanvasForeground(canvas, ih->data->dark_shadow);
      cdCanvasLine(canvas, x1, y1, (int)(x1 + step_x), y2);
      cdCanvasLine(canvas, (int)(x1 + step_x), y1 - 1, (int)(x1 + 2. * step_x), y2 - 1);
      cdCanvasLine(canvas, (int)(x1 + step_x * 2.), y1, (int)(x1 + 3. * step_x), y2);
      cdCanvasLine(canvas, (int)(x1 + step_x * 3.), y1 + 1, (int)(x1 + 4. * step_x), y2 + 1);
      cdCanvasLine(canvas, (int)(x1 + step_x * 4.), y1, x2, y2);
      break;

    case ITABS_BOTTOM:
      cdCanvasForeground(canvas, ih->data->mid_shadow);
      cdCanvasLine(canvas, (int)(x1 + step_x * 3.), y1, (int)(x1 + 4. * step_x), y2);
      cdCanvasLine(canvas, (int)(x1 + step_x), y1, (int)(x1 + 2. * step_x), y2);

      cdCanvasForeground(canvas, ih->data->dark_shadow);
      cdCanvasLine(canvas, x1, y1, (int)(x1 + step_x), y2);
      cdCanvasLine(canvas, (int)(x1 + step_x), y1 - 1, (int)(x1 + 2. * step_x), y2 - 1);
      cdCanvasLine(canvas, (int)(x1 + step_x * 2.), y1, (int)(x1 + 3. * step_x), y2);
      cdCanvasLine(canvas, (int)(x1 + step_x * 3.), y1 + 1, (int)(x1 + 4. * step_x), y2 + 1);
      cdCanvasLine (canvas, (int)(x1 + step_x * 4.), y1, x2, y2);
      break;

    case ITABS_LEFT:
      cdCanvasForeground(canvas, ih->data->mid_shadow);
      cdCanvasLine(canvas, x1, (int)(y1 + 3. * step_y), x2, (int)(y1 + step_y * 4.));
      cdCanvasLine(canvas, x1, (int)(y1 + step_y), x2, (int)(y1 + step_y * 2.));

      cdCanvasForeground(canvas, ih->data->dark_shadow);
      cdCanvasLine(canvas, x1, y1, x2, (int)(y1 + step_y));
      cdCanvasLine(canvas, x1 - 1, (int)(y1 + step_y), x2 - 1, (int)(y1 + step_y * 2.));
      cdCanvasLine(canvas, x1, (int)(y1 + step_y * 2.), x2, (int)(y1 + step_y * 3.));
      cdCanvasLine(canvas, x1 + 1, (int)(y1 + 3. * step_y), x2 + 1, (int)(y1 + step_y * 4.));
      cdCanvasLine(canvas, x1, (int)(y1 + 4. * step_y), x2, y2);
      break;

    case ITABS_RIGHT:
      cdCanvasForeground(canvas, ih->data->mid_shadow);
      cdCanvasLine(canvas, x1, (int)(y1 + 3. * step_y), x2, (int)(y1 + step_y * 4.));
      cdCanvasLine(canvas, x1, (int)(y1 + step_y), x2, (int)(y1 + step_y * 2.));

      cdCanvasForeground(canvas, ih->data->dark_shadow);
      cdCanvasLine(canvas, x1, y1, x2, (int)(y1 + step_y));
      cdCanvasLine(canvas, x1 - 1, (int)(y1 + step_y), x2 - 1, (int)(y1 + step_y * 2.));
      cdCanvasLine(canvas, x1, (int)(y1 + step_y * 2.), x2, (int)(y1 + step_y * 3.));
      cdCanvasLine(canvas, x1 + 1, (int)(y1 + 3. * step_y), x2 + 1, (int)(y1 + step_y * 4.));
      cdCanvasLine(canvas, x1, (int)(y1 + 4. * step_y), x2, y2);
      break;
  }
  
  cdCanvasForeground (canvas, old_color);
}

/* Draws the borders */
static void iTabsDrawBorder(Ihandle* ih, ItabsType type, int x1, int y1, int x2, int y2)
{
  cdCanvas* canvas = ih->data->cddbuffer;

  switch(type)
  {
    case ITABS_TOP:
      cdCanvasForeground(canvas, ih->data->light_shadow);
      cdCanvasLine(canvas, x1, y1, x2, y2);
      break;

    case ITABS_LEFT:
      cdCanvasForeground(canvas, ih->data->light_shadow);
      cdCanvasLine(canvas, x1, y1, x2, y2);
      break;

    case ITABS_BOTTOM:
      cdCanvasForeground(canvas, ih->data->dark_shadow);
      cdCanvasLine(canvas, x1, y1, x2, y2);
      cdCanvasForeground(canvas, ih->data->mid_shadow);
      if (x1 > x2)
        cdCanvasLine(canvas, x2+1, y1+1, x1-1, y2+1);
      else
        cdCanvasLine(canvas, x1+1, y1+1, x2-1, y2+1);
      break;

    case ITABS_RIGHT:
      cdCanvasForeground(canvas, ih->data->dark_shadow);
      cdCanvasLine(canvas, x1, y1, x2, y2);
      cdCanvasForeground(canvas, ih->data->mid_shadow);
      if (y1 > y2)
        cdCanvasLine(canvas, x1-1, y2+1, x2-1, y1-1);
      else
        cdCanvasLine(canvas, x1-1, y1+1, x2-1, y2-1);
      break;
  }
}

/* Draws a tab corner */ 
static void iTabsDrawCorner(Ihandle* ih, int color, int x1, int y1, int x2, int y2)
{
  cdCanvas* canvas = ih->data->cddbuffer;
  
  cdCanvasForeground(canvas, ih->data->bgcolor);
  cdCanvasLine(canvas, x1, y1, x2, y1);
  cdCanvasLine(canvas, x1, y1, x1, y2);
  cdCanvasForeground(canvas,color);
  cdCanvasLine(canvas, x1, y2, x2, y1);
}

/* Draws a tab */
static void iTabsDrawTab(Ihandle* ih, int tab_index, int offset)
{
  cdCanvas* canvas = ih->data->cddbuffer;
  int x1, y1, x2, y2, x3, y3, x4, y4, x1a, y1a, x4a, y4a;
  int tab_w, tab_h;
  int x_text = 0, y_text = 0;
  int line_thick, tab_len, full_tab_len;
  ItabsType types[3];
  ItabsBool beforeCurrent, afterCurrent;
  int extraHeight = (tab_index == ih->data->current_tab) ? 0 : 1;
  ItabsBrokenType type;

  /* there is an error in visible tabs that happen when you change the tab configuration dynamically */
  if (tab_index < 0 || tab_index > ih->data->number_of_tabs-1)
    return;

  /* Defines what type of tab that will be drawn and sets the length */
  full_tab_len = ih->data->tabs_line.tabs_info[tab_index].tab_len;

  if (tab_index == (ih->data->tabs_line.end_tab + 1))
  {
    type = ITABS_BROKEN_END;
    tab_len = ih->data->tabs_line.broken_space_end;
  }
  else if (tab_index == (ih->data->tabs_line.start_tab - 1))
  {
    type = ITABS_BROKEN_START;
    tab_len = ih->data->tabs_line.broken_space_start;
  }
  else if (ih->data->tabs_line.broken_center_visible == ITABS_TRUE)
  {
    type = ITABS_BROKEN_CENTER;
    tab_len = ih->data->tabs_line.broken_tab_space;
  }
  else
  {
    type = ITABS_BROKEN_NONE;
    tab_len = full_tab_len;
  }

  if (tab_len > full_tab_len)
    tab_len = full_tab_len;

  /* If the selected tab is not the first,  and this tab is visible,
   * subtracts one pixel from him and advances the tab on one pixel,
   * to prevent the side of tab is confused with the border control. */
  if (ih->data->current_tab != 0 &&
      ((ih->data->tabs_line.start_tab == 0 && tab_index == 0) ||
       (ih->data->tabs_line.start_tab > 0 && tab_index == (ih->data->tabs_line.start_tab - 1))))
  {
    offset += 1;
    tab_len--;
    full_tab_len--;
  }

  line_thick = ih->data->tabs_line.line_thick;

  if (tab_index != ih->data->current_tab)
    line_thick -= ITABS_CURRENT_EXTRA_PIXELS;

  if (ih->data->tabs_type == ITABS_LEFT || ih->data->tabs_type == ITABS_RIGHT)
  {
    tab_w = line_thick;
    tab_h = tab_len;
  }
  else
  {
    tab_w = tab_len;
    tab_h = line_thick;
  }

  /* Calculates the border of tabs, text position and the sides will be drawn */
  switch(ih->data->tabs_type)
  {
    case ITABS_TOP:
      x1 = x1a = offset;
      y1 = ih->data->h - *(ih->data->tabs_line.line_h) - 1;
      y1a = y1 + extraHeight;
      x2 = x1;
      y2 = y1 + tab_h - 1;
      x3 = x2 + tab_w - 1;
      y3 = y2;
      x4 = x3;
      y4 = y1;
      x4a = x3;
      y4a = y1;

      /* defines the order to draw */
      types[0] = ITABS_LEFT;
      types[1] = ITABS_TOP;
      types[2] = ITABS_RIGHT;

      break;

    case ITABS_BOTTOM:
      x1 = x1a = offset;
      y1 = *(ih->data->tabs_line.line_h) - 1;
      y1a = y1 - extraHeight;
      x2 = x1;
      y2 = y1 - tab_h + 1;
      x3 = x2 + tab_w - 1;
      y3 = y2;
      x4 = x3;
      y4 = y1;
      x4a = x3;
      y4a = y1;

      types[0] = ITABS_LEFT;
      types[1] = ITABS_BOTTOM;
      types[2] = ITABS_RIGHT;

      break;

    case ITABS_LEFT:
      x1 = *(ih->data->tabs_line.line_w) - 1;
      x1a = x1 - extraHeight;
      y1 = y1a = ih->data->h - offset - 1;
      x2 = x1 - tab_w + 1;
      y2 = y1;
      x3 = x2;
      y3 = y1 - tab_h + 1;
      x4 = x1;
      y4 = y3;
      x4a = x1a;
      y4a = y3;

      types[0] = ITABS_TOP;
      types[1] = ITABS_LEFT;
      types[2] = ITABS_BOTTOM;

      break;

    case ITABS_RIGHT:
      x1 = ih->data->w - ih->data->tabs_line.line_thick - 1;
      x1a = x1 + extraHeight;
      y1 = y1a = ih->data->h - offset - 1;
      x2 = x1 + tab_w - 1;
      y2 = y1;
      x3 = x2;
      y3 = y1 - tab_h + 1;
      x4 = x1;
      y4 = y3;
      x4a = x1a;
      y4a = y3;

      types[0] = ITABS_TOP;
      types[1] = ITABS_RIGHT;
      types[2] = ITABS_BOTTOM;

      break;

    default: 
      return;
  }

  if (tab_index == (ih->data->current_tab - 1))
  {
    beforeCurrent = ITABS_TRUE;
    afterCurrent  = ITABS_FALSE;
  }
  else if (tab_index == (ih->data->current_tab + 1))
  {
    beforeCurrent = ITABS_FALSE;
    afterCurrent  = ITABS_TRUE;
  }
  else
  {
    beforeCurrent = ITABS_FALSE;
    afterCurrent  = ITABS_FALSE;
  }

  /* Draws the square of tab*/
  cdCanvasForeground(canvas, ih->data->bgcolor);

  /* Fills the square of tab */
  iTabsFillArea(canvas, x1, y1, x2, y2, x3, y3, x4, y4);

  /* Draws the border on the top */
  iTabsDrawBorder(ih, types[1], x2, y2, x3, y3);

  switch(type)
  {
    case ITABS_BROKEN_END:
      switch(ih->data->tabs_type)
      {
        case ITABS_TOP:
          cdCanvasClipArea(canvas, x1, x3 - ITABS_BORDER - 1, y1, y3);
          break;
        case ITABS_BOTTOM:
          cdCanvasClipArea(canvas, x2, x4 - ITABS_BORDER - 1, y2, y4);
          break;
        case ITABS_LEFT:
          cdCanvasClipArea(canvas, x3, x1, y3 + ITABS_BORDER + 1, y1);
          break;
        case ITABS_RIGHT:
          cdCanvasClipArea(canvas, x4, x2, y4 + ITABS_BORDER + 1, y2);
          break;
      }

      if (beforeCurrent == ITABS_TRUE)
        iTabsDrawBrokenBorder(ih, types[0], x1a, y1a, x2, y2);
      else if (afterCurrent == ITABS_TRUE)
        iTabsDrawBrokenBorder(ih, types[2], x3, y3, x4a, y4a);
      else
      {
        iTabsDrawBorder(ih, types[0], x1a, y1a, x2, y2);
        iTabsDrawBrokenBorder(ih, types[2], x3, y3, x4a, y4a);
      }

      if (types[1] == ITABS_TOP)
        iTabsDrawCorner(ih, ih->data->light_shadow, x1 + ITABS_BORDER - 1, y3 + ITABS_BORDER - 1,
                                                    x1 + ITABS_BORDER + 1, y3 + ITABS_BORDER - 3);

      if (types[1] == ITABS_BOTTOM)
        iTabsDrawCorner(ih, ih->data->dark_shadow, x2 + ITABS_BORDER - 1, y2 + ITABS_BORDER - 1,
                                                   x2 + ITABS_BORDER + 1, y2 + ITABS_BORDER + 1);
      break;

    case ITABS_BROKEN_START:
      switch(ih->data->tabs_type)
      {
        case ITABS_TOP:
          cdCanvasClipArea(canvas, x1 + ITABS_BORDER + 1, x3, y1, y3);
          break;
        case ITABS_BOTTOM:
          cdCanvasClipArea(canvas, x2 + ITABS_BORDER + 1, x4, y2, y4);
          break;
        case ITABS_LEFT:
          cdCanvasClipArea(canvas, x3, x1, y3, y1 - ITABS_BORDER - 1);
          break;
        case ITABS_RIGHT:
          cdCanvasClipArea(canvas, x4, x2, y4, y2 - ITABS_BORDER - 1);
          break;
      }

      if (beforeCurrent == ITABS_TRUE)
        iTabsDrawBrokenBorder(ih, types[0], x1a, y1a, x2, y2);
      else if (afterCurrent == ITABS_TRUE)
        iTabsDrawBrokenBorder(ih, types[2], x3, y3, x4a, y4a);
      else
      {
        iTabsDrawBrokenBorder(ih, types[0], x1a, y1a, x2, y2);
        iTabsDrawBorder(ih, types[2], x3, y3, x4a, y4a);
      }

      if (types[1] == ITABS_TOP)
        iTabsDrawCorner(ih, ih->data->dark_shadow, x3 + ITABS_BORDER - 1, y3 + ITABS_BORDER - 1,
                                                   x3 + ITABS_BORDER - 3, y3 + ITABS_BORDER - 3);

      if (types[1] == ITABS_BOTTOM)
        iTabsDrawCorner(ih, ih->data->dark_shadow, x4 + ITABS_BORDER - 1, y2 + ITABS_BORDER - 1,
                                                   x4 + ITABS_BORDER - 3, y2 + ITABS_BORDER + 1);
      break;

    case ITABS_BROKEN_CENTER:
      switch(ih->data->tabs_type)
      {
        case ITABS_TOP:
          cdCanvasClipArea (canvas, x1 + ITABS_BORDER + 1, x3 - ITABS_BORDER - 1,
                                    y1 + ITABS_BORDER + 1, y3 - ITABS_BORDER - 1);
          break;
        case ITABS_BOTTOM:
          cdCanvasClipArea (canvas, x2 + ITABS_BORDER + 1, x4 - ITABS_BORDER - 1,
                                    y2 + ITABS_BORDER + 1, y4 - ITABS_BORDER - 1);
          break;
        case ITABS_LEFT:
          cdCanvasClipArea (canvas, x3 + ITABS_BORDER + 1, x1 - ITABS_BORDER - 1,
                                    y3 + ITABS_BORDER + 1, y1 - ITABS_BORDER - 1);
          break;
        case ITABS_RIGHT:
          cdCanvasClipArea (canvas, x4 + ITABS_BORDER + 1, x2 - ITABS_BORDER - 1,
                                    y4 + ITABS_BORDER + 1, y2 - ITABS_BORDER - 1);
          break;
      }

      iTabsDrawBrokenBorder(ih, types[0], x1, y1, x2, y2);
      iTabsDrawBrokenBorder(ih, types[2], x3, y3, x4, y4);
      break;

    case ITABS_BROKEN_NONE:
      switch(ih->data->tabs_type)
      {
        case ITABS_TOP:
          cdCanvasClipArea (canvas, x1 + ITABS_BORDER + 1, x3 - ITABS_BORDER - 1,
                                    y1 + ITABS_BORDER + 1, y3 - ITABS_BORDER - 1);
          break;
        case ITABS_BOTTOM:
          cdCanvasClipArea (canvas, x2 + ITABS_BORDER + 1, x4 - ITABS_BORDER - 1,
                                    y2 + ITABS_BORDER + 1, y4 - ITABS_BORDER - 1);
          break;
        case ITABS_LEFT:
          cdCanvasClipArea (canvas, x3 + ITABS_BORDER + 1, x1 - ITABS_BORDER - 1,
                                    y3 + ITABS_BORDER + 1, y1 - ITABS_BORDER - 1);
          break;
        case ITABS_RIGHT:
          cdCanvasClipArea (canvas, x4 + ITABS_BORDER + 1, x2 - ITABS_BORDER - 1,
                                    y4 + ITABS_BORDER + 1, y2 - ITABS_BORDER - 1);
          break;
      }

      if (beforeCurrent == ITABS_TRUE)
        iTabsDrawBorder(ih, types[0], x1a, y1a, x2, y2);
      else if (afterCurrent == ITABS_TRUE)
        iTabsDrawBorder(ih, types[2], x3, y3, x4a, y4a);
      else
      {
        iTabsDrawBorder(ih, types[0], x1a, y1a, x2, y2);
        iTabsDrawBorder(ih, types[2], x3, y3, x4a, y4a);
      }

      if (types[1] == ITABS_TOP)
      {
        iTabsDrawCorner(ih, ih->data->light_shadow, x1 + ITABS_BORDER - 1, y3 + ITABS_BORDER - 1,
                                                    x1 + ITABS_BORDER + 1, y3 + ITABS_BORDER - 3);

        iTabsDrawCorner(ih, ih->data->dark_shadow, x3 + ITABS_BORDER - 1, y3 + ITABS_BORDER - 1,
                                                   x3 + ITABS_BORDER - 3, y3 + ITABS_BORDER - 3);
      }

      if (types[1] == ITABS_BOTTOM)
      {
        iTabsDrawCorner(ih, ih->data->dark_shadow, x2 + ITABS_BORDER - 1, y2 + ITABS_BORDER - 1,
                                                   x2 + ITABS_BORDER + 1, y2 + ITABS_BORDER + 1);

        iTabsDrawCorner(ih, ih->data->dark_shadow, x4 + ITABS_BORDER - 1, y2 + ITABS_BORDER - 1,
                                                   x4 + ITABS_BORDER - 3, y2 + ITABS_BORDER + 1);
      }

      if (types[1] == ITABS_LEFT)
      {
        iTabsDrawCorner(ih, ih->data->light_shadow, x3 + ITABS_BORDER - 1, y1 + ITABS_BORDER - 1,
                                                    x3 + ITABS_BORDER + 1, y1 + ITABS_BORDER - 3);

        iTabsDrawCorner(ih, ih->data->dark_shadow, x3 + ITABS_BORDER - 1, y3 + ITABS_BORDER - 1,
                                                   x3 + ITABS_BORDER + 1, y3 + ITABS_BORDER + 1);
      }

      if (types[1] == ITABS_RIGHT)
      {
        iTabsDrawCorner(ih, ih->data->dark_shadow, x2 + ITABS_BORDER - 1, y2 + ITABS_BORDER - 1,
                                                   x2 + ITABS_BORDER - 3, y2 + ITABS_BORDER - 3);

        iTabsDrawCorner(ih, ih->data->dark_shadow, x2 + ITABS_BORDER - 1, y4 + ITABS_BORDER - 1,
                                                   x2 + ITABS_BORDER - 3, y4 + ITABS_BORDER + 1);
      }

      break;
  }

  if (ih->data->tabs_type == ITABS_LEFT || ih->data->tabs_type == ITABS_RIGHT)
    tab_h = full_tab_len;
  else
    tab_w = full_tab_len;

  switch(ih->data->tabs_type)
  {
    case ITABS_TOP:
      x_text = x1 + tab_w / 2;
      y_text = y1 + tab_h / 2;
      break;
    case ITABS_BOTTOM:
      x_text = x2 + tab_w / 2;
      y_text = y2 + tab_h / 2;
      break;
    case ITABS_LEFT:
      x_text = x1 - tab_w / 2;
      y_text = y1 - tab_h / 2;
      break;
    case ITABS_RIGHT:
      x_text = x2 - tab_w / 2;
      y_text = y2 - tab_h / 2;
      break;
  }
  cdCanvasTextAlignment (canvas, CD_CENTER);

  /* Draws the text */
  cdCanvasForeground (canvas, ih->data->fgcolor); 

  if (ih->data->tabs_orientation == ITABS_VERTICAL)
    cdCanvasTextOrientation(canvas, 90);
  else
    cdCanvasTextOrientation(canvas, 0);

  {
    char* tab_font = NULL;
    int bold = 0;

    if (iupdrvIsActive(ih) && IupGetInt(ih->data->tabs_line.tabs_info[tab_index].ihandle, "ACTIVE"))
    {
      if (tab_index == ih->data->current_tab)
      {
        tab_font = ih->data->font_active;
        bold = 1;
      }
    }
    else
    {
      cdCanvasForeground(canvas, CD_DARK_GRAY);
      tab_font = ih->data->font_inactive;
    }

    iTabsSetCDFont(ih, tab_font, bold);
  }

  cdCanvasClip(canvas, CD_CLIPAREA);

  {
    char* text = iupAttribGet(ih->data->tabs_line.tabs_info[tab_index].ihandle, "TABTITLE");
    if (!text) text = "   ";

    if (ih->data->has_focus && (tab_index == ih->data->current_tab))
    {
      int x1, y1, x2, y2;
      cdCanvasGetTextBox(canvas, x_text, y_text, text, &x1, &x2, &y1, &y2);
      if (ih->data->tabs_orientation == ITABS_VERTICAL)
        {y1-=2; y2+=2;}
      else
        {x1-=2; x2+=2;}
      ih->data->focus_x1 = x1;
      ih->data->focus_x2 = x2;
      ih->data->focus_y1 = y1;
      ih->data->focus_y2 = y2;
    }

    cdCanvasText(canvas, x_text, y_text, text);
  }

  cdCanvasClip (canvas, CD_CLIPOFF);
}

/* Draws the lower border of the tab labels */
static void iTabsDrawLabelLowerBorder(Ihandle* ih, int start, int end)
{
  switch(ih->data->tabs_type)
  {
    case ITABS_TOP:
      iTabsDrawBorder(ih, ITABS_TOP, start, ih->data->h - *(ih->data->tabs_line.line_h) - 1 - 1,
                                       end, ih->data->h - *(ih->data->tabs_line.line_h) - 1 - 1);
      break;

    case ITABS_LEFT:
      if (!start)
        iTabsDrawBorder(ih, ITABS_LEFT, *(ih->data->tabs_line.line_w) - 1 + 1, ih->data->h - 1 - start,
                                        *(ih->data->tabs_line.line_w) - 1 + 1, ih->data->h - 1 - end);
      else
        iTabsDrawBorder(ih, ITABS_LEFT, *(ih->data->tabs_line.line_w) - 1 + 1, ih->data->h - 1 - start,
                                        *(ih->data->tabs_line.line_w) - 1 + 1, 0);
      break;

    case ITABS_BOTTOM:
      iTabsDrawBorder(ih, ITABS_BOTTOM, start, *(ih->data->tabs_line.line_h) - 1 + 1,
                                          end, *(ih->data->tabs_line.line_h) - 1 + 1);
      break;

    case ITABS_RIGHT:
      if (!start)
        iTabsDrawBorder(ih, ITABS_RIGHT, ih->data->w - *(ih->data->tabs_line.line_w) - 1 - 1, ih->data->h - 1 - start,
                                         ih->data->w - *(ih->data->tabs_line.line_w) - 1 - 1, ih->data->h - 1 - end);
      else
        iTabsDrawBorder(ih, ITABS_RIGHT, ih->data->w - *(ih->data->tabs_line.line_w) - 1 - 1, ih->data->h - 1 - start,
                                         ih->data->w - *(ih->data->tabs_line.line_w) - 1 - 1, 0);
      break;
  }
}

/* Draws a normal button */
static void iTabsDrawButton(Ihandle* ih, int x, int y, int width, int height)
{
  cdCanvas* canvas = ih->data->cddbuffer;
  int  old_color  = cdCanvasForeground(canvas, ih->data->bgcolor);

  cdCanvasBegin(canvas, CD_FILL);
    cdCanvasVertex(canvas, x, y);
    cdCanvasVertex(canvas, x + width - 1, y);
    cdCanvasVertex(canvas, x + width - 1, y + height - 1);
    cdCanvasVertex(canvas, x, y + height - 1);
  cdCanvasEnd(canvas);

  cdCanvasForeground (canvas, old_color);

  iTabsDrawBorder(ih, ITABS_TOP, x + width - 1, y + height - 1, x, y + height - 1);
  iTabsDrawBorder(ih, ITABS_LEFT, x, y + height - 1, x, y);
  iTabsDrawBorder(ih, ITABS_BOTTOM, x, y, x + width - 1, y);
  iTabsDrawBorder(ih, ITABS_RIGHT, x + width - 1, y, x + width - 1, y + height - 1);
}

/* Draws arrows
*
*  x, y ..........: lower border of the bounding box
*  width, height .: dimensions of the bounding box
*  type ..........: ITABS_LEFT, ITABS_RIGHT, ITABS_TOP, ITABS_BOTTOM
*/
static void iTabsDrawArrow(Ihandle* ih, int x, int y, int width, int height, int type)
{
  cdCanvas* canvas = ih->data->cddbuffer;
  long  old_color;

  if (iupdrvIsActive(ih))
    old_color = cdCanvasForeground(canvas, CD_BLACK);
  else
    old_color = cdCanvasForeground(canvas, CD_DARK_GRAY);

  cdCanvasBegin(canvas, CD_CLOSED_LINES);
  switch(type)
  {
    case ITABS_LEFT:
      cdCanvasVertex(canvas, x + 4, y + height / 2);
      cdCanvasVertex(canvas, x + width - 1 - 4, y + height - 1 - 4);
      cdCanvasVertex(canvas, x + width - 1 - 4, y + 4);
      break;

    case ITABS_RIGHT:
      cdCanvasVertex(canvas, x + width - 1 - 4, y + height / 2);
      cdCanvasVertex(canvas, x + 4, y + height - 1 - 4);
      cdCanvasVertex(canvas, x + 4, y + 4);
      break;

    case ITABS_TOP:
      cdCanvasVertex(canvas, x + 4, y + 4);
      cdCanvasVertex(canvas, x + width / 2, y + height - 1 - 4);
      cdCanvasVertex(canvas, x + width - 1 - 4, y + 4);
      break;

    case ITABS_BOTTOM:
      cdCanvasVertex(canvas, x + 4, y + height - 1 - 4);
      cdCanvasVertex(canvas, x + width / 2, y + 4);
      cdCanvasVertex(canvas, x + width - 1 - 4, y + height - 1 - 4);
      break;
  }
  cdCanvasEnd(canvas);

  cdCanvasBegin(canvas, CD_FILL);
  switch(type)
  {
    case ITABS_LEFT:
      cdCanvasVertex(canvas, x + 4, y + height / 2);
      cdCanvasVertex(canvas, x + width - 1 - 4, y + height - 1 - 4);
      cdCanvasVertex(canvas, x + width - 1 - 4, y + 4);
      break;

    case ITABS_RIGHT:
      cdCanvasVertex(canvas, x + width - 1 - 4, y + height / 2);
      cdCanvasVertex(canvas, x + 4, y + height - 1 - 4);
      cdCanvasVertex(canvas, x + 4, y + 4);
      break;

    case ITABS_TOP:
      cdCanvasVertex(canvas, x + 4, y + 4);
      cdCanvasVertex(canvas, x + width / 2, y + height - 1 - 4);
      cdCanvasVertex(canvas, x + width - 1 - 4, y + 4);
      break;

    case ITABS_BOTTOM:
      cdCanvasVertex(canvas, x + 4, y + height - 1 - 4);
      cdCanvasVertex(canvas, x + width / 2, y + 4);
      cdCanvasVertex(canvas, x + width - 1 - 4, y + height - 1 - 4);
      break;
  }
  cdCanvasEnd(canvas);

  cdCanvasForeground(canvas, old_color);
}

/* Draws dots "..." */
static void iTabsDrawDots(Ihandle* ih, int x, int y, int width, int height)
{
  x -= 1;
  y += height / 3 - 1;
  cdCanvasRect(ih->data->cddbuffer, x + 3, x + 4, y + 3, y + 4);
  cdCanvasRect(ih->data->cddbuffer, x + width / 2, x+ width / 2 + 1 , y + 3, y + 4);
  cdCanvasRect(ih->data->cddbuffer, x + width - 1 - 3, x + width - 1 - 2, y + 3, y + 4);
}

/* Draws the scroll_visible with the dots button "..." */
static void iTabsDrawScroll(Ihandle* ih)
{
  int x = ih->data->tabs_line.scroll_x,
       y = ih->data->tabs_line.scroll_y,
       w = ih->data->tabs_line.scroll_w,
       h = ih->data->tabs_line.scroll_h;
  int scroll_updown;

  if (ih->data->tabs_type == ITABS_TOP ||
      ih->data->tabs_type == ITABS_BOTTOM)
    scroll_updown = 0;
  else
    scroll_updown = 1;

  if (ih->data->tabs_orientation == ITABS_HORIZONTAL)
  {
    int x3 = w / 3;

    iTabsDrawButton(ih, x, y, x3, h);
    iTabsDrawArrow (ih, x, y, x3, h, scroll_updown ? ITABS_TOP : ITABS_LEFT);

    iTabsDrawButton(ih, x + x3, y, x3, h);
    iTabsDrawDots  (ih, x + x3, y, x3, h);

    iTabsDrawButton(ih, x + 2 * x3, y, x3, h );
    iTabsDrawArrow (ih, x + 2 * x3, y, x3, h, scroll_updown ? ITABS_BOTTOM : ITABS_RIGHT);
  }
  else
  {
    int y3 = h / 3;

    iTabsDrawButton(ih, x, y, w, y3);
    iTabsDrawArrow (ih, x, y, w, y3, scroll_updown ? ITABS_BOTTOM : ITABS_RIGHT);

    iTabsDrawButton(ih, x, y + y3, w, y3);
    iTabsDrawDots  (ih, x, y + y3, w, y3);

    iTabsDrawButton(ih, x, y + 2 * y3, w, y3);
    iTabsDrawArrow (ih, x, y + 2 * y3, w, y3, scroll_updown ? ITABS_TOP : ITABS_LEFT);
  }

  iTabsDrawCorner(ih, ih->data->dark_shadow,  x + w - 1, y + h  - 1, x + w - 3, y + h - 3);
  iTabsDrawCorner(ih, ih->data->dark_shadow,  x + w - 1, y, x + w - 3, y + 2);
  iTabsDrawCorner(ih, ih->data->light_shadow, x, y + h - 1, x + 2, y + h - 3);
  iTabsDrawCorner(ih, ih->data->dark_shadow,  x, y, x + 2, y + 2);
}

/* Clear tab background */
static void iTabsDrawBackground(Ihandle* ih)
{
  int x1, y1, x2, y2, x3, y3, x4, y4;
  cdCanvas* canvas = ih->data->cddbuffer;

  cdCanvasBackground(canvas, cdIupConvertColor(iupControlBaseGetParentBgColor(ih)));
  cdCanvasClear(canvas);

  switch(ih->data->tabs_type)
  {
    case ITABS_TOP:
      x1 = 0;
      y1 = ih->data->h - *(ih->data->tabs_line.line_h) - 1 - 1;
      x2 = x1;
      y2 = 0;
      x3 = ih->data->w - 1;
      y3 = y2;
      x4 = x3;
      y4 = y1;
      break;

    case ITABS_BOTTOM:
      x1 = 0;
      y1 = *(ih->data->tabs_line.line_h) - 1 + 1;
      x2 = x1;
      y2 = ih->data->h - 1;
      x3 = ih->data->w - 1;
      y3 = y2;
      x4 = x3;
      y4 = y1;
      break;

    case ITABS_LEFT:
      x1 = *(ih->data->tabs_line.line_w) - 1 + 1;
      y1 = ih->data->h - 1;
      x2 = ih->data->w - 1;
      y2 = y1;
      x3 = x2;
      y3 = 0;
      x4 = x1;
      y4 = y3;
      break;

    case ITABS_RIGHT:
      x1 = ih->data->w - *(ih->data->tabs_line.line_w) - 1 - 1;
      y1 = ih->data->h - 1;
      x2 = 0;
      y2 = y1;
      x3 = x2;
      y3 = 0;
      x4 = x1;
      y4 = y3;
      break;

    default:
      return;
  }

  /* Foreground color */
  cdCanvasForeground(canvas, ih->data->bgcolor);
  
  cdCanvasBegin(canvas, CD_FILL);
    cdCanvasVertex(canvas, x1, y1);
    cdCanvasVertex(canvas, x2, y2);
    cdCanvasVertex(canvas, x3, y3);
    cdCanvasVertex(canvas, x4, y4);
  cdCanvasEnd(canvas);
  
  cdCanvasBegin(canvas, CD_CLOSED_LINES);
    cdCanvasVertex(canvas, x1, y1);
    cdCanvasVertex(canvas, x2, y2);
    cdCanvasVertex(canvas, x3, y3);
    cdCanvasVertex(canvas, x4, y4);
  cdCanvasEnd(canvas);
}

/* Draws tab borders */
static void iTabsDrawTabsBorders(Ihandle* ih)
{
  int x1, y1, x2, y2, x3, y3, x4, y4;
  ItabsType types[3];

  switch(ih->data->tabs_type)
  {
    case ITABS_TOP:
      x1 = 0;
      y1 = ih->data->h - *(ih->data->tabs_line.line_h) - 1 - 1;
      x2 = x1;
      y2 = 0;
      x3 = ih->data->w - 1;
      y3 = y2;
      x4 = x3;
      y4 = y1;

      /* defines the order to draw */
      types[0] = ITABS_LEFT;
      types[1] = ITABS_BOTTOM;
      types[2] = ITABS_RIGHT;
      break;

    case ITABS_BOTTOM:
      x1 = 0;
      y1 = *(ih->data->tabs_line.line_h) - 1 + 1;
      x2 = x1;
      y2 = ih->data->h - 1;
      x3 = ih->data->w - 1;
      y3 = y2;
      x4 = x3;
      y4 = y1;

      /* defines the order to draw */
      types[0] = ITABS_LEFT;
      types[1] = ITABS_TOP;
      types[2] = ITABS_RIGHT;
      break;

    case ITABS_LEFT:
      x1 = *(ih->data->tabs_line.line_w) - 1 + 1;
      y1 = ih->data->h - 1;
      x2 = ih->data->w - 1;
      y2 = y1;
      x3 = x2;
      y3 = 0;
      x4 = x1;
      y4 = y3;

      /* defines the order to draw */
      types[0] = ITABS_TOP;
      types[1] = ITABS_RIGHT;
      types[2] = ITABS_BOTTOM;
      break;

    case ITABS_RIGHT:
      x1 = ih->data->w - *(ih->data->tabs_line.line_w) - 1 - 1;
      y1 = ih->data->h - 1;
      x2 = 0;
      y2 = y1;
      x3 = x2;
      y3 = 0;
      x4 = x1;
      y4 = y3;

      /* defines the order to draw */
      types[0] = ITABS_TOP;
      types[1] = ITABS_LEFT;
      types[2] = ITABS_BOTTOM;
      break;

    default:
      return;
  }

  iTabsDrawBorder(ih, types[0], x1, y1, x2, y2);
  iTabsDrawBorder(ih, types[1], x2, y2, x3, y3);
  iTabsDrawBorder(ih, types[2], x3, y3, x4, y4);
}

/* Draws all the tabs and the scroll_visible, if it exists */
static void iTabsDrawLineOfTabs(Ihandle* ih)
{
  int offset = 0;
  int c = 0;

  /* draws decorations, when necessary */
  if (ih->data->tabs_line.broken_start_visible == ITABS_TRUE)
  {
    iTabsDrawTab(ih, ih->data->tabs_line.start_tab - 1, offset);
    offset += ih->data->tabs_line.broken_space_start;
  }

  if (ih->data->tabs_line.broken_center_visible == ITABS_FALSE)
  {
    for(c = ih->data->tabs_line.start_tab; c <= ih->data->tabs_line.end_tab; c++)
    {
      if (c != ih->data->current_tab)
      {
        iTabsDrawTab(ih, c, offset);
        offset += ih->data->tabs_line.tabs_info[c].tab_len;
      }
      else
      { 
        if (ih->data->current_tab != 0)
          iTabsDrawLabelLowerBorder(ih, 0, offset);

        iTabsDrawTab(ih, c, offset);
        offset += ih->data->tabs_line.tabs_info[c].tab_len;
        iTabsDrawLabelLowerBorder(ih, offset - 1, ih->data->w);
      }
    }
  }
  else
  {
    if (ih->data->current_tab != 0)
      iTabsDrawLabelLowerBorder(ih, 0, offset);

    iTabsDrawTab(ih, ih->data->current_tab, offset);
    offset += ih->data->tabs_line.broken_tab_space;
    iTabsDrawLabelLowerBorder(ih, offset, ih->data->w);
  }

  if (ih->data->tabs_line.broken_end_visible)
  {
    iTabsDrawTab(ih, ih->data->tabs_line.end_tab + 1, offset);
    offset += ih->data->tabs_line.broken_space_end;
  }

  if (ih->data->tabs_line.scroll_visible == ITABS_TRUE)
    iTabsDrawScroll(ih);
}

/* Calls the user callback to change of tab */
static int iTabsCallTabChangeCb(Ihandle* ih, Ihandle* new_tab, Ihandle* old_tab)
{
  IFnnn cb = NULL;
  
  if (!IupGetInt(new_tab, "ACTIVE"))
    return IUP_IGNORE;

  cb = (IFnnn)IupGetCallback(ih, "TABCHANGE_CB");
  if (!cb)
    return IUP_DEFAULT;
  
  return cb(ih, new_tab, old_tab);
}

/* ========================================================================= */
/* Functions to calculate the tab sizes  and the tab visibilities            */
/* ========================================================================= */

/* Calculates the box size that involves the text of the tabs */
static void iTabsCalcTextSize(Ihandle* ih)
{
  char* title;
  int t, text_w, size, tabsize;
  int text_width, text_height;

  iupdrvFontGetCharSize(ih, NULL, &text_height);
  ih->data->tabs_line.text_h = text_height;

  /* Get the global size of tab */
  size = iupAttribGetInt(ih, "TABSIZE");

  for(t = 0; t < ih->data->number_of_tabs; t++)
  {
    Ihandle* tab_child = ih->data->tabs_line.tabs_info[t].ihandle;

    title = iupAttribGet(tab_child, "TABTITLE");
    if (!title) title = "   ";

    /* Get the size of an specific tab */   
    tabsize = iupAttribGetInt(tab_child, "TABSIZE");
    text_width = iupdrvFontGetStringWidth(ih, title);

    /* Uses the specific size if the value is larger than the minimum size */
    if (tabsize > text_width)
      text_w = tabsize;
    else
    {
      /* Else, uses the tab size if the value is larger than the minimum size */
      if (size > text_width)
        text_w = size;
      else /* otherwise, uses the minimum size */
        text_w = text_width;
    }

    ih->data->tabs_line.tabs_info[t].text_w = text_w;
  }
}

static void iTabsCalcTabSize(Ihandle* ih)
{
  int t, tab_len, tab_thick;

  iTabsCalcTextSize(ih);

  ih->data->tabs_line.line_thick  = 0;
  ih->data->tabs_line.line_length = 0;

  for(t = 0; t < ih->data->number_of_tabs; t++)
  {
    /* initialize with the text size */
    if (ih->data->tabs_render == ITABS_RENDER_PLANE_ALIGN)
    {
      tab_len   = ih->data->tabs_line.tabs_info[t].text_w;
      tab_thick = ih->data->tabs_line.text_h;
    }
    else
    {
      tab_len   = ih->data->tabs_line.text_h;
      tab_thick = ih->data->tabs_line.tabs_info[t].text_w;
    }

    /* add the borders */
    if (abs(ih->data->current_tab - t) == 1) /* if neighbor of the current tab */
      tab_len += ITABS_BORDER;
    else if (t == ih->data->current_tab)     /* if tab is the current tab */
    {
      if (ih->data->current_tab == 0 || ih->data->current_tab == (ih->data->number_of_tabs - 1))
        tab_len += 2 * ITABS_BORDER;
      else
        tab_len += 3 * ITABS_BORDER;
    }
    else
      tab_len += 2 * ITABS_BORDER;

    /* add margins */
    if (ih->data->tabs_render == ITABS_RENDER_PLANE_ALIGN)
      tab_len += 2 * ITABS_MARGIN;
    else
      tab_len += 2 * ITABS_SPACING;

    ih->data->tabs_line.tabs_info[t].tab_len = tab_len;

    ih->data->tabs_line.line_length += tab_len;      /* is always the sum on the lengths */
    
    if (tab_thick > ih->data->tabs_line.line_thick)   /* is always the maximum thickness */
      ih->data->tabs_line.line_thick = tab_thick;
  }

  if (ih->data->tabs_render == ITABS_RENDER_PLANE_ALIGN)
    ih->data->tabs_line.line_thick += 2 * ITABS_SPACING + 2 * ITABS_BORDER;
  else
    ih->data->tabs_line.line_thick += 2 * ITABS_MARGIN + 2 * ITABS_BORDER;

  if (ih->data->tabs_line.line_thick < ih->data->tabs_line.scroll_thick) 
    ih->data->tabs_line.line_thick = ih->data->tabs_line.scroll_thick;

  /* extra pixels of the selected tab */
  ih->data->tabs_line.line_thick += ITABS_CURRENT_EXTRA_PIXELS;
}

static void iTabsCalcScrollPos(Ihandle* ih)
{
  int x1=0, y1=0, x2=0, y2=0;

  switch(ih->data->tabs_type)
  {
    case ITABS_TOP:
      ih->data->tabs_line.scroll_x = ih->data->w - ih->data->tabs_line.scroll_w - 2;
      ih->data->tabs_line.scroll_y = ih->data->h - *(ih->data->tabs_line.line_h);
      y1=0; y2=*(ih->data->tabs_line.line_h);
      x1=0; x2=ih->data->w-1;
      break;
    case ITABS_BOTTOM:
      ih->data->tabs_line.scroll_x = ih->data->w - ih->data->tabs_line.scroll_w - 2;
      ih->data->tabs_line.scroll_y = *(ih->data->tabs_line.line_h) - 2 - ih->data->tabs_line.scroll_h + 1;
      y1=ih->data->h - *(ih->data->tabs_line.line_h); y2=ih->data->h-1;
      x1=0; x2=ih->data->w-1;
      break;
    case ITABS_LEFT:
      ih->data->tabs_line.scroll_x = *(ih->data->tabs_line.line_w) - 2 - ih->data->tabs_line.scroll_w + 1; 
      ih->data->tabs_line.scroll_y = 0;
      x1=0; x2=*(ih->data->tabs_line.line_w);
      y1=0; y2=ih->data->h-1;
      break;                               
    case ITABS_RIGHT:
      ih->data->tabs_line.scroll_x = ih->data->w - *(ih->data->tabs_line.line_w);
      ih->data->tabs_line.scroll_y = 0;
      x1=ih->data->w-*(ih->data->tabs_line.line_w); x2=ih->data->w-1;
      y1=0; y2=ih->data->h-1;
      break;
  }

  iupAttribSetStrf(ih, "TIPRECT", "%d %d %d %d", x1, y1, x2, y2);
}

/* Calculates the visibility of tabs */
static void iTabsCalcVisibleTabs(Ihandle* ih)
{
  int line_length = *ih->data->max_line_length;        
  int old_line_length;
  int t, t_aux = 0;

  /* Verifies if all the tabs are visible */
  if (*ih->data->max_line_length > ih->data->tabs_line.line_length)
  {
    /* All the tabs are visible */
    ih->data->tabs_line.scroll_visible = ITABS_FALSE;
    ih->data->tabs_line.broken_start_visible = ITABS_FALSE;
    ih->data->tabs_line.broken_end_visible = ITABS_FALSE;
    ih->data->tabs_line.broken_center_visible = ITABS_FALSE;

    ih->data->tabs_line.start_tab = 0;
    ih->data->tabs_line.end_tab = ih->data->number_of_tabs - 1;

    return;
  }

  /* Subtracts space of scroll */
  line_length -= ih->data->tabs_line.scroll_len + ITABS_SCROLL_SPACING;

  ih->data->tabs_line.scroll_visible = ITABS_TRUE;

  /* Verifies if current tab is smaller than start tab, adjusting if necessary */
  if (ih->data->current_tab < ih->data->tabs_line.start_tab)
    ih->data->tabs_line.start_tab = ih->data->current_tab;

  /* Processes tabs in sequence, until the total length overcome the virtual length */
  t = ih->data->tabs_line.start_tab;
  old_line_length = line_length;

  while(1)
  {
    do
    {
      while (ih->data->tabs_line.start_tab < ih->data->current_tab)
      {
        if (ih->data->tabs_line.tabs_info[ih->data->tabs_line.start_tab].tab_len <= line_length)
            break;
        ih->data->tabs_line.start_tab++;
      }

      for(t = ih->data->tabs_line.start_tab, t_aux = 0;
           t < ih->data->number_of_tabs; t++, t_aux++)
      {
        if (ih->data->tabs_line.tabs_info[t].tab_len > line_length)
        {
          if ((t - 1) < ih->data->current_tab)
          {
            while (t_aux > 0)
            {
              line_length +=
                ih->data->tabs_line.tabs_info[t - t_aux].tab_len;

              if (ih->data->tabs_line.start_tab == 0)
              {
                line_length -= ITABS_BROKEN_TAB;
                ih->data->tabs_line.broken_start_visible = ITABS_TRUE;
              }

              ih->data->tabs_line.start_tab++;

              if (ih->data->tabs_line.tabs_info[t].tab_len <= line_length)
                break;

              t_aux--;
            }

            if (ih->data->tabs_line.tabs_info[t].tab_len > line_length)        
            {
              /* no success */
              ih->data->tabs_line.broken_center_visible = ITABS_TRUE;
              break;                /* break two loops */
            }
          }
          else
          {
            t--;
            break;                 /* break two loops */
          }
        }

        line_length -= ih->data->tabs_line.tabs_info[t].tab_len;
        ih->data->tabs_line.broken_center_visible = ITABS_FALSE;
      }

      if (t == ih->data->number_of_tabs)
        t--;
    }
    while (t < ih->data->current_tab);

    if (ih->data->tabs_line.start_tab > 0 &&
        ih->data->tabs_line.broken_center_visible != ITABS_TRUE)
    {
      if (ih->data->tabs_line.tabs_info[ih->data->tabs_line.start_tab - 1].tab_len <
          (line_length - ITABS_BROKEN_TAB))
      {
        ih->data->tabs_line.start_tab--;
        line_length = old_line_length;
        continue;
      }
    }

    break;
  }

  if (ih->data->tabs_line.start_tab > 0)
  {
    t = ih->data->tabs_line.start_tab;
    line_length = old_line_length;
    line_length -= ITABS_BROKEN_TAB;
    ih->data->tabs_line.broken_start_visible = ITABS_TRUE;

    while(1)
    {
      do
      {
        while (ih->data->tabs_line.start_tab < ih->data->current_tab)
        {
          if (ih->data->tabs_line.tabs_info[ih->data->tabs_line.start_tab].tab_len <= line_length)
              break;
          ih->data->tabs_line.start_tab++;
        }

        for(t = ih->data->tabs_line.start_tab, t_aux = 0;
             t < ih->data->number_of_tabs; t++, t_aux++)
        {
          if (ih->data->tabs_line.tabs_info[t].tab_len > line_length)
          {
            if ((t - 1) < ih->data->current_tab)
            {
              while (t_aux > 0)
              {
                line_length +=
                  ih->data->tabs_line.tabs_info[t - t_aux].tab_len;

                if (ih->data->tabs_line.start_tab == 0)
                {
                  line_length -= ITABS_BROKEN_TAB;
                  ih->data->tabs_line.broken_start_visible = ITABS_TRUE;
                }

                ih->data->tabs_line.start_tab++;

                if (ih->data->tabs_line.tabs_info[t].tab_len <= line_length)
                  break;

                t_aux--;
              }

              if (ih->data->tabs_line.tabs_info[t].tab_len > line_length)
              {
                /* no success */
                ih->data->tabs_line.broken_center_visible = ITABS_TRUE;
                break;                /* break two loops */
              }
            }
            else
            {
              t--;
              break;                 /* break two loops */
            }
          }

          line_length -= ih->data->tabs_line.tabs_info[t].tab_len;
          ih->data->tabs_line.broken_center_visible = ITABS_FALSE;
        }

        if (t == ih->data->number_of_tabs)
          t--;
      }
      while (t < ih->data->current_tab);

      if (ih->data->tabs_line.start_tab > 0 &&
          ih->data->tabs_line.broken_center_visible != ITABS_TRUE)
      {
        if (ih->data->tabs_line.tabs_info[ih->data->tabs_line.start_tab - 1].tab_len <
            (line_length - ITABS_BROKEN_TAB))
        {
          ih->data->tabs_line.start_tab--;
          line_length = old_line_length;
          continue;
        }
      }

      break;
    }
  }
  else
    ih->data->tabs_line.broken_start_visible = ITABS_FALSE;

  /* space not occupied by tabs is destined for the broken tabs */
  ih->data->tabs_line.broken_tab_space = line_length;

  if (ih->data->tabs_line.broken_start_visible == ITABS_TRUE)
    ih->data->tabs_line.broken_tab_space += ITABS_BROKEN_TAB;

  if (ih->data->tabs_line.broken_center_visible == ITABS_TRUE &&
      ih->data->tabs_line.start_tab == t)
  {
    /* If there is one visible tab and more tabs to the right,
       it must be clipped */
    ih->data->tabs_line.end_tab = ih->data->tabs_line.start_tab;
    ih->data->tabs_line.broken_end_visible = ITABS_FALSE;
    ih->data->tabs_line.broken_start_visible = ITABS_FALSE;
  }
  else
  {
    ih->data->tabs_line.end_tab = t;
    ih->data->tabs_line.broken_center_visible = ITABS_FALSE;

    /* If the last tab is not visible, it must be included the
       right broken tab, and delete one of tabs, when necessary */
    if (ih->data->tabs_line.end_tab < (ih->data->number_of_tabs - 1))
    {
      ih->data->tabs_line.broken_end_visible = ITABS_TRUE;

      if (line_length < ITABS_BROKEN_TAB)
      {
        if (ih->data->tabs_line.start_tab == ih->data->tabs_line.end_tab)
        {
          /* no space to broken tabs */
          ih->data->tabs_line.broken_center_visible = ITABS_TRUE;
          ih->data->tabs_line.broken_end_visible = ITABS_FALSE;
          ih->data->tabs_line.broken_start_visible = ITABS_FALSE;

          ih->data->tabs_line.broken_tab_space +=
            ih->data->tabs_line.tabs_info[ih->data->tabs_line.start_tab].tab_len;
        }
        else if (ih->data->current_tab == ih->data->tabs_line.end_tab)
        {
          ih->data->tabs_line.broken_tab_space +=
            ih->data->tabs_line.tabs_info[ih->data->tabs_line.start_tab].tab_len;
          ih->data->tabs_line.start_tab++;
          ih->data->tabs_line.broken_start_visible = ITABS_TRUE;
        }
        else
        {
          ih->data->tabs_line.broken_tab_space +=
            ih->data->tabs_line.tabs_info[ih->data->tabs_line.end_tab].tab_len;
          ih->data->tabs_line.end_tab--;
        }
      }
    }
    else
      ih->data->tabs_line.broken_end_visible = ITABS_FALSE;
  }

  if (ih->data->tabs_line.broken_start_visible == ITABS_TRUE &&
      ih->data->tabs_line.broken_end_visible == ITABS_TRUE)
  {
    ih->data->tabs_line.broken_space_start = (int)
      ceil((double) ih->data->tabs_line.broken_tab_space / 2);
    ih->data->tabs_line.broken_space_end = (int)
      floor((double) ih->data->tabs_line.broken_tab_space / 2);
  }
  else if (ih->data->tabs_line.broken_start_visible == ITABS_TRUE)
  {
    ih->data->tabs_line.broken_space_start = ih->data->tabs_line.broken_tab_space;
    ih->data->tabs_line.broken_space_end = 0;
  }
  else if (ih->data->tabs_line.broken_end_visible == ITABS_TRUE)
  {
    ih->data->tabs_line.broken_space_end = ih->data->tabs_line.broken_tab_space;
    ih->data->tabs_line.broken_space_start = 0;
  }
  else
  {
    ih->data->tabs_line.broken_space_end = 0;
    ih->data->tabs_line.broken_space_start = 0;
  }

  if (ih->data->tabs_line.start_tab == 0)
    ih->data->tabs_line.broken_start_visible = ITABS_FALSE;
}

static void iTabsGetDecorOffset(Ihandle* ih, int *x, int *y)
{
  switch(ih->data->tabs_type)
  {
    case ITABS_TOP:
      *x = ITABS_MARGIN;
      *y = *(ih->data->tabs_line.line_h) + ITABS_MARGIN;
      break;

    case ITABS_LEFT:
      *x = *(ih->data->tabs_line.line_w) + ITABS_MARGIN;
      *y = ITABS_MARGIN;
      break;

    case ITABS_RIGHT:
    case ITABS_BOTTOM:
    default:
      *x = ITABS_MARGIN;
      *y = ITABS_MARGIN;
      break;
  }
}

static void iTabsGetDecorSize(Ihandle* ih, int *w, int *h)
{
  switch(ih->data->tabs_type)
  {
    case ITABS_TOP:
      *w = 2 * ITABS_MARGIN;
      *h = *(ih->data->tabs_line.line_h) + 2 * ITABS_MARGIN;
      break;

    case ITABS_BOTTOM:
      *w = 2 * ITABS_MARGIN;
      *h = *(ih->data->tabs_line.line_h) + 2 * ITABS_MARGIN;
      break;

    case ITABS_LEFT:
      *w = *(ih->data->tabs_line.line_w) + 2 * ITABS_MARGIN;
      *h = 2 * ITABS_MARGIN;
      break;

    case ITABS_RIGHT:
      *w = *(ih->data->tabs_line.line_w) + 2 * ITABS_MARGIN;
      *h = 2 * ITABS_MARGIN;
      break;
  }
}

/* Changes the zbox value and redraws the tabs */
static void iTabsSetNewCurrentTab(Ihandle* ih, int tab)
{
  char* next_tab_name = IupGetName(iTabsGetTabIhandle(ih, tab));
  if (next_tab_name == NULL)
    return;

  ih->data->current_tab = tab;
  IupSetAttribute(ih->data->zbox, "VALUE", next_tab_name);

  iTabsCalcVisibleTabs(ih); /* calculates the visibility of tabs (depends on the sizes calculated and on the current tab) */

  IupUpdate(ih);
}


/* ========================================================================= */
/* IupCanvas Callbacks                                                       */
/* ========================================================================= */


static int iTabsFocus_CB(Ihandle* ih, int focus)
{
  ih->data->has_focus = focus;
  IupUpdate(ih);
  return IUP_DEFAULT;
}

static int iTabsResize_CB(Ihandle* ih)
{
  if (!ih->data->cddbuffer)
  {
    /* update canvas size */
    cdCanvasActivate(ih->data->cdcanvas);

    /* this can fail if canvas size is zero */
    ih->data->cddbuffer = cdCreateCanvas(CD_DBUFFER, ih->data->cdcanvas);
  }

  if (!ih->data->cddbuffer)
    return IUP_DEFAULT;

  /* update canvas size */
  cdCanvasActivate(ih->data->cddbuffer);
  cdCanvasGetSize(ih->data->cddbuffer, &ih->data->w, &ih->data->h, NULL, NULL);

  iTabsCalcTabSize(ih);     /* calculates sizes of Tab line (depends on the TABTITLE of all children and on FONT) */
  iTabsCalcScrollPos(ih);   /* calculates the position of the scroll (depends on the sizes calculated) */
  iTabsCalcVisibleTabs(ih); /* calculates the visibility of tabs (depends on the sizes calculated and on the current tab) */

  return IUP_DEFAULT;
}

static int iTabsRedraw_CB(Ihandle* ih)
{
  char* clip_rect;

  if (!ih->data->cddbuffer)
    return IUP_DEFAULT;

  /* Paints the background and draws the border */
  iTabsDrawBackground(ih);
  iTabsDrawTabsBorders(ih);

  /* Draws tabs and decorations */
  iTabsDrawLineOfTabs(ih);

  clip_rect = iupAttribGet(ih, "CLIPRECT");
  if (clip_rect)
  {
    int x1, x2, y1, y2;
    sscanf(clip_rect, "%d %d %d %d", &x1, &y1, &x2, &y2);
    y1 = cdIupInvertYAxis(y1, ih->data->h);
    y2 = cdIupInvertYAxis(y2, ih->data->h);
    cdCanvasClipArea(ih->data->cdcanvas, x1, x2, y1, y2);
    cdCanvasClip(ih->data->cdcanvas, CD_CLIPAREA);
  }

  /* Ends the draw process */
	cdCanvasFlush(ih->data->cddbuffer);
  if (ih->data->has_focus)
    cdIupDrawFocusRect(ih, ih->data->cdcanvas, ih->data->focus_x1, ih->data->focus_y1, ih->data->focus_x2, ih->data->focus_y2);

  if (clip_rect)
    cdCanvasClip(ih->data->cdcanvas, CD_CLIPOFF);

  return IUP_DEFAULT;
}

/* Callback to popup menu, with the tabs list */
static int iTabsMenu_CB(Ihandle* ih)
{
  char* number = iupAttribGet(ih, "_IUPTABS_ID");
  int tab_number = 0;
  int result;

  sscanf (number, "%d", &tab_number);

  if (ih->data->current_tab != tab_number)
  {
    result = iTabsCallTabChangeCb(ih, iTabsGetTabIhandle(ih, tab_number),
                                      iTabsGetTabIhandle(ih, ih->data->current_tab));
    if (result != IUP_IGNORE)
      iTabsSetNewCurrentTab(ih, tab_number);
  }

  return IUP_DEFAULT;
}

/* Creates a menu with all the tab labels */
static Ihandle* iTabsMakeMenuFromTabs(Ihandle* ih)
{
  int c = 0;
  Ihandle* menu = IupMenu(NULL);
  Ihandle* item = NULL;

  for(c = 0; c < ih->data->number_of_tabs; c++)
  {
    char* title = iupAttribGet(ih->data->tabs_line.tabs_info[c].ihandle, "TABTITLE");
    if (!title) title = "   ";

    item = IupItem(title, NULL);
    IupSetCallback(item, "ACTION", (Icallback) iTabsMenu_CB);

    iupAttribSetStrf(item, "_IUPTABS_ID", "%4d", c);

    if (c == ih->data->current_tab)
      IupSetAttribute(item, "VALUE", "ON");

    IupAppend(menu, item);
  }

  return menu;
}

static int iTabsGetNextTab(Ihandle* ih)
{
  int next_tab;

  if (ih->data->current_tab < (ih->data->number_of_tabs - 1))
    next_tab = ih->data->current_tab + 1;
  else
    next_tab = 0;

  /* find the next active tab */
  while(next_tab != ih->data->current_tab &&
        !IupGetInt(iTabsGetTabIhandle(ih, next_tab), "ACTIVE"))
  {
    if (next_tab < (ih->data->number_of_tabs - 1))
      next_tab = next_tab + 1;
    else
      next_tab = 0;
  }

  if (next_tab == ih->data->current_tab)
    return -1;

  return next_tab;
}

static int iTabsGetPreviousTab(Ihandle* ih)
{
  int previous_tab;
  if (ih->data->current_tab > 0)
    previous_tab = ih->data->current_tab - 1;
  else
    previous_tab = ih->data->number_of_tabs - 1;

  /* find the previous active tab */
  while(previous_tab != ih->data->current_tab &&
        !IupGetInt(iTabsGetTabIhandle(ih, previous_tab), "ACTIVE"))
  {
    if (previous_tab > 0)
      previous_tab = previous_tab - 1;
    else
      previous_tab = ih->data->number_of_tabs - 1;
  }

  if (previous_tab == ih->data->current_tab)
    return -1;

  return previous_tab;
}

/* Key press Callback */
static int iTabsKeyPress_CB(Ihandle* ih, int c, int press)
{
  if (press == 0)
    return IUP_DEFAULT;

  if (((c == K_RIGHT || c == K_sRIGHT) && (ih->data->tabs_type == ITABS_TOP  || ih->data->tabs_type == ITABS_BOTTOM)) ||
     ((c == K_DOWN  || c == K_sDOWN)  && (ih->data->tabs_type == ITABS_LEFT || ih->data->tabs_type == ITABS_RIGHT)))
  {
    int result;
    int next_tab = iTabsGetNextTab(ih);
    if (next_tab == -1)
      return IUP_IGNORE;  /* to avoid arrow keys being processed by the system */

    result = iTabsCallTabChangeCb(ih, iTabsGetTabIhandle(ih, next_tab), 
                                      iTabsGetTabIhandle(ih, ih->data->current_tab));
    if (result != IUP_IGNORE)
      iTabsSetNewCurrentTab(ih, next_tab);

    return IUP_IGNORE;  /* to avoid arrow keys being processed by the system */
  }
  else if (((c == K_LEFT || c == K_sLEFT) && (ih->data->tabs_type == ITABS_TOP  || ih->data->tabs_type == ITABS_BOTTOM)) || 
          ((c == K_UP   || c == K_sUP)   && (ih->data->tabs_type == ITABS_LEFT || ih->data->tabs_type == ITABS_RIGHT)))
  {
    int result;
    int previous_tab = iTabsGetPreviousTab(ih);
    if (previous_tab == -1)
      return IUP_IGNORE;  /* to avoid arrow keys being processed by the system */

    result = iTabsCallTabChangeCb(ih, iTabsGetTabIhandle(ih, previous_tab), 
                                      iTabsGetTabIhandle(ih, ih->data->current_tab));
    if (result != IUP_IGNORE)
      iTabsSetNewCurrentTab(ih, previous_tab);

    return IUP_IGNORE;  /* to avoid arrow keys being processed by the system */
  }

  return IUP_DEFAULT;
}

/* Button press Callback of IUP Canvas */
static int iTabsButton_CB (Ihandle* ih, int b, int press, int mx, int my)
{
  static int last_tab_press = -1;
  int* virtual_width = NULL;
  int* virtual_height = NULL;
  int  width = 0, height = 0, offset = 0;
  int*  virtual_mx = NULL;
  int*  virtual_my = NULL;
  int   click_x = mx, click_y = ih->data->h-1 - my;
  int   count = 0;

  /* Only events of the left button are handled */
  if (b != IUP_BUTTON1)
    return IUP_DEFAULT;

  /* Calculates the bounding box size of the tabs queue */
  switch(ih->data->tabs_type)
  {
    case ITABS_TOP:
      width  = ih->data->w;
      height = ih->data->tabs_line.line_thick;
      virtual_width  = &width;
      virtual_height = &height;
      virtual_mx  = &mx;
      virtual_my  = &my;
      *virtual_my = -(*virtual_my - (ih->data->tabs_line.line_thick - 1));
      break;

    case ITABS_BOTTOM:
      width  = ih->data->w;
      height = ih->data->tabs_line.line_thick;
      virtual_width  = &width;
      virtual_height = &height;
      virtual_mx  = &mx;
      virtual_my  = &my;
      *virtual_my = *virtual_my - ih->data->h + ih->data->tabs_line.line_thick;
      break;

    case ITABS_LEFT:
      width  = ih->data->tabs_line.line_thick;
      height = ih->data->h;
      virtual_width  = &height;
      virtual_height = &width;
      virtual_mx  = &my;
      virtual_my  = &mx;
      *virtual_my = -(*virtual_my - (ih->data->tabs_line.line_thick - 1));
      break;

    case ITABS_RIGHT:
      width  = ih->data->tabs_line.line_thick;
      height = ih->data->h;
      virtual_width  = &height;
      virtual_height = &width;
      virtual_mx  = &my;
      virtual_my  = &mx;
      *virtual_my = *virtual_my - ih->data->w + ih->data->tabs_line.line_thick;
      break;
  }

  /* Adjusts the offset when the first tab is not selected */
  if (ih->data->current_tab != 0)
    offset = 1;

  /* Checks if the mouse is on the tabs area */
  if ((*virtual_mx > (*virtual_width - 1)) ||
    (*virtual_my > (*virtual_height - 1)) ||
    (*virtual_mx < 0) || (*virtual_my < 0))
  {
    /* If a button release or any scroll_visible button are pressed, they'll are released */
    if (!press && ih->data->tabs_line.button != ITABS_BUTTON_NONE)
      ih->data->tabs_line.button = ITABS_BUTTON_NONE;

    return IUP_DEFAULT;
  }

  if (!press && ih->data->tabs_line.button != ITABS_BUTTON_NONE)
    ih->data->tabs_line.button += 3; /* increment from BUTPRESS_* to BUTRELEASE_* */

  /* If there is a scroll_visible, verifies if the click was on one of buttons */
  if (ih->data->tabs_line.scroll_visible == ITABS_TRUE)
  {
    int scroll_pos, scroll_space;

    if (ih->data->tabs_orientation == ITABS_HORIZONTAL)
    {
      scroll_space = ih->data->tabs_line.scroll_w;
      scroll_pos = click_x - ih->data->tabs_line.scroll_x;
    }
    else
    {
      scroll_space = ih->data->tabs_line.scroll_h;
      scroll_pos = click_y - ih->data->tabs_line.scroll_y;

      /* the controls are inverted when text is vertical */
      scroll_pos = scroll_space - scroll_pos;
    }

    if (click_y > ih->data->tabs_line.scroll_y && click_x > ih->data->tabs_line.scroll_x &&
        click_y < ih->data->tabs_line.scroll_y+ih->data->tabs_line.scroll_h && click_x < ih->data->tabs_line.scroll_x+ih->data->tabs_line.scroll_w)
    {
      /* Verifies which button was pressed by user */
      if (scroll_pos > 2 * scroll_space / 3)        /* forward button*/
      {
        /* If press, to draw it pressed */
        if (press)
        {
          ih->data->tabs_line.button = ITABS_BUTTONPRESS_FORWARD ;
        }
        else if (ih->data->tabs_line.button == ITABS_BUTTONRELEASE_FORWARD)
        /* else, change the tab */
        {
          int result;
          int next_tab = iTabsGetNextTab(ih);
          if (next_tab == -1)
            return IUP_DEFAULT;

          result = iTabsCallTabChangeCb(ih, iTabsGetTabIhandle(ih, next_tab), iTabsGetTabIhandle(ih, ih->data->current_tab));
          if (result != IUP_IGNORE)
            iTabsSetNewCurrentTab(ih, next_tab);
        }    
      }
      else if (scroll_pos > scroll_space / 3)        /* menu button*/
      {
        /* If press, to draw it pressed */
        if (press)
        {
          ih->data->tabs_line.button = ITABS_BUTTONPRESS_MENU ;
        }
        else if (ih->data->tabs_line.button == ITABS_BUTTONRELEASE_MENU)
        /* else, change the tab */
        {
          Ihandle* menu = iTabsMakeMenuFromTabs (ih);
          IupPopup(menu, IUP_MOUSEPOS, IUP_MOUSEPOS);
          IupDestroy(menu);
        }
      }
      else        /* backward button*/
      {
        /* If press, to draw it pressed */
        if (press)
        {
          ih->data->tabs_line.button = ITABS_BUTTONPRESS_BACKWARD ;
        }
        else if (ih->data->tabs_line.button == ITABS_BUTTONRELEASE_BACKWARD)
          /* else, change the tab */
        {
          int result;
          int previous_tab = iTabsGetPreviousTab(ih);
          if (previous_tab == -1)
            return IUP_DEFAULT;

          result = iTabsCallTabChangeCb(ih, iTabsGetTabIhandle(ih, previous_tab), 
                                            iTabsGetTabIhandle(ih, ih->data->current_tab));
          if (result != IUP_IGNORE)
            iTabsSetNewCurrentTab(ih, previous_tab);
        }
      }

      return IUP_DEFAULT;
    }
  }

  /* If there is any scroll_visible button pressed, it will be released */
  if (!press && ih->data->tabs_line.button != ITABS_BUTTON_NONE)
  {
    ih->data->tabs_line.button = ITABS_BUTTON_NONE;
    IupUpdate(ih);
    return IUP_DEFAULT;
  }

  /* Checks when the click was on the left broken tab */
  if (ih->data->tabs_line.broken_start_visible == ITABS_TRUE)
  {
    if (*virtual_mx >= offset &&
       *virtual_mx <= (offset + ih->data->tabs_line.broken_space_start - 1) &&
       *virtual_my <= (ih->data->tabs_line.line_thick - ITABS_CURRENT_EXTRA_PIXELS - 1))
    {
      if (press)
        last_tab_press = ih->data->tabs_line.start_tab-1;
      else if (last_tab_press == ih->data->tabs_line.start_tab-1)
      {
        int result = iTabsCallTabChangeCb(ih, iTabsGetTabIhandle(ih, last_tab_press), 
                                              iTabsGetTabIhandle(ih, ih->data->current_tab));
        if (result != IUP_IGNORE)
          iTabsSetNewCurrentTab(ih, last_tab_press);
      }

      return IUP_DEFAULT;
    }
    else
      offset += ih->data->tabs_line.broken_space_start;
  }

  /* Checks when the click was on one of tabs */
  for(count = ih->data->tabs_line.start_tab; count <= ih->data->tabs_line.end_tab; count++)
  {
    if (*virtual_mx >= offset && 
       *virtual_mx <= (offset + ih->data->tabs_line.tabs_info[count].tab_len - 1))
    {
      if (count != ih->data->current_tab)
      {
        if (*virtual_my <= (ih->data->tabs_line.line_thick - ITABS_CURRENT_EXTRA_PIXELS - 1))
        {
          if (press)
            last_tab_press = count;
          else if (last_tab_press == count)
          {
            int result = iTabsCallTabChangeCb(ih, iTabsGetTabIhandle(ih, last_tab_press), 
                                                  iTabsGetTabIhandle(ih, ih->data->current_tab));
            if (result != IUP_IGNORE)
              iTabsSetNewCurrentTab(ih, last_tab_press);
          }

          return IUP_DEFAULT;
        }
      }
    }

    offset += ih->data->tabs_line.tabs_info[count].tab_len;
  }

  /* Checks when the click was on the right broken tab */
  if (ih->data->tabs_line.broken_end_visible == ITABS_TRUE)
  {
    if (*virtual_mx >= offset &&
       *virtual_mx <= (offset + ih->data->tabs_line.broken_space_end - 1) &&
       *virtual_my <= (ih->data->tabs_line.line_thick - ITABS_CURRENT_EXTRA_PIXELS - 1))
    {
      if (press)
        last_tab_press = ih->data->tabs_line.end_tab + 1;
      else if (last_tab_press == ih->data->tabs_line.end_tab + 1)
      {
        int result = iTabsCallTabChangeCb(ih, iTabsGetTabIhandle(ih, last_tab_press),
                                              iTabsGetTabIhandle(ih, ih->data->current_tab));
        if (result != IUP_IGNORE)
          iTabsSetNewCurrentTab(ih, last_tab_press);
      }

      return IUP_DEFAULT;
    }
    else
      offset += ih->data->tabs_line.broken_space_end;
  }

  return IUP_DEFAULT;
}

static void iTabsRefreshTabs(Ihandle* ih)
{
  int t, old_number_of_tabs = ih->data->number_of_tabs;
  Ihandle* child, *old_current_tab_handle = NULL;

  if (ih->data->tabs_line.tabs_info)
    old_current_tab_handle = ih->data->tabs_line.tabs_info[ih->data->current_tab].ihandle;

  ih->data->number_of_tabs = 0;
  child = ih->data->zbox->firstchild;
  while(child)
  {
    if (IupGetName(child) == NULL)
      iupAttribSetHandleName(child);

    ih->data->number_of_tabs++;
    child = child->brother;
  }

  if (old_number_of_tabs < ih->data->number_of_tabs)
    ih->data->tabs_line.tabs_info = (ItabsDrawInfo*)realloc(ih->data->tabs_line.tabs_info, ih->data->number_of_tabs * sizeof (ItabsDrawInfo));

  t = 0;
  child = ih->data->zbox->firstchild;
  while (child)
  {
    ih->data->tabs_line.tabs_info[t].ihandle = child;

    if (child == old_current_tab_handle)
      ih->data->current_tab = t;           /* current tab was moved */

    child = child->brother;
    t++;
  }

  if (old_current_tab_handle &&
      old_current_tab_handle != ih->data->tabs_line.tabs_info[ih->data->current_tab].ihandle)
  {
    /* current tab was removed, reset to first child */
    ih->data->current_tab = 0;
  }
}


/* ========================================================================= */
/* Attributes                                                                */
/* ========================================================================= */


static int iTabsSetAlignmentAttrib(Ihandle* ih, const char* value)
{
  int inherit;
  return iupClassObjectSetAttribute(ih->data->zbox, "ALIGNMENT", value, &inherit);
}

static char* iTabsGetAlignmentAttrib(Ihandle* ih)
{
  int inherit;
  char *def_value;
  return iupClassObjectGetAttribute(ih->data->zbox, "ALIGNMENT", &def_value, &inherit);
}

static int iTabsSetTabOrientationAttrib(Ihandle* ih, const char* value)
{
  if (iupStrEqualNoCase(value, "VERTICAL"))
    ih->data->tabs_orientation = ITABS_VERTICAL;
  else
    ih->data->tabs_orientation = ITABS_HORIZONTAL;

  iTabsUpdateRender(ih);

  return 0;  /* do not store value in hash table */
}

static char* iTabsGetTabOrientationAttrib(Ihandle* ih)
{
  if (ih->data->tabs_orientation == ITABS_VERTICAL)
    return "VERTICAL";
  else
    return "HORIZONTAL";
}

static int iTabsSetTabTypeAttrib(Ihandle* ih, const char* value)
{
  if (iupStrEqualNoCase(value, "BOTTOM"))
    ih->data->tabs_type = ITABS_BOTTOM;
  else if (iupStrEqualNoCase(value, "LEFT"))
    ih->data->tabs_type = ITABS_LEFT;
  else if (iupStrEqualNoCase(value, "RIGHT"))
    ih->data->tabs_type = ITABS_RIGHT;
  else /* "TOP" */
    ih->data->tabs_type = ITABS_TOP;

  iTabsUpdateRender(ih);

  return 0;  /* do not store value in hash table */
}

static char* iTabsGetTabTypeAttrib(Ihandle* ih)
{
  if (ih->data->tabs_type == ITABS_BOTTOM)
    return "BOTTOM";
  else if (ih->data->tabs_type == ITABS_LEFT)
    return "LEFT";
  else if (ih->data->tabs_type == ITABS_RIGHT)
    return "RIGHT";
  else /* ITABS_TOP */
    return "TOP";
}

static int iTabsSetRepaintAttrib(Ihandle* ih, const char* value)
{
  (void)value;

  iTabsCalcTabSize(ih);     /* calculate sizes of Tab line (depends on the TABTITLE of all children) */
  iTabsCalcScrollPos(ih);   /* calculates the position of the scroll (depends on the sizes calculated) */
  iTabsCalcVisibleTabs(ih); /* calculates the visibility of tabs (depends on the sizes calculated and on the current tab) */

  IupUpdate(ih);

  return 0;  /* do not store value in hash table */
}

static int iTabsSetFontActiveAttrib(Ihandle* ih, const char* value)
{
  char* native = IupMapFont(value);
  if (!native) native = (char*)value;

  if (native)
  {
    if (ih->data->font_active)
      free(ih->data->font_active);

    ih->data->font_active = iupStrDup(native);
    IupUpdate(ih);
  }
  return 1;
}

static int iTabsSetFontInactiveAttrib(Ihandle* ih, const char* value)
{
  char* native = IupMapFont(value);
  if (!native) native = (char*)value;

  if (native)
  {
    if (ih->data->font_inactive)
      free(ih->data->font_inactive);
    
    ih->data->font_inactive = iupStrDup(native);
    IupUpdate(ih);
  }
  return 1;
}

static int iTabsSetFgColorAttrib(Ihandle* ih, const char* value)
{
  ih->data->fgcolor = cdIupConvertColor(value);
  IupUpdate(ih);
  return 1;
}

static int iTabsSetBgColorAttrib(Ihandle* ih, const char* value)
{
  if (!value)
    value = iupControlBaseGetParentBgColor(ih);

  ih->data->bgcolor = cdIupConvertColor(value);
  cdIupCalcShadows(ih->data->bgcolor, &ih->data->light_shadow, &ih->data->mid_shadow, &ih->data->dark_shadow);

  IupUpdate(ih);
  return 1;
}

static int iTabsSetActiveAttrib(Ihandle* ih, const char* value)
{
  iupBaseSetActiveAttrib(ih, value);
  IupUpdate(ih);  /* post repaint so children is activated/deactivated */
  return 0;   /* do not store value in hash table */
}

static int iTabsSetValueAttrib(Ihandle* ih, const char* value)
{
  int c;
  Ihandle* new_tab = IupGetHandle(value);

  if (new_tab == NULL)
    return 0;

  for(c = 0; c < ih->data->number_of_tabs; c++)
  {
    if (ih->data->tabs_line.tabs_info[c].ihandle == new_tab)
    {
      iTabsSetNewCurrentTab(ih, c);
      break;
    }
  }
  return 0;  /* do not store value in hash table */
}

static char* iTabsGetValueAttrib(Ihandle* ih)
{
  return IupGetAttribute(ih->data->zbox, "VALUE");
}

static int iTabsSetValuePosAttrib(Ihandle* ih, const char* value)
{
  int pos;

  if (!iupStrToInt(value, &pos))
    return 0;

  if (pos<0) pos=0;
  if (pos>ih->data->number_of_tabs-1) pos=ih->data->number_of_tabs-1;

  iTabsSetNewCurrentTab(ih, pos);
  return 0;  /* do not store value in hash table */
}

static char* iTabsGetValuePosAttrib(Ihandle* ih)
{
  char *str = iupStrGetMemory(50);
  sprintf(str, "%d", ih->data->current_tab);
  return str;
}

static char* iTabsGetClientSizeAttrib(Ihandle* ih)
{
  int width, height, decorwidth, decorheight;
  char* str = iupStrGetMemory(20);
  width = ih->currentwidth;
  height = ih->currentheight;
  iTabsGetDecorSize(ih, &decorwidth, &decorheight);
  width -= decorwidth;
  height -= decorheight;
  if (width < 0) width = 0;
  if (height < 0) height = 0;
  sprintf(str, "%dx%d", width, height);
  return str;
}


/* ========================================================================= */
/* Methods                                                                   */
/* ========================================================================= */


static Ihandle* iTabsGetInnerContainerMethod(Ihandle* ih)
{
  return ih->data->zbox;
}

static void iTabsComputeNaturalSizeMethod(Ihandle* ih, int *w, int *h, int *expand)
{
  int decorwidth, decorheight;
  Ihandle* child = ih->data->zbox;  /* zbox is always non NULL */

  iTabsCalcTabSize(ih);  /* make sure that decoration is updated, even if UPDATE has not been set after other changes */
  iTabsGetDecorSize(ih, &decorwidth, &decorheight);

  /* update child natural size first */
  iupBaseComputeNaturalSize(child);

  *expand = child->expand;
  *w = child->naturalwidth + decorwidth;
  *h = child->naturalheight + decorheight;
}

static void iTabsSetChildrenCurrentSizeMethod(Ihandle* ih, int shrink)
{
  int width, height, decorwidth, decorheight;

  iTabsGetDecorSize(ih, &decorwidth, &decorheight);

  width = ih->currentwidth-decorwidth;
  height = ih->currentheight-decorheight;
  if (width < 0) width = 0;
  if (height < 0) height = 0;

  iupBaseSetCurrentSize(ih->data->zbox, width, height, shrink);
}

static void iTabsSetChildrenPositionMethod(Ihandle* ih, int x, int y)
{
  /* IupTabs is the native parent of its children,
  so the position is restarted at (0,0) */
  iTabsGetDecorOffset(ih, &x, &y);

  /* Child coordinates are relative to client left-top corner. */
  iupBaseSetPosition(ih->data->zbox, x, y);
}

static void iTabsChildAddedMethod(Ihandle* ih, Ihandle* child)
{
  (void)child;
  iTabsRefreshTabs(ih);    /* update the list of tabs */
}

static void iTabsChildRemovedMethod(Ihandle* ih, Ihandle* child)
{
  (void)child;
  iTabsRefreshTabs(ih);    /* update the list of tabs */
}

static int iTabsMapMethod(Ihandle* ih)
{
  ih->data->cdcanvas = cdCreateCanvas(CD_IUP, ih);
  if (!ih->data->cdcanvas)
    return IUP_ERROR;

  /* this can fail if canvas size is zero */
  ih->data->cddbuffer = cdCreateCanvas(CD_DBUFFER, ih->data->cdcanvas);

  return IUP_NOERROR;
}

static void iTabsUnMapMethod(Ihandle* ih)
{
  if (ih->data->cddbuffer)
  {
    cdKillCanvas(ih->data->cddbuffer);
    ih->data->cddbuffer = NULL;
  }

  if (ih->data->cdcanvas)
  {
    cdKillCanvas(ih->data->cdcanvas);
    ih->data->cdcanvas = NULL;
  }
}

static void iTabsDestroyMethod(Ihandle* ih)
{
  if (ih->data->font_inactive)
    free(ih->data->font_inactive);

  if (ih->data->font_inactive)
    free(ih->data->font_inactive);

  free(ih->data->tabs_line.tabs_info);
}

static int iTabsCreateMethod(Ihandle* ih, void **params)
{
  /* free the data allocated by IupCanvas */
  if (ih->data)
    free(ih->data);
  ih->data = iupALLOCCTRLDATA();

  /* change the IupCanvas default values */
  iupAttribSetStr(ih, "BORDER", "NO");

  ih->data->zbox = IupZbox(NULL);
  ih->firstchild = ih->data->zbox;   /* zbox is actually the only child of Tabs */
  ih->data->zbox->parent = ih;

  /* add children */
  if (params)
  {
    Ihandle** iparams = (Ihandle**)params;
    while (*iparams) 
    {
      IupAppend(ih, *iparams);    /* this in fact will add the child to the zbox */
      iparams++;
    }
  }

  /* IupCanvas callbacks */
  IupSetCallback(ih, "RESIZE_CB",   (Icallback)iTabsResize_CB);
  IupSetCallback(ih, "ACTION",      (Icallback)iTabsRedraw_CB);
  IupSetCallback(ih, "BUTTON_CB",   (Icallback)iTabsButton_CB);
  IupSetCallback(ih, "KEYPRESS_CB", (Icallback)iTabsKeyPress_CB);
  IupSetCallback(ih, "FOCUS_CB",    (Icallback)iTabsFocus_CB);

  /* initialize colors */
  ih->data->bgcolor = -1;
  ih->data->fgcolor = CD_BLACK;
  ih->data->light_shadow = CD_WHITE;
  ih->data->mid_shadow   = CD_GRAY;
  ih->data->dark_shadow  = CD_DARK_GRAY;

  /* Context initialize with values related to type of tabs */  
  ih->data->tabs_type = ITABS_TOP;
  ih->data->tabs_orientation = ITABS_HORIZONTAL;

  /* No button is pressed */
  ih->data->tabs_line.button = ITABS_BUTTON_NONE ;

  iTabsUpdateRender(ih);

  return IUP_NOERROR;
}

static Iclass* iTabsGetClass(void)
{
  Iclass* ic = iupClassNew(iupCanvasGetClass());

  ic->name = "tabs";
  ic->format = "g"; /* array of Ihandle */
  ic->nativetype = IUP_TYPECANVAS;
  ic->childtype  = IUP_CHILD_ONE;
  ic->is_interactive = 1;

  /* Class functions */
  ic->Create  = iTabsCreateMethod;
  ic->Destroy = iTabsDestroyMethod;
  ic->Map     = iTabsMapMethod;
  ic->UnMap   = iTabsUnMapMethod;

  ic->ComputeNaturalSize = iTabsComputeNaturalSizeMethod;
  ic->SetChildrenCurrentSize     = iTabsSetChildrenCurrentSizeMethod;
  ic->SetChildrenPosition        = iTabsSetChildrenPositionMethod;

  ic->GetInnerContainer = iTabsGetInnerContainerMethod;
  ic->ChildAdded     = iTabsChildAddedMethod;
  ic->ChildRemoved   = iTabsChildRemovedMethod;

  /* Do not need to set base attributes because they are inherited from IupCanvas */

  /* IupTabs Callbacks */
  iupClassRegisterCallback(ic, "TABCHANGE_CB", "nn");

  /* IupTabs only */
  iupClassRegisterAttribute(ic, "ALIGNMENT", iTabsGetAlignmentAttrib, iTabsSetAlignmentAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "TABTYPE",   iTabsGetTabTypeAttrib, iTabsSetTabTypeAttrib,  IUPAF_SAMEASSYSTEM, "TOP", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "TABORIENTATION", iTabsGetTabOrientationAttrib, iTabsSetTabOrientationAttrib, IUPAF_SAMEASSYSTEM, "HORIZONTAL", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "REPAINT", NULL, iTabsSetRepaintAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "FONT_ACTIVE", NULL, iTabsSetFontActiveAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "FONT_INACTIVE", NULL, iTabsSetFontInactiveAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT); 
  iupClassRegisterAttribute(ic, "VALUE", iTabsGetValueAttrib, iTabsSetValueAttrib, NULL, NULL, IUPAF_NO_DEFAULTVALUE|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "VALUEPOS", iTabsGetValuePosAttrib, iTabsSetValuePosAttrib, NULL, NULL, IUPAF_NO_DEFAULTVALUE|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "TABTITLE", NULL, NULL, NULL, NULL, IUPAF_NO_DEFAULTVALUE|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "TABSIZE", NULL, NULL, NULL, NULL, IUPAF_NO_DEFAULTVALUE|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "FGCOLOR", NULL, iTabsSetFgColorAttrib, NULL, NULL, IUPAF_NOT_MAPPED);

  /* Overwrite IupCanvas Attributes */
  iupClassRegisterAttribute(ic, "ACTIVE", iupBaseGetActiveAttrib, iTabsSetActiveAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_DEFAULT);
  iupClassRegisterAttribute(ic, "BGCOLOR", iupControlBaseGetBgColorAttrib, iTabsSetBgColorAttrib, NULL, "255 255 255", IUPAF_DEFAULT); /* overwrite canvas implementation, set a system default to force a new default */

  /* Base Container */
  iupClassRegisterAttribute(ic, "EXPAND", iupBaseContainerGetExpandAttrib, NULL, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "CLIENTSIZE", iTabsGetClientSizeAttrib, NULL, NULL, NULL, IUPAF_NO_DEFAULTVALUE|IUPAF_READONLY|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);    /* overwrite canvas default */
  
  return ic;
}

void IupOldTabsOpen(void)
{
  iupRegisterClass(iTabsGetClass());
}
