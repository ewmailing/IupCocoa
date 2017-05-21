/** \file
 * \brief GL Icon (Image+Text) base functions.
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "iup.h"
#include "iupcbs.h"

#include "iup_object.h"
#include "iup_attrib.h"
#include "iup_str.h"
#include "iup_register.h"
#include "iup_image.h"

#include "iup_glcontrols.h"
#include "iup_glfont.h"
#include "iup_gldraw.h"
#include "iup_glicon.h"
#include "iup_glimage.h"


enum{ IUP_GLPOS_LEFT, IUP_GLPOS_RIGHT, IUP_GLPOS_TOP, IUP_GLPOS_BOTTOM };


static char* iGLIconGetImageName(Ihandle* ih, const char* baseattrib, const char* state)
{
  char attrib[1024];
  strcpy(attrib, baseattrib);
  strcat(attrib, state);
  return iupAttribGet(ih, attrib);
}

Ihandle* iupGLIconGetImageHandle(Ihandle* ih, const char* baseattrib, const char* imagename, int active)
{
  Ihandle* image = NULL;
  int make_inactive = 0;

  iupAttribSet(ih, "MAKEINACTIVE", NULL);

  if (baseattrib)
  {
    if (active)
    {
      int pressed = iupAttribGetInt(ih, "PRESSED");
      if (pressed)
        image = iupImageGetHandle(iGLIconGetImageName(ih, baseattrib, "PRESS"));
      else
      {
        int highlight = iupAttribGetInt(ih, "HIGHLIGHT");
        if (highlight)
          image = iupImageGetHandle(iGLIconGetImageName(ih, baseattrib, "HIGHLIGHT"));
      }
    }
    else
    {
      image = iupImageGetHandle(iGLIconGetImageName(ih, baseattrib, "INACTIVE"));
      if (!image)
        make_inactive = 1;
    }
  }

  if (!image)
    image = iupImageGetHandle(imagename);

  if (image && make_inactive)
    iupAttribSet(image, "MAKEINACTIVE", "1");

  return image;
}

static int iGLIconGetImagePosition(const char* value)
{
  if (!value)
    return IUP_GLPOS_LEFT;
  if (iupStrEqualNoCase(value, "RIGHT"))
    return IUP_GLPOS_RIGHT;
  else if (iupStrEqualNoCase(value, "BOTTOM"))
    return IUP_GLPOS_BOTTOM;
  else if (iupStrEqualNoCase(value, "TOP"))
    return IUP_GLPOS_TOP;
  else /* "LEFT" */
    return IUP_GLPOS_LEFT;
}

static void iGLIconGetAlignment(const char* value, int *horiz_alignment, int *vert_alignment)
{
  char value1[30], value2[30];

  if (!value)
  {
    *horiz_alignment = IUP_ALIGN_ALEFT;
    *vert_alignment = IUP_ALIGN_ATOP;
    return;
  }

  iupStrToStrStr(value, value1, value2, ':');

  if (iupStrEqualNoCase(value1, "ARIGHT"))
    *horiz_alignment = IUP_ALIGN_ARIGHT;
  else if (iupStrEqualNoCase(value1, "ACENTER"))
    *horiz_alignment = IUP_ALIGN_ACENTER;
  else /* "ALEFT" */
    *horiz_alignment = IUP_ALIGN_ALEFT;

  if (iupStrEqualNoCase(value2, "ABOTTOM"))
    *vert_alignment = IUP_ALIGN_ABOTTOM;
  else if (iupStrEqualNoCase(value2, "ACENTER"))
    *vert_alignment = IUP_ALIGN_ACENTER;
  else /* "ATOP" */
    *vert_alignment = IUP_ALIGN_ATOP;
}

static void iGLIconGetPosition(Ihandle* ih, int icon_width, int icon_height, int *x, int *y, int width, int height)
{
  int horiz_padding = 0, vert_padding = 0;
  int horiz_alignment, vert_alignment;

  IupGetIntInt(ih, "PADDING", &horiz_padding, &vert_padding);
  iGLIconGetAlignment(iupAttribGetStr(ih, "ALIGNMENT"), &horiz_alignment, &vert_alignment);

  if (horiz_alignment == IUP_ALIGN_ARIGHT)
    *x = icon_width - (width + 2 * horiz_padding);
  else if (horiz_alignment == IUP_ALIGN_ACENTER)
    *x = (icon_width - (width + 2 * horiz_padding)) / 2;
  else  /* ALEFT */
    *x = 0;

  if (vert_alignment == IUP_ALIGN_ABOTTOM)
    *y = icon_height - (height + 2 * vert_padding);
  else if (vert_alignment == IUP_ALIGN_ACENTER)
    *y = (icon_height - (height + 2 * vert_padding)) / 2;
  else  /* ATOP */
    *y = 0;

  *x += horiz_padding;
  *y += vert_padding;
}

