/** \file
 * \brief Scintilla control: Brace highlighting
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


/***** BRACE HIGHLIGHTING ****
SCI_BRACEHIGHLIGHT(int pos1, int pos2)
SCI_BRACEBADLIGHT(int pos1)
SCI_BRACEHIGHLIGHTINDICATOR(bool useBraceHighlightIndicator, int indicatorNumber)
SCI_BRACEBADLIGHTINDICATOR(bool useBraceBadLightIndicator, int indicatorNumber)
SCI_BRACEMATCH(int position, int maxReStyle)
*/

static int iScintillaSetBraceHighlightAttrib(Ihandle* ih, const char* value)
{
  int pos1, pos2;

  if (iupStrToIntInt(value, &pos1, &pos2, ':') != 2) 
    return 0;

  IupScintillaSendMessage(ih, SCI_BRACEHIGHLIGHT, pos1, pos2);

  return 0;
}

static int iScintillaSetBraceBadlightAttrib(Ihandle* ih, const char* value)
{
  int pos1;

  if (!iupStrToInt(value, &pos1))
    return 0;

  IupScintillaSendMessage(ih, SCI_BRACEBADLIGHT, pos1, 0);

  return 0;
}

/*
static int iScintillaSetBraceHighlightIndicatorAttrib(Ihandle* ih, const char* value)
{
  int indicatorNumber;

  if (!iupStrToInt(value, &indicatorNumber))
    return 0;

  IupScintillaSendMessage(ih, SCI_BRACEHIGHLIGHTINDICATOR, ih->data->useBraceHLIndicator, indicatorNumber);

  return 0;
}

static int iScintillaSetBraceBadlightIndicatorAttrib(Ihandle* ih, const char* value)
{
  int indicatorNumber;

  if (!iupStrToInt(value, &indicatorNumber))
    return 0;

  IupScintillaSendMessage(ih, SCI_BRACEBADLIGHTINDICATOR, ih->data->useBraceBLIndicator, indicatorNumber);

  return 0;
}

static int iScintillaSetUseBraceHLIndicatorAttrib(Ihandle *ih, const char *value)
{
  if (iupStrBoolean(value))
    ih->data->useBraceHLIndicator = 1;
  else
    ih->data->useBraceHLIndicator = 0;
  return 0;
}

static char* iScintillaGetUseBraceHLIndicatorAttrib(Ihandle* ih)
{
  return iupStrReturnBoolean (ih->data->useBraceHLIndicator); 
}

static int iScintillaSetUseBraceBLIndicatorAttrib(Ihandle *ih, const char *value)
{
  if (iupStrBoolean(value))
    ih->data->useBraceBLIndicator = 1;
  else
    ih->data->useBraceBLIndicator = 0;
  return 0;
}

static char* iScintillaGetUseBraceBLIndicatorAttrib(Ihandle* ih)
{
  return iupStrReturnBoolean (ih->data->useBraceBLIndicator); 
}
*/

static char* iScintillaGetBraceMatchAttribId(Ihandle* ih, int pos)
{
  if (pos < 0)
    return "-1";

  return iupStrReturnInt(IupScintillaSendMessage(ih, SCI_BRACEMATCH, pos, 0));
}

void iupScintillaRegisterBraceLight(Iclass* ic)
{
  iupClassRegisterAttribute(ic, "BRACEHIGHLIGHT", NULL, iScintillaSetBraceHighlightAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "BRACEBADLIGHT",  NULL, iScintillaSetBraceBadlightAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  /* unused until we support Indicators */
/*  iupClassRegisterAttribute(ic, "BRACEHLINDICATOR", NULL, iScintillaSetBraceHighlightIndicatorAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);  */
/*  iupClassRegisterAttribute(ic, "BRACEBLINDICATOR", NULL, iScintillaSetBraceBadlightIndicatorAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);   */
/*  iupClassRegisterAttribute(ic, "USEBRACEHLINDICATOR", iScintillaGetUseBraceHLIndicatorAttrib, iScintillaSetUseBraceHLIndicatorAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NO_INHERIT);  */
/*  iupClassRegisterAttribute(ic, "USEBRACEBLINDICATOR", iScintillaGetUseBraceBLIndicatorAttrib, iScintillaSetUseBraceBLIndicatorAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NO_INHERIT);  */
  iupClassRegisterAttributeId(ic, "BRACEMATCH", iScintillaGetBraceMatchAttribId, NULL, IUPAF_READONLY|IUPAF_NO_INHERIT);
}
