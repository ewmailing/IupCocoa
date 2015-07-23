/** \file
 * \brief Button Control
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <stdarg.h>

#include "iup.h"
#include "iupcbs.h"

#include "iup_object.h"
#include "iup_attrib.h"
#include "iup_str.h"
#include "iup_drv.h"
#include "iup_drvfont.h"
#include "iup_image.h"
#include "iup_stdcontrols.h"
#include "iup_register.h"
#include "iup_draw.h"
#include "iup_key.h"


/* from IupRadio implementation */
Ihandle *iupRadioFindToggleParent(Ihandle* ih_toggle);

enum{IUP_IMGPOS_LEFT, IUP_IMGPOS_RIGHT, IUP_IMGPOS_TOP, IUP_IMGPOS_BOTTOM};

struct _IcontrolData 
{
  /* attributes */
  int horiz_padding, vert_padding;  /* button margin */
  int spacing, img_position;        /* used when both text and image are displayed */
  int horiz_alignment, vert_alignment;  
  int border_width;

  /* aux */
  int has_focus,
    highlight,
    pressed;
};


/****************************************************************/


static void iFlatButtonDrawBorder(IdrawCanvas* dc, int xmin, int xmax, int ymin, int ymax, int border_width, const char* color, char* bgcolor, int active)
{
  unsigned char r = 0, g = 0, b = 0;

  if (!color || border_width == 0 || xmin == xmax || ymin == ymax)
    return;

  if (xmin > xmax) { int _t = xmin; xmin = xmax; xmax = _t; }
  if (ymin > ymax) { int _t = ymin; ymin = ymax; ymax = _t; }

  iupStrToRGB(color, &r, &g, &b);
  if (!active)
  {
    unsigned char bg_r = 0, bg_g = 0, bg_b = 0;
    iupStrToRGB(bgcolor, &bg_r, &bg_g, &bg_b);
    iupImageColorMakeInactive(&r, &g, &b, bg_r, bg_g, bg_b);
  }

  iupDrawRectangle(dc, xmin, ymin, xmax, ymax, r, g, b, IUP_DRAW_STROKE);
  while (border_width > 1)
  {
    border_width--;
    iupDrawRectangle(dc, xmin + border_width, 
                         ymin + border_width, 
                         xmax - border_width, 
                         ymax - border_width, r, g, b, IUP_DRAW_STROKE);
  } 
}

static void iFlatButtonDrawBox(IdrawCanvas* dc, int xmin, int xmax, int ymin, int ymax, const char* color, char* bgcolor, int active)
{
  unsigned char r = 0, g = 0, b = 0;

  if (!color || xmin == xmax || ymin == ymax)
    return;

  if (xmin > xmax) { int _t = xmin; xmin = xmax; xmax = _t; }
  if (ymin > ymax) { int _t = ymin; ymin = ymax; ymax = _t; }

  iupStrToRGB(color, &r, &g, &b);
  if (!active)
  {
    unsigned char bg_r = 0, bg_g = 0, bg_b = 0;
    iupStrToRGB(bgcolor, &bg_r, &bg_g, &bg_b);
    iupImageColorMakeInactive(&r, &g, &b, bg_r, bg_g, bg_b);
  }

  iupDrawRectangle(dc, xmin, ymin, xmax, ymax, r, g, b, IUP_DRAW_FILL);
}

static char* iFlatButtonMakeImageName(Ihandle* ih, const char* baseattrib, const char* state)
{
  char attrib[1024];
  strcpy(attrib, baseattrib);
  strcat(attrib, state);
  return iupAttribGet(ih, attrib);
}

static const char* iFlatButtonGetImageName(Ihandle* ih, const char* baseattrib, const char* basename, int active, int *make_inactive)
{
  const char* imagename = NULL;

  *make_inactive = 0;

  if (baseattrib)
  {
    if (active)
    {
      if (ih->data->pressed)
        imagename = iFlatButtonMakeImageName(ih, baseattrib, "PRESS");
      else
      {
        if (ih->data->highlight)
          imagename = iFlatButtonMakeImageName(ih, baseattrib, "HIGHLIGHT");
      }
    }
    else
    {
      imagename = iFlatButtonMakeImageName(ih, baseattrib, "INACTIVE");
      if (!imagename)
        *make_inactive = 1;
    }
  }

  if (imagename)
    return imagename;
  else
    return basename;
}

