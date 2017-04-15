/** \file
 * \brief GLToggle Control.
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "iup.h"
#include "iupcbs.h"
#include "iupglcontrols.h"

#include "iup_object.h"
#include "iup_attrib.h"
#include "iup_str.h"
#include "iup_register.h"

#include "iup_glcontrols.h"
#include "iup_gldraw.h"
#include "iup_glicon.h"
#include "iup_glsubcanvas.h"


/* from IupRadio implementation */
Ihandle *iupRadioFindToggleParent(Ihandle* ih_toggle);

/* from IupGLButton implementation */
void iupGLButtonDraw(Ihandle* ih);

static void iGLToggleDrawCheckMark(Ihandle* ih)
{
  char *image = iupAttribGet(ih, "IMAGE");
  char* title = iupAttribGet(ih, "TITLE");
  int active = iupAttribGetInt(ih, "ACTIVE");
  int pressed = iupAttribGetInt(ih, "PRESSED");
  int highlight = iupAttribGetInt(ih, "HIGHLIGHT");
  char* fgcolor = iupAttribGetStr(ih, "FGCOLOR");
  char* bgcolor = iupAttribGetStr(ih, "BGCOLOR");
  float bwidth = iupAttribGetFloat(ih, "BORDERWIDTH");
  int border_width = (int)ceil(bwidth);
  int check_width = iupAttribGetInt(ih, "CHECKMARKWIDTH");
  int spacing = iupAttribGetInt(ih, "SPACING");
  Ihandle* radio = iupRadioFindToggleParent(ih);
  int check = iupAttribGetBoolean(ih, "VALUE");
  int rightbutton = iupAttribGetBoolean(ih, "RIGHTBUTTON");
  char* bordercolor = iupAttribGetStr(ih, "BORDERCOLOR");
  int y = (ih->currentheight - check_width) / 2;
  int x1 = 0;
  int x2 = check_width + spacing;

  if (pressed)
  {
    char* presscolor = iupAttribGetStr(ih, "PSCOLOR");
    if (presscolor)
      bgcolor = presscolor;
  }
  else if (highlight)
  {
    char* hlcolor = iupAttribGetStr(ih, "HLCOLOR");
    if (hlcolor)
      bgcolor = hlcolor;
  }
  if (!bgcolor)
    bgcolor = "255 255 255";

  if (rightbutton)
  {
    x1 = ih->currentwidth - check_width - border_width;
    x2 = 0;
  }

  if (radio)
  {
    iupGLDrawSmallDisc(ih, x1 + check_width / 2, y + check_width / 2, check_width / 2, bgcolor, 1);
    iupGLDrawSmallCircle(ih, x1 + check_width / 2, y + check_width / 2, check_width / 2, bwidth, bordercolor, active);

    if (check)
    {
      iupGLDrawSmallDisc(ih, x1 + check_width / 2, y + check_width / 2, check_width / 4, bordercolor, active);
      iupGLDrawSmallCircle(ih, x1 + check_width / 2, y + check_width / 2, check_width / 4, bwidth, bordercolor, active);
    }
  }
  else
  {
    iupGLDrawBox(ih, x1 + border_width, x1 + check_width - border_width, y + border_width, y + check_width - border_width, bgcolor, 1);
    iupGLDrawRect(ih, x1, x1 + check_width, y, y + check_width, bwidth, bordercolor, active, 0);

    if (check)
    {
      int points[6];

      points[0] = x1 + 3;
      points[1] = y + check_width / 2;
      points[2] = x1 + check_width / 2;
      points[3] = y + check_width - 3;
      points[4] = x1 + check_width - 3;
      points[5] = y + 3;

      iupGLDrawPolyline(ih, points, 3, 3, bordercolor, active, 0);
    }
  }

  iupGLIconDraw(ih, x2, 0,
                    ih->currentwidth - check_width - spacing, ih->currentheight,
                    "IMAGE", image, title, fgcolor, active);
}

static int iGLToggleACTION(Ihandle* ih)
{
  int checkmark = iupAttribGetInt(ih, "CHECKMARK");
  if (checkmark)
    iGLToggleDrawCheckMark(ih);
  else
    iupGLButtonDraw(ih);

  return IUP_DEFAULT;
}

static void iGLToggleNotify(Ihandle* ih, int selected)
{
  IFni cb = (IFni)IupGetCallback(ih, "ACTION");
  if (cb)
  {
    int ret = cb(ih, selected);
    if (ret == IUP_CLOSE)
      IupExitLoop();
  }

  if (iupObjectCheck(ih))
    iupBaseCallValueChangedCb(ih);
}

