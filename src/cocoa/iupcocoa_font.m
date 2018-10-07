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

#include "iupcocoa_drv.h"

//#include "iupmac_info.h"

@interface IupCocoaFont : NSObject
@property(nonatomic, retain) NSFont* theFont;
@property(nonatomic, copy) NSString* iupFontName;
@property(nonatomic, copy) NSString* typeFace;
@property(nonatomic, retain) NSMutableDictionary* attributeDictionary;
@property(nonatomic, assign) int fontSize;
@property(nonatomic, assign) int charWidth;
@property(nonatomic, assign) int charHeight;
@end

@implementation IupCocoaFont

// In manual mode (non-ARC), we are still responsible for releasing sythesized properties, even when there is no explicit ivar.
- (void) dealloc
{
	[self setTheFont:nil];
	[self setIupFontName:nil];
	[self setTypeFace:nil];
	[self setAttributeDictionary:nil];
	[super dealloc];
}

@end


typedef struct ImacFont_
{
  char standardfont[200];
  __unsafe_unretained NSFont *hFont;
  __unsafe_unretained NSDictionary *attributes;
  int charwidth, charheight;
} ImacFont;

static Iarray* mac_fonts = NULL;

// We keep all the fonts in a data structure so we can release them on shutdown because IUP doesn't have an explicit life-cycle for individual fonts.
// This is not necessarily for caching. I am under the impression that Apple already caches fonts.
static NSMutableArray<IupCocoaFont*>* s_arrayOfFonts = nil;
// Even though I believe Apple caches NSFont, because we have our own additional object wrapper with additional properties, we might as well do our own caching.
static NSMutableDictionary<NSString*, IupCocoaFont*>* s_mapOfFonts = nil;
// This is for easy access to our system font since it is used so often.
static IupCocoaFont* s_systemFont = nil;


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

static IupCocoaFont* cocoaGetSystemFont()
{
	if(nil == s_systemFont)
	{
		IupCocoaFont* the_font = [[IupCocoaFont alloc] init];
		s_systemFont = the_font;
		
		NSFont* ns_font = [NSFont systemFontOfSize:0];
		[s_systemFont setTheFont:ns_font];
		
		NSMutableDictionary* attribute_dict = [[NSMutableDictionary alloc] init];
		[attribute_dict autorelease];
		
		
		
		[attribute_dict setValue:ns_font forKey:NSFontAttributeName];

		[the_font setTheFont:ns_font];
		[the_font setAttributeDictionary:attribute_dict];

		/*
		familyName: .AppleSystemUIFont
		fontName: .AppleSystemUIFont
		displayName: System Font Regular
		*/
		int font_size = (int)[ns_font pointSize];
		NSString* ns_font_name = [ns_font fontName];
		// Remove leading dot?
		//ns_font_name = [ns_font_name stringByReplacingOccurrencesOfString:@"." withString:@""];
		NSString* ns_iup_font_name = [NSString stringWithFormat:@"%@, %d", ns_font_name, font_size];
//		NSLog(@"ns_iup_font_name: %@", ns_iup_font_name);
		[the_font setIupFontName:ns_iup_font_name];
		[the_font setFontSize:font_size];
		[the_font setTypeFace:ns_font_name];


		NSLayoutManager *lm = [[NSLayoutManager alloc] init];
		int char_height = iupROUND([lm defaultLineHeightForFont:ns_font]);
		[the_font setCharHeight:char_height];
		[lm release];
	
		// https://developer.apple.com/library/content/documentation/Cocoa/Conceptual/TextLayout/Tasks/StringHeight.html#//apple_ref/doc/uid/20001809-CJBGBIBB
		// defaultLineHeightForFont: 16
		// boundingRectForFont: (width = 21.099609375, height = 17.6337890625)
		NSRect rect = [ns_font boundingRectForFont];
		int char_width = iupROUND(rect.size.width);
		[the_font setCharWidth:char_width];

		// Add to global map
		NSCAssert(s_mapOfFonts, @"s_mapOfFonts must be initialized");
		[s_mapOfFonts setObject:s_systemFont forKey:ns_iup_font_name];
	}
	return s_systemFont;
}

