/** \file
 * \brief Scintilla control: Style Definition
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


#include <Scintilla.h>

#include "iup.h"
#include "iup_drvfont.h"

#include "iup_object.h"
#include "iup_attrib.h"
#include "iup_str.h"

#include "iupsci.h"


/***** STYLE DEFINITION *****
SCI_STYLERESETDEFAULT
SCI_STYLECLEARALL
SCI_STYLESETFONT(int styleNumber, char *fontName)
SCI_STYLEGETFONT(int styleNumber, char *fontName)
SCI_STYLESETSIZE(int styleNumber, int sizeInPoints)
SCI_STYLEGETSIZE(int styleNumber)
SCI_STYLESETSIZEFRACTIONAL(int styleNumber, int sizeInHundredthPoints)
SCI_STYLEGETSIZEFRACTIONAL(int styleNumber)
SCI_STYLESETBOLD(int styleNumber, bool bold)
SCI_STYLEGETBOLD(int styleNumber)
SCI_STYLESETWEIGHT(int styleNumber, int weight)
SCI_STYLEGETWEIGHT(int styleNumber)
SCI_STYLESETITALIC(int styleNumber, bool italic)
SCI_STYLEGETITALIC(int styleNumber)
SCI_STYLESETUNDERLINE(int styleNumber, bool underline)
SCI_STYLEGETUNDERLINE(int styleNumber)
SCI_STYLESETFORE(int styleNumber, int colour)
SCI_STYLEGETFORE(int styleNumber)
SCI_STYLESETBACK(int styleNumber, int colour)
SCI_STYLEGETBACK(int styleNumber)
SCI_STYLESETEOLFILLED(int styleNumber, bool eolFilled)
SCI_STYLEGETEOLFILLED(int styleNumber)
SCI_STYLESETCHARACTERSET(int styleNumber, int charSet)
SCI_STYLEGETCHARACTERSET(int styleNumber)
SCI_STYLESETCASE(int styleNumber, int caseMode)
SCI_STYLEGETCASE(int styleNumber)
SCI_STYLESETVISIBLE(int styleNumber, bool visible)
SCI_STYLEGETVISIBLE(int styleNumber)
--SCI_STYLESETCHANGEABLE(int styleNumber, bool changeable)
--SCI_STYLEGETCHANGEABLE(int styleNumber)
SCI_STYLESETHOTSPOT(int styleNumber, bool hotspot)
SCI_STYLEGETHOTSPOT(int styleNumber)*/

static char* iScintillaGetCaseStyleAttrib(Ihandle* ih, int style)
{
  int caseSty;

  if(style == IUP_INVALID_ID)
    style = 0;  /* Lexer style default */
  
  caseSty = (int)IupScintillaSendMessage(ih, SCI_STYLEGETCASE, style, 0);
  
  if(caseSty == SC_CASE_UPPER)
    return "UPPERCASE";
  else if(caseSty == SC_CASE_LOWER)
    return "LOWERCASE";
  else
    return "SC_CASE_MIXED";
}

static int iScintillaSetCaseStyleAttrib(Ihandle* ih, int style, const char* value)
{
  if(style == IUP_INVALID_ID)
    style = 0;  /* Lexer style default */
  
  if (iupStrEqualNoCase(value, "UPPERCASE"))
    IupScintillaSendMessage(ih, SCI_STYLESETCASE, style, SC_CASE_UPPER);
  else if (iupStrEqualNoCase(value, "LOWERCASE"))
    IupScintillaSendMessage(ih, SCI_STYLESETCASE, style, SC_CASE_LOWER);
  else
    IupScintillaSendMessage(ih, SCI_STYLESETCASE, style, SC_CASE_MIXED);  /* default */

  return 0;
}

static char* iScintillaGetVisibleStyleAttrib(Ihandle* ih, int style)
{
  if(style == IUP_INVALID_ID)
    style = 0;  /* Lexer style default */

  return iupStrReturnBoolean((int)IupScintillaSendMessage(ih, SCI_STYLEGETVISIBLE, style, 0)); 
}

static int iScintillaSetVisibleStyleAttrib(Ihandle* ih, int style, const char* value)
{
  if(style == IUP_INVALID_ID)
    style = 0;  /* Lexer style default */

  if (iupStrBoolean(value))
    IupScintillaSendMessage(ih, SCI_STYLESETVISIBLE, style, 1);
  else
    IupScintillaSendMessage(ih, SCI_STYLESETVISIBLE, style, 0);

  return 0;
}