static int iGLToggleBUTTON_CB(Ihandle* ih, int button, int pressed, int x, int y, char* status)
{
  if (button == IUP_BUTTON1)
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
          last_tg = (Ihandle*)iupAttribGet(radio, "_IUP_GLTOGGLE_LASTRADIO");
          if (iupObjectCheck(last_tg) && last_tg != ih)
            iupAttribSet(last_tg, "VALUE", "OFF");
          else
            last_tg = NULL;

          iupAttribSet(radio, "_IUP_GLTOGGLE_LASTRADIO", (char*)ih);
        }

        iupAttribSet(ih, "VALUE", "ON");
        selected = 1;
      }
    }

    /* "PRESSED" was already updated */
    iupGLSubCanvasRedraw(ih);

    if (!pressed)
    {
      if (last_tg && ih != last_tg)
        iGLToggleNotify(last_tg, 0);

      if (!radio || ih != last_tg)
        iGLToggleNotify(ih, selected);
    }
  }

  (void)x;
  (void)y;
  (void)status;
  return IUP_DEFAULT;
}

static int iGLToggleSetValueAttrib(Ihandle* ih, const char* value)
{
  Ihandle* radio = iupRadioFindToggleParent(ih);
  if (radio)
  {
    /* can only set Radio to ON */
    if (iupStrEqualNoCase(value, "TOGGLE") || iupStrBoolean(value))
    {
      Ihandle* last_tg = (Ihandle*)iupAttribGet(radio, "_IUP_GLTOGGLE_LASTRADIO");
      if (iupObjectCheck(last_tg) && last_tg != ih)
        iupAttribSet(last_tg, "VALUE", "OFF");

      iupAttribSet(radio, "_IUP_GLTOGGLE_LASTRADIO", (char*)ih);
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

      return 0;
    }
  }

  return 1;
}

static char* iGLToggleGetRadioAttrib(Ihandle* ih)
{
  Ihandle* radio = iupRadioFindToggleParent(ih);
  return iupStrReturnBoolean(radio!=NULL);
}

static void iGLToggleComputeNaturalSizeMethod(Ihandle* ih, int *w, int *h, int *children_expand)
{
  int check = iupAttribGetInt(ih, "CHECKMARK");
  if (check)
  {
    int check_width = iupAttribGetInt(ih, "CHECKMARKWIDTH");
    int spacing = iupAttribGetInt(ih, "SPACING");
    float bwidth = iupAttribGetFloat(ih, "BORDERWIDTH");
    int border_width = (int)ceil(bwidth);
    if (border_width == 0)
      return;

    /* undo add to the label natural size */
    *w -= 2 * border_width;
    *h -= 2 * border_width;

    /* add to the label natural size */
    *w += check_width + spacing;
    *h = iupMAX(*h, check_width);
  }

  (void)children_expand; /* unset if not a container */
}

static int iGLToggleMapMethod(Ihandle* ih)
{
  Ihandle* radio = iupRadioFindToggleParent(ih);
  if (radio)
  {
    if (!iupAttribGet(radio, "_IUP_GLTOGGLE_LASTRADIO"))
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

static int iGLToggleCreateMethod(Ihandle* ih, void** params)
{
  IupSetCallback(ih, "GL_ACTION", iGLToggleACTION);
  IupSetCallback(ih, "GL_BUTTON_CB", (Icallback)iGLToggleBUTTON_CB);

  (void)params; /* button create already parsed title */
  return IUP_NOERROR;
}


/******************************************************************************/


Iclass* iupGLToggleNewClass(void)
{
  Iclass* ic = iupClassNew(iupRegisterFindClass("glbutton"));

  ic->name = "gltoggle";
  ic->format = "s"; /* one string */
  ic->nativetype = IUP_TYPEVOID;
  ic->childtype = IUP_CHILDNONE;
  ic->is_interactive = 0;

  /* Class functions */
  ic->New = iupGLToggleNewClass;
  ic->Create = iGLToggleCreateMethod;
  ic->ComputeNaturalSize = iGLToggleComputeNaturalSizeMethod;
  ic->Map = iGLToggleMapMethod;

  iupClassRegisterCallback(ic, "ACTION", "i");
  iupClassRegisterCallback(ic, "VALUECHANGED_CB", "");

  iupClassRegisterAttribute(ic, "VALUE", NULL, iGLToggleSetValueAttrib, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "RADIO", iGLToggleGetRadioAttrib, NULL, NULL, NULL, IUPAF_READONLY | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "CHECKMARK", NULL, NULL, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "CHECKMARKWIDTH", NULL, NULL, IUPAF_SAMEASSYSTEM, "14", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "RIGHTBUTTON", NULL, NULL, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "IGNORERADIO", NULL, NULL, NULL, NULL, IUPAF_NO_INHERIT);

  return ic;
}

Ihandle* IupGLToggle(const char* title)
{
  void *params[2];
  params[0] = (void*)title;
  params[1] = NULL;
  return IupCreatev("gltoggle", params);
}

