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
  fonts[i].hFont = [hFont retain];
  fonts[i].attributes = [attributes copy];
//	fonts[i].charheight = (int)([hFont ascender] + fabs([hFont descender]) + 0.5);
	//	fonts[i].charheight = (int)([hFont capHeight] + 0.5);
//	fonts[i].charheight = (int)([hFont defaultLineHeightForFont] + 0.5);
	NSLayoutManager *lm = [[NSLayoutManager alloc] init];
	fonts[i].charheight = iupROUND([lm defaultLineHeightForFont:hFont]);
	[lm release];

	
  NSRect rect = [hFont boundingRectForFont];
  fonts[i].charwidth = iupROUND(rect.size.width);

	[attributes release];
	
  return &fonts[i];
}

char* iupdrvGetSystemFont(void)
{
  static char systemfont[200] = "";
  NSFont *font = [NSFont systemFontOfSize:0];
	// systemfont: ".AppleSystemUIFont 13.00 pt. P [] (0x60800004b400) fobj=0x10040a600, spc=3.59"
	NSLog(@"systemfont: %@", font);
	// ".AppleSystemUIFont"
  const char *name = [[font familyName] UTF8String];
  if(*name)
    strlcpy(systemfont,name, 200);
  else
    strlcpy(systemfont, "Helvetica Neue, 13", 200);
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
	
	if(NULL == value)
	{
		
		NSFont* default_font = [NSFont systemFontOfSize:0];
//		NSFont* default_font = [NSFont labelFontOfSize:0];
//		NSFont* default_font = [NSFont labelFontOfSize:13];
//		NSFont* default_font = [NSFont userFontOfSize:0];
		iupdrvGetSystemFont();
		
		// FIXME: LEAK (this is not freed anywhere)
		
		ImacFont* macfont = (ImacFont*)calloc(1, sizeof(ImacFont));
		const char* font_name = [[default_font familyName] UTF8String];
		NSLog(@"font name: %@", [default_font familyName]);
		NSLog(@"font size: %f", [default_font pointSize]);

		// max size is in ImacFont definition
		strlcpy(macfont->standardfont, font_name, 200);
		macfont->hFont = [default_font retain];
/*
		NSFont* font_copy = [NSFont fontWithName:[default_font familyName] size:[default_font pointSize]];
		NSLog(@"font_copy %@", font_copy);
*/
		NSDictionary* font_attributes = @{
			NSFontAttributeName:default_font
//			NSFontAttributeName:font_copy
//			NSFontAttributeName:[NSNumber numberWithDouble:(double)[default_font pointSize]],
		};
		macfont->attributes = [font_attributes retain];

		//		NSSize size = [str sizeWithAttributes:[NSDictionary dictionaryWithObjectsAndKeys:[NSFont systemFontOfSize:0], NSFontAttributeName, nil]];

		
		iupAttribSet(ih, "_IUP_WINFONT", (char*)macfont);

		// https://developer.apple.com/library/content/documentation/Cocoa/Conceptual/TextLayout/Tasks/StringHeight.html#//apple_ref/doc/uid/20001809-CJBGBIBB
		// defaultLineHeightForFont: 16
		// boundingRectForFont: (width = 21.099609375, height = 17.6337890625)
		NSLayoutManager *lm = [[NSLayoutManager alloc] init];
		macfont->charheight = iupROUND([lm defaultLineHeightForFont:default_font]);
		[lm release];

		// We're going to have problems with NSTextField height
		// http://stackoverflow.com/questions/13010264/change-nstextfields-height-according-to-font-size
		// https://lists.apple.com/archives/cocoa-dev/2006/Jan/msg01874.html
		
		
  NSRect font_rect = [default_font boundingRectForFont];
  macfont->charwidth = iupROUND(font_rect.size.width);
		
		
//		macfont->charheight
//		pointSize
		return macfont;
		
	}
	else
	{
	  ImacFont* macfont = macFindFont(value);
	  if (!macfont)
	  {
		iupERROR1("Failed to create Font: %s", value); 
		return NULL;
	  }

	  iupAttribSet(ih, "_IUP_WINFONT", (char*)macfont);
	  return macfont;
	}
}


