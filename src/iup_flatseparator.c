/** \file
 * \brief Separator Control
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


enum { ISEPARATOR_VERT, ISEPARATOR_HORIZ };
enum { ISEPARATOR_FILL, ISEPARATOR_LINE, ISEPARATOR_SUNKENLINE, ISEPARATOR_DUALLINES, ISEPARATOR_GRIP };

struct _IcontrolData
{
  iupCanvas canvas;  /* from IupCanvas (must reserve it) */

  int orientation, 
      barsize, 
      style;
};


/****************************************************************/


static int iFlatSeparatorRedraw_CB(Ihandle* ih)
{
  IdrawCanvas* dc;

  dc = iupdrvDrawCreateCanvas(ih);

  iupDrawParentBackground(dc, ih);

  if (ih->data->style != ISEPARATOR_FILL)
  {
    int w, h, x, y;
    long color = iupDrawStrToColor(IupGetAttribute(ih, "COLOR"), iupDrawColor(160, 160, 160, 255));
    iupdrvDrawGetSize(dc, &w, &h);

    if (ih->data->style == ISEPARATOR_GRIP)
    {
      int i, count;
      long sunken_color;
      if (iupDrawRed(color) + iupDrawGreen(color) + iupDrawBlue(color) > 3 * 190)
        sunken_color = iupDrawColor(100, 100, 100, 255);
      else
        sunken_color = iupDrawColor(255, 255, 255, 255);

      if (ih->data->orientation == ISEPARATOR_VERT)
      {
        x = ih->data->barsize / 2 - 1;
        y = 2;
        count = (h - 2) / 5;
      }
      else
      {
        x = 2;
        y = ih->data->barsize / 2 - 1;
        count = (w - 2) / 5;
      }

      for (i = 0; i < count; i++)
      {
        iupdrvDrawRectangle(dc, x + 1, y + 1, x + 2, y + 2, sunken_color, IUP_DRAW_FILL, 1);
        iupdrvDrawRectangle(dc, x, y, x + 1, y + 1, color, IUP_DRAW_FILL, 1);

        if (ih->data->orientation == ISEPARATOR_VERT)
          y += 5;
        else
          x += 5;
      }
    }
    else if (ih->data->style == ISEPARATOR_DUALLINES)
    {
      if (ih->data->orientation == ISEPARATOR_VERT)
      {
        x = ih->data->barsize / 2;

        iupdrvDrawLine(dc, x - 1, 0, x - 1, h - 1, color, IUP_DRAW_STROKE, 1);
        iupdrvDrawLine(dc, x + 1, 0, x + 1, h - 1, color, IUP_DRAW_STROKE, 1);
      }
      else
      {
        y = ih->data->barsize / 2;

        iupdrvDrawLine(dc, 0, y - 1, w - 1, y - 1, color, IUP_DRAW_STROKE, 1);
        iupdrvDrawLine(dc, 0, y + 1, w - 1, y + 1, color, IUP_DRAW_STROKE, 1);
      }
    }
    else if (ih->data->style == ISEPARATOR_SUNKENLINE)
    {
      long sunken_color;
      if (iupDrawRed(color) + iupDrawGreen(color) + iupDrawBlue(color) > 3 * 190)
        sunken_color = iupDrawColor(100, 100, 100, 255);
      else
        sunken_color = iupDrawColor(255, 255, 255, 255);

      if (ih->data->orientation == ISEPARATOR_VERT)
      {
        x = ih->data->barsize / 2;

        iupdrvDrawLine(dc, x, 0, x, h - 1, color, IUP_DRAW_STROKE, 1);
        iupdrvDrawLine(dc, x + 1, 0, x + 1, h - 1, sunken_color, IUP_DRAW_STROKE, 1);
      }
      else
      {
        y = ih->data->barsize / 2;

        iupdrvDrawLine(dc, 0, y, w - 1, y, color, IUP_DRAW_STROKE, 1);
        iupdrvDrawLine(dc, 0, y + 1, w - 1, y + 1, sunken_color, IUP_DRAW_STROKE, 1);
      }
    }
    else /* ISEPARATOR_LINE */
    {
      if (ih->data->orientation == ISEPARATOR_VERT)
      {
        x = ih->data->barsize / 2;

        iupdrvDrawLine(dc, x, 0, x, h - 1, color, IUP_DRAW_STROKE, 1);
      }
      else
      {
        y = ih->data->barsize / 2;

        iupdrvDrawLine(dc, 0, y, w - 1, y, color, IUP_DRAW_STROKE, 1);
      }
    }
  }
  else /* ISEPARATOR_FILL */
  {
    char* color = iupAttribGet(ih, "COLOR");  /* ignore the default value */
    if (color)
    {
      long color = iupDrawStrToColor(IupGetAttribute(ih, "COLOR"), iupDrawColor(160, 160, 160, 255));
      int w, h;
      iupdrvDrawGetSize(dc, &w, &h);

      iupdrvDrawRectangle(dc, 0, 0, w - 1, h - 1, color, IUP_DRAW_FILL, 1);
    }
  }

  iupdrvDrawFlush(dc);

  iupdrvDrawKillCanvas(dc);

  return IUP_DEFAULT;
}


