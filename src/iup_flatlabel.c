/** \file
 * \brief FlatLabel Control
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

struct _IcontrolData
{
  iupCanvas canvas;  /* from IupCanvas (must reserve it) */

  /* attributes */
  int horiz_padding, vert_padding;  /* button margin */
  int spacing, img_position;        /* used when both text and image are displayed */
  int horiz_alignment, vert_alignment;  
};


/****************************************************************/


static int iFlatLabelRedraw_CB(Ihandle* ih)
{
  char *image = iupAttribGet(ih, "IMAGE");
  char* title = iupAttribGet(ih, "TITLE");
  int active = IupGetInt(ih, "ACTIVE");  /* native implementation */
  char* fgcolor = iupAttribGetStr(ih, "FGCOLOR");
  char* bgcolor = NULL;
  char* bgimage = iupAttribGet(ih, "BACKIMAGE");
  char* fgimage = iupAttribGet(ih, "FRONTIMAGE");
  char* text_align = iupAttribGetStr(ih, "TEXTALIGNMENT");
  const char* draw_image;
  IdrawCanvas* dc = iupdrvDrawCreateCanvas(ih);
  int make_inactive = 0;

  iupDrawParentBackground(dc, ih);

  if (!bgcolor)
    bgcolor = iupBaseNativeParentGetBgColorAttrib(ih);

  if (bgimage) /* draw background */
  {
    draw_image = iupFlatGetImageName(ih, "BACKIMAGE", bgimage, 0, 0, active, &make_inactive);
    iupdrvDrawImage(dc, draw_image, make_inactive, 0, 0);
  }
  else
    iupFlatDrawBox(dc, 0, ih->currentwidth - 1,
                           0, ih->currentheight - 1,
                           bgcolor, NULL, 1);  /* background is always active */

  draw_image = iupFlatGetImageName(ih, "IMAGE", image, 0, 0, active, &make_inactive);
  iupFlatDrawIcon(ih, dc, 0, 0,
                  ih->currentwidth, ih->currentheight,
                  ih->data->img_position, ih->data->spacing, ih->data->horiz_alignment, ih->data->vert_alignment, ih->data->horiz_padding, ih->data->vert_padding,
                  draw_image, make_inactive, title, text_align, fgcolor, bgcolor, active);

  if (fgimage)
  {
    draw_image = iupFlatGetImageName(ih, "FRONTIMAGE", fgimage, 0, 0, active, &make_inactive);
    iupdrvDrawImage(dc, draw_image, make_inactive, 0, 0);
  }
  else if (!image && !title)
  {
    int space = 2;
    iupFlatDrawBorder(dc, space, ih->currentwidth - 1 - space,
                              space, ih->currentheight - 1 - space,
                              1, "0 0 0", bgcolor, active);
    space++;
    iupFlatDrawBox(dc, space, ih->currentwidth - 1 - space,
                           space, ih->currentheight - 1 - space,
                           fgcolor, bgcolor, active);
  }

  iupdrvDrawFlush(dc);

  iupdrvDrawKillCanvas(dc);

  return IUP_DEFAULT;
}


/***********************************************************************************************/


static int iFlatLabelSetActiveAttrib(Ihandle* ih, const char* value)
{
  iupBaseSetActiveAttrib(ih, value);
  iupdrvRedrawNow(ih);
  return 0; 
}

static int iFlatLabelSetAlignmentAttrib(Ihandle* ih, const char* value)
{
  char value1[30], value2[30];

  iupStrToStrStr(value, value1, value2, ':');

 ih->data->horiz_alignment = iupFlatGetHorizontalAlignment(value1);
 ih->data->vert_alignment = iupFlatGetVerticalAlignment(value2);

  if (ih->handle)
    iupdrvRedrawNow(ih);

  return 1;
}

static char* iFlatLabelGetAlignmentAttrib(Ihandle *ih)
{
  char* horiz_align2str[3] = {"ALEFT", "ACENTER", "ARIGHT"};
  char* vert_align2str[3] = {"ATOP", "ACENTER", "ABOTTOM"};
  return iupStrReturnStrf("%s:%s", horiz_align2str[ih->data->horiz_alignment], vert_align2str[ih->data->vert_alignment]);
}

static int iFlatLabelSetPaddingAttrib(Ihandle* ih, const char* value)
{
  iupStrToIntInt(value, &ih->data->horiz_padding, &ih->data->vert_padding, 'x');
  if (ih->handle)
    iupdrvRedrawNow(ih);
  return 0;
}

static int iFlatLabelRedrawSetAttrib(Ihandle* ih, const char* value)
{
  (void)value;
  iupdrvPostRedraw(ih);
  return 1;
}

static char* iFlatLabelGetPaddingAttrib(Ihandle* ih)
{
  /* this method can be called before map */
  return iupStrReturnIntInt(ih->data->horiz_padding, ih->data->vert_padding, 'x');
}

static int iFlatLabelSetImagePositionAttrib(Ihandle* ih, const char* value)
{
  ih->data->img_position = iupFlatGetImagePosition(value);

  if (ih->handle)
    iupdrvRedrawNow(ih);

  return 0;
}