static void iFlatButtonDrawImage(Ihandle* ih, IdrawCanvas* dc, int x, int y, const char* baseattrib, const char* imagename, int active)
{
  int img_w, img_h;
  int make_inactive;
  const char* name = iFlatButtonGetImageName(ih, baseattrib, imagename, active, &make_inactive);
  iupDrawImage(dc, name, make_inactive, x, y, &img_w, &img_h);
}

void iFlatButtonDrawText(Ihandle* ih, IdrawCanvas* dc, int x, int y, const char* str, const char* color, const char* bgcolor, int active)
{
  unsigned char r = 0, g = 0, b = 0;

  if (!color || !str || str[0] == 0)
    return;

  iupStrToRGB(color, &r, &g, &b);
  if (!active)
  {
    unsigned char bg_r = 0, bg_g = 0, bg_b = 0;
    iupStrToRGB(bgcolor, &bg_r, &bg_g, &bg_b);
    iupImageColorMakeInactive(&r, &g, &b, bg_r, bg_g, bg_b);
  }

  iupDrawText(dc, str, (int)strlen(str), x, y, r, g, b, IupGetAttribute(ih, "FONT"));
}

static void iFlatButtonGetIconPosition(Ihandle* ih, int icon_width, int icon_height, int *x, int *y, int width, int height)
{
  if (ih->data->horiz_alignment == IUP_ALIGN_ARIGHT)
    *x = icon_width - (width + 2 * ih->data->horiz_padding);
  else if (ih->data->horiz_alignment == IUP_ALIGN_ACENTER)
    *x = (icon_width - (width + 2 * ih->data->horiz_padding)) / 2;
  else  /* ALEFT */
    *x = 0;

  if (ih->data->vert_alignment == IUP_ALIGN_ABOTTOM)
    *y = icon_height - (height + 2 * ih->data->vert_padding);
  else if (ih->data->vert_alignment == IUP_ALIGN_ACENTER)
    *y = (icon_height - (height + 2 * ih->data->vert_padding)) / 2;
  else  /* ATOP */
    *y = 0;

  *x += ih->data->horiz_padding;
  *y += ih->data->vert_padding;
}

static void iFlatButtonGetImageTextPosition(int x, int y, int img_position, int spacing,
                                            int img_width, int img_height, int txt_width, int txt_height,
                                            int *img_x, int *img_y, int *txt_x, int *txt_y)
{
  switch (img_position)
  {
  case IUP_IMGPOS_TOP:
    *img_y = y;
    *txt_y = y + img_height + spacing;
    if (img_width > txt_width)
    {
      *img_x = x;
      *txt_x = x + (img_width - txt_width) / 2;
    }
    else
    {
      *img_x = x + (txt_width - img_width) / 2;
      *txt_x = x;
    }
    break;
  case IUP_IMGPOS_BOTTOM:
    *img_y = y + txt_height + spacing;
    *txt_y = y;
    if (img_width > txt_width)
    {
      *img_x = x;
      *txt_x = x + (img_width - txt_width) / 2;
    }
    else
    {
      *img_x = x + (txt_width - img_width) / 2;
      *txt_x = x;
    }
    break;
  case IUP_IMGPOS_RIGHT:
    *img_x = x + txt_width + spacing;
    *txt_x = x;
    if (img_height > txt_height)
    {
      *img_y = y;
      *txt_y = y + (img_height - txt_height) / 2;
    }
    else
    {
      *img_y = y + (txt_height - img_height) / 2;
      *txt_y = y;
    }
    break;
  default: /* IUP_IMGPOS_LEFT (image at left of text) */
    *img_x = x;
    *txt_x = x + img_width + spacing;
    if (img_height > txt_height)
    {
      *img_y = y;
      *txt_y = y + (img_height - txt_height) / 2;
    }
    else
    {
      *img_y = y + (txt_height - img_height) / 2;
      *txt_y = y;
    }
    break;
  }
}

