/** \file
 * \brief Scintilla control: Margin
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

/***** MARGIN *****
SCI_SETMARGINTYPEN(int margin, int type)
SCI_GETMARGINTYPEN(int margin)
SCI_SETMARGINWIDTHN(int margin, int pixelWidth)
SCI_GETMARGINWIDTHN(int margin)
SCI_SETMARGINMASKN(int margin, int mask)
SCI_GETMARGINMASKN(int margin)
SCI_SETMARGINSENSITIVEN(int margin, bool sensitive)
SCI_GETMARGINSENSITIVEN(int margin)
--SCI_SETMARGINCURSORN(int margin, int cursor)
--SCI_GETMARGINCURSORN(int margin)
SCI_SETMARGINLEFT(<unused>, int pixels)
SCI_GETMARGINLEFT
SCI_SETMARGINRIGHT(<unused>, int pixels)
SCI_GETMARGINRIGHT
--SCI_SETFOLDMARGINCOLOUR(bool useSetting, int colour)
--SCI_SETFOLDMARGINHICOLOUR(bool useSetting, int colour)
SCI_MARGINSETTEXT(int line, char *text)
SCI_MARGINGETTEXT(int line, char *text)
SCI_MARGINSETSTYLE(int line, int style)
SCI_MARGINGETSTYLE(int line)
--SCI_MARGINSETSTYLES(int line, char *styles)
--SCI_MARGINGETSTYLES(int line, char *styles)
SCI_MARGINTEXTCLEARALL
--SCI_MARGINSETSTYLEOFFSET(int style)
--SCI_MARGINGETSTYLEOFFSET
--SCI_SETMARGINOPTIONS(int marginOptions)
--SCI_GETMARGINOPTIONS
*/

static char* iScintillaGetMarginTypeAttribId(Ihandle* ih, int margin)
{
  int type = IupScintillaSendMessage(ih, SCI_GETMARGINTYPEN, margin, 0);

  if (type == SC_MARGIN_NUMBER)
    return "NUMBER";
  if (type == SC_MARGIN_TEXT)
    return "TEXT";
  if (type == SC_MARGIN_RTEXT)
    return "RTEXT";
  if (type == SC_MARGIN_BACK)
    return "BACKGROUND";
  if (type == SC_MARGIN_FORE)
    return "FOREGROUND";

  return "SYMBOL";
}

static int iScintillaSetMarginTypeAttribId(Ihandle* ih, int margin, const char* value)
{
  if (iupStrEqualNoCase(value, "NUMBER"))
    IupScintillaSendMessage(ih, SCI_SETMARGINTYPEN, margin, SC_MARGIN_NUMBER);
  else if (iupStrEqualNoCase(value, "TEXT"))
    IupScintillaSendMessage(ih, SCI_SETMARGINTYPEN, margin, SC_MARGIN_TEXT);
  else if (iupStrEqualNoCase(value, "RTEXT"))
    IupScintillaSendMessage(ih, SCI_SETMARGINTYPEN, margin, SC_MARGIN_RTEXT);
  else if (iupStrEqualNoCase(value, "BACKGROUND"))
    IupScintillaSendMessage(ih, SCI_SETMARGINTYPEN, margin, SC_MARGIN_BACK);
  else if (iupStrEqualNoCase(value, "FOREGROUND"))
    IupScintillaSendMessage(ih, SCI_SETMARGINTYPEN, margin, SC_MARGIN_FORE);
  else  /* SYMBOL */
    IupScintillaSendMessage(ih, SCI_SETMARGINTYPEN, margin, SC_MARGIN_SYMBOL);

  return 0;
}

static char* iScintillaGetMarginWidthAttribId(Ihandle* ih, int margin)
{
  int pixelWidth = IupScintillaSendMessage(ih, SCI_GETMARGINWIDTHN, margin, 0);
  return iupStrReturnInt(pixelWidth);
}

static int iScintillaSetMarginWidthAttribId(Ihandle* ih, int margin, const char* value)
{
  int pixelWidth;

  if (iupStrToInt(value, &pixelWidth))
    IupScintillaSendMessage(ih, SCI_SETMARGINWIDTHN, margin, pixelWidth);

  return 0;
}

static char* iScintillaGetMarginMaskFoldersAttribId(Ihandle* ih, int margin)
{
  int mask = IupScintillaSendMessage(ih, SCI_GETMARGINMASKN, margin, 0);
  return iupStrReturnBoolean(mask & SC_MASK_FOLDERS); 
}

static int iScintillaSetMarginMaskFoldersAttribId(Ihandle* ih, int margin, const char* value)
{
  if (iupStrBoolean(value))
    IupScintillaSendMessage(ih, SCI_SETMARGINMASKN, margin, SC_MASK_FOLDERS);
  else
    IupScintillaSendMessage(ih, SCI_SETMARGINMASKN, margin, ~SC_MASK_FOLDERS);

  return 0;
}

static char* iScintillaGetMarginSensitiveAttribId(Ihandle* ih, int margin)
{
  return iupStrReturnBoolean(IupScintillaSendMessage(ih, SCI_SETMARGINSENSITIVEN, margin, 0)); 
}

