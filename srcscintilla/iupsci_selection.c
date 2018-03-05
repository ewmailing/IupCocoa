/** \file
 * \brief Scintilla control: Selection and information
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


#include <Scintilla.h>

#include "iup.h"
#include "iup_scintilla.h"

#include "iup_object.h"
#include "iup_attrib.h"
#include "iup_str.h"

#include "iupsci.h"


/***** SELECTION AND INFORMATION *****
SCI_GETTEXTLENGTH
--SCI_GETLENGTH
SCI_GETLINECOUNT
SCI_SETFIRSTVISIBLELINE(int lineDisplay)
SCI_GETFIRSTVISIBLELINE
SCI_LINESONSCREEN
--SCI_GETMODIFY
SCI_SETSEL(int anchorPos, int currentPos)
SCI_GOTOPOS(int position)
--SCI_GOTOLINE(int line)
--SCI_SETCURRENTPOS(int position)
SCI_GETCURRENTPOS
--SCI_SETANCHOR(int position)
--SCI_GETANCHOR
--SCI_SETSELECTIONSTART(int position)
SCI_GETSELECTIONSTART
--SCI_SETSELECTIONEND(int position)
SCI_GETSELECTIONEND
SCI_SETEMPTYSELECTION(int pos)
SCI_SELECTALL
SCI_LINEFROMPOSITION(int position)
SCI_POSITIONFROMLINE(int line)
--SCI_GETLINEENDPOSITION(int line)
SCI_LINELENGTH(int line)
SCI_GETCOLUMN(int position)
--SCI_FINDCOLUMN(int line, int column)
--SCI_POSITIONFROMPOINT(int x, int y)
--SCI_POSITIONFROMPOINTCLOSE(int x, int y)
--SCI_CHARPOSITIONFROMPOINT(int x, int y)
--SCI_CHARPOSITIONFROMPOINTCLOSE(int x, int y)
--SCI_POINTXFROMPOSITION(<unused>, int position)
--SCI_POINTYFROMPOSITION(<unused>, int position)
--SCI_HIDESELECTION(bool hide)
SCI_GETSELTEXT(<unused>, char *text)
SCI_GETCURLINE(int textLen, char *text)
--SCI_SELECTIONISRECTANGLE
--SCI_SETSELECTIONMODE(int mode)
--SCI_GETSELECTIONMODE
--SCI_GETLINESELSTARTPOSITION(int line)
--SCI_GETLINESELENDPOSITION(int line)
SCI_MOVECARETINSIDEVIEW
SCI_WORDENDPOSITION(int position, bool onlyWordCharacters)
SCI_WORDSTARTPOSITION(int position, bool onlyWordCharacters)
SCI_ISRANGEWORD(int start, int end)
--SCI_POSITIONBEFORE(int position)
--SCI_POSITIONAFTER(int position)
--SCI_POSITIONRELATIVE(int position, int relative)
--SCI_COUNTCHARACTERS(int startPos, int endPos)
--SCI_TEXTWIDTH(int styleNumber, const char *text)
--SCI_TEXTHEIGHT(int line)
--SCI_CHOOSECARETX
--SCI_MOVESELECTEDLINESUP
--SCI_MOVESELECTEDLINESDOWN
--SCI_SETMOUSESELECTIONRECTANGULARSWITCH(bool mouseSelectionRectangularSwitch)
--SCI_GETMOUSESELECTIONRECTANGULARSWITCH
SCI_SETSELFORE
SCI_SETSELBACK
SCI_GETSELALPHA
*/

static char* iScintillaGetCaretStyleAttrib(Ihandle* ih)
{
  int style = (int)IupScintillaSendMessage(ih, SCI_GETCARETSTYLE, 0, 0);
  if (style == CARETSTYLE_INVISIBLE)
    return "INVISIBLE";
  else if (style == CARETSTYLE_BLOCK)
    return "BLOCK";
  else
    return "LINE";
}

static int iScintillaSetCaretStyleAttrib(Ihandle* ih, const char* value)
{
  if (iupStrEqualNoCase(value, "INVISIBLE"))
    IupScintillaSendMessage(ih, SCI_SETCARETSTYLE, CARETSTYLE_INVISIBLE, 0);
  else if (iupStrEqualNoCase(value, "BLOCK"))
    IupScintillaSendMessage(ih, SCI_SETCARETSTYLE, CARETSTYLE_BLOCK, 0);
  else if (iupStrEqualNoCase(value, "LINE"))
    IupScintillaSendMessage(ih, SCI_SETCARETSTYLE, CARETSTYLE_LINE, 0);
  return 0;
}