static void iFlatButtonDrawIcon(Ihandle* ih, IdrawCanvas* dc, int icon_x, int icon_y, int icon_width, int icon_height,
                                const char *baseattrib, const char* imagename, const char* title, const char* fgcolor, const char* bgcolor, int active)
{
  int x, y, width, height;

  if (imagename)
  {
    if (title)
    {
      int img_x, img_y, txt_x, txt_y;
      int txt_width, txt_height;
      int img_width, img_height;

      iupdrvFontGetMultiLineStringSize(ih, title, &txt_width, &txt_height);
      iupImageGetInfo(imagename, &img_width, &img_height, NULL);

      if (ih->data->img_position == IUP_IMGPOS_RIGHT || ih->data->img_position == IUP_IMGPOS_LEFT)
      {
        width = img_width + txt_width + ih->data->spacing;
        height = iupMAX(img_height, txt_height);
      }
      else
      {
        width = iupMAX(img_width, txt_width);
        height = img_height + txt_height + ih->data->spacing;
      }

      iFlatButtonGetIconPosition(ih, icon_width, icon_height, &x, &y, width, height);

      iFlatButtonGetImageTextPosition(x, y, ih->data->img_position, ih->data->spacing,
                                      img_width, img_height, txt_width, txt_height,
                                      &img_x, &img_y, &txt_x, &txt_y);

      iFlatButtonDrawImage(ih, dc, img_x + icon_x, img_y + icon_y, baseattrib, imagename, active);
      iFlatButtonDrawText(ih, dc, txt_x + icon_x, txt_y + icon_y, title, fgcolor, bgcolor, active);
    }
    else
    {
      iupImageGetInfo(imagename, &width, &height, NULL);

      iFlatButtonGetIconPosition(ih, icon_width, icon_height, &x, &y, width, height);

      iFlatButtonDrawImage(ih, dc, x + icon_x, y + icon_y, baseattrib, imagename, active);
    }
  }
  else if (title)
  {
    iupdrvFontGetMultiLineStringSize(ih, title, &width, &height);

    iFlatButtonGetIconPosition(ih, icon_width, icon_height, &x, &y, width, height);

    iFlatButtonDrawText(ih, dc, x + icon_x, y + icon_y, title, fgcolor, bgcolor, active);
  }
}

static int iFlatButtonRedraw_CB(Ihandle* ih)
{
  char *image = iupAttribGet(ih, "IMAGE");
  char* title = iupAttribGet(ih, "TITLE");
  int active = IupGetInt(ih, "ACTIVE");  /* native implementation */
  int selected = iupAttribGetInt(ih, "VALUE");
  char* fgcolor = iupAttribGetStr(ih, "FGCOLOR");
  char* bgcolor = iupAttribGetStr(ih, "BGCOLOR");
  char* bgimage = iupAttribGet(ih, "BACKIMAGE");
  char* fgimage = iupAttribGet(ih, "FRONTIMAGE");
  int border_width = ih->data->border_width;
  int draw_border = 0;
  int old_pressed = ih->data->pressed;
  IdrawCanvas* dc = iupDrawCreateCanvas(ih);
  
  iupDrawParentBackground(dc);

  if (!bgcolor)
    bgcolor = iupBaseNativeParentGetBgColorAttrib(ih);

  if (ih->data->pressed || selected)
  {
    char* presscolor = iupAttribGetStr(ih, "PSCOLOR");
    if (presscolor)
      bgcolor = presscolor;

    draw_border = 1;

    if (!ih->data->pressed && (bgimage || image))
      ih->data->pressed = 1;
  }
  else if (ih->data->highlight)
  {
    char* hlcolor = iupAttribGetStr(ih, "HLCOLOR");
    if (hlcolor)
      bgcolor = hlcolor;

    draw_border = 1;
  }

  /* draw border - can still be disabled setting border_width=0 */
  if (draw_border)
  {
    char* bordercolor = iupAttribGetStr(ih, "BORDERCOLOR");
    iFlatButtonDrawBorder(dc, 0, ih->currentwidth - 1, 
                              0, ih->currentheight - 1, 
                              border_width, bordercolor, bgcolor, active);
  }

  /* draw background */
  if (bgimage)
    iFlatButtonDrawImage(ih, dc, border_width, border_width, "BACKIMAGE", bgimage, active);
  else
    iFlatButtonDrawBox(dc, border_width, ih->currentwidth - 1 - border_width,
                           border_width, ih->currentheight - 1 - border_width,
                           bgcolor, NULL, 1);  /* always active */

  iFlatButtonDrawIcon(ih, dc, border_width, border_width,
                              ih->currentwidth - 2 * border_width, ih->currentheight - 2 * border_width,
                              "IMAGE", image, title, fgcolor, bgcolor, active);

  if (fgimage)
    iFlatButtonDrawImage(ih, dc, border_width, border_width, "FRONTIMAGE", fgimage, active);
  else if (!image && !title)
  {
    int space = border_width + 2;
    iFlatButtonDrawBorder(dc, space, ih->currentwidth - 1 - space,
                              space, ih->currentheight - 1 - space,
                              1, "0 0 0", bgcolor, active);
    space++;
    iFlatButtonDrawBox(dc, space, ih->currentwidth - 1 - space,
                           space, ih->currentheight - 1 - space,
                           fgcolor, bgcolor, active);
  }


  if (selected && !old_pressed && (bgimage || image))
    ih->data->pressed = 0;

  if (ih->data->has_focus)
    iupDrawFocusRect(dc, border_width, border_width, ih->currentwidth - 2 * border_width, ih->currentheight - 2 * border_width);

  iupDrawFlush(dc);

  iupDrawKillCanvas(dc);

  return IUP_DEFAULT;
}

