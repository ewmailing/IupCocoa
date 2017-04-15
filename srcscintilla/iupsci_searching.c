/** \file
 * \brief Scintilla control: Searching
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


/***** Searching *****
-- SCI_FINDTEXT(int flags, Sci_TextToFind *ttf)
-- SCI_SEARCHANCHOR
-- SCI_SEARCHNEXT(int searchFlags, const char *text)
-- SCI_SEARCHPREV(int searchFlags, const char *text)
SCI_SETTARGETSTART(int pos)
SCI_GETTARGETSTART
SCI_SETTARGETEND(int pos)
SCI_GETTARGETEND
SCI_TARGETFROMSELECTION
SCI_SETSEARCHFLAGS(int searchFlags)
SCI_GETSEARCHFLAGS
SCI_SEARCHINTARGET(int length, const char *text)
SCI_REPLACETARGET(int length, const char *text)
-- SCI_REPLACETARGETRE(int length, const char *text)
-- SCI_GETTAG(int tagNumber, char *tagValue)
*/

static int iScintillaSetTargetFromSelectionAttrib(Ihandle* ih, const char* value)
{
  IupScintillaSendMessage(ih, SCI_TARGETFROMSELECTION, 0, 0);
  (void)value;
  return 0;
}

static char* iScintillaGetSearchFlagsAttrib(Ihandle* ih)
{
  int flags = IupScintillaSendMessage(ih, SCI_GETSEARCHFLAGS, 0, 0);
  char* str = iupStrGetMemory(50);

  *str = 0;
  if (flags & SCFIND_MATCHCASE) strcpy(str, "MATCHCASE");

  if (*str) strcat(str, "|");
  if (flags & SCFIND_WHOLEWORD) strcat(str, "WHOLEWORD");

  if (*str) strcat(str, "|");
  if (flags & SCFIND_WORDSTART) strcat(str, "WORDSTART");

  if (*str) strcat(str, "|");
  if (flags & SCFIND_REGEXP) strcat(str, "REGEXP");

  if (*str) strcat(str, "|");
  if (flags & SCFIND_POSIX) strcat(str, "POSIX");

  return str;
}

static int iScintillaSetSearchFlagsAttrib(Ihandle* ih, const char* value)
{
  if (!value)
    IupScintillaSendMessage(ih, SCI_SETSEARCHFLAGS, 0, 0);
  else
  {
    int flags = 0;
    char* VALUE = iupStrDup(value);
    iupStrUpper(VALUE, VALUE);

    if (strstr(VALUE, "MATCHCASE"))
      flags |= SCFIND_MATCHCASE;
    if (strstr(VALUE, "WHOLEWORD"))
      flags |= SCFIND_WHOLEWORD;
    if (strstr(VALUE, "WORDSTART"))
      flags |= SCFIND_WORDSTART;
    if (strstr(VALUE, "REGEXP"))
      flags |= SCFIND_REGEXP;
    if (strstr(VALUE, "POSIX"))
      flags |= SCFIND_POSIX;

    IupScintillaSendMessage(ih, SCI_SETSEARCHFLAGS, flags, 0);
    free(VALUE);
  }

  return 0;
}

static char* iScintillaGetTargetEndAttrib(Ihandle* ih)
{
  return iupStrReturnInt(IupScintillaSendMessage(ih, SCI_GETTARGETEND, 0, 0));
}

static int iScintillaSetTargetEndAttrib(Ihandle* ih, const char* value)
{
  int end;

  if (!value)
    return 0;

  iupStrToInt(value, &end);
  if (end < 1) end = IupScintillaSendMessage(ih, SCI_GETTEXTLENGTH, 0, 0);

  IupScintillaSendMessage(ih, SCI_SETTARGETEND, end, 0);

  return 0;
}

static char* iScintillaGetTargetStartAttrib(Ihandle* ih)
{
  return iupStrReturnInt(IupScintillaSendMessage(ih, SCI_GETTARGETSTART, 0, 0));
}

static int iScintillaSetTargetStartAttrib(Ihandle* ih, const char* value)
{
  int start;

  if (!value)
    return 0;

  iupStrToInt(value, &start);
  if (start < 1) start = 1;

  IupScintillaSendMessage(ih, SCI_SETTARGETSTART, start, 0);

  return 0;
}

static int iScintillaSetSearchInTargetAttrib(Ihandle* ih, const char* value)
{
  int len = strlen(value);

  if (!value)
    return 0;

  IupScintillaSendMessage(ih, SCI_SEARCHINTARGET, len, (sptr_t)value);

  return 0;
}

static int iScintillaSetReplaceTargetAttrib(Ihandle* ih, const char* value)
{
  if (!value)
    return 0;

  IupScintillaSendMessage(ih, SCI_REPLACETARGET, (uptr_t)-1, (sptr_t)value);
  return 0;
}

void iupScintillaRegisterSearching(Iclass* ic)
{
  iupClassRegisterAttribute(ic, "SEARCHFLAGS", iScintillaGetSearchFlagsAttrib, iScintillaSetSearchFlagsAttrib, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "TARGETEND", iScintillaGetTargetEndAttrib, iScintillaSetTargetEndAttrib, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "TARGETSTART", iScintillaGetTargetStartAttrib, iScintillaSetTargetStartAttrib, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "TARGETFROMSELECTION", NULL, iScintillaSetTargetFromSelectionAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SEARCHINTARGET", NULL, iScintillaSetSearchInTargetAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "REPLACETARGET", NULL, iScintillaSetReplaceTargetAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
}
