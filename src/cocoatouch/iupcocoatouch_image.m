/** \file
 * \brief Image Resource.
 *
 * See Copyright Notice in "iup.h"
 */

#import <UIKit/UIKit.h>

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#include "iup.h"

#include "iup_object.h"
#include "iup_attrib.h"
#include "iup_str.h"
#include "iup_image.h"

#include "iupcocoatouch_drv.h"


/* Adapted from SDL (zlib)
 * Calculate the pad-aligned scanline width of a surface
 */
static int CalculateBytesPerRow(int width, int bytes_per_pixel)
{
	int pitch;
	int bits_per_pixel = bytes_per_pixel * 8;
	/* Surface should be 4-byte aligned for speed */
	pitch = width * bytes_per_pixel;
	switch (bits_per_pixel) {
		case 1:
			pitch = (pitch + 7) / 8;
			break;
		case 4:
			pitch = (pitch + 1) / 2;
			break;
		default:
			break;
	}
	pitch = (pitch + 3) & ~3;   /* 4-byte aligning */
	return (pitch);
}

static int CalculateRowLength(int width, int bytes_per_pixel)
{
	int pitch = CalculateBytesPerRow(width, bytes_per_pixel);
	return pitch/bytes_per_pixel;
}

static void cocoaTouchCGDataProviderReleaseMallocPixelDataCallback(void* info, const void* data, size_t size)
{
	free((void*)data);
}


// FIXME: Carried over implementation. Probably wrong. Untested, don't know what calls this, don't know how to test.
void iupdrvImageGetRawData(void* handle, unsigned char* imgdata)
{
#if 0
  int x,y;
  unsigned char *red,*green,*blue,*alpha;
  NSImage *image = (__bridge NSImage*)handle;
  NSBitmapImageRep *bitmap = nil;
  if([[image representations] count]>0) bitmap = [[image representations] objectAtIndex:0];
  if(bitmap==nil) return;
	NSInteger w = [bitmap pixelsWide];
  NSInteger h = [bitmap pixelsHigh];
  NSInteger bpp = [bitmap bitsPerPixel];
  NSInteger planesize = w*h;
  unsigned char *bits = [bitmap bitmapData]; 
  red = imgdata;
  green = imgdata+planesize;
  blue = imgdata+2*planesize;
  alpha = imgdata+3*planesize;
  for(y=0;y<h;y++) {
    for(x=0;x<w;x++) {
      if(bpp>=24) {
        *red++ = *bits++;
        *green++ = *bits++;
        *blue++ = *bits++;
      }
      if(bpp==32) {
        *alpha++ = *bits++;
      }
    }
  }
#endif
}