char* iupdrvGetSystemFont(void)
{
/*
  static char systemfont[200] = "";
  NSFont *font = [NSFont systemFontOfSize:0];
	// systemfont: ".AppleSystemUIFont 13.00 pt. P [] (0x60800004b400) fobj=0x10040a600, spc=3.59"
	NSLog(@"systemfont: %@", font);
	// ".AppleSystemUIFont"
  const char *name = [[font familyName] UTF8String];
	NSLog(@"familyName: %@", [font familyName]);
	NSLog(@"fontName: %@", [font fontName]);
	NSLog(@"displayName: %@", [font displayName]);

  if(*name)
    strlcpy(systemfont,name, 200);
  else
    strlcpy(systemfont, "Helvetica Neue, 13", 200);
  return systemfont;
*/

	static char system_font_cstr[200] = "";
	if(system_font_cstr[0] == '\0')
	{
		if(nil == s_systemFont)
		{
			// make sure font system was initialized
			iupdrvFontInit();
			cocoaGetSystemFont();
		}
	    strlcpy(system_font_cstr, [[s_systemFont iupFontName] UTF8String], 200);
	}
	return (char*)system_font_cstr;

}

static char* iupmacFindHFont(NSFont *hFont)
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

static NSFont *iupmacGetHFont(const char* value)
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
		macfont->charheight = (int)ceil([lm defaultLineHeightForFont:default_font]);
		[lm release];

		// We're going to have problems with NSTextField height
		// http://stackoverflow.com/questions/13010264/change-nstextfields-height-according-to-font-size
		// https://lists.apple.com/archives/cocoa-dev/2006/Jan/msg01874.html
		
		
  NSRect font_rect = [default_font boundingRectForFont];
  macfont->charwidth = (int)ceil(font_rect.size.width);
		
		
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


static IupCocoaFont* cocoaFindFont(const char* iup_font_name)
{
	char type_face[50] = "";
	int font_size = 0;

	int is_bold = 0;
	int is_italic = 0;
    int is_underline = 0;
	int is_strikeout = 0;

	if(NULL == iup_font_name)
	{
		// return system font?
		return NULL;
	}

	NSString* ns_iup_font_name = [NSString stringWithUTF8String:iup_font_name];

	// Check our cache
	IupCocoaFont* the_font = [s_mapOfFonts objectForKey:ns_iup_font_name];
	if(nil != the_font)
	{
		return the_font;
	}



	if(!iupGetFontInfo(iup_font_name, type_face, &font_size, &is_bold, &is_italic, &is_underline, &is_strikeout))
	{
		return NULL;
	}
	
	NSString* ns_type_face = [NSString stringWithUTF8String:type_face];
	NSLog(@"type_face: %@", ns_type_face);
	NSFont* ns_font = [NSFont fontWithName:ns_type_face size:(CGFloat)font_size];
	
	if(nil == ns_font)
	{
		return NULL;
	}

	the_font = [[IupCocoaFont alloc] init];
	[the_font autorelease];
	
	[s_mapOfFonts setObject:the_font forKey:ns_iup_font_name];

	

	NSFontTraitMask trait_mask = 0;
  	if(is_italic)
	{
		trait_mask |= NSItalicFontMask;
	}
	if(is_bold)
	{
		trait_mask |= NSBoldFontMask;
	}
	
	NSMutableDictionary* attribute_dict = [[NSMutableDictionary alloc] init];
	[attribute_dict autorelease];

	if(is_underline)
	{
  		[attribute_dict setValue:[NSNumber numberWithInt:NSUnderlinePatternSolid|NSUnderlineStyleSingle]
  			forKey:NSUnderlineStyleAttributeName];
	}
	if(is_strikeout)
	{
  		[attribute_dict setValue:[NSNumber numberWithInt:NSUnderlinePatternSolid|NSUnderlineStyleSingle]
			forKey:NSStrikethroughStyleAttributeName];

	}
	
	ns_font = [[NSFontManager sharedFontManager] convertFont:ns_font toHaveTrait:trait_mask];
	if(nil == ns_font)
	{
	    return NULL;
	}
	
 	[attribute_dict setValue:ns_font forKey:NSFontAttributeName];

	[the_font setTheFont:ns_font];
	[the_font setAttributeDictionary:attribute_dict];
	[the_font setIupFontName:ns_iup_font_name];
	[the_font setFontSize:font_size];
	[the_font setTypeFace:ns_type_face];


	NSLayoutManager *lm = [[NSLayoutManager alloc] init];
	int char_height = iupROUND([lm defaultLineHeightForFont:ns_font]);
	[the_font setCharHeight:char_height];
	[lm release];
	
	// https://developer.apple.com/library/content/documentation/Cocoa/Conceptual/TextLayout/Tasks/StringHeight.html#//apple_ref/doc/uid/20001809-CJBGBIBB
	// defaultLineHeightForFont: 16
	// boundingRectForFont: (width = 21.099609375, height = 17.6337890625)
	NSRect rect = [ns_font boundingRectForFont];
	int char_width = iupROUND(rect.size.width);
	[the_font setCharWidth:char_width];


	return the_font;
}

