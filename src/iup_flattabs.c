/** \file
* \brief iupflattabs control
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

#include "iup_object.h"
#include "iup_attrib.h"
#include "iup_str.h"
#include "iup_drv.h"
#include "iup_drvfont.h"
#include "iup_stdcontrols.h"
#include "iup_layout.h"
#include "iup_image.h"
#include "iup_register.h"
#include "iup_drvdraw.h"


#define ITABS_CLOSE_SIZE 13
#define ITABS_CLOSE_SPACING 12
#define ITABS_CLOSE_BORDER 8

static void iFlatTabsInitializeCloseImage(void)
{
  Ihandle *image_close;

  unsigned char img_close[ITABS_CLOSE_SIZE * ITABS_CLOSE_SIZE] =
  {
    1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
    1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1,
    0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0,
    0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0,
    0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 0, 0, 0,
    0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0,
    0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 0, 0, 0,
    0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0,
    0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0,
    1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1,
    1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
  };

  image_close = IupImage(ITABS_CLOSE_SIZE, ITABS_CLOSE_SIZE, img_close);
  IupSetAttribute(image_close, "0", "BGCOLOR");
  IupSetAttribute(image_close, "1", "0 0 0");
  IupSetHandle("IMGFLATCLOSE", image_close);

  image_close = IupImage(ITABS_CLOSE_SIZE, ITABS_CLOSE_SIZE, img_close);
  IupSetAttribute(image_close, "0", "BGCOLOR");
  IupSetAttribute(image_close, "1", "128 128 128");
  IupSetHandle("IMGFLATCLOSEPRESS", image_close);

  image_close = IupImage(ITABS_CLOSE_SIZE, ITABS_CLOSE_SIZE, img_close);
  IupSetAttribute(image_close, "0", "BGCOLOR");
  IupSetAttribute(image_close, "1", "50 150 255");
  IupSetHandle("IMGFLATCLOSEHIGH", image_close);
}


static Ihandle* iFlatTabsGetCurrentTab(Ihandle* ih)
{
  return (Ihandle*)iupAttribGet(ih, "_IUPFTABS_VALUE_HANDLE");
}

static void iFlatTabsSetCurrentTab(Ihandle* ih, Ihandle* child)
{
  Ihandle* current_child = iFlatTabsGetCurrentTab(ih);
  if (current_child)
    IupSetAttribute(current_child, "VISIBLE", "No");

  iupAttribSet(ih, "_IUPFTABS_VALUE_HANDLE", (char*)child);
  IupSetAttribute(child, "VISIBLE", "Yes");

  IupUpdate(ih);
}

static void iFlatTabsGetIconSize(Ihandle* ih, int pos, int *w, int *h)
{
  char* image = iupAttribGetId(ih, "TABIMAGE", pos);
  char* title = iupAttribGetId(ih, "TABTITLE", pos);

  *w = 0;
  *h = 0;

  if (image)
  {
    iupImageGetInfo(image, w, h, NULL);

    if (title)
    {
      int img_position = iupFlatGetImagePosition(iupAttribGetStr(ih, "TABSIMAGEPOSITION"));
      int spacing = iupAttribGetInt(ih, "TABSIMAGESPACING");
      int text_w, text_h;
      iupdrvFontGetMultiLineStringSize(ih, title, &text_w, &text_h);

      if (img_position == IUP_IMGPOS_RIGHT ||
          img_position == IUP_IMGPOS_LEFT)
      {
        *w += text_w + spacing;
        *h = iupMAX(*h, text_h);
      }
      else
      {
        *w = iupMAX(*w, text_w);
        *h += text_h + spacing;
      }
    }
  }
  else if (title)
    iupdrvFontGetMultiLineStringSize(ih, title, w, h);
}

static void iFlatTabsSetFont(Ihandle* ih)
{
  char* tabs_font = iupAttribGet(ih, "TABSFONT");
  if (tabs_font)
  {
    char* old_font = IupGetAttribute(ih, "FONT");
    iupAttribSetStr(ih, "_IUP_OLDFONT", old_font);

    iupAttribSetStr(ih, "FONT", tabs_font);
    iupdrvSetFontAttrib(ih, tabs_font);
  }
}

static void iFlatTabsResetFont(Ihandle* ih)
{
  char* old_font = iupAttribGet(ih, "_IUP_OLDFONT");
  if (old_font)
  {
    iupAttribSetStr(ih, "FONT", old_font);
    iupdrvSetFontAttrib(ih, old_font);

    iupAttribSet(ih, "_IUP_OLDFONT", NULL);
  }
}

static void iFlatTabsSetTabFont(Ihandle* ih, int pos)
{
  char* font = iupAttribGetId(ih, "TABFONT", pos);
  if (font)
  {
    char* old_font = IupGetAttribute(ih, "FONT");
    iupAttribSetStr(ih, "_IUP_OLDTABFONT", old_font);

    iupAttribSetStr(ih, "FONT", font);
    iupdrvSetFontAttrib(ih, font);
  }
}

static void iFlatTabsResetTabFont(Ihandle* ih)
{
  char* old_font = iupAttribGet(ih, "_IUP_OLDTABFONT");
  if (old_font)
  {
    iupAttribSetStr(ih, "FONT", old_font);
    iupdrvSetFontAttrib(ih, old_font);

    iupAttribSet(ih, "_IUP_OLDTABFONT", NULL);
  }
}

static int iFlatTabsGetTitleHeight(Ihandle* ih, int *title_width)
{
  int vert_padding, horiz_padding;
  int title_height = 0, w, h, pos, show_close = 0;
  Ihandle* child;

  IupGetIntInt(ih, "TABSPADDING", &horiz_padding, &vert_padding);

  if (title_width)
  {
    *title_width = 0;
    show_close = iupAttribGetBoolean(ih, "SHOWCLOSE");
  }

  iFlatTabsSetFont(ih);

  for (pos = 0, child = ih->firstchild; child; child = child->brother, pos++)
  {
    iFlatTabsGetIconSize(ih, pos, &w, &h);

    if (h > title_height)
      title_height = h;

    if (title_width)
    {
      *title_width += w + 2 * horiz_padding;

      if (show_close)
        *title_width += ITABS_CLOSE_SIZE + ITABS_CLOSE_SPACING + ITABS_CLOSE_BORDER;
    }
  }

  iFlatTabsResetFont(ih);

  return title_height + 2 * vert_padding;
}

static void iFlatGetAlignment(Ihandle* ih, int *horiz_alignment, int *vert_alignment)
{
  char* value = iupAttribGetStr(ih, "TABSALIGNMENT");
  char value1[30], value2[30];

  iupStrToStrStr(value, value1, value2, ':');

  *horiz_alignment = iupFlatGetHorizontalAlignment(value1);
  *vert_alignment = iupFlatGetVerticalAlignment(value2);
}

static int iFlatTabsRedraw_CB(Ihandle* ih)
{
  Ihandle* current_child = iFlatTabsGetCurrentTab(ih);
  char* bgcolor = iupAttribGetStr(ih, "BGCOLOR");
  char* forecolor = iupAttribGetStr(ih, "FORECOLOR");
  char* highcolor = iupAttribGetStr(ih, "HIGHCOLOR");
  char* tabs_bgcolor = iupAttribGetStr(ih, "TABSBACKCOLOR");
  char* tabs_forecolor = iupAttribGetStr(ih, "TABSFORECOLOR");
  char* tabs_highcolor = iupAttribGetStr(ih, "TABSHIGHCOLOR");
  int img_position = iupFlatGetImagePosition(iupAttribGetStr(ih, "TABSIMAGEPOSITION"));
  int active = IupGetInt(ih, "ACTIVE");  /* native implementation */
  int spacing = iupAttribGetInt(ih, "TABSIMAGESPACING");
  int horiz_padding, vert_padding;
  int show_lines = iupAttribGetBoolean(ih, "SHOWLINES");
  IdrawCanvas* dc = iupdrvDrawCreateCanvas(ih);
  int title_height = iFlatTabsGetTitleHeight(ih, NULL);
  int fixedwidth = iupAttribGetInt(ih, "FIXEDWIDTH");
  Ihandle* child;
  int pos, horiz_alignment, vert_alignment, x = 0;
  unsigned char line_r = 0, line_g = 0, line_b = 0;
  int show_close = iupAttribGetBoolean(ih, "SHOWCLOSE");

  iupdrvDrawParentBackground(dc);

  /* draw child area background */
  iupFlatDrawBox(dc, 0, ih->currentwidth - 1,
                 title_height, ih->currentheight - 1, bgcolor, NULL, 1);

  IupGetIntInt(ih, "TABSPADDING", &horiz_padding, &vert_padding);
  iFlatGetAlignment(ih, &horiz_alignment, &vert_alignment);

  if (show_lines)
  {
    char* title_line_color = iupAttribGetStr(ih, "TABSLINECOLOR");
    iupStrToRGB(title_line_color, &line_r, &line_g, &line_b);
  }

  iFlatTabsSetFont(ih);

  for (pos = 0, child = ih->firstchild; child; child = child->brother, pos++)
  {
    int tabvisible = iupAttribGetBooleanId(ih, "TABVISIBLE", pos);
    if (tabvisible)
    {
      char* tab_image = iupAttribGetId(ih, "TABIMAGE", pos);
      char* tab_title = iupAttribGetId(ih, "TABTITLE", pos);
      char* tab_backcolor = iupAttribGetId(ih, "TABBACKCOLOR", pos);
      char* tab_forecolor = iupAttribGetId(ih, "TABFORECOLOR", pos);
      char* tab_highcolor = iupAttribGetId(ih, "TABHIGHCOLOR", pos);
      int w, tab_active;
      char* background_color;
      char* foreground_color;
      int icon_width;

      if (!active)
        tab_active = active;
      else
        tab_active = iupAttribGetBooleanId(ih, "TABACTIVE", pos);

      if (fixedwidth)
        w = fixedwidth;
      else
      {
        int h;
        iFlatTabsGetIconSize(ih, pos, &w, &h);
        w += 2 * horiz_padding;

        if (show_close)
          w += ITABS_CLOSE_SIZE + ITABS_CLOSE_SPACING + ITABS_CLOSE_BORDER;
      }

      if (current_child == child)
      {
        /* current tab is always drawn with these colors */
        background_color = bgcolor;
        foreground_color = forecolor;
      }
      else
      {
        int tab_highlighted = iupAttribGetInt(ih, "_IUPFTABS_HIGHLIGHTED");

        /* other tabs are drawn with these colors */
        background_color = tabs_bgcolor;
        if (tab_backcolor)
          background_color = tab_backcolor;

        foreground_color = tabs_forecolor;
        if (tab_forecolor)
          foreground_color = tab_forecolor;

        if (pos == tab_highlighted)
        {
          if (highcolor)
            foreground_color = highcolor;
          else
            foreground_color = forecolor;

          if (tabs_highcolor || tab_highcolor)
          {
            if (tab_highcolor)
              background_color = tab_highcolor;
            else
              background_color = tabs_highcolor;
          }
        }
      }

      /* draw title background */
      iupFlatDrawBox(dc, x, x + w, 0, title_height, background_color, NULL, 1);

      if (show_lines)
      {
        if (current_child == child)
        {
          iupdrvDrawLine(dc, x, 0, x + w - 1, 0, line_r, line_g, line_b, IUP_DRAW_STROKE); /* tab top horizontal */
          iupdrvDrawLine(dc, x, 0, x, title_height - 1, line_r, line_g, line_b, IUP_DRAW_STROKE); /* tab left vertical */
          iupdrvDrawLine(dc, x + w - 1, 0, x + w - 1, title_height - 1, line_r, line_g, line_b, IUP_DRAW_STROKE); /* tab right vertical */
        }
        else
          iupdrvDrawLine(dc, x, title_height - 1, x + w - 1, title_height - 1, line_r, line_g, line_b, IUP_DRAW_STROKE); /* tab bottom horizontal */
      }

      icon_width = w;
      if (show_close)
        icon_width -= ITABS_CLOSE_SIZE + ITABS_CLOSE_SPACING + ITABS_CLOSE_BORDER;

      iFlatTabsSetTabFont(ih, pos);

      iupFlatDrawIcon(ih, dc, x, 0,
                      icon_width, title_height,
                      img_position, spacing, horiz_alignment, vert_alignment, horiz_padding, vert_padding,
                      tab_image, 0, tab_title, foreground_color, background_color, tab_active);

      iFlatTabsResetTabFont(ih);

      if (show_close)
      {
        int close_x = x + w - ITABS_CLOSE_BORDER - ITABS_CLOSE_SIZE;
        int close_y = (title_height - (ITABS_CLOSE_SIZE)) / 2;
        char* imagename = iupAttribGetStr(ih, "CLOSEIMAGE");
        int tab_close_high = iupAttribGetInt(ih, "_IUPFTABS_CLOSEHIGH");
        int tab_close_press = iupAttribGetInt(ih, "_IUPFTABS_CLOSEPRESS");

        if (pos == tab_close_press)
          imagename = iupAttribGetStr(ih, "CLOSEIMAGEPRESS");
        else if (pos == tab_close_high)
        {
          if (tabs_highcolor || tab_highcolor)
          {
            if (tab_highcolor)
              background_color = tab_highcolor;
            else
              background_color = tabs_highcolor;
          }
          else
            background_color = iupAttribGetStr(ih, "CLOSEHIGHCOLOR");

          iupFlatDrawBox(dc, close_x - ITABS_CLOSE_BORDER, close_x + ITABS_CLOSE_SIZE + ITABS_CLOSE_BORDER, close_y - ITABS_CLOSE_BORDER, close_y + ITABS_CLOSE_SIZE + ITABS_CLOSE_BORDER, background_color, NULL, 1);
          imagename = iupAttribGetStr(ih, "CLOSEIMAGEHIGH");
        }

        iupdrvDrawImage(dc, imagename, !tab_active, close_x, close_y);
      }

      // goto next tab area
      x += w;
      if (x > ih->currentwidth)
        break;
    }
  }

  iFlatTabsResetFont(ih);

  /* draw title free background */
  if (x < ih->currentwidth)
  {
    iupFlatDrawBox(dc, x, ih->currentwidth - 1, 0, title_height, tabs_bgcolor, NULL, 1);

    /* free area bottom line */
    if (show_lines)
      iupdrvDrawLine(dc, x, title_height - 1, ih->currentwidth - 1, title_height - 1, line_r, line_g, line_b, IUP_DRAW_STROKE);
  }

  /* lines around children */
  if (show_lines)
  {
    iupdrvDrawLine(dc, 0, title_height, 0, ih->currentheight - 1, line_r, line_g, line_b, IUP_DRAW_STROKE); /* left vertical */
    iupdrvDrawLine(dc, ih->currentwidth - 1, title_height, ih->currentwidth - 1, ih->currentheight - 1, line_r, line_g, line_b, IUP_DRAW_STROKE); /* right vertical */
    iupdrvDrawLine(dc, 0, ih->currentheight - 1, ih->currentwidth - 1, ih->currentheight - 1, line_r, line_g, line_b, IUP_DRAW_STROKE); /* bottom horizontal */
  }

  iupdrvDrawFlush(dc);

  iupdrvDrawKillCanvas(dc);

  return IUP_DEFAULT;
}

