/** \file
 * \brief DropButton Control
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
#include "iup_draw.h"
#include "iup_key.h"



struct _IcontrolData 
{
  iupCanvas canvas;  /* from IupCanvas (must reserve it) */

  /* attributes */
  int horiz_padding, vert_padding;  /* button margin */
  int spacing, img_position;        /* used when both text and image are displayed */
  int horiz_alignment, vert_alignment;  
  int border_width;
  int arrow_size, arrow_padding;

  /* draw aux - state */
  int has_focus,
      highlighted,
      pressed, 
      over_arrow,
      dropped;

  Ihandle* dropchild;
  Ihandle* dropdialog;
};


enum { IUP_POS_BOTTOMLEFT, IUP_POS_TOPLEFT, IUP_POS_BOTTOMRIGHT, IUP_POS_TOPRIGHT };


/****************************************************************/


static int iDropButtonRedraw_CB(Ihandle* ih)
{
  char *image = iupAttribGet(ih, "IMAGE");
  char* title = iupAttribGet(ih, "TITLE");
  int active = IupGetInt(ih, "ACTIVE");  /* native implementation */
  char* fgcolor = iupAttribGetStr(ih, "FGCOLOR");
  char* bgcolor = iupAttribGet(ih, "BGCOLOR");  /* don't get with default value, if NULL will use from parent */
  char* bgimage = iupAttribGet(ih, "BACKIMAGE");
  char* fgimage = iupAttribGet(ih, "FRONTIMAGE");
  char* text_align = iupAttribGetStr(ih, "TEXTALIGNMENT");
  const char* draw_image;
  int border_width = ih->data->border_width;
  int draw_border = iupAttribGetBoolean(ih, "SHOWBORDER");
  int image_pressed;
  int drop_onarrow = iupAttribGetBoolean(ih, "DROPONARROW");
  int arrow_active = iupAttribGetBoolean(ih, "ARROWACTIVE");
  IdrawCanvas* dc = iupdrvDrawCreateCanvas(ih);
  int make_inactive = 0;
  char* bgcolor_button, *bgcolor_arrow;

  iupDrawParentBackground(dc, ih);

  if (!bgcolor)
  {
    if (draw_border)
      bgcolor = iupFlatGetDarkerBgColor(ih);
    else
      bgcolor = iupBaseNativeParentGetBgColorAttrib(ih);
  }

  if (arrow_active && !active)
    arrow_active = 0;
  if (arrow_active && !ih->data->dropchild)
    arrow_active = 0;

  bgcolor_button = bgcolor;
  bgcolor_arrow = bgcolor;

  if (ih->data->pressed || (ih->data->dropped && !ih->data->highlighted))
  {
    char* presscolor = iupAttribGetStr(ih, "PSCOLOR");
    if (presscolor)
    {
      if (drop_onarrow && ih->data->over_arrow)
        bgcolor_arrow = presscolor;
      else
        bgcolor_button = presscolor;
    }

    draw_border = 1;
  }
  else if (ih->data->highlighted)
  {
    char* hlcolor = iupAttribGetStr(ih, "HLCOLOR");
    if (hlcolor)
    {
      if (drop_onarrow && ih->data->over_arrow)
        bgcolor_arrow = hlcolor;
      else
        bgcolor_button = hlcolor;
    }

    draw_border = 1;
  }

  if (!arrow_active)
    bgcolor_arrow = bgcolor;

  /* draw border - can still be removed by setting border_width=0 */
  if (draw_border)
  {
    char* bordercolor = iupAttribGetStr(ih, "BORDERCOLOR");

    if (ih->data->pressed || (ih->data->dropped && !ih->data->highlighted))
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

    if (drop_onarrow)
    {
      iupFlatDrawBorder(dc, 0, ih->currentwidth - 1 - ih->data->arrow_size,
                            0, ih->currentheight - 1,
                            border_width, bordercolor, bgcolor_button, active);
      iupFlatDrawBorder(dc, ih->currentwidth - 1 - ih->data->arrow_size - border_width, ih->currentwidth - 1,
                            0, ih->currentheight - 1,
                            border_width, bordercolor, bgcolor_arrow, active);
    }
    else
    {
      iupFlatDrawBorder(dc, 0, ih->currentwidth - 1,
                            0, ih->currentheight - 1,
                            border_width, bordercolor, bgcolor_button, active);
    }
  }

  /* simulate pressed when dropped and has images (but colors and borders are not included) */
  image_pressed = ih->data->pressed;
  if (ih->data->dropped && !ih->data->pressed && (bgimage || image))
    image_pressed = 1;

  /* draw background */
  if (bgimage)
  {
    draw_image = iupFlatGetImageName(ih, "BACKIMAGE", bgimage, image_pressed, ih->data->highlighted, active, &make_inactive);
    iupdrvDrawImage(dc, draw_image, make_inactive, border_width, border_width);
  }
  else
  {
    if (drop_onarrow)
    {
      iupFlatDrawBox(dc, border_width, ih->currentwidth - 1 - border_width - ih->data->arrow_size,
                         border_width, ih->currentheight - 1 - border_width,
                         bgcolor_button, NULL, 1);  /* background is always active */

      iupFlatDrawBox(dc, ih->currentwidth - 1 - ih->data->arrow_size + border_width, ih->currentwidth - 1 - border_width,
                         border_width, ih->currentheight - 1 - border_width,
                         bgcolor_arrow, NULL, 1);  /* background is always active */
    }
    else
      iupFlatDrawBox(dc, border_width, ih->currentwidth - 1 - border_width,
                         border_width, ih->currentheight - 1 - border_width,
                         bgcolor_button, NULL, 1);  /* background is always active */
  }

  draw_image = iupFlatGetImageName(ih, "IMAGE", image, image_pressed, ih->data->highlighted, active, &make_inactive);
  iupFlatDrawIcon(ih, dc, border_width, border_width,
                  ih->currentwidth - 2 * border_width - ih->data->arrow_size, ih->currentheight - 2 * border_width,
                  ih->data->img_position, ih->data->spacing, ih->data->horiz_alignment, ih->data->vert_alignment, ih->data->horiz_padding, ih->data->vert_padding,
                  draw_image, make_inactive, title, text_align, fgcolor, bgcolor_button, active);

  if (fgimage)
  {
    draw_image = iupFlatGetImageName(ih, "FRONTIMAGE", fgimage, image_pressed, ih->data->highlighted, active, &make_inactive);
    iupdrvDrawImage(dc, draw_image, make_inactive, border_width, border_width);
  }
  else if (!image && !title) /* color only button */
  {
    int space = border_width + 2;
    iupFlatDrawBorder(dc, space, ih->currentwidth - 1 - space - ih->data->arrow_size,
                          space, ih->currentheight - 1 - space,
                          1, "0 0 0", bgcolor_button, active);
    space++;
    iupFlatDrawBox(dc, space, ih->currentwidth - 1 - space - ih->data->arrow_size,
                       space, ih->currentheight - 1 - space,
                       fgcolor, bgcolor_button, active);
  }

  int arrow_images = iupAttribGetInt(ih, "ARROWIMAGES");
  if (arrow_images)
  {
    int make_inactive;
    const char* image;

    image = iupFlatGetImageName(ih, "ARROWIMAGE", NULL, arrow_active ? ih->data->pressed : 0, arrow_active ? ih->data->highlighted : 0, arrow_active, &make_inactive);
    iupdrvDrawImage(dc, image, make_inactive, 
                    ih->currentwidth - 1 - ih->data->arrow_size - border_width + ih->data->arrow_padding, 
                    (ih->currentheight - ih->data->arrow_size) / 2 + ih->data->arrow_padding);
  }
  else
  {
    char* arrow_color = iupAttribGet(ih, "ARROWCOLOR");
    if (!arrow_color)
      arrow_color = fgcolor;
    
    iupFlatDrawArrow(dc, ih->currentwidth - 1 - ih->data->arrow_size - border_width + ih->data->arrow_padding,
                         (ih->currentheight - ih->data->arrow_size)/2 + ih->data->arrow_padding,
                         ih->data->arrow_size - 2 * ih->data->arrow_padding, arrow_color, bgcolor_arrow, arrow_active, IUPDRAW_ARROW_BOTTOM);
  }

  if (ih->data->has_focus)
    iupdrvDrawFocusRect(dc, border_width, border_width, ih->currentwidth - 1 - border_width, ih->currentheight - 1 - border_width);

  iupdrvDrawFlush(dc);

  iupdrvDrawKillCanvas(dc);

  return IUP_DEFAULT;
}

