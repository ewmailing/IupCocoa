/** \file
 * \brief Scintilla control: Autocompletion
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


/***** Autocompletion *****
SCI_AUTOCSHOW(int lenEntered, const char *list)
SCI_AUTOCCANCEL
SCI_AUTOCACTIVE
SCI_AUTOCPOSSTART
SCI_AUTOCCOMPLETE
-- SCI_AUTOCSTOPS(<unused>, const char *chars)
-- SCI_AUTOCSETSEPARATOR(char separator)
-- SCI_AUTOCGETSEPARATOR
SCI_AUTOCSELECT(<unused>, const char *select)
SCI_AUTOCGETCURRENT
-- SCI_AUTOCGETCURRENTTEXT(<unused>, char *text)
-- SCI_AUTOCSETCANCELATSTART(bool cancel)
-- SCI_AUTOCGETCANCELATSTART
-- SCI_AUTOCSETFILLUPS(<unused>, const char *chars)
-- SCI_AUTOCSETCHOOSESINGLE(bool chooseSingle)
-- SCI_AUTOCGETCHOOSESINGLE
-- SCI_AUTOCSETIGNORECASE(bool ignoreCase)
-- SCI_AUTOCGETIGNORECASE
-- SCI_AUTOCSETCASEINSENSITIVEBEHAVIOUR(int behaviour)
-- SCI_AUTOCGETCASEINSENSITIVEBEHAVIOUR
-- SCI_AUTOCSETORDER(int order)
-- SCI_AUTOCGETORDER
-- SCI_AUTOCSETAUTOHIDE(bool autoHide)
-- SCI_AUTOCGETAUTOHIDE
SCI_AUTOCSETDROPRESTOFWORD(bool dropRestOfWord)
SCI_AUTOCGETDROPRESTOFWORD
-- SCI_REGISTERIMAGE(int type, const char *xpmData)
-- SCI_REGISTERRGBAIMAGE(int type, const char *pixels)
-- SCI_CLEARREGISTEREDIMAGES
-- SCI_AUTOCSETTYPESEPARATOR(char separatorCharacter)
-- SCI_AUTOCGETTYPESEPARATOR
SCI_AUTOCSETMAXHEIGHT(int rowCount)
SCI_AUTOCGETMAXHEIGHT
SCI_AUTOCSETMAXWIDTH(int characterCount)
SCI_AUTOCGETMAXWIDTH
*/

static int iScintillaSetAutoCShowAttrib(Ihandle* ih, int number, const char* value)
{
  if(number < 0) number = 0;
  IupScintillaSendMessage(ih, SCI_AUTOCSHOW, number, (sptr_t)value);
  return 0;
}

static int iScintillaSetAutoCCancelAttrib(Ihandle* ih, const char* value)
{
  IupScintillaSendMessage(ih, SCI_AUTOCCANCEL, 0, 0);
  (void)value;
  return 0;
}

static char* iScintillaGetAutoCActiveAttrib(Ihandle* ih)
{
  return iupStrReturnBoolean((int)IupScintillaSendMessage(ih, SCI_AUTOCACTIVE, 0, 0));
}

static char* iScintillaGetAutoCPosStartAttrib(Ihandle* ih)
{
  return iupStrReturnInt((int)IupScintillaSendMessage(ih, SCI_AUTOCPOSSTART, 0, 0));
}

static int iScintillaSetAutoCCompleteAttrib(Ihandle* ih, const char* value)
{
  IupScintillaSendMessage(ih, SCI_AUTOCCOMPLETE, 0, 0);
  (void)value;
  return 0;
}

static char* iScintillaGetAutoCSelectedIndexAttrib(Ihandle* ih)
{
  return iupStrReturnInt((int)IupScintillaSendMessage(ih, SCI_AUTOCGETCURRENT, 0, 0));
}

static int iScintillaSetAutoCSelectAttrib(Ihandle* ih, const char* value)
{
  IupScintillaSendMessage(ih, SCI_AUTOCSELECT, 0, (sptr_t)value);
  return 0;
}

static char* iScintillaGetAutoCDropRestOfWordAttrib(Ihandle* ih)
{
  return iupStrReturnBoolean((int)IupScintillaSendMessage(ih, SCI_AUTOCGETDROPRESTOFWORD, 0, 0));
}

static int iScintillaSetAutoCDropRestOfWordAttrib(Ihandle* ih, const char* value)
{
  if (iupStrBoolean(value))
    IupScintillaSendMessage(ih, SCI_AUTOCSETDROPRESTOFWORD, 1, 0);
  else
    IupScintillaSendMessage(ih, SCI_AUTOCSETDROPRESTOFWORD, 0, 0);

  return 0;
}

static char* iScintillaGetAutoCMaxHeightAttrib(Ihandle* ih)
{
  return iupStrReturnInt((int)IupScintillaSendMessage(ih, SCI_AUTOCGETMAXHEIGHT, 0, 0));
}

static int iScintillaSetAutoCMaxHeightAttrib(Ihandle* ih, const char* value)
{
  int rowCount = 1;

  if (!value)
    return 0;

  iupStrToInt(value, &rowCount);
  if (rowCount < 1) rowCount = 1;

  IupScintillaSendMessage(ih, SCI_AUTOCSETMAXHEIGHT, rowCount, 0);

  return 0;
}

static char* iScintillaGetAutoCMaxWidthAttrib(Ihandle* ih)
{
  return iupStrReturnInt((int)IupScintillaSendMessage(ih, SCI_AUTOCGETMAXWIDTH, 0, 0));
}

static int iScintillaSetAutoCMaxWidthAttrib(Ihandle* ih, const char* value)
{
  int charCount = 0;

  if (!value)
    return 0;

  iupStrToInt(value, &charCount);
  if (charCount < 0) charCount = 0;

  IupScintillaSendMessage(ih, SCI_AUTOCSETMAXWIDTH, charCount, 0);

  return 0;
}

void iupScintillaRegisterAutocompletion(Iclass* ic)
{
  iupClassRegisterAttributeId(ic, "AUTOCSHOW", NULL, iScintillaSetAutoCShowAttrib, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic,   "AUTOCCANCEL", NULL, iScintillaSetAutoCCancelAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic,   "AUTOCACTIVE", iScintillaGetAutoCActiveAttrib, NULL, NULL, NULL, IUPAF_READONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic,   "AUTOCPOSSTART", iScintillaGetAutoCPosStartAttrib, NULL, NULL, NULL, IUPAF_READONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic,   "AUTOCCOMPLETE", NULL, iScintillaSetAutoCCompleteAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic,   "AUTOCSELECT", NULL, iScintillaSetAutoCSelectAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic,   "AUTOCSELECTEDINDEX", iScintillaGetAutoCSelectedIndexAttrib, NULL, NULL, NULL, IUPAF_READONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic,   "AUTOCDROPRESTOFWORD", iScintillaGetAutoCDropRestOfWordAttrib, iScintillaSetAutoCDropRestOfWordAttrib, IUPAF_SAMEASSYSTEM, "NO", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic,   "AUTOCMAXHEIGHT", iScintillaGetAutoCMaxHeightAttrib, iScintillaSetAutoCMaxHeightAttrib, IUPAF_SAMEASSYSTEM, "5", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic,   "AUTOCMAXWIDTH", iScintillaGetAutoCMaxWidthAttrib, iScintillaSetAutoCMaxWidthAttrib, IUPAF_SAMEASSYSTEM, "0", IUPAF_NO_INHERIT);
}
