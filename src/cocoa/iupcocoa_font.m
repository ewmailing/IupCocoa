/** \file
 * \brief MAC Font mapping
 *
 * See Copyright Notice in "iup.h"
 */


#include <stdlib.h>
#include <stdio.h>

#import <Cocoa/Cocoa.h>

#include "iup.h"

#include "iup_str.h"
#include "iup_array.h"
#include "iup_attrib.h"
#include "iup_object.h"
#include "iup_drv.h"
#include "iup_drvfont.h"
#include "iup_assert.h"

//#include "iupmac_info.h"


typedef struct ImacFont_
{
  char standardfont[200];
  __unsafe_unretained NSFont *hFont;
  __unsafe_unretained NSDictionary *attributes;
  int charwidth, charheight;
} ImacFont;

static Iarray* mac_fonts = NULL;

static ImacFont* macFindFont(const char *standardfont)
{
  NSFont *hFont;
  NSMutableDictionary *attributes = [[NSMutableDictionary alloc] init];
  int height_pixels;
  char typeface[50] = "";
  int height = 8;
  int is_bold = 0,
    is_italic = 0, 
    is_underline = 0,
    is_strikeout = 0;
  int i, count = iupArrayCount(mac_fonts);    
  float factor = [[NSScreen mainScreen] userSpaceScaleFactor];
  const char* mapped_name;

  /* Check if the standardfont already exists in cache */
  ImacFont* fonts = (ImacFont*)iupArrayGetData(mac_fonts);
  for (i = 0; i < count; i++)
  {
    if (iupStrEqualNoCase(standardfont, fonts[i].standardfont))
      return &fonts[i];
  }
	
	int size = 12; // ?????

  /* parse the old format first */    
  if (!iupFontParseWin(standardfont, typeface, &size, &is_bold, &is_italic, &is_underline, &is_strikeout))
  {
    if (!iupFontParseX(standardfont, typeface, &size, &is_bold, &is_italic, &is_underline, &is_strikeout))
    {
      if (!iupFontParsePango(standardfont, typeface, &size, &is_bold, &is_italic, &is_underline, &is_strikeout))
        return NULL;
    }
  }

  mapped_name = iupFontGetWinName(typeface);
  if (mapped_name)
    strcpy(typeface, mapped_name);
                        
  
  /* get in pixels */
  height_pixels = (int)(height * factor);

  if (height_pixels == 0)
    return NULL;    

  hFont = [NSFont fontWithName:[NSString stringWithUTF8String:typeface] size:height];
  if(is_italic)
        hFont = [[NSFontManager sharedFontManager] convertFont:hFont 
              toHaveTrait:NSItalicFontMask];
  if(is_bold)
	    hFont = [[NSFontManager sharedFontManager] convertFont:hFont 
	          toHaveTrait:NSBoldFontMask];
  if(is_underline)                           
  		[attributes setValue:[NSNumber numberWithInt:NSUnderlinePatternSolid|NSUnderlineStyleSingle] 
		forKey:NSUnderlineStyleAttributeName];
  if(is_strikeout)                           
  		[attributes setValue:[NSNumber numberWithInt:NSUnderlinePatternSolid|NSUnderlineStyleSingle] 
		forKey:NSStrikethroughStyleAttributeName];   
  [attributes setValue:hFont forKey:NSFontAttributeName];
	
  if (!hFont)
    return NULL;

  /* create room in the array */
  fonts = (ImacFont*)iupArrayInc(mac_fonts);

  strcpy(fonts[i].standardfont, standardfont);
  fonts[i].hFont = hFont;          
  fonts[i].attributes = [attributes copy];
  fonts[i].charheight = (int)([hFont ascender] + [hFont descender]);      
  NSRect rect = [hFont boundingRectForFont];
  fonts[i].charwidth = (int)rect.size.width;

	[attributes release];
	
  return &fonts[i];
}

char* iupdrvGetSystemFont(void)
{
  static char systemfont[200] = "";
  NSFont *font = [NSFont systemFontOfSize:10];
  char *name = [[font familyName] UTF8String];
  if(*name)
    strcpy(systemfont,name);
  else
    strcpy(systemfont, "Tahoma, 10");
  return systemfont;
}

char* iupmacFindHFont(NSFont *hFont)
{
  int i, count = iupArrayCount(mac_fonts);

  /* Check if the standardfont already exists in cache */
  ImacFont* fonts = (ImacFont*)iupArrayGetData(mac_fonts);
  for (i = 0; i < count; i++)
  {
    if (hFont == fonts[i].hFont)
      return fonts[i].standardfont;
  }

  return NULL;
}

NSFont *iupmacGetHFont(const char* value)
{
  ImacFont* macfont = macFindFont(value);
  if (!macfont)
    return NULL;
  else
    return macfont->hFont;
}