static void iFlatTabsCheckCurrentTab(Ihandle* ih, Ihandle* check_child, int pos, int removed)
{
  Ihandle* current_child = iFlatTabsGetCurrentTab(ih);
  if (current_child == check_child)
  {
    int p;
    Ihandle* child;

    /* if given tab is the current tab,
    then the current tab must be changed to a visible tab */

    /* this function is called after the child has being removed from the hierarchy,
    but before the system tab being removed. */

    p = 0;
    if (removed && p == pos)
      p++;

    for (child = ih->firstchild; child; child = child->brother)
    {
      if (p != pos && iupAttribGetBooleanId(ih, "TABVISIBLE", p))
      {
        iFlatTabsSetCurrentTab(ih, child);
        return;
      }

      p++;
      if (removed && p == pos)
        p++;  /* increment twice to compensate for child already removed */
    }
  }
}

static int iFlatTabsFindTab(Ihandle* ih, int cur_x, int cur_y, int show_close, int *inside_close)
{
  int title_height = iFlatTabsGetTitleHeight(ih, NULL);

  *inside_close = 0;

  if (cur_y < title_height)
  {
    Ihandle* child;
    int pos, horiz_padding, vert_padding, x = 0;
    int fixedwidth = iupAttribGetInt(ih, "FIXEDWIDTH");

    IupGetIntInt(ih, "TABSPADDING", &horiz_padding, &vert_padding);

    iFlatTabsSetFont(ih);

    for (pos = 0, child = ih->firstchild; child; child = child->brother, pos++)
    {
      int tabvisible = iupAttribGetBooleanId(ih, "TABVISIBLE", pos);
      if (tabvisible)
      {
        int w;

        if (fixedwidth)
          w = fixedwidth;
        else
        {
          int h;
          iFlatTabsGetIconSize(ih, pos, &w, &h);
          w += 2 * horiz_padding;

          if (show_close)
            w += ITABS_CLOSE_SIZE + ITABS_CLOSE_SPACING + ITABS_CLOSE_BORDER;
        }

        if (cur_x > x && cur_x < x + w)
        {
          if (show_close)
          {
            int close_end = x + w - ITABS_CLOSE_BORDER;
            int close_start = close_end - ITABS_CLOSE_SIZE;
            if (cur_x >= close_start && cur_x <= close_end)
              *inside_close = 1;
          }

          iFlatTabsResetFont(ih);
          return pos;
        }

        x += w;

        if (x > ih->currentwidth)
          break;
      }
    }

    iFlatTabsResetFont(ih);
  }

  return -1;
}


