/** \file
 * \brief Scintilla control: White space
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


#include <Scintilla.h>

#include "iup.h"

#include "iup_object.h"
#include "iup_attrib.h"
#include "iup_str.h"

#include "iupsci.h"


/***** WHITE SPACE *****
SCI_SETVIEWWS(int wsMode)
SCI_GETVIEWWS
SCI_SETWHITESPACEFORE(bool useWhitespaceForeColour, int colour)
SCI_SETWHITESPACEBACK(bool useWhitespaceBackColour, int colour)
SCI_SETWHITESPACESIZE(int size)
SCI_GETWHITESPACESIZE
SCI_SETEXTRAASCENT(int extraAscent)
SCI_GETEXTRAASCENT
SCI_SETEXTRADESCENT(int extraDescent)
SCI_GETEXTRADESCENT
*/

static int iScintillaSetViewWSAttrib(Ihandle *ih, const char *value)
{
  if (iupStrEqualNoCase(value, "INVISIBLE"))
    IupScintillaSendMessage(ih, SCI_SETVIEWWS, SCWS_INVISIBLE, 0);
  if (iupStrEqualNoCase(value, "VISIBLEALWAYS"))
    IupScintillaSendMessage(ih, SCI_SETVIEWWS, SCWS_VISIBLEALWAYS, 0);
  else if (iupStrEqualNoCase(value, "VISIBLEAFTERINDENT"))
    IupScintillaSendMessage(ih, SCI_SETVIEWWS, SCWS_VISIBLEAFTERINDENT, 0);

  return 0;
}

static char* iScintillaGetViewWSAttrib(Ihandle* ih)
{
  if (IupScintillaSendMessage(ih, SCI_GETVIEWWS, 0, 0) == SCWS_INVISIBLE)
    return "INVISIBLE";
  else if (IupScintillaSendMessage(ih, SCI_GETVIEWWS, 0, 0) == SCWS_VISIBLEALWAYS)
    return "VISIBLEALWAYS";
  else if (IupScintillaSendMessage(ih, SCI_GETVIEWWS, 0, 0) == SCWS_VISIBLEAFTERINDENT)
    return "VISIBLEAFTERINDENT";

  return "UNDEFINED";
}

static int iScintillaSetWSFgColorAttrib(Ihandle *ih, const char *value)
{
  if (!value)
  {
    IupScintillaSendMessage(ih, SCI_SETWHITESPACEFORE, 0, 0);
    return 0;
  }
  else
  {
    unsigned char r, g, b;
    if (!iupStrToRGB(value, &r, &g, &b))
      return 0;

    IupScintillaSendMessage(ih, SCI_SETWHITESPACEFORE, 1, iupScintillaEncodeColor(r, g, b));
    return 1;
  }
}

static int iScintillaSetWSBgColorAttrib(Ihandle *ih, const char *value)
{
  if (!value)
  {
    IupScintillaSendMessage(ih, SCI_SETWHITESPACEBACK, 0, 0);
    return 0;
  }
  else
  {
    unsigned char r, g, b;
    if (!iupStrToRGB(value, &r, &g, &b))
      return 0;

    IupScintillaSendMessage(ih, SCI_SETWHITESPACEBACK, 1, iupScintillaEncodeColor(r, g, b));

    return 1;
  }
}

static int iScintillaSetWSSizeAttrib(Ihandle *ih, const char *value)
{
  int size;
  if (!iupStrToInt(value, &size))
    return 0;

  IupScintillaSendMessage(ih, SCI_SETWHITESPACESIZE, size, 0);

  return 0;
}

static char* iScintillaGetWSSizeAttrib(Ihandle* ih)
{
  int size = IupScintillaSendMessage(ih, SCI_GETWHITESPACESIZE, 0, 0);
  return iupStrReturnInt(size);
}

static int iScintillaSetWSExtraAscentAttrib(Ihandle *ih, const char *value)
{
  int asc;
  if (!iupStrToInt(value, &asc))
    return 0;

  IupScintillaSendMessage(ih, SCI_SETEXTRAASCENT, asc, 0);

  return 0;
}

static char* iScintillaGetWSExtraAscentAttrib(Ihandle* ih)
{
  int asc = IupScintillaSendMessage(ih, SCI_GETEXTRAASCENT, 0, 0);
  return iupStrReturnInt(asc);
}

static int iScintillaSetWSExtraDescentAttrib(Ihandle *ih, const char *value)
{
  int desc;
  if (!iupStrToInt(value, &desc))
    return 0;

  IupScintillaSendMessage(ih, SCI_SETEXTRADESCENT, desc, 0);

  return 0;
}

static char* iScintillaGetWSExtraDescentAttrib(Ihandle* ih)
{
  int desc = IupScintillaSendMessage(ih, SCI_GETEXTRADESCENT, 0, 0);
  return iupStrReturnInt(desc);
}

void iupScintillaRegisterWhiteSpace(Iclass* ic)
{
  iupClassRegisterAttribute(ic, "EXTRAASCENT", iScintillaGetWSExtraDescentAttrib, iScintillaSetWSExtraDescentAttrib, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "EXTRADESCENT", iScintillaGetWSExtraAscentAttrib, iScintillaSetWSExtraAscentAttrib, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "WHITESPACEVIEW", iScintillaGetViewWSAttrib, iScintillaSetViewWSAttrib, IUPAF_SAMEASSYSTEM, "INVISIBLE", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "WHITESPACESIZE", iScintillaGetWSSizeAttrib, iScintillaSetWSSizeAttrib, IUPAF_SAMEASSYSTEM, "1", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "WHITESPACEFGCOLOR", NULL, iScintillaSetWSFgColorAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "WHITESPACEBGCOLOR", NULL, iScintillaSetWSBgColorAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
}