static char* iFlatLabelGetImagePositionAttrib(Ihandle *ih)
{
  char* img_pos2str[4] = {"LEFT", "RIGHT", "TOP", "BOTTOM"};
  return iupStrReturnStr(img_pos2str[ih->data->img_position]);
}

static int iFlatLabelSetSpacingAttrib(Ihandle* ih, const char* value)
{
  iupStrToInt(value, &ih->data->spacing);
  if (ih->handle)
    iupdrvRedrawNow(ih);
  return 0;
}

static char* iFlatLabelGetSpacingAttrib(Ihandle *ih)
{
  return iupStrReturnInt(ih->data->spacing);
}


/*****************************************************************************************/


static int iFlatLabelCreateMethod(Ihandle* ih, void** params)
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
  ih->data->horiz_alignment = IUP_ALIGN_ACENTER;
  ih->data->vert_alignment = IUP_ALIGN_ACENTER;

  /* internal callbacks */
  IupSetCallback(ih, "ACTION", (Icallback)iFlatLabelRedraw_CB);

  return IUP_NOERROR;
}

static void iFlatLabelComputeNaturalSizeMethod(Ihandle* ih, int *w, int *h, int *children_expand)
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
  }

  (void)children_expand; /* unset if not a container */
}


/******************************************************************************/


Iclass* iupFlatLabelNewClass(void)
{
  Iclass* ic = iupClassNew(iupRegisterFindClass("canvas"));

  ic->name = "flatlabel";
  ic->format = "s"; /* one string */
  ic->nativetype = IUP_TYPECANVAS;
  ic->childtype = IUP_CHILDNONE;
  ic->is_interactive = 1;

  /* Class functions */
  ic->New = iupFlatLabelNewClass;
  ic->Create = iFlatLabelCreateMethod;
  ic->ComputeNaturalSize = iFlatLabelComputeNaturalSizeMethod;

  /* Overwrite Visual */
  iupClassRegisterAttribute(ic, "ACTIVE", iupBaseGetActiveAttrib, iFlatLabelSetActiveAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_DEFAULT);
  iupClassRegisterAttribute(ic, "BGCOLOR", iupBaseNativeParentGetBgColorAttrib, NULL, IUPAF_SAMEASSYSTEM, "DLGBGCOLOR", IUPAF_NO_SAVE | IUPAF_DEFAULT);

  /* Special */
  iupClassRegisterAttribute(ic, "TITLE", NULL, iFlatLabelRedrawSetAttrib, NULL, NULL, IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);

  /* IupFlatLabel */
  iupClassRegisterAttribute(ic, "ALIGNMENT", iFlatLabelGetAlignmentAttrib, iFlatLabelSetAlignmentAttrib, "ALEFT:ACENTER", NULL, IUPAF_NOT_MAPPED | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "PADDING", iFlatLabelGetPaddingAttrib, iFlatLabelSetPaddingAttrib, IUPAF_SAMEASSYSTEM, "0x0", IUPAF_NOT_MAPPED);
  iupClassRegisterAttribute(ic, "SPACING", iFlatLabelGetSpacingAttrib, iFlatLabelSetSpacingAttrib, IUPAF_SAMEASSYSTEM, "2", IUPAF_NOT_MAPPED | IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "FGCOLOR", NULL, iFlatLabelRedrawSetAttrib, "DLGFGCOLOR", NULL, IUPAF_NOT_MAPPED);  /* force the new default value */

  iupClassRegisterAttribute(ic, "IMAGE", NULL, iFlatLabelRedrawSetAttrib, NULL, NULL, IUPAF_IHANDLENAME | IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "IMAGEINACTIVE", NULL, NULL, NULL, NULL, IUPAF_IHANDLENAME | IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);
  
  iupClassRegisterAttribute(ic, "IMAGEPOSITION", iFlatLabelGetImagePositionAttrib, iFlatLabelSetImagePositionAttrib, IUPAF_SAMEASSYSTEM, "LEFT", IUPAF_NOT_MAPPED | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "TEXTALIGNMENT", NULL, NULL, IUPAF_SAMEASSYSTEM, "ALEFT", IUPAF_NOT_MAPPED | IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "BACKIMAGE", NULL, iFlatLabelRedrawSetAttrib, NULL, NULL, IUPAF_IHANDLENAME | IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "BACKIMAGEINACTIVE", NULL, NULL, NULL, NULL, IUPAF_IHANDLENAME | IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);
  
  iupClassRegisterAttribute(ic, "FITTOBACKIMAGE", NULL, NULL, NULL, NULL, IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "FRONTIMAGE", NULL, iFlatLabelRedrawSetAttrib, NULL, NULL, IUPAF_IHANDLENAME | IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "FRONTIMAGEINACTIVE", NULL, NULL, NULL, NULL, IUPAF_IHANDLENAME | IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);

  return ic;
}

Ihandle* IupFlatLabel(const char* title)
{
  void *params[2];
  params[0] = (void*)title;
  params[1] = NULL;
  return IupCreatev("flatlabel", params);
}