/*****************************************************************************************/

static int iFlatTabsButton_CB(Ihandle* ih, int button, int pressed, int x, int y, char* status)
{
  IFniiiis cb = (IFniiiis)IupGetCallback(ih, "FLAT_BUTTON_CB");
  if (cb)
  {
    if (cb(ih, button, pressed, x, y, status) == IUP_IGNORE)
      return IUP_DEFAULT;
  }

  if (button == IUP_BUTTON1 && pressed)
  {
    int inside_close;
    int show_close = iupAttribGetBoolean(ih, "SHOWCLOSE");
    int tab_found = iFlatTabsFindTab(ih, x, y, show_close, &inside_close);
    if (tab_found != -1 && iupAttribGetBooleanId(ih, "TABACTIVE", tab_found))
    {
      if (show_close && inside_close)
      {
        iupAttribSetInt(ih, "_IUPFTABS_CLOSEPRESS", tab_found);  /* used for press feedback */
        iupdrvRedrawNow(ih);
      }
      else
      {
        Ihandle* child = IupGetChild(ih, tab_found);
        Ihandle* prev_child = iFlatTabsGetCurrentTab(ih);
        if (prev_child != child)
        {
          IFnnn cb = (IFnnn)IupGetCallback(ih, "TABCHANGE_CB");
          int ret = IUP_DEFAULT;

          if (cb)
            ret = cb(ih, child, prev_child);
          else
          {
            IFnii cb2 = (IFnii)IupGetCallback(ih, "TABCHANGEPOS_CB");
            if (cb2)
            {
              int pos = tab_found;
              int prev_pos = IupGetChildPos(ih, prev_child);
              ret = cb2(ih, pos, prev_pos);
            }
          }

          if (ret == IUP_DEFAULT)
            iFlatTabsSetCurrentTab(ih, child);
        }

        iupAttribSetInt(ih, "_IUPFTABS_CLOSEPRESS", -1);
      }
    }
  }
  else if (button == IUP_BUTTON1 && !pressed)
  {
    int show_close = iupAttribGetBoolean(ih, "SHOWCLOSE");
    if (show_close)
    {
      int tab_close_press = iupAttribGetInt(ih, "_IUPFTABS_CLOSEPRESS");
      int inside_close;
      int tab_found = iFlatTabsFindTab(ih, x, y, show_close, &inside_close);

      iupAttribSetInt(ih, "_IUPFTABS_CLOSEPRESS", -1);

      if (tab_found != -1 && iupAttribGetBooleanId(ih, "TABACTIVE", tab_found) && inside_close && tab_close_press == tab_found)
      {
        Ihandle *child = IupGetChild(ih, tab_found);
        if (child)
        {
          int ret = IUP_DEFAULT;
          IFni cb = (IFni)IupGetCallback(ih, "TABCLOSE_CB");
          if (cb)
            ret = cb(ih, tab_found);

          if (ret == IUP_CONTINUE) /* destroy tab and children */
          {
            IupDestroy(child);
            IupRefreshChildren(ih);
            iupdrvRedrawNow(ih);
          }
          else if (ret == IUP_DEFAULT) /* hide tab and children */
            IupSetAttributeId(ih, "TABVISIBLE", tab_found, "No");
        }
      }
    }
  }
  else if (button == IUP_BUTTON3 && pressed)
  {
    IFni cb = (IFni)IupGetCallback(ih, "RIGHTCLICK_CB");
    if (cb)
    {
      int show_close = iupAttribGetBoolean(ih, "SHOWCLOSE");
      int inside_close;
      int tab_found = iFlatTabsFindTab(ih, x, y, show_close, &inside_close);
      if (tab_found != -1 && iupAttribGetBooleanId(ih, "TABACTIVE", tab_found))
        cb(ih, tab_found);
    }
  }

  return IUP_DEFAULT;
}

