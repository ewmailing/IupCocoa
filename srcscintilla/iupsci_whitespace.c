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
  else if (iupStrEqualNoCase(value, "VISIBLEALWAYS"))
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
  int size = (int)IupScintillaSendMessage(ih, SCI_GETWHITESPACESIZE, 0, 0);
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
  int asc = (int)IupScintillaSendMessage(ih, SCI_GETEXTRAASCENT, 0, 0);
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
  int desc = (int)IupScintillaSendMessage(ih, SCI_GETEXTRADESCENT, 0, 0);
  return iupStrReturnInt(desc);
}

static int iScintillaSetEolVisibleAttrib(Ihandle *ih, const char *value)
{
  IupScintillaSendMessage(ih, SCI_SETVIEWEOL, iupStrBoolean(value), 0);
  return 0;
}

static char* iScintillaGetEolVisibleAttrib(Ihandle* ih)
{
  int visible = (int)IupScintillaSendMessage(ih, SCI_GETVIEWEOL, 0, 0);
  return iupStrReturnBoolean(visible);
}

static int iScintillaSetEolModeAttrib(Ihandle *ih, const char *value)
{
  if (iupStrEqualNoCase(value, "CR"))
    IupScintillaSendMessage(ih, SCI_SETEOLMODE, SC_EOL_CR, 0);
  else if (iupStrEqualNoCase(value, "CRLF"))
    IupScintillaSendMessage(ih, SCI_SETEOLMODE, SC_EOL_CRLF, 0);
  else
    IupScintillaSendMessage(ih, SCI_SETEOLMODE, SC_EOL_LF, 0);
  return 0;
}

static char* iScintillaGetEolModeAttrib(Ihandle* ih)
{
  int eolmode = (int)IupScintillaSendMessage(ih, SCI_GETEOLMODE, 0, 0);
  if (eolmode == SC_EOL_CR)
    return "CR";
  else if (eolmode == SC_EOL_CRLF)
    return "CRLF";
  else
    return "LF";
}

static char* iScintillaGetEolAttrib(Ihandle* ih)
{
  int eolmode = (int)IupScintillaSendMessage(ih, SCI_GETEOLMODE, 0, 0);
  if (eolmode == SC_EOL_CR)
    return "\r";
  else if (eolmode == SC_EOL_CRLF)
    return "\r\n";
  else
    return "\n";
}

static int iScintillaSetFixEolAttrib(Ihandle *ih, const char *value)
{
  int eolmode = SC_EOL_LF;
  if (iupStrEqualNoCase(value, "CR"))
    eolmode = SC_EOL_CR;
  else if (iupStrEqualNoCase(value, "CRLF"))
    eolmode = SC_EOL_CRLF;
  IupScintillaSendMessage(ih, SCI_CONVERTEOLS, eolmode, 0);
  return 0;
}

void iupScintillaRegisterWhiteSpace(Iclass* ic)
{
  iupClassRegisterAttribute(ic, "EXTRAASCENT", iScintillaGetWSExtraDescentAttrib, iScintillaSetWSExtraDescentAttrib, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "EXTRADESCENT", iScintillaGetWSExtraAscentAttrib, iScintillaSetWSExtraAscentAttrib, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "WHITESPACEVIEW", iScintillaGetViewWSAttrib, iScintillaSetViewWSAttrib, IUPAF_SAMEASSYSTEM, "INVISIBLE", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "WHITESPACESIZE", iScintillaGetWSSizeAttrib, iScintillaSetWSSizeAttrib, IUPAF_SAMEASSYSTEM, "3", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "WHITESPACEFGCOLOR", NULL, iScintillaSetWSFgColorAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "WHITESPACEBGCOLOR", NULL, iScintillaSetWSBgColorAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "EOLVISIBLE", iScintillaGetEolVisibleAttrib, iScintillaSetEolVisibleAttrib, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "FIXEOL", NULL, iScintillaSetFixEolAttrib, NULL, NULL, IUPAF_WRITEONLY | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "EOLMODE", iScintillaGetEolModeAttrib, iScintillaSetEolModeAttrib, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "EOL", iScintillaGetEolAttrib, NULL, NULL, NULL, IUPAF_READONLY|IUPAF_NO_INHERIT);
}
