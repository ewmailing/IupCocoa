/** \file
 * \brief Scintilla control: Folding
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

/***** FOLDING *****
--SCI_VISIBLEFROMDOCLINE(int docLine)
--SCI_DOCLINEFROMVISIBLE(int displayLine)
SCI_SHOWLINES(int lineStart, int lineEnd)
SCI_HIDELINES(int lineStart, int lineEnd)
--SCI_GETLINEVISIBLE(int line)
--SCI_GETALLLINESVISIBLE
SCI_SETFOLDLEVEL(int line, int level)
SCI_GETFOLDLEVEL(int line)
--SCI_SETAUTOMATICFOLD(int automaticFold)
--SCI_GETAUTOMATICFOLD
SCI_SETFOLDFLAGS(int flags)
--SCI_GETLASTCHILD(int line, int level)
SCI_GETFOLDPARENT(int line)
SCI_SETFOLDEXPANDED(int line, bool expanded)
SCI_GETFOLDEXPANDED(int line)
--SCI_CONTRACTEDFOLDNEXT(int lineStart)
SCI_TOGGLEFOLD(int line)
SCI_FOLDLINE(int line, int action)
SCI_FOLDCHILDREN(int line, int action)
SCI_FOLDALL(int action)
--SCI_EXPANDCHILDREN(int line, int level)
SCI_ENSUREVISIBLE(int line)
--SCI_ENSUREVISIBLEENFORCEPOLICY(int line)
*/

static int iScintillaSetFoldFlagsAttrib(Ihandle* ih, const char* value)
{
  if (iupStrEqualNoCase(value, "LEVELNUMBERS")) /* for debug only */
    IupScintillaSendMessage(ih, SCI_SETFOLDFLAGS, SC_FOLDFLAG_LEVELNUMBERS, 0);
  else if (iupStrEqualNoCase(value, "LINEBEFORE_EXPANDED"))
    IupScintillaSendMessage(ih, SCI_SETFOLDFLAGS, SC_FOLDFLAG_LINEBEFORE_EXPANDED, 0);
  else if (iupStrEqualNoCase(value, "LINEBEFORE_CONTRACTED"))
    IupScintillaSendMessage(ih, SCI_SETFOLDFLAGS, SC_FOLDFLAG_LINEBEFORE_CONTRACTED, 0);
  else if (iupStrEqualNoCase(value, "LINEAFTER_EXPANDED"))
    IupScintillaSendMessage(ih, SCI_SETFOLDFLAGS, SC_FOLDFLAG_LINEAFTER_EXPANDED, 0);
  else  /* LINEAFTER_CONTRACTED */
    IupScintillaSendMessage(ih, SCI_SETFOLDFLAGS, SC_FOLDFLAG_LINEAFTER_CONTRACTED, 0);

  return 0;
}

static char* iScintillaGetFoldLevelAttrib(Ihandle* ih, int line)
{
  int level = (int)IupScintillaSendMessage(ih, SCI_GETFOLDLEVEL, line, 0);
  level = level & SC_FOLDLEVELNUMBERMASK;
  level = level - SC_FOLDLEVELBASE;
  return iupStrReturnInt(level);
}

static int iScintillaSetFoldLevelAttrib(Ihandle* ih, int line, const char* value)
{
  int newLevel;
  if (iupStrToInt(value, &newLevel))
  {
    int level = (int)IupScintillaSendMessage(ih, SCI_GETFOLDLEVEL, line, 0);
    int whiteflag = level & SC_FOLDLEVELWHITEFLAG;
    int headerflag = level & SC_FOLDLEVELHEADERFLAG;
    newLevel = newLevel + SC_FOLDLEVELBASE;
    level = newLevel | whiteflag | headerflag;
    IupScintillaSendMessage(ih, SCI_SETFOLDLEVEL, line, level);
  }

  return 0;
}

static char* iScintillaGetFoldLevelWhiteAttrib(Ihandle* ih, int line)
{
  int level = (int)IupScintillaSendMessage(ih, SCI_GETFOLDLEVEL, line, 0);
  int white = level & SC_FOLDLEVELWHITEFLAG? 1: 0;
  return iupStrReturnBoolean(white);
}

