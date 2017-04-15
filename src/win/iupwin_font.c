/** \file
 * \brief Windows Font mapping
 *
 * See Copyright Notice in "iup.h"
 */


#include <stdlib.h>
#include <stdio.h>

#include <windows.h>

#include "iup.h"

#include "iup_str.h"
#include "iup_array.h"
#include "iup_attrib.h"
#include "iup_object.h"
#include "iup_drv.h"
#include "iup_drvfont.h"
#include "iup_assert.h"

#include "iupwin_drv.h"
#include "iupwin_info.h"
#include "iupwin_str.h"


typedef struct IwinFont_
{
  char font[200];
  HFONT hFont;
  int charwidth, charheight;
} IwinFont;

static Iarray* win_fonts = NULL;

static IwinFont* winFindFont(const char *font)
{
  HFONT hFont;
  int height_pixels;  /* negative value */
  char typeface[50] = "";
  int size = 8;
  int is_bold = 0,
    is_italic = 0, 
    is_underline = 0,
    is_strikeout = 0;
  int res = iupwinGetScreenRes();
  int i, count = iupArrayCount(win_fonts);
  const char* mapped_name;

  /* Check if the font already exists in cache */
  IwinFont* fonts = (IwinFont*)iupArrayGetData(win_fonts);
  for (i = 0; i < count; i++)
  {
    if (iupStrEqualNoCase(font, fonts[i].font))
      return &fonts[i];
  }

  if (!iupGetFontInfo(font, typeface, &size, &is_bold, &is_italic, &is_underline, &is_strikeout))
    return NULL;

  /* Map standard names to native names */
  mapped_name = iupFontGetWinName(typeface);
  if (mapped_name)
    strcpy(typeface, mapped_name);

  /* get in pixels */
  if (size < 0)  
    height_pixels = size;    /* already in pixels */
  else
    height_pixels = -iupWIN_PT2PIXEL(size, res);

  if (height_pixels == 0)
    return NULL;

  hFont = CreateFont(height_pixels, 0,0,0,
                    (is_bold) ? FW_BOLD : FW_NORMAL,
                    is_italic, is_underline, is_strikeout,
                    DEFAULT_CHARSET,OUT_TT_PRECIS,
                    CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,
                    FF_DONTCARE|DEFAULT_PITCH,
                    iupwinStrToSystem(typeface));

  if (!hFont)
    return NULL;

  /* create room in the array */
  fonts = (IwinFont*)iupArrayInc(win_fonts);

  strcpy(fonts[i].font, font);
  fonts[i].hFont = hFont;

  {
    HDC hdc = GetDC(NULL);
    HFONT oldfont = (HFONT)SelectObject(hdc, hFont);

    TEXTMETRIC tm;
    GetTextMetrics(hdc, &tm);
    /* NOTICE that this is different from CD.
       In IUP we need "average" width,
       in CD is "maximum" width. */
    fonts[i].charwidth = tm.tmAveCharWidth; 
    fonts[i].charheight = tm.tmHeight;

    SelectObject(hdc, oldfont);
    ReleaseDC(NULL, hdc);
  }

  return &fonts[i];
}

static void winFontFromLogFontA(LOGFONTA* logfont, char* font)
{
  int is_bold = (logfont->lfWeight == FW_NORMAL)? 0: 1;
  int is_italic = logfont->lfItalic;
  int is_underline = logfont->lfUnderline;
  int is_strikeout = logfont->lfStrikeOut;
  int height_pixels = logfont->lfHeight;  /* negative value */
  int res = iupwinGetScreenRes();
  int size = iupWIN_PIXEL2PT(-height_pixels, res);  /* return in points */

  sprintf(font, "%s, %s%s%s%s %d", logfont->lfFaceName, 
                                   is_bold?"Bold ":"", 
                                   is_italic?"Italic ":"", 
                                   is_underline?"Underline ":"", 
                                   is_strikeout?"Strikeout ":"", 
                                   size);
}

char* iupdrvGetSystemFont(void)
{
  static char str[200]; /* must return a static string, because it will be used as the default value for the FONT attribute */
  NONCLIENTMETRICSA ncm;
  ncm.cbSize = sizeof(NONCLIENTMETRICSA);
  /* this is before setting utf8mode, so use the ANSI version */
  if (SystemParametersInfoA(SPI_GETNONCLIENTMETRICS, ncm.cbSize, &ncm, FALSE))
    winFontFromLogFontA(&ncm.lfMessageFont, str);
  else
  {
    if (iupwinIsVistaOrNew())
      strcpy(str, "Segoe UI, 9");
    else
      strcpy(str, "Tahoma, 10");
  }
  return str;
}

char* iupwinFindHFont(HFONT hFont)
{
  int i, count = iupArrayCount(win_fonts);

  /* Check if the font already exists in cache */
  IwinFont* fonts = (IwinFont*)iupArrayGetData(win_fonts);
  for (i = 0; i < count; i++)
  {
    if (hFont == fonts[i].hFont)
      return fonts[i].font;
  }

  return NULL;
}

HFONT iupwinGetHFont(const char* value)
{
  IwinFont* winfont = winFindFont(value);
  if (!winfont)
    return NULL;
  else
    return winfont->hFont;
}

