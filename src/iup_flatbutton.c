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
#include "iup_drvdraw.h"
#include "iup_key.h"


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

  /* aux */
  int has_focus,
    highlighted,
    pressed;
};


/****************************************************************/


static char* iFlatButtonMakeImageName(Ihandle* ih, const char* baseattrib, const char* state)
{
  char attrib[1024];
  strcpy(attrib, baseattrib);
  strcat(attrib, state);
  return iupAttribGet(ih, attrib);
}

static const char* iFlatButtonGetImageName(Ihandle* ih, const char* baseattrib, const char* imagename, int active, int *make_inactive)
{
  const char* new_imagename = NULL;

  *make_inactive = 0;

  if (baseattrib)
  {
    if (active)
    {
      if (ih->data->pressed)
        new_imagename = iFlatButtonMakeImageName(ih, baseattrib, "PRESS");
      else
      {
        if (ih->data->highlighted)
          new_imagename = iFlatButtonMakeImageName(ih, baseattrib, "HIGHLIGHT");
      }
    }
    else
    {
      new_imagename = iFlatButtonMakeImageName(ih, baseattrib, "INACTIVE");
      if (!new_imagename)
        *make_inactive = 1;
    }
  }

  if (new_imagename)
    return new_imagename;
  else
    return imagename;
}

static void iFlatButtonDrawImage(Ihandle* ih, IdrawCanvas* dc, int x, int y, const char* baseattrib, const char* imagename, int active)
{
  int make_inactive;
  const char* name = iFlatButtonGetImageName(ih, baseattrib, imagename, active, &make_inactive);
  iupdrvDrawImage(dc, name, make_inactive, x, y);
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
  IdrawCanvas* dc = iupdrvDrawCreateCanvas(ih);
  int make_inactive = 0;

  iupdrvDrawParentBackground(dc);

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
  else if (ih->data->highlighted)
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
    if (ih->data->pressed || selected)
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

  /* draw background */
  if (bgimage)
    iFlatButtonDrawImage(ih, dc, border_width, border_width, "BACKIMAGE", bgimage, active);
  else
    iupFlatDrawBox(dc, border_width, ih->currentwidth - 1 - border_width,
                           border_width, ih->currentheight - 1 - border_width,
                           bgcolor, NULL, 1);  /* always active */

  iupFlatDrawIcon(ih, dc, border_width, border_width,
                  ih->currentwidth - 2 * border_width, ih->currentheight - 2 * border_width,
                  ih->data->img_position, ih->data->spacing, ih->data->horiz_alignment, ih->data->vert_alignment, ih->data->horiz_padding, ih->data->vert_padding,
                  iFlatButtonGetImageName(ih, "IMAGE", image, active, &make_inactive), make_inactive, title, fgcolor, bgcolor, active);

  if (fgimage)
    iFlatButtonDrawImage(ih, dc, border_width, border_width, "FRONTIMAGE", fgimage, active);
  else if (!image && !title)
  {
    int space = border_width + 2;
    iupFlatDrawBorder(dc, space, ih->currentwidth - 1 - space,
                              space, ih->currentheight - 1 - space,
                              1, "0 0 0", bgcolor, active);
    space++;
    iupFlatDrawBox(dc, space, ih->currentwidth - 1 - space,
                           space, ih->currentheight - 1 - space,
                           fgcolor, bgcolor, active);
  }


  if (selected && !old_pressed && (bgimage || image))
    ih->data->pressed = 0;

  if (ih->data->has_focus)
    iupdrvDrawFocusRect(dc, border_width, border_width, ih->currentwidth - border_width, ih->currentheight - border_width);

  iupdrvDrawFlush(dc);

  iupdrvDrawKillCanvas(dc);

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

  ih->data->highlighted = 1;
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

  ih->data->highlighted = 0;
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

 ih->data->horiz_alignment = iupFlatGetHorizontalAlignment(value1);
 ih->data->vert_alignment = iupFlatGetVerticalAlignment(value2);

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
  ih->data->img_position = iupFlatGetImagePosition(value);

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

static char* iFlatButtonGetHighlightedAttrib(Ihandle* ih)
{
  return iupStrReturnBoolean(ih->data->highlighted);
}

static char* iFlatButtonGetPressedAttrib(Ihandle* ih)
{
  return iupStrReturnBoolean(ih->data->pressed);
}

static char* iFlatButtonGetHasFocusAttrib(Ihandle* ih)
{
  return iupStrReturnBoolean(ih->data->has_focus);
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

      /* make sure it has at least one name */
      if (!iupAttribGetHandleName(ih))
        iupAttribSetHandleName(ih);
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
  iupClassRegisterAttribute(ic, "IGNORERADIO", NULL, NULL, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "HIGHLIGHTED", iFlatButtonGetHighlightedAttrib, NULL, NULL, NULL, IUPAF_READONLY | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "PRESSED", iFlatButtonGetPressedAttrib, NULL, NULL, NULL, IUPAF_READONLY | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "HASFOCUS", iFlatButtonGetHasFocusAttrib, NULL, NULL, NULL, IUPAF_READONLY | IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "BORDERCOLOR", NULL, NULL, IUPAF_SAMEASSYSTEM, "50 150 255", IUPAF_DEFAULT);  /* inheritable */
  iupClassRegisterAttribute(ic, "BORDERPSCOLOR", NULL, NULL, NULL, NULL, IUPAF_DEFAULT);  /* inheritable */
  iupClassRegisterAttribute(ic, "BORDERHLCOLOR", NULL, NULL, NULL, NULL, IUPAF_DEFAULT);  /* inheritable */
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
