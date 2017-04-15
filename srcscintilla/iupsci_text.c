/** \file
 * \brief Scintilla control: Text retrieval and modification
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

/***** TEXT RETRIEVAL AND MODIFICATION *****
SCI_GETTEXT(int length, char *text)
SCI_SETTEXT(<unused>, const char *text)
SCI_SETSAVEPOINT
SCI_GETLINE(int line, char *text)
   --SCI_REPLACESEL(<unused>, const char *text)
SCI_SETREADONLY(bool readOnly)
SCI_GETREADONLY
   --SCI_GETTEXTRANGE(<unused>, Sci_TextRange *tr)
   --SCI_ALLOCATE(int bytes, <unused>)
SCI_ADDTEXT(int length, const char *s)
   --SCI_ADDSTYLEDTEXT(int length, cell *s)
SCI_APPENDTEXT(int length, const char *s)
SCI_INSERTTEXT(int pos, const char *text)
SCI_CLEARALL
SCI_DELETERANGE(int pos, int deleteLength)
SCI_CLEARDOCUMENTSTYLE
SCI_GETCHARAT(int position)
   --SCI_GETSTYLEAT(int position)
   --SCI_GETSTYLEDTEXT(<unused>, Sci_TextRange *tr)
   --SCI_SETSTYLEBITS(int bits)
   --SCI_GETSTYLEBITS
   --SCI_TARGETASUTF8(<unused>, char *s)
   --SCI_ENCODEDFROMUTF8(const char *utf8, char *encoded)
   --SCI_SETLENGTHFORENCODE(int bytes)
*/

static char* iScintillaGetValueAttrib(Ihandle* ih)
{
  int len = IupScintillaSendMessage(ih, SCI_GETTEXTLENGTH, 0, 0);
  char* str = iupStrGetMemory(len+1);
  IupScintillaSendMessage(ih, SCI_GETTEXT, len+1, (sptr_t)str); /* include also the terminator */
  return str;
}

static int iScintillaSetValueAttrib(Ihandle* ih, const char* value)
{
  ih->data->ignore_change = 1;
  IupScintillaSendMessage(ih, SCI_SETTEXT, 0, (sptr_t)value);
  ih->data->ignore_change = 0;
  return 0;
}

static char* iScintillaGetLineAttrib(Ihandle* ih, int line)
{
  int len = IupScintillaSendMessage(ih, SCI_LINELENGTH, line, 0);
  char* str = iupStrGetMemory(len+1); 
  IupScintillaSendMessage(ih, SCI_GETLINE, line, (sptr_t)str);
  return str;
}

static char* iScintillaGetReadOnlyAttrib(Ihandle* ih)
{
  return iupStrReturnBoolean(IupScintillaSendMessage(ih, SCI_GETREADONLY, 0, 0));
}

static int iScintillaSetReadOnlyAttrib(Ihandle* ih, const char* value)
{
  if (iupStrBoolean(value))
    IupScintillaSendMessage(ih, SCI_SETREADONLY, 1, 0);
  else
    IupScintillaSendMessage(ih, SCI_SETREADONLY, 0, 0);

  return 0;
}

static int iScintillaSetPrependTextAttrib(Ihandle* ih, const char* value)
{
  int len = strlen(value);

  ih->data->ignore_change = 1;
  IupScintillaSendMessage(ih, SCI_ADDTEXT, len, (sptr_t)value);

  if(ih->data->append_newline)
    IupScintillaSendMessage(ih, SCI_ADDTEXT, 1, (sptr_t)"\n");
  ih->data->ignore_change = 0;

  return 0;
}

static int iScintillaSetAppendTextAttrib(Ihandle* ih, const char* value)
{
  int len = strlen(value);

  ih->data->ignore_change = 1;
  if(ih->data->append_newline)
    IupScintillaSendMessage(ih, SCI_APPENDTEXT, 1, (sptr_t)"\n");

  IupScintillaSendMessage(ih, SCI_APPENDTEXT, len, (sptr_t)value);
  ih->data->ignore_change = 0;

  return 0;
}

static int iScintillaSetInsertTextAttrib(Ihandle* ih, int pos, const char* value)
{
  ih->data->ignore_change = 1;
  IupScintillaSendMessage(ih, SCI_INSERTTEXT, pos, (sptr_t)value);
  ih->data->ignore_change = 0;
  return 0;
}

static int iScintillaSetClearAllAttrib(Ihandle* ih, const char* value)
{
  (void)value;

  ih->data->ignore_change = 1;
  IupScintillaSendMessage(ih, SCI_CLEARALL, 0, 0);
  ih->data->ignore_change = 0;
  return 0;
}

static int iScintillaSetClearDocumentAttrib(Ihandle* ih, const char* value)
{
  (void)value;

  IupScintillaSendMessage(ih, SCI_CLEARDOCUMENTSTYLE, 0, 0);
  return 0;
}

static int iScintillaSetSavePointAttrib(Ihandle* ih, const char* value)
{
  (void)value;

  IupScintillaSendMessage(ih, SCI_SETSAVEPOINT, 0, 0);
  return 0;
}

static char* iScintillaGetModifyAttrib(Ihandle* ih)
{
  return iupStrReturnBoolean (IupScintillaSendMessage(ih, SCI_GETMODIFY, 0, 0)); 
}

static int iScintillaSetDeleteRangeAttrib(Ihandle* ih, const char* value)
{
  int pos, len;
  iupStrToIntInt(value, &pos, &len, ',');

  ih->data->ignore_change = 1;
  IupScintillaSendMessage(ih, SCI_DELETERANGE, pos, len);
  ih->data->ignore_change = 0;
  return 0;
}

static char* iScintillaGetCharAttrib(Ihandle* ih, int pos)
{
  char str[2];
  str[0] = (char)IupScintillaSendMessage(ih, SCI_GETCHARAT, pos, 0);
  str[1] = 0;
  return iupStrReturnStr(str);
}

static int iScintillaSetAppendNewlineAttrib(Ihandle* ih, const char* value)
{
  if (iupStrBoolean(value))
    ih->data->append_newline = 1;
  else
    ih->data->append_newline = 0;
  return 0;
}

static char* iScintillaGetAppendNewlineAttrib(Ihandle* ih)
{
  return iupStrReturnBoolean (ih->data->append_newline); 
}

void iupScintillaRegisterText(Iclass* ic)
{
  iupClassRegisterAttribute(ic,   "APPENDNEWLINE", iScintillaGetAppendNewlineAttrib, iScintillaSetAppendNewlineAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic,   "APPEND", NULL, iScintillaSetAppendTextAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic,   "PREPEND", NULL, iScintillaSetPrependTextAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic,   "VALUE", iScintillaGetValueAttrib, iScintillaSetValueAttrib, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "INSERT", NULL, iScintillaSetInsertTextAttrib, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "LINE", iScintillaGetLineAttrib, NULL, IUPAF_READONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "CHAR", iScintillaGetCharAttrib, NULL, IUPAF_READONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic,   "DELETERANGE", NULL, iScintillaSetDeleteRangeAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic,   "READONLY", iScintillaGetReadOnlyAttrib, iScintillaSetReadOnlyAttrib, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic,   "CLEARALL", NULL, iScintillaSetClearAllAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic,   "CLEARDOCUMENTSTYLE", NULL, iScintillaSetClearDocumentAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic,   "SAVEDSTATE", iScintillaGetModifyAttrib, iScintillaSetSavePointAttrib, NULL, NULL, IUPAF_NO_INHERIT);
}