static IwinFont* winFontCreateNativeFont(Ihandle *ih, const char* value)
{
  IwinFont* winfont = winFindFont(value);
  if (!winfont)
  {
    iupERROR1("Failed to create Font: %s", value); 
    return NULL;
  }

  iupAttribSet(ih, "_IUP_WINFONT", (char*)winfont);
  return winfont;
}

static IwinFont* winFontGet(Ihandle *ih)
{
  IwinFont* winfont = (IwinFont*)iupAttribGet(ih, "_IUP_WINFONT");
  if (!winfont)
  {
    winfont = winFontCreateNativeFont(ih, iupGetFontValue(ih));
    if (!winfont)
      winfont = winFontCreateNativeFont(ih, IupGetGlobal("DEFAULTFONT"));
  }
  return winfont;
}

char* iupwinGetHFontAttrib(Ihandle *ih)
{
  IwinFont* winfont = winFontGet(ih);
  if (!winfont)
    return NULL;
  else
    return (char*)winfont->hFont;
}

int iupdrvSetFontAttrib(Ihandle* ih, const char* value)
{
  IwinFont* winfont = winFontCreateNativeFont(ih, value);
  if (!winfont)
    return 1;

  /* If FONT is changed, must update the SIZE attribute */
  iupBaseUpdateAttribFromFont(ih);

  /* FONT attribute must be able to be set before mapping, 
      so the font is enable for size calculation. */
  if (ih->handle && (ih->iclass->nativetype != IUP_TYPEVOID))
    SendMessage(ih->handle, WM_SETFONT, (WPARAM)winfont->hFont, MAKELPARAM(TRUE,0));

  return 1;
}

static HDC winFontGetDC(Ihandle* ih)
{
  if (ih->iclass->nativetype == IUP_TYPEVOID)
    return GetDC(NULL);
  else
    return GetDC(ih->handle);  /* handle can be NULL here */
}

static void winFontReleaseDC(Ihandle* ih, HDC hdc)
{
  if (ih->iclass->nativetype == IUP_TYPEVOID)
    ReleaseDC(NULL, hdc);
  else
    ReleaseDC(ih->handle, hdc);  /* handle can be NULL here */
}

void iupdrvFontGetMultiLineStringSize(Ihandle* ih, const char* str, int *w, int *h)
{
  int max_w = 0;

  IwinFont* winfont = winFontGet(ih);
  if (!winfont)
  {
    if (w) *w = 0;
    if (h) *h = 0;
    return;
  }

  if (!str)
  {
    if (w) *w = 0;
    if (h) *h = winfont->charheight * 1;
    return;
  }

  if (str[0])
  {
    SIZE size;
    int len, wlen;
    const char *nextstr;
    const char *curstr = str;

    HDC hdc = winFontGetDC(ih);
    HFONT oldhfont = (HFONT)SelectObject(hdc, winfont->hFont);
    TCHAR* wstr = iupwinStrToSystem(str);

    do
    {
      nextstr = iupStrNextLine(curstr, &len);
      if (len)
      {
#ifdef UNICODE
        wlen = MultiByteToWideChar(iupwinStrGetUTF8Mode()? CP_UTF8: CP_ACP, 0, curstr, len, 0, 0);
#else
        wlen = len;
#endif

        size.cx = 0;
        GetTextExtentPoint32(hdc, wstr, wlen, &size);
        max_w = iupMAX(max_w, size.cx);

        wstr += wlen+1;
      }

      curstr = nextstr;
    } while(*nextstr);

    SelectObject(hdc, oldhfont);
    winFontReleaseDC(ih, hdc);
  }

  if (w) *w = max_w;
  if (h) *h = winfont->charheight * iupStrLineCount(str);
}

int iupdrvFontGetStringWidth(Ihandle* ih, const char* str)
{
  HDC hdc;
  HFONT oldhfont, hFont;
  SIZE size;
  int len;
  char* line_end;  
  TCHAR* wstr;
  if (!str || str[0]==0)
    return 0;

  hFont = (HFONT)iupwinGetHFontAttrib(ih);
  if (!hFont)
    return 0;

  hdc = winFontGetDC(ih);
  oldhfont = SelectObject(hdc, hFont);

  line_end = strchr(str, '\n');
  if (line_end)
    len = (int)(line_end-str);
  else
    len = (int)strlen(str);

  wstr = iupwinStrToSystemLen(str, &len);
  GetTextExtentPoint32(hdc, wstr, len, &size);

  SelectObject(hdc, oldhfont);
  winFontReleaseDC(ih, hdc);

  return size.cx;
}

void iupdrvFontGetCharSize(Ihandle* ih, int *charwidth, int *charheight)
{
  IwinFont* winfont = winFontGet(ih);
  if (!winfont)
  {
    if (charwidth)  *charwidth = 0;
    if (charheight) *charheight = 0;
    return;
  }

  if (charwidth)  *charwidth = winfont->charwidth; 
  if (charheight) *charheight = winfont->charheight;
}

void iupdrvFontInit(void)
{
  win_fonts = iupArrayCreate(50, sizeof(IwinFont));
}

void iupdrvFontFinish(void)
{
  int i, count = iupArrayCount(win_fonts);
  IwinFont* fonts = (IwinFont*)iupArrayGetData(win_fonts);
  for (i = 0; i < count; i++)
  {
    DeleteObject(fonts[i].hFont);
    fonts[i].hFont = NULL;
  }
  iupArrayDestroy(win_fonts);
}