static char* iScintillaGetCaretWidthAttrib(Ihandle* ih)
{
  int width = (int)IupScintillaSendMessage(ih, SCI_GETCARETWIDTH, 0, 0);
  return iupStrReturnInt(width);
}

static int iScintillaSetCaretWidthAttrib(Ihandle* ih, const char* value)
{
  int width;
  if (iupStrToInt(value, &width))
    IupScintillaSendMessage(ih, SCI_SETCARETWIDTH, width, 0);
  return 0;
}

static char* iScintillaGetCaretColorAttrib(Ihandle* ih)
{
  unsigned char r, g, b;
  long color = (long)IupScintillaSendMessage(ih, SCI_GETCARETFORE, 0, 0);
  iupScintillaDecodeColor(color, &r, &g, &b);
  return iupStrReturnRGB(r, g, b);
}

static int iScintillaSetCaretColorAttrib(Ihandle* ih, const char* value)
{
  unsigned char r, g, b;
  if (iupStrToRGB(value, &r, &g, &b))
    IupScintillaSendMessage(ih, SCI_SETCARETFORE, iupScintillaEncodeColor(r, g, b), 0);
  return 0;
}

/*******************************************************************************************/

static char* iScintillaGetCurrentLineAttrib(Ihandle* ih)
{
  int textLen = (int)IupScintillaSendMessage(ih, SCI_GETCURLINE, 0, 0);
  char* str = iupStrGetMemory(textLen+1);
  IupScintillaSendMessage(ih, SCI_GETCURLINE, textLen, (sptr_t)str);
  return str;
}

static char* iScintillaGetCountAttrib(Ihandle* ih)
{
  int count = (int)IupScintillaSendMessage(ih, SCI_GETTEXTLENGTH, 0, 0);
  return iupStrReturnInt(count);
}

static char* iScintillaGetLineCountAttrib(Ihandle* ih)
{
  int count = (int)IupScintillaSendMessage(ih, SCI_GETLINECOUNT, 0, 0);
  return iupStrReturnInt(count);
}

static char* iScintillaGetCaretAttrib(Ihandle* ih)
{
  int col, lin, pos;

  pos = (int)IupScintillaSendMessage(ih, SCI_GETCURRENTPOS, 0, 0);
  iupScintillaConvertPosToLinCol(ih, pos, &lin, &col);

  return iupStrReturnIntInt(lin, col, ',');
}

static int iScintillaSetCaretAttrib(Ihandle* ih, const char* value)
{
  int pos, lin = 0, col = 0;
  iupStrToIntInt(value, &lin, &col, ',');  /* be permissive in SetCaret, do not abort if invalid */
  if (lin < 0) lin = 0;
  if (col < 0) col = 0;

  iupScintillaConvertLinColToPos(ih, lin, col, &pos);

  IupScintillaSendMessage(ih, SCI_GOTOPOS, pos, 0);

  return 0;
}

static char* iScintillaGetCaretPosAttrib(Ihandle* ih)
{
  return iupStrReturnInt((int)IupScintillaSendMessage(ih, SCI_GETCURRENTPOS, 0, 0));
}

static int iScintillaSetCaretPosAttrib(Ihandle* ih, const char* value)
{
  int pos = 0;

  if (!value)
    return 0;

  iupStrToInt(value, &pos);    /* be permissive in SetCaret, do not abort if invalid */
  if (pos < 0) pos = 0;

  IupScintillaSendMessage(ih, SCI_GOTOPOS, pos, 0);

  return 0;
}

static int iScintillaSetCaretToViewAttrib(Ihandle *ih, const char *value)
{
  (void)value;
  IupScintillaSendMessage(ih, SCI_MOVECARETINSIDEVIEW, 0, 0);
  return 0;
}

static char* iScintillaGetSelectedTextAttrib(Ihandle* ih)
{
  int start = (int)IupScintillaSendMessage(ih, SCI_GETSELECTIONSTART, 0, 0);
  int end   = (int)IupScintillaSendMessage(ih, SCI_GETSELECTIONEND, 0, 0);
  char* str;

  if(start == end)
    return NULL;

  str = iupStrGetMemory(end - start + 1 + 1);
  IupScintillaSendMessage(ih, SCI_GETSELTEXT, 0, (sptr_t)str);
  return str;
}