// FIXME: Carried over implementation. Probably wrong. Untested, don't know what calls this, don't know how to test.
// NOTE: Returns an UIImage with retain count of 1
void* iupdrvImageCreateImageRaw(int width, int height, int bpp, iupColor* colors, int colors_count, unsigned char *imgdata)
{
	UIImage* ui_image = nil;


	if(32 == bpp)
	{
		int row_length = CalculateRowLength(width, 4);
		unsigned char* pixels = malloc(row_length*height*(32/8));
		unsigned char* start_pixels = pixels; // we're going to iterate through pixels, but we also want to keep a pointer to the start
		if(NULL == pixels)
		{
			return NULL;
		}
		unsigned char* source_pixel = imgdata;

		for(int y=0;y<height;y++)
		{
			for(int x=0;x<row_length;x++)
			{
				unsigned char s_r = *source_pixel;
				source_pixel++;
				unsigned char s_g = *source_pixel;
				source_pixel++;
				unsigned char s_b = *source_pixel;
				source_pixel++;
				unsigned char s_a = *source_pixel;
				source_pixel++;

				*pixels = s_r;
				pixels++;
				*pixels = s_g;
				pixels++;
				*pixels = s_b;
				pixels++;
				*pixels = s_a;
				pixels++;
			}
		}


		CGColorSpaceRef color_space = CGColorSpaceCreateDeviceRGB();
		CGBitmapInfo bitmap_info = kCGBitmapByteOrder32Big | kCGImageAlphaPremultipliedLast;//kCGBitmapByteOrderDefault;
		CGDataProviderRef data_provider = CGDataProviderCreateWithData(NULL, start_pixels, row_length*height*bpp, &cocoaTouchCGDataProviderReleaseMallocPixelDataCallback);
		CGImageRef cg_image = CGImageCreate(width, height, 8, 32, row_length*(32/8), color_space, bitmap_info, data_provider, NULL, false, kCGRenderingIntentDefault);
		ui_image = [UIImage imageWithCGImage:cg_image];

		CGColorSpaceRelease(color_space);
        CGDataProviderRelease(data_provider);
        CGImageRelease(cg_image);
	}
	else if(24 == bpp)
	{
		int row_length = CalculateRowLength(width, 3);
		unsigned char* pixels = malloc(row_length*height*(24/8));
		unsigned char* start_pixels = pixels; // we're going to iterate through pixels, but we also want to keep a pointer to the start
		if(NULL == pixels)
		{
			return NULL;
		}
		unsigned char* source_pixel = imgdata;

		for(int y=0;y<height;y++)
		{
			for(int x=0;x<row_length;x++)
			{
				unsigned char s_r = *source_pixel;
				source_pixel++;
				unsigned char s_g = *source_pixel;
				source_pixel++;
				unsigned char s_b = *source_pixel;
				source_pixel++;

				*pixels = s_r;
				pixels++;
				*pixels = s_g;
				pixels++;
				*pixels = s_b;
				pixels++;
			}
		}


		CGColorSpaceRef color_space = CGColorSpaceCreateDeviceRGB();
		CGBitmapInfo bitmap_info = kCGBitmapByteOrder32Big | kCGImageAlphaPremultipliedLast;//kCGBitmapByteOrderDefault;
		CGDataProviderRef data_provider = CGDataProviderCreateWithData(NULL, start_pixels, row_length*height*bpp, &cocoaTouchCGDataProviderReleaseMallocPixelDataCallback);
		CGImageRef cg_image = CGImageCreate(width, height, 8, 24, row_length*(24/8), color_space, bitmap_info, data_provider, NULL, false, kCGRenderingIntentDefault);
		ui_image = [UIImage imageWithCGImage:cg_image];

		CGColorSpaceRelease(color_space);
        CGDataProviderRelease(data_provider);
        CGImageRelease(cg_image);

	}
	else if(8 == bpp)
	{
		// We'll make a full 32-bit image for this case
		int row_length = CalculateRowLength(width, 4);
		unsigned char* pixels = malloc(row_length*height*(32/8));
		unsigned char* start_pixels = pixels; // we're going to iterate through pixels, but we also want to keep a pointer to the start
		if(NULL == pixels)
		{
			return NULL;
		}	
		unsigned char* source_pixel = imgdata;

		int colors_count = 0;
		iupColor colors[256];
		
		// FIXME: What should we do here?
		int has_alpha = false;

		
		for(int y=0;y<height;y++)
		{
			for(int x=0;x<row_length;x++)
			{
				unsigned char index = *source_pixel;
				iupColor* c = &colors[index];

				*pixels = c->r;
				pixels++;
				*pixels = c->g;
				pixels++;
				*pixels = c->b;
				pixels++;

				if(has_alpha)
				{
					*pixels = c->a;
				}
				else
				{
					*pixels = 255;
				}
				pixels++;
				source_pixel++;

			}
		}


		CGColorSpaceRef color_space = CGColorSpaceCreateDeviceRGB();
		CGBitmapInfo bitmap_info = kCGBitmapByteOrder32Big | kCGImageAlphaPremultipliedLast;//kCGBitmapByteOrderDefault;
		CGDataProviderRef data_provider = CGDataProviderCreateWithData(NULL, start_pixels, row_length*height*bpp, &cocoaTouchCGDataProviderReleaseMallocPixelDataCallback);
		CGImageRef cg_image = CGImageCreate(width, height, 8, 32, row_length*(32/8), color_space, bitmap_info, data_provider, NULL, false, kCGRenderingIntentDefault);
		ui_image = [UIImage imageWithCGImage:cg_image];

		CGColorSpaceRelease(color_space);
        CGDataProviderRelease(data_provider);
        CGImageRelease(cg_image);

	}
		
	// The typical pattern is to call image = iupImageGetImage(),
	// and then call [foo setImage:image];
	// This might imply that we should return as autoreleased.
	// But I think IUP is supposed to run iupdrvImageDestroy() if things are written correctly.
	// That would mean we want to return with a retain count of 1
	return [ui_image retain];
}

