/** \file
 * \brief Font mapping
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdlib.h> 
#include <string.h> 
#include <stdio.h> 

#include "iup.h"

#include "iup_str.h"
#include "iup_drvfont.h"
#include "iup_assert.h"
#include "iup_attrib.h"
#include "iup_class.h"

typedef struct _IfontNameMap {
  const char* pango;
  const char* x;
  const char* win;
} IfontNameMap;

#define IFONT_NAME_MAP_SIZE 7

static IfontNameMap ifont_name_map[IFONT_NAME_MAP_SIZE] = {
  {"sans",      "helvetica",              "arial"},
  {NULL,        "new century schoolbook", "century schoolbook"},
  {"monospace", "courier",                "courier new"},
  {NULL,        "lucida",                 "lucida sans unicode"},
  {NULL,        "lucidabright",           "lucida bright"},
  {NULL,        "lucidatypewriter",       "lucida console"},
  {"serif",     "times",                  "times new roman"}
};

const char* iupFontGetPangoName(const char* name)
{
  int i;
  if (!name)
    return NULL;
  for (i=0; i<IFONT_NAME_MAP_SIZE; i++)
  {
    if (iupStrEqualNoCase(ifont_name_map[i].win, name))
      return ifont_name_map[i].pango;
    if (iupStrEqualNoCase(ifont_name_map[i].x, name))
      return ifont_name_map[i].pango;
  }

  return NULL;
}

const char* iupFontGetWinName(const char* name)
{
  int i;
  if (!name)
    return NULL;
  for (i=0; i<IFONT_NAME_MAP_SIZE; i++)
  {
    if (iupStrEqualNoCase(ifont_name_map[i].pango, name))
      return ifont_name_map[i].win;
    if (iupStrEqualNoCase(ifont_name_map[i].x, name))
      return ifont_name_map[i].win;
  }

  return NULL;
}

const char* iupFontGetXName(const char* name)
{
  int i;
  if (!name)
    return NULL;
  for (i=0; i<IFONT_NAME_MAP_SIZE; i++)
  {
    if (iupStrEqualNoCase(ifont_name_map[i].win, name))
      return ifont_name_map[i].x;
    if (iupStrEqualNoCase(ifont_name_map[i].pango, name))
      return ifont_name_map[i].x;
  }

  return NULL;
}

char *IupUnMapFont(const char *standardfont)
{
  int size = 0;
  int is_bold = 0,
    is_italic = 0, 
    is_underline = 0,
    is_strikeout = 0;
  char typeface[1024];
  char *iup_typeface, *iup_style;

  iupASSERT(standardfont!=NULL);
  if (!standardfont)
    return NULL;

  if (!iupFontParsePango(standardfont, typeface, &size, &is_bold, &is_italic, &is_underline, &is_strikeout))
    return NULL;

  if (strstr(typeface, "Helvetica"))
    iup_typeface = "HELVETICA_";
  else if (strstr(typeface, "Courier"))
    iup_typeface = "COURIER_";
  else if (strstr(typeface, "Times"))
    iup_typeface = "TIMES_";
  else
    return NULL;

  if (!is_bold && !is_italic)
    iup_style = "NORMAL_";
  else if (!is_bold && is_italic)
    iup_style = "ITALIC_";
  else if (is_bold && !is_italic)
    iup_style = "BOLD_";
  else
    return NULL;

  return iupStrReturnStrf("%s%s%d", iup_typeface, iup_style, size);
}

static char* iFontGetStyle(const char* iupfont, int *size)
{
  char* style = NULL;

  if (strstr(iupfont, "NORMAL_"))
  {
    style = "";
    iupfont += strlen("NORMAL_");
  }
  else if (strstr(iupfont, "ITALIC_"))
  {
    style = "Italic";
    iupfont += strlen("ITALIC_");
  }
  else if (strstr(iupfont, "BOLD_"))
  {
    style = "Bold";
    iupfont += strlen("BOLD_");
  }
  else 
    return NULL;

  *size = atoi(iupfont);
  return style;
}

char *IupMapFont(const char *iupfont)
{
  int size = 0;
  char *typeface, *style;

  iupASSERT(iupfont!=NULL);
  if (!iupfont)
    return NULL;

  if (strstr(iupfont, "HELVETICA_"))
  {
    typeface = "Helvetica";
    style = iFontGetStyle(iupfont+strlen("HELVETICA_"), &size);
    if (!style || size==0)
      return NULL;
  }
  else if (strstr(iupfont, "COURIER_"))
  {
    typeface = "Courier";
    style = iFontGetStyle(iupfont+strlen("COURIER_"), &size);
    if (!style || size==0)
      return NULL;
  }
  else if (strstr(iupfont, "TIMES_"))
  {
    typeface = "Times";
    style = iFontGetStyle(iupfont+strlen("TIMES_"), &size);
    if (!style || size==0)
      return NULL;
  }
  else 
    return NULL;

  return iupStrReturnStrf("%s, %s %d", typeface, style, size);
}

int iupSetFontAttrib(Ihandle* ih, const char* value)
{
  /* when set FONT can be an OLD IUP Font or a Native font */
  const char* standardfont = IupMapFont(value);
  if (!standardfont)
    standardfont = value;

  IupStoreAttribute(ih, "STANDARDFONT", standardfont);
  return 0;
}