static int iScintillaSetMarginSensitiveAttribId(Ihandle* ih, int margin, const char* value)
{
  if (iupStrBoolean(value))
    IupScintillaSendMessage(ih, SCI_SETMARGINSENSITIVEN, margin, 1);
  else
    IupScintillaSendMessage(ih, SCI_SETMARGINSENSITIVEN, margin, 0);

  return 0;
}

static char* iScintillaGetMarginLeftAttrib(Ihandle* ih)
{
  int pixels = IupScintillaSendMessage(ih, SCI_GETMARGINLEFT, 0, 0);
  return iupStrReturnInt(pixels);
}

static int iScintillaSetMarginLeftAttrib(Ihandle* ih, const char* value)
{
  int pixels;

  if (iupStrToInt(value, &pixels))
    IupScintillaSendMessage(ih, SCI_SETMARGINLEFT, 0, pixels);

  return 0;
}

static char* iScintillaGetMarginRightAttrib(Ihandle* ih)
{
  int pixels = IupScintillaSendMessage(ih, SCI_GETMARGINRIGHT, 0, 0);
  return iupStrReturnInt(pixels);
}

static int iScintillaSetMarginRightAttrib(Ihandle* ih, const char* value)
{
  int pixels;

  if (iupStrToInt(value, &pixels))
    IupScintillaSendMessage(ih, SCI_SETMARGINRIGHT, 0, pixels);

  return 0;
}

static char* iScintillaGetMarginTextAttribId(Ihandle* ih, int line)
{
  int len = IupScintillaSendMessage(ih, SCI_MARGINGETTEXT, line, 0);
  char* str = iupStrGetMemory(len+1);
  IupScintillaSendMessage(ih, SCI_MARGINGETTEXT, line, (sptr_t)str);
  return str;
}

static int iScintillaSetMarginTextAttribId(Ihandle* ih, int line, const char* value)
{
  IupScintillaSendMessage(ih, SCI_MARGINSETTEXT, line, (sptr_t)value);
  return 0;
}

static int iScintillaSetMarginTextClearAllAttrib(Ihandle* ih, const char* value)
{
  (void)value;
  IupScintillaSendMessage(ih, SCI_MARGINTEXTCLEARALL, 0, 0);
  return 0;
}

static char* iScintillaGetMarginTextStyleAttribId(Ihandle* ih, int line)
{
  int style = IupScintillaSendMessage(ih, SCI_MARGINGETSTYLE, line, 0);
  return iupStrReturnInt(style);
}

static int iScintillaSetMarginTextStyleAttribId(Ihandle* ih, int line, const char* value)
{
  int style;

  iupStrToInt(value, &style);
  
  IupScintillaSendMessage(ih, SCI_MARGINSETSTYLE, line, style);

  return 0;
}

static char* iScintillaGetMarginCursorAttribId(Ihandle* ih, int margin)
{
  if(IupScintillaSendMessage(ih, SCI_GETMARGINCURSORN, margin, 0) == SC_CURSORARROW)
    return "ARROW";
  else
    return "REVERSEARROW";
}

static int iScintillaSetMarginCursorAttribId(Ihandle* ih, int margin, const char* value)
{
  if (iupStrEqualNoCase(value, "ARROW"))
    IupScintillaSendMessage(ih, SCI_SETMARGINCURSORN, margin, SC_CURSORARROW);
  else  /* REVERSEARROW */
    IupScintillaSendMessage(ih, SCI_SETMARGINCURSORN, margin, SC_CURSORREVERSEARROW);

  return 0;
}

void iupScintillaRegisterMargin(Iclass* ic)
{
  iupClassRegisterAttributeId(ic, "MARGINTYPE", iScintillaGetMarginTypeAttribId, iScintillaSetMarginTypeAttribId, IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "MARGINWIDTH", iScintillaGetMarginWidthAttribId, iScintillaSetMarginWidthAttribId, IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "MARGINMASKFOLDERS", iScintillaGetMarginMaskFoldersAttribId, iScintillaSetMarginMaskFoldersAttribId, IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "MARGINSENSITIVE", iScintillaGetMarginSensitiveAttribId, iScintillaSetMarginSensitiveAttribId, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic,   "MARGINLEFT", iScintillaGetMarginLeftAttrib, iScintillaSetMarginLeftAttrib, IUPAF_SAMEASSYSTEM, "1", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic,   "MARGINRIGHT", iScintillaGetMarginRightAttrib, iScintillaSetMarginRightAttrib, IUPAF_SAMEASSYSTEM, "1", IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "MARGINTEXT", iScintillaGetMarginTextAttribId, iScintillaSetMarginTextAttribId, IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "MARGINTEXTSTYLE", iScintillaGetMarginTextStyleAttribId, iScintillaSetMarginTextStyleAttribId, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic,   "MARGINTEXTCLEARALL", NULL, iScintillaSetMarginTextClearAllAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "MARGINCURSOR", iScintillaGetMarginCursorAttribId, iScintillaSetMarginCursorAttribId, IUPAF_NO_INHERIT);
}
