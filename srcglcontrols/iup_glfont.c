/** \file
 * \brief GL Font functions.
 *
 * See Copyright Notice in "iup.h"
 */

#ifdef WIN32
#include <windows.h>
#else
#include <iconv.h>
#endif

#include <FTGL/ftgl.h>

#include <stdio.h>
#include <stdlib.h>

#include "iup.h"
#include "iupgl.h"

#include "iup_assert.h"
#include "iup_object.h"
#include "iup_attrib.h"
#include "iup_str.h"
#include "iup_array.h"
#include "iup_drvfont.h"

#include "iup_glcontrols.h"
#include "iup_glfont.h"


typedef struct _IglFont
{
  char filename[10240];
  int size;
  FTGLfont* ftgl_font;
  int charwidth, charheight;
} IglFont;


static char* gl_utf8_buffer = NULL;
static int gl_utf8_buffer_len = 0;


#ifdef WIN32
typedef char regName[MAX_PATH];
static regName* win_fonts = NULL;
static int win_fonts_count = 0;

static int iCompareStr(const void *a, const void *b)
{
  const regName*aa = a;
  const regName*bb = b;
  return strcmp(*aa, *bb);
}

static void winInitFontNames(void)
{
  const CHAR* subkey = "Software\\Microsoft\\Windows NT\\CurrentVersion\\Fonts";
  HKEY hkey = NULL;
  DWORD dwIndex = 0;
  LONG ret;
  DWORD dwValuesCount, dwValueSize;

  ret = RegOpenKeyExA(HKEY_LOCAL_MACHINE, subkey, 0, KEY_READ, &hkey);
  if (ret != ERROR_SUCCESS)
    return;

  ret = RegQueryInfoKey(hkey, NULL, NULL, NULL, NULL, NULL, NULL,
                        &dwValuesCount, NULL, NULL, NULL, NULL);
  if (ret != ERROR_SUCCESS)
    return;

  win_fonts_count = (int)dwValuesCount;
  win_fonts = malloc(sizeof(regName) * win_fonts_count);

  for (dwIndex = 0; dwIndex < dwValuesCount; dwIndex++)
  {
    dwValueSize = MAX_PATH - 1;
    ret = RegEnumValueA(hkey, dwIndex, win_fonts[dwIndex], &dwValueSize, NULL, NULL, NULL, NULL);
    if (ret != ERROR_SUCCESS)
    {
      win_fonts_count = (int)dwIndex;
      break;
    }
  }

  qsort(win_fonts, win_fonts_count, sizeof(regName), iCompareStr);

  RegCloseKey(hkey);
}

static int winReadStringKey(HKEY base_key, const char* subkey, const char* value_name, char* value)
{
  HKEY key;
  DWORD max_size = 1024;

  if (RegOpenKeyExA(base_key, subkey, 0, KEY_READ, &key) != ERROR_SUCCESS)
    return 0;

  if (RegQueryValueExA(key, value_name, NULL, NULL, (LPBYTE)value, &max_size) != ERROR_SUCCESS)
  {
    RegCloseKey(key);
    return 0;
  }

  RegCloseKey(key);
  return 1;
}

