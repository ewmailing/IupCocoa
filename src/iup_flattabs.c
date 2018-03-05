/** \file
* \brief iupflattabs control
*
* See Copyright Notice in "iup.h"
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>

#include "iup.h"
#include "iupcbs.h"

#include "iup_object.h"
#include "iup_attrib.h"
#include "iup_str.h"
#include "iup_drv.h"
#include "iup_drvfont.h"
#include "iup_stdcontrols.h"
#include "iup_layout.h"
#include "iup_image.h"
#include "iup_register.h"
#include "iup_drvdraw.h"
#include "iup_varg.h"


#define ITABS_CLOSE_SIZE 13
#define ITABS_CLOSE_SPACING 12
#define ITABS_CLOSE_BORDER 8
#define ITABS_NONE -1
#define ITABS_SB_LEFT -2
#define ITABS_SB_RIGHT -3
#define ITABS_EXTRABUTTON1 -4

#define ITABS_TABID2EXTRABUT(_id) (ITABS_EXTRABUTTON1 - _id + 1)
#define ITABS_EXTRABUT2TABID(_id) (ITABS_EXTRABUTTON1 - _id + 1) /* equal to the above, the conversion is symmetric */

static Ihandle* load_image_expand_down(void)
{
  unsigned char imgdata[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0,
    0, 0, 0, 8, 0, 0, 0, 48, 0, 0, 0, 21, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 21, 0, 0, 0, 48, 0, 0, 0, 8,
    0, 0, 0, 45, 0, 0, 0, 109, 0, 0, 0, 93, 0, 0, 0, 24, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 24, 0, 0, 0, 93, 0, 0, 0, 109, 0, 0, 0, 45,
    0, 0, 0, 17, 0, 0, 0, 94, 0, 0, 0, 119, 0, 0, 0, 93, 0, 0, 0, 24, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 24, 0, 0, 0, 93, 0, 0, 0, 119, 0, 0, 0, 93, 0, 0, 0, 16,
    0, 0, 0, 1, 0, 0, 0, 24, 0, 0, 0, 93, 0, 0, 0, 118, 0, 0, 0, 92, 0, 0, 0, 25, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 25, 0, 0, 0, 92, 0, 0, 0, 118, 0, 0, 0, 93, 0, 0, 0, 24, 0, 0, 0, 1,
    0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 24, 0, 0, 0, 92, 0, 0, 0, 118, 0, 0, 0, 92, 0, 0, 0, 25, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 25, 0, 0, 0, 92, 0, 0, 0, 118, 0, 0, 0, 92, 0, 0, 0, 24, 0, 0, 0, 1, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 24, 0, 0, 0, 92, 0, 0, 0, 117, 0, 0, 0, 90, 0, 0, 0, 26, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 26, 0, 0, 0, 90, 0, 0, 0, 117, 0, 0, 0, 92, 0, 0, 0, 24, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 25, 0, 0, 0, 90, 0, 0, 0, 117, 0, 0, 0, 90, 0, 0, 0, 26, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 26, 0, 0, 0, 90, 0, 0, 0, 117, 0, 0, 0, 90, 0, 0, 0, 25, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 25, 0, 0, 0, 92, 0, 0, 0, 116, 0, 0, 0, 90, 0, 0, 0, 28, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 28, 0, 0, 0, 90, 0, 0, 0, 116, 0, 0, 0, 92, 0, 0, 0, 25, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 25, 0, 0, 0, 90, 0, 0, 0, 116, 0, 0, 0, 90, 0, 0, 0, 28, 0, 0, 0, 4, 0, 0, 0, 2, 0, 0, 0, 26, 0, 0, 0, 90, 0, 0, 0, 116, 0, 0, 0, 90, 0, 0, 0, 25, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 25, 0, 0, 0, 92, 0, 0, 0, 116, 0, 0, 0, 90, 0, 0, 0, 30, 0, 0, 0, 29, 0, 0, 0, 90, 0, 0, 0, 116, 0, 0, 0, 92, 0, 0, 0, 25, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 26, 0, 0, 0, 90, 0, 0, 0, 116, 0, 0, 0, 101, 0, 0, 0, 101, 0, 0, 0, 116, 0, 0, 0, 90, 0, 0, 0, 26, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 25, 0, 0, 0, 90, 0, 0, 0, 120, 0, 0, 0, 120, 0, 0, 0, 90, 0, 0, 0, 25, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 26, 0, 0, 0, 89, 0, 0, 0, 89, 0, 0, 0, 26, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 16, 0, 0, 0, 16, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

  Ihandle* image = IupImageRGBA(24, 16, imgdata);
  return image;
}

static Ihandle* load_image_expand_up(void)
{
  unsigned char imgdata[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 16, 0, 0, 0, 16, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 26, 0, 0, 0, 88, 0, 0, 0, 88, 0, 0, 0, 26, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 26, 0, 0, 0, 92, 0, 0, 0, 119, 0, 0, 0, 119, 0, 0, 0, 92, 0, 0, 0, 26, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 26, 0, 0, 0, 90, 0, 0, 0, 116, 0, 0, 0, 100, 0, 0, 0, 100, 0, 0, 0, 114, 0, 0, 0, 90, 0, 0, 0, 26, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 25, 0, 0, 0, 90, 0, 0, 0, 117, 0, 0, 0, 90, 0, 0, 0, 31, 0, 0, 0, 30, 0, 0, 0, 90, 0, 0, 0, 117, 0, 0, 0, 90, 0, 0, 0, 25, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 26, 0, 0, 0, 90, 0, 0, 0, 117, 0, 0, 0, 90, 0, 0, 0, 28, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 26, 0, 0, 0, 90, 0, 0, 0, 117, 0, 0, 0, 90, 0, 0, 0, 26, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 26, 0, 0, 0, 90, 0, 0, 0, 116, 0, 0, 0, 90, 0, 0, 0, 26, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 26, 0, 0, 0, 90, 0, 0, 0, 116, 0, 0, 0, 90, 0, 0, 0, 26, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 25, 0, 0, 0, 92, 0, 0, 0, 117, 0, 0, 0, 92, 0, 0, 0, 26, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 26, 0, 0, 0, 92, 0, 0, 0, 117, 0, 0, 0, 92, 0, 0, 0, 25, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 24, 0, 0, 0, 92, 0, 0, 0, 117, 0, 0, 0, 92, 0, 0, 0, 26, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 26, 0, 0, 0, 92, 0, 0, 0, 117, 0, 0, 0, 92, 0, 0, 0, 24, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 24, 0, 0, 0, 93, 0, 0, 0, 117, 0, 0, 0, 92, 0, 0, 0, 25, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 25, 0, 0, 0, 92, 0, 0, 0, 117, 0, 0, 0, 93, 0, 0, 0, 24, 0, 0, 0, 1, 0, 0, 0, 0,
    0, 0, 0, 1, 0, 0, 0, 24, 0, 0, 0, 93, 0, 0, 0, 118, 0, 0, 0, 93, 0, 0, 0, 25, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 25, 0, 0, 0, 93, 0, 0, 0, 118, 0, 0, 0, 93, 0, 0, 0, 23, 0, 0, 0, 1,
    0, 0, 0, 17, 0, 0, 0, 94, 0, 0, 0, 119, 0, 0, 0, 93, 0, 0, 0, 24, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 24, 0, 0, 0, 93, 0, 0, 0, 119, 0, 0, 0, 94, 0, 0, 0, 16,
    0, 0, 0, 46, 0, 0, 0, 111, 0, 0, 0, 94, 0, 0, 0, 24, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 24, 0, 0, 0, 94, 0, 0, 0, 111, 0, 0, 0, 46,
    0, 0, 0, 7, 0, 0, 0, 48, 0, 0, 0, 20, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 20, 0, 0, 0, 48, 0, 0, 0, 7,
    0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

  Ihandle* image = IupImageRGBA(24, 16, imgdata);
  return image;
}

static void iFlatTabsInitializeImages(void)
{
  Ihandle *image;

  unsigned char img_close[ITABS_CLOSE_SIZE * ITABS_CLOSE_SIZE] =
  {
    1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
    1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1,
    0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0,
    0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0,
    0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 0, 0, 0,
    0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0,
    0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 0, 0, 0,
    0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0,
    0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0,
    1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1,
    1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
  };

  image = IupImage(ITABS_CLOSE_SIZE, ITABS_CLOSE_SIZE, img_close);
  IupSetAttribute(image, "0", "BGCOLOR");
  IupSetAttribute(image, "1", "0 0 0");
  IupSetHandle("IMGFLATCLOSE", image);

  image = IupImage(ITABS_CLOSE_SIZE, ITABS_CLOSE_SIZE, img_close);
  IupSetAttribute(image, "0", "BGCOLOR");
  IupSetAttribute(image, "1", "255 255 255");
  IupSetHandle("IMGFLATCLOSEPRESS", image);

  image = load_image_expand_down();
  IupSetHandle("IupFlatExpandDown", image);

  image = load_image_expand_up();
  IupSetHandle("IupFlatExpandUp", image);
}

static int iFlatTabsGetExtraWidth(Ihandle* ih, int extra_buttons, int img_position, int horiz_padding);
static void iFlatTabsGetTabSize(Ihandle* ih, int fixedwidth, int horiz_padding, int vert_padding, int show_close, int pos, int *tab_w, int *tab_h);

static void iFlatTabsCheckVisibleTab(Ihandle* ih, Ihandle* child)
{
  int child_pos = IupGetChildPos(ih, child);
  int scroll_pos = iupAttribGetInt(ih, "_IUPFTABS_SCROLLPOS");

  if (child_pos == scroll_pos)
    return; /* already visible */

  if (child_pos < scroll_pos)
  {
    /* if before current scroll simply scroll to child */
    iupAttribSetInt(ih, "_IUPFTABS_SCROLLPOS", child_pos);
    return;
  }

  if (child_pos > scroll_pos)
  {
    int extra_width, horiz_padding, vert_padding;
    int tab_w, tab_h, pos;
    int* visible_width, check_width;
    int img_position = iupFlatGetImagePosition(iupAttribGetStr(ih, "TABSIMAGEPOSITION"));
    int extra_buttons = iupAttribGetInt(ih, "EXTRABUTTONS");
    int fixedwidth = iupAttribGetInt(ih, "FIXEDWIDTH");
    int show_close = iupAttribGetBoolean(ih, "SHOWCLOSE");
    int count = IupGetChildCount(ih);
    int old_scroll_pos = scroll_pos;

    iupAttribGetIntInt(ih, "TABSPADDING", &horiz_padding, &vert_padding, 'x');
    extra_width = iFlatTabsGetExtraWidth(ih, extra_buttons, img_position, horiz_padding);

    visible_width = calloc(count, sizeof(int));

    check_width = 0;
    for (pos = scroll_pos, child = ih->firstchild; child && pos <= child_pos; child = child->brother, pos++)
    {
      int tabvisible = iupAttribGetBooleanId(ih, "TABVISIBLE", pos);
      if (tabvisible)
      {
        iFlatTabsGetTabSize(ih, fixedwidth, horiz_padding, vert_padding, show_close, pos, &tab_w, &tab_h);

        visible_width[pos] = tab_w;
        check_width += tab_w;
      }
    }

    while (check_width > ih->currentwidth - extra_width && scroll_pos < child_pos)
    {
      scroll_pos++;

      check_width = 0;
      for (pos = scroll_pos; pos <= child_pos; pos++)
        check_width += visible_width[pos];
    }

    if (old_scroll_pos != scroll_pos)
      iupAttribSetInt(ih, "_IUPFTABS_SCROLLPOS", scroll_pos);

    free(visible_width);
  }
}

static Ihandle* iFlatTabsGetCurrentTab(Ihandle* ih)
{
  return (Ihandle*)iupAttribGet(ih, "_IUPFTABS_VALUE_HANDLE");
}

static void iFlatTabsSetCurrentTab(Ihandle* ih, Ihandle* child)
{
  Ihandle* current_child = iFlatTabsGetCurrentTab(ih);
  if (current_child)
    IupSetAttribute(current_child, "VISIBLE", "No");

  iupAttribSet(ih, "_IUPFTABS_VALUE_HANDLE", (char*)child);
  if (child)
  {
    IupSetAttribute(child, "VISIBLE", "Yes");

    iFlatTabsCheckVisibleTab(ih, child);
  }

  if (ih->handle)
    iupdrvPostRedraw(ih);
}

static void iFlatTabsGetIconSize(Ihandle* ih, int pos, int *w, int *h)
{
  char* image = iupAttribGetId(ih, "TABIMAGE", pos);
  char* title = iupAttribGetId(ih, "TABTITLE", pos);

  *w = 0;
  *h = 0;

  if (image)
  {
    iupImageGetInfo(image, w, h, NULL);

    if (title)
    {
      int img_position = iupFlatGetImagePosition(iupAttribGetStr(ih, "TABSIMAGEPOSITION"));
      int spacing = iupAttribGetInt(ih, "TABSIMAGESPACING");
      int text_w, text_h;

      iupDrawGetTextSize(ih, title, &text_w, &text_h);

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
    iupDrawGetTextSize(ih, title, w, h);
}

static void iFlatTabsSetTabFont(Ihandle* ih, int pos)
{
  char* font = iupAttribGetId(ih, "TABFONT", pos);
  if (font)
    iupAttribSetStr(ih, "DRAWFONT", font);
  else
  {
    char* tabs_font = iupAttribGet(ih, "TABSFONT");
    iupAttribSetStr(ih, "DRAWFONT", tabs_font);
  }
}

static void iFlatTabsGetTabSize(Ihandle* ih, int fixedwidth, int horiz_padding, int vert_padding, int show_close, int pos, int *tab_w, int *tab_h)
{
  iFlatTabsSetTabFont(ih, pos);

  iFlatTabsGetIconSize(ih, pos, tab_w, tab_h);

  if (fixedwidth)
    *tab_w = fixedwidth;

  (*tab_w) += 2 * horiz_padding;
  (*tab_h) += 2 * vert_padding;

  if (show_close)
    (*tab_w) += ITABS_CLOSE_SPACING + ITABS_CLOSE_SIZE + ITABS_CLOSE_BORDER;
}

static int iFlatTabsGetTitleHeight(Ihandle* ih, int *title_width, int scrolled_width)
{
  int vert_padding, horiz_padding;
  int title_height = 0, tab_w, tab_h, pos;
  int fixedwidth = iupAttribGetInt(ih, "FIXEDWIDTH");
  int show_close = iupAttribGetBoolean(ih, "SHOWCLOSE");
  int scroll_pos = iupAttribGetInt(ih, "_IUPFTABS_SCROLLPOS");
  Ihandle* child;

  iupAttribGetIntInt(ih, "TABSPADDING", &horiz_padding, &vert_padding, 'x');

  if (title_width)
    *title_width = 0;

  for (pos = 0, child = ih->firstchild; child; child = child->brother, pos++)
  {
    int tabvisible = iupAttribGetBooleanId(ih, "TABVISIBLE", pos);
    if (tabvisible)
    {
      if (title_width && pos == scroll_pos && scrolled_width)
        *title_width = 0;

      iFlatTabsGetTabSize(ih, fixedwidth, horiz_padding, vert_padding, show_close, pos, &tab_w, &tab_h);

      if (tab_h > title_height)
        title_height = tab_h;

      if (title_width)
        *title_width += tab_w;
    }
  }

  if (pos == 0) /* empty tabs, let the application define at least TABTITLE0 */
  {
    iFlatTabsGetTabSize(ih, fixedwidth, horiz_padding, vert_padding, show_close, pos, &tab_w, &tab_h);
    title_height = tab_h;
    if (title_width) *title_width = tab_w;
  }

  return title_height;
}

static void iFlatTabsSetExtraFont(Ihandle* ih, int id)
{
  char* font = iupAttribGetId(ih, "EXTRAFONT", id);
  if (font)
    iupAttribSetStr(ih, "DRAWFONT", font);
  else
  {
    char* tabs_font = iupAttribGet(ih, "TABSFONT");
    iupAttribSetStr(ih, "DRAWFONT", tabs_font);
  }
}

static int iFlatTabsGetExtraWidthId(Ihandle* ih, int i, int img_position, int horiz_padding)
{
  char* image = iupAttribGetId(ih, "EXTRAIMAGE", i);
  char* title = iupAttribGetId(ih, "EXTRATITLE", i);

  int w = 0;

  iFlatTabsSetExtraFont(ih, i);

  if (image)
  {
    iupImageGetInfo(image, &w, NULL, NULL);

    if (title)
    {
      int spacing = iupAttribGetInt(ih, "TABSIMAGESPACING");
      int text_w, text_h;

      iupDrawGetTextSize(ih, title, &text_w, &text_h);

      if (img_position == IUP_IMGPOS_RIGHT ||
          img_position == IUP_IMGPOS_LEFT)
        w += text_w + spacing;
      else
        w = iupMAX(w, text_w);
    }
  }
  else if (title)
    iupDrawGetTextSize(ih, title, &w, NULL);

  w += 2 * horiz_padding;

  return w;
}

static int iFlatTabsGetExtraWidth(Ihandle* ih, int extra_buttons, int img_position, int horiz_padding)
{
  int extra_width = 0, i;

  if (extra_buttons == 0)
    return 0;

  for (i = 1; i <= extra_buttons; i++)
  {
    int w = iFlatTabsGetExtraWidthId(ih, i, img_position, horiz_padding);
    extra_width += w;
  }

  return extra_width;
}

static int iFlatTabsGetExtraActive(Ihandle* ih, int id)
{
  if (!iupAttribGetId(ih, "EXTRAACTIVE", id))
    return 1; /* default is yes */

  return iupAttribGetBooleanId(ih, "EXTRAACTIVE", id);
}

static void iFlatTabsGetAlignment(const char* alignment, int *horiz_alignment, int *vert_alignment)
{
  char value1[30], value2[30];
  iupStrToStrStr(alignment, value1, value2, ':');
  *horiz_alignment = iupFlatGetHorizontalAlignment(value1);
  *vert_alignment = iupFlatGetVerticalAlignment(value2);
}

static void iFlatTabsDrawScrollLeftButton(IdrawCanvas* dc, const char *tabs_bgcolor, const char *tabs_forecolor, int active, int title_height)
{
  int scroll_width = title_height / 2;
  int arrow_size = (scroll_width + 1) / 2;

  int x = (scroll_width - arrow_size) / 2;
  int y = (title_height - arrow_size) / 2;

  iupFlatDrawArrow(dc, x, y, arrow_size, tabs_forecolor, tabs_bgcolor, active, IUPDRAW_ARROW_LEFT);
}

static void iFlatTabsDrawScrollRightButton(IdrawCanvas* dc, const char *tabs_bgcolor, const char *tabs_forecolor, int active, int title_height, int width)
{
  int scroll_width = title_height/2;
  int arrow_size = (scroll_width + 1) / 2;

  int x = width - 1 - scroll_width + (scroll_width - arrow_size) / 2;
  int y = (title_height - arrow_size) / 2;

  iupFlatDrawArrow(dc, x, y, arrow_size, tabs_forecolor, tabs_bgcolor, active, IUPDRAW_ARROW_RIGHT);
}

static int iFlatTabsRedraw_CB(Ihandle* ih)
{
  Ihandle* current_child = iFlatTabsGetCurrentTab(ih);
  char* bgcolor = iupAttribGetStr(ih, "BGCOLOR");
  char* forecolor = iupAttribGetStr(ih, "FORECOLOR");
  char* highcolor = iupAttribGetStr(ih, "HIGHCOLOR");
  char* tabs_bgcolor = iupAttribGet(ih, "TABSBACKCOLOR");
  char* tabs_forecolor = iupAttribGetStr(ih, "TABSFORECOLOR");
  char* tabs_highcolor = iupAttribGetStr(ih, "TABSHIGHCOLOR");
  int img_position = iupFlatGetImagePosition(iupAttribGetStr(ih, "TABSIMAGEPOSITION"));
  char* alignment = iupAttribGetStr(ih, "TABSALIGNMENT");
  char* text_align = iupAttribGetStr(ih, "TABSTEXTALIGNMENT");
  int active = IupGetInt(ih, "ACTIVE");  /* native implementation */
  int spacing = iupAttribGetInt(ih, "TABSIMAGESPACING");
  int horiz_padding, vert_padding, scroll_pos;
  int show_lines = iupAttribGetBoolean(ih, "SHOWLINES");
  int title_width;
  int title_height = iFlatTabsGetTitleHeight(ih, &title_width, 1);
  int fixedwidth = iupAttribGetInt(ih, "FIXEDWIDTH");
  Ihandle* child;
  int pos, horiz_alignment, vert_alignment, tab_x = 0;
  long line_color = 0;
  int show_close = iupAttribGetBoolean(ih, "SHOWCLOSE");
  int tab_highlighted = iupAttribGetInt(ih, "_IUPFTABS_HIGHLIGHTED");
  int extra_width;
  int extra_buttons = iupAttribGetInt(ih, "EXTRABUTTONS");

  IdrawCanvas* dc = iupdrvDrawCreateCanvas(ih);

  if (!tabs_bgcolor)
    tabs_bgcolor = iupBaseNativeParentGetBgColorAttrib(ih);

  /* draw child area background */
  iupFlatDrawBox(dc, 0, ih->currentwidth - 1, title_height, ih->currentheight - 1, bgcolor, NULL, 1);

  /* title area background */
  iupFlatDrawBox(dc, 0, ih->currentwidth - 1, 0, title_height - 1, tabs_bgcolor, NULL, 1);

  iupAttribGetIntInt(ih, "TABSPADDING", &horiz_padding, &vert_padding, 'x');
  iFlatTabsGetAlignment(alignment, &horiz_alignment, &vert_alignment);
  extra_width = iFlatTabsGetExtraWidth(ih, extra_buttons, img_position, horiz_padding);

  if (show_lines)
  {
    char* title_line_color = iupAttribGetStr(ih, "TABSLINECOLOR");
    line_color = iupDrawStrToColor(title_line_color, line_color);

    /* tab bottom horizontal and top children horizontal */
    iupdrvDrawLine(dc, 0, title_height - 1, ih->currentwidth - 1, title_height - 1, line_color, IUP_DRAW_STROKE, 1);
  }

  scroll_pos = iupAttribGetInt(ih, "_IUPFTABS_SCROLLPOS");
  if (scroll_pos > 0)
  {
    int scroll_width = title_height / 2;
    tab_x += scroll_width;
  }

  child = ih->firstchild;
  for (pos = 0; pos < scroll_pos; pos++)
    child = child->brother;

  for (pos = scroll_pos; child; child = child->brother, pos++)
  {
    int tabvisible = iupAttribGetBooleanId(ih, "TABVISIBLE", pos);
    if (tabvisible)
    {
      char* tab_image = iupAttribGetId(ih, "TABIMAGE", pos);
      char* tab_title = iupAttribGetId(ih, "TABTITLE", pos);
      char* tab_backcolor = iupAttribGetId(ih, "TABBACKCOLOR", pos);
      char* tab_forecolor = iupAttribGetId(ih, "TABFORECOLOR", pos);
      char* tab_highcolor = iupAttribGetId(ih, "TABHIGHCOLOR", pos);
      char* background_color = NULL;
      int tab_w, tab_h, tab_active, reset_clip;
      char* foreground_color;
      int icon_width, make_inactive = 0;

      if (!active)
        tab_active = active;
      else
        tab_active = iupAttribGetBooleanId(ih, "TABACTIVE", pos);

      iFlatTabsGetTabSize(ih, fixedwidth, horiz_padding, vert_padding, show_close, pos, &tab_w, &tab_h);  /* this will also set any id based font */

      if (current_child == child)
      {
        /* current tab is always drawn with these colors */
        background_color = bgcolor;
        foreground_color = forecolor;
      }
      else
      {
        /* other tabs are drawn with these colors */
        if (tab_backcolor)
          background_color = tab_backcolor;

        foreground_color = tabs_forecolor;
        if (tab_forecolor)
          foreground_color = tab_forecolor;

        if (pos == tab_highlighted)
        {
          if (highcolor)
            foreground_color = highcolor;
          else
            foreground_color = forecolor;

          if (tabs_highcolor || tab_highcolor)
          {
            if (tab_highcolor)
              background_color = tab_highcolor;
            else
              background_color = tabs_highcolor;
          }
        }
      }

      if (tab_image)
      {
        make_inactive = 0;

        if (!tab_active)
        {
          char* tab_image_inative = iupAttribGetId(ih, "TABIMAGEINACTIVE", pos);
          if (!tab_image_inative)
            make_inactive = 1;
          else
            tab_image = tab_image_inative;
        }
        else if (pos == tab_highlighted)
        {
          char* tab_image_highlight = iupAttribGetId(ih, "TABIMAGEHIGHLIGHT", pos);
          if (tab_image_highlight)
            tab_image = tab_image_highlight;
        }
      }

      reset_clip = 0;
      if (title_width > ih->currentwidth - extra_width) /* has right scroll button */
      {
        int scroll_width = title_height / 2;
        if (tab_x + tab_w > ih->currentwidth - extra_width - scroll_width)
        {
          iupdrvDrawSetClipRect(dc, tab_x, 0, ih->currentwidth - extra_width - scroll_width, title_height);
          reset_clip = 1;
        }
      }

      /* draw tab title background */
      if (background_color)
        iupFlatDrawBox(dc, tab_x, tab_x + tab_w, 0, title_height - 1, background_color, NULL, 1);
      else
        background_color = tabs_bgcolor;

      if (show_lines && current_child == child)
      {
        iupdrvDrawLine(dc, tab_x, 0, tab_x + tab_w - 1, 0, line_color, IUP_DRAW_STROKE, 1); /* tab top horizontal */
        iupdrvDrawLine(dc, tab_x, 0, tab_x, title_height - 1, line_color, IUP_DRAW_STROKE, 1); /* tab left vertical */
        iupdrvDrawLine(dc, tab_x + tab_w - 1, 0, tab_x + tab_w - 1, title_height - 1, line_color, IUP_DRAW_STROKE, 1); /* tab right vertical */
      }

      icon_width = tab_w;
      if (show_close)
        icon_width -= ITABS_CLOSE_SPACING + ITABS_CLOSE_SIZE + ITABS_CLOSE_BORDER;

      iupFlatDrawIcon(ih, dc, tab_x, 0,
                      icon_width, title_height,
                      img_position, spacing, horiz_alignment, vert_alignment, horiz_padding, vert_padding,
                      tab_image, make_inactive, tab_title, text_align, foreground_color, background_color, tab_active);

      if (show_close)
      {
        int close_x = tab_x + tab_w - ITABS_CLOSE_BORDER - ITABS_CLOSE_SIZE;
        int close_y = (title_height - (ITABS_CLOSE_SIZE)) / 2;
        const char* imagename;
        int tab_close_high = iupAttribGetInt(ih, "_IUPFTABS_CLOSEHIGH");
        int tab_close_press = iupAttribGetInt(ih, "_IUPFTABS_CLOSEPRESS");

        if (pos == tab_close_press)
        {
          background_color = iupAttribGetStr(ih, "CLOSEPRESSCOLOR");
          iupFlatDrawBox(dc, close_x - ITABS_CLOSE_BORDER, close_x + ITABS_CLOSE_SIZE + ITABS_CLOSE_BORDER, close_y - ITABS_CLOSE_BORDER, close_y + ITABS_CLOSE_SIZE + ITABS_CLOSE_BORDER, background_color, NULL, 1);
        }
        else if (pos == tab_close_high)
        {
          background_color = iupAttribGetStr(ih, "CLOSEHIGHCOLOR");
          iupFlatDrawBox(dc, close_x - ITABS_CLOSE_BORDER, close_x + ITABS_CLOSE_SIZE + ITABS_CLOSE_BORDER, close_y - ITABS_CLOSE_BORDER, close_y + ITABS_CLOSE_SIZE + ITABS_CLOSE_BORDER, background_color, NULL, 1);
        }

        imagename = iupFlatGetImageName(ih, "CLOSEIMAGE", NULL, pos == tab_close_press, pos == tab_close_high, tab_active, &make_inactive);
        iupdrvDrawImage(dc, imagename, make_inactive, close_x, close_y);
      }

      /* goto next tab area */
      tab_x += tab_w;

      if (reset_clip)
      {
        iupdrvDrawResetClip(dc);
        break;
      }
    }
  }

  if (scroll_pos > 0)
  {
    char* foreground_color = tabs_forecolor;
    if (tab_highlighted == ITABS_SB_LEFT)
    {
      if (highcolor)
        foreground_color = highcolor;
      else
        foreground_color = forecolor;
    }

    iFlatTabsDrawScrollLeftButton(dc, tabs_bgcolor, foreground_color, active, title_height);
  }

  if (title_width > ih->currentwidth - extra_width)
  {
    char* foreground_color = tabs_forecolor;
    if (tab_highlighted == ITABS_SB_RIGHT)
    {
      if (highcolor)
        foreground_color = highcolor;
      else
        foreground_color = forecolor;
    }

    iFlatTabsDrawScrollRightButton(dc, tabs_bgcolor, foreground_color, active, title_height, ih->currentwidth - extra_width);
  }

  if (extra_buttons)
  {
    int i, right_extra_width = 0, extra_id;
    int extra_active, make_inactive, extra_x, extra_w;
    int extra_horiz_alignment, extra_vert_alignment;

    for (i = 1; i <= extra_buttons; i++)
    {
      const char* extra_image = iupAttribGetId(ih, "EXTRAIMAGE", i);
      char* extra_title = iupAttribGetId(ih, "EXTRATITLE", i);
      char* extra_alignment = iupAttribGetId(ih, "EXTRAALIGNMENT", i);
      char* extra_forecolor = iupAttribGetId(ih, "EXTRAFORECOLOR", i);
      int extra_press = iupAttribGetInt(ih, "_IUPFTABS_EXTRAPRESS");

      extra_horiz_alignment = horiz_alignment; 
      extra_vert_alignment = vert_alignment;
      iFlatTabsGetAlignment(extra_alignment, &extra_horiz_alignment, &extra_vert_alignment);

      if (!active)
        extra_active = active;
      else
        extra_active = iFlatTabsGetExtraActive(ih, i);

      if (!extra_forecolor)
        extra_forecolor = tabs_forecolor;

      extra_id = ITABS_EXTRABUT2TABID(i);

      extra_w = iFlatTabsGetExtraWidthId(ih, i, img_position, horiz_padding);  /* this will also set any id based font */

      extra_x = ih->currentwidth - right_extra_width - extra_w;

      if (extra_press == extra_id)
      {
        char* extra_presscolor = iupAttribGetId(ih, "EXTRAPRESSCOLOR", i);
        if (!extra_presscolor)
          extra_presscolor = iupAttribGetStr(ih, "CLOSEPRESSCOLOR");

        iupFlatDrawBox(dc, extra_x + horiz_padding / 2, extra_x + extra_w - horiz_padding / 2, vert_padding / 2, title_height - 1 - vert_padding / 2, extra_presscolor, NULL, 1);
      }
      else if (tab_highlighted == extra_id)
      {
        char* extra_highcolor = iupAttribGetId(ih, "EXTRAHIGHCOLOR", i);
        if (!extra_highcolor)
          extra_highcolor = iupAttribGetStr(ih, "CLOSEHIGHCOLOR");

        iupFlatDrawBox(dc, extra_x + horiz_padding / 2, extra_x + extra_w - horiz_padding / 2, vert_padding / 2, title_height - 1 - vert_padding / 2, extra_highcolor, NULL, 1);
      }

      extra_image = iupFlatGetImageNameId(ih, "EXTRAIMAGE", i, extra_image, extra_press == extra_id, tab_highlighted == extra_id, extra_active, &make_inactive);

      iupFlatDrawIcon(ih, dc, extra_x, 0,
                      extra_w, title_height - 1,
                      img_position, spacing, extra_horiz_alignment, extra_vert_alignment, horiz_padding, vert_padding,
                      extra_image, make_inactive, extra_title, text_align, extra_forecolor, tabs_bgcolor, extra_active);

      right_extra_width += extra_w;
    }
  }

  /* lines around children */
  if (show_lines)
  {
    iupdrvDrawLine(dc, 0, title_height, 0, ih->currentheight - 1, line_color, IUP_DRAW_STROKE, 1); /* left children vertical */
    iupdrvDrawLine(dc, ih->currentwidth - 1, title_height, ih->currentwidth - 1, ih->currentheight - 1, line_color, IUP_DRAW_STROKE, 1); /* right children vertical */
    iupdrvDrawLine(dc, 0, ih->currentheight - 1, ih->currentwidth - 1, ih->currentheight - 1, line_color, IUP_DRAW_STROKE, 1); /* bottom children horizontal */
  }

  iupdrvDrawFlush(dc);

  iupdrvDrawKillCanvas(dc);

  return IUP_DEFAULT;
}

static int iFlatTabsResize_CB(Ihandle* ih, int width, int height)
{
  int scroll_pos = iupAttribGetInt(ih, "_IUPFTABS_SCROLLPOS");
  if (scroll_pos)
  {
    int title_width;
    iFlatTabsGetTitleHeight(ih, &title_width, 0);

    if (title_width > width)
    {
      /* tabs are larger than the element, leave scroll_pos as it is */
      return IUP_DEFAULT;
    }

    /* tabs fit in element area, reset scroll_pos */
    iupAttribSetInt(ih, "_IUPFTABS_SCROLLPOS", 0);
  }

  (void)height;
  return IUP_DEFAULT;
}

static int iFlatTabsCallTabChange(Ihandle* ih, Ihandle* prev_child, int prev_pos, Ihandle* child)
{
  IFnnn cb = (IFnnn)IupGetCallback(ih, "TABCHANGE_CB");
  int ret = IUP_DEFAULT;

  if (cb)
    ret = cb(ih, child, prev_child);
  else
  {
    IFnii cb2 = (IFnii)IupGetCallback(ih, "TABCHANGEPOS_CB");
    if (cb2)
    {
      int pos = IupGetChildPos(ih, child);
      ret = cb2(ih, pos, prev_pos);
    }
  }

  return ret;
}

static void iFlatTabsCheckCurrentTab(Ihandle* ih, Ihandle* check_child, int check_pos, int removed)
{
  Ihandle* current_child = iFlatTabsGetCurrentTab(ih);
  if (current_child == check_child)
  {
    int p;
    Ihandle* child;

    /* if given tab is the current tab,
    then the current tab must be changed to a visible tab */

    /* this function is also called after the child has being removed from the hierarchy,
    but before the system tab being removed. */

    /* look forward first */
    child = IupGetChild(ih, check_pos);
    if (child) /* could be the last */
    {
      p = check_pos;
      if (removed)
        p++;  /* increment to compensate for child already removed, but id based attributes are not updated yet */
      else
      {
        child = child->brother;
        p++; /* increment to get the next child */
      }

      for (; child; child = child->brother, p++)
      {
        if (iupAttribGetBooleanId(ih, "TABVISIBLE", p))
        {
          /* found a new tab to be current */
          if (iupAttribGetBoolean(ih, "TABCHANGEONCHECK"))
            iFlatTabsCallTabChange(ih, check_child, check_pos, child); /* ignore return value */

          iFlatTabsSetCurrentTab(ih, child);
          return;
        }
      }
    }

    /* look backward */
    child = IupGetChild(ih, check_pos - 1);
    if (child) /* could be the first */
    {
      p = check_pos - 1;

      while (p >= 0 && child)
      {
        if (iupAttribGetBooleanId(ih, "TABVISIBLE", p))
        {
          /* found a new tab to be current */
          if (iupAttribGetBoolean(ih, "TABCHANGEONCHECK"))
            iFlatTabsCallTabChange(ih, check_child, check_pos, child); /* ignore return value */

          iFlatTabsSetCurrentTab(ih, child);
          return;
        }

        p--;
        child = IupGetChild(ih, p);
      }
    }

    /* could NOT find a new tab to be current (empty or all invisible) */
    iFlatTabsSetCurrentTab(ih, NULL);
  }
}

static int iFlatTabsFindTab(Ihandle* ih, int cur_x, int cur_y, int show_close, int *inside_close)
{
  int title_width;
  int title_height = iFlatTabsGetTitleHeight(ih, &title_width, 1);

  *inside_close = 0;

  if (cur_y < title_height)
  {
    Ihandle* child;
    int pos, horiz_padding, vert_padding, tab_x = 0, scroll_pos;
    int fixedwidth = iupAttribGetInt(ih, "FIXEDWIDTH");
    int img_position = iupFlatGetImagePosition(iupAttribGetStr(ih, "TABSIMAGEPOSITION"));
    int extra_width;
    int extra_buttons = iupAttribGetInt(ih, "EXTRABUTTONS");

    iupAttribGetIntInt(ih, "TABSPADDING", &horiz_padding, &vert_padding, 'x');
    extra_width = iFlatTabsGetExtraWidth(ih, extra_buttons, img_position, horiz_padding);

    scroll_pos = iupAttribGetInt(ih, "_IUPFTABS_SCROLLPOS");

    if (scroll_pos > 0)
    {
      int scroll_width = title_height / 2;
      if (cur_x < scroll_width)
        return ITABS_SB_LEFT;

      tab_x += scroll_width;
    }

    if (title_width > ih->currentwidth - extra_width)
    {
      int scroll_width = title_height / 2;
      if (cur_x > ih->currentwidth - extra_width - scroll_width && cur_x < ih->currentwidth - extra_width)
        return ITABS_SB_RIGHT;
    }

    if (extra_buttons)
    {
      int i, right_extra_width = 0;
      for (i = 1; i <= extra_buttons; i++)
      {
        int w = iFlatTabsGetExtraWidthId(ih, i, img_position, horiz_padding);

        if (cur_x > ih->currentwidth - right_extra_width - w && cur_x < ih->currentwidth - right_extra_width)
          return ITABS_EXTRABUT2TABID(i);

        right_extra_width += w;
      }
    }

    child = ih->firstchild;
    for (pos = 0; pos < scroll_pos; pos++)
      child = child->brother;

    for (pos = scroll_pos; child; child = child->brother, pos++)
    {
      int tabvisible = iupAttribGetBooleanId(ih, "TABVISIBLE", pos);
      if (tabvisible)
      {
        int tab_w, tab_h;

        iFlatTabsGetTabSize(ih, fixedwidth, horiz_padding, vert_padding, show_close, pos, &tab_w, &tab_h);

        if (cur_x > tab_x && cur_x < tab_x + tab_w)
        {
          if (show_close)
          {
            int close_end = tab_x + tab_w - ITABS_CLOSE_BORDER;
            int close_start = close_end - ITABS_CLOSE_SIZE;
            if (cur_x >= close_start && cur_x <= close_end)
              *inside_close = 1;
          }

          return pos;
        }

        tab_x += tab_w;

        if (tab_x > ih->currentwidth)
          break;
      }
    }
  }

  return ITABS_NONE;
}


/*****************************************************************************************/

static void iFlatTabsToggleExpand(Ihandle* ih)
{
  int expand_pos = iupAttribGetInt(ih, "EXPANDBUTTONPOS");
  int expand_state = iupAttribGetBoolean(ih, "EXPANDBUTTONSTATE");
  if (expand_state)
  {
    int title_height = iFlatTabsGetTitleHeight(ih, NULL, 0);
    iupAttribSetId(ih, "EXTRAIMAGE", expand_pos, "IupFlatExpandDown");
    iupAttribSet(ih, "EXPANDBUTTONSTATE", "No");
    IupSetStrf(ih, "MAXSIZE", "x%d", title_height);
    iupAttribSetInt(ih, "_IUP_FULLHEIGHT", ih->currentheight);
    IupRefresh(ih);
  }
  else
  {
    iupAttribSetId(ih, "EXTRAIMAGE", expand_pos, "IupFlatExpandUp");
    iupAttribSet(ih, "EXPANDBUTTONSTATE", "Yes");
    IupSetAttribute(ih, "MAXSIZE", NULL);
    iupAttribSet(ih, "_IUP_FULLHEIGHT", NULL);
    IupRefresh(ih);
  }
}

static int iFlatTabsKillFocus_CB(Ihandle* ih)
{
  ih->currentheight = IupGetInt2(ih, "MAXSIZE");
  iupLayoutUpdate(ih);
  IupSetCallback(ih, "KILLFOCUS_CB", NULL);
  return IUP_DEFAULT;
}

static int iFlatTabsButton_CB(Ihandle* ih, int button, int pressed, int x, int y, char* status)
{
  IFniiiis cb = (IFniiiis)IupGetCallback(ih, "FLAT_BUTTON_CB");
  if (cb)
  {
    if (cb(ih, button, pressed, x, y, status) == IUP_IGNORE)
      return IUP_DEFAULT;
  }

  if (button == IUP_BUTTON1 && pressed)
  {
    int inside_close;
    int show_close = iupAttribGetBoolean(ih, "SHOWCLOSE");
    int tab_found = iFlatTabsFindTab(ih, x, y, show_close, &inside_close);
    if (tab_found > ITABS_NONE && iupAttribGetBooleanId(ih, "TABACTIVE", tab_found))
    {
      if (show_close && inside_close)
      {
        iupAttribSetInt(ih, "_IUPFTABS_CLOSEPRESS", tab_found);  /* used for press feedback */
        iupdrvPostRedraw(ih);
      }
      else
      {
        Ihandle* child = IupGetChild(ih, tab_found);
        Ihandle* prev_child = iFlatTabsGetCurrentTab(ih);

        iupAttribSetInt(ih, "_IUPFTABS_CLOSEPRESS", ITABS_NONE);

        if (prev_child != child)
        {
          int prev_pos = IupGetChildPos(ih, prev_child);
          int ret = iFlatTabsCallTabChange(ih, prev_child, prev_pos, child);
          if (ret == IUP_DEFAULT)
            iFlatTabsSetCurrentTab(ih, child);
        }

        if (iupAttribGetBoolean(ih, "EXPANDBUTTON") && !iupAttribGetBoolean(ih, "EXPANDBUTTONSTATE"))
        {
          ih->currentheight = iupAttribGetInt(ih, "_IUP_FULLHEIGHT");
          IupSetAttribute(ih, "ZORDER", "TOP");
          iupLayoutUpdate(ih);
          IupSetCallback(ih, "KILLFOCUS_CB", (Icallback)iFlatTabsKillFocus_CB);
        }
      }
    }
    else if (tab_found == ITABS_SB_LEFT)
    {
      int pos = iupAttribGetInt(ih, "_IUPFTABS_SCROLLPOS");
      while (pos > 0)
      {
        pos--;
        if (iupAttribGetBooleanId(ih, "TABVISIBLE", pos))
          break;
      }
      iupAttribSetInt(ih, "_IUPFTABS_SCROLLPOS", pos);
      iupdrvPostRedraw(ih);
    }
    else if (tab_found == ITABS_SB_RIGHT)
    {
      int pos = iupAttribGetInt(ih, "_IUPFTABS_SCROLLPOS");
      int count = IupGetChildCount(ih);
      while (pos < count)
      {
        pos++;
        if (iupAttribGetBooleanId(ih, "TABVISIBLE", pos))
          break;
      }
      iupAttribSetInt(ih, "_IUPFTABS_SCROLLPOS", pos);
      iupdrvPostRedraw(ih);
    }
    else if (tab_found <= ITABS_EXTRABUTTON1)
    {
      IFnii cb = (IFnii)IupGetCallback(ih, "EXTRABUTTON_CB");
      if (cb)
        cb(ih, ITABS_TABID2EXTRABUT(tab_found), 1);

      iupAttribSetInt(ih, "_IUPFTABS_EXTRAPRESS", tab_found);
      iupdrvPostRedraw(ih);
    }
  }
  else if (button == IUP_BUTTON1 && !pressed)
  {
    int extra_buttons;
    int tab_found = ITABS_NONE;

    int show_close = iupAttribGetBoolean(ih, "SHOWCLOSE");
    if (show_close)
    {
      int tab_close_press = iupAttribGetInt(ih, "_IUPFTABS_CLOSEPRESS");
      int inside_close;
      tab_found = iFlatTabsFindTab(ih, x, y, show_close, &inside_close);

      iupAttribSetInt(ih, "_IUPFTABS_CLOSEPRESS", ITABS_NONE);

      if (tab_found > ITABS_NONE && iupAttribGetBooleanId(ih, "TABACTIVE", tab_found) && inside_close && tab_close_press == tab_found)
      {
        Ihandle *child = IupGetChild(ih, tab_found);
        if (child)
        {
          int ret = IUP_DEFAULT;
          IFni cb = (IFni)IupGetCallback(ih, "TABCLOSE_CB");
          if (cb)
            ret = cb(ih, tab_found);

          if (ret == IUP_CONTINUE) /* destroy tab and children */
          {
            IupDestroy(child);
            IupRefreshChildren(ih);
          }
          else if (ret == IUP_DEFAULT) /* hide tab and children */
            IupSetAttributeId(ih, "TABVISIBLE", tab_found, "No");
          else
            iupdrvPostRedraw(ih);
        }
      }
    }

    extra_buttons = iupAttribGetInt(ih, "EXTRABUTTONS");
    if (extra_buttons)
    {
      int extra_press = iupAttribGetInt(ih, "_IUPFTABS_EXTRAPRESS");
      int inside_close;
      if (!show_close)
        tab_found = iFlatTabsFindTab(ih, x, y, show_close, &inside_close);

      iupAttribSetInt(ih, "_IUPFTABS_EXTRAPRESS", ITABS_NONE);

      if (tab_found <= ITABS_EXTRABUTTON1 && iFlatTabsGetExtraActive(ih, tab_found) && extra_press == tab_found)
      {
        IFnii cb = (IFnii)IupGetCallback(ih, "EXTRABUTTON_CB");
        if (cb)
          cb(ih, ITABS_TABID2EXTRABUT(tab_found), 0);

        if (iupAttribGetBoolean(ih, "EXPANDBUTTON") && ITABS_TABID2EXTRABUT(tab_found) == iupAttribGetInt(ih, "EXPANDBUTTONPOS"))
          iFlatTabsToggleExpand(ih);

        iupdrvPostRedraw(ih);
      }
    }
  }
  else if (button == IUP_BUTTON3 && pressed)
  {
    IFni cb = (IFni)IupGetCallback(ih, "RIGHTCLICK_CB");
    if (cb)
    {
      int show_close = iupAttribGetBoolean(ih, "SHOWCLOSE");
      int inside_close;
      int tab_found = iFlatTabsFindTab(ih, x, y, show_close, &inside_close);
      if (tab_found > ITABS_NONE && iupAttribGetBooleanId(ih, "TABACTIVE", tab_found))
        cb(ih, tab_found);
    }
  }

  return IUP_DEFAULT;
}

static void iFlatTabsSetTipVisible(Ihandle* ih, const char* tip)
{
  int visible = IupGetInt(ih, "TIPVISIBLE");

  /* do not call IupSetAttribute */
  iupAttribSetStr(ih, "TIP", tip);
  iupdrvBaseSetTipAttrib(ih, tip);

  if (visible)
  {
    IupSetAttribute(ih, "TIPVISIBLE", "No");
    if (tip)
      IupSetAttribute(ih, "TIPVISIBLE", "Yes");
  }
}

static int iFlatTabsCheckTip(Ihandle* ih, const char* new_tip)
{
  char* tip = iupAttribGet(ih, "TIP");
  if (!tip && !new_tip)
    return 1;
  if (iupStrEqual(tip, new_tip))
    return 1;
  return 0;
}

static void iFlatTabsResetTip(Ihandle* ih)
{
  char* tip = iupAttribGet(ih, "TABSTIP");
  if (!iFlatTabsCheckTip(ih, tip))
    iFlatTabsSetTipVisible(ih, tip);
}

static void iFlatTabsSetTip(Ihandle *ih, const char* tip)
{
  if (!iFlatTabsCheckTip(ih, tip))
    iFlatTabsSetTipVisible(ih, tip);
}

static int iFlatTabsMotion_CB(Ihandle *ih, int x, int y, char *status)
{
  int tab_found, tab_highlighted, redraw = 0;
  int inside_close, show_close, tab_active;

  IFniis cb = (IFniis)IupGetCallback(ih, "FLAT_MOTION_CB");
  if (cb)
  {
    if (cb(ih, x, y, status) == IUP_IGNORE)
      return IUP_DEFAULT;
  }

  show_close = iupAttribGetBoolean(ih, "SHOWCLOSE");
  tab_highlighted = iupAttribGetInt(ih, "_IUPFTABS_HIGHLIGHTED");
  tab_found = iFlatTabsFindTab(ih, x, y, show_close, &inside_close);

  if (tab_found == ITABS_NONE)
    iFlatTabsResetTip(ih);
  else
  {
    if (tab_found > ITABS_NONE)
    {
      char* tab_tip = iupAttribGetId(ih, "TABTIP", tab_found);
      if (tab_tip)
        iFlatTabsSetTip(ih, tab_tip);
      else
        iFlatTabsResetTip(ih);
    }
    else
    {
      char* extra_tip = iupAttribGetId(ih, "EXTRATIP", ITABS_TABID2EXTRABUT(tab_found));
      if (extra_tip)
        iFlatTabsSetTip(ih, extra_tip);
      else
        iFlatTabsResetTip(ih);
    }
  }

  tab_active = 1;
  if (tab_found > ITABS_NONE)
    tab_active = iupAttribGetBooleanId(ih, "TABACTIVE", tab_found);

  if (!tab_active)
    return IUP_DEFAULT;

  if (tab_found != tab_highlighted && !inside_close)
  {
    iupAttribSetInt(ih, "_IUPFTABS_HIGHLIGHTED", tab_found);
    redraw = 1;
  }

  if (show_close && tab_found >= ITABS_NONE)
  {
    int tab_close_high, tab_close_press;

    tab_close_high = iupAttribGetInt(ih, "_IUPFTABS_CLOSEHIGH");
    if (inside_close)
    {
      if (tab_close_high != tab_found)
      {
        iupAttribSetInt(ih, "_IUPFTABS_HIGHLIGHTED", ITABS_NONE);
        iupAttribSetInt(ih, "_IUPFTABS_CLOSEHIGH", tab_found);
        redraw = 1;
      }
    }
    else
    {
      if (tab_close_high != ITABS_NONE)
      {
        iupAttribSetInt(ih, "_IUPFTABS_CLOSEHIGH", ITABS_NONE);
        redraw = 1;
      }
    }

    tab_close_press = iupAttribGetInt(ih, "_IUPFTABS_CLOSEPRESS");
    if (tab_close_press != ITABS_NONE && !inside_close)
    {
      iupAttribSetInt(ih, "_IUPFTABS_CLOSEPRESS", ITABS_NONE);
      redraw = 1;
    }
  }

  if (redraw)
    iupdrvPostRedraw(ih);

  return IUP_DEFAULT;
}

static int iFlatTabsLeaveWindow_CB(Ihandle* ih)
{
  int tab_highlighted, tab_close_high, redraw = 0;

  IFn cb = (IFn)IupGetCallback(ih, "FLAT_LEAVEWINDOW_CB");
  if (cb)
  {
    if (cb(ih) == IUP_IGNORE)
      return IUP_DEFAULT;
  }

  tab_highlighted = iupAttribGetInt(ih, "_IUPFTABS_HIGHLIGHTED");
  if (tab_highlighted != ITABS_NONE)
  {
    iupAttribSetInt(ih, "_IUPFTABS_HIGHLIGHTED", ITABS_NONE);
    redraw = 1;
  }

  tab_close_high = iupAttribGetInt(ih, "_IUPFTABS_CLOSEHIGH");
  if (tab_close_high != ITABS_NONE)
  {
    iupAttribSetInt(ih, "_IUPFTABS_CLOSEHIGH", ITABS_NONE);
    redraw = 1;
  }

  iFlatTabsResetTip(ih);

  if (redraw)
    iupdrvPostRedraw(ih);

  return IUP_DEFAULT;
}


/*****************************************************************************************/


static int iFlatTabsSetValueHandleAttrib(Ihandle* ih, const char* value)
{
  Ihandle* current_child;
  Ihandle *child = (Ihandle*)value;

  if (!iupObjectCheck(child))
    return 0;

  current_child = iFlatTabsGetCurrentTab(ih);
  if (current_child != child)
  {
    int pos = IupGetChildPos(ih, child);
    if (pos != -1) /* found child */
      iFlatTabsSetCurrentTab(ih, child);
  }

  return 0;
}

static char* iFlatTabsGetValueHandleAttrib(Ihandle* ih)
{
  return (char*)iFlatTabsGetCurrentTab(ih);
}

static char* iFlatTabsGetCountAttrib(Ihandle* ih)
{
  return iupStrReturnInt(IupGetChildCount(ih));
}

static int iFlatTabsSetValuePosAttrib(Ihandle* ih, const char* value)
{
  Ihandle* child, *current_child;
  int pos;

  if (!iupStrToInt(value, &pos))
    return 0;

  child = IupGetChild(ih, pos);
  current_child = iFlatTabsGetCurrentTab(ih);
  if (child && current_child != child)  /* found child and NOT current */
    iFlatTabsSetCurrentTab(ih, child);

  return 0;
}

static char* iFlatTabsGetValuePosAttrib(Ihandle* ih)
{
  Ihandle* current_child = iFlatTabsGetCurrentTab(ih);
  int pos = IupGetChildPos(ih, current_child);
  if (pos != -1) /* found child */
    return iupStrReturnInt(pos);
  return NULL;
}

static int iFlatTabsSetValueAttrib(Ihandle* ih, const char* value)
{
  Ihandle *child;

  if (!value)
    return 0;

  child = IupGetHandle(value);
  if (!child)
    return 0;

  iFlatTabsSetValueHandleAttrib(ih, (char*)child);

  return 0;
}

static char* iFlatTabsGetValueAttrib(Ihandle* ih)
{
  Ihandle* child = (Ihandle*)iFlatTabsGetValueHandleAttrib(ih);
  return IupGetName(child);
}

static int iFlatTabsSetTabVisibleAttrib(Ihandle* ih, int pos, const char* value)
{
  Ihandle* child = IupGetChild(ih, pos);
  if (child)
  {
    if (!iupStrBoolean(value))
    {
      iupAttribSetStrId(ih, "TABVISIBLE", pos, value);
      iFlatTabsCheckCurrentTab(ih, child, pos, 0);
    }
  }

  if (ih->handle)
    iupdrvPostRedraw(ih);

  return 1;
}

static char* iFlatTabsGetClientSizeAttrib(Ihandle* ih)
{
  int width = ih->currentwidth;
  int height = ih->currentheight;

  height -= iFlatTabsGetTitleHeight(ih, NULL, 0);

  if (iupAttribGetBoolean(ih, "SHOWLINES"))
  {
    height -= 1;
    width -= 2;
  }

  if (width < 0) width = 0;
  if (height < 0) height = 0;

  return iupStrReturnIntInt(width, height, 'x');
}

static char* iFlatTabsGetBgColorAttrib(Ihandle* ih)
{
  if (iupAttribGet(ih, "BGCOLOR"))
    return NULL;  /* get from the hash table */
  else
    return "255 255 255";
}

static int iFlatTabsSetTipAttrib(Ihandle* ih, const char* value)
{
  iupAttribSetStr(ih, "TABSTIP", value);
  return iupdrvBaseSetTipAttrib(ih, value);
}

static int iFlatTabsSetActiveAttrib(Ihandle* ih, const char* value)
{
  iupdrvPostRedraw(ih);
  return iupBaseSetActiveAttrib(ih, value);
}

static int iFlatTabsUpdateSetAttrib(Ihandle* ih, const char* value)
{
  (void)value;
  if (ih->handle)
    iupdrvPostRedraw(ih);
  return 1;
}

static int iFlatTabsSetTabFontStyleAttrib(Ihandle* ih, int id, const char* value)
{
  int size = 0;
  int is_bold = 0,
    is_italic = 0,
    is_underline = 0,
    is_strikeout = 0;
  char typeface[1024];
  char* font;

  if (!value)
    return 0;

  font = iupAttribGetId(ih, "TABFONT", id);
  if (!font)
    font = iupAttribGet(ih, "TABSFONT");
  if (!font)
    font = IupGetAttribute(ih, "FONT");

  if (!iupGetFontInfo(font, typeface, &size, &is_bold, &is_italic, &is_underline, &is_strikeout))
    return 0;

  IupSetfAttributeId(ih, "TABFONT", id, "%s, %s %d", typeface, value, size);

  return 0;
}

static char* iFlatTabsGetTabFontStyleAttrib(Ihandle* ih, int id)
{
  int size = 0;
  int is_bold = 0,
    is_italic = 0,
    is_underline = 0,
    is_strikeout = 0;
  char typeface[1024];

  char* font = iupAttribGetId(ih, "TABFONT", id);
  if (!font)
    font = iupAttribGet(ih, "TABSFONT");
  if (!font)
    font = IupGetAttribute(ih, "FONT");

  if (!iupGetFontInfo(font, typeface, &size, &is_bold, &is_italic, &is_underline, &is_strikeout))
    return NULL;

  return iupStrReturnStrf("%s%s%s%s", is_bold ? "Bold " : "", is_italic ? "Italic " : "", is_underline ? "Underline " : "", is_strikeout ? "Strikeout " : "");
}

static int iFlatTabsSetTabFontSizeAttrib(Ihandle* ih, int id, const char* value)
{
  int size = 0;
  int is_bold = 0,
    is_italic = 0,
    is_underline = 0,
    is_strikeout = 0;
  char typeface[1024];
  char* font;

  if (!value)
    return 0;

  font = iupAttribGetId(ih, "TABFONT", id);
  if (!font)
    font = iupAttribGet(ih, "TABSFONT");
  if (!font)
    font = IupGetAttribute(ih, "FONT");

  if (!iupGetFontInfo(font, typeface, &size, &is_bold, &is_italic, &is_underline, &is_strikeout))
    return 0;

  IupSetfAttributeId(ih, "TABFONT", id, "%s, %s %d", typeface, is_bold ? "Bold " : "", is_italic ? "Italic " : "", is_underline ? "Underline " : "", is_strikeout ? "Strikeout " : "", value);

  return 0;
}

static char* iFlatTabsGetTabFontSizeAttrib(Ihandle* ih, int id)
{
  int size = 0;
  int is_bold = 0,
    is_italic = 0,
    is_underline = 0,
    is_strikeout = 0;
  char typeface[1024];

  char* font = iupAttribGetId(ih, "TABFONT", id);
  if (!font)
    font = iupAttribGet(ih, "TABSFONT");
  if (!font)
    font = IupGetAttribute(ih, "FONT");

  if (!iupGetFontInfo(font, typeface, &size, &is_bold, &is_italic, &is_underline, &is_strikeout))
    return NULL;

  return iupStrReturnInt(size);
}

static char* iFlatTabsGetTabsFontSizeAttrib(Ihandle* ih)
{
  int size = 0;
  int is_bold = 0,
    is_italic = 0,
    is_underline = 0,
    is_strikeout = 0;
  char typeface[1024];

  char* font = iupAttribGet(ih, "TABSFONT");
  if (!font)
    font = IupGetAttribute(ih, "FONT");

  if (!iupGetFontInfo(font, typeface, &size, &is_bold, &is_italic, &is_underline, &is_strikeout))
    return NULL;

  return iupStrReturnInt(size);
}

static int iFlatTabsSetTabsFontSizeAttrib(Ihandle* ih, const char* value)
{
  int size = 0;
  int is_bold = 0,
    is_italic = 0,
    is_underline = 0,
    is_strikeout = 0;
  char typeface[1024];
  char* font;

  if (!value)
    return 0;

  font = iupAttribGet(ih, "TABSFONT");
  if (!font)
    font = IupGetAttribute(ih, "FONT");

  if (!iupGetFontInfo(font, typeface, &size, &is_bold, &is_italic, &is_underline, &is_strikeout))
    return 0;

  IupSetfAttribute(ih, "TABSFONT", "%s, %s%s%s%s %s", typeface, is_bold ? "Bold " : "", is_italic ? "Italic " : "", is_underline ? "Underline " : "", is_strikeout ? "Strikeout " : "", value);
  return 0;
}

static char* iFlatTabsGetTabsFontStyleAttrib(Ihandle* ih)
{
  int size = 0;
  int is_bold = 0,
    is_italic = 0,
    is_underline = 0,
    is_strikeout = 0;
  char typeface[1024];

  char* font = iupAttribGet(ih, "TABSFONT");
  if (!font)
    font = IupGetAttribute(ih, "FONT");

  if (!iupGetFontInfo(font, typeface, &size, &is_bold, &is_italic, &is_underline, &is_strikeout))
    return NULL;

  return iupStrReturnStrf("%s%s%s%s", is_bold ? "Bold " : "", is_italic ? "Italic " : "", is_underline ? "Underline " : "", is_strikeout ? "Strikeout " : "");
}

static int iFlatTabsSetTabsFontStyleAttrib(Ihandle* ih, const char* value)
{
  int size = 0;
  int is_bold = 0,
    is_italic = 0,
    is_underline = 0,
    is_strikeout = 0;
  char typeface[1024];
  char* font;

  if (!value)
    return 0;

  font = iupAttribGet(ih, "TABSFONT");
  if (!font)
    font = IupGetAttribute(ih, "FONT");

  if (!iupGetFontInfo(font, typeface, &size, &is_bold, &is_italic, &is_underline, &is_strikeout))
    return 0;

  IupSetfAttribute(ih, "TABSFONT", "%s, %s %d", typeface, value, size);

  return 0;
}

static int iFlatTabsSetExpandButtonAttrib(Ihandle* ih, const char* value)
{
  if (iupStrBoolean(value) && !iupAttribGetBoolean(ih, "EXPANDBUTTON"))
  {
    int extra_buttons = iupAttribGetInt(ih, "EXTRABUTTONS");
    extra_buttons++;
    iupAttribSetInt(ih, "EXTRABUTTONS", extra_buttons);
    iupAttribSetInt(ih, "EXPANDBUTTONPOS", extra_buttons);

    iupAttribSetId(ih, "EXTRAIMAGE", extra_buttons, "IupFlatExpandUp");
    iupAttribSetId(ih, "EXTRAALIGNMENT", extra_buttons, "ACENTER:ABOTTOM");
  }

  return 1;
}

static int iFlatTabsSetExpandButtonStateAttrib(Ihandle* ih, const char* value)
{
  if (iupAttribGetBoolean(ih, "EXPANDBUTTON"))
  {
    int expand_state = iupAttribGetBoolean(ih, "EXPANDBUTTONSTATE");
    if (iupStrBoolean(value))
    {
      if (!expand_state)
        iFlatTabsToggleExpand(ih);
    }
    else
    {
      if (expand_state)
        iFlatTabsToggleExpand(ih);
    }
  }
  return 1;
}


/*********************************************************************************/


static int iFlatTabsConvertXYToPos(Ihandle* ih, int x, int y)
{
  int inside_close;
  int show_close = iupAttribGetBoolean(ih, "SHOWCLOSE");
  int tab_found = iFlatTabsFindTab(ih, x, y, show_close, &inside_close);
  if (tab_found > ITABS_NONE)
    return tab_found;
  else
    return -1;
}

#define ATTRIB_ID_COUNT 8
const static char* flattabs_attrib_id[ATTRIB_ID_COUNT] = {
  "TABTITLE",
  "TABIMAGE",
  "TABVISIBLE",
  "TABACTIVE",
  "TABFORECOLOR",
  "TABBACKCOLOR",
  "TABHIGHCOLOR",
  "TABFONT"
};

static void iFlatTabsChildAddedMethod(Ihandle* ih, Ihandle* child)
{
  Ihandle* current_child;
  char* bgcolor;
  int i, p, count, pos;

  pos = IupGetChildPos(ih, child);
  count = IupGetChildCount(ih);

  /* if inserted before last tab, must update Id based attributes */
  for (p = count - 1; p > pos; p--)
  {
    for (i = 0; i < ATTRIB_ID_COUNT; i++)
    {
      char* value = iupAttribGetId(ih, flattabs_attrib_id[i], p - 1);
      iupAttribSetStrId(ih, flattabs_attrib_id[i], p, value);
      iupAttribSetStrId(ih, flattabs_attrib_id[i], p - 1, NULL);
    }
  }

  /* transfer form child to Id based attribute */
  for (i = 0; i < ATTRIB_ID_COUNT; i++)
  {
    if (!iupAttribGetId(ih, flattabs_attrib_id[i], pos))
    {
      char* value = iupAttribGet(child, flattabs_attrib_id[i]);
      if (value)
        iupAttribSetStrId(ih, flattabs_attrib_id[i], pos, value);
      else if (iupStrEqual(flattabs_attrib_id[i], "TABVISIBLE") || iupStrEqual(flattabs_attrib_id[i], "TABACTIVE"))
        iupAttribSetStrId(ih, flattabs_attrib_id[i], pos, "Yes");  /* ensure a default value */
    }
  }

  /* make sure it has at least one name */
  if (!iupAttribGetHandleName(child))
    iupAttribSetHandleName(child);

  bgcolor = iupAttribGetStr(ih, "BGCOLOR");
  iupAttribSetStr(child, "BGCOLOR", bgcolor);

  current_child = iFlatTabsGetCurrentTab(ih);
  if (!current_child)
    iFlatTabsSetCurrentTab(ih, child);
  else
  {
    IupSetAttribute(child, "VISIBLE", "No");
    if (ih->handle)
      iupdrvPostRedraw(ih);
  }
}

static void iFlatTabsChildRemovedMethod(Ihandle* ih, Ihandle* child, int pos)
{
  int p, i, count;

  iFlatTabsCheckCurrentTab(ih, child, pos, 1);

  count = IupGetChildCount(ih);

  /* if removed before last tab, must update Id based attributes */
  for (p = pos; p < count; p++)
  {
    for (i = 0; i < ATTRIB_ID_COUNT; i++)
    {
      char* value = iupAttribGetId(ih, flattabs_attrib_id[i], p + 1);
      iupAttribSetStrId(ih, flattabs_attrib_id[i], p, value);
      iupAttribSetStrId(ih, flattabs_attrib_id[i], p + 1, NULL);
    }
  }

  if (ih->handle)
  {
    int scroll_pos = iupAttribGetInt(ih, "_IUPFTABS_SCROLLPOS");
    if (scroll_pos >= count)
      iupAttribSetInt(ih, "_IUPFTABS_SCROLLPOS", 0);

    iupdrvPostRedraw(ih);
  }
}

static void iFlatTabsComputeNaturalSizeMethod(Ihandle* ih, int *w, int *h, int *children_expand)
{
  Ihandle* child;
  int children_naturalwidth, children_naturalheight;

  /* calculate total children natural size (even for hidden children) */
  children_naturalwidth = 0;
  children_naturalheight = 0;

  for (child = ih->firstchild; child; child = child->brother)
  {
    /* update child natural size first */
    iupBaseComputeNaturalSize(child);

    *children_expand |= child->expand;
    children_naturalwidth = iupMAX(children_naturalwidth, child->naturalwidth);
    children_naturalheight = iupMAX(children_naturalheight, child->naturalheight);
  }

  *w = children_naturalwidth;
  *h = children_naturalheight + iFlatTabsGetTitleHeight(ih, NULL, 0);

  if (iupAttribGetBoolean(ih, "SHOWLINES"))
  {
    *h += 1;
    *w += 2;
  }
}

static void iFlatTabsSetChildrenCurrentSizeMethod(Ihandle* ih, int shrink)
{
  Ihandle* child;
  int title_width;
  int title_height = iFlatTabsGetTitleHeight(ih, &title_width, 0);
  int width = ih->currentwidth;
  int height = ih->currentheight - title_height;

  if (iupAttribGetBoolean(ih, "SHOWLINES"))
  {
    width -= 2;
    height -= 1;
  }

  if (width < 0) width = 0;
  if (height < 0) height = 0;

  for (child = ih->firstchild; child; child = child->brother)
  {
    child->currentwidth = width;
    child->currentheight = height;

    if (child->firstchild)
      iupClassObjectSetChildrenCurrentSize(child, shrink);
  }
}

static void iFlatTabsSetChildrenPositionMethod(Ihandle* ih, int x, int y)
{
  /* In all systems, each tab is a native window covering the client area.
     Child coordinates are relative to client left-top corner of the tab page. */
  Ihandle* child;
  char* offset = iupAttribGet(ih, "CHILDOFFSET");
  int title_width;

  /* Native container, position is reset */
  x = 0;
  y = 0;

  if (offset) iupStrToIntInt(offset, &x, &y, 'x');

  y += iFlatTabsGetTitleHeight(ih, &title_width, 0);

  if (iupAttribGetBoolean(ih, "SHOWLINES"))
    x += 1;

  for (child = ih->firstchild; child; child = child->brother)
    iupBaseSetPosition(child, x, y);
}

static int iFlatTabsCreateMethod(Ihandle* ih, void **params)
{
  /* add children */
  if (params)
  {
    Ihandle** iparams = (Ihandle**)params;
    while (*iparams)
    {
      IupAppend(ih, *iparams);
      iparams++;
    }
  }

  IupSetCallback(ih, "_IUP_XY2POS_CB", (Icallback)iFlatTabsConvertXYToPos);

  iupAttribSetInt(ih, "_IUPFTABS_HIGHLIGHTED", ITABS_NONE);
  iupAttribSetInt(ih, "_IUPFTABS_CLOSEHIGH", ITABS_NONE);
  iupAttribSetInt(ih, "_IUPFTABS_CLOSEPRESS", ITABS_NONE);
  iupAttribSetInt(ih, "_IUPFTABS_EXTRAPRESS", ITABS_NONE);

  IupSetCallback(ih, "ACTION", (Icallback)iFlatTabsRedraw_CB);
  IupSetCallback(ih, "BUTTON_CB", (Icallback)iFlatTabsButton_CB);
  IupSetCallback(ih, "MOTION_CB", (Icallback)iFlatTabsMotion_CB);
  IupSetCallback(ih, "LEAVEWINDOW_CB", (Icallback)iFlatTabsLeaveWindow_CB);
  IupSetCallback(ih, "RESIZE_CB", (Icallback)iFlatTabsResize_CB);

  return IUP_NOERROR;
}

Iclass* iupFlatTabsNewClass(void)
{
  Iclass* ic = iupClassNew(iupRegisterFindClass("canvas"));

  ic->name = "flattabs";
  ic->format = "g"; /* array of Ihandle */
  ic->nativetype = IUP_TYPECONTROL;
  ic->childtype = IUP_CHILDMANY;
  ic->is_interactive = 1;
  ic->has_attrib_id = 1;

  /* Class functions */
  ic->New = iupFlatTabsNewClass;
  ic->Create = iFlatTabsCreateMethod;

  ic->ChildAdded = iFlatTabsChildAddedMethod;
  ic->ChildRemoved = iFlatTabsChildRemovedMethod;

  ic->ComputeNaturalSize = iFlatTabsComputeNaturalSizeMethod;
  ic->SetChildrenCurrentSize = iFlatTabsSetChildrenCurrentSizeMethod;
  ic->SetChildrenPosition = iFlatTabsSetChildrenPositionMethod;

  /* IupFlatTabs Callbacks */
  iupClassRegisterCallback(ic, "TABCHANGE_CB", "nn");
  iupClassRegisterCallback(ic, "TABCHANGEPOS_CB", "ii");
  iupClassRegisterCallback(ic, "RIGHTCLICK_CB", "i");
  iupClassRegisterCallback(ic, "TABCLOSE_CB", "i");
  iupClassRegisterCallback(ic, "EXTRABUTTON_CB", "ii");

  iupClassRegisterCallback(ic, "FLAT_BUTTON_CB", "iiiis");
  iupClassRegisterCallback(ic, "FLAT_MOTION_CB", "iis");
  iupClassRegisterCallback(ic, "FLAT_LEAVEWINDOW_CB", "");

  /* Base Container */
  iupClassRegisterAttribute(ic, "EXPAND", iupBaseContainerGetExpandAttrib, NULL, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NOT_MAPPED | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "CLIENTOFFSET", iupBaseGetClientOffsetAttrib, NULL, NULL, NULL, IUPAF_NOT_MAPPED | IUPAF_READONLY | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "CLIENTSIZE", iFlatTabsGetClientSizeAttrib, NULL, NULL, NULL, IUPAF_READONLY | IUPAF_NOT_MAPPED | IUPAF_NO_INHERIT);

  /* Native Container */
  iupClassRegisterAttribute(ic, "CHILDOFFSET", NULL, NULL, NULL, NULL, IUPAF_NOT_MAPPED | IUPAF_NO_INHERIT);

  /* replace IupCanvas behavior */
  iupClassRegisterReplaceAttribDef(ic, "BORDER", "NO", NULL);
  iupClassRegisterReplaceAttribFlags(ic, "BORDER", IUPAF_READONLY | IUPAF_NO_INHERIT);
  iupClassRegisterReplaceAttribFlags(ic, "SCROLLBAR", IUPAF_READONLY | IUPAF_NO_INHERIT);

  iupClassRegisterReplaceAttribFunc(ic, "ACTIVE", NULL, iFlatTabsSetActiveAttrib);
  iupClassRegisterAttribute(ic, "TIP", NULL, iFlatTabsSetTipAttrib, NULL, NULL, IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);

  /* IupFlatTabs only */
  iupClassRegisterAttribute(ic, "VALUE", iFlatTabsGetValueAttrib, iFlatTabsSetValueAttrib, NULL, NULL, IUPAF_NOT_MAPPED | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "VALUEPOS", iFlatTabsGetValuePosAttrib, iFlatTabsSetValuePosAttrib, IUPAF_SAMEASSYSTEM, "0", IUPAF_NO_SAVE | IUPAF_NOT_MAPPED | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "VALUE_HANDLE", iFlatTabsGetValueHandleAttrib, iFlatTabsSetValueHandleAttrib, NULL, NULL, IUPAF_NOT_MAPPED | IUPAF_NO_INHERIT | IUPAF_IHANDLE | IUPAF_NO_STRING);
  iupClassRegisterAttribute(ic, "COUNT", iFlatTabsGetCountAttrib, NULL, NULL, NULL, IUPAF_READONLY | IUPAF_NOT_MAPPED | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "FIXEDWIDTH", NULL, iFlatTabsUpdateSetAttrib, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "TABCHANGEONCHECK", NULL, NULL, NULL, NULL, IUPAF_NO_INHERIT);

  /* IupFlatTabs Child only */
  iupClassRegisterAttributeId(ic, "TABTITLE", NULL, (IattribSetIdFunc)iFlatTabsUpdateSetAttrib, IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "TABIMAGE", NULL, (IattribSetIdFunc)iFlatTabsUpdateSetAttrib, IUPAF_IHANDLENAME | IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "TABVISIBLE", NULL, iFlatTabsSetTabVisibleAttrib, IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "TABACTIVE", NULL, (IattribSetIdFunc)iFlatTabsUpdateSetAttrib, IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "TABFORECOLOR", NULL, (IattribSetIdFunc)iFlatTabsUpdateSetAttrib, IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "TABBACKCOLOR", NULL, (IattribSetIdFunc)iFlatTabsUpdateSetAttrib, IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "TABHIGHCOLOR", NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "TABFONT", NULL, (IattribSetIdFunc)iFlatTabsUpdateSetAttrib, IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "TABTIP", NULL, NULL, IUPAF_NO_INHERIT);

  iupClassRegisterAttributeId(ic, "TABFONTSTYLE", iFlatTabsGetTabFontStyleAttrib, iFlatTabsSetTabFontStyleAttrib, IUPAF_NO_SAVE | IUPAF_NOT_MAPPED | IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "TABFONTSIZE", iFlatTabsGetTabFontSizeAttrib, iFlatTabsSetTabFontSizeAttrib, IUPAF_NO_SAVE | IUPAF_NOT_MAPPED | IUPAF_NO_INHERIT);

  /* Visual for current TAB */
  iupClassRegisterAttribute(ic, "BGCOLOR", iFlatTabsGetBgColorAttrib, iFlatTabsUpdateSetAttrib, IUPAF_SAMEASSYSTEM, "255 255 255", IUPAF_DEFAULT);   /* inherited */
  iupClassRegisterAttribute(ic, "FORECOLOR", NULL, iFlatTabsUpdateSetAttrib, IUPAF_SAMEASSYSTEM, "50 150 255", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "HIGHCOLOR", NULL, NULL, NULL, NULL, IUPAF_NO_INHERIT);

  /* Visual for the other TABS */
  iupClassRegisterAttribute(ic, "TABSFORECOLOR", NULL, iFlatTabsUpdateSetAttrib, IUPAF_SAMEASSYSTEM, "DLGFGCOLOR", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "TABSBACKCOLOR", NULL, iFlatTabsUpdateSetAttrib, IUPAF_SAMEASSYSTEM, "DLGBGCOLOR", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "TABSHIGHCOLOR", NULL, NULL, NULL, NULL, IUPAF_NO_INHERIT);

  /* Visual for all TABS */
  iupClassRegisterAttribute(ic, "TABSFONT", NULL, iupdrvSetFontAttrib, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "TABSFONTSTYLE", iFlatTabsGetTabsFontStyleAttrib, iFlatTabsSetTabsFontStyleAttrib, NULL, NULL, IUPAF_NO_SAVE | IUPAF_NOT_MAPPED | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "TABSFONTSIZE", iFlatTabsGetTabsFontSizeAttrib, iFlatTabsSetTabsFontSizeAttrib, NULL, NULL, IUPAF_NO_SAVE | IUPAF_NOT_MAPPED | IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "SHOWLINES", NULL, iFlatTabsUpdateSetAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "TABSLINECOLOR", NULL, iFlatTabsUpdateSetAttrib, IUPAF_SAMEASSYSTEM, "180 180 180", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "TABSIMAGEPOSITION", NULL, iFlatTabsUpdateSetAttrib, IUPAF_SAMEASSYSTEM, "LEFT", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "TABSIMAGESPACING", NULL, iFlatTabsUpdateSetAttrib, IUPAF_SAMEASSYSTEM, "2", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "TABSALIGNMENT", NULL, iFlatTabsUpdateSetAttrib, "ACENTER:ACENTER", NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "TABSPADDING", NULL, iFlatTabsUpdateSetAttrib, IUPAF_SAMEASSYSTEM, "10x10", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "TABSTEXTALIGNMENT", NULL, NULL, IUPAF_SAMEASSYSTEM, "ALEFT", IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "SHOWCLOSE", NULL, NULL, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "CLOSEIMAGE", NULL, iFlatTabsUpdateSetAttrib, IUPAF_SAMEASSYSTEM, "IMGFLATCLOSE", IUPAF_IHANDLENAME | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "CLOSEIMAGEPRESS", NULL, iFlatTabsUpdateSetAttrib, IUPAF_SAMEASSYSTEM, "IMGFLATCLOSEPRESS", IUPAF_IHANDLENAME | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "CLOSEIMAGEHIGHLIGHT", NULL, NULL, NULL, NULL, IUPAF_IHANDLENAME | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "CLOSEIMAGEINACTIVE", NULL, iFlatTabsUpdateSetAttrib, NULL, NULL, IUPAF_IHANDLENAME | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "CLOSEPRESSCOLOR", NULL, NULL, IUPAF_SAMEASSYSTEM, "80 180 245", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "CLOSEHIGHCOLOR", NULL, NULL, IUPAF_SAMEASSYSTEM, "200 220 245", IUPAF_NO_INHERIT);

  /* Extra Buttons */
  iupClassRegisterAttribute(ic, "EXTRABUTTONS", NULL, NULL, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "EXTRATITLE", NULL, (IattribSetIdFunc)iFlatTabsUpdateSetAttrib, IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "EXTRAACTIVE", NULL, (IattribSetIdFunc)iFlatTabsUpdateSetAttrib, IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "EXTRAFORECOLOR", NULL, (IattribSetIdFunc)iFlatTabsUpdateSetAttrib, IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "EXTRAPRESSCOLOR", NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "EXTRAHIGHCOLOR", NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "EXTRAFONT", NULL, (IattribSetIdFunc)iFlatTabsUpdateSetAttrib, IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "EXTRAALIGNMENT", NULL, (IattribSetIdFunc)iFlatTabsUpdateSetAttrib, IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "EXTRAIMAGE", NULL, (IattribSetIdFunc)iFlatTabsUpdateSetAttrib, IUPAF_IHANDLENAME | IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "EXTRAIMAGEPRESS", NULL, (IattribSetIdFunc)iFlatTabsUpdateSetAttrib, IUPAF_IHANDLENAME | IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "EXTRAIMAGEHIGHLIGHT", NULL, NULL, IUPAF_IHANDLENAME | IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "EXTRAIMAGEINACTIVE", NULL, (IattribSetIdFunc)iFlatTabsUpdateSetAttrib, IUPAF_IHANDLENAME | IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "EXTRATIP", NULL, NULL, IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "EXPANDBUTTON", NULL, iFlatTabsSetExpandButtonAttrib, NULL, NULL, IUPAF_NOT_MAPPED | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "EXPANDBUTTONPOS", NULL, NULL, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "EXPANDBUTTONSTATE", NULL, iFlatTabsSetExpandButtonStateAttrib, IUPAF_SAMEASSYSTEM, "Yes", IUPAF_NOT_MAPPED | IUPAF_NO_INHERIT);
  

  /* Default node images */
  if (!IupGetHandle("IMGFLATCLOSE"))
    iFlatTabsInitializeImages();

  return ic;
}

Ihandle* IupFlatTabsv(Ihandle** params)
{
  return IupCreatev("flattabs", (void**)params);
}

Ihandle*  IupFlatTabsV(Ihandle* child, va_list arglist)
{
  return IupCreateV("flattabs", child, arglist);
}

Ihandle* IupFlatTabs(Ihandle* child, ...)
{
  Ihandle *ih;

  va_list arglist;
  va_start(arglist, child);
  ih = IupCreateV("flattabs", child, arglist);
  va_end(arglist);

  return ih;
}