static void iFlatButtonNotify(Ihandle* ih, int is_toggle)
{
  Icallback cb = IupGetCallback(ih, "FLAT_ACTION");
  if (cb)
  {
    int ret = cb(ih);
    if (ret == IUP_CLOSE)
      IupExitLoop();
  }

  if (is_toggle)
  {
    if (iupObjectCheck(ih))
      iupBaseCallValueChangedCb(ih);
  }
}

static int iFlatButtonButton_CB(Ihandle* ih, int button, int pressed, int x, int y, char* status)
{
  IFniiiis cb = (IFniiiis)IupGetCallback(ih, "FLAT_BUTTON_CB");
  if (cb)
  {
    if (cb(ih, button, pressed, x, y, status) == IUP_IGNORE)
      return IUP_DEFAULT;
  }

  if (button == IUP_BUTTON1)
  {
    if (iupAttribGetBoolean(ih, "TOGGLE"))
    {
      Ihandle* radio = iupRadioFindToggleParent(ih);
      int selected = iupAttribGetInt(ih, "VALUE");
      Ihandle* last_tg = NULL;

      if (!pressed)
      {
        if (selected)  /* was ON */
        {
          if (!radio)
          {
            iupAttribSet(ih, "VALUE", "OFF");
            selected = 0;
          }
          else
            last_tg = ih;  /* to avoid the callback call */
        }
        else  /* was OFF */
        {
          if (radio)
          {
            last_tg = (Ihandle*)iupAttribGet(radio, "_IUP_FLATBUTTON_LASTRADIO");
            if (iupObjectCheck(last_tg) && last_tg != ih)
            {
              iupAttribSet(last_tg, "VALUE", "OFF");
              iupdrvRedrawNow(last_tg);
            }
            else
              last_tg = NULL;

            iupAttribSet(radio, "_IUP_FLATBUTTON_LASTRADIO", (char*)ih);
          }

          iupAttribSet(ih, "VALUE", "ON");
          selected = 1;
        }
      }

      ih->data->pressed = pressed;
      iupdrvRedrawNow(ih);

      if (!pressed)
      {
        if (last_tg && ih != last_tg)
          iFlatButtonNotify(last_tg, 1);

        if (!radio || ih != last_tg)
          iFlatButtonNotify(ih, 1);
      }
    }
    else
    {
      ih->data->pressed = pressed;
      iupdrvRedrawNow(ih);

      if (!pressed)
        iFlatButtonNotify(ih, 0);
    }
  }

  return IUP_DEFAULT;
}

static int iFlatButtonActivate_CB(Ihandle* ih)
{
  char status[IUPKEY_STATUS_SIZE] = IUPKEY_STATUS_INIT;

  iFlatButtonButton_CB(ih, IUP_BUTTON1, 1, 0, 0, status);

  iupdrvSleep(100);

  iFlatButtonButton_CB(ih, IUP_BUTTON1, 0, 0, 0, status);

  return IUP_DEFAULT;
}

