/** \file
 * \brief Scintilla control: Cut, copy and paste, and Undo
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


/***** CUT, COPY AND PASTE *****
SCI_CUT
SCI_COPY
SCI_PASTE
SCI_CLEAR
SCI_CANPASTE
   --SCI_COPYRANGE(int start, int end)
   --SCI_COPYTEXT(int length, const char *text)
   --SCI_COPYALLOWLINE
   --SCI_SETPASTECONVERTENDINGS(bool convert)
   --SCI_GETPASTECONVERTENDINGS
*/

static int iScintillaSetClipboardAttrib(Ihandle *ih, const char *value)
{
  if (iupStrEqualNoCase(value, "COPY"))
    IupScintillaSendMessage(ih, SCI_COPY, 0, 0);
  else if (iupStrEqualNoCase(value, "CUT"))
    IupScintillaSendMessage(ih, SCI_CUT, 0, 0);
  else if (iupStrEqualNoCase(value, "PASTE"))
    IupScintillaSendMessage(ih, SCI_PASTE, 0, 0);
  else if (iupStrEqualNoCase(value, "CLEAR"))
    IupScintillaSendMessage(ih, SCI_CLEAR, 0, 0);

  return 0;
}

static char* iScintillaGetCanPasteAttrib(Ihandle* ih)
{
  return iupStrReturnBoolean((int)IupScintillaSendMessage(ih, SCI_CANPASTE, 0, 0));
}

/***** Undo and Redo *****
SCI_UNDO
SCI_CANUNDO
SCI_EMPTYUNDOBUFFER
SCI_REDO
SCI_CANREDO
SCI_SETUNDOCOLLECTION(bool collectUndo)
SCI_GETUNDOCOLLECTION
SCI_BEGINUNDOACTION
SCI_ENDUNDOACTION
--SCI_ADDUNDOACTION(int token, int flags)
*/

static int iScintillaSetUndoAttrib(Ihandle *ih, const char *value)
{
  if (iupStrBoolean(value))
    IupScintillaSendMessage(ih, SCI_UNDO, 0, 0);
  else if (iupStrEqualNoCase(value, "ALL"))
  {
    while ((int)IupScintillaSendMessage(ih, SCI_CANUNDO, 0, 0))
      IupScintillaSendMessage(ih, SCI_UNDO, 0, 0);
  }
  else
    IupScintillaSendMessage(ih, SCI_EMPTYUNDOBUFFER, 0, 0);
  return 0;
}

static char* iScintillaGetUndoAttrib(Ihandle* ih)
{
  return iupStrReturnBoolean((int)IupScintillaSendMessage(ih, SCI_CANUNDO, 0, 0));
}

static int iScintillaSetRedoAttrib(Ihandle *ih, const char *value)
{
  if (iupStrBoolean(value))
    IupScintillaSendMessage(ih, SCI_REDO, 0, 0);
  else if (iupStrEqualNoCase(value, "ALL"))
  {
    while ((int)IupScintillaSendMessage(ih, SCI_CANREDO, 0, 0))
      IupScintillaSendMessage(ih, SCI_REDO, 0, 0);
  }
  else
    IupScintillaSendMessage(ih, SCI_EMPTYUNDOBUFFER, 0, 0);
  return 0;
}

static char* iScintillaGetRedoAttrib(Ihandle* ih)
{
  return iupStrReturnBoolean((int)IupScintillaSendMessage(ih, SCI_CANREDO, 0, 0));
}

static int iScintillaSetUndoCollectAttrib(Ihandle *ih, const char *value)
{
  if (iupStrBoolean(value))
    IupScintillaSendMessage(ih, SCI_SETUNDOCOLLECTION, 1, 0);
  else
    IupScintillaSendMessage(ih, SCI_SETUNDOCOLLECTION, 0, 0);
  return 0;
}

static char* iScintillaGetUndoCollectAttrib(Ihandle* ih)
{
  return iupStrReturnBoolean((int)IupScintillaSendMessage(ih, SCI_GETUNDOCOLLECTION, 0, 0));
}

static int iScintillaSetUndoActionAttrib(Ihandle *ih, const char *value)
{
  if (iupStrEqualNoCase(value, "BEGIN"))
    IupScintillaSendMessage(ih, SCI_BEGINUNDOACTION, 0, 0);
  else if(iupStrEqualNoCase(value, "END"))
    IupScintillaSendMessage(ih, SCI_ENDUNDOACTION, 0, 0);
  return 0;
}

void iupScintillaRegisterClipboard(Iclass* ic)
{
  iupClassRegisterAttribute(ic, "CLIPBOARD", iScintillaGetCanPasteAttrib, iScintillaSetClipboardAttrib, NULL, NULL, IUPAF_NO_SAVE|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "UNDO", iScintillaGetUndoAttrib, iScintillaSetUndoAttrib, NULL, NULL, IUPAF_NO_SAVE|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "REDO", iScintillaGetRedoAttrib, iScintillaSetRedoAttrib, NULL, NULL, IUPAF_NO_SAVE|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "UNDOCOLLECT", iScintillaGetUndoCollectAttrib, iScintillaSetUndoCollectAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "UNDOACTION", NULL, iScintillaSetUndoActionAttrib, NULL, NULL, IUPAF_WRITEONLY | IUPAF_NO_INHERIT);
}
