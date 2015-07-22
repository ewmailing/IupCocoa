/** \file
 * \brief GTK Font mapping
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <Font.h>
#include <View.h>

#include "iup.h"

#include "iup_str.h"
#include "iup_attrib.h"
#include "iup_array.h"
#include "iup_object.h"
#include "iup_drv.h"
#include "iup_drvfont.h"
#include "iup_assert.h"

#include "iuphaiku_drv.h"

#define UNIMPLEMENTED printf("%s (%s %d) UNIMPLEMENTED\n",__func__,__FILE__,__LINE__);

static const int fontHandleSize = B_FONT_FAMILY_LENGTH + B_FONT_STYLE_LENGTH + 7;

typedef struct _IbeFont
{
	char standardfont[fontHandleSize];
	BFont* font;
} IbeFont;

static Iarray* be_fonts = NULL;

char* iupdrvGetSystemFont(void)
{
  static char str[B_FONT_FAMILY_LENGTH + B_FONT_STYLE_LENGTH + 7];
  /* must return a static string, because it will be used as the default value
   * for the FONT attribute */

  font_family family;
  font_style style;
  be_plain_font->GetFamilyAndStyle(&family, &style);
  float size = be_plain_font->Size();

  // FIXME use one of IUP standard formats here (ParseWin, ParseX or ParsePango)
  sprintf(str, "%s, %s %d",family, style, (int)size);
  return str;
}

char* iupgtkGetPangoFontDescAttrib(Ihandle *ih)
{
	UNIMPLEMENTED
		return NULL;
}

char* iupgtkGetPangoLayoutAttrib(Ihandle *ih)
{
	UNIMPLEMENTED
		return NULL;
}

char* iupgtkGetFontIdAttrib(Ihandle *ih)
{
  /* Used by IupGLCanvas for IupGLUseFont */
	UNIMPLEMENTED
		return NULL;
}

static BFont* beFindFont(const char* standardfont)
{
  IbeFont* fonts = (IbeFont*)iupArrayGetData(be_fonts);
  int count = iupArrayCount(be_fonts);

  /* Check if the standardfont already exists in cache */
  int i;
  for (i = 0; i < count; i++)
  {
    if (iupStrEqualNoCase(standardfont, fonts[i].standardfont))
      return fonts[i].font;
  }

  BFont* font = new BFont();
  /* not found, create a new one */
  {
    int size = 0;
    int is_bold = 0,
      is_underline = 0,
      is_strikeout = 0,
      is_italic = 0;
    char typeface[1024];
    const char* mapped_name;

    /* parse the old Windows format first */
    if (!iupFontParseWin(standardfont, typeface, &size, &is_bold, &is_italic, &is_underline, &is_strikeout))
    {
      if (!iupFontParseX(standardfont, typeface, &size, &is_bold, &is_italic, &is_underline, &is_strikeout))
      {
        if (!iupFontParsePango(standardfont, typeface, &size, &is_bold, &is_italic, &is_underline, &is_strikeout))
          return NULL;
      }
    }

    /* Map standard names to native names */
    mapped_name = iupFontGetPangoName(typeface);
    if (mapped_name)
    {
      strcpy(typeface, mapped_name);
    }

	font->SetSize(size);
	font->SetFamilyAndStyle(typeface, NULL);
	uint16 face = 0;
	if (is_bold) face |= B_BOLD_FACE;
	if (is_italic) face |= B_ITALIC_FACE;
	if (is_underline) face |= B_UNDERSCORE_FACE;
	if (is_strikeout) face |= B_STRIKEOUT_FACE;
	font->SetFace(face);
  }

  if (!font) 
    return NULL;

  /* create room in the array */
  fonts = (IbeFont*)iupArrayInc(be_fonts);

  strcpy(fonts[i].standardfont, standardfont);
  fonts[i].font = font;
  return fonts[i].font;
}

static BFont* beFontCreateNativeFont(Ihandle* ih, const char* value)
{
  BFont* bfont = beFindFont(value);
  if (!bfont)
  {
    iupERROR1("Failed to create Font: %s", value); 
    return NULL;
  }

  iupAttribSet(ih, "_IUP_HAIKUFONT", (char*)bfont);
  return bfont;
}


int iupdrvSetStandardFontAttrib(Ihandle* ih, const char* value)
{
  BFont* bfont = beFontCreateNativeFont(ih, value);
  if(!bfont)
    return 1;

  /* If FONT is changed, must update the SIZE attribute */
  iupBaseUpdateSizeFromFont(ih);

  /* FONT attribute must be able to be set before mapping, 
    so the font is enable for size calculation. */
  if (ih->handle && (ih->iclass->nativetype != IUP_TYPEVOID))
  {
	BHandler* handler = (BHandler*)ih->handle;
	BView* view = viewFromHandler(handler);
	if (!view) {
		debugger("Invalid handle ! what is it ?");
	}
	view->SetFont(bfont);
    //iupgtkFontUpdatePangoLayout(ih, gtkFontGetWidgetPangoLayout(ih));
  }

  return 1;
}

static int beFontHeight(const BFont* font)
{
  font_height height;
  font->GetHeight(&height);
  return (int)(height.ascent + height.descent + height.leading);
}

static BFont* beFontGet(Ihandle *ih)
{
  BFont* bfont = (BFont*)iupAttribGet(ih, "_IUP_HAIKUFONT");
  if(!bfont)
    bfont = beFontCreateNativeFont(ih, iupGetFontAttrib(ih));
  return bfont;
}

void iupdrvFontGetMultiLineStringSize(Ihandle* ih, const char* str, int *w, int *h)
{
  int max_w = 0;

  BFont* bfont = beFontGet(ih);
  if (!bfont)
  {
    if (w) *w = 0;
    if (h) *h = 0;
    return;
  }

  int height = beFontHeight(bfont);

  if (!str)
  {
    if (w) *w = 0;
    if (h) *h = height;
    return;
  }
  
  if (str[0])
  {
    int len, lw;
    const char *nextstr;
    const char *curstr = str;

    do
    {
      nextstr = iupStrNextLine(curstr, &len);
      if (len)
      {
        lw = (int)bfont->StringWidth(curstr, len);
        max_w = iupMAX(max_w, lw);
      }

      curstr = nextstr;
    } while(*nextstr);
  }

  if (w) *w = max_w;
  if (h) *h = height * iupStrLineCount(str);
}

int iupdrvFontGetStringWidth(Ihandle* ih, const char* str)
{
  BFont* bfont = beFontGet(ih);
  if (!bfont || !str)
  {
	return 0;
  }

  return (int)bfont->StringWidth(str);
}

void iupdrvFontGetCharSize(Ihandle* ih, int *charwidth, int *charheight)
{
  BFont* befont = beFontGet(ih);
  if(!befont)
  {
    if (charwidth)  *charwidth = 0;
    if (charheight) *charheight = 0;
    return;
  }

  if (charheight) {
    *charheight = beFontHeight(befont);
  }

  if (charwidth) {
    *charwidth = (int)befont->StringWidth("M");
  }
}

void iupdrvFontInit(void)
{
  be_fonts = iupArrayCreate(50, sizeof(IbeFont));
}

void iupdrvFontFinish(void)
{
  int i, count = iupArrayCount(be_fonts);
  IbeFont* fonts = (IbeFont*)iupArrayGetData(be_fonts);
  for (i = 0; i < count; i++)
  {
    delete fonts[i].font;
  }
  iupArrayDestroy(be_fonts);
}