static int iFlatButtonFocus_CB(Ihandle* ih, int focus)
{
  IFni cb = (IFni)IupGetCallback(ih, "FLAT_FOCUS_CB");
  if (cb)
  {
    if (cb(ih, focus) == IUP_IGNORE)
      return IUP_DEFAULT;
  }

  ih->data->has_focus = focus;
  iupdrvRedrawNow(ih);

  return IUP_DEFAULT;
}

static int iFlatButtonEnterWindow_CB(Ihandle* ih)
{
  IFn cb = (IFn)IupGetCallback(ih, "FLAT_ENTERWINDOW_CB");
  if (cb)
  {
    if (cb(ih) == IUP_IGNORE)
      return IUP_DEFAULT;
  }

  ih->data->highlight = 1;
  iupdrvRedrawNow(ih);

  return IUP_DEFAULT;
}

static int iFlatButtonLeaveWindow_CB(Ihandle* ih)
{
  IFn cb = (IFn)IupGetCallback(ih, "FLAT_LEAVEWINDOW_CB");
  if (cb)
  {
    if (cb(ih) == IUP_IGNORE)
      return IUP_DEFAULT;
  }

  ih->data->highlight = 0;
  iupdrvRedrawNow(ih);

  return IUP_DEFAULT;
}


/***********************************************************************************************/


static int iFlatButtonSetActiveAttrib(Ihandle* ih, const char* value)
{
  iupBaseSetActiveAttrib(ih, value);
  iupdrvRedrawNow(ih);
  return 0; 
}

static int iFlatButtonSetAlignmentAttrib(Ihandle* ih, const char* value)
{
  char value1[30], value2[30];

  iupStrToStrStr(value, value1, value2, ':');

  if (iupStrEqualNoCase(value1, "ARIGHT"))
    ih->data->horiz_alignment = IUP_ALIGN_ARIGHT;
  else if (iupStrEqualNoCase(value1, "ALEFT"))
    ih->data->horiz_alignment = IUP_ALIGN_ALEFT;
  else /* "ACENTER" */
    ih->data->horiz_alignment = IUP_ALIGN_ACENTER;

  if (iupStrEqualNoCase(value2, "ABOTTOM"))
    ih->data->vert_alignment = IUP_ALIGN_ABOTTOM;
  else if (iupStrEqualNoCase(value2, "ATOP"))
    ih->data->vert_alignment = IUP_ALIGN_ATOP;
  else /* "ACENTER" */
    ih->data->vert_alignment = IUP_ALIGN_ACENTER;

  if (ih->handle)
    iupdrvRedrawNow(ih);

  return 1;
}

static char* iFlatButtonGetAlignmentAttrib(Ihandle *ih)
{
  char* horiz_align2str[3] = {"ALEFT", "ACENTER", "ARIGHT"};
  char* vert_align2str[3] = {"ATOP", "ACENTER", "ABOTTOM"};
  return iupStrReturnStrf("%s:%s", horiz_align2str[ih->data->horiz_alignment], vert_align2str[ih->data->vert_alignment]);
}

static int iFlatButtonSetPaddingAttrib(Ihandle* ih, const char* value)
{
  iupStrToIntInt(value, &ih->data->horiz_padding, &ih->data->vert_padding, 'x');
  if (ih->handle)
    iupdrvRedrawNow(ih);
  return 0;
}

static int iFlatButtonSetBgColorAttrib(Ihandle* ih, const char* value)
{
  iupAttribSet(ih, "BGCOLOR", value);
  iupImageUpdateParent(ih);
  iupdrvRedrawNow(ih);
  return 1;
}

static char* iFlatButtonGetBgColorAttrib(Ihandle* ih)
{
  char* value = iupAttribGet(ih, "BGCOLOR");
  if (!value)
    return iupBaseNativeParentGetBgColorAttrib(ih);
  else
    return value;
}

static char* iFlatButtonGetPaddingAttrib(Ihandle* ih)
{
  return iupStrReturnIntInt(ih->data->horiz_padding, ih->data->vert_padding, 'x');
}