int iupdrvImageGetRawInfo(void* handle, int *w, int *h, int *bpp, iupColor* colors, int *colors_count)
{
  /* How to get the pallete? */
  (void)colors;
  (void)colors_count;
  return iupdrvImageGetInfo(handle, w, h, bpp);
}




// NOTE: Returns an UIImage with retain count of 1
void* iupdrvImageCreateImage(Ihandle *ih, const char* bgcolor, int make_inactive)
{
	UIImage* ui_image = nil;
	int bpp;
	int width;
	int height;
	unsigned char* imgdata = (unsigned char*)iupAttribGetStr(ih, "WID");

	width = ih->currentwidth;
	height = ih->currentheight;
	bpp = iupAttribGetInt(ih, "BPP");

	unsigned char bg_r=0, bg_g=0, bg_b=0;
	iupStrToRGB(bgcolor, &bg_r, &bg_g, &bg_b);


	if(32 == bpp)
	{
		int row_length = CalculateRowLength(width, 4);
		unsigned char* pixels = malloc(row_length*height*(32/8));
		unsigned char* start_pixels = pixels; // we're going to iterate through pixels, but we also want to keep a pointer to the start
		if(NULL == pixels)
		{
			return NULL;
		}
		unsigned char* source_pixel = imgdata;

		for(int y=0;y<height;y++)
		{
			for(int x=0;x<row_length;x++)
			{
				unsigned char s_r = *source_pixel;
				source_pixel++;
				unsigned char s_g = *source_pixel;
				source_pixel++;
				unsigned char s_b = *source_pixel;
				source_pixel++;
				unsigned char s_a = *source_pixel;
				source_pixel++;

				if(make_inactive)
				{
					iupImageColorMakeInactive(&s_r, &s_g, &s_b, bg_r, bg_g, bg_b);
				}

				*pixels = s_r;
				pixels++;
				*pixels = s_g;
				pixels++;
				*pixels = s_b;
				pixels++;
				*pixels = s_a;
				pixels++;
			}
		}


		CGColorSpaceRef color_space = CGColorSpaceCreateDeviceRGB();
		CGBitmapInfo bitmap_info = kCGBitmapByteOrder32Big | kCGImageAlphaPremultipliedLast;//kCGBitmapByteOrderDefault;
		CGDataProviderRef data_provider = CGDataProviderCreateWithData(NULL, start_pixels, row_length*height*bpp, &cocoaTouchCGDataProviderReleaseMallocPixelDataCallback);
		CGImageRef cg_image = CGImageCreate(width, height, 8, 32, row_length*(32/8), color_space, bitmap_info, data_provider, NULL, false, kCGRenderingIntentDefault);
		ui_image = [UIImage imageWithCGImage:cg_image];

		CGColorSpaceRelease(color_space);
        CGDataProviderRelease(data_provider);
        CGImageRelease(cg_image);
	}
	else if(24 == bpp)
	{
		int row_length = CalculateRowLength(width, 3);
		unsigned char* pixels = malloc(row_length*height*(24/8));
		unsigned char* start_pixels = pixels; // we're going to iterate through pixels, but we also want to keep a pointer to the start
		if(NULL == pixels)
		{
			return NULL;
		}
		unsigned char* source_pixel = imgdata;

		for(int y=0;y<height;y++)
		{
			for(int x=0;x<row_length;x++)
			{
				unsigned char s_r = *source_pixel;
				source_pixel++;
				unsigned char s_g = *source_pixel;
				source_pixel++;
				unsigned char s_b = *source_pixel;
				source_pixel++;

				if(make_inactive)
				{
					iupImageColorMakeInactive(&s_r, &s_g, &s_b, bg_r, bg_g, bg_b);
				}

				*pixels = s_r;
				pixels++;
				*pixels = s_g;
				pixels++;
				*pixels = s_b;
				pixels++;
			}
		}


		CGColorSpaceRef color_space = CGColorSpaceCreateDeviceRGB();
		CGBitmapInfo bitmap_info = kCGBitmapByteOrder32Big ;//kCGBitmapByteOrderDefault;
		CGDataProviderRef data_provider = CGDataProviderCreateWithData(NULL, start_pixels, row_length*height*bpp, &cocoaTouchCGDataProviderReleaseMallocPixelDataCallback);
		CGImageRef cg_image = CGImageCreate(width, height, 8, 24, row_length*(24/8), color_space, bitmap_info, data_provider, NULL, false, kCGRenderingIntentDefault);
		ui_image = [UIImage imageWithCGImage:cg_image];

		CGColorSpaceRelease(color_space);
        CGDataProviderRelease(data_provider);
        CGImageRelease(cg_image);

	}
	else if(8 == bpp)
	{
		// We'll make a full 32-bit image for this case
		int row_length = CalculateRowLength(width, 4);
		unsigned char* pixels = malloc(row_length*height*(32/8));
		unsigned char* start_pixels = pixels; // we're going to iterate through pixels, but we also want to keep a pointer to the start
		if(NULL == pixels)
		{
			return NULL;
		}	
		unsigned char* source_pixel = imgdata;

		int colors_count = 0;
		iupColor colors[256];
		
		int has_alpha = iupImageInitColorTable(ih, colors, &colors_count);

		
		for(int y=0;y<height;y++)
		{
			for(int x=0;x<row_length;x++)
			{
				unsigned char index = *source_pixel;
				iupColor* c = &colors[index];

				unsigned char s_r = c->r;
				unsigned char s_g = c->g;
				unsigned char s_b = c->b;
				unsigned char s_a;

				if(has_alpha)
				{
					s_a = c->a;
				}
				else
				{
					s_a = 255;
				}

				if(make_inactive)
				{
					iupImageColorMakeInactive(&s_r, &s_g, &s_b, bg_r, bg_g, bg_b);
				}


				*pixels = s_r;
				pixels++;
				*pixels = s_g;
				pixels++;
				*pixels = s_b;
				pixels++;
				*pixels = s_a;
				pixels++;		 
			}
		}


		CGColorSpaceRef color_space = CGColorSpaceCreateDeviceRGB();
		CGBitmapInfo bitmap_info = kCGBitmapByteOrder32Big | kCGImageAlphaPremultipliedLast;//kCGBitmapByteOrderDefault;
		CGDataProviderRef data_provider = CGDataProviderCreateWithData(NULL, start_pixels, row_length*height*bpp, &cocoaTouchCGDataProviderReleaseMallocPixelDataCallback);
		CGImageRef cg_image = CGImageCreate(width, height, 8, 32, row_length*(32/8), color_space, bitmap_info, data_provider, NULL, false, kCGRenderingIntentDefault);
		ui_image = [UIImage imageWithCGImage:cg_image];

		CGColorSpaceRelease(color_space);
        CGDataProviderRelease(data_provider);
        CGImageRelease(cg_image);

	}
		
	int bgcolor_depend = 0;
	if(bgcolor_depend || make_inactive)
	{
		iupAttribSetStr(ih, "_IUP_BGCOLOR_DEPEND", "1");
	}

	// The typical pattern is to call image = iupImageGetImage(),
	// and then call [foo setImage:image];
	// This might imply that we should return as autoreleased.
	// But I think IUP is supposed to run iupdrvImageDestroy() if things are written correctly.
	// That would mean we want to return with a retain count of 1
	return [ui_image retain];
}

