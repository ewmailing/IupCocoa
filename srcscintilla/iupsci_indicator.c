/** \file
 * \brief Scintilla control: Cursor and Zooming
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

/***** INDICATORS ****
SCI_INDICSETSTYLE(int indicator, int indicatorStyle)
SCI_INDICGETSTYLE(int indicator) ? int
SCI_INDICSETFORE(int indicator, colour fore)
SCI_INDICGETFORE(int indicator) ? colour
SCI_INDICSETALPHA(int indicator, alpha alpha)
SCI_INDICGETALPHA(int indicator) ? int
SCI_INDICSETOUTLINEALPHA(int indicator, alpha alpha)
SCI_INDICGETOUTLINEALPHA(int indicator) ? int
--SCI_INDICSETUNDER(int indicator, bool under)
--SCI_INDICGETUNDER(int indicator) ? bool
--SCI_INDICSETHOVERSTYLE(int indicator, int indicatorStyle)
--SCI_INDICGETHOVERSTYLE(int indicator) ? int
--SCI_INDICSETHOVERFORE(int indicator, colour fore)
--SCI_INDICGETHOVERFORE(int indicator) ? colour
--SCI_INDICSETFLAGS(int indicator, int flags)
--SCI_INDICGETFLAGS(int indicator) ? int

SCI_SETINDICATORCURRENT(int indicator)
SCI_GETINDICATORCURRENT ? int
SCI_SETINDICATORVALUE(int value)
SCI_GETINDICATORVALUE ? int
SCI_INDICATORFILLRANGE(int start, int lengthFill)
SCI_INDICATORCLEARRANGE(int start, int lengthClear)
--SCI_INDICATORALLONFOR(int pos) ? int
--SCI_INDICATORVALUEAT(int indicator, int pos) ? int
--SCI_INDICATORSTART(int indicator, int pos) ? int
--SCI_INDICATOREND(int indicator, int pos) ? int
--SCI_FINDINDICATORSHOW(int start, int end)
--SCI_FINDINDICATORFLASH(int start, int end)
--SCI_FINDINDICATORHIDE*/

static char* iScintillaGetCurrentIndicatorAttrib(Ihandle* ih)
{
  int currentIndicator = (int)IupScintillaSendMessage(ih, SCI_GETINDICATORCURRENT, 0, 0);
  return iupStrReturnInt(currentIndicator);
}

static int iScintillaSetCurrentIndicatorAttrib(Ihandle* ih, const char* value)
{
  int indicator;
  if (iupStrToInt(value, &indicator))
    IupScintillaSendMessage(ih, SCI_SETINDICATORCURRENT, indicator, 0);
  return 0;
}

static char* iScintillaGetIndicatorValueAttrib(Ihandle* ih)
{
  int value = (int)IupScintillaSendMessage(ih, SCI_GETINDICATORVALUE, 0, 0);
  return iupStrReturnInt(value);
}

static int iScintillaSetIndicatorValueAttrib(Ihandle* ih, const char* value)
{
  int ivalue;
  if (iupStrToInt(value, &ivalue))
    IupScintillaSendMessage(ih, SCI_SETINDICATORVALUE, ivalue, 0);
  return 0;
}

static int iScintillaSetIndicatorClearRangeAttrib(Ihandle *ih, const char *value)
{
  int start, lengthClear;

  if (!value)
    return 0;

  iupStrToIntInt(value, &start, &lengthClear, ':');
  if (start < 0 || lengthClear <= 0)
    return 0;

  IupScintillaSendMessage(ih, SCI_INDICATORCLEARRANGE, start, lengthClear);

  return 0;
}

static int iScintillaSetIndicatorFillRangeAttrib(Ihandle *ih, const char *value)
{
  int start, lengthFill;

  if (!value)
    return 0;

  iupStrToIntInt(value, &start, &lengthFill, ':');
  if (start < 0 || lengthFill <= 0)
    return 0;

  IupScintillaSendMessage(ih, SCI_INDICATORFILLRANGE, start, lengthFill);

  return 0;
}

