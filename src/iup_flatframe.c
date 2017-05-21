/** \file
 * \brief FlatFrame Control.
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdio.h>
#include <stdlib.h>

#include "iup.h"
#include "iupcbs.h"

#include "iup_object.h"
#include "iup_attrib.h"
#include "iup_str.h"
#include "iup_drv.h"
#include "iup_drvfont.h"
#include "iup_stdcontrols.h"
#include "iup_layout.h"
#include "iup_register.h"
#include "iup_drvdraw.h"
#include "iup_image.h"


static void iFlatFrameGetIconSize(Ihandle* ih, int *w, int *h)
{
  char* image = iupAttribGet(ih, "IMAGE");
  char* title = iupAttribGet(ih, "TITLE");
  int horiz_padding = 0, vert_padding = 0;
  IupGetIntInt(ih, "TITLEPADDING", &horiz_padding, &vert_padding);

  *w = 0;
  *h = 0;

  if (image)
  {
    iupImageGetInfo(image, w, h, NULL);

    if (title)
    {
      int img_position = iupFlatGetImagePosition(iupAttribGetStr(ih, "TITLEIMAGEPOSITION"));
      int spacing = iupAttribGetInt(ih, "TITLEIMAGESPACING");
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

  *w += 2 * horiz_padding;
  *h += 2 * vert_padding;
}

static int iFlatFrameGetTitleHeight(Ihandle* ih)
{
  int width, height;
  iFlatFrameGetIconSize(ih, &width, &height);

  if (height && iupAttribGetBoolean(ih, "TITLELINE"))
    height += iupAttribGetInt(ih, "TITLELINEWIDTH");

  return height;
}

static int iFlatFrameRedraw_CB(Ihandle* ih)
{
  char* backcolor = iupAttribGetStr(ih, "BGCOLOR");
  int frame_width = iupAttribGetInt(ih, "FRAMEWIDTH");
  int frame = iupAttribGetBoolean(ih, "FRAME");
  IdrawCanvas* dc = iupdrvDrawCreateCanvas(ih);
  int title_height = iFlatFrameGetTitleHeight(ih);

  iupdrvDrawParentBackground(dc);

  if (!backcolor)
    backcolor = iupBaseNativeParentGetBgColorAttrib(ih);

  /* draw border - can still be disabled setting frame_width=0 */
  if (frame)
  {
    char* frame_color = iupAttribGetStr(ih, "FRAMECOLOR");

    iupFlatDrawBorder(dc, 0, ih->currentwidth - 1,
                          0, ih->currentheight - 1,
                          frame_width, frame_color, NULL, 1);
  }
  else
    frame_width = 0;

  /* draw child area background */
  iupFlatDrawBox(dc, frame_width, ih->currentwidth - 1 - frame_width,
                     frame_width + title_height, ih->currentheight - 1 - frame_width, backcolor, NULL, 1);

  if (title_height)
  {
    char *titleimage = iupAttribGet(ih, "TITLEIMAGE");
    char* title = iupAttribGet(ih, "TITLE");
    char* titlecolor = iupAttribGetStr(ih, "TITLECOLOR");
    char* titlebgcolor = iupAttribGetStr(ih, "TITLEBGCOLOR");
    int title_alignment = iupFlatGetHorizontalAlignment(iupAttribGetStr(ih, "TITLEALIGNMENT"));
    int img_position = iupFlatGetImagePosition(iupAttribGetStr(ih, "TITLEIMAGEPOSITION"));
    int spacing = iupAttribGetInt(ih, "TITLEIMAGESPACING");
    int horiz_padding, vert_padding;

    int title_line = 0;
    if (iupAttribGetBoolean(ih, "TITLELINE"))
      title_line = iupAttribGetInt(ih, "TITLELINEWIDTH");
    
    IupGetIntInt(ih, "TITLEPADDING", &horiz_padding, &vert_padding);

    /* draw title background */
    iupFlatDrawBox(dc, frame_width, ih->currentwidth - 1 - frame_width,
                       frame_width, frame_width + title_height - 1 - title_line, titlebgcolor, NULL, 1);

    if (iupAttribGetBoolean(ih, "TITLELINE"))
    {
      int i;
      char* title_line_color = iupAttribGetStr(ih, "TITLELINECOLOR");
      unsigned char r = 0, g = 0, b = 0;
      iupStrToRGB(title_line_color, &r, &g, &b);

      for (i = 0; i < title_line; i++)
        iupdrvDrawLine(dc, frame_width, frame_width + title_height - 1 - i,
                           ih->currentwidth - 1 - frame_width, frame_width + title_height - 1 - i,
                           r, g, b, IUP_DRAW_STROKE);
    }

    iupFlatDrawIcon(ih, dc, frame_width, frame_width,
                    ih->currentwidth - 2 * frame_width, title_height - title_line,
                    img_position, spacing, title_alignment, IUP_ALIGN_ATOP, horiz_padding, vert_padding,
                    titleimage, 0, title, titlecolor, NULL, 1);
  }

  iupdrvDrawFlush(dc);

  iupdrvDrawKillCanvas(dc);

  return IUP_DEFAULT;
}


