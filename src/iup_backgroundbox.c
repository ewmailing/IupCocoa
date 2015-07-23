/** \file
 * \brief IupBackgroundBox control
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "iup.h"
#include "iupcbs.h"
#include "iupkey.h"

#include "iup_object.h"
#include "iup_register.h"
#include "iup_attrib.h"
#include "iup_str.h"
#include "iup_stdcontrols.h"
#include "iup_layout.h"
#include "iup_drv.h"


/*****************************************************************************\
|* Methods                                                                   *|
\*****************************************************************************/

static char* iBackgroundBoxGetBgColorAttrib(Ihandle* ih)
{
  if (iupAttribGet(ih, "BGCOLOR"))
    return NULL;  /* get from the hash table */
  else
    return iupBaseNativeParentGetBgColorAttrib(ih);
}

static int iBackgroundBoxSetBgColorAttrib(Ihandle* ih, const char* value)
{
  (void)value;
  IupUpdate(ih); /* post a redraw */
  return 1;  /* save on the hash table */
}

static int iBackgroundBoxGetBorder(Ihandle* ih)
{
  if (iupAttribGetBoolean(ih, "BORDER"))
    return 1;
  else
    return 0;
}

static char* iBackgroundBoxGetClientOffsetAttrib(Ihandle* ih)
{
  int dx = 0, dy = 0;
  if (iupAttribGetBoolean(ih, "BORDER"))
  {
    dx = 1;
    dy = 1;
  }
  return iupStrReturnIntInt(dx, dy, 'x');
}

static void iBackgroundBoxComputeNaturalSizeMethod(Ihandle* ih, int *w, int *h, int *children_expand)
{
  if (ih->firstchild)
  {
    int border = iBackgroundBoxGetBorder(ih);

    iupBaseComputeNaturalSize(ih->firstchild);

    *children_expand = ih->firstchild->expand;
    *w = ih->firstchild->naturalwidth + 2*border;
    *h = ih->firstchild->naturalheight + 2*border;
  }
}

static void iBackgroundBoxSetChildrenCurrentSizeMethod(Ihandle* ih, int shrink)
{
  if (ih->firstchild)
  {
    int border = iBackgroundBoxGetBorder(ih);
    int width  = (ih->currentwidth  > border ? ih->currentwidth  - border : 0);
    int height = (ih->currentheight > border ? ih->currentheight - border : 0);
    iupBaseSetCurrentSize(ih->firstchild, width, height, shrink);
  }
}

static void iBackgroundBoxSetChildrenPositionMethod(Ihandle* ih, int x, int y)
{
  if (ih->firstchild)
  {
    char* offset = iupAttribGet(ih, "CHILDOFFSET");

    /* Native container, position is reset */
    x = 0;
    y = 0;

    if (offset) iupStrToIntInt(offset, &x, &y, 'x');

    iupBaseSetPosition(ih->firstchild, x, y);
  }
}

static int iBackgroundBoxCreateMethod(Ihandle* ih, void** params)
{
  IupSetAttribute(ih, "CANFOCUS", "NO");

  if (params)
  {
    Ihandle** iparams = (Ihandle**)params;
    if (iparams[0]) IupAppend(ih, iparams[0]);
  }

  return IUP_NOERROR;
}

Iclass* iupBackgroundBoxNewClass(void)
{
  Iclass* ic = iupClassNew(iupRegisterFindClass("canvas"));

  ic->name   = "backgroundbox";
  ic->format = "h";   /* one Ihandle* */
  ic->nativetype = IUP_TYPECANVAS;
  ic->childtype  = IUP_CHILDMANY+1;  /* 1 child */
  ic->is_interactive = 0;

  /* Class functions */
  ic->New = iupBackgroundBoxNewClass;
  ic->Create  = iBackgroundBoxCreateMethod;

  ic->ComputeNaturalSize = iBackgroundBoxComputeNaturalSizeMethod;
  ic->SetChildrenCurrentSize = iBackgroundBoxSetChildrenCurrentSizeMethod;
  ic->SetChildrenPosition = iBackgroundBoxSetChildrenPositionMethod;

  /* Base Container */
  iupClassRegisterAttribute(ic, "EXPAND", iupBaseContainerGetExpandAttrib, NULL, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "CLIENTOFFSET", iBackgroundBoxGetClientOffsetAttrib, NULL, NULL, NULL, IUPAF_NOT_MAPPED | IUPAF_READONLY | IUPAF_NO_INHERIT);
  {
    IattribGetFunc drawsize_get = NULL;
    iupClassRegisterGetAttribute(ic, "DRAWSIZE", &drawsize_get, NULL, NULL, NULL, NULL);
    iupClassRegisterAttribute(ic, "CLIENTSIZE", drawsize_get, NULL, NULL, NULL, IUPAF_READONLY|IUPAF_NO_INHERIT);
  }

  /* Native Container */
  iupClassRegisterAttribute(ic, "CHILDOFFSET", NULL, NULL, NULL, NULL, IUPAF_NOT_MAPPED | IUPAF_NO_INHERIT);

  /* replace IupCanvas behavior */
  iupClassRegisterAttribute(ic, "BGCOLOR", iBackgroundBoxGetBgColorAttrib, iBackgroundBoxSetBgColorAttrib, IUPAF_SAMEASSYSTEM, "DLGBGCOLOR", IUPAF_NO_SAVE | IUPAF_DEFAULT);
  iupClassRegisterReplaceAttribDef  (ic, "BORDER", "NO", NULL);
  iupClassRegisterReplaceAttribFlags(ic, "BORDER", IUPAF_NO_INHERIT);
  iupClassRegisterReplaceAttribDef  (ic, "SCROLLBAR", "NO", NULL);

  return ic;
}

Ihandle* IupBackgroundBox(Ihandle* child)
{
  void *children[2];
  children[0] = (void*)child;
  children[1] = NULL;
  return IupCreatev("backgroundbox", children);
}
