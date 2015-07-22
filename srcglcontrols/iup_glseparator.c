/** \file
 * \brief GLSeparator Control.
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


static int iGLSeparatorIsVertical(Ihandle* ih)
{
  char* value = iupAttribGetStr(ih, "ORIENTATION");
  if (iupStrEqualNoCase(value, "HORIZONTAL"))
    return 0;
  return 1;
}

static int iGLSeparatorACTION(Ihandle* ih)
{
  int x1, y1, x2, y2;
  int active = iupAttribGetInt(ih, "ACTIVE");
  char* color = iupAttribGetStr(ih, "BORDERCOLOR");
  float bwidth = iupAttribGetFloat(ih, "BORDERWIDTH");

  if (iGLSeparatorIsVertical(ih))
  {
    y1 = 0;
    y2 = ih->currentheight-1;
    x1 = x2 = ih->currentwidth / 2;
  }
  else
  {
    x1 = 0;
    x2 = ih->currentwidth-1;
    y1 = y2 = ih->currentheight / 2;
  }

  iupGLDrawLine(ih, x1, y1, x2, y2, bwidth, color, active);

  return IUP_DEFAULT;
}

static int iGLSeparatorCreateMethod(Ihandle* ih, void** params)
{
  IupSetCallback(ih, "GL_ACTION", iGLSeparatorACTION);
  
  (void)params;
  return IUP_NOERROR;
}

static void iGLSeparatorComputeNaturalSizeMethod(Ihandle* ih, int *w, int *h, int *children_expand)
{
  int natural_w = 0,
      natural_h = 0;
  float bwidth = iupAttribGetFloat(ih, "BORDERWIDTH");
  int border_width = (int)ceil(bwidth);

  if (iGLSeparatorIsVertical(ih))
  {
    natural_w = border_width;
    if (ih->userheight <= 0 && !(ih->expand & IUP_EXPAND_HEIGHT))
      ih->expand = IUP_EXPAND_HFREE;
  }
  else
  {
    natural_h = border_width;
    if (ih->userwidth <= 0 && !(ih->expand & IUP_EXPAND_WIDTH))
      ih->expand = IUP_EXPAND_WFREE;
  }

  *w = natural_w;
  *h = natural_h;

  (void)children_expand; /* unset if not a container */
}


/******************************************************************************/


Iclass* iupGLSeparatorNewClass(void)
{
  Iclass* ic = iupClassNew(iupRegisterFindClass("glsubcanvas"));

  ic->name = "glseparator";
  ic->format = ""; /* none */
  ic->nativetype = IUP_TYPEVOID;
  ic->childtype = IUP_CHILDNONE;
  ic->is_interactive = 0;

  /* Class functions */
  ic->New = iupGLSeparatorNewClass;
  ic->Create = iGLSeparatorCreateMethod;
  ic->ComputeNaturalSize = iGLSeparatorComputeNaturalSizeMethod;

  /* IupSeparator only */
  iupClassRegisterAttribute(ic, "ORIENTATION", NULL, NULL, IUPAF_SAMEASSYSTEM, "VERTICAL", IUPAF_NO_INHERIT);

  return ic;
}

Ihandle* IupGLSeparator(void)
{
  return IupCreate("glseparator");
}