static int iScintillaSetSelectedTextAttrib(Ihandle* ih, const char* value)
{
  int start = (int)IupScintillaSendMessage(ih, SCI_GETSELECTIONSTART, 0, 0);
  int end   = (int)IupScintillaSendMessage(ih, SCI_GETSELECTIONEND, 0, 0);

  if(start == end)
    return 0;

  IupScintillaSendMessage(ih, SCI_REPLACESEL, 0, (sptr_t)value);

  return 0;
}

static char* iScintillaGetSelectionAttrib(Ihandle* ih)
{
  int start = (int)IupScintillaSendMessage(ih, SCI_GETSELECTIONSTART, 0, 0);
  int end   = (int)IupScintillaSendMessage(ih, SCI_GETSELECTIONEND, 0, 0);
  int start_col, start_lin, end_col, end_lin;

  if (start == end)
    return NULL;

  start_lin = (int)IupScintillaSendMessage(ih, SCI_LINEFROMPOSITION, start, 0);
  start_col = (int)IupScintillaSendMessage(ih, SCI_GETCOLUMN, start, 0);

  end_lin = (int)IupScintillaSendMessage(ih, SCI_LINEFROMPOSITION, end, 0);
  end_col = (int)IupScintillaSendMessage(ih, SCI_GETCOLUMN, end, 0);

  return iupStrReturnStrf("%d,%d:%d,%d", start_lin, start_col, end_lin, end_col);
}

static int iScintillaSetSelectionAttrib(Ihandle* ih, const char* value)
{
  int lin_start=1, col_start=1, lin_end=1, col_end=1;
  int anchorPos, currentPos, len_start, len_end, line_count;

  if (!value || iupStrEqualNoCase(value, "NONE"))
  {
    IupScintillaSendMessage(ih, SCI_SETEMPTYSELECTION, 0, 0);
    return 0;
  }

  if (iupStrEqualNoCase(value, "ALL"))
  {
    IupScintillaSendMessage(ih, SCI_SELECTALL, 0, 0);
    return 0;
  }

  if (sscanf(value, "%d,%d:%d,%d", &lin_start, &col_start, &lin_end, &col_end)!=4)
    return 0;

  if (lin_start < 0 || lin_end < 0)
    return 0;

  line_count = (int)IupScintillaSendMessage(ih, SCI_GETLINECOUNT, 0, 0);
  if (lin_start >= line_count)
    lin_start = line_count - 1;
  if (lin_end >= line_count)
    lin_end = line_count - 1;

  len_start = (int)IupScintillaSendMessage(ih, SCI_LINELENGTH, lin_start, 0);
  len_end = (int)IupScintillaSendMessage(ih, SCI_LINELENGTH, lin_end, 0);

  if (col_start >= len_start)
    col_start = len_start - 1;
  if (col_end > len_end) /* col_end correspond to the character after the last selected character */
    col_end = len_end;

  anchorPos  = (int)IupScintillaSendMessage(ih, SCI_POSITIONFROMLINE, lin_start, 0) + col_start;
  currentPos = (int)IupScintillaSendMessage(ih, SCI_POSITIONFROMLINE, lin_end, 0)   + col_end;

  IupScintillaSendMessage(ih, SCI_SETSEL, anchorPos, currentPos);

  return 0;
}

static char* iScintillaGetSelectionPosAttrib(Ihandle* ih)
{
  int start = (int)IupScintillaSendMessage(ih, SCI_GETSELECTIONSTART, 0, 0);
  int end   = (int)IupScintillaSendMessage(ih, SCI_GETSELECTIONEND, 0, 0);

  if (start == end)
    return NULL;

  return iupStrReturnIntInt(start, end, ':');
}

static int iScintillaSetSelectionPosAttrib(Ihandle* ih, const char* value)
{
  int anchorPos = 0, currentPos = 0;

  if (!value || iupStrEqualNoCase(value, "NONE"))
  {
    IupScintillaSendMessage(ih, SCI_SETEMPTYSELECTION, 0, 0);
    return 0;
  }

  if (iupStrEqualNoCase(value, "ALL"))
  {
    IupScintillaSendMessage(ih, SCI_SELECTALL, 0, 0);
    return 0;
  }

  if (iupStrToIntInt(value, &anchorPos, &currentPos, ':') != 2)
    return 0;

  if (anchorPos<0 || currentPos<0)
    return 0;

  IupScintillaSendMessage(ih, SCI_SETSEL, anchorPos, currentPos);

  return 0;
}