static IupCocoaFont* cocoaFontCreateNativeFont(Ihandle* ih, const char* value)
{
	IupCocoaFont* the_font = cocoaFindFont(value);
	if(nil == the_font)
	{
		iupERROR1("Failed to create Font: %s", value);
		return NULL;
	}

	iupAttribSet(ih, "_IUP_COCOAFONT", (char*)the_font);
	return the_font;
}

static IupCocoaFont* cocoaFontGet(Ihandle* ih)
{
	IupCocoaFont* the_font = (IupCocoaFont*)iupAttribGet(ih, "_IUP_COCOAFONT");
	if(nil == the_font)
	{
	    the_font = cocoaFontCreateNativeFont(ih, iupGetFontValue(ih));
		if(nil == the_font)
		{
			the_font = cocoaFontCreateNativeFont(ih, IupGetGlobal("DEFAULTFONT"));
		}
	}
	return the_font;
}

int iupdrvSetFontAttrib(Ihandle* ih, const char* value)
{
	IupCocoaFont* iup_font = cocoaFontCreateNativeFont(ih, value);
	if(nil == iup_font)
	{
		return 0;
	}
	
	/* If FONT is changed, must update the SIZE attribute */
	iupBaseUpdateAttribFromFont(ih);
	
	/* FONT attribute must be able to be set before mapping,
	 so the font is enable for size calculation. */
	if (ih->handle && (ih->iclass->nativetype != IUP_TYPEVOID))
	{
		// Not sure what IUP wants me to do. Call needsDisplay?
		id the_widget = (id)ih->handle;
		if([the_widget respondsToSelector:@selector(setFont:)])
		{
			[the_widget setFont:[iup_font theFont]];
		}
		iupdrvDisplayUpdate(ih);
	}
	return 1;
}


// REMOVE
static const char* iupdrvGetFontAttrib(Ihandle* ih)
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



static NSDictionary* iupmacGetHFontAttrib(Ihandle *ih)
{
  ImacFont* macfont = macFontGet(ih);
  if (!macfont)
    return NULL;
  else
//    return (char *)macfont->attributes;
	return macfont->attributes;
}

// REMOVE
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