void* iupdrvImageCreateIcon(Ihandle *ih)
{
  return iupdrvImageCreateImage(ih, NULL, 0);
}

void* iupdrvImageCreateCursor(Ihandle *ih)
{
#if 0
  int bpp,y,x,hx,hy,
      width = ih->currentwidth,
      height = ih->currentheight,
      line_size = (width+7)/8,
      size_bytes = line_size*height;
  unsigned char *imgdata = (unsigned char*)iupAttribGetStr(ih, "WID");
  char *sbits, *mbits, *sb, *mb;
  unsigned char r, g, b;

  bpp = iupAttribGetInt(ih, "BPP");
  if (bpp > 8)
    return NULL;

  sbits = (char*)malloc(2*size_bytes);
  if (!sbits) return NULL;
  memset(sbits, 0, 2*size_bytes);
  mbits = sbits + size_bytes;

  sb = sbits;
  mb = mbits;
  for (y=0; y<height; y++)
  {
    for (x=0; x<width; x++)
    {
      int byte = x/8;
      int bit = x%8;
      int index = (int)imgdata[y*width+x];
      /* index==0 is transparent */
      if (index == 1)
        sb[byte] = (char)(sb[byte] | (1<<bit));
      if (index != 0)
        mb[byte] = (char)(mb[byte] | (1<<bit));
    }

    sb += line_size;
    mb += line_size;
  }

  hx=0; hy=0;
  iupStrToIntInt(iupAttribGet(ih, "HOTSPOT"), &hx, &hy, ':');

  NSData *tiffData = [NSData dataWithBytes:imgdata length:(width*height*(bpp/8))];
  NSImage *source = [[NSImage alloc] initWithData:tiffData];
  NSSize size = {width,height};
  [source setSize:size]; 

  NSPoint point = {hx,hy};

  NSCursor *cursor = [[NSCursor alloc] initWithImage:source hotSpot:point];

  free(sbits);
  return (void*)CFBridgingRetain(cursor);
#else
	return NULL;
#endif
}