static int iDropButtonGetDropPosition(Ihandle* ih)
{
  char* value = iupAttribGetStr(ih, "DROPPOSITION");
  if (iupStrEqualNoCase(value, "TOPLEFT"))
    return IUP_POS_TOPLEFT;
  if (iupStrEqualNoCase(value, "BOTTOMRIGHT"))
    return IUP_POS_BOTTOMRIGHT;
  if (iupStrEqualNoCase(value, "TOPRIGHT"))
    return IUP_POS_TOPRIGHT;
  return IUP_POS_BOTTOMLEFT;
}

static void iDropButtonShowDrop(Ihandle* ih)
{
  IFni cb;

  if (!ih->data->dropchild)
    return;

  cb = (IFni)IupGetCallback(ih, "DROPDOWN_CB");
  if (cb)
    cb(ih, ih->data->dropped);

  if (ih->data->dropped)
  {
    int drop_pos = iDropButtonGetDropPosition(ih);
    int x, y;

    Ihandle* parent_dialog = IupGetAttributeHandle(ih->data->dropdialog, "PARENTDIALOG");
    Ihandle* dialog = IupGetDialog(ih);
    if (dialog != parent_dialog)
      IupSetAttributeHandle(ih->data->dropdialog, "PARENTDIALOG", dialog);

    x = IupGetInt(ih, "X");
    y = IupGetInt(ih, "Y");

    /* update drop dialog size before calc pos */
    IupRefresh(ih->data->dropdialog);

    switch (drop_pos)
    {
    case IUP_POS_TOPLEFT:
      y -= ih->data->dropdialog->currentheight;
      break;
    case IUP_POS_BOTTOMRIGHT:
      x += ih->currentwidth - ih->data->dropdialog->currentwidth;
      y += ih->currentheight;
      break;
    case IUP_POS_TOPRIGHT:
      x += ih->currentwidth - ih->data->dropdialog->currentwidth;
      y -= ih->data->dropdialog->currentheight;
      break;
    default: /* IUP_POS_BOTTOMLEFT */
      y += ih->currentheight;
      break;
    }

    iupdrvRedrawNow(ih);
    IupShowXY(ih->data->dropdialog, x, y);
  }
  else
  {
    IupHide(ih->data->dropdialog);
    iupdrvRedrawNow(ih);
  }

  cb = (IFni)IupGetCallback(ih, "DROPSHOW_CB");
  if (cb)
    cb(ih, ih->data->dropped);
}