static char* iScintillaGetHotSpotStyleAttrib(Ihandle* ih, int style)
{
  if(style == IUP_INVALID_ID)
    style = 0;  /* Lexer style default */

  return iupStrReturnBoolean((int)IupScintillaSendMessage(ih, SCI_STYLEGETHOTSPOT, style, 0)); 
}

static int iScintillaSetHotSpotStyleAttrib(Ihandle* ih, int style, const char* value)
{
  if(style == IUP_INVALID_ID)
    style = 0;  /* Lexer style default */

  if (iupStrBoolean(value))
    IupScintillaSendMessage(ih, SCI_STYLESETHOTSPOT, style, 1);
  else
    IupScintillaSendMessage(ih, SCI_STYLESETHOTSPOT, style, 0);

  return 0;
}

static char* iScintillaGetCharSetStyleAttrib(Ihandle* ih, int style)
{
  int charset;
  
  if(style == IUP_INVALID_ID)
    style = 0;  /* Lexer style default */
  
  charset = (int)IupScintillaSendMessage(ih, SCI_STYLEGETCHARACTERSET, style, 0);

  if(charset == SC_CHARSET_EASTEUROPE)
    return "EASTEUROPE";
  else if(charset == SC_CHARSET_RUSSIAN)
    return "RUSSIAN";
  else if(charset == SC_CHARSET_GB2312)
    return "GB2312";
  else if(charset == SC_CHARSET_HANGUL)
    return "HANGUL";
  else if(charset == SC_CHARSET_SHIFTJIS)
    return "SHIFTJIS";
  else
    return "ANSI";
}

static int iScintillaSetCharSetStyleAttrib(Ihandle* ih, int style, const char* value)
{
  if(style == IUP_INVALID_ID)
    style = 0;  /* Lexer style default */

  /* These character sets are supported both on Windows as on GTK */
  if (iupStrEqualNoCase(value, "EASTEUROPE"))
    IupScintillaSendMessage(ih, SCI_STYLESETCHARACTERSET, style, SC_CHARSET_EASTEUROPE);
  else if (iupStrEqualNoCase(value, "RUSSIAN"))
    IupScintillaSendMessage(ih, SCI_STYLESETCHARACTERSET, style, SC_CHARSET_RUSSIAN);
  else if (iupStrEqualNoCase(value, "GB2312"))  /* Chinese charset */
    IupScintillaSendMessage(ih, SCI_STYLESETCHARACTERSET, style, SC_CHARSET_GB2312);
  else if (iupStrEqualNoCase(value, "HANGUL"))  /* Korean charset */
    IupScintillaSendMessage(ih, SCI_STYLESETCHARACTERSET, style, SC_CHARSET_HANGUL);
  else if (iupStrEqualNoCase(value, "SHIFTJIS"))  /* Japanese charset */
    IupScintillaSendMessage(ih, SCI_STYLESETCHARACTERSET, style, SC_CHARSET_SHIFTJIS);
  else
    IupScintillaSendMessage(ih, SCI_STYLESETCHARACTERSET, style, SC_CHARSET_ANSI);  /* default */

  return 0;
}

static char* iScintillaGetEolFilledStyleAttrib(Ihandle* ih, int style)
{
  if(style == IUP_INVALID_ID)
    style = 0;  /* Lexer style default */

  return iupStrReturnBoolean((int)IupScintillaSendMessage(ih, SCI_STYLEGETEOLFILLED, style, 0)); 
}

static int iScintillaSetEolFilledStyleAttrib(Ihandle* ih, int style, const char* value)
{
  if(style == IUP_INVALID_ID)
    style = 0;  /* Lexer style default */

  if (iupStrBoolean(value))
    IupScintillaSendMessage(ih, SCI_STYLESETEOLFILLED, style, 1);
  else
    IupScintillaSendMessage(ih, SCI_STYLESETEOLFILLED, style, 0);

  return 0;
}

static char* iScintillaGetFontSizeFracStyleAttrib(Ihandle* ih, int style)
{
  int size;

  if(style == IUP_INVALID_ID)
    style = 0;  /* Lexer style default */

  size = (int)IupScintillaSendMessage(ih, SCI_STYLESETSIZEFRACTIONAL, style, 0);

  return iupStrReturnDouble((double)size / (double)SC_FONT_SIZE_MULTIPLIER);
}