static int iFlatTabsMotion_CB(Ihandle *ih, int x, int y, char *status)
{
  int tab_found, tab_highlighted, redraw = 0;
  int inside_close, show_close, tab_active;

  IFniis cb = (IFniis)IupGetCallback(ih, "FLAT_MOTION_CB");
  if (cb)
  {
    if (cb(ih, x, y, status) == IUP_IGNORE)
      return IUP_DEFAULT;
  }

  show_close = iupAttribGetBoolean(ih, "SHOWCLOSE");
  tab_highlighted = iupAttribGetInt(ih, "_IUPFTABS_HIGHLIGHTED");
  tab_found = iFlatTabsFindTab(ih, x, y, show_close, &inside_close);

  tab_active = 1;
  if (tab_found != -1)
    tab_active = iupAttribGetBooleanId(ih, "TABACTIVE", tab_found);

  if (!tab_active)
    return IUP_DEFAULT;

  if (tab_found != tab_highlighted && !inside_close)
  {
    iupAttribSetInt(ih, "_IUPFTABS_HIGHLIGHTED", tab_found);
    redraw = 1;
  }

  if (show_close)
  {
    int tab_close_high, tab_close_press;

    tab_close_high = iupAttribGetInt(ih, "_IUPFTABS_CLOSEHIGH");
    if (inside_close)
    {
      if (tab_close_high != tab_found)
      {
        iupAttribSetInt(ih, "_IUPFTABS_HIGHLIGHTED", -1);
        iupAttribSetInt(ih, "_IUPFTABS_CLOSEHIGH", tab_found);
        redraw = 1;
      }
    }
    else
    {
      if (tab_close_high != -1)
      {
        iupAttribSetInt(ih, "_IUPFTABS_CLOSEHIGH", -1);
        redraw = 1;
      }
    }

    tab_close_press = iupAttribGetInt(ih, "_IUPFTABS_CLOSEPRESS");
    if (tab_close_press != -1 && !inside_close)
    {
      iupAttribSetInt(ih, "_IUPFTABS_CLOSEPRESS", -1);
      redraw = 1;
    }
  }

  if (redraw)
    iupdrvRedrawNow(ih);

  return IUP_DEFAULT;
}

static int iFlatTabsLeaveWindow_CB(Ihandle* ih)
{
  int tab_highlighted, tab_close_high, redraw = 0;

  IFn cb = (IFn)IupGetCallback(ih, "FLAT_LEAVEWINDOW_CB");
  if (cb)
  {
    if (cb(ih) == IUP_IGNORE)
      return IUP_DEFAULT;
  }

  tab_highlighted = iupAttribGetInt(ih, "_IUPFTABS_HIGHLIGHTED");
  if (tab_highlighted != -1)
  {
    iupAttribSetInt(ih, "_IUPFTABS_HIGHLIGHTED", -1);
    redraw = 1;
  }

  tab_close_high = iupAttribGetInt(ih, "_IUPFTABS_CLOSEHIGH");
  if (tab_close_high != -1)
  {
    iupAttribSetInt(ih, "_IUPFTABS_CLOSEHIGH", -1);
    redraw = 1;
  }

  if (redraw)
    iupdrvRedrawNow(ih);

  return IUP_DEFAULT;
}


/*****************************************************************************************/


static int iFlatTabsSetValueHandleAttrib(Ihandle* ih, const char* value)
{
  Ihandle* current_child;
  Ihandle *child = (Ihandle*)value;

  if (!iupObjectCheck(child))
    return 0;

  current_child = iFlatTabsGetCurrentTab(ih);
  if (current_child != child)
  {
    int pos = IupGetChildPos(ih, child);
    if (pos != -1) /* found child */
      iFlatTabsSetCurrentTab(ih, child);
  }

  return 0;
}

static char* iFlatTabsGetValueHandleAttrib(Ihandle* ih)
{
  return iupAttribGet(ih, "_IUPFTABS_VALUE_HANDLE");
}