/*****************************************************************************************/


static char* iFlatFrameGetDecorSizeAttrib(Ihandle* ih)
{
  int height = 0;
  int width = 0;

  if (iupAttribGetBoolean(ih, "FRAME"))
  {
    width = 2 * iupAttribGetInt(ih, "FRAMEWIDTH") + 2 * iupAttribGetInt(ih, "FRAMESPACE");
    height = width;
  }

  height += iFlatFrameGetTitleHeight(ih);

  return iupStrReturnIntInt(width, height, 'x');
}

static char* iFlatFrameGetDecorOffsetAttrib(Ihandle* ih)
{
  int dx = 0;
  int dy = 0;

  if (iupAttribGetBoolean(ih, "FRAME"))
  {
    dx = iupAttribGetInt(ih, "FRAMEWIDTH") + iupAttribGetInt(ih, "FRAMESPACE");
    dy = dx;
  }

  dy += iFlatFrameGetTitleHeight(ih);

  return iupStrReturnIntInt(dx, dy, 'x');
}

static int iFlatFrameCreateMethod(Ihandle* ih, void** params)
{
  (void)params;
  IupSetCallback(ih, "ACTION", (Icallback)iFlatFrameRedraw_CB);
  return IUP_NOERROR;
}


/******************************************************************************/


Ihandle* IupFlatFrame(Ihandle* child)
{
  void *children[2];
  children[0] = (void*)child;
  children[1] = NULL;
  return IupCreatev("flatframe", children);
}

Iclass* iupFlatFrameNewClass(void)
{
  Iclass* ic = iupClassNew(iupRegisterFindClass("backgroundbox"));

  ic->name = "flatframe";
  ic->format = "h"; /* one Ihandle* */
  ic->nativetype = IUP_TYPECONTROL;
  ic->childtype = IUP_CHILDMANY+1;   /* one child */
  ic->is_interactive = 0;

  /* Class functions */
  ic->New = iupFlatFrameNewClass;
  ic->Create = iFlatFrameCreateMethod;

  /* replace IupCanvas behavior */
  iupClassRegisterReplaceAttribFlags(ic, "BORDER", IUPAF_READONLY);

  /* replace IupBackgroundBox behavior */
  iupClassRegisterAttribute(ic, "DECORATION", NULL, NULL, IUPAF_SAMEASSYSTEM, "Yes", IUPAF_NOT_MAPPED | IUPAF_READONLY | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "DECORSIZE", iFlatFrameGetDecorSizeAttrib, NULL, NULL, NULL, IUPAF_NOT_MAPPED | IUPAF_READONLY | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "DECOROFFSET", iFlatFrameGetDecorOffsetAttrib, NULL, NULL, NULL, IUPAF_NOT_MAPPED | IUPAF_READONLY | IUPAF_NO_INHERIT);

  /* Special */
  iupClassRegisterAttribute(ic, "BGCOLOR", NULL, NULL, IUPAF_SAMEASSYSTEM, "DLGBGCOLOR", IUPAF_DEFAULT);

  iupClassRegisterAttribute(ic, "TITLE", NULL, NULL, NULL, NULL, IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "TITLECOLOR", NULL, NULL, IUPAF_SAMEASSYSTEM, "DLGFGCOLOR", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "TITLEBGCOLOR", NULL, NULL, IUPAF_SAMEASSYSTEM, "DLGBGCOLOR", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "TITLELINE", NULL, NULL, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "TITLELINECOLOR", NULL, NULL, IUPAF_SAMEASSYSTEM, "DLGFGCOLOR", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "TITLELINEWIDTH", NULL, NULL, IUPAF_SAMEASSYSTEM, "1", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "TITLEIMAGE", NULL, NULL, NULL, NULL, IUPAF_IHANDLENAME | IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "TITLEIMAGEPOSITION", NULL, NULL, IUPAF_SAMEASSYSTEM, "LEFT", IUPAF_NOT_MAPPED | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "TITLEIMAGESPACING", NULL, NULL, IUPAF_SAMEASSYSTEM, "2", IUPAF_NOT_MAPPED | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "TITLEALIGNMENT", NULL, NULL, "ACENTER", NULL, IUPAF_NOT_MAPPED | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "TITLEPADDING", NULL, NULL, IUPAF_SAMEASSYSTEM, "0x0", IUPAF_NOT_MAPPED | IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "FRAME", NULL, NULL, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "FRAMECOLOR", NULL, NULL, IUPAF_SAMEASSYSTEM, "DLGFGCOLOR", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "FRAMEWIDTH", NULL, NULL, IUPAF_SAMEASSYSTEM, "1", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "FRAMESPACE", NULL, NULL, IUPAF_SAMEASSYSTEM, "2", IUPAF_NO_INHERIT);

  return ic;
}