char* iupGetFontAttrib(Ihandle* ih)
{
  return iupAttribGetStr(ih, "STANDARDFONT");
}

void iupUpdateStandardFontAttrib(Ihandle* ih)
{
  iupAttribSetClassObject(ih, "STANDARDFONT", iupGetFontAttrib(ih));
}

int iupGetFontInfo(const char* standardfont, char *typeface, int *size, int *is_bold, int *is_italic, int *is_underline, int *is_strikeout)
{
  if (size) *size = 0;
  if (is_bold) *is_bold = 0;
  if (is_italic) *is_italic = 0; 
  if (is_underline) *is_underline = 0;
  if (is_strikeout) *is_strikeout = 0;
  if (typeface) *typeface = 0;
  
  /* parse the old Windows format first */
  if (!iupFontParseWin(standardfont, typeface, size, is_bold, is_italic, is_underline, is_strikeout))
  {
    if (!iupFontParseX(standardfont, typeface, size, is_bold, is_italic, is_underline, is_strikeout))
    {
      if (!iupFontParsePango(standardfont, typeface, size, is_bold, is_italic, is_underline, is_strikeout))
        return 0;
    }
  }

  return 1;
}

char* iupGetFontFaceAttrib(Ihandle* ih)
{
  int size = 0;
  int is_bold = 0,
    is_italic = 0, 
    is_underline = 0,
    is_strikeout = 0;
  char typeface[1024];
  char* standardfont; 
  
  standardfont = iupGetFontAttrib(ih);

  if (!iupGetFontInfo(standardfont, typeface, &size, &is_bold, &is_italic, &is_underline, &is_strikeout))
    return NULL;

  return iupStrReturnStr(typeface);
}

int iupSetFontFaceAttrib(Ihandle* ih, const char* value)
{
  int size = 0;
  int is_bold = 0,
    is_italic = 0,
    is_underline = 0,
    is_strikeout = 0;
  char typeface[1024];
  char* standardfont;

  if (!value)
    return 0;

  standardfont = iupGetFontAttrib(ih);

  if (!iupGetFontInfo(standardfont, typeface, &size, &is_bold, &is_italic, &is_underline, &is_strikeout))
    return 0;

  IupSetfAttribute(ih, "STANDARDFONT", "%s, %s%s%s%s %d", value, is_bold ? "Bold " : "", is_italic ? "Italic " : "", is_underline ? "Underline " : "", is_strikeout ? "Strikeout " : "", size);
  return 0;
}

char* iupGetFontSizeAttrib(Ihandle* ih)
{
  int size = 0;
  int is_bold = 0,
    is_italic = 0, 
    is_underline = 0,
    is_strikeout = 0;
  char typeface[1024];
  char* standardfont; 
  
  standardfont = iupGetFontAttrib(ih);

  if (!iupGetFontInfo(standardfont, typeface, &size, &is_bold, &is_italic, &is_underline, &is_strikeout))
    return NULL;

  return iupStrReturnInt(size);
}

int iupSetFontSizeAttrib(Ihandle* ih, const char* value)
{
  int size = 0;
  int is_bold = 0,
    is_italic = 0, 
    is_underline = 0,
    is_strikeout = 0;
  char typeface[1024];
  char* standardfont; 

  if (!value)
    return 0;
  
  standardfont = iupGetFontAttrib(ih);

  if (!iupGetFontInfo(standardfont, typeface, &size, &is_bold, &is_italic, &is_underline, &is_strikeout))
    return 0;

  IupSetfAttribute(ih, "STANDARDFONT", "%s, %s%s%s%s %s", typeface, is_bold?"Bold ":"", is_italic?"Italic ":"", is_underline?"Underline ":"", is_strikeout?"Strikeout ":"", value);
  return 0;
}

