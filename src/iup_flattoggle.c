/** \file
 * \brief Toggle Control
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdlib.h>

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
#include "iup_drvdraw.h"
#include "iup_draw.h"
#include "iup_drvinfo.h"
#include "iup_key.h"


#define ITOGGLE_SPACE  3
#define ITOGGLE_MARGIN 2

/* from IupRadio implementation */
Ihandle *iupRadioFindToggleParent(Ihandle* ih_toggle);


struct _IcontrolData
{
  iupCanvas canvas;  /* from IupCanvas (must reserve it) */

  /* attributes */
  int horiz_padding, vert_padding;  /* button margin */
  int spacing, img_position;        /* used when both text and image are displayed */
  int horiz_alignment, vert_alignment;
  int border_width;
  int check_size, check_spacing;

  /* aux */
  int has_focus,
    highlighted,
    pressed;
};


/****************************************************************/


static int iFlatToggleRedraw_CB(Ihandle* ih)
{
  char *image = iupAttribGet(ih, "IMAGE");
  char* title = iupAttribGet(ih, "TITLE");
  int active = IupGetInt(ih, "ACTIVE");  /* native implementation */
  int notdef = iupStrEqualNoCase(iupAttribGet(ih, "VALUE"), "NOTDEF");
  int selected = (notdef) ? -1 : iupAttribGetInt(ih, "VALUE");
  char* fgcolor = iupAttribGetStr(ih, "FGCOLOR");
  char* bgcolor = iupAttribGet(ih, "BGCOLOR");
  char* bgimage = iupAttribGet(ih, "BACKIMAGE");
  char* fgimage = iupAttribGet(ih, "FRONTIMAGE");
  char* text_align = iupAttribGetStr(ih, "TEXTALIGNMENT");
  int check_right = iupAttribGetInt(ih, "CHECKRIGHT");
  Ihandle* radio = iupRadioFindToggleParent(ih);
  const char* draw_image;
  int border_width = ih->data->border_width;
  int draw_border = iupAttribGetBoolean(ih, "SHOWBORDER");
  int image_pressed, icon_position, check_position, icon_width;
  IdrawCanvas* dc = iupdrvDrawCreateCanvas(ih);
  int make_inactive = 0;

  iupDrawParentBackground(dc, ih);

  if (!bgcolor)
  {
    if (draw_border)
      bgcolor = iupFlatGetDarkerBgColor(ih);
    else
      bgcolor = iupBaseNativeParentGetBgColorAttrib(ih);
  }

  if (ih->data->check_size)
  {
    check_position = 0;
    icon_position = ih->data->check_size + ih->data->check_spacing;
    if (check_right)
    {
      check_position = ih->currentwidth - ih->data->check_size;
      icon_position = 0;
    }
    icon_width = ih->currentwidth - ih->data->check_size - ih->data->check_spacing;
  }
  else
  {
    if (ih->data->pressed || (selected && !ih->data->highlighted))
    {
      char* presscolor = iupAttribGetStr(ih, "PSCOLOR");
      if (presscolor)
        bgcolor = presscolor;

      draw_border = 1;
    }
    else if (ih->data->highlighted)
    {
      char* hlcolor = iupAttribGetStr(ih, "HLCOLOR");
      if (hlcolor)
        bgcolor = hlcolor;

      draw_border = 1;
    }

    check_position = 0;
    icon_position = 0;
    icon_width = ih->currentwidth;
  }

  /* draw border - can still be disabled setting border_width=0 */
  if (draw_border)
  {
    char* bordercolor = iupAttribGetStr(ih, "BORDERCOLOR");

    if (ih->data->pressed || (selected && !ih->data->highlighted))
    {
      char* presscolor = iupAttribGetStr(ih, "BORDERPSCOLOR");
      if (presscolor)
        bordercolor = presscolor;
    }
    else if (ih->data->highlighted)
    {
      char* hlcolor = iupAttribGetStr(ih, "BORDERHLCOLOR");
      if (hlcolor)
        bordercolor = hlcolor;
    }

    iupFlatDrawBorder(dc, 0, ih->currentwidth - 1,
                          0, ih->currentheight - 1,
                          border_width, bordercolor, bgcolor, active);
  }

  /* simulate pressed when selected and has images (but colors and borders are not included) */
  image_pressed = ih->data->pressed;
  if (selected && !ih->data->pressed && (bgimage || image))
    image_pressed = 1;

  /* draw background */
  if (bgimage)
  {
    draw_image = iupFlatGetImageName(ih, "BACKIMAGE", bgimage, image_pressed, ih->data->highlighted, active, &make_inactive);
    iupdrvDrawImage(dc, draw_image, make_inactive, border_width, border_width);
  }
  else 
    iupFlatDrawBox(dc, border_width, ih->currentwidth - 1 - border_width,
                       border_width, ih->currentheight - 1 - border_width,
                       bgcolor, NULL, 1);  /* background is always active */

  /* draw icon */
  draw_image = iupFlatGetImageName(ih, "IMAGE", image, image_pressed, ih->data->highlighted, active, &make_inactive);
  iupFlatDrawIcon(ih, dc, border_width + icon_position, border_width,
                          icon_width - 2 * border_width, ih->currentheight - 2 * border_width,
                          ih->data->img_position, ih->data->spacing, ih->data->horiz_alignment, ih->data->vert_alignment, ih->data->horiz_padding, ih->data->vert_padding,
                          draw_image, make_inactive, title, text_align, fgcolor, bgcolor, active);

  if (fgimage)
  {
    draw_image = iupFlatGetImageName(ih, "FRONTIMAGE", fgimage, image_pressed, ih->data->highlighted, active, &make_inactive);
    iupdrvDrawImage(dc, draw_image, make_inactive, border_width, border_width);
  }
  else if (!image && !title)
  {
    int space = border_width + ITOGGLE_MARGIN;
    iupFlatDrawBorder(dc, space + icon_position, icon_width - 1 - space,
                          space, ih->currentheight - 1 - space,
                          1, "0 0 0", bgcolor, active);
    space++;
    iupFlatDrawBox(dc, space + icon_position, icon_width - 1 - space,
                       space, ih->currentheight - 1 - space,
                       fgcolor, bgcolor, active);
  }

  if (ih->data->check_size)
  {
    int xc = check_position + ih->data->check_size / 2;
    int yc = ih->currentheight / 2;
    int radius = ih->data->check_size / 2 - ITOGGLE_MARGIN;
    int check_xmin = check_position + ITOGGLE_MARGIN;
    int check_ymin = (ih->currentheight - ih->data->check_size) / 2 + ITOGGLE_MARGIN;
    int check_size = ih->data->check_size - 2 * ITOGGLE_MARGIN;
    char* check_image = iupAttribGet(ih, "CHECKIMAGE");

    if (check_image)
    {
      if (selected == -1)
        draw_image = iupFlatGetImageName(ih, "CHECKIMAGENOTDEF", check_image, ih->data->pressed, ih->data->highlighted, active, &make_inactive);
      else if (selected)
        draw_image = iupFlatGetImageName(ih, "CHECKIMAGEON", check_image, ih->data->pressed, ih->data->highlighted, active, &make_inactive);

      iupdrvDrawImage(dc, draw_image, make_inactive, check_xmin, check_ymin);
    }
    else
    {
      char* check_fgcolor = iupAttribGet(ih, "CHECKFGCOLOR");
      char* check_bgcolor = iupAttribGetStr(ih, "CHECKBGCOLOR");
      char* bordercolor   = iupAttribGetStr(ih, "BORDERCOLOR");

      if (!check_fgcolor)
        check_fgcolor = fgcolor;

      if (ih->data->pressed || (selected && !ih->data->highlighted))
      {
        char* presscolor = iupAttribGetStr(ih, "CHECKPSCOLOR");
        if (presscolor)
          check_fgcolor = presscolor;
      }
      else if (ih->data->highlighted)
      {
        char* hlcolor = iupAttribGetStr(ih, "CHECKHLCOLOR");
        if (hlcolor)
          check_fgcolor = hlcolor;
        else
          check_fgcolor = bordercolor; /* only when highlighted */
      }

      /* check background */
      if (radio)
        iupFlatDrawDrawCircle(dc, xc, yc, radius, 1, 1, check_bgcolor, bgcolor, active);
      else
        iupFlatDrawBox(dc, check_xmin, check_xmin + check_size,
                           check_ymin, check_ymin + check_size,
                           check_bgcolor, bgcolor, active);

      /* check border */
      if (radio)
        iupFlatDrawDrawCircle(dc, xc, yc, radius, 0, 2, check_fgcolor, bgcolor, active);
      else
        iupFlatDrawBorder(dc, check_xmin, check_xmin + check_size,
                              check_ymin, check_ymin + check_size,
                              2, check_fgcolor, bgcolor, active);

      /* check mark */
      if (selected)
      {
        if (radio)
          iupFlatDrawDrawCircle(dc, xc, yc, radius - ITOGGLE_SPACE, 1, 1, check_fgcolor, check_bgcolor, active);
        else if (selected == -1)
          iupFlatDrawBox(dc, check_xmin + ITOGGLE_SPACE, check_xmin + check_size - ITOGGLE_SPACE,
                             check_ymin + ITOGGLE_SPACE, check_ymin + check_size - ITOGGLE_SPACE,
                             check_fgcolor, check_bgcolor, active);
        else
          iupFlatDrawCheckMark(dc, check_xmin + ITOGGLE_SPACE, check_xmin + check_size - ITOGGLE_SPACE,
                                   check_ymin + ITOGGLE_SPACE, check_ymin + check_size - ITOGGLE_SPACE,
                                   check_fgcolor, check_bgcolor, active);
      }
    }
  }

  if (ih->data->has_focus)
    iupdrvDrawFocusRect(dc, border_width, border_width, ih->currentwidth - 1 - border_width, ih->currentheight - 1 - border_width);

  iupdrvDrawFlush(dc);

  iupdrvDrawKillCanvas(dc);

  return IUP_DEFAULT;
}