static ImacFont* macFontCreateNativeFont(Ihandle *ih, const char* value)
{
  ImacFont* macfont = macFindFont(value);
  if (!macfont)
  {
    iupERROR1("Failed to create Font: %s", value); 
    return NULL;
  }

  iupAttribSetStr(ih, "_IUP_WINFONT", (char*)macfont);
  return macfont;
}

static ImacFont* macFontGet(Ihandle *ih)
{
  ImacFont* macfont = (ImacFont*)iupAttribGet(ih, "_IUP_WINFONT");
  if (!macfont)
    macfont = macFontCreateNativeFont(ih, iupGetFontAttrib(ih));
  return macfont;
}

NSDictionary* iupmacGetHFontAttrib(Ihandle *ih)
{
  ImacFont* macfont = macFontGet(ih);
  if (!macfont)
    return NULL;
  else
//    return (char *)macfont->attributes;
	return macfont->attributes;
}

int iupdrvSetStandardFontAttrib(Ihandle* ih, const char* value)
{
#if 0  // iupBaseUpdateSizeFromFont missing
  ImacFont* macfont = macFontCreateNativeFont(ih, value);
  if (!macfont)
    return 1;

  /* If FONT is changed, must update the SIZE attribute */
  iupBaseUpdateSizeFromFont(ih);

  /* FONT attribute must be able to be set before mapping, 
      so the font is enable for size calculation. */
  if (ih->handle && (ih->iclass->nativetype != IUP_TYPEVOID)) {
	
  }
#endif
  return 1;
}

void iupdrvFontGetMultiLineStringSize(Ihandle* ih, const char* str, int *w, int *h)
{
  int num_lin, max_w;

  ImacFont* macfont = macFontGet(ih);
  if (!macfont)
  {
    if (w) *w = 0;
    if (h) *h = 0;
    return;
  }

  if (!str)
  {
    if (w) *w = 0;
    if (h) *h = macfont->charheight * 1;
    return;
  }

  max_w = 0;
  num_lin = 1;
  if (str[0])
  {

	  int len;
    const char *nextstr;
    const char *curstr = str;

    do
    {
      nextstr = iupStrNextLine(curstr, &len);  
	  NSString *str = [[NSString alloc] initWithBytes:curstr length:len encoding:NSUTF8StringEncoding];
//	  NSSize size = [str sizeWithAttributes: macfont->attributes];

		NSSize size = [str sizeWithAttributes:[NSDictionary dictionaryWithObjectsAndKeys:[NSFont systemFontOfSize:11], NSFontAttributeName, nil]];


		
		
		max_w = iupMAX(max_w, size.width);

      curstr = nextstr;
      if (*nextstr)
        num_lin++;
    } while(*nextstr);

  }

  if (w) *w = max_w;
  if (h) *h = macfont->charheight*num_lin;
}

int iupdrvFontGetStringWidth(Ihandle* ih, const char* str)
{
	
//	return 40;
	
  NSDictionary *attributes;
  int len;
  char* line_end;

  if (!str || str[0]==0)
    return 0;

  attributes = (NSDictionary*)iupmacGetHFontAttrib(ih);
	NSLog(@"disabled iupdrvFontGetStringWidth due to crash");
	return 0;

	
  if (!attributes || ([attributes count] == 0))
  {
	  NSLog(@"missing font attributes");
    return 0;
  }

  line_end = strchr(str, '\n');
  if (line_end)
    len = line_end-str;
  else
    len = strlen(str);
                     
  NSString *ns_str = [[NSString alloc] initWithBytes:str length:len encoding:NSUTF8StringEncoding];
	
	ImacFont* macfont = macFontGet(ih);

  NSSize size = [ns_str sizeWithAttributes: macfont->attributes];

  return size.width;
}

void iupdrvFontGetCharSize(Ihandle* ih, int *charwidth, int *charheight)
{
  ImacFont* macfont = macFontGet(ih);
  if (!macfont)
  {
    if (charwidth)  *charwidth = 0;
    if (charheight) *charheight = 0;
    return;
  }

  if (charwidth)  *charwidth = macfont->charwidth; 
  if (charheight) *charheight = macfont->charheight;
}

void iupdrvFontInit(void)
{
  mac_fonts = iupArrayCreate(50, sizeof(ImacFont));
}

void iupdrvFontFinish(void)
{
  int i, count = iupArrayCount(mac_fonts);
  ImacFont* fonts = (ImacFont*)iupArrayGetData(mac_fonts);
  for (i = 0; i < count; i++)
  {
    fonts[i].hFont = nil;
	fonts[i].attributes = nil;
  }
  iupArrayDestroy(mac_fonts);
}