static int iFlatButtonSetImagePositionAttrib(Ihandle* ih, const char* value)
{
  if (iupStrEqualNoCase(value, "RIGHT"))
    ih->data->img_position = IUP_IMGPOS_RIGHT;
  else if (iupStrEqualNoCase(value, "BOTTOM"))
    ih->data->img_position = IUP_IMGPOS_BOTTOM;
  else if (iupStrEqualNoCase(value, "TOP"))
    ih->data->img_position = IUP_IMGPOS_TOP;
  else /* "LEFT" */
    ih->data->img_position = IUP_IMGPOS_LEFT;

  if (ih->handle)
    iupdrvRedrawNow(ih);

  return 0;
}

static char* iFlatButtonGetImagePositionAttrib(Ihandle *ih)
{
  char* img_pos2str[4] = {"LEFT", "RIGHT", "TOP", "BOTTOM"};
  return iupStrReturnStr(img_pos2str[ih->data->img_position]);
}

static int iFlatButtonSetSpacingAttrib(Ihandle* ih, const char* value)
{
  iupStrToInt(value, &ih->data->spacing);
  if (ih->handle)
    iupdrvRedrawNow(ih);
  return 0;
}

static char* iFlatButtonGetSpacingAttrib(Ihandle *ih)
{
  return iupStrReturnInt(ih->data->spacing);
}

static int iFlatButtonSetBorderWidthAttrib(Ihandle* ih, const char* value)
{
  iupStrToInt(value, &ih->data->border_width);
  if (ih->handle)
    iupdrvRedrawNow(ih);
  return 0;
}

static char* iFlatButtonGetBorderWidthAttrib(Ihandle *ih)
{
  return iupStrReturnInt(ih->data->border_width);
}

static int iFlatButtonSetValueAttrib(Ihandle* ih, const char* value)
{
  if (iupAttribGetBoolean(ih, "TOGGLE"))
  {
    Ihandle* radio = iupRadioFindToggleParent(ih);
    if (radio)
    {
      /* can only set Radio to ON */
      if (iupStrEqualNoCase(value, "TOGGLE") || iupStrBoolean(value))
      {
        Ihandle* last_tg = (Ihandle*)iupAttribGet(radio, "_IUP_FLATBUTTON_LASTRADIO");
        if (iupObjectCheck(last_tg) && last_tg != ih)
        {
          iupAttribSet(last_tg, "VALUE", "OFF");
          if (last_tg->handle)
            iupdrvRedrawNow(last_tg);
        }

        iupAttribSet(radio, "_IUP_FLATBUTTON_LASTRADIO", (char*)ih);
      }
      else
        return 0;
    }
    else
    {
      if (iupStrEqualNoCase(value, "TOGGLE"))
      {
        int oldcheck = iupAttribGetBoolean(ih, "VALUE");
        if (oldcheck)
          iupAttribSet(ih, "VALUE", "OFF");
        else
          iupAttribSet(ih, "VALUE", "ON");

        if (ih->handle)
          iupdrvRedrawNow(ih);

        return 0;
      }
    }

    if (ih->handle)
      iupdrvPostRedraw(ih);

    return 1;
  }
  else
    return 0;
}

static char* iFlatButtonGetRadioAttrib(Ihandle* ih)
{
  if (iupAttribGetBoolean(ih, "TOGGLE"))
  {
    Ihandle* radio = iupRadioFindToggleParent(ih);
    return iupStrReturnBoolean(radio != NULL);
  }
  else
    return NULL;
}


/*****************************************************************************************/


static int iFlatButtonCreateMethod(Ihandle* ih, void** params)
{
  if (params && params[0])
  {
    iupAttribSetStr(ih, "TITLE", (char*)(params[0]));
  }
  
  /* free the data allocated by IupCanvas */
  free(ih->data);
  ih->data = iupALLOCCTRLDATA();

  /* change the IupCanvas default values */
  iupAttribSet(ih, "BORDER", "NO");
  ih->expand = IUP_EXPAND_NONE;

  /* non zero default values */
  ih->data->spacing = 2;
  ih->data->border_width = 1;
  ih->data->horiz_alignment = IUP_ALIGN_ACENTER;
  ih->data->vert_alignment = IUP_ALIGN_ACENTER;
  
  IupSetCallback(ih, "ACTION", (Icallback)iFlatButtonRedraw_CB);
  IupSetCallback(ih, "BUTTON_CB", (Icallback)iFlatButtonButton_CB);
  IupSetCallback(ih, "FOCUS_CB", (Icallback)iFlatButtonFocus_CB);
  IupSetCallback(ih, "LEAVEWINDOW_CB", iFlatButtonLeaveWindow_CB);
  IupSetCallback(ih, "ENTERWINDOW_CB", iFlatButtonEnterWindow_CB);
  IupSetCallback(ih, "K_CR", (Icallback)iFlatButtonActivate_CB);
  IupSetCallback(ih, "K_SP", (Icallback)iFlatButtonActivate_CB);

  return IUP_NOERROR;
}