static void iFlatToggleNotify(Ihandle* ih)
{
  IFni cb = (IFni)IupGetCallback(ih, "FLAT_ACTION");
  if (cb)
  {
    char *value = iupAttribGet(ih, "VALUE");
    int ret, v = iupAttribGetInt(ih, "VALUE");
    if (iupStrEqualNoCase(value, "NOTDEF"))
      v = -1;

    ret = cb(ih, v);
    if (ret == IUP_CLOSE)
      IupExitLoop();
  }

  if (iupObjectCheck(ih))
    iupBaseCallValueChangedCb(ih);
}

static int iFlatToggleButton_CB(Ihandle* ih, int button, int pressed, int x, int y, char* status)
{
  IFniiiis cb = (IFniiiis)IupGetCallback(ih, "FLAT_BUTTON_CB");
  if (cb)
  {
    if (cb(ih, button, pressed, x, y, status) == IUP_IGNORE)
      return IUP_DEFAULT;
  }

  if (button == IUP_BUTTON1)
  {
    Ihandle* radio = iupRadioFindToggleParent(ih);
    int tstate = iupAttribGetInt(ih, "3STATE");
    int notdef = iupStrEqualNoCase(iupAttribGet(ih, "VALUE"), "NOTDEF");
    int selected =  (notdef) ? -1 : iupAttribGetInt(ih, "VALUE");
    Ihandle* last_tg = NULL;

    if (!pressed)
    {
      if (selected>0)  /* was ON */
      {
        if (!radio)
        {
          if (tstate)
          {
            iupAttribSet(ih, "VALUE", "NOTDEF");
            selected = -1;
          }
          else
          {
            iupAttribSet(ih, "VALUE", "OFF");
            selected = 0;
          }
        }
        else
          last_tg = ih;  /* to avoid the callback call */

      }
      else if (selected == -1)
      {
        iupAttribSet(ih, "VALUE", "OFF");
        selected = 0;
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
        iFlatToggleNotify(last_tg);

      if (!radio || ih != last_tg)
        iFlatToggleNotify(ih);
    }
  }

  return IUP_DEFAULT;
}

static int iFlatToggleActivate_CB(Ihandle* ih)
{
  char status[IUPKEY_STATUS_SIZE] = IUPKEY_STATUS_INIT;

  iFlatToggleButton_CB(ih, IUP_BUTTON1, 1, 0, 0, status);

  iupdrvSleep(100);

  iFlatToggleButton_CB(ih, IUP_BUTTON1, 0, 0, 0, status);

  return IUP_DEFAULT;
}

static int iFlatToggleFocus_CB(Ihandle* ih, int focus)
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

static int iFlatToggleEnterWindow_CB(Ihandle* ih)
{
  IFn cb = (IFn)IupGetCallback(ih, "FLAT_ENTERWINDOW_CB");
  if (cb)
  {
    if (cb(ih) == IUP_IGNORE)
      return IUP_DEFAULT;
  }

  ih->data->highlighted = 1;
  iupdrvRedrawNow(ih);

  return IUP_DEFAULT;
}

static int iFlatToggleLeaveWindow_CB(Ihandle* ih)
{
  IFn cb = (IFn)IupGetCallback(ih, "FLAT_LEAVEWINDOW_CB");
  if (cb)
  {
    if (cb(ih) == IUP_IGNORE)
      return IUP_DEFAULT;
  }

  ih->data->highlighted = 0;
  iupdrvRedrawNow(ih);

  return IUP_DEFAULT;
}


/***********************************************************************************************/


static int iFlatToggleSetActiveAttrib(Ihandle* ih, const char* value)
{
  iupBaseSetActiveAttrib(ih, value);
  iupdrvRedrawNow(ih);
  return 0;
}

static int iFlatToggleSetAlignmentAttrib(Ihandle* ih, const char* value)
{
  char value1[30], value2[30];

  iupStrToStrStr(value, value1, value2, ':');

  ih->data->horiz_alignment = iupFlatGetHorizontalAlignment(value1);
  ih->data->vert_alignment = iupFlatGetVerticalAlignment(value2);

  if (ih->handle)
    iupdrvRedrawNow(ih);

  return 1;
}

static char* iFlatToggleGetAlignmentAttrib(Ihandle *ih)
{
  char* horiz_align2str[3] = { "ALEFT", "ACENTER", "ARIGHT" };
  char* vert_align2str[3] = { "ATOP", "ACENTER", "ABOTTOM" };
  return iupStrReturnStrf("%s:%s", horiz_align2str[ih->data->horiz_alignment], vert_align2str[ih->data->vert_alignment]);
}

static int iFlatToggleSetPaddingAttrib(Ihandle* ih, const char* value)
{
  iupStrToIntInt(value, &ih->data->horiz_padding, &ih->data->vert_padding, 'x');
  if (ih->handle)
    iupdrvRedrawNow(ih);
  return 0;
}

static int iFlatToggleGetDefaultCheckSize(void)
{
  int dpi = iupRound(iupdrvGetScreenDpi());
  if (dpi > 120)
    return 24;
  else
    return 16;
}

static int iFlatToggleSetCheckSizeAttrib(Ihandle* ih, const char* value)
{
  if (!value)
    ih->data->check_size = iFlatToggleGetDefaultCheckSize();
  else
    iupStrToInt(value, &ih->data->check_size);
  if (ih->handle)
    iupdrvRedrawNow(ih);
  return 0;
}

static char* iFlatToggleGetCheckSizeAttrib(Ihandle* ih)
{
  return iupStrReturnInt(ih->data->check_size);
}

static int iFlatToggleSetCheckSpacingAttrib(Ihandle* ih, const char* value)
{
  iupStrToInt(value, &ih->data->check_spacing);
  if (ih->handle)
    iupdrvRedrawNow(ih);
  return 0;
}

static char* iFlatToggleGetCheckSpacingAttrib(Ihandle* ih)
{
  return iupStrReturnInt(ih->data->check_spacing);
}

static int iFlatToggleSetAttribPostRedraw(Ihandle* ih, const char* value)
{
  (void)value;
  if (ih->handle)
    iupdrvPostRedraw(ih);
  return 1;
}

static char* iFlatToggleGetBgColorAttrib(Ihandle* ih)
{
  char* value = iupAttribGet(ih, "BGCOLOR");
  if (!value)
    return iupBaseNativeParentGetBgColorAttrib(ih);
  else
    return value;
}

static char* iFlatToggleGetPaddingAttrib(Ihandle* ih)
{
  return iupStrReturnIntInt(ih->data->horiz_padding, ih->data->vert_padding, 'x');
}

static int iFlatToggleSetImagePositionAttrib(Ihandle* ih, const char* value)
{
  ih->data->img_position = iupFlatGetImagePosition(value);

  if (ih->handle)
    iupdrvRedrawNow(ih);

  return 0;
}

static char* iFlatToggleGetImagePositionAttrib(Ihandle *ih)
{
  char* img_pos2str[4] = { "LEFT", "RIGHT", "TOP", "BOTTOM" };
  return iupStrReturnStr(img_pos2str[ih->data->img_position]);
}

static int iFlatToggleSetSpacingAttrib(Ihandle* ih, const char* value)
{
  iupStrToInt(value, &ih->data->spacing);
  if (ih->handle)
    iupdrvRedrawNow(ih);
  return 0;
}

static char* iFlatToggleGetSpacingAttrib(Ihandle *ih)
{
  return iupStrReturnInt(ih->data->spacing);
}

static int iFlatToggleSetBorderWidthAttrib(Ihandle* ih, const char* value)
{
  iupStrToInt(value, &ih->data->border_width);
  if (ih->handle)
    iupdrvRedrawNow(ih);
  return 0;
}

static char* iFlatToggleGetBorderWidthAttrib(Ihandle *ih)
{
  return iupStrReturnInt(ih->data->border_width);
}

static int iFlatToggleSetValueAttrib(Ihandle* ih, const char* value)
{
  Ihandle* radio = iupRadioFindToggleParent(ih);
  if (radio)
  {
    /* can only set Radio to ON */
    if (iupStrBoolean(value))
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
    int oldcheck = iupAttribGetBoolean(ih, "VALUE");
    if (oldcheck)
      iupAttribSet(ih, "VALUE", "OFF");
    else
      iupAttribSet(ih, "VALUE", "ON");

    if (ih->handle)
      iupdrvRedrawNow(ih);

    return 0;
  }

  if (ih->handle)
    iupdrvPostRedraw(ih);

  return 1;
}

static char* iFlatToggleGetRadioAttrib(Ihandle* ih)
{
  Ihandle* radio = iupRadioFindToggleParent(ih);
  return iupStrReturnBoolean(radio != NULL);
}

static char* iFlatToggleGetHighlightedAttrib(Ihandle* ih)
{
  return iupStrReturnBoolean(ih->data->highlighted);
}

static char* iFlatToggleGetPressedAttrib(Ihandle* ih)
{
  return iupStrReturnBoolean(ih->data->pressed);
}

static char* iFlatToggleGetHasFocusAttrib(Ihandle* ih)
{
  return iupStrReturnBoolean(ih->data->has_focus);
}


/*****************************************************************************************/

static int iFlatToggleCreateMethod(Ihandle* ih, void** params)
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
  ih->data->check_size = iFlatToggleGetDefaultCheckSize();
  ih->data->check_spacing = 5;

  /* initial values - don't use default so they can be set to NULL */
  iupAttribSet(ih, "HLCOLOR", "200 225 245");
  iupAttribSet(ih, "PSCOLOR", "150 200 235");

  /* internal callbacks */
  IupSetCallback(ih, "ACTION", (Icallback)iFlatToggleRedraw_CB);
  IupSetCallback(ih, "BUTTON_CB", (Icallback)iFlatToggleButton_CB);
  IupSetCallback(ih, "FOCUS_CB", (Icallback)iFlatToggleFocus_CB);
  IupSetCallback(ih, "LEAVEWINDOW_CB", iFlatToggleLeaveWindow_CB);
  IupSetCallback(ih, "ENTERWINDOW_CB", iFlatToggleEnterWindow_CB);
  IupSetCallback(ih, "K_CR", (Icallback)iFlatToggleActivate_CB);
  IupSetCallback(ih, "K_SP", (Icallback)iFlatToggleActivate_CB);

  return IUP_NOERROR;
}

