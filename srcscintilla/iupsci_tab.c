/** \file
 * \brief Scintilla control: Tabs and Indentation Guides
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

/***** TABS AND INDENTATION GUIDES ****
SCI_SETTABWIDTH(int widthInChars)
SCI_GETTABWIDTH
SCI_SETUSETABS(bool useTabs)
SCI_GETUSETABS
--SCI_SETINDENT(int widthInChars)
--SCI_GETINDENT
--SCI_SETTABINDENTS(bool tabIndents)
--SCI_GETTABINDENTS
--SCI_SETBACKSPACEUNINDENTS(bool bsUnIndents)
--SCI_GETBACKSPACEUNINDENTS
--SCI_SETLINEINDENTATION(int line, int indentation)
--SCI_GETLINEINDENTATION(int line)
--SCI_GETLINEINDENTPOSITION(int line)
SCI_SETINDENTATIONGUIDES(int indentView)
SCI_GETINDENTATIONGUIDES
SCI_SETHIGHLIGHTGUIDE(int column)
SCI_GETHIGHLIGHTGUIDE
*/

static char* iScintillaGetTabSizeAttrib(Ihandle *ih)
{
  int widthInChars;
  widthInChars = (int)IupScintillaSendMessage(ih, SCI_GETTABWIDTH, 0, 0);
  return iupStrReturnInt(widthInChars);
}

static int iScintillaSetTabSizeAttrib(Ihandle *ih, const char *value)
{
  int widthInChars;
  iupStrToInt(value, &widthInChars);

  if(widthInChars < 2)
    widthInChars = 2;

  IupScintillaSendMessage(ih, SCI_SETTABWIDTH, widthInChars, 0);

  return 0;
}

static char* iScintillaGetHighlightGuideAttrib(Ihandle *ih)
{
  int col = (int)IupScintillaSendMessage(ih, SCI_GETHIGHLIGHTGUIDE, 0, 0);
  return iupStrReturnInt(col);
}

static int iScintillaSetHighlightGuideAttrib(Ihandle *ih, const char *value)
{
  int col;
  if (!iupStrToInt(value, &col))
    return 0;

  IupScintillaSendMessage(ih, SCI_SETHIGHLIGHTGUIDE, col, 0);

  return 0;
}

static char* iScintillaGetIndentationGuidesAttrib(Ihandle *ih)
{
  int indentView = (int)IupScintillaSendMessage(ih, SCI_GETINDENTATIONGUIDES, 0, 0);
  char* str[] = {"NONE", "REAL", "LOOKFORWARD", "LOOKBOTH"};
  return str[indentView];
}

static int iScintillaSetIndentationGuidesAttrib(Ihandle *ih, const char *value)
{
  int indentView;

  if (iupStrEqualNoCase(value, "REAL"))
    indentView = SC_IV_REAL;
  else if (iupStrEqualNoCase(value, "LOOKFORWARD"))
    indentView = SC_IV_LOOKFORWARD;
  else if (iupStrEqualNoCase(value, "LOOKBOTH"))
    indentView = SC_IV_LOOKBOTH;
  else /* NONE */
    indentView = SC_IV_NONE;

  IupScintillaSendMessage(ih, SCI_SETINDENTATIONGUIDES, indentView, 0);

  return 0;
}

static int iScintillaSetUseTabsAttrib(Ihandle* ih, const char* value)
{
  IupScintillaSendMessage(ih, SCI_SETUSETABS, iupStrBoolean(value), 0);
  return 0;
}

static char* iScintillaGetUseTabsAttrib(Ihandle* ih)
{
  return iupStrReturnBoolean((int)IupScintillaSendMessage(ih, SCI_GETUSETABS, 0, 0)); 
}

void iupScintillaRegisterTab(Iclass* ic)
{
  iupClassRegisterAttribute(ic, "TABSIZE", iScintillaGetTabSizeAttrib, iScintillaSetTabSizeAttrib, IUPAF_SAMEASSYSTEM, "8", IUPAF_DEFAULT);
  iupClassRegisterAttribute(ic, "INDENTATIONGUIDES", iScintillaGetIndentationGuidesAttrib, iScintillaSetIndentationGuidesAttrib, IUPAF_SAMEASSYSTEM, "NONE", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "HIGHLIGHTGUIDE", iScintillaGetHighlightGuideAttrib, iScintillaSetHighlightGuideAttrib, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "USETABS", iScintillaGetUseTabsAttrib, iScintillaSetUseTabsAttrib, IUPAF_SAMEASSYSTEM, "Yes", IUPAF_NO_INHERIT);
}