static int iFlatButtonMapMethod(Ihandle* ih)
{
  if (iupAttribGetBoolean(ih, "TOGGLE"))
  {
    Ihandle* radio = iupRadioFindToggleParent(ih);
    if (radio)
    {
      if (!iupAttribGet(radio, "_IUP_FLATBUTTON_LASTRADIO"))
      {
        /* this is the first toggle in the radio, and then set it with VALUE=ON */
        iupAttribSet(ih, "VALUE", "ON");
      }
    }
  }
  return IUP_NOERROR;
}

static void iFlatButtonComputeNaturalSizeMethod(Ihandle* ih, int *w, int *h, int *children_expand)
{
  int fit2backimage = iupAttribGetBoolean(ih, "FITTOBACKIMAGE");
  char* bgimage = iupAttribGet(ih, "BACKIMAGE");
  if (fit2backimage && bgimage)
    iupImageGetInfo(bgimage, w, h, NULL);
  else
  {
    char* image = iupAttribGet(ih, "IMAGE");
    char* title = iupAttribGet(ih, "TITLE");

    *w = 0,
    *h = 0;

    if (image)
    {
      iupImageGetInfo(image, w, h, NULL);

      if (title)
      {
        int text_w, text_h;
        iupdrvFontGetMultiLineStringSize(ih, title, &text_w, &text_h);

        if (ih->data->img_position == IUP_IMGPOS_RIGHT ||
            ih->data->img_position == IUP_IMGPOS_LEFT)
        {
          *w += text_w + ih->data->spacing;
          *h = iupMAX(*h, text_h);
        }
        else
        {
          *w = iupMAX(*w, text_w);
          *h += text_h + ih->data->spacing;
        }
      }
    }
    else if (title)
      iupdrvFontGetMultiLineStringSize(ih, title, w, h);

    *w += 2 * ih->data->horiz_padding;
    *h += 2 * ih->data->vert_padding;

    *w += 2 * ih->data->border_width;
    *h += 2 * ih->data->border_width;
  }

  (void)children_expand; /* unset if not a container */
}


/******************************************************************************/