static void iDropButtonNotify(Ihandle* ih, int pressed)
{
  if (pressed && ih->data->over_arrow)
  {
    int arrow_active = iupAttribGetBoolean(ih, "ARROWACTIVE");
    if (arrow_active && ih->data->dropchild)
    {
      if (iupAttribGet(ih, "_IUPDROP_CLOSE_ON_FOCUS"))
        return;

      ih->data->dropped = !ih->data->dropped;
      iDropButtonShowDrop(ih);
    }

    return;
  }

  if (!pressed && !ih->data->over_arrow)
  {
    Icallback cb;

    if (ih->data->dropped && ih->data->dropchild)
    {
      ih->data->dropped = 0;
      iDropButtonShowDrop(ih);
    }

    cb = IupGetCallback(ih, "FLAT_ACTION");
    if (cb)
    {
      int ret = cb(ih);
      if (ret == IUP_CLOSE)
        IupExitLoop();
    }
  }

  if (!pressed)
    iupAttribSet(ih, "_IUPDROP_CLOSE_ON_FOCUS", NULL);

  iupdrvRedrawNow(ih);
}

static int iDropButtonMotion_CB(Ihandle* ih, int x, int y, char* status)
{
  int drop_onarrow = iupAttribGetBoolean(ih, "DROPONARROW");
  int over_arrow = 1;
  IFniis cb = (IFniis)IupGetCallback(ih, "FLAT_MOTION_CB");
  if (cb)
  {
    if (cb(ih, x, y, status) == IUP_IGNORE)
      return IUP_DEFAULT;
  }

  if (drop_onarrow && (x < ih->currentwidth - ih->data->arrow_size))
      over_arrow = 0;

  if (over_arrow != ih->data->over_arrow)
  {
    ih->data->over_arrow = over_arrow;
    iupdrvRedrawNow(ih);
  }

  return IUP_DEFAULT;
}