void* iupdrvImageCreateMask(Ihandle *ih)
{
#if 0
  int bpp,y,x,
      width = ih->currentwidth,
      height = ih->currentheight,
      line_size = (width+7)/8,
      size_bytes = line_size*height;
  unsigned char *imgdata = (unsigned char*)iupAttribGetStr(ih, "WID");
  char *bits, *sb;
  unsigned char colors[256];

  bpp = iupAttribGetInt(ih, "BPP");
  if (bpp > 8)
    return NULL;

  bits = (char*)malloc(size_bytes);
  if (!bits) return NULL;
  memset(bits, 0, size_bytes);

  iupImageInitNonBgColors(ih, colors);

  sb = bits;
  for (y=0; y<height; y++)
  {
    for (x=0; x<width; x++)
    {
      int byte = x/8;
      int bit = x%8;
      int index = (int)imgdata[y*width+x];
      if (colors[index])
        sb[byte] = (char)(sb[byte] | (1<<bit));
    }

    sb += line_size;
  }

  NSData *tiffData = [NSData dataWithBytes:imgdata length:(width*height*(bpp/8))];
  NSImage *mask = [[NSImage alloc] initWithData:tiffData];
  NSSize size = {width,height};
  [mask setSize:size]; 
  free(bits);
  return (void*)CFBridgingRetain(mask);
#else
	return NULL;
#endif
}