static int iScintillaSetFontSizeFracStyleAttrib(Ihandle* ih, int style, const char* value)
{
  double size;

  if(style == IUP_INVALID_ID)
    style = 0;  /* Lexer style default */

  iupStrToDouble(value, &size);

  IupScintillaSendMessage(ih, SCI_STYLESETSIZEFRACTIONAL, style, (int)(size*SC_FONT_SIZE_MULTIPLIER));

  return 0;
}

static char* iScintillaGetFontSizeStyleAttrib(Ihandle* ih, int style)
{
  int size;

  if(style == IUP_INVALID_ID)
    style = 0;  /* Lexer style default */

  size = (int)IupScintillaSendMessage(ih, SCI_STYLEGETSIZE, style, 0);
  return iupStrReturnInt(size);
}

static int iScintillaSetFontSizeStyleAttrib(Ihandle* ih, int style, const char* value)
{
  int size;

  if(style == IUP_INVALID_ID)
    style = 0;  /* Lexer style default */

  iupStrToInt(value, &size);

  IupScintillaSendMessage(ih, SCI_STYLESETSIZE, style, size);

  return 0;
}

static char* iScintillaGetFontStyleAttrib(Ihandle* ih, int style)
{
  char* str = iupStrGetMemory(15);

  if(style == IUP_INVALID_ID)
    style = 0;  /* Lexer style default */

  IupScintillaSendMessage(ih, SCI_STYLEGETFONT, style, (sptr_t)str);

  return str;
}

static int iScintillaSetFontStyleAttrib(Ihandle* ih, int style, const char* value)
{
  if(style == IUP_INVALID_ID)
    style = 0;  /* Lexer style default */

  IupScintillaSendMessage(ih, SCI_STYLESETFONT, style, (sptr_t)value);

  return 0;
}

static char* iScintillaGetWeightStyleAttrib(Ihandle* ih, int style)
{
  int weight = (int)IupScintillaSendMessage(ih, SCI_STYLEGETWEIGHT, style, 0);

  if(style == IUP_INVALID_ID)
    style = 0;  /* Lexer style default */

  weight = (int)IupScintillaSendMessage(ih, SCI_STYLEGETWEIGHT, style, 0);
  return iupStrReturnInt(weight);
}

static int iScintillaSetWeightStyleAttrib(Ihandle* ih, int style, const char* value)
{
  int weight = 0;

  if(style == IUP_INVALID_ID)
    style = 0;  /* Lexer style default */

  if (!value || value[0]==0 || iupStrEqualNoCase(value, "NORMAL"))
    weight = 400;
  else if (iupStrEqualNoCase(value, "SEMIBOLD"))
    weight = 600;
  else if (iupStrEqualNoCase(value, "BOLD"))
    weight = 700;
  else
  {
    iupStrToInt(value, &weight);
    if(weight < 1)
      weight = 1;
    if(weight > 999)
      weight = 999;
  }

  IupScintillaSendMessage(ih, SCI_STYLESETWEIGHT, style, weight);

  return 0;
}

static char* iScintillaGetUnderlineStyleAttrib(Ihandle* ih, int style)
{
  if(style == IUP_INVALID_ID)
    style = 0;  /* Lexer style default */

  return iupStrReturnBoolean((int)IupScintillaSendMessage(ih, SCI_STYLEGETUNDERLINE, style, 0)); 
}

static int iScintillaSetUnderlineStyleAttrib(Ihandle* ih, int style, const char* value)
{
  if(style == IUP_INVALID_ID)
    style = 0;  /* Lexer style default */

  if (iupStrBoolean(value))
    IupScintillaSendMessage(ih, SCI_STYLESETUNDERLINE, style, 1);
  else
    IupScintillaSendMessage(ih, SCI_STYLESETUNDERLINE, style, 0);

  return 0;
}

static char* iScintillaGetItalicStyleAttrib(Ihandle* ih, int style)
{
  if(style == IUP_INVALID_ID)
    style = 0;  /* Lexer style default */

  return iupStrReturnBoolean((int)IupScintillaSendMessage(ih, SCI_STYLEGETITALIC, style, 0)); 
}

static int iScintillaSetItalicStyleAttrib(Ihandle* ih, int style, const char* value)
{
  if(style == IUP_INVALID_ID)
    style = 0;  /* Lexer style default */

  if (iupStrBoolean(value))
    IupScintillaSendMessage(ih, SCI_STYLESETITALIC, style, 1);
  else
    IupScintillaSendMessage(ih, SCI_STYLESETITALIC, style, 0);

  return 0;
}

