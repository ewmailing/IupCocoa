/** \file
 * \brief Scintilla control: Scrolling and automatic scrolling
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


#include <Scintilla.h>

#include "iup.h"
#include "iup_scintilla.h"

#include "iup_object.h"
#include "iup_attrib.h"
#include "iup_str.h"

#include "iupsci.h"

/***** SCROLLING AND AUTOMATIC SCROLLING 
SCI_LINESCROLL(int column, int line)
SCI_SCROLLCARET
--SCI_SETXCARETPOLICY(int caretPolicy, int caretSlop)
--SCI_SETYCARETPOLICY(int caretPolicy, int caretSlop)
--SCI_SETVISIBLEPOLICY(int caretPolicy, int caretSlop)
SCI_SETHSCROLLBAR(bool visible)
SCI_GETHSCROLLBAR
SCI_SETVSCROLLBAR(bool visible)
SCI_GETVSCROLLBAR
--SCI_GETXOFFSET
--SCI_SETXOFFSET(int xOffset)
--SCI_SETSCROLLWIDTH(int pixelWidth)
--SCI_GETSCROLLWIDTH
--SCI_SETSCROLLWIDTHTRACKING(bool tracking)
--SCI_GETSCROLLWIDTHTRACKING
--SCI_SETENDATLASTLINE(bool endAtLastLine)
--SCI_GETENDATLASTLINE
*/

static int iScintillaSetScrollByAttrib(Ihandle *ih, const char *value)
{
  int lin, col;
  iupStrToIntInt(value, &lin, &col, ',');

  IupScintillaSendMessage(ih, SCI_LINESCROLL, col, lin);

  return 0;
}

static int iScintillaSetScrollCaretAttrib(Ihandle *ih, const char *value)
{
  (void)value;
  IupScintillaSendMessage(ih, SCI_SCROLLCARET, 0, 0);
  return 0;
}

static char* iScintillaGetScrollWidthAttrib(Ihandle* ih)
{
  int pixelWidth = IupScintillaSendMessage(ih, SCI_GETSCROLLWIDTH, 0, 0);
  return iupStrReturnInt(pixelWidth);
}

static int iScintillaSetScrollWidthAttrib(Ihandle* ih, const char* value)
{
  int pixelWidth;

  iupStrToInt(value, &pixelWidth);
  
  if(pixelWidth < 1)
    pixelWidth = 2000;

  IupScintillaSendMessage(ih, SCI_SETSCROLLWIDTH, pixelWidth, 0);

  return 0;
}

static char* iScintillaGetScrollbarAttrib(Ihandle* ih)
{
  if (ih->data->sb == (IUP_SB_HORIZ | IUP_SB_VERT))
    return "YES";
  if (ih->data->sb & IUP_SB_HORIZ)
    return "HORIZONTAL";
  if (ih->data->sb & IUP_SB_VERT)
    return "VERTICAL";
  
  return "NO";
}

static int iScintillaSetScrollbarAttrib(Ihandle* ih, const char* value)
{
  if (!value)
    value = "YES";    /* default is YES */

  if (iupStrEqualNoCase(value, "YES"))
    ih->data->sb = IUP_SB_HORIZ | IUP_SB_VERT;
  else if (iupStrEqualNoCase(value, "HORIZONTAL"))
    ih->data->sb = IUP_SB_HORIZ;
  else if (iupStrEqualNoCase(value, "VERTICAL"))
    ih->data->sb = IUP_SB_VERT;
  else
    ih->data->sb = IUP_SB_NONE;

  return 0;
}

void iupScintillaRegisterScrolling(Iclass* ic)
{
  iupClassRegisterAttribute(ic, "SCROLLBAR", iScintillaGetScrollbarAttrib, iScintillaSetScrollbarAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "SCROLLBY", NULL, iScintillaSetScrollByAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SCROLLTOCARET", NULL, iScintillaSetScrollCaretAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SCROLLWIDTH", iScintillaGetScrollWidthAttrib, iScintillaSetScrollWidthAttrib, IUPAF_SAMEASSYSTEM, "2000", IUPAF_NO_INHERIT);
}