static int iDropButtonButton_CB(Ihandle* ih, int button, int pressed, int x, int y, char* status)
{
  IFniiiis cb = (IFniiiis)IupGetCallback(ih, "FLAT_BUTTON_CB");
  if (cb)
  {
    if (cb(ih, button, pressed, x, y, status) == IUP_IGNORE)
      return IUP_DEFAULT;
  }

  if (button == IUP_BUTTON1)
  {
    int drop_onarrow = iupAttribGetBoolean(ih, "DROPONARROW");
    if (drop_onarrow && (x < ih->currentwidth - ih->data->arrow_size))
      ih->data->over_arrow = 0;
    else
      ih->data->over_arrow = 1;

    ih->data->pressed = pressed;

    iDropButtonNotify(ih, pressed);
  }

  return IUP_DEFAULT;
}

static int iDropButtonFocus_CB(Ihandle* ih, int focus)
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

static int iDropButtonEnterWindow_CB(Ihandle* ih)
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

static int iDropButtonLeaveWindow_CB(Ihandle* ih)
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

static int iDropButtonActivate_CB(Ihandle* ih)
{
  char status[IUPKEY_STATUS_SIZE] = IUPKEY_STATUS_INIT;

  iDropButtonButton_CB(ih, IUP_BUTTON1, 1, 0, 0, status);

  iupdrvSleep(100);

  iDropButtonButton_CB(ih, IUP_BUTTON1, 0, 0, 0, status);

  return IUP_DEFAULT;
}


/***********************************************************************************************/


static int iDropButtonSetShowDropdownAttrib(Ihandle* ih, const char* value)
{
  if (ih->data->dropchild)
  {
    iupAttribSet(ih, "_IUPDROP_CLOSE_ON_FOCUS", NULL);
    ih->data->dropped = iupStrBoolean(value);
    iDropButtonShowDrop(ih);
  }
  return 0;
}

static int iDropButtonSetActiveAttrib(Ihandle* ih, const char* value)
{
  iupBaseSetActiveAttrib(ih, value);
  iupdrvRedrawNow(ih);
  return 0; 
}

static int iDropButtonSetAlignmentAttrib(Ihandle* ih, const char* value)
{
  char value1[30], value2[30];

  iupStrToStrStr(value, value1, value2, ':');

 ih->data->horiz_alignment = iupFlatGetHorizontalAlignment(value1);
 ih->data->vert_alignment = iupFlatGetVerticalAlignment(value2);

  if (ih->handle)
    iupdrvRedrawNow(ih);

  return 1;
}

static char* iDropButtonGetAlignmentAttrib(Ihandle *ih)
{
  char* horiz_align2str[3] = {"ALEFT", "ACENTER", "ARIGHT"};
  char* vert_align2str[3] = {"ATOP", "ACENTER", "ABOTTOM"};
  return iupStrReturnStrf("%s:%s", horiz_align2str[ih->data->horiz_alignment], vert_align2str[ih->data->vert_alignment]);
}

static int iDropButtonSetPaddingAttrib(Ihandle* ih, const char* value)
{
  iupStrToIntInt(value, &ih->data->horiz_padding, &ih->data->vert_padding, 'x');
  if (ih->handle)
    iupdrvRedrawNow(ih);
  return 0;
}

static int iDropButtonSetAttribPostRedraw(Ihandle* ih, const char* value)
{
  (void)value;
  if (ih->handle)
    iupdrvPostRedraw(ih);
  return 1;
}

static char* iDropButtonGetBgColorAttrib(Ihandle* ih)
{
  char* value = iupAttribGet(ih, "BGCOLOR");
  if (!value)
    return iupBaseNativeParentGetBgColorAttrib(ih);
  else
    return value;
}