char* iupGetDefaultFontFaceGlobalAttrib(void)
{
  int size = 0;
  int is_bold = 0,
    is_italic = 0,
    is_underline = 0,
    is_strikeout = 0;
  char typeface[1024];

  char* standardfont = IupGetGlobal("DEFAULTFONT");

  if (!iupGetFontInfo(standardfont, typeface, &size, &is_bold, &is_italic, &is_underline, &is_strikeout))
    return NULL;

  return iupStrReturnStr(typeface);
}

int iupSetDefaultFontFaceGlobalAttrib(const char* value)
{
  int size = 0;
  int is_bold = 0,
    is_italic = 0,
    is_underline = 0,
    is_strikeout = 0;
  char typeface[1024];
  char* standardfont;

  if (!value)
    return 0;

  standardfont = IupGetGlobal("DEFAULTFONT");

  if (!iupGetFontInfo(standardfont, typeface, &size, &is_bold, &is_italic, &is_underline, &is_strikeout))
    return 0;

  IupSetfAttribute(NULL, "DEFAULTFONT", "%s, %s%s%s%s %d", value, is_bold ? "Bold " : "", is_italic ? "Italic " : "", is_underline ? "Underline " : "", is_strikeout ? "Strikeout " : "", size);

  return 0;
}

char* iupGetDefaultFontStyleGlobalAttrib(void)
{
  int size = 0;
  int is_bold = 0,
    is_italic = 0,
    is_underline = 0,
    is_strikeout = 0;
  char typeface[1024];

  char* standardfont = IupGetGlobal("DEFAULTFONT");

  if (!iupGetFontInfo(standardfont, typeface, &size, &is_bold, &is_italic, &is_underline, &is_strikeout))
    return NULL;

  return iupStrReturnStrf("%s%s%s%s", is_bold ? "Bold " : "", is_italic ? "Italic " : "", is_underline ? "Underline " : "", is_strikeout ? "Strikeout " : "");
}

int iupSetDefaultFontStyleGlobalAttrib(const char* value)
{
  int size = 0;
  int is_bold = 0,
    is_italic = 0,
    is_underline = 0,
    is_strikeout = 0;
  char typeface[1024];
  char* standardfont;

  if (!value)
    return 0;

  standardfont = IupGetGlobal("DEFAULTFONT");

  if (!iupGetFontInfo(standardfont, typeface, &size, &is_bold, &is_italic, &is_underline, &is_strikeout))
    return 0;

  IupSetfAttribute(NULL, "DEFAULTFONT", "%s, %s %d", typeface, value, size);

  return 0;
}

void iupSetDefaultFontSizeGlobalAttrib(const char* value)
{
  int size = 0;
  int is_bold = 0,
    is_italic = 0, 
    is_underline = 0,
    is_strikeout = 0;
  char typeface[1024];
  char* standardfont; 

  if (!value)
    return;
  
  standardfont = IupGetGlobal("DEFAULTFONT");

  if (!iupGetFontInfo(standardfont, typeface, &size, &is_bold, &is_italic, &is_underline, &is_strikeout))
    return;

  IupSetfAttribute(NULL, "DEFAULTFONT", "%s, %s%s%s%s %s", typeface, is_bold?"Bold ":"", is_italic?"Italic ":"", is_underline?"Underline ":"", is_strikeout?"Strikeout ":"", value);

  return;
}

char* iupGetDefaultFontSizeGlobalAttrib(void)
{
  int size = 0;
  int is_bold = 0,
    is_italic = 0, 
    is_underline = 0,
    is_strikeout = 0;
  char typeface[1024];

  char* standardfont = IupGetGlobal("DEFAULTFONT");

  if (!iupGetFontInfo(standardfont, typeface, &size, &is_bold, &is_italic, &is_underline, &is_strikeout))
    return NULL;

  return iupStrReturnInt(size);
}