void* iupdrvImageLoad(const char* name, int type)
{
#if 0
  //int iup2mac[3] = {IMAGE_BITMAP, IMAGE_ICON, IMAGE_CURSOR};
  NSImage *image;
  NSString *path = [[NSString alloc] initWithUTF8String:name];
  image = [[NSImage alloc] initWithContentsOfFile: path];
  NSBitmapImageRep *rep = [[image representations] objectAtIndex: 0];
  // If you think you might get something other than a bitmap image representation,
  // check for it here.

  NSSize size = NSMakeSize ([rep pixelsWide], [rep pixelsHigh]);
  [image setSize: size];
  
  return (void*)CFBridgingRetain(image);
#else
    UIImage* ui_image;

	NSString* bundle_path = [[NSBundle mainBundle] bundlePath];

	NSString* ns_name = [NSString stringWithUTF8String:name];
	
	// Problem: The path either must be absolute, or it must be in the application bundle.
	// TODO: We could also try to look elsewhere if we choose to, but beware of Sandboxing.
	// Do we need to worry about images embedded in the IUP frameworks? (I think not because they are compiled into code.)
	
	// First, just try what was given. This could be an absolute path or current working directory.
    ui_image = [[UIImage alloc] initWithContentsOfFile:ns_name];
	if(nil == ui_image)
	{
		// Next, let's try the app bundle
		NSString* resource_path = [[NSBundle mainBundle] resourcePath];
		NSString* the_path = [resource_path stringByAppendingPathComponent:ns_name];
   		ui_image = [[UIImage alloc] initWithContentsOfFile:the_path];
	}
	
	// giving up
	if(nil == ui_image)
	{
		return NULL;
	}

	return ui_image;
#endif
}

int iupdrvImageGetInfo(void* handle, int *w, int *h, int *bpp)
{
#if 0
  NSImage *image = (__bridge NSImage*)handle;
  NSBitmapImageRep *bitmap = nil;
  if([[image representations] count]>0) bitmap = [[image representations] objectAtIndex:0];
  if(bitmap==nil) return 0;
  if(w) *w = [bitmap pixelsWide];
  if(h) *h = [bitmap pixelsHigh];
  if(bpp) *bpp = [bitmap bitsPerPixel];
#endif
 	UIImage* ui_image = (UIImage*)handle;
	if(nil == ui_image)
	{
		return 0;
	}
	// Careful: Do we want points or pixels?
	// [ui_image size] gives us points.
	// Multiply the [ui_image scale] to convert to pixels.
	// Or more directly, use CGImage to get pixels.
	/*
	CGSize image_size = [ui_image size];
	CGPoint image_scale = [ui_image scale];
	if(w) *w = image_size.width * image_scale;
	if(h) *h = image_size.height * image_scale;
	*/
	CGImageRef cg_image = [ui_image CGImage];
	if(w) *w = (int)CGImageGetWidth(cg_image);
	if(h) *h = (int)CGImageGetHeight(cg_image);
	if(bpp) *bpp = (int)CGImageGetBitsPerComponent(cg_image);

	return 1;
}

// [[UIApplication sharedApplication] setApplicationIconImage: [NSImage imageNamed: @"Icon_name.icns"]]

void iupdrvImageDestroy(void* handle, int type)
{
#if 0
#if __has_feature(objc_arc)
	NSImage* image = CFBridgingRelease(handle);
	
#else
  switch (type)
  {
  case IUPIMAGE_IMAGE:
    [handle release];
    break;
  case IUPIMAGE_ICON:
    [handle release];
    break;
  case IUPIMAGE_CURSOR:
    [handle release];
    break;
  }
#endif
 
#else

 	UIImage* ui_image = (UIImage*)handle;
	[ui_image release];
	
#endif


}

void iupdrvImageGetData(void* handle, unsigned char* imgdata)
{
	
}