int iupdrvSetFontAttrib(Ihandle* ih, const char* value)
{
	return 1;
}

const char* iupdrvGetFontAttrib(Ihandle* ih)
{
	return NULL;
}


static ImacFont* macFontGet(Ihandle *ih)
{
  ImacFont* macfont = (ImacFont*)iupAttribGet(ih, "_IUP_WINFONT");
  if (!macfont)
  {
	  // FIXME: Used to call iupGetFontAttrib, but function seems to have been removed between 3.15 and 3.21
    macfont = macFontCreateNativeFont(ih, iupdrvGetFontAttrib(ih));
  }
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
#if 1 // iupBaseUpdateSizeFromFont missing
  ImacFont* macfont = macFontCreateNativeFont(ih, value);
  if (!macfont)
    return 1;

	/* If FONT is changed, must update the SIZE attribute */
	iupBaseUpdateAttribFromFont(ih);

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
	int max_h;
	
	/* This won't work because other callbacks assume the string size and then add padding on top of it.
	id native_object = ih->handle;
	if([native_object respondsToSelector:@selector(sizeToFit)])
   {
//	   [native_object sizeToFit];
	   NSRect the_rect = [native_object frame];
	   
	   if (w) *w = the_rect.size.width;
	   if (h) *h = the_rect.size.height;
	   
	   return;
   }
*/

	
	
	
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
	max_h = 0;
  num_lin = 1;
  if (str[0])
  {

	  int len;
    const char *nextstr;
    const char *curstr = str;
	  NSMutableArray* array_of_nsstring_lines = [[NSMutableArray alloc] init];

    do
    {
      nextstr = iupStrNextLine(curstr, &len);  
	  NSString *str = [[NSString alloc] initWithBytes:curstr length:len encoding:NSUTF8StringEncoding];
/*
		NSSize size = [str sizeWithAttributes: macfont->attributes];

//		NSSize size = [str sizeWithAttributes:[NSDictionary dictionaryWithObjectsAndKeys:[NSFont systemFontOfSize:0], NSFontAttributeName, nil]];
*/
		if(nil != str)
		{
			[array_of_nsstring_lines addObject:str];
			[str release];
		}
		else
		{
			[array_of_nsstring_lines addObject:@""];

		}
		
		
//		max_w = iupMAX(max_w, size.width + 0.5);

      curstr = nextstr;
      if (*nextstr)
	  {
        num_lin++;
	  }
    } while(*nextstr);

	  NSString* joined_string = [array_of_nsstring_lines componentsJoinedByString:@"\n"];
	  // 10.11+
	  // boundingRectWithSize:options:attributes:context:
	  //
	  // sizeWithFont:constrainedToSize:lineBreakMode: on iOS, but deprecated in iOS7
	  NSSize size = [joined_string sizeWithAttributes: macfont->attributes];

	  max_w = iupROUND(size.width);
	  max_h = iupROUND(size.height);

	  [array_of_nsstring_lines release];
  }
	

	
  if (w)
  {
	  
  *w = max_w;
	  
	  
  }
//	if (h) *h = (macfont->charheight*num_lin)+0.5;
	if (h)
	{
		*h = max_h;
	}
}

int iupdrvFontGetStringWidth(Ihandle* ih, const char* str)
{
	
//	return 40;
	
  NSDictionary *attributes;
  size_t len;
  char* line_end;

  if (!str || str[0]==0)
    return 0;

  attributes = (NSDictionary*)iupmacGetHFontAttrib(ih);
//	NSLog(@"disabled iupdrvFontGetStringWidth due to crash");
//	return 0;

	
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


	// 10.11+
	// boundingRectWithSize:options:attributes:context:
	//
	// sizeWithFont:constrainedToSize:lineBreakMode: on iOS, but deprecated in iOS7
  NSSize size = [ns_str sizeWithAttributes: macfont->attributes];
	[ns_str release];
	
  return iupROUND(size.width);
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
	  
	  [fonts[i].hFont release];
	  [fonts[i].attributes release];
    fonts[i].hFont = nil;
	fonts[i].attributes = nil;
//	  free(fonts[i]);
//	  fonts[i] = NULL;
  }
  iupArrayDestroy(mac_fonts);
}