static char* iDropButtonGetPaddingAttrib(Ihandle* ih)
{
  return iupStrReturnIntInt(ih->data->horiz_padding, ih->data->vert_padding, 'x');
}

static int iDropButtonSetImagePositionAttrib(Ihandle* ih, const char* value)
{
  ih->data->img_position = iupFlatGetImagePosition(value);

  if (ih->handle)
    iupdrvRedrawNow(ih);

  return 0;
}

static char* iDropButtonGetImagePositionAttrib(Ihandle *ih)
{
  char* img_pos2str[4] = {"LEFT", "RIGHT", "TOP", "BOTTOM"};
  return iupStrReturnStr(img_pos2str[ih->data->img_position]);
}

static int iDropButtonSetSpacingAttrib(Ihandle* ih, const char* value)
{
  iupStrToInt(value, &ih->data->spacing);
  if (ih->handle)
    iupdrvRedrawNow(ih);
  return 0;
}

static char* iDropButtonGetSpacingAttrib(Ihandle *ih)
{
  return iupStrReturnInt(ih->data->spacing);
}

static int iDropButtonSetBorderWidthAttrib(Ihandle* ih, const char* value)
{
  iupStrToInt(value, &ih->data->border_width);
  if (ih->handle)
    iupdrvRedrawNow(ih);
  return 0;
}

static char* iDropButtonGetBorderWidthAttrib(Ihandle *ih)
{
  return iupStrReturnInt(ih->data->border_width);
}

static int iDropButtonSetArrowSizeAttrib(Ihandle* ih, const char* value)
{
  iupStrToInt(value, &ih->data->arrow_size);
  if (ih->handle)
    iupdrvRedrawNow(ih);
  return 0;
}

static char* iDropButtonGetArrowSizeAttrib(Ihandle *ih)
{
  return iupStrReturnInt(ih->data->arrow_size);
}

static int iDropButtonSetArrowPaddingAttrib(Ihandle* ih, const char* value)
{
  iupStrToInt(value, &ih->data->arrow_padding);
  if (ih->handle)
    iupdrvRedrawNow(ih);
  return 0;
}

static char* iDropButtonGetArrowPaddingAttrib(Ihandle *ih)
{
  return iupStrReturnInt(ih->data->arrow_padding);
}

static char* iDropButtonGetHighlightedAttrib(Ihandle* ih)
{
  return iupStrReturnBoolean(ih->data->highlighted);
}

static char* iDropButtonGetPressedAttrib(Ihandle* ih)
{
  return iupStrReturnBoolean(ih->data->pressed);
}

static char* iDropButtonGetHasFocusAttrib(Ihandle* ih)
{
  return iupStrReturnBoolean(ih->data->has_focus);
}

static void iDropButtonSetDropChild(Ihandle* ih, Ihandle* dropchild)
{
  if (ih->data->dropchild != dropchild)
  {
    if (ih->data->dropchild)
      IupDetach(ih->data->dropchild);

    /* make sure it has at least one name */
    if (!iupAttribGetHandleName(dropchild))
      iupAttribSetHandleName(dropchild);

    ih->data->dropchild = dropchild;

    IupAppend(ih->data->dropdialog, ih->data->dropchild);

    if (ih->data->dropdialog->handle)
      IupMap(ih->data->dropchild);
  }
}

static int iDropButtonSetDropChildHandleAttrib(Ihandle* ih, const char* value)
{
  iDropButtonSetDropChild(ih, (Ihandle*)value);
  return 0;
}

static char* iDropButtonGetDropChildHandleAttrib(Ihandle* ih)
{
  return (char*)ih->data->dropchild;
}

static int iDropButtonSetDropChildAttrib(Ihandle* ih, const char* value)
{
  iDropButtonSetDropChild(ih, IupGetHandle(value));
  return 0;
}

static char* iDropButtonGetDropChildAttrib(Ihandle* ih)
{
  return IupGetName(ih->data->dropchild);
}


/*****************************************************************************************/


static int iDropButtonMouseOnTop(Ihandle* ih)
{
  int cur_x, cur_y;
  int x = IupGetInt(ih, "X");
  int y = IupGetInt(ih, "Y");
  IupGetIntInt(NULL, "CURSORPOS", &cur_x, &cur_y);
  if (cur_x < x || cur_y < y ||
      cur_x > x + ih->currentwidth || cur_y > y + ih->currentheight)
    return 0;
  else
    return 1;
}