static char* iFlatTabsGetCountAttrib(Ihandle* ih)
{
  return iupStrReturnInt(IupGetChildCount(ih));
}

static int iFlatTabsSetValuePosAttrib(Ihandle* ih, const char* value)
{
  Ihandle* child, *current_child;
  int pos;

  if (!iupStrToInt(value, &pos))
    return 0;

  child = IupGetChild(ih, pos);
  current_child = iFlatTabsGetCurrentTab(ih);
  if (child && current_child != child)  /* found child and NOT current */
    iFlatTabsSetCurrentTab(ih, child);

  return 0;
}

static char* iFlatTabsGetValuePosAttrib(Ihandle* ih)
{
  Ihandle* current_child = iFlatTabsGetCurrentTab(ih);
  int pos = IupGetChildPos(ih, current_child);
  if (pos != -1) /* found child */
    return iupStrReturnInt(pos);
  return NULL;
}

static int iFlatTabsSetValueAttrib(Ihandle* ih, const char* value)
{
  Ihandle *child;

  if (!value)
    return 0;

  child = IupGetHandle(value);
  if (!child)
    return 0;

  iFlatTabsSetValueHandleAttrib(ih, (char*)child);

  return 0;
}

static char* iFlatTabsGetValueAttrib(Ihandle* ih)
{
  Ihandle* child = (Ihandle*)iFlatTabsGetValueHandleAttrib(ih);
  return IupGetName(child);
}

static char* iFlatTabsGetExtraBoxHandleAttrib(Ihandle* ih)
{
  return iupAttribGet(ih, "_IUPFTABS_EXTRABOX");
}

static char* iFlatTabsGetExtraBoxAttrib(Ihandle* ih)
{
  Ihandle* child = (Ihandle*)iFlatTabsGetExtraBoxHandleAttrib(ih);
  return IupGetName(child);
}

static int iFlatTabsSetTabVisibleAttrib(Ihandle* ih, int pos, const char* value)
{
  Ihandle* child = IupGetChild(ih, pos);
  if (child)
  {
    if (!iupStrBoolean(value))
      iFlatTabsCheckCurrentTab(ih, child, pos, 0);
  }

  IupUpdate(ih);
  return 1;
}

static char* iFlatTabsGetClientSizeAttrib(Ihandle* ih)
{
  int width = ih->currentwidth;
  int height = ih->currentheight;

  height -= iFlatTabsGetTitleHeight(ih, NULL);

  if (iupAttribGetBoolean(ih, "SHOWLINES"))
  {
    height -= 1;
    width -= 2;
  }

  if (width < 0) width = 0;
  if (height < 0) height = 0;

  return iupStrReturnIntInt(width, height, 'x');
}

static char* iFlatTabsGetBgColorAttrib(Ihandle* ih)
{
  if (iupAttribGet(ih, "BGCOLOR"))
    return NULL;  /* get from the hash table */
  else
    return "255 255 255";
}

static int iFlatTabsSetActiveAttrib(Ihandle* ih, const char* value)
{
  IupUpdate(ih);
  return iupBaseSetActiveAttrib(ih, value);
}

static int iFlatTabsUpdateSetAttrib(Ihandle* ih, const char* value)
{
  (void)value;
  IupUpdate(ih);
  return 1;
}

static int iFlatTabsSetTabFontStyleAttrib(Ihandle* ih, int id, const char* value)
{
  int size = 0;
  int is_bold = 0,
    is_italic = 0,
    is_underline = 0,
    is_strikeout = 0;
  char typeface[1024];
  char* font;

  if (!value)
    return 0;

  font = iupAttribGetId(ih, "TABFONT", id);
  if (!font)
    font = iupAttribGet(ih, "TABSFONT");
  if (!font)
    font = IupGetAttribute(ih, "FONT");

  if (!iupGetFontInfo(font, typeface, &size, &is_bold, &is_italic, &is_underline, &is_strikeout))
    return 0;

  IupSetfAttributeId(ih, "TABFONT", id, "%s, %s %d", typeface, value, size);

  return 0;
}

static char* iFlatTabsGetTabFontStyleAttrib(Ihandle* ih, int id)
{
  int size = 0;
  int is_bold = 0,
    is_italic = 0,
    is_underline = 0,
    is_strikeout = 0;
  char typeface[1024];

  char* font = iupAttribGetId(ih, "TABFONT", id);
  if (!font)
    font = iupAttribGet(ih, "TABSFONT");
  if (!font)
    font = IupGetAttribute(ih, "FONT");

  if (!iupGetFontInfo(font, typeface, &size, &is_bold, &is_italic, &is_underline, &is_strikeout))
    return NULL;

  return iupStrReturnStrf("%s%s%s%s", is_bold ? "Bold " : "", is_italic ? "Italic " : "", is_underline ? "Underline " : "", is_strikeout ? "Strikeout " : "");
}

static int iFlatTabsSetTabFontSizeAttrib(Ihandle* ih, int id, const char* value)
{
  int size = 0;
  int is_bold = 0,
    is_italic = 0,
    is_underline = 0,
    is_strikeout = 0;
  char typeface[1024];
  char* font;

  if (!value)
    return 0;

  font = iupAttribGetId(ih, "TABFONT", id);
  if (!font)
    font = iupAttribGet(ih, "TABSFONT");
  if (!font)
    font = IupGetAttribute(ih, "FONT");

  if (!iupGetFontInfo(font, typeface, &size, &is_bold, &is_italic, &is_underline, &is_strikeout))
    return 0;

  IupSetfAttributeId(ih, "TABFONT", id, "%s, %s %d", typeface, is_bold ? "Bold " : "", is_italic ? "Italic " : "", is_underline ? "Underline " : "", is_strikeout ? "Strikeout " : "", value);

  return 0;
}

static char* iFlatTabsGetTabFontSizeAttrib(Ihandle* ih, int id)
{
  int size = 0;
  int is_bold = 0,
    is_italic = 0,
    is_underline = 0,
    is_strikeout = 0;
  char typeface[1024];

  char* font = iupAttribGetId(ih, "TABFONT", id);
  if (!font)
    font = iupAttribGet(ih, "TABSFONT");
  if (!font)
    font = IupGetAttribute(ih, "FONT");

  if (!iupGetFontInfo(font, typeface, &size, &is_bold, &is_italic, &is_underline, &is_strikeout))
    return NULL;

  return iupStrReturnInt(size);
}