/***********************************************************************************************/


static int iFlatSeparatorSetOrientationAttrib(Ihandle* ih, const char* value)
{
  if (iupStrEqualNoCase(value, "HORIZONTAL"))
  {
    ih->data->orientation = ISEPARATOR_HORIZ;
    ih->expand = IUP_EXPAND_WFREE;
  }
  else  /* Default = VERTICAL */
  {
    ih->data->orientation = ISEPARATOR_VERT;
    ih->expand = IUP_EXPAND_HFREE;
  }

  return 0;  /* do not store value in hash table */
}

static char* iFlatSeparatorGetOrientationAttrib(Ihandle* ih)
{
  const char* orientation_str[] = { "VERTICAL", "HORIZONTAL" };
  return (char*)orientation_str[ih->data->orientation];
}

static char* iFlatSeparatorGetStyleAttrib(Ihandle* ih)
{
  const char* style_str[] = { "FILL", "LINE", "SUNKENLINE", "DUALLINES", "GRIP" };
  return (char*)style_str[ih->data->style];
}

static int iFlatSeparatorSetStyleAttrib(Ihandle* ih, const char* value)
{
  if (iupStrEqualNoCase(value, "FILL"))
    ih->data->style = ISEPARATOR_FILL;
  else if (iupStrEqualNoCase(value, "LINE"))
    ih->data->style = ISEPARATOR_LINE;
  else if (iupStrEqualNoCase(value, "DUALLINES"))
    ih->data->style = ISEPARATOR_DUALLINES;
  else if (iupStrEqualNoCase(value, "GRIP"))
    ih->data->style = ISEPARATOR_GRIP;
  else 
    ih->data->style = ISEPARATOR_SUNKENLINE;
  IupUpdate(ih);
  return 0; /* do not store value in hash table */
}

static int iFlatSeparatorSetBarSizeAttrib(Ihandle* ih, const char* value)
{
  iupStrToInt(value, &ih->data->barsize);
  IupUpdate(ih);
  return 0; /* do not store value in hash table */
}

static char* iFlatSeparatorGetBarSizeAttrib(Ihandle* ih)
{
  return iupStrReturnInt(ih->data->barsize);
}


/*****************************************************************************************/


static int iFlatSeparatorCreateMethod(Ihandle* ih, void** params)
{
  (void)params;

  /* free the data allocated by IupCanvas */
  free(ih->data);
  ih->data = iupALLOCCTRLDATA();

  ih->data->barsize = 5;
  ih->data->style = ISEPARATOR_SUNKENLINE;
  ih->data->orientation = ISEPARATOR_VERT;
  ih->expand = IUP_EXPAND_HFREE;

  /* change the IupCanvas default values */
  iupAttribSet(ih, "BORDER", "NO");
  iupAttribSet(ih, "CANFOCUS", "NO");

  /* internal callbacks */
  IupSetCallback(ih, "ACTION", (Icallback)iFlatSeparatorRedraw_CB);

  return IUP_NOERROR;
}

static void iFlatSeparatorComputeNaturalSizeMethod(Ihandle* ih, int *w, int *h, int *children_expand)
{
  int natural_w = 0,
      natural_h = 0;

  if (ih->data->orientation == ISEPARATOR_HORIZ)
    natural_h = ih->data->barsize;
  else 
    natural_w = ih->data->barsize;

  *w = natural_w;
  *h = natural_h;

  (void)children_expand; /* unset if not a container */
}


/******************************************************************************/


Iclass* iupFlatSeparatorNewClass(void)
{
  Iclass* ic = iupClassNew(iupRegisterFindClass("canvas"));

  ic->name = "flatseparator";
  ic->format = NULL;  /* no parameters */
  ic->nativetype = IUP_TYPECANVAS;
  ic->childtype = IUP_CHILDNONE;
  ic->is_interactive = 0;

  /* Class functions */
  ic->New = iupFlatSeparatorNewClass;
  ic->Create = iFlatSeparatorCreateMethod;
  ic->ComputeNaturalSize = iFlatSeparatorComputeNaturalSizeMethod;

  iupClassRegisterAttribute(ic, "ORIENTATION", iFlatSeparatorGetOrientationAttrib, iFlatSeparatorSetOrientationAttrib, IUPAF_SAMEASSYSTEM, "VERTICAL", IUPAF_NOT_MAPPED | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "STYLE", iFlatSeparatorGetStyleAttrib, iFlatSeparatorSetStyleAttrib, IUPAF_SAMEASSYSTEM, "SUNKENLINE", IUPAF_NOT_MAPPED | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "COLOR", NULL, NULL, IUPAF_SAMEASSYSTEM, "160, 160, 160", IUPAF_NOT_MAPPED | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "BARSIZE", iFlatSeparatorGetBarSizeAttrib, iFlatSeparatorSetBarSizeAttrib, IUPAF_SAMEASSYSTEM, "5", IUPAF_NOT_MAPPED | IUPAF_NO_INHERIT);

  return ic;
}

Ihandle* IupFlatSeparator(void)
{
  return IupCreate("flatseparator");
}