static char* winGetFontDir(void)
{
  static char font_dir[1024];
  if (!winReadStringKey(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders", "Fonts", font_dir))
    return "";
  else
    return font_dir;
}

static int iGLGetFontFilenameFromSystem(const char *font_name, int is_bold, int is_italic, char* fileName)
{
  CHAR szData[1024];
  const CHAR* subkey = "Software\\Microsoft\\Windows NT\\CurrentVersion\\Fonts";
  char localFontName[MAX_PATH];
  int i;

  if (iupStrEqualNoCase(font_name, "Courier") || iupStrEqualNoCase(font_name, "Monospace"))
    font_name = "Courier New";
  else if (iupStrEqualNoCase(font_name, "Times") || iupStrEqualNoCase(font_name, "Serif"))
    font_name = "Times New Roman";
  else if (iupStrEqualNoCase(font_name, "Helvetica") || iupStrEqualNoCase(font_name, "Sans"))
    font_name = "Arial";

  strcpy(localFontName, font_name);

  if (is_bold)
    strcat(localFontName, " Bold");

  if (is_italic)
    strcat(localFontName, " Italic");

  for (i = 0; i < win_fonts_count; i++)
  {
    if (iupStrEqualNoCasePartial(win_fonts[i], localFontName))
    {
      if (winReadStringKey(HKEY_LOCAL_MACHINE, subkey, win_fonts[i], szData))
      {
        /* szData already includes file extension */
        sprintf(fileName, "%s\\%s", winGetFontDir(), szData);
        return 1;
      }
    }
  }

  return 0;
}
#else
#ifndef NO_FONTCONFIG
#include <fontconfig/fontconfig.h>

static int iGLGetFontFilenameFromSystem(const char *font_name, int is_bold, int is_italic, char* fileName)
{
  char styles[4][20];
  int style_size;
  FcObjectSet *os = 0;
  FcFontSet *fs;
  FcPattern *pat;
  int bResult = 0;

  if (iupStrEqualNoCase(font_name, "Courier") || iupStrEqualNoCase(font_name, "Courier New") || iupStrEqualNoCase(font_name, "Monospace"))
    font_name = "freemono";
  else if (iupStrEqualNoCase(font_name, "Times") || iupStrEqualNoCase(font_name, "Times New Roman") || iupStrEqualNoCase(font_name, "Serif"))
    font_name = "freeserif";
  else if (iupStrEqualNoCase(font_name, "Helvetica") || iupStrEqualNoCase(font_name, "Arial") || iupStrEqualNoCase(font_name, "Sans"))
    font_name = "freesans";

  if (is_bold && is_italic)
  {
    strcpy(styles[0], "BoldItalic");
    strcpy(styles[1], "Bold Italic");
    strcpy(styles[2], "Bold Oblique");
    strcpy(styles[3], "BoldOblique");
    style_size = 4;
  }
  else if (is_bold)
  {
    strcpy(styles[0], "Bold");
    style_size = 1;
  }
  else if (is_italic)
  {
    strcpy(styles[0], "Italic");
    strcpy(styles[1], "Oblique");
    style_size = 2;
  }
  else
  {
    strcpy(styles[0], "Regular");
    strcpy(styles[1], "Normal");
    strcpy(styles[2], "Medium");
    style_size = 3;
  }

  pat = FcPatternCreate();
  os = FcObjectSetBuild(FC_FAMILY, FC_FILE, FC_STYLE, NULL);
  fs = FcFontList(NULL, pat, os);
  if (pat)
    FcPatternDestroy(pat);

  if (fs)
  {
    int j, s;

    for (j = 0; j < fs->nfont; j++)
    {
      FcChar8 *file;
      FcChar8 *style;
      FcChar8 *family;

      FcPatternGetString(fs->fonts[j], FC_FILE, 0, &file);
      FcPatternGetString(fs->fonts[j], FC_STYLE, 0, &style);
      FcPatternGetString(fs->fonts[j], FC_FAMILY, 0, &family);

      if (iupStrEqualNoCasePartial((char*)family, font_name))
      {
        /* check if the font is of the correct type. */
        for (s = 0; s < style_size; s++)
        {
          if (iupStrEqualNoCase(styles[s], (char*)style))
          {
            strcpy(fileName, (char*)file);
            bResult = 1;
            FcFontSetDestroy(fs);
            return bResult;
          }

          /* set value to use if no more correct font of same family is found. */
          strcpy(fileName, (char*)file);
          bResult = 1;
        }
      }
    }
    FcFontSetDestroy(fs);
  }

  return bResult;
}
#else
static int iGLGetFontFilenameFromSystem(const char *font_name, int is_bold, int is_italic, char* fileName)
{
  (void)font_name;
  (void)is_bold;
  (void)is_italic;
  (void)fileName;
  return 0;
}
#endif
#endif

static int iGLGetFontFilenameFromTypeface(const char* path, const char* typeface, const char* ext, char* filename)
{
  FILE *file;

  /* current directory */
  sprintf(filename, "%s.%s", typeface, ext);
  file = fopen(filename, "r");

  if (!file)
  {
    /* path from the environment */
    if (path)
    {
      sprintf(filename, "%s/%s.%s", path, typeface, ext);
      file = fopen(filename, "r");
    }

    if (!file)
    {
#ifdef WIN32
      /* Windows Font folder */
      sprintf(filename, "%s\\%s.%s", winGetFontDir(), typeface, ext);
      file = fopen(filename, "r");

      if (!file)
        return 0;
#else
      return 0;
#endif
    }
  }

  fclose(file);
  return 1;
}

static char* iGLGetFontNameTex(const char* typeface)
{
  if (iupStrEqualNoCase(typeface, "sans") ||
    iupStrEqualNoCase(typeface, "helvetica") ||
    iupStrEqualNoCase(typeface, "arial"))
    return "heros";
  if (iupStrEqualNoCase(typeface, "monospace") ||
    iupStrEqualNoCase(typeface, "courier") ||
    iupStrEqualNoCase(typeface, "courier new"))
    return "cursor";
  if (iupStrEqualNoCase(typeface, "serif") ||
    iupStrEqualNoCase(typeface, "times") ||
    iupStrEqualNoCase(typeface, "times new roman"))
    return "termes";
  return NULL;
}

static char* iGLGetFontNameTTF(const char* typeface)
{
  if (iupStrEqualNoCase(typeface, "sans") ||
    iupStrEqualNoCase(typeface, "helvetica") ||
    iupStrEqualNoCase(typeface, "arial"))
    return "arial";
  if (iupStrEqualNoCase(typeface, "monospace") ||
    iupStrEqualNoCase(typeface, "courier") ||
    iupStrEqualNoCase(typeface, "courier new"))
    return "cour";
  if (iupStrEqualNoCase(typeface, "serif") ||
    iupStrEqualNoCase(typeface, "times") ||
    iupStrEqualNoCase(typeface, "times new roman"))
    return "times";
  return NULL;
}

static int iGLGetFontFilenameFromNamesOTF(const char* path, const char* typeface, char* filename, int is_bold, int is_italic)
{
  char fontname[100];
  char* face = iGLGetFontNameTex(typeface);
  if (face)
    sprintf(fontname, "texgyre%s-", face);
  else
    sprintf(fontname, "texgyre%s-", typeface);

  if (is_bold && is_italic)
    strcat(fontname, "bolditalic");
  else if (is_italic)
    strcat(fontname, "italic");
  else if (is_bold)
    strcat(fontname, "bold");
  else
    strcat(fontname, "regular");

  return iGLGetFontFilenameFromTypeface(path, fontname, "otf", filename);
}

static int iGLGetFontFilenameFromNamesTTF(const char* path, const char* typeface, char* filename, int is_bold, int is_italic)
{
  char fontname[100];
  char* face = iGLGetFontNameTTF(typeface);
  if (face)
    strcpy(fontname, face);
  else
    strcpy(fontname, typeface);

  if (is_bold && is_italic)
    strcat(fontname, "bi");
  else if (is_italic)
    strcat(fontname, "i");
  else if (is_bold)
    strcat(fontname, "bd");

  return iGLGetFontFilenameFromTypeface(path, fontname, "ttf", filename);
}

static void iGLGetFontFilename(char* filename, const char *typeface, int is_bold, int is_italic)
{
  char *path = getenv("FREETYPEFONTS_DIR");
  if (!path)
    path = IupGetGlobal("FREETYPEFONTS_DIR");

  /* search for the font in the system */
  /* "Helvetica" "Courier" "Times" "Segoe UI" "Tahoma" etc */
  if (iGLGetFontFilenameFromSystem(typeface, is_bold, is_italic, filename))
    return;

  /* try typeface as a file title, compose with path to get a filename
     assume style already in the typeface */
  /* "ariali" "courbd" "texgyrecursor-bold" */
  if (iGLGetFontFilenameFromTypeface(path, typeface, "ttf", filename))
    return;
  if (iGLGetFontFilenameFromTypeface(path, typeface, "otf", filename))
    return;

  /* check for the pre-defined names, and use style to compose the filename */
  /* "cursor" */
  if (iGLGetFontFilenameFromNamesTTF(path, typeface, filename, is_bold, is_italic))
    return;
  if (iGLGetFontFilenameFromNamesOTF(path, typeface, filename, is_bold, is_italic))
    return;

  /* try the typeface as file name */
  strcpy(filename, typeface);
}

static int iGLFontGetFontAveWidth(FTGLfont* ftgl_font);

static IglFont* iGLFindFont(Ihandle* ih, Ihandle* gl_parent, const char *font)
{
  char filename[10240];
  char typeface[50] = "";
  FTGLfont* ftgl_font;
  int size = 8;
  int is_bold = 0,
    is_italic = 0,
    is_underline = 0,
    is_strikeout = 0;
  int i, count;
  IglFont* fonts;
  double res = IupGetDouble(NULL, "SCREENDPI");
  Iarray* gl_fonts = (Iarray*)iupAttribGet(gl_parent, "GL_FONTLIST");
  if (!gl_fonts)
  {
    gl_fonts = iupArrayCreate(50, sizeof(IglFont));
    iupAttribSet(gl_parent, "GL_FONTLIST", (char*)gl_fonts);
  }

  if (!iupGetFontInfo(font, typeface, &size, &is_bold, &is_italic, &is_underline, &is_strikeout))
    return NULL;

  if (is_underline)
    iupAttribSet(ih, "UNDERLINE", "1");
  else
    iupAttribSet(ih, "UNDERLINE", NULL);

  iGLGetFontFilename(filename, typeface, is_bold, is_italic);

  if (size < 0)
    size = (int)((-size*72.0) / res + 0.5);  /* convert to points */

  /* Check if the filename already exists in cache */
  fonts = (IglFont*)iupArrayGetData(gl_fonts);
  count = iupArrayCount(gl_fonts);
  for (i = 0; i < count; i++)
  {
    if (iupStrEqualNoCase(filename, fonts[i].filename) && fonts[i].size == size)
      return &fonts[i];
  }

  ftgl_font = ftglCreateTextureFont(filename);
  if (!ftgl_font)
    return NULL;

  ftglSetFontFaceSize(ftgl_font, size, (int)res);

  /* create room in the array */
  fonts = (IglFont*)iupArrayInc(gl_fonts);

  strcpy(fonts[i].filename, filename);
  fonts[i].ftgl_font = ftgl_font;
  fonts[i].size = size;

  /* NOTICE that this is different from CD,
  here we need average width,
  there is maximum width. */
  fonts[i].charwidth = iGLFontGetFontAveWidth(ftgl_font);
  fonts[i].charheight = iupRound(ftglGetFontLineHeight(ftgl_font));

  return &fonts[i];
}

static IglFont* iGLFontCreateNativeFont(Ihandle *ih, const char* value)
{
  IglFont* glfont;
  Ihandle* gl_parent = (Ihandle*)iupAttribGet(ih, "GL_CANVAS");
  IupGLMakeCurrent(gl_parent);

  glfont = iGLFindFont(ih, gl_parent, value);
  if (!glfont)
  {
    iupERROR1("Failed to create Font: %s", value);
    return NULL;
  }

  iupAttribSet(ih, "_IUP_GLFONT", (char*)glfont);
  return glfont;
}

static IglFont* iGLFontGet(Ihandle *ih)
{
  IglFont* glfont = (IglFont*)iupAttribGet(ih, "_IUP_GLFONT");
  if (!glfont)
    glfont = iGLFontCreateNativeFont(ih, IupGetAttribute(ih, "FONT"));
  return glfont;
}

char* iupStrConvertToUTF8(const char* str, int len, char* utf8_buffer, int *utf8_buffer_max, int utf8mode);

static void iGLFontConvertToUTF8(const char* str, int len)
{
  /* FTGL multibyte strings are always UTF-8 */
  int utf8mode = IupGetInt(NULL, "UTF8MODE");

  gl_utf8_buffer = iupStrConvertToUTF8(str, len, gl_utf8_buffer, &gl_utf8_buffer_len, utf8mode);
}

static int iGLFontGetFontAveWidth(FTGLfont* ftgl_font)
{
  static int first = 1;
  static char sample[512] = "";
  if (first)
  {
    int utf8mode = IupGetInt(NULL, "UTF8MODE");

    /* use all Latin-1 characters */
    int i;
    for (i = 32; i < 256; i++)
      sample[i - 32] = (char)i;

    if (utf8mode) IupSetGlobal("UTF8MODE", "No");
    iGLFontConvertToUTF8(sample, 256 - 32);
    if (utf8mode) IupSetGlobal("UTF8MODE", "Yes");

    i = (int)strlen(gl_utf8_buffer);
    memcpy(sample, gl_utf8_buffer, i + 1);
    first = 0;
  }

  return iupRound(ftglGetFontAdvance(ftgl_font, sample) / (256.0f - 32.0f));
}

int iupGLFontGetStringWidth(Ihandle* ih, const char* str, int len)
{
  IglFont* glfont;

  if (!ih->handle)
    return 0;

  glfont = iGLFontGet(ih);
  if (!glfont)
    return 0;

  iGLFontConvertToUTF8(str, len);
  return iupRound(ftglGetFontAdvance(glfont->ftgl_font, gl_utf8_buffer));
}

void iupGLFontGetMultiLineStringSize(Ihandle* ih, const char* str, int *w, int *h)
{
  int max_w = 0;
  IglFont* glfont;

  if (!ih->handle)
  {
    iupdrvFontGetMultiLineStringSize(ih, str, w, h);
    return;
  }

  glfont = iGLFontGet(ih);
  if (!glfont)
  {
    if (w) *w = 0;
    if (h) *h = 0;
    return;
  }

  if (!str)
  {
    if (w) *w = 0;
    if (h) *h = glfont->charheight * 1;
    return;
  }

  if (str[0] && w)
  {
    int size;
    int len;
    const char *nextstr;
    const char *curstr = str;

    do
    {
      nextstr = iupStrNextLine(curstr, &len);
      if (len)
      {
        iGLFontConvertToUTF8(curstr, len);
        size = iupRound(ftglGetFontAdvance(glfont->ftgl_font, gl_utf8_buffer));
        max_w = iupMAX(max_w, size);
      }

      curstr = nextstr;
    } while (*nextstr);
  }

  if (w) *w = max_w;
  if (h) *h = glfont->charheight * iupStrLineCount(str);
}

int iupGLFontSetFontAttrib(Ihandle* ih, const char* value)
{
  IglFont* glfont;

  if (!ih->handle)
    return iupdrvSetFontAttrib(ih, value);

  glfont = iGLFontCreateNativeFont(ih, value);
  if (glfont)
  {
    /* If FONT is changed, must update the SIZE attribute */
    char* value = iupAttribGet(ih, "SIZE");
    if (!value)
      return 1;

    IupSetStrAttribute(ih, "SIZE", value);
  }

  return 1;
}

void iupGLFontGetCharSize(Ihandle* ih, int *charwidth, int *charheight)
{
  IglFont* glfont;

  if (!ih->handle)
  {
    iupdrvFontGetCharSize(ih, charwidth, charheight);
    return;
  }

  glfont = iGLFontGet(ih);
  if (!glfont)
  {
    if (charwidth)  *charwidth = 0;
    if (charheight) *charheight = 0;
    return;
  }

  if (charwidth)  *charwidth = glfont->charwidth;
  if (charheight) *charheight = glfont->charheight;
}

void iupGLFontGetDim(Ihandle* ih, int *maxwidth, int *height, int *ascent, int *descent)
{
  IglFont* glfont;

  if (!ih->handle)
    return;

  glfont = iGLFontGet(ih);
  if (!glfont)
    return;

  if (maxwidth) *maxwidth = glfont->charwidth;
  if (height)   *height = glfont->charheight;
  if (ascent)   *ascent = iupRound(ftglGetFontAscender(glfont->ftgl_font));
  if (descent)  *descent = iupRound(-ftglGetFontDescender(glfont->ftgl_font));
}

void iupGLFontRenderString(Ihandle* ih, const char* str, int len)
{
  IglFont* glfont;

  if (!ih->handle)
    return;

  glfont = iGLFontGet(ih);
  if (!glfont)
    return;

  iGLFontConvertToUTF8(str, len);
  /* render always at baseline */
  ftglRenderFont(glfont->ftgl_font, gl_utf8_buffer, FTGL_RENDER_ALL);
}

void iupGLFontInit(void)
{
#ifdef WIN32
  if (!win_fonts)
    winInitFontNames();
#endif
}

void iupGLFontRelease(Ihandle* gl_parent)
{
  Iarray* gl_fonts = (Iarray*)iupAttribGet(gl_parent, "GL_FONTLIST");
  if (gl_fonts)
  {
    int i, count = iupArrayCount(gl_fonts);
    IglFont* fonts = (IglFont*)iupArrayGetData(gl_fonts);
    for (i = 0; i < count; i++)
    {
      ftglDestroyFont(fonts[i].ftgl_font);
      fonts[i].ftgl_font = NULL;
    }
    iupArrayDestroy(gl_fonts);
    iupAttribSet(gl_parent, "GL_FONTLIST", NULL);
  }
}

void iupGLFontFinish(void)
{
  if (gl_utf8_buffer)
  {
    free(gl_utf8_buffer);
    gl_utf8_buffer = NULL;
    gl_utf8_buffer_len = 0;
  }

#ifdef WIN32
  if (win_fonts)
    free(win_fonts);
  win_fonts = NULL;
  win_fonts_count = 0;
#endif
}