static char* iScintillaGetBoldStyleAttrib(Ihandle* ih, int style)
{
  if(style == IUP_INVALID_ID)
    style = 0;  /* Lexer style default */

  return iupStrReturnBoolean((int)IupScintillaSendMessage(ih, SCI_STYLEGETBOLD, style, 0)); 
}

static int iScintillaSetBoldStyleAttrib(Ihandle* ih, int style, const char* value)
{
  if(style == IUP_INVALID_ID)
    style = 0;  /* Lexer style default */

  if (iupStrBoolean(value))
    IupScintillaSendMessage(ih, SCI_STYLESETBOLD, style, 1);
  else
    IupScintillaSendMessage(ih, SCI_STYLESETBOLD, style, 0);

  return 0;
}

static char* iScintillaGetFgColorStyleAttrib(Ihandle* ih, int style)
{
  long color;
  unsigned char r, g, b;

  if(style == IUP_INVALID_ID)
    style = 0;  /* Lexer style default */

  color = (int)IupScintillaSendMessage(ih, SCI_STYLEGETFORE, style, 0);
  iupScintillaDecodeColor(color, &r, &g, &b);
  return iupStrReturnRGB(r, g, b);
}

static int iScintillaSetFgColorStyleAttrib(Ihandle* ih, int style, const char* value)
{
  unsigned char r, g, b;

  if (!iupStrToRGB(value, &r, &g, &b))
    return 0;

  if(style == IUP_INVALID_ID)
    style = 0;  /* Lexer style default */

  IupScintillaSendMessage(ih, SCI_STYLESETFORE, style, iupScintillaEncodeColor(r, g, b));

  return 0;
}

static char* iScintillaGetBgColorStyleAttrib(Ihandle* ih, int style)
{
  long color;
  unsigned char r, g, b;

  if(style == IUP_INVALID_ID)
    style = 0;  /* Lexer style default */

  color = (int)IupScintillaSendMessage(ih, SCI_STYLEGETBACK, style, 0);
  iupScintillaDecodeColor(color, &r, &g, &b);
  return iupStrReturnRGB(r, g, b);
}

static int iScintillaSetBgColorStyleAttrib(Ihandle* ih, int style, const char* value)
{
  unsigned char r, g, b;

  if (!iupStrToRGB(value, &r, &g, &b))
    return 0;

  if(style == IUP_INVALID_ID)
    style = 0;  /* Lexer style default */

  IupScintillaSendMessage(ih, SCI_STYLESETBACK, style, iupScintillaEncodeColor(r, g, b));

  return 0;
}

static int iScintillaSetClearAllStyleAttrib(Ihandle* ih, const char* value)
{
  (void)value;

  IupScintillaSendMessage(ih, SCI_STYLECLEARALL, 0, 0);
  return 0;
}

static int iScintillaSetResetDefaultStyleAttrib(Ihandle* ih, const char* value)
{
  (void)value;

  IupScintillaSendMessage(ih, SCI_STYLERESETDEFAULT, 0, 0);
  return 0;
}

static int iScintillaSetStartStylingAttrib(Ihandle *ih, const char *value)
{
  int pos;
  if (iupStrToInt(value, &pos))
    IupScintillaSendMessage(ih, SCI_STARTSTYLING, pos, 0x1f);  /* mask=31 */

  return 0;
}

static int iScintillaSetStylingAttrib(Ihandle* ih, int style, const char* value)
{
  int length;
  if (iupStrToInt(value, &length))
    IupScintillaSendMessage(ih, SCI_SETSTYLING, length, style);
  return 0;
}

static int iScintillaSetFontAttrib(Ihandle* ih, const char* value)
{
  if (!ih->handle)
    return iupdrvSetFontAttrib(ih, value);
  else
  {
    int i, size = 0,
       is_bold = 0,
      is_italic = 0,
      is_underline = 0,
      is_strikeout = 0;
    char typeface[1024];

    if (!iupdrvSetFontAttrib(ih, value))
      return 0;

    if (!iupGetFontInfo(value, typeface, &size, &is_bold, &is_italic, &is_underline, &is_strikeout))
      return 0;

    for (i = 0; i < 256; i++)
    {
      IupScintillaSendMessage(ih, SCI_STYLESETFONT, i, (sptr_t)typeface);
      IupScintillaSendMessage(ih, SCI_STYLESETSIZE, i, size);
      IupScintillaSendMessage(ih, SCI_STYLESETBOLD, i, is_bold);
      IupScintillaSendMessage(ih, SCI_STYLESETITALIC, i, is_italic);
      IupScintillaSendMessage(ih, SCI_STYLESETUNDERLINE, i, is_underline);
    }

    return 1;
  }
}

