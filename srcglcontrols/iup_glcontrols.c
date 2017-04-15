/** \file
 * \brief initializes the GL controls.
 *
 * See Copyright Notice in "iup.h"
 */


#include "iup.h"
#include "iupgl.h"
#include "iupglcontrols.h"

#include "iup_object.h"
#include "iup_childtree.h"
#include "iup_register.h"
#include "iup_attrib.h"
#include "iup_glcontrols.h"


int IupGLControlsOpen(void)
{
  IupGLCanvasOpen();

  if (IupGetGlobal("_IUP_GLCONTROLS_OPEN"))
    return IUP_OPENED;

  iupRegisterClass(iupGLCanvasBoxNewClass());
  iupRegisterClass(iupGLSubCanvasNewClass());
  iupRegisterClass(iupGLLabelNewClass());
  iupRegisterClass(iupGLSeparatorNewClass());
  iupRegisterClass(iupGLButtonNewClass());
  iupRegisterClass(iupGLToggleNewClass());
  iupRegisterClass(iupGLLinkNewClass());
  iupRegisterClass(iupGLProgressBarNewClass());
  iupRegisterClass(iupGLValNewClass());
  iupRegisterClass(iupGLFrameNewClass());
  iupRegisterClass(iupGLExpanderNewClass());
  iupRegisterClass(iupGLScrollBoxNewClass());
  iupRegisterClass(iupGLSizeBoxNewClass());
  iupRegisterClass(iupGLTextNewClass());

  IupSetGlobal("_IUP_GLCONTROLS_OPEN", "1");

  return IUP_NOERROR;
}