static char* iScintillaGetWordPosAttrib(Ihandle* ih, int pos)
{
  int start = (int)IupScintillaSendMessage(ih, SCI_WORDSTARTPOSITION, pos, 1);
  int end = (int)IupScintillaSendMessage(ih, SCI_WORDENDPOSITION, pos, 1);

  if (start == end)
    return NULL;

  return iupStrReturnIntInt(start, end, ':');
}

#ifdef SCI_ISRANGEWORD
static char* iScintillaGetIsWordAttrib(Ihandle* ih)
{
  int start, end, wordEnd;
  const char *range = IupGetAttribute(ih, "WORDRANGE");

  iupStrToIntInt(range, &start, &end, ':');

  if (start == end)
    return 0;

  wordEnd = (int)IupScintillaSendMessage(ih, SCI_WORDENDPOSITION, start, 1);

  return iupStrReturnBoolean((int)IupScintillaSendMessage(ih, SCI_ISRANGEWORD, start, end) && wordEnd == end);
}
#endif

static int iScintillaSetFirstVisibleLineAttrib(Ihandle* ih, const char* value)
{
  int line = 0;

  if (iupStrToInt(value, &line))
    IupScintillaSendMessage(ih, SCI_SETFIRSTVISIBLELINE, line, 0);

  return 0;
}

static char* iScintillaGetFirstVisibleLineAttrib(Ihandle* ih)
{
  int line = (int)IupScintillaSendMessage(ih, SCI_GETFIRSTVISIBLELINE, 0, 0);
  return iupStrReturnInt(line);
}

static char* iScintillaGetVisibleLinesCountAttrib(Ihandle* ih)
{
  int count = (int)IupScintillaSendMessage(ih, SCI_LINESONSCREEN, 0, 0);
  return iupStrReturnInt(count);
}

static char* iScintillaGetCaretLineVisibleAttrib(Ihandle* ih)
{
  return iupStrReturnBoolean((int)IupScintillaSendMessage(ih, SCI_GETCARETLINEVISIBLE, 0, 0));
}

static int iScintillaSetCaretLineVisibleAttrib(Ihandle* ih, const char* value)
{
  if (iupStrBoolean(value))
    IupScintillaSendMessage(ih, SCI_SETCARETLINEVISIBLE, 1, 0);
  else
    IupScintillaSendMessage(ih, SCI_SETCARETLINEVISIBLE, 0, 0);

  return 0;
}

static char* iScintillaGetCaretLineBackColorAttrib(Ihandle* ih)
{
  unsigned char r, g, b;
  long color = (long)IupScintillaSendMessage(ih, SCI_GETCARETLINEBACK, 0, 0);
  iupScintillaDecodeColor(color, &r, &g, &b);
  return iupStrReturnRGB(r, g, b);
}

static int iScintillaSetCaretLineBackColorAttrib(Ihandle* ih, const char* value)
{
  unsigned char r, g, b;
  if (iupStrToRGB(value, &r, &g, &b))
    IupScintillaSendMessage(ih, SCI_SETCARETLINEBACK, iupScintillaEncodeColor(r, g, b), 0);
  return 0;
}


static int iScintillaSetCaretLineBackAlphaAttrib(Ihandle* ih, const char* value)
{
  int alpha;
  if (iupStrToInt(value, &alpha))
    IupScintillaSendMessage(ih, SCI_SETCARETLINEBACKALPHA, alpha, 0);
  return 0;
}

static char* iScintillaGetCaretLineBackAlphaAttrib(Ihandle* ih)
{
  int alpha = (int)IupScintillaSendMessage(ih, SCI_GETCARETLINEBACKALPHA, 0, 0);
  return iupStrReturnInt(alpha);
}

static int iScintillaSetSelectionForeColorAttrib(Ihandle* ih, const char* value)
{
  if (!value)
    IupScintillaSendMessage(ih, SCI_SETSELFORE, 0, 0);
  else
  {
    unsigned char r, g, b;

    if (!iupStrToRGB(value, &r, &g, &b))
      return 0;

    IupScintillaSendMessage(ih, SCI_SETSELFORE, 1, iupScintillaEncodeColor(r, g, b));
  }
  return 1;
}