static int iScintillaSetFgColorAttrib(Ihandle *ih, const char *value)
{
  unsigned char r, g, b;
  long fgcolor;
  int i;

  if (!iupStrToRGB(value, &r, &g, &b))
    return 0;

  fgcolor = iupScintillaEncodeColor(r, g, b);

  for (i = 0; i < 256; i++)
    IupScintillaSendMessage(ih, SCI_STYLESETFORE, i, fgcolor);

  return 1;
}

static int iScintillaSetBgColorAttrib(Ihandle *ih, const char *value)
{
  unsigned char r, g, b;
  long fgcolor;
  int i;

  if (!iupStrToRGB(value, &r, &g, &b))
    return 0;

  fgcolor = iupScintillaEncodeColor(r, g, b);

  for (i = 0; i < 256; i++)
    IupScintillaSendMessage(ih, SCI_STYLESETBACK, i, fgcolor);

  return 1;
}

void iupScintillaRegisterStyle(Iclass* ic)
{
  iupClassRegisterAttribute(ic, "FONT", NULL, iScintillaSetFontAttrib, IUPAF_SAMEASSYSTEM, "DEFAULTFONT", IUPAF_NOT_MAPPED);  /* inherited */
  iupClassRegisterAttribute(ic, "BGCOLOR", NULL, iScintillaSetBgColorAttrib, IUPAF_SAMEASSYSTEM, "TXTBGCOLOR", IUPAF_DEFAULT);  
  iupClassRegisterAttribute(ic, "FGCOLOR", NULL, iScintillaSetFgColorAttrib, IUPAF_SAMEASSYSTEM, "TXTFGCOLOR", IUPAF_DEFAULT);  /* usually black */

  iupClassRegisterAttribute(ic,   "STYLERESET", NULL, iScintillaSetResetDefaultStyleAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic,   "STYLECLEARALL", NULL, iScintillaSetClearAllStyleAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "STYLEFONT", iScintillaGetFontStyleAttrib, iScintillaSetFontStyleAttrib, IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "STYLEFONTSIZE", iScintillaGetFontSizeStyleAttrib, iScintillaSetFontSizeStyleAttrib, IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "STYLEFONTSIZEFRAC", iScintillaGetFontSizeFracStyleAttrib, iScintillaSetFontSizeFracStyleAttrib, IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "STYLEBOLD", iScintillaGetBoldStyleAttrib, iScintillaSetBoldStyleAttrib, IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "STYLEWEIGHT", iScintillaGetWeightStyleAttrib, iScintillaSetWeightStyleAttrib, IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "STYLEITALIC", iScintillaGetItalicStyleAttrib, iScintillaSetItalicStyleAttrib, IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "STYLEUNDERLINE", iScintillaGetUnderlineStyleAttrib, iScintillaSetUnderlineStyleAttrib, IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "STYLEFGCOLOR", iScintillaGetFgColorStyleAttrib, iScintillaSetFgColorStyleAttrib, IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "STYLEBGCOLOR", iScintillaGetBgColorStyleAttrib, iScintillaSetBgColorStyleAttrib, IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "STYLEEOLFILLED", iScintillaGetEolFilledStyleAttrib, iScintillaSetEolFilledStyleAttrib, IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "STYLECHARSET", iScintillaGetCharSetStyleAttrib, iScintillaSetCharSetStyleAttrib, IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "STYLECASE", iScintillaGetCaseStyleAttrib, iScintillaSetCaseStyleAttrib, IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "STYLEVISIBLE", iScintillaGetVisibleStyleAttrib, iScintillaSetVisibleStyleAttrib, IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "STYLEHOTSPOT", iScintillaGetHotSpotStyleAttrib, iScintillaSetHotSpotStyleAttrib, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic,   "STARTSTYLING", NULL, iScintillaSetStartStylingAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "STYLING", NULL, iScintillaSetStylingAttrib, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
}
