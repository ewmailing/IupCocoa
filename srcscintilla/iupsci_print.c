/** \file
 * \brief Scintilla control: Line wrapping
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


/***** PRINTING ****
SCI_SETPRINTMAGNIFICATION(int magnification)
SCI_GETPRINTMAGNIFICATION
SCI_SETPRINTCOLOURMODE(int mode)
SCI_GETPRINTCOLOURMODE
SCI_SETPRINTWRAPMODE
SCI_GETPRINTWRAPMODE
*/

static char* iScintillaGetPrintMagnificationAttrib(Ihandle* ih)
{
  int magnification = (int)IupScintillaSendMessage(ih, SCI_GETPRINTMAGNIFICATION, 0, 0);
  return iupStrReturnInt(magnification);
}

static int iScintillaSetPrintMagnificationAttrib(Ihandle* ih, const char* value)
{
  int magnification;

  if (iupStrToInt(value, &magnification))
    IupScintillaSendMessage(ih, SCI_SETPRINTMAGNIFICATION, magnification, 0);

  return 0;
}

static char* iScintillaGetPrintWrapAttrib(Ihandle *ih)
{
  int type = (int)IupScintillaSendMessage(ih, SCI_GETPRINTWRAPMODE, 0, 0);

  if(type == SC_WRAP_WORD)
    return "WORD";
  else if(type == SC_WRAP_CHAR)
    return "CHAR";
  else
    return "NONE";
}

static int iScintillaSetPrintWrapAttrib(Ihandle *ih, const char *value)
{
  if (iupStrEqualNoCase(value, "WORD"))
    IupScintillaSendMessage(ih, SCI_SETPRINTWRAPMODE, SC_WRAP_WORD, 0);
  else if (iupStrEqualNoCase(value, "CHAR"))
    IupScintillaSendMessage(ih, SCI_SETPRINTWRAPMODE, SC_WRAP_CHAR, 0);
  else
    IupScintillaSendMessage(ih, SCI_SETPRINTWRAPMODE, SC_WRAP_NONE, 0);

  return 0;
}

static char* iScintillaGetPrintColorAttrib(Ihandle *ih)
{
  int type = (int)IupScintillaSendMessage(ih, SCI_GETPRINTCOLOURMODE, 0, 0);

  if (type == SC_PRINT_COLOURONWHITEDEFAULTBG)
    return "COLORONWHITEDEFAULTBG";
  else if (type == SC_PRINT_COLOURONWHITE)
    return "COLORONWHITE";
  else if (type == SC_PRINT_BLACKONWHITE)
    return "BLACKONWHITE";
  else if (type == SC_PRINT_INVERTLIGHT)
    return "INVERTLIGHT";
  else
    return "NORMAL";
}

static int iScintillaSetPrintColorAttrib(Ihandle *ih, const char *value)
{
  if (iupStrEqualNoCase(value, "COLORONWHITEDEFAULTBG"))
    IupScintillaSendMessage(ih, SCI_SETPRINTCOLOURMODE, SC_PRINT_COLOURONWHITEDEFAULTBG, 0);
  else if (iupStrEqualNoCase(value, "COLORONWHITE"))
    IupScintillaSendMessage(ih, SCI_SETPRINTCOLOURMODE, SC_PRINT_COLOURONWHITE, 0);
  else if (iupStrEqualNoCase(value, "BLACKONWHITE"))
    IupScintillaSendMessage(ih, SCI_SETPRINTCOLOURMODE, SC_PRINT_BLACKONWHITE, 0);
  else if (iupStrEqualNoCase(value, "INVERTLIGHT"))
    IupScintillaSendMessage(ih, SCI_SETPRINTCOLOURMODE, SC_PRINT_INVERTLIGHT, 0);
  else
    IupScintillaSendMessage(ih, SCI_SETPRINTCOLOURMODE, SC_PRINT_NORMAL, 0);

  return 0;
}

int iupSciGetPrintMarginUnits(Ihandle* ih)
{
  char* units = iupAttribGetStr(ih, "PRINTMARGINUNITS");
  if (iupStrEqualNoCase(units, "INCH"))
    return PRINTUNITS_INCH;
  else if (iupStrEqualNoCase(units, "CM"))
    return PRINTUNITS_CM;
  else
    return PRINTUNITS_PIXELS;
}

int iupSciGetPrintMargin(Ihandle* ih, const char* margin_attrib, int units, double dpi)
{
  double margin = IupGetDouble(ih, margin_attrib);
  if (units == PRINTUNITS_PIXELS)
    return (int)margin;
  else if (units == PRINTUNITS_INCH)
    return (int)(margin * dpi);
  else
    return (int)((margin * dpi) / 2.54);
}

void iupScintillaRegisterPrint(Iclass* ic)
{
  iupClassRegisterAttribute(ic, "PRINT", NULL, iupdrvScintillaPrintAttrib, NULL, NULL, IUPAF_WRITEONLY | IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "PRINTDIALOG", NULL, NULL, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "PRINTMARGINLEFT", NULL, NULL, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "PRINTMARGINTOP", NULL, NULL, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "PRINTMARGINRIGHT", NULL, NULL, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "PRINTMARGINBOTTOM", NULL, NULL, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "PRINTMARGINUNITS", NULL, NULL, IUPAF_SAMEASSYSTEM, "INCH", IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "PRINTWORDWRAP", iScintillaGetPrintWrapAttrib, iScintillaSetPrintWrapAttrib, IUPAF_SAMEASSYSTEM, "WORD", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "PRINTCOLOR", iScintillaGetPrintColorAttrib, iScintillaSetPrintColorAttrib, IUPAF_SAMEASSYSTEM, "NORMAL", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "PRINTMAGNIFICATION", iScintillaGetPrintMagnificationAttrib, iScintillaSetPrintMagnificationAttrib, NULL, NULL, IUPAF_NO_INHERIT);
}