#if 0
static void helperFontGetMultiLineStringSize(ImacFont* macfont, const char* str, int *w, int *h)
{
	int num_lin, max_w;
	int max_h;
	
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

	  max_w = (int)ceil(size.width);
	  max_h = (int)ceil(size.height);

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
#else

static void helperFontGetMultiLineStringSize(IupCocoaFont* iup_font, const char* str, int *w, int *h)
{
	int num_lin;
	int max_w;
	int max_h;
	
	if(!iup_font)
	{
		if (w) *w = 0;
		if (h) *h = 0;
		return;
	}

	if(!str)
	{
		if (w) *w = 0;
		if (h) *h = [iup_font charHeight];
		return;
	}

	max_w = 0;
	max_h = 0;
	num_lin = 1;
	if(str[0])
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
		NSSize size = [joined_string sizeWithAttributes:[iup_font attributeDictionary]];
		
		max_w = (int)ceil(size.width);
		max_h = (int)ceil(size.height);
		
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
#endif

void iupdrvFontGetMultiLineStringSize(Ihandle* ih, const char* str, int *w, int *h)
{
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
	IupCocoaFont* iup_font = cocoaFontGet(ih);
	helperFontGetMultiLineStringSize(iup_font, str, w, h);
}

int iupdrvFontGetStringWidth(Ihandle* ih, const char* str)
{
	size_t len;
	char* line_end;
	
	if(!str || str[0]==0)
	{
		return 0;
	}
	
	IupCocoaFont* iup_font = cocoaFontGet(ih);
	if(nil == iup_font)
	{
		return 0;
	}
	
	line_end = strchr(str, '\n');
	if (line_end)
	{
		len = line_end-str;
	}
	else
	{
		len = strlen(str);
	}
	
	NSString* ns_str = [[NSString alloc] initWithBytes:str length:len encoding:NSUTF8StringEncoding];
	
	// 10.11+
	// boundingRectWithSize:options:attributes:context:
	//
	// sizeWithFont:constrainedToSize:lineBreakMode: on iOS, but deprecated in iOS7
	NSSize size = [ns_str sizeWithAttributes:[iup_font attributeDictionary]];
	[ns_str release];
	
	return (int)ceil(size.width);
}

void iupdrvFontGetCharSize(Ihandle* ih, int *charwidth, int *charheight)
{
	
	IupCocoaFont* iup_font = cocoaFontGet(ih);
	if(!iup_font)
	{
		if (charwidth)  *charwidth = 0;
		if (charheight) *charheight = 0;
		return;
	}
	
	if (charwidth)  *charwidth = [iup_font charWidth];
	if (charheight) *charheight = [iup_font charHeight];
}

void iupdrvFontGetTextSize(const char* font_name, const char* str, int *w, int *h)
{
#if 0
	ImacFont* macfont = macFindFont(font);
	if (macfont)
	{
		// FIXME: quick and dirty fix to get around Iup internal API changes
		helperFontGetMultiLineStringSize(macfont, str, w, h);
	}
#else
	IupCocoaFont* the_font = cocoaFindFont(font_name);
	if(the_font)
	{
		// FIXME: quick and dirty fix to get around Iup internal API changes
		helperFontGetMultiLineStringSize(the_font, str, w, h);
	}
#endif
}

void iupdrvFontInit(void)
{
  mac_fonts = iupArrayCreate(50, sizeof(ImacFont));

	if(nil == s_arrayOfFonts)
	{
		s_arrayOfFonts = [[NSMutableArray alloc] init];
	}
	if(nil == s_mapOfFonts)
	{
		s_mapOfFonts = [[NSMutableDictionary alloc] init];
	}
	if(nil == s_systemFont)
	{
		// this will set s_systemFont
		cocoaGetSystemFont();
	}
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

	// This will release all the fonts we've allocated.
	[s_arrayOfFonts release];
	s_arrayOfFonts = nil;
	[s_mapOfFonts release];
	s_mapOfFonts = nil;
	[s_systemFont release];
	s_systemFont = nil;

}