static char* iFlatTabsGetTabsFontSizeAttrib(Ihandle* ih)
{
  int size = 0;
  int is_bold = 0,
    is_italic = 0,
    is_underline = 0,
    is_strikeout = 0;
  char typeface[1024];

  char* font = iupAttribGet(ih, "TABSFONT");
  if (!font)
    font = IupGetAttribute(ih, "FONT");

  if (!iupGetFontInfo(font, typeface, &size, &is_bold, &is_italic, &is_underline, &is_strikeout))
    return NULL;

  return iupStrReturnInt(size);
}

static int iFlatTabsSetTabsFontSizeAttrib(Ihandle* ih, const char* value)
{
  int size = 0;
  int is_bold = 0,
    is_italic = 0,
    is_underline = 0,
    is_strikeout = 0;
  char typeface[1024];
  char* font;

  if (!value)
    return 0;

  font = iupAttribGet(ih, "TABSFONT");
  if (!font)
    font = IupGetAttribute(ih, "FONT");

  if (!iupGetFontInfo(font, typeface, &size, &is_bold, &is_italic, &is_underline, &is_strikeout))
    return 0;

  IupSetfAttribute(ih, "TABSFONT", "%s, %s%s%s%s %s", typeface, is_bold ? "Bold " : "", is_italic ? "Italic " : "", is_underline ? "Underline " : "", is_strikeout ? "Strikeout " : "", value);
  return 0;
}

static char* iFlatTabsGetTabsFontStyleAttrib(Ihandle* ih)
{
  int size = 0;
  int is_bold = 0,
    is_italic = 0,
    is_underline = 0,
    is_strikeout = 0;
  char typeface[1024];

  char* font = iupAttribGet(ih, "TABSFONT");
  if (!font)
    font = IupGetAttribute(ih, "FONT");

  if (!iupGetFontInfo(font, typeface, &size, &is_bold, &is_italic, &is_underline, &is_strikeout))
    return NULL;

  return iupStrReturnStrf("%s%s%s%s", is_bold ? "Bold " : "", is_italic ? "Italic " : "", is_underline ? "Underline " : "", is_strikeout ? "Strikeout " : "");
}

static int iFlatTabsSetTabsFontStyleAttrib(Ihandle* ih, const char* value)
{
  int size = 0;
  int is_bold = 0,
    is_italic = 0,
    is_underline = 0,
    is_strikeout = 0;
  char typeface[1024];
  char* font;

  if (!value)
    return 0;

  font = iupAttribGet(ih, "TABSFONT");
  if (!font)
    font = IupGetAttribute(ih, "FONT");

  if (!iupGetFontInfo(font, typeface, &size, &is_bold, &is_italic, &is_underline, &is_strikeout))
    return 0;

  IupSetfAttribute(ih, "TABSFONT", "%s, %s %d", typeface, value, size);

  return 0;
}

static int iFlatTabsSetTabsFontAttrib(Ihandle* ih, const char* value)
{
  iupdrvSetFontAttrib(ih, value);
  return 1;
}



/*********************************************************************************/


static void iFlatTabsChildAddedMethod(Ihandle* ih, Ihandle* child)
{
#define CHILD_ATTRIB_COUNT 7
  const char* child_attrib[CHILD_ATTRIB_COUNT] = {
    "TABTITLE",
    "TABIMAGE",
    "TABVISIBLE",
    "TABACTIVE",
    "TABFORECOLOR",
    "TABBACKCOLOR",
    "TABFONT"
  };
  Ihandle* current_child;
  char* bgcolor;

  int i, pos = IupGetChildPos(ih, child);

  for (i = 0; i < CHILD_ATTRIB_COUNT; i++)
  {
    if (!iupAttribGetId(ih, child_attrib[i], pos))
    {
      /* transfer form child to Id based attribute */
      char* value = iupAttribGet(child, child_attrib[i]);
      if (value)
        iupAttribSetStrId(ih, child_attrib[i], pos, value);
      else if (iupStrEqual(child_attrib[i], "TABVISIBLE") || iupStrEqual(child_attrib[i], "TABACTIVE")) 
        iupAttribSetStrId(ih, child_attrib[i], pos, "Yes");  /* ensure a default value */
    }
  }

  /* make sure it has at least one name */
  if (!iupAttribGetHandleName(child))
    iupAttribSetHandleName(child);

  bgcolor = iupAttribGetStr(ih, "BGCOLOR");
  iupAttribSetStr(child, "BGCOLOR", bgcolor);

  current_child = iFlatTabsGetCurrentTab(ih);
  if (!current_child)
    iFlatTabsSetCurrentTab(ih, child);
  else
    IupSetAttribute(child, "VISIBLE", "No");
}

static void iFlatTabsChildRemovedMethod(Ihandle* ih, Ihandle* child, int pos)
{
  iFlatTabsCheckCurrentTab(ih, child, pos, 1);
}

static void iFlatTabsComputeNaturalSizeMethod(Ihandle* ih, int *w, int *h, int *children_expand)
{
  Ihandle* child, *extra_box;
  int children_naturalwidth, children_naturalheight;

  /* calculate total children natural size (even for hidden children) */
  children_naturalwidth = 0;
  children_naturalheight = 0;

  for (child = ih->firstchild; child; child = child->brother)
  {
    /* update child natural size first */
    iupBaseComputeNaturalSize(child);

    *children_expand |= child->expand;
    children_naturalwidth = iupMAX(children_naturalwidth, child->naturalwidth);
    children_naturalheight = iupMAX(children_naturalheight, child->naturalheight);
  }

  *w = children_naturalwidth;
  *h = children_naturalheight + iFlatTabsGetTitleHeight(ih, NULL);

  if (iupAttribGetBoolean(ih, "SHOWLINES"))
  {
    *h += 1;
    *w += 2;
  }

  /* computed but not included in natural size */
  extra_box = (Ihandle*)iupAttribGet(ih, "_IUPFTABS_EXTRABOX");
  iupBaseComputeNaturalSize(extra_box);
}

static void iFlatTabsSetChildrenCurrentSizeMethod(Ihandle* ih, int shrink)
{
  Ihandle* child, *extra_box;
  int title_width;
  int title_height = iFlatTabsGetTitleHeight(ih, &title_width);
  int width = ih->currentwidth;
  int height = ih->currentheight - title_height;

  if (iupAttribGetBoolean(ih, "SHOWLINES"))
  {
    width -= 2;
    height -= 1;
  }

  if (width < 0) width = 0;
  if (height < 0) height = 0;

  for (child = ih->firstchild; child; child = child->brother)
  {
    child->currentwidth = width;
    child->currentheight = height;

    if (child->firstchild)
      iupClassObjectSetChildrenCurrentSize(child, shrink);
  }

  extra_box = (Ihandle*)iupAttribGet(ih, "_IUPFTABS_EXTRABOX");
  width = ih->currentwidth - title_width;
  if (width < 0) width = 0;
  height = title_height;
  iupBaseSetCurrentSize(extra_box, width, height, shrink);
}