static char* iScintillaGetIndicatorStyleAttrib(Ihandle* ih, int indicator)
{
  int type = (int)IupScintillaSendMessage(ih, SCI_INDICGETSTYLE, indicator, 0);

  if (type == INDIC_PLAIN)
    return "PLAIN";
  if (type == INDIC_SQUIGGLE)
    return "SQUIGGLE";
  if (type == INDIC_TT)
    return "TT";
  if (type == INDIC_DIAGONAL)
    return "DIAGONAL";
  if (type == INDIC_STRIKE)
    return "STRIKE";
  if (type == INDIC_HIDDEN)
    return "HIDDEN";
  if (type == INDIC_BOX)
    return "BOX";
  if (type == INDIC_ROUNDBOX)
    return "ROUNDBOX";
  if (type == INDIC_STRAIGHTBOX)
    return "STRAIGHTBOX";
  if (type == INDIC_DASH)
    return "DASH";
  if (type == INDIC_DOTS)
    return "DOTS";
  if (type == INDIC_SQUIGGLELOW)
    return "SQUIGGLELOW";
  if (type == INDIC_DOTBOX)
    return "DOTBOX";
  if (type == INDIC_SQUIGGLEPIXMAP)
    return "SQUIGGLEPIXMAP";
  if (type == INDIC_COMPOSITIONTHICK)
    return "COMPOSITIONTHICK";
#ifdef INDIC_COMPOSITIONTHIN  
  if (type == INDIC_COMPOSITIONTHIN)
    return "COMPOSITIONTHIN";
  if (type == INDIC_TEXTFORE)
    return "TEXTFORE";
  if (type == INDIC_FULLBOX)
    return "FULLBOX";
#endif

  return "PLAIN";
}

static int iScintillaSetIndicatorStyleAttrib(Ihandle* ih, int indicator, const char *value)
{
  if (iupStrEqualNoCase(value, "PLAIN"))
    IupScintillaSendMessage(ih, SCI_INDICSETSTYLE, indicator, INDIC_PLAIN);
  else if (iupStrEqualNoCase(value, "SQUIGGLE"))
    IupScintillaSendMessage(ih, SCI_INDICSETSTYLE, indicator, INDIC_SQUIGGLE);
  else if (iupStrEqualNoCase(value, "TT"))
    IupScintillaSendMessage(ih, SCI_INDICSETSTYLE, indicator, INDIC_TT);
  else if (iupStrEqualNoCase(value, "DIAGONAL"))
    IupScintillaSendMessage(ih, SCI_INDICSETSTYLE, indicator, INDIC_DIAGONAL);
  else if (iupStrEqualNoCase(value, "STRIKE"))
    IupScintillaSendMessage(ih, SCI_INDICSETSTYLE, indicator, INDIC_STRIKE);
  else if (iupStrEqualNoCase(value, "HIDDEN"))
    IupScintillaSendMessage(ih, SCI_INDICSETSTYLE, indicator, INDIC_HIDDEN);
  else if (iupStrEqualNoCase(value, "BOX"))
    IupScintillaSendMessage(ih, SCI_INDICSETSTYLE, indicator, INDIC_BOX);
  else if (iupStrEqualNoCase(value, "ROUNDBOX"))
    IupScintillaSendMessage(ih, SCI_INDICSETSTYLE, indicator, INDIC_ROUNDBOX);
  else if (iupStrEqualNoCase(value, "STRAIGHTBOX"))
    IupScintillaSendMessage(ih, SCI_INDICSETSTYLE, indicator, INDIC_STRAIGHTBOX);
  else if (iupStrEqualNoCase(value, "DASH"))
    IupScintillaSendMessage(ih, SCI_INDICSETSTYLE, indicator, INDIC_DASH);
  else if (iupStrEqualNoCase(value, "DOTS"))
    IupScintillaSendMessage(ih, SCI_INDICSETSTYLE, indicator, INDIC_DOTS);
  else if (iupStrEqualNoCase(value, "SQUIGGLELOW"))
    IupScintillaSendMessage(ih, SCI_INDICSETSTYLE, indicator, INDIC_SQUIGGLELOW);
  else if (iupStrEqualNoCase(value, "DOTBOX"))
    IupScintillaSendMessage(ih, SCI_INDICSETSTYLE, indicator, INDIC_DOTBOX);
  else if (iupStrEqualNoCase(value, "SQUIGGLEPIXMAP"))
    IupScintillaSendMessage(ih, SCI_INDICSETSTYLE, indicator, INDIC_SQUIGGLEPIXMAP);
  else if (iupStrEqualNoCase(value, "COMPOSITIONTHICK"))
    IupScintillaSendMessage(ih, SCI_INDICSETSTYLE, indicator, INDIC_COMPOSITIONTHICK);
#ifdef INDIC_COMPOSITIONTHIN  
  else if (iupStrEqualNoCase(value, "COMPOSITIONTHIN"))
    IupScintillaSendMessage(ih, SCI_INDICSETSTYLE, indicator, INDIC_COMPOSITIONTHIN);
  else if (iupStrEqualNoCase(value, "TEXTFORE"))
    IupScintillaSendMessage(ih, SCI_INDICSETSTYLE, indicator, INDIC_TEXTFORE);
  else if (iupStrEqualNoCase(value, "FULLBOX"))
    IupScintillaSendMessage(ih, SCI_INDICSETSTYLE, indicator, INDIC_FULLBOX);
#endif

  return 0;
}