static void iGLIconGetImageTextPosition(int x, int y, int img_position, int spacing, 
                                        int img_width, int img_height, int txt_width, int txt_height,
                                        int *img_x, int *img_y, int *txt_x, int *txt_y)
{
  switch (img_position)
  {
  case IUP_GLPOS_TOP:
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
  case IUP_GLPOS_BOTTOM:
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
  case IUP_GLPOS_RIGHT:
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
  default: /* IUP_GLPOS_LEFT (image at left of text) */
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

void iupGLIconDraw(Ihandle* ih, int icon_x, int icon_y, int icon_width, int icon_height,
                   const char *baseattrib, const char* imagename, const char* title, const char* fgcolor, int active)
{
  int x, y, width, height;

  if (imagename)
  {
    if (title)
    {
      int spacing = iupAttribGetInt(ih, "SPACING");
      int img_position = iGLIconGetImagePosition(iupAttribGetStr(ih, "IMAGEPOSITION"));
      int img_x, img_y, txt_x, txt_y;
      int txt_width, txt_height;
      int img_width, img_height;

      iupGLFontGetMultiLineStringSize(ih, title, &txt_width, &txt_height);
      iupGLImageGetInfo(imagename, &img_width, &img_height, NULL);

      if (img_position == IUP_GLPOS_RIGHT ||
          img_position == IUP_GLPOS_LEFT)
      {
        width = img_width + txt_width + spacing;
        height = iupMAX(img_height, txt_height);
      }
      else
      {
        width = iupMAX(img_width, txt_width);
        height = img_height + txt_height + spacing;
      }

      iGLIconGetPosition(ih, icon_width, icon_height, &x, &y, width, height);

      iGLIconGetImageTextPosition(x, y, img_position, spacing,
                                  img_width, img_height, txt_width, txt_height,
                                  &img_x, &img_y, &txt_x, &txt_y);

      iupGLDrawIconImage(ih, img_x + icon_x, img_y + icon_y, baseattrib, imagename, active);
      iupGLDrawMultilineText(ih, txt_x + icon_x, txt_y + icon_y, title, fgcolor, active);
    }
    else
    {
      iupGLImageGetInfo(imagename, &width, &height, NULL);

      iGLIconGetPosition(ih, icon_width, icon_height, &x, &y, width, height);

      iupGLDrawIconImage(ih, x + icon_x, y + icon_y, baseattrib, imagename, active);
    }
  }
  else if (title)
  {
    iupGLFontGetMultiLineStringSize(ih, title, &width, &height);

    iGLIconGetPosition(ih, icon_width, icon_height, &x, &y, width, height);

    iupGLDrawMultilineText(ih, x + icon_x, y + icon_y, title, fgcolor, active);
  }
}

void iupGLIconGetSize(Ihandle* ih, const char* imagename, const char* title, int *w, int *h)
{
  *w = 0, 
  *h = 0;

  if (imagename)
  {
    iupGLImageGetInfo(imagename, w, h, NULL);

    if (title)
    {
      int spacing = iupAttribGetInt(ih, "SPACING");
      int img_position = iGLIconGetImagePosition(iupAttribGetStr(ih, "IMAGEPOSITION"));
      int text_w, text_h;
      iupGLFontGetMultiLineStringSize(ih, title, &text_w, &text_h);

      if (img_position == IUP_GLPOS_RIGHT ||
          img_position == IUP_GLPOS_LEFT)
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
    iupGLFontGetMultiLineStringSize(ih, title, w, h);

  {
    int horiz_padding = 0, vert_padding = 0;
    IupGetIntInt(ih, "PADDING", &horiz_padding, &vert_padding);
    *w += 2 * horiz_padding;
    *h += 2 * vert_padding;
  }
}

void iupGLIconRegisterAttrib(Iclass* ic)
{
  iupClassRegisterAttribute(ic, "SPACING", NULL, NULL, IUPAF_SAMEASSYSTEM, "2", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "IMAGEPOSITION", NULL, NULL, IUPAF_SAMEASSYSTEM, "LEFT", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "ALIGNMENT", NULL, NULL, IUPAF_SAMEASSYSTEM, "ALEFT:ACENTER", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "PADDING", NULL, NULL, IUPAF_SAMEASSYSTEM, "0x0", IUPAF_NO_INHERIT);
}
