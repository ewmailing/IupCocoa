/** \file
 * \brief GLLabel Control.
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "iup.h"
#include "iupcbs.h"
#include "iupglcontrols.h"

#include "iup_object.h"
#include "iup_attrib.h"
#include "iup_str.h"
#include "iup_register.h"

#include "iup_glcontrols.h"
#include "iup_glicon.h"


static int iGLLabelACTION(Ihandle* ih)
{
  char *image = iupAttribGet(ih, "IMAGE");
  char* title = iupAttribGet(ih, "TITLE");
  int active = iupAttribGetInt(ih, "ACTIVE");
  char* fgcolor = iupAttribGetStr(ih, "FGCOLOR");

  iupGLIconDraw(ih, 0, 0, ih->currentwidth, ih->currentheight, "IMAGE", image, title, fgcolor, active);

  return IUP_DEFAULT;
}

static int iGLLabelCreateMethod(Ihandle* ih, void** params)
{
  if (params && params[0])
    iupAttribSetStr(ih, "TITLE", (char*)(params[0]));

  IupSetCallback(ih, "GL_ACTION", iGLLabelACTION);
  
  return IUP_NOERROR;
}

static void iGLLabelComputeNaturalSizeMethod(Ihandle* ih, int *w, int *h, int *children_expand)
{
  int natural_w = 0, 
      natural_h = 0;
  char* image = iupAttribGet(ih, "IMAGE");
  char* title = iupAttribGet(ih, "TITLE");

  iupGLIconGetSize(ih, image, title, &natural_w, &natural_h);

  *w = natural_w;
  *h = natural_h;

  (void)children_expand; /* unset if not a container */
}


/******************************************************************************/


Iclass* iupGLLabelNewClass(void)
{
  Iclass* ic = iupClassNew(iupRegisterFindClass("glsubcanvas"));

  ic->name = "gllabel";
  ic->format = "s"; /* one string */
  ic->nativetype = IUP_TYPEVOID;
  ic->childtype = IUP_CHILDNONE;
  ic->is_interactive = 0;

  /* Class functions */
  ic->New = iupGLLabelNewClass;
  ic->Create = iGLLabelCreateMethod;
  ic->ComputeNaturalSize = iGLLabelComputeNaturalSizeMethod;

  iupGLIconRegisterAttrib(ic);

  iupClassRegisterAttribute(ic, "IMAGE", NULL, NULL, NULL, NULL, IUPAF_IHANDLENAME | IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "IMAGEPRESS", NULL, NULL, NULL, NULL, IUPAF_IHANDLENAME | IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "IMAGEHIGHLIGHT", NULL, NULL, NULL, NULL, IUPAF_IHANDLENAME | IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "IMAGEINACTIVE", NULL, NULL, NULL, NULL, IUPAF_IHANDLENAME | IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "TITLE", NULL, NULL, NULL, NULL, IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "FGCOLOR", NULL, NULL, "0 0 0", NULL, IUPAF_DEFAULT);  /* inheritable */

  return ic;
}

Ihandle* IupGLLabel(const char* title)
{
  void *params[2];
  params[0] = (void*)title;
  params[1] = NULL;
  return IupCreatev("gllabel", params);
}