static int iScintillaSetFoldLevelWhiteAttrib(Ihandle* ih, int line, const char* value)
{
  int level = (int)IupScintillaSendMessage(ih, SCI_GETFOLDLEVEL, line, 0);
  int headerflag = level & SC_FOLDLEVELHEADERFLAG;
  int white = iupStrBoolean(value);
  int whiteflag = white? SC_FOLDLEVELWHITEFLAG: 0;
  level = level & SC_FOLDLEVELNUMBERMASK;
  level = level | whiteflag | headerflag;
  IupScintillaSendMessage(ih, SCI_SETFOLDLEVEL, line, level);
  return 0;
}

static char* iScintillaGetFoldLevelHeaderAttrib(Ihandle* ih, int line)
{
  int level = (int)IupScintillaSendMessage(ih, SCI_GETFOLDLEVEL, line, 0);
  int header = level & SC_FOLDLEVELHEADERFLAG ? 1 : 0;
  return iupStrReturnBoolean(header);
}

static int iScintillaSetFoldLevelHeaderAttrib(Ihandle* ih, int line, const char* value)
{
  int level = (int)IupScintillaSendMessage(ih, SCI_GETFOLDLEVEL, line, 0);
  int whiteflag = level & SC_FOLDLEVELWHITEFLAG;
  int header = iupStrBoolean(value);
  int headerflag = header ? SC_FOLDLEVELHEADERFLAG: 0;
  level = level & SC_FOLDLEVELNUMBERMASK;
  level = level | whiteflag | headerflag;
  IupScintillaSendMessage(ih, SCI_SETFOLDLEVEL, line, level);
  return 0;
}

static int iScintillaSetEnsureVisibleAttrib(Ihandle* ih, int line, const char* value)
{
  if (iupStrEqualNoCase(value, "ENFORCEPOLICY"))
    IupScintillaSendMessage(ih, SCI_ENSUREVISIBLEENFORCEPOLICY, line, 0);
  else
    IupScintillaSendMessage(ih, SCI_ENSUREVISIBLE, line, 0);
  return 0;
}

static int iScintillaSetFoldAllAttrib(Ihandle* ih, const char* value)
{
  if (iupStrEqualNoCase(value, "CONTRACT"))
    IupScintillaSendMessage(ih, SCI_FOLDALL, SC_FOLDACTION_CONTRACT, 0);
  else if (iupStrEqualNoCase(value, "EXPAND"))
    IupScintillaSendMessage(ih, SCI_FOLDALL, SC_FOLDACTION_EXPAND, 0);
  else if (iupStrEqualNoCase(value, "TOGGLE"))
    IupScintillaSendMessage(ih, SCI_FOLDALL, SC_FOLDACTION_TOGGLE, 0);
  return 0;
}

static int iScintillaSetShowLinesAttrib(Ihandle* ih, const char* value)
{
  int start = 0, end = -1;
  iupStrToIntInt(value, &start, &end, ':');
  if (end == -1)
    end = (int)IupScintillaSendMessage(ih, SCI_GETLINECOUNT, 0, 0) - 1;
  IupScintillaSendMessage(ih, SCI_SHOWLINES, start, end);
  return 0;
}

static int iScintillaSetHideLinesAttrib(Ihandle* ih, const char* value)
{
  int start = 0, end = -1;
  iupStrToIntInt(value, &start, &end, ':');
  if (end == -1)
    end = (int)IupScintillaSendMessage(ih, SCI_GETLINECOUNT, 0, 0) - 1;
  IupScintillaSendMessage(ih, SCI_HIDELINES, start, end);
  return 0;
}

static int iScintillaSetFoldToggleAttrib(Ihandle* ih, const char* value)
{
  int line, level;
  
  iupStrToInt(value, &line);

  level = (int)IupScintillaSendMessage(ih, SCI_GETFOLDLEVEL, line, 0);
  if (level & SC_FOLDLEVELHEADERFLAG)
  {
    IupScintillaSendMessage(ih, SCI_TOGGLEFOLD, line, 0);
    return 0;
  }

  return 0;
}