static int iScintillaSetSelectionBackColorAttrib(Ihandle* ih, const char* value)
{
  if (!value)
    IupScintillaSendMessage(ih, SCI_SETSELBACK, 0, 0);
  else
  {
    unsigned char r, g, b;

    if (!iupStrToRGB(value, &r, &g, &b))
      return 0;

    IupScintillaSendMessage(ih, SCI_SETSELBACK, 1, iupScintillaEncodeColor(r, g, b));
  }
  return 1;
}

static int iScintillaSetSelectionAlphaAttrib(Ihandle* ih, const char* value)
{
  int alpha;
  if (iupStrToInt(value, &alpha))
    IupScintillaSendMessage(ih, SCI_SETSELALPHA, alpha, 0);
  return 0;
}

static char* iScintillaGetSelectionAlphaAttrib(Ihandle* ih)
{
  int alpha = (int)IupScintillaSendMessage(ih, SCI_GETSELALPHA, 0, 0);
  return iupStrReturnInt(alpha);
}


void iupScintillaRegisterSelection(Iclass* ic)
{
  iupClassRegisterAttribute(ic, "CARETLINEVISIBLE",   iScintillaGetCaretLineVisibleAttrib,   iScintillaSetCaretLineVisibleAttrib,   NULL, NULL, IUPAF_NO_SAVE | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "CARETLINEBACKCOLOR", iScintillaGetCaretLineBackColorAttrib, iScintillaSetCaretLineBackColorAttrib, NULL, NULL, IUPAF_NO_SAVE | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "CARETLINEBACKALPHA", iScintillaGetCaretLineBackAlphaAttrib, iScintillaSetCaretLineBackAlphaAttrib, NULL, NULL, IUPAF_NO_SAVE | IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "CARETSTYLE", iScintillaGetCaretStyleAttrib, iScintillaSetCaretStyleAttrib, NULL, NULL, IUPAF_NO_SAVE | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "CARETWIDTH", iScintillaGetCaretWidthAttrib, iScintillaSetCaretWidthAttrib, NULL, NULL, IUPAF_NO_SAVE | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "CARETCOLOR", iScintillaGetCaretColorAttrib, iScintillaSetCaretColorAttrib, NULL, NULL, IUPAF_NO_SAVE | IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "CARET", iScintillaGetCaretAttrib, iScintillaSetCaretAttrib, NULL, NULL, IUPAF_NO_SAVE | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "CARETPOS", iScintillaGetCaretPosAttrib, iScintillaSetCaretPosAttrib, IUPAF_SAMEASSYSTEM, "0", IUPAF_NO_SAVE|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "CARETTOVIEW", NULL, iScintillaSetCaretToViewAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "COUNT", iScintillaGetCountAttrib, NULL, NULL, NULL, IUPAF_READONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "LINECOUNT", iScintillaGetLineCountAttrib, NULL, NULL, NULL, IUPAF_READONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "LINEVALUE", iScintillaGetCurrentLineAttrib, NULL, NULL, NULL, IUPAF_READONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SELECTEDTEXT", iScintillaGetSelectedTextAttrib, iScintillaSetSelectedTextAttrib, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SELECTION", iScintillaGetSelectionAttrib, iScintillaSetSelectionAttrib, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SELECTIONPOS", iScintillaGetSelectionPosAttrib, iScintillaSetSelectionPosAttrib, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SELECTIONFGCOLOR", NULL, iScintillaSetSelectionForeColorAttrib, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SELECTIONBGCOLOR", NULL, iScintillaSetSelectionBackColorAttrib, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SELECTIONALPHA", iScintillaGetSelectionAlphaAttrib, iScintillaSetSelectionAlphaAttrib,  NULL, NULL, IUPAF_NO_SAVE | IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "WORDPOS", iScintillaGetWordPosAttrib, NULL, IUPAF_READONLY | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "WORDRANGE", NULL, NULL, NULL, NULL, IUPAF_NOT_MAPPED | IUPAF_NO_INHERIT);
#ifdef SCI_ISRANGEWORD
  iupClassRegisterAttribute(ic, "ISWORD", iScintillaGetIsWordAttrib, NULL, NULL, NULL, IUPAF_READONLY);
#endif  
  iupClassRegisterAttribute(ic, "VISIBLELINESCOUNT", iScintillaGetVisibleLinesCountAttrib, NULL, NULL, NULL, IUPAF_READONLY | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "FIRSTVISIBLELINE", iScintillaGetFirstVisibleLineAttrib, iScintillaSetFirstVisibleLineAttrib, NULL, NULL, IUPAF_NO_INHERIT);
}