static int iDropButtonDialogFocusCB(Ihandle* dlg, int focus)
{
  Ihandle* ih = IupGetAttributeHandle(dlg, "DROPBUTTON");
  /* when drop dialog loses its focus it must be closed */
  if (!focus && ih->data->dropped)
  {
    /* count not use highlighted because in GTK we get a leavewindow when clicking the button and the dialog is dropped */
    if (iDropButtonMouseOnTop(ih))
      iupAttribSet(ih, "_IUPDROP_CLOSE_ON_FOCUS", "1");
    else
      iupAttribSet(ih, "_IUPDROP_CLOSE_ON_FOCUS", NULL);

    ih->data->dropped = 0;
    iDropButtonShowDrop(ih);
  }
  return IUP_DEFAULT;
}

static int iDropButtonDialogKeyEscCB(Ihandle* dlg)
{
  Ihandle* ih = IupGetAttributeHandle(dlg, "DROPBUTTON");
  ih->data->dropped = 0;
  iDropButtonShowDrop(ih);
  return IUP_DEFAULT;
}

static Ihandle* iDropButtonCreateDropDialog(Ihandle* ih)
{
  Ihandle* dlg = IupDialog(NULL);

  IupSetAttribute(dlg, "RESIZE", "NO");
  IupSetAttribute(dlg, "MENUBOX", "NO");
  IupSetAttribute(dlg, "MAXBOX", "NO");
  IupSetAttribute(dlg, "MINBOX", "NO");

  IupSetAttributeHandle(dlg, "DROPBUTTON", ih);

  IupSetCallback(dlg, "FOCUS_CB", (Icallback)iDropButtonDialogFocusCB);
  IupSetCallback(dlg, "K_ESC", (Icallback)iDropButtonDialogKeyEscCB);

  return dlg;
}

static int iDropButtonCreateMethod(Ihandle* ih, void** params)
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
  ih->data->horiz_alignment = IUP_ALIGN_ALEFT;
  ih->data->vert_alignment = IUP_ALIGN_ACENTER;
  ih->data->arrow_size = 24;
  ih->data->arrow_padding = 5;
  ih->data->horiz_padding = 3;
  ih->data->vert_padding = 3;

  /* initial values - don't use default so they can be set to NULL */
  iupAttribSet(ih, "HLCOLOR", "200 225 245");
  iupAttribSet(ih, "PSCOLOR", "150 200 235");

  /* internal callbacks */
  IupSetCallback(ih, "ACTION", (Icallback)iDropButtonRedraw_CB);
  IupSetCallback(ih, "BUTTON_CB", (Icallback)iDropButtonButton_CB);
  IupSetCallback(ih, "MOTION_CB", (Icallback)iDropButtonMotion_CB);
  IupSetCallback(ih, "FOCUS_CB", (Icallback)iDropButtonFocus_CB);
  IupSetCallback(ih, "LEAVEWINDOW_CB", iDropButtonLeaveWindow_CB);
  IupSetCallback(ih, "ENTERWINDOW_CB", iDropButtonEnterWindow_CB);
  IupSetCallback(ih, "K_CR", (Icallback)iDropButtonActivate_CB);
  IupSetCallback(ih, "K_SP", (Icallback)iDropButtonActivate_CB);

  ih->data->dropdialog = iDropButtonCreateDropDialog(ih);

  if (params && params[0])
    iDropButtonSetDropChild(ih, (Ihandle*)(params[0]));

  return IUP_NOERROR;
}