static int iScintillaSetFoldLineAttrib(Ihandle* ih, int line, const char* value)
{
  if (iupStrEqualNoCase(value, "CONTRACT"))
    IupScintillaSendMessage(ih, SCI_FOLDLINE, line, SC_FOLDACTION_CONTRACT);
  else if (iupStrEqualNoCase(value, "EXPAND"))
    IupScintillaSendMessage(ih, SCI_FOLDLINE, line, SC_FOLDACTION_EXPAND);
  else if (iupStrEqualNoCase(value, "TOGGLE"))
    IupScintillaSendMessage(ih, SCI_FOLDLINE, line, SC_FOLDACTION_TOGGLE);
  return 0;
}

static int iScintillaSetFoldChildrenAttrib(Ihandle* ih, int line, const char* value)
{
  if (iupStrEqualNoCase(value, "CONTRACT"))
    IupScintillaSendMessage(ih, SCI_FOLDCHILDREN, line, SC_FOLDACTION_CONTRACT);
  else if (iupStrEqualNoCase(value, "EXPAND"))
    IupScintillaSendMessage(ih, SCI_FOLDCHILDREN, line, SC_FOLDACTION_EXPAND);
  else if (iupStrEqualNoCase(value, "TOGGLE"))
    IupScintillaSendMessage(ih, SCI_FOLDCHILDREN, line, SC_FOLDACTION_TOGGLE);
  return 0;
}

static char* iScintillaGetFoldParentAttrib(Ihandle* ih, int start_line)
{
  int line = (int)IupScintillaSendMessage(ih, SCI_GETFOLDPARENT, start_line, 0);
  return iupStrReturnInt(line);
}

static char* iScintillaGetFoldExpandedAttrib(Ihandle* ih, int line)
{
  int expand = (int)IupScintillaSendMessage(ih, SCI_GETFOLDEXPANDED, line, 0);
  return iupStrReturnInt(expand);
}

static int iScintillaSetFoldExpandedAttrib(Ihandle* ih, int line, const char* value)
{
  if (iupStrBoolean(value))
    IupScintillaSendMessage(ih, SCI_SETFOLDEXPANDED, line, 1);
  else
    IupScintillaSendMessage(ih, SCI_SETFOLDEXPANDED, line, 0);
  return 0;
}

void iupScintillaRegisterFolding(Iclass* ic)
{
  iupClassRegisterAttribute(ic, "FOLDFLAGS", NULL, iScintillaSetFoldFlagsAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "FOLDTOGGLE", NULL, iScintillaSetFoldToggleAttrib, NULL, NULL, IUPAF_WRITEONLY | IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "FOLDPARENT", iScintillaGetFoldParentAttrib, NULL, IUPAF_READONLY | IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "FOLDLINE", NULL, iScintillaSetFoldLineAttrib, IUPAF_WRITEONLY | IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "FOLDCHILDREN", NULL, iScintillaSetFoldChildrenAttrib, IUPAF_WRITEONLY | IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "FOLDEXPANDED", iScintillaGetFoldExpandedAttrib, iScintillaSetFoldExpandedAttrib, IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "FOLDLEVEL", iScintillaGetFoldLevelAttrib, iScintillaSetFoldLevelAttrib, IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "FOLDLEVELWHITE", iScintillaGetFoldLevelWhiteAttrib, iScintillaSetFoldLevelWhiteAttrib, IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "FOLDLEVELHEADER", iScintillaGetFoldLevelHeaderAttrib, iScintillaSetFoldLevelHeaderAttrib, IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "ENSUREVISIBLE", NULL, iScintillaSetEnsureVisibleAttrib, IUPAF_WRITEONLY | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "FOLDALL", NULL, iScintillaSetFoldAllAttrib, NULL, NULL, IUPAF_WRITEONLY | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SHOWLINES", NULL, iScintillaSetShowLinesAttrib, NULL, NULL, IUPAF_WRITEONLY | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "HIDELINES", NULL, iScintillaSetHideLinesAttrib, NULL, NULL, IUPAF_WRITEONLY | IUPAF_NO_INHERIT);
}
