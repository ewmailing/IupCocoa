
/** \file
* \brief IupImgLib
*
* See Copyright Notice in iup.h
*/

#include <stdlib.h>
#include <stdio.h>

#include "iup.h"

#include "iup_str.h"
#include "iup_image.h"

#include "iup_imglib.h"

/* source code, included only here */
#include "iup_imglib_circleprogress.h"

static void create_CircleProgress_animation(void)
{
  Ihandle* animation = IupUser();
  int i;
  char name[30];

  for (i = 0; i < 12; i++)
  {
    sprintf(name, "IUP_CircleProgress%d", i);
    IupAppend(animation, IupGetHandle(name));
  }

  IupSetHandle("IUP_CircleProgressAnimation", animation);
  IupSetAttribute(animation, "FRAMETIME", "83");  /* near 1s for a full turn */
}

void iupImglibCircleProgress(void)
{
  IupSetHandle("IUP_CircleProgress0", load_image_CircleProgress0());
  IupSetHandle("IUP_CircleProgress1", load_image_CircleProgress1());
  IupSetHandle("IUP_CircleProgress2", load_image_CircleProgress2());
  IupSetHandle("IUP_CircleProgress3", load_image_CircleProgress3());
  IupSetHandle("IUP_CircleProgress4", load_image_CircleProgress4());
  IupSetHandle("IUP_CircleProgress5", load_image_CircleProgress5());
  IupSetHandle("IUP_CircleProgress6", load_image_CircleProgress6());
  IupSetHandle("IUP_CircleProgress7", load_image_CircleProgress7());
  IupSetHandle("IUP_CircleProgress8", load_image_CircleProgress8());
  IupSetHandle("IUP_CircleProgress9", load_image_CircleProgress9());
  IupSetHandle("IUP_CircleProgress10", load_image_CircleProgress10());
  IupSetHandle("IUP_CircleProgress11", load_image_CircleProgress11());

  create_CircleProgress_animation();
}