static char* iScintillaGetIndicatorFgColorAttrib(Ihandle* ih, int indicator)
{
  long color;
  unsigned char r, g, b;

  if (indicator == IUP_INVALID_ID)
    indicator = 0;

  color = (int)IupScintillaSendMessage(ih, SCI_INDICGETFORE, indicator, 0);
  iupScintillaDecodeColor(color, &r, &g, &b);
  return iupStrReturnRGB(r, g, b);
}

static int iScintillaSetIndicatorFgColorAttrib(Ihandle* ih, int indicator, const char* value)
{
  unsigned char r, g, b;

  if (!iupStrToRGB(value, &r, &g, &b))
    return 0;

  if (indicator == IUP_INVALID_ID)
    indicator = 0;

  IupScintillaSendMessage(ih, SCI_INDICSETFORE, indicator, iupScintillaEncodeColor(r, g, b));

  return 0;
}

static int iScintillaSetIndicatorOutlineAlphaAttrib(Ihandle* ih, int indicator, const char* value)
{
  int alpha;
  if (iupStrToInt(value, &alpha))
    IupScintillaSendMessage(ih, SCI_INDICGETOUTLINEALPHA, indicator, alpha);
  return 0;
}

static char* iScintillaGetIndicatorOutlineAlphaAttrib(Ihandle* ih, int indicator)
{
  int alpha = (int)IupScintillaSendMessage(ih, SCI_INDICSETOUTLINEALPHA, indicator, 0);
  return iupStrReturnInt(alpha);
}

static int iScintillaSetIndicatorAlphaAttrib(Ihandle* ih, int indicator, const char* value)
{
  int alpha;
  if (iupStrToInt(value, &alpha))
    IupScintillaSendMessage(ih, SCI_INDICSETALPHA, indicator, alpha);
  return 0;
}

static char* iScintillaGetIndicatorAlphaAttrib(Ihandle* ih, int indicator)
{
  int alpha = (int)IupScintillaSendMessage(ih, SCI_INDICGETALPHA, indicator, 0);
  return iupStrReturnInt(alpha);
}

void iupScintillaRegisterIndicators(Iclass* ic)
{
  iupClassRegisterAttribute(ic, "INDICATORCURRENT", iScintillaGetCurrentIndicatorAttrib, iScintillaSetCurrentIndicatorAttrib, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "INDICATORVALUE", iScintillaGetIndicatorValueAttrib, iScintillaSetIndicatorValueAttrib, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "INDICATORCLEARRANGE", NULL, iScintillaSetIndicatorClearRangeAttrib, NULL, NULL, IUPAF_WRITEONLY | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "INDICATORFILLRANGE", NULL, iScintillaSetIndicatorFillRangeAttrib, NULL, NULL, IUPAF_WRITEONLY | IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "INDICATORSTYLE", iScintillaGetIndicatorStyleAttrib, iScintillaSetIndicatorStyleAttrib, IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "INDICATORFGCOLOR", iScintillaGetIndicatorFgColorAttrib, iScintillaSetIndicatorFgColorAttrib, IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "INDICATOROUTLINEALPHA", iScintillaGetIndicatorOutlineAlphaAttrib, iScintillaSetIndicatorOutlineAlphaAttrib, IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "INDICATORALPHA", iScintillaGetIndicatorAlphaAttrib, iScintillaSetIndicatorAlphaAttrib, IUPAF_NO_INHERIT);
}