char* iupGetFontStyleAttrib(Ihandle* ih)
{
  int size = 0;
  int is_bold = 0,
    is_italic = 0, 
    is_underline = 0,
    is_strikeout = 0;
  char typeface[1024];
  char* standardfont = iupGetFontAttrib(ih);

  if (!iupGetFontInfo(standardfont, typeface, &size, &is_bold, &is_italic, &is_underline, &is_strikeout))
    return NULL;

  return iupStrReturnStrf("%s%s%s%s", is_bold?"Bold ":"", is_italic?"Italic ":"", is_underline?"Underline ":"", is_strikeout?"Strikeout ":"");
}

int iupSetFontStyleAttrib(Ihandle* ih, const char* value)
{
  int size = 0;
  int is_bold = 0,
    is_italic = 0, 
    is_underline = 0,
    is_strikeout = 0;
  char typeface[1024];
  char* standardfont; 

  if (!value)
    return 0;
  
  standardfont = iupGetFontAttrib(ih);

  if (!iupGetFontInfo(standardfont, typeface, &size, &is_bold, &is_italic, &is_underline, &is_strikeout))
    return 0;

  IupSetfAttribute(ih, "STANDARDFONT", "%s, %s %d", typeface, value, size);

  return 0;
}

/**************************************************************/
/* Native Font Format, compatible with Pango Font Description */
/**************************************************************/

/*
The string contains the font name, the style and the size. 
Style can be a free combination of some names separated by spaces.
Font name can be a list of font family names separated by comma.
*/

enum {                          /* style */
 FONT_PLAIN  = 0,
 FONT_BOLD   = 1,
 FONT_ITALIC = 2,
 FONT_UNDERLINE = 4,
 FONT_STRIKEOUT = 8
};

/* this code is shared between CD and IUP, must be updated on both libraries */
static int iFontFindStyleName(const char *name, int len, int *style)
{
#define STYLE_NUM_NAMES 21
  static struct { const char* name; int style; } style_names[STYLE_NUM_NAMES] = {
    {"Normal",         0},
    {"Oblique",        FONT_ITALIC},
    {"Italic",         FONT_ITALIC},
    {"Small-Caps",     0},
    {"Ultra-Light",    0},
    {"Light",          0},
    {"Medium",         0},
    {"Semi-Bold",      FONT_BOLD},
    {"Bold",           FONT_BOLD},
    {"Ultra-Bold",     FONT_BOLD},
    {"Heavy",          0},
    {"Ultra-Condensed",0},
    {"Extra-Condensed",0},
    {"Condensed",      0},
    {"Semi-Condensed", 0},
    {"Semi-Expanded",  0},
    {"Expanded",       0},
    {"Extra-Expanded", 0},
    {"Ultra-Expanded", 0},
    {"Underline", FONT_UNDERLINE},
    {"Strikeout", FONT_STRIKEOUT}
  };

  int i;
  for (i = 0; i < STYLE_NUM_NAMES; i++)
  {
    /* iupStrEqualPartial(style_names[i].name, name) */
    if (strncmp(style_names[i].name, name, len)==0)
    {
      *style = style_names[i].style;
      return 1;
    }
  }

  return 0;
}

#define is_style_sep(_x) (_x == ' ' || _x == ',')

/* this code is shared between CD and IUP, must be updated on both libraries */
static const char * iFontGetStyleWord(const char *str, const char *last, int *wordlen)
{
  const char *result;
  
  while (last > str && is_style_sep(*(last - 1)))
    last--;

  result = last;
  while (result > str && !is_style_sep(*(result - 1)))
    result--;

  *wordlen = (int)(last - result);
  
  return result;
}

/* this code is shared between CD and IUP, must be updated on both libraries */
int iupFontParsePango(const char *standardfont, char *typeface, int *size, int *bold, int *italic, int *underline, int *strikeout)
{
  const char *p, *last;
  int len, wordlen, style;

  if (standardfont[0] == '-')  /* X font, abort */
    return 0;

  len = (int)strlen(standardfont);
  last = standardfont + len;
  p = iFontGetStyleWord(standardfont, last, &wordlen);

  /* Look for a size at the end of the string */
  if (wordlen != 0)
  {
    int new_size = atoi(p);
    if (new_size != 0)
    {
      *size = new_size;
      last = p;
    }
  }

  /* Now parse style words */
  style = 0;
  p = iFontGetStyleWord(standardfont, last, &wordlen);
  while (wordlen != 0)
  {
    int new_style = 0;

    if (!iFontFindStyleName(p, wordlen, &new_style))
      break;
    else
    {
      style |= new_style;

      last = p;
      p = iFontGetStyleWord(standardfont, last, &wordlen);
    }
  }

  *bold = 0;
  *italic = 0;
  *underline = 0;
  *strikeout = 0;

  if (style&FONT_BOLD)
    *bold = 1;
  if (style&FONT_ITALIC)
    *italic = 1;
  if (style&FONT_UNDERLINE)
    *underline = 1;
  if (style&FONT_STRIKEOUT)
    *strikeout = 1;

  /* Remainder is font family list. */

  /* Trim off trailing separators */
  while (last > standardfont && is_style_sep(*(last - 1)))
    last--;

  /* Trim off leading separators */
  while (last > standardfont && is_style_sep(*standardfont))
    standardfont++;

  if (standardfont != last)
  {
    len = (int)(last - standardfont);
    strncpy(typeface, standardfont, len);
    typeface[len] = 0;
    return 1;
  }
  else
    return 0;
}