Iclass* iupFlatButtonNewClass(void)
{
  Iclass* ic = iupClassNew(iupRegisterFindClass("canvas"));

  ic->name = "flatbutton";
  ic->format = "s"; /* one string */
  ic->nativetype = IUP_TYPECANVAS;
  ic->childtype = IUP_CHILDNONE;
  ic->is_interactive = 1;

  /* Class functions */
  ic->New = iupFlatButtonNewClass;
  ic->Create = iFlatButtonCreateMethod;
  ic->ComputeNaturalSize = iFlatButtonComputeNaturalSizeMethod;
  ic->Map = iFlatButtonMapMethod;

  /* Callbacks */
  iupClassRegisterCallback(ic, "FLAT_ACTION", "");
  iupClassRegisterCallback(ic, "FLAT_BUTTON_CB", "iiiis");
  iupClassRegisterCallback(ic, "FLAT_FOCUS_CB", "i");
  iupClassRegisterCallback(ic, "FLAT_ENTERWINDOW_CB", "ii");
  iupClassRegisterCallback(ic, "FLAT_LEAVEWINDOW_CB", "");
  iupClassRegisterCallback(ic, "VALUECHANGED_CB", "");

  /* Overwrite Visual */
  iupClassRegisterAttribute(ic, "ACTIVE", iupBaseGetActiveAttrib, iFlatButtonSetActiveAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_DEFAULT);

  /* Special */
  iupClassRegisterAttribute(ic, "TITLE", NULL, NULL, NULL, NULL, IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);

  /* IupButton only */
  iupClassRegisterAttribute(ic, "VALUE", NULL, iFlatButtonSetValueAttrib, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "RADIO", iFlatButtonGetRadioAttrib, NULL, NULL, NULL, IUPAF_READONLY | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "TOGGLE", NULL, NULL, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "ALIGNMENT", iFlatButtonGetAlignmentAttrib, iFlatButtonSetAlignmentAttrib, "ACENTER:ACENTER", NULL, IUPAF_NOT_MAPPED | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "PADDING", iFlatButtonGetPaddingAttrib, iFlatButtonSetPaddingAttrib, IUPAF_SAMEASSYSTEM, "0x0", IUPAF_NOT_MAPPED);
  iupClassRegisterAttribute(ic, "SPACING", iFlatButtonGetSpacingAttrib, iFlatButtonSetSpacingAttrib, IUPAF_SAMEASSYSTEM, "2", IUPAF_NOT_MAPPED | IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "BORDERCOLOR", NULL, NULL, IUPAF_SAMEASSYSTEM, "50 150 255", IUPAF_DEFAULT);  /* inheritable */
  iupClassRegisterAttribute(ic, "BORDERWIDTH", iFlatButtonGetBorderWidthAttrib, iFlatButtonSetBorderWidthAttrib, IUPAF_SAMEASSYSTEM, "1", IUPAF_DEFAULT);  /* inheritable */
  iupClassRegisterAttribute(ic, "FGCOLOR", NULL, NULL, "DLGFGCOLOR", NULL, IUPAF_NOT_MAPPED);  /* force the new default value */
  iupClassRegisterAttribute(ic, "BGCOLOR", iFlatButtonGetBgColorAttrib, iFlatButtonSetBgColorAttrib, IUPAF_SAMEASSYSTEM, "DLGBGCOLOR", IUPAF_NO_SAVE | IUPAF_DEFAULT);
  iupClassRegisterAttribute(ic, "HLCOLOR", NULL, NULL, IUPAF_SAMEASSYSTEM, "200 225 245", IUPAF_DEFAULT);  /* inheritable */
  iupClassRegisterAttribute(ic, "PSCOLOR", NULL, NULL, IUPAF_SAMEASSYSTEM, "150 200 235", IUPAF_DEFAULT);  /* inheritable */

  iupClassRegisterAttribute(ic, "IMAGE", NULL, NULL, NULL, NULL, IUPAF_IHANDLENAME | IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "IMAGEPRESS", NULL, NULL, NULL, NULL, IUPAF_IHANDLENAME | IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "IMAGEHIGHLIGHT", NULL, NULL, NULL, NULL, IUPAF_IHANDLENAME | IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "IMAGEINACTIVE", NULL, NULL, NULL, NULL, IUPAF_IHANDLENAME | IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);
  
  iupClassRegisterAttribute(ic, "IMAGEPOSITION", iFlatButtonGetImagePositionAttrib, iFlatButtonSetImagePositionAttrib, IUPAF_SAMEASSYSTEM, "LEFT", IUPAF_NOT_MAPPED | IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "BACKIMAGE", NULL, NULL, NULL, NULL, IUPAF_IHANDLENAME | IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "BACKIMAGEPRESS", NULL, NULL, NULL, NULL, IUPAF_IHANDLENAME | IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "BACKIMAGEHIGHLIGHT", NULL, NULL, NULL, NULL, IUPAF_IHANDLENAME | IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "BACKIMAGEINACTIVE", NULL, NULL, NULL, NULL, IUPAF_IHANDLENAME | IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);
  
  iupClassRegisterAttribute(ic, "FITTOBACKIMAGE", NULL, NULL, NULL, NULL, IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "FRONTIMAGE", NULL, NULL, NULL, NULL, IUPAF_IHANDLENAME | IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "FRONTIMAGEPRESS", NULL, NULL, NULL, NULL, IUPAF_IHANDLENAME | IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "FRONTIMAGEHIGHLIGHT", NULL, NULL, NULL, NULL, IUPAF_IHANDLENAME | IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "FRONTIMAGEINACTIVE", NULL, NULL, NULL, NULL, IUPAF_IHANDLENAME | IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);

  return ic;
}

Ihandle* IupFlatButton(const char* title)
{
  void *params[2];
  params[0] = (void*)title;
  params[1] = NULL;
  return IupCreatev("flatbutton", params);
}