static void iDropButtonComputeNaturalSizeMethod(Ihandle* ih, int *w, int *h, int *children_expand)
{
  int fit2backimage = iupAttribGetBoolean(ih, "FITTOBACKIMAGE");
  char* bgimage = iupAttribGet(ih, "BACKIMAGE");
  if (fit2backimage && bgimage)
    iupImageGetInfo(bgimage, w, h, NULL);
  else
  {
    int visiblecolumns;
    char* image = iupAttribGet(ih, "IMAGE");
    char* title = iupAttribGet(ih, "TITLE");

    *w = 0;
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

    visiblecolumns = iupAttribGetInt(ih, "VISIBLECOLUMNS");
    if (visiblecolumns)
    {
      *w = iupdrvFontGetStringWidth(ih, "WWWWWWWWWW");
      *w = (visiblecolumns*(*w)) / 10;
    }

    *w += 2 * ih->data->horiz_padding;
    *h += 2 * ih->data->vert_padding;

    *w += 2 * ih->data->border_width;
    *h += 2 * ih->data->border_width;

    *w += ih->data->arrow_size;
  }

  (void)children_expand; /* unset if not a container */
}


/******************************************************************************/


Iclass* iupDropButtonNewClass(void)
{
  Iclass* ic = iupClassNew(iupRegisterFindClass("canvas"));

  ic->name = "dropbutton";
  ic->format = "h";   /* one Ihandle* */
  ic->nativetype = IUP_TYPECANVAS;
  ic->childtype = IUP_CHILDNONE;
  ic->is_interactive = 1;

  /* Class functions */
  ic->New = iupDropButtonNewClass;
  ic->Create = iDropButtonCreateMethod;
  ic->ComputeNaturalSize = iDropButtonComputeNaturalSizeMethod;

  /* Callbacks */
  iupClassRegisterCallback(ic, "FLAT_ACTION", "");
  iupClassRegisterCallback(ic, "FLAT_BUTTON_CB", "iiiis");
  iupClassRegisterCallback(ic, "FLAT_MOTION_CB", "iis");
  iupClassRegisterCallback(ic, "FLAT_FOCUS_CB", "i");
  iupClassRegisterCallback(ic, "FLAT_ENTERWINDOW_CB", "ii");
  iupClassRegisterCallback(ic, "FLAT_LEAVEWINDOW_CB", "");
  iupClassRegisterCallback(ic, "DROPDOWN_CB", "i");
  iupClassRegisterCallback(ic, "DROPSHOW_CB", "i");

  /* Overwrite Visual */
  iupClassRegisterAttribute(ic, "ACTIVE", iupBaseGetActiveAttrib, iDropButtonSetActiveAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_DEFAULT);

  /* Special */
  iupClassRegisterAttribute(ic, "TITLE", NULL, iDropButtonSetAttribPostRedraw, NULL, NULL, IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);

  /* IupDropButton */
  iupClassRegisterAttribute(ic, "ALIGNMENT", iDropButtonGetAlignmentAttrib, iDropButtonSetAlignmentAttrib, "ALEFT:ACENTER", NULL, IUPAF_NOT_MAPPED | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "PADDING", iDropButtonGetPaddingAttrib, iDropButtonSetPaddingAttrib, IUPAF_SAMEASSYSTEM, "3x3", IUPAF_NOT_MAPPED);
  iupClassRegisterAttribute(ic, "SPACING", iDropButtonGetSpacingAttrib, iDropButtonSetSpacingAttrib, IUPAF_SAMEASSYSTEM, "2", IUPAF_NOT_MAPPED | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "HIGHLIGHTED", iDropButtonGetHighlightedAttrib, NULL, NULL, NULL, IUPAF_READONLY | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "PRESSED", iDropButtonGetPressedAttrib, NULL, NULL, NULL, IUPAF_READONLY | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "HASFOCUS", iDropButtonGetHasFocusAttrib, NULL, NULL, NULL, IUPAF_READONLY | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SHOWBORDER", NULL, NULL, NULL, NULL, IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "BORDERCOLOR", NULL, NULL, IUPAF_SAMEASSYSTEM, "50 150 255", IUPAF_DEFAULT);  /* inheritable */
  iupClassRegisterAttribute(ic, "BORDERPSCOLOR", NULL, NULL, NULL, NULL, IUPAF_DEFAULT);  /* inheritable */
  iupClassRegisterAttribute(ic, "BORDERHLCOLOR", NULL, NULL, NULL, NULL, IUPAF_DEFAULT);  /* inheritable */
  iupClassRegisterAttribute(ic, "BORDERWIDTH", iDropButtonGetBorderWidthAttrib, iDropButtonSetBorderWidthAttrib, IUPAF_SAMEASSYSTEM, "1", IUPAF_DEFAULT);  /* inheritable */
  iupClassRegisterAttribute(ic, "FGCOLOR", NULL, NULL, "DLGFGCOLOR", NULL, IUPAF_NOT_MAPPED);  /* force the new default value */
  iupClassRegisterAttribute(ic, "BGCOLOR", iDropButtonGetBgColorAttrib, iDropButtonSetAttribPostRedraw, IUPAF_SAMEASSYSTEM, "DLGBGCOLOR", IUPAF_NO_SAVE | IUPAF_DEFAULT);
  iupClassRegisterAttribute(ic, "HLCOLOR", NULL, NULL, NULL, NULL, IUPAF_DEFAULT);  /* inheritable */
  iupClassRegisterAttribute(ic, "PSCOLOR", NULL, NULL, NULL, NULL, IUPAF_DEFAULT);  /* inheritable */

  iupClassRegisterAttribute(ic, "IMAGE", NULL, NULL, NULL, NULL, IUPAF_IHANDLENAME | IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "IMAGEPRESS", NULL, NULL, NULL, NULL, IUPAF_IHANDLENAME | IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "IMAGEHIGHLIGHT", NULL, NULL, NULL, NULL, IUPAF_IHANDLENAME | IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "IMAGEINACTIVE", NULL, NULL, NULL, NULL, IUPAF_IHANDLENAME | IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);
  
  iupClassRegisterAttribute(ic, "IMAGEPOSITION", iDropButtonGetImagePositionAttrib, iDropButtonSetImagePositionAttrib, IUPAF_SAMEASSYSTEM, "LEFT", IUPAF_NOT_MAPPED | IUPAF_NO_INHERIT);
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

  iupClassRegisterAttribute(ic, "DROPCHILD", iDropButtonGetDropChildAttrib, iDropButtonSetDropChildAttrib, NULL, NULL, IUPAF_IHANDLENAME | IUPAF_NOT_MAPPED | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "DROPCHILD_HANDLE", iDropButtonGetDropChildHandleAttrib, iDropButtonSetDropChildHandleAttrib, NULL, NULL, IUPAF_NOT_MAPPED | IUPAF_NO_INHERIT | IUPAF_IHANDLE | IUPAF_NO_STRING);

  iupClassRegisterAttribute(ic, "ARROWIMAGES", NULL, NULL, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "ARROWSIZE", iDropButtonGetArrowSizeAttrib, iDropButtonSetArrowSizeAttrib, IUPAF_SAMEASSYSTEM, "24", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "ARROWPADDING", iDropButtonGetArrowPaddingAttrib, iDropButtonSetArrowPaddingAttrib, IUPAF_SAMEASSYSTEM, "4", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "ARROWACTIVE", NULL, NULL, IUPAF_SAMEASSYSTEM, "Yes", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "ARROWCOLOR", NULL, NULL, NULL, NULL, IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "ARROWIMAGE", NULL, NULL, NULL, NULL, IUPAF_IHANDLENAME | IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "ARROWIMAGEPRESS", NULL, NULL, NULL, NULL, IUPAF_IHANDLENAME | IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "ARROWIMAGEHIGHLIGHT", NULL, NULL, NULL, NULL, IUPAF_IHANDLENAME | IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "ARROWIMAGEINACTIVE", NULL, NULL, NULL, NULL, IUPAF_IHANDLENAME | IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "DROPONARROW", NULL, NULL, IUPAF_SAMEASSYSTEM, "Yes", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SHOWDROPDOWN", NULL, iDropButtonSetShowDropdownAttrib, NULL, NULL, IUPAF_WRITEONLY | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "DROPPOSITION", NULL, NULL, IUPAF_SAMEASSYSTEM, "BOTTOMLEFT", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "VISIBLECOLUMNS", NULL, NULL, NULL, NULL, IUPAF_NOT_MAPPED | IUPAF_NO_INHERIT);

  return ic;
}

Ihandle* IupDropButton(Ihandle* dropchild)
{
  void *children[2];
  children[0] = (void*)dropchild;
  children[1] = NULL;
  return IupCreatev("dropbutton", children);
}