/* this code is shared between CD and IUP, must be updated on both libraries */
int iupFontParseWin(const char *value, char *typeface, int *size, int *bold, int *italic, int *underline, int *strikeout)
{
  int c;

  if (value[0] == '-')  /* X font, abort */
    return 0;

  if (strstr(value, ":") == NULL)  /* check for the first separator */
    return 0;

  if (value[0] == ':')  /* check for NO typeface */
    value++;       /* skip separator */
  else
  {
    c = (int)strcspn(value, ":");      /* extract typeface */
    if (c == 0) return 0;
    strncpy(typeface, value, c);
    typeface[c] = '\0';
    value += c+1;  /* skip typeface and separator */
  }

  if (strstr(value, ":") == NULL)  /* check for the second separator */
    return 0;

  /* init style to none */
  *bold = 0;
  *italic = 0;
  *underline = 0;
  *strikeout = 0;

  if (value[0] == ':')  /* check for NO style list */
    value++;       /* skip separator */
  else
  {
    while (strlen(value)) /* extract style (bold/italic etc) */
    {
      char style[30];

      c = (int)strcspn(value, ":,");
      if (c == 0)
        break;

      strncpy(style, value, c);
      style[c] = '\0';

      if(iupStrEqual(style, "BOLD"))
        *bold = 1; 
      else if(iupStrEqual(style,"ITALIC"))
        *italic = 1; 
      else if(iupStrEqual(style,"UNDERLINE"))
        *underline = 1; 
      else if(iupStrEqual(style,"STRIKEOUT"))
        *strikeout = 1; 

      value += c; /* skip only the style */

      if(value[0] == ':')  /* end of style list */
      {
        value++;
        break;
      }

      value++;   /* skip separator */
    }
  }

  /* extract size in points */
  if (!iupStrToInt(value, size))
    return 0;

  if (size == 0)
    return 0;

  return 1;
}

/* this code is shared between CD and IUP, must be updated on both libraries */
int iupFontParseX(const char *standardfont, char *typeface, int *size, int *bold, int *italic, int *underline, int *strikeout)
{
  char style1[30], style2[30];
  char* token;
  char font[1024];

  if (standardfont[0] != '-')
    return 0;

  strcpy(font, standardfont+1);  /* skip first '-' */

  *bold = 0;
  *italic = 0;
  *underline = 0;
  *strikeout = 0;

  /* fndry */
  token = strtok(font, "-");
  if (!token) return 0;

  /* fmly */
  token = strtok(NULL, "-");
  if (!token) return 0;
  strcpy(typeface, token);

  /* wght */
  token = strtok(NULL, "-");
  if (!token) return 0;
  strcpy(style1, token);
  if (strstr("bold", style1))
    *bold = 1;

  /* slant */
  token = strtok(NULL, "-");
  if (!token) return 0;
  strcpy(style2, token);
  if (*style2 == 'i' || *style2 == 'o')
    *italic = 1;

  /* sWdth */
  token = strtok(NULL, "-");
  if (!token) return 0;
  /* adstyl */
  token = strtok(NULL, "-");
  if (!token) return 0;

  /* pxlsz */
  token = strtok(NULL, "-");
  if (!token) return 0;
  *size = -atoi(token); /* size in pixels */

  if (*size < 0)
    return 1;

  /* ptSz */
  token = strtok(NULL, "-");
  if (!token) return 0;
  *size = atoi(token)/10; /* size in deci-points */

  if (*size > 0)
    return 1;

  return 0;
}