static int iFlatToggleMapMethod(Ihandle* ih)
{
  Ihandle* radio = iupRadioFindToggleParent(ih);
  if (radio)
  {
    if (!iupAttribGet(radio, "_IUP_FLATBUTTON_LASTRADIO"))
    {
      /* this is the first toggle in the radio, and then set it with VALUE=ON */
      iupAttribSet(ih, "VALUE", "ON");
    }

    /* make sure it has at least one name */
    if (!iupAttribGetHandleName(ih))
      iupAttribSetHandleName(ih);
  }
  return IUP_NOERROR;
}

static void iFlatToggleComputeNaturalSizeMethod(Ihandle* ih, int *w, int *h, int *children_expand)
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

    if (ih->data->check_size)
    {
      *w += ih->data->check_size;
      *w += ih->data->check_spacing;
      if (ih->data->check_size > *h)
        *h = ih->data->check_size;
    }
  }

  (void)children_expand; /* unset if not a container */
}


/******************************************************************************/


Iclass* iupFlatToggleNewClass(void)
{
  Iclass* ic = iupClassNew(iupRegisterFindClass("canvas"));

  ic->name = "flattoggle";
  ic->format = "s"; /* one string */
  ic->nativetype = IUP_TYPECANVAS;
  ic->childtype = IUP_CHILDNONE;
  ic->is_interactive = 1;

  /* Class functions */
  ic->New = iupFlatToggleNewClass;
  ic->Create = iFlatToggleCreateMethod;
  ic->ComputeNaturalSize = iFlatToggleComputeNaturalSizeMethod;
  ic->Map = iFlatToggleMapMethod;

  /* Callbacks */
  iupClassRegisterCallback(ic, "FLAT_ACTION", "i");
  iupClassRegisterCallback(ic, "FLAT_BUTTON_CB", "iiiis");
  iupClassRegisterCallback(ic, "FLAT_FOCUS_CB", "i");
  iupClassRegisterCallback(ic, "FLAT_ENTERWINDOW_CB", "ii");
  iupClassRegisterCallback(ic, "FLAT_LEAVEWINDOW_CB", "");
  iupClassRegisterCallback(ic, "VALUECHANGED_CB", "");

  /* Overwrite Visual */
  iupClassRegisterAttribute(ic, "ACTIVE", iupBaseGetActiveAttrib, iFlatToggleSetActiveAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_DEFAULT);

  /* Special */
  iupClassRegisterAttribute(ic, "TITLE", NULL, iFlatToggleSetAttribPostRedraw, NULL, NULL, IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);

  /* IupButton only */
  iupClassRegisterAttribute(ic, "VALUE", NULL, iFlatToggleSetValueAttrib, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "RADIO", iFlatToggleGetRadioAttrib, NULL, NULL, NULL, IUPAF_READONLY | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "ALIGNMENT", iFlatToggleGetAlignmentAttrib, iFlatToggleSetAlignmentAttrib, "ACENTER:ACENTER", NULL, IUPAF_NOT_MAPPED | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "PADDING", iFlatToggleGetPaddingAttrib, iFlatToggleSetPaddingAttrib, IUPAF_SAMEASSYSTEM, "10x10", IUPAF_NOT_MAPPED);
  iupClassRegisterAttribute(ic, "SPACING", iFlatToggleGetSpacingAttrib, iFlatToggleSetSpacingAttrib, IUPAF_SAMEASSYSTEM, "2", IUPAF_NOT_MAPPED | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "IGNORERADIO", NULL, NULL, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "HIGHLIGHTED", iFlatToggleGetHighlightedAttrib, NULL, NULL, NULL, IUPAF_READONLY | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "PRESSED", iFlatToggleGetPressedAttrib, NULL, NULL, NULL, IUPAF_READONLY | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "HASFOCUS", iFlatToggleGetHasFocusAttrib, NULL, NULL, NULL, IUPAF_READONLY | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SHOWBORDER", NULL, NULL, NULL, NULL, IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "BORDERCOLOR", NULL, NULL, IUPAF_SAMEASSYSTEM, "50 150 255", IUPAF_DEFAULT);  /* inheritable */
  iupClassRegisterAttribute(ic, "BORDERPSCOLOR", NULL, NULL, NULL, NULL, IUPAF_DEFAULT);  /* inheritable */
  iupClassRegisterAttribute(ic, "BORDERHLCOLOR", NULL, NULL, NULL, NULL, IUPAF_DEFAULT);  /* inheritable */
  iupClassRegisterAttribute(ic, "BORDERWIDTH", iFlatToggleGetBorderWidthAttrib, iFlatToggleSetBorderWidthAttrib, IUPAF_SAMEASSYSTEM, "1", IUPAF_DEFAULT);  /* inheritable */
  iupClassRegisterAttribute(ic, "FGCOLOR", NULL, NULL, "DLGFGCOLOR", NULL, IUPAF_NOT_MAPPED);  /* force the new default value */
  iupClassRegisterAttribute(ic, "BGCOLOR", iFlatToggleGetBgColorAttrib, iFlatToggleSetAttribPostRedraw, IUPAF_SAMEASSYSTEM, "DLGBGCOLOR", IUPAF_NO_SAVE | IUPAF_DEFAULT);
  iupClassRegisterAttribute(ic, "HLCOLOR", NULL, NULL, NULL, NULL, IUPAF_DEFAULT);  /* inheritable */
  iupClassRegisterAttribute(ic, "PSCOLOR", NULL, NULL, NULL, NULL, IUPAF_DEFAULT);  /* inheritable */

  iupClassRegisterAttribute(ic, "IMAGE", NULL, NULL, NULL, NULL, IUPAF_IHANDLENAME | IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "IMAGEPRESS", NULL, NULL, NULL, NULL, IUPAF_IHANDLENAME | IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "IMAGEHIGHLIGHT", NULL, NULL, NULL, NULL, IUPAF_IHANDLENAME | IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "IMAGEINACTIVE", NULL, NULL, NULL, NULL, IUPAF_IHANDLENAME | IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "IMAGEPOSITION", iFlatToggleGetImagePositionAttrib, iFlatToggleSetImagePositionAttrib, IUPAF_SAMEASSYSTEM, "LEFT", IUPAF_NOT_MAPPED | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "TEXTALIGNMENT", NULL, NULL, IUPAF_SAMEASSYSTEM, "ALEFT", IUPAF_NOT_MAPPED | IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "BACKIMAGE", NULL, NULL, NULL, NULL, IUPAF_IHANDLENAME | IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "BACKIMAGEPRESS", NULL, NULL, NULL, NULL, IUPAF_IHANDLENAME | IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "BACKIMAGEHIGHLIGHT", NULL, NULL, NULL, NULL, IUPAF_IHANDLENAME | IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "BACKIMAGEINACTIVE", NULL, NULL, NULL, NULL, IUPAF_IHANDLENAME | IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "FITTOBACKIMAGE", NULL, NULL, NULL, NULL, IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "FRONTIMAGE", NULL, NULL, NULL, NULL, IUPAF_IHANDLENAME | IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "FRONTIMAGEPRESS", NULL, NULL, NULL, NULL, IUPAF_IHANDLENAME | IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "FRONTIMAGEHIGHLIGHT", NULL, NULL, NULL, NULL, IUPAF_IHANDLENAME | IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "FRONTIMAGEINACTIVE", NULL, NULL, NULL, NULL, IUPAF_IHANDLENAME | IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "CHECKRIGHT", NULL, NULL, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "CHECKSIZE", iFlatToggleGetCheckSizeAttrib, iFlatToggleSetCheckSizeAttrib, NULL, NULL, IUPAF_NOT_MAPPED);
  iupClassRegisterAttribute(ic, "CHECKSPACING", iFlatToggleGetCheckSpacingAttrib, iFlatToggleSetCheckSpacingAttrib, IUPAF_SAMEASSYSTEM, "5", IUPAF_NOT_MAPPED);
  iupClassRegisterAttribute(ic, "CHECKBGCOLOR", NULL, NULL, IUPAF_SAMEASSYSTEM, "TXTBGCOLOR", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "CHECKFGCOLOR", NULL, NULL, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "CHECKHLCOLOR", NULL, NULL, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "CHECKPSCOLOR", NULL, NULL, NULL, NULL, IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "CHECKIMAGE", NULL, NULL, NULL, NULL, IUPAF_IHANDLENAME | IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "CHECKIMAGEPRESS", NULL, NULL, NULL, NULL, IUPAF_IHANDLENAME | IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "CHECKIMAGEHIGHLIGHT", NULL, NULL, NULL, NULL, IUPAF_IHANDLENAME | IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "CHECKIMAGEINACTIVE", NULL, NULL, NULL, NULL, IUPAF_IHANDLENAME | IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "CHECKIMAGEON", NULL, NULL, NULL, NULL, IUPAF_IHANDLENAME | IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "CHECKIMAGEONPRESS", NULL, NULL, NULL, NULL, IUPAF_IHANDLENAME | IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "CHECKIMAGEONHIGHLIGHT", NULL, NULL, NULL, NULL, IUPAF_IHANDLENAME | IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "CHECKIMAGEONINACTIVE", NULL, NULL, NULL, NULL, IUPAF_IHANDLENAME | IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "CHECKIMAGENOTDEF", NULL, NULL, NULL, NULL, IUPAF_IHANDLENAME | IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "CHECKIMAGENOTDEFPRESS", NULL, NULL, NULL, NULL, IUPAF_IHANDLENAME | IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "CHECKIMAGENOTDEFHIGHLIGHT", NULL, NULL, NULL, NULL, IUPAF_IHANDLENAME | IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "CHECKIMAGENOTDEFINACTIVE", NULL, NULL, NULL, NULL, IUPAF_IHANDLENAME | IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);

  return ic;
}

Ihandle* IupFlatToggle(const char* title)
{
  void *params[2];
  params[0] = (void*)title;
  params[1] = NULL;
  return IupCreatev("flattoggle", params);
}