static void iFlatTabsSetChildrenPositionMethod(Ihandle* ih, int x, int y)
{
  /* In all systems, each tab is a native window covering the client area.
     Child coordinates are relative to client left-top corner of the tab page. */
  Ihandle* child, *extra_box;
  char* offset = iupAttribGet(ih, "CHILDOFFSET");
  int title_width;

  /* Native container, position is reset */
  x = 0;
  y = 0;

  if (offset) iupStrToIntInt(offset, &x, &y, 'x');

  y += iFlatTabsGetTitleHeight(ih, &title_width);

  if (iupAttribGetBoolean(ih, "SHOWLINES"))
    x += 1;

  for (child = ih->firstchild; child; child = child->brother)
    iupBaseSetPosition(child, x, y);

  extra_box = (Ihandle*)iupAttribGet(ih, "_IUPFTABS_EXTRABOX");
  iupBaseSetPosition(extra_box, title_width, 0);
}

static int iFlatTabsCreateMethod(Ihandle* ih, void **params)
{
  Ihandle* extra_box;

  /* add children */
  if(params)
  {
    Ihandle** iparams = (Ihandle**)params;
    while (*iparams) 
    {
      IupAppend(ih, *iparams);
      iparams++;
    }
  }

  iupAttribSetInt(ih, "_IUPFTABS_HIGHLIGHTED", -1);
  iupAttribSetInt(ih, "_IUPFTABS_CLOSEHIGH", -1);
  iupAttribSetInt(ih, "_IUPFTABS_CLOSEPRESS", -1);

  IupSetCallback(ih, "ACTION", (Icallback)iFlatTabsRedraw_CB);
  IupSetCallback(ih, "BUTTON_CB", (Icallback)iFlatTabsButton_CB);
  IupSetCallback(ih, "MOTION_CB", (Icallback)iFlatTabsMotion_CB);
  IupSetCallback(ih, "LEAVEWINDOW_CB", iFlatTabsLeaveWindow_CB);

  extra_box = IupHbox(NULL);
  iupAttribSet(ih, "_IUPFTABS_EXTRABOX", (char*)extra_box);
  iupAttribSetHandleName(extra_box);

  return IUP_NOERROR;
}

Iclass* iupFlatTabsNewClass(void)
{
  Iclass* ic = iupClassNew(iupRegisterFindClass("canvas"));

  ic->name = "flattabs";
  ic->format = "g"; /* array of Ihandle */
  ic->nativetype = IUP_TYPECONTROL;
  ic->childtype  = IUP_CHILDMANY;
  ic->is_interactive = 1;
  ic->has_attrib_id = 1;

  /* Class functions */
  ic->New = iupFlatTabsNewClass;
  ic->Create = iFlatTabsCreateMethod;

  ic->ChildAdded = iFlatTabsChildAddedMethod;
  ic->ChildRemoved = iFlatTabsChildRemovedMethod;

  ic->ComputeNaturalSize = iFlatTabsComputeNaturalSizeMethod;
  ic->SetChildrenCurrentSize = iFlatTabsSetChildrenCurrentSizeMethod;
  ic->SetChildrenPosition = iFlatTabsSetChildrenPositionMethod;

  /* IupFlatTabs Callbacks */
  iupClassRegisterCallback(ic, "TABCHANGE_CB", "nn");
  iupClassRegisterCallback(ic, "TABCHANGEPOS_CB", "ii");
  iupClassRegisterCallback(ic, "RIGHTCLICK_CB", "i");
  iupClassRegisterCallback(ic, "TABCLOSE_CB", "i");

  iupClassRegisterCallback(ic, "FLAT_BUTTON_CB", "iiiis");
  iupClassRegisterCallback(ic, "FLAT_MOTION_CB", "iis");
  iupClassRegisterCallback(ic, "FLAT_LEAVEWINDOW_CB", "");

  /* Base Container */
  iupClassRegisterAttribute(ic, "EXPAND", iupBaseContainerGetExpandAttrib, NULL, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NOT_MAPPED | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "CLIENTOFFSET", iupBaseGetClientOffsetAttrib, NULL, NULL, NULL, IUPAF_NOT_MAPPED | IUPAF_READONLY | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "CLIENTSIZE", iFlatTabsGetClientSizeAttrib, NULL, NULL, NULL, IUPAF_READONLY | IUPAF_NOT_MAPPED | IUPAF_NO_INHERIT);

  /* Native Container */
  iupClassRegisterAttribute(ic, "CHILDOFFSET", NULL, NULL, NULL, NULL, IUPAF_NOT_MAPPED | IUPAF_NO_INHERIT);

  /* replace IupCanvas behavior */
  iupClassRegisterReplaceAttribDef(ic, "BORDER", "NO", NULL);
  iupClassRegisterReplaceAttribFlags(ic, "BORDER", IUPAF_READONLY|IUPAF_NO_INHERIT);
  iupClassRegisterReplaceAttribDef(ic, "SCROLLBAR", "NO", NULL);
  iupClassRegisterReplaceAttribFlags(ic, "SCROLLBAR", IUPAF_READONLY | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "CANFOCUS", NULL, NULL, IUPAF_SAMEASSYSTEM, "NO", IUPAF_NO_INHERIT);

  iupClassRegisterReplaceAttribFunc(ic, "ACTIVE", NULL, iFlatTabsSetActiveAttrib);

  /* IupFlatTabs only */
  iupClassRegisterAttribute(ic, "VALUE", iFlatTabsGetValueAttrib, iFlatTabsSetValueAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "VALUEPOS", iFlatTabsGetValuePosAttrib, iFlatTabsSetValuePosAttrib, IUPAF_SAMEASSYSTEM, "0", IUPAF_NO_SAVE|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "VALUE_HANDLE", iFlatTabsGetValueHandleAttrib, iFlatTabsSetValueHandleAttrib, NULL, NULL, IUPAF_NOT_MAPPED | IUPAF_NO_INHERIT | IUPAF_IHANDLE | IUPAF_NO_STRING);
  iupClassRegisterAttribute(ic, "COUNT", iFlatTabsGetCountAttrib, NULL, NULL, NULL, IUPAF_READONLY|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "FIXEDWIDTH", NULL, iFlatTabsUpdateSetAttrib, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "EXTRABOX", iFlatTabsGetExtraBoxAttrib, NULL, NULL, NULL, IUPAF_IHANDLENAME | IUPAF_READONLY | IUPAF_NOT_MAPPED | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "EXTRABOX_HANDLE", iFlatTabsGetExtraBoxHandleAttrib, NULL, NULL, NULL, IUPAF_IHANDLE | IUPAF_READONLY | IUPAF_NOT_MAPPED | IUPAF_NO_INHERIT);

  /* IupFlatTabs Child only */
  iupClassRegisterAttributeId(ic, "TABTITLE", NULL, (IattribSetIdFunc)iFlatTabsUpdateSetAttrib, IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "TABIMAGE", NULL, (IattribSetIdFunc)iFlatTabsUpdateSetAttrib, IUPAF_IHANDLENAME | IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "TABVISIBLE", NULL, iFlatTabsSetTabVisibleAttrib, IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "TABACTIVE", NULL, (IattribSetIdFunc)iFlatTabsUpdateSetAttrib, IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "TABFORECOLOR", NULL, (IattribSetIdFunc)iFlatTabsUpdateSetAttrib, IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "TABBACKCOLOR", NULL, (IattribSetIdFunc)iFlatTabsUpdateSetAttrib, IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "TABHIGHCOLOR", NULL, NULL, IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "TABFONT", NULL, (IattribSetIdFunc)iFlatTabsUpdateSetAttrib, IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "TABFONTSTYLE", iFlatTabsGetTabFontStyleAttrib, iFlatTabsSetTabFontStyleAttrib, IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "TABFONTSIZE", iFlatTabsGetTabFontSizeAttrib, iFlatTabsSetTabFontSizeAttrib, IUPAF_NO_INHERIT);

  /* Visual for current TAB */
  iupClassRegisterAttribute(ic, "BGCOLOR", iFlatTabsGetBgColorAttrib, iFlatTabsUpdateSetAttrib, IUPAF_SAMEASSYSTEM, "255 255 255", IUPAF_DEFAULT);   /* inherited */
  iupClassRegisterAttribute(ic, "FORECOLOR", NULL, iFlatTabsUpdateSetAttrib, IUPAF_SAMEASSYSTEM, "50 150 255", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "HIGHCOLOR", NULL, NULL, NULL, NULL, IUPAF_NO_INHERIT);

  /* Visual for other TABS */
  iupClassRegisterAttribute(ic, "TABSFORECOLOR", NULL, iFlatTabsUpdateSetAttrib, IUPAF_SAMEASSYSTEM, "DLGFGCOLOR", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "TABSBACKCOLOR", NULL, iFlatTabsUpdateSetAttrib, IUPAF_SAMEASSYSTEM, "DLGBGCOLOR", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "TABSHIGHCOLOR", NULL, NULL, NULL, NULL, IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "TABSFONT", NULL, iFlatTabsSetTabsFontAttrib, NULL, NULL, IUPAF_NOT_MAPPED | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "TABSFONTSTYLE", iFlatTabsGetTabsFontStyleAttrib, iFlatTabsSetTabsFontStyleAttrib, NULL, NULL, IUPAF_NO_SAVE | IUPAF_NOT_MAPPED | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "TABSFONTSIZE", iFlatTabsGetTabsFontSizeAttrib, iFlatTabsSetTabsFontSizeAttrib, NULL, NULL, IUPAF_NO_SAVE | IUPAF_NOT_MAPPED | IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "SHOWLINES", NULL, iFlatTabsUpdateSetAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "TABSLINECOLOR", NULL, iFlatTabsUpdateSetAttrib, IUPAF_SAMEASSYSTEM, "180 180 180", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "TABSIMAGEPOSITION", NULL, iFlatTabsUpdateSetAttrib, IUPAF_SAMEASSYSTEM, "LEFT", IUPAF_NOT_MAPPED | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "TABSIMAGESPACING", NULL, iFlatTabsUpdateSetAttrib, IUPAF_SAMEASSYSTEM, "2", IUPAF_NOT_MAPPED | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "TABSALIGNMENT", NULL, iFlatTabsUpdateSetAttrib, "ACENTER:ACENTER", NULL, IUPAF_NOT_MAPPED | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "TABSPADDING", NULL, iFlatTabsUpdateSetAttrib, IUPAF_SAMEASSYSTEM, "10x10", IUPAF_NOT_MAPPED | IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "SHOWCLOSE", NULL, NULL, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "CLOSEIMAGE", NULL, iFlatTabsUpdateSetAttrib, IUPAF_SAMEASSYSTEM, "IMGFLATCLOSE", IUPAF_IHANDLENAME | IUPAF_NO_DEFAULTVALUE | IUPAF_NOT_MAPPED | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "CLOSEIMAGEHIGH", NULL, iFlatTabsUpdateSetAttrib, IUPAF_SAMEASSYSTEM, "IMGFLATCLOSEHIGH", IUPAF_IHANDLENAME | IUPAF_NO_DEFAULTVALUE | IUPAF_NOT_MAPPED | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "CLOSEIMAGEPRESS", NULL, iFlatTabsUpdateSetAttrib, IUPAF_SAMEASSYSTEM, "IMGFLATCLOSEPRESS", IUPAF_IHANDLENAME | IUPAF_NO_DEFAULTVALUE | IUPAF_NOT_MAPPED | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "CLOSEHIGHCOLOR", NULL, NULL, IUPAF_SAMEASSYSTEM, "200 220 245", IUPAF_NO_INHERIT);

  /* Default node images */
  if (!IupGetHandle("IMGFLATCLOSE"))
    iFlatTabsInitializeCloseImage();

  return ic;
}

Ihandle* IupFlatTabs(Ihandle* first,...)
{
  Ihandle **children;
  Ihandle *ih;

  va_list arglist;
  va_start(arglist, first);
  children = (Ihandle**)iupObjectGetParamList(first, arglist);
  va_end(arglist);

  ih = IupCreatev("flattabs", (void**)children);
  free(children);

  return ih;
}

Ihandle* IupFlatTabsv(Ihandle** params)
{
  return IupCreatev("flattabs", (void**)params);
}
