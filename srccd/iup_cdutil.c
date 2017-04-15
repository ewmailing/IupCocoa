/** \file
 * \brief cdiuputil. CD and IUP utilities for the IupControls
 *
 * See Copyright Notice in "iup.h"
 */


#include <stdlib.h>
#include <stdarg.h>
#include <memory.h>

#include <cd.h>

#include "iup.h"
#include "iup_attrib.h"
#include "iup_str.h"
#include "iup_drv.h"
#include "iup_cdutil.h"
#include "iup_image.h"


long cdIupConvertColor(const char *color)
{
  unsigned char r, g, b;
  if (iupStrToRGB(color, &r, &g, &b))
    return cdEncodeColor(r, g, b);
  else
    return 0;
}

void cdIupCalcShadows(long bgcolor, long *light_shadow, long *mid_shadow, long *dark_shadow)
{
  int r, bg_r = cdRed(bgcolor);
  int g, bg_g = cdGreen(bgcolor);
  int b, bg_b = cdBlue(bgcolor);

  /* light_shadow */

  int max = bg_r;
  if (bg_g > max) max = bg_g;
  if (bg_b > max) max = bg_b;

  if (255-max < 64)
  {
    r = 255;
    g = 255;
    b = 255;
  }
  else
  {
    /* preserve some color information */
    if (bg_r == max) r = 255;
    else             r = bg_r + (255-max);
    if (bg_g == max) g = 255;
    else             g = bg_g + (255-max);
    if (bg_b == max) b = 255;
    else             b = bg_b + (255-max);
  }

  if (light_shadow) *light_shadow = cdEncodeColor((unsigned char)r, (unsigned char)g, (unsigned char)b); 

  /* dark_shadow */
  r = bg_r - 128;
  g = bg_g - 128;
  b = bg_b - 128;
  if (r < 0) r = 0;
  if (g < 0) g = 0;
  if (b < 0) b = 0;

  if (dark_shadow) *dark_shadow = cdEncodeColor((unsigned char)r, (unsigned char)g, (unsigned char)b); 

  /*   mid_shadow = (dark_shadow+bgcolor)/2    */
  if (mid_shadow) *mid_shadow = cdEncodeColor((unsigned char)((bg_r+r)/2), (unsigned char)((bg_g+g)/2), (unsigned char)((bg_b+b)/2));
}

void cdIupDrawSunkenRect(cdCanvas *canvas, int x1, int y1, int x2, int y2, long light_shadow, long mid_shadow, long dark_shadow)
{
  cdCanvasForeground(canvas, mid_shadow);
  cdCanvasLine(canvas, x1, y1+1,   x1, y2);
  cdCanvasLine(canvas, x1,  y2, x2-1, y2);

  cdCanvasForeground(canvas, dark_shadow);
  cdCanvasLine(canvas, x1+1, y1+2, x1+1, y2-1);
  cdCanvasLine(canvas, x1+1, y2-1, x2-2, y2-1);

  cdCanvasForeground(canvas, light_shadow);
  cdCanvasLine(canvas, x1, y1, x2, y1);
  cdCanvasLine(canvas, x2, y1, x2, y2);
}

void cdIupDrawRaiseRect(cdCanvas *canvas, int x1, int y1, int x2, int y2, long light_shadow, long mid_shadow, long dark_shadow)
{
  cdCanvasForeground(canvas, light_shadow);
  cdCanvasLine(canvas, x1, y1+1,   x1, y2);
  cdCanvasLine(canvas, x1,  y2, x2-1, y2);

  cdCanvasForeground(canvas, dark_shadow);
  cdCanvasLine(canvas, x1, y1, x2, y1);
  cdCanvasLine(canvas, x2, y1, x2, y2);

  cdCanvasForeground(canvas, mid_shadow);
  cdCanvasLine(canvas, x1+1, y1+1, x2-1, y1+1);
  cdCanvasLine(canvas, x2-1, y1+2, x2-1, y2-1);
}

void cdIupDrawVertSunkenMark(cdCanvas *canvas, int x, int y1, int y2, long light_shadow, long dark_shadow)
{
  cdCanvasForeground(canvas, dark_shadow);
  cdCanvasLine(canvas, x-1, y1, x-1, y2);
  cdCanvasForeground(canvas, light_shadow);
  cdCanvasLine(canvas,   x, y1,   x, y2);
}

void cdIupDrawHorizSunkenMark(cdCanvas *canvas, int x1, int x2, int y, long light_shadow, long dark_shadow)
{
  cdCanvasForeground(canvas, dark_shadow);
  cdCanvasLine(canvas, x1, y+1, x2, y+1);
  cdCanvasForeground(canvas, light_shadow);
  cdCanvasLine(canvas, x1, y, x2, y);
}


/************************************************************************/


static void iUtilInitPalette(Ihandle* image, long* palette, long bgcolor, int make_inactive)
{
  int i;
  unsigned char r, g, b, bg_r, bg_g, bg_b;

  cdDecodeColor(bgcolor, &bg_r, &bg_g, &bg_b);

  for(i = 0; i < 256; i++)
  {
    char* color = IupGetAttributeId(image, "", i);
    r = bg_r; g = bg_g; b = bg_b;
    iupStrToRGB(color, &r, &g, &b);  /* no need to test for BGCOLOR, if this failed it will not set the parameters */

    if (make_inactive)
      iupImageColorMakeInactive(&r, &g, &b, bg_r, bg_g, bg_b);

    palette[i] = cdEncodeColor(r, g, b);
  }
}

static unsigned char* iUtilBuildImageBuffer(Ihandle *image, int width, int height, int depth, int make_inactive, long bgcolor)
{
  int size, plane_size, i, j;
  unsigned char bg_r, bg_g, bg_b;
  unsigned char* image_buffer;
  /* images from stock or resources are not supported */
  unsigned char* data = (unsigned char*)IupGetAttribute(image, "WID");
  if (!data)
    return NULL;

  plane_size = width*height;
  size = plane_size*depth;

  image_buffer = malloc(size);
  if (!image_buffer)
    return NULL;

  cdDecodeColor(bgcolor, &bg_r, &bg_g, &bg_b);

  /* IUP image is top-down, CD image is bottom up */
  if (depth==1)
  {
    /* inactive will be set at the palette */
    for (i=0; i<height; i++)
      memcpy(image_buffer + i*width, data + (height-1 - i)*width, width);
  }
  else if (depth==3)
  {
    int pos;
    unsigned char r, g, b;
    unsigned char *dst_r = image_buffer + 0*plane_size;
    unsigned char *dst_g = image_buffer + 1*plane_size;
    unsigned char *dst_b = image_buffer + 2*plane_size;

    for (i=0; i<height; i++)
    {
      int line_offset = i*width;
      unsigned char *line_r = dst_r + line_offset;
      unsigned char *line_g = dst_g + line_offset;
      unsigned char *line_b = dst_b + line_offset;
      unsigned char *src_rgb = data + (height-1 - i)*(3*width);

      for (j=0; j<width; j++)
      {
        pos = j*3;
        r = src_rgb[pos+0];
        g = src_rgb[pos+1];
        b = src_rgb[pos+2];

        if (make_inactive)
          iupImageColorMakeInactive(&r, &g, &b, bg_r, bg_g, bg_b);

        line_r[j] = r;
        line_g[j] = g;
        line_b[j] = b;
      }
    }
  }
  else /* depth==4 */
  {
    int pos;
    unsigned char r, g, b, a;
    unsigned char *dst_r = image_buffer + 0*plane_size;
    unsigned char *dst_g = image_buffer + 1*plane_size;
    unsigned char *dst_b = image_buffer + 2*plane_size;
    unsigned char *dst_a = image_buffer + 3*plane_size;

    for (i=0; i<height; i++)
    {
      int line_offset = i*width;
      unsigned char *line_r = dst_r + line_offset;
      unsigned char *line_g = dst_g + line_offset;
      unsigned char *line_b = dst_b + line_offset;
      unsigned char *line_a = dst_a + line_offset;
      unsigned char *src_rgba = data + (height-1 - i)*(4*width);

      for (j=0; j<width; j++)
      {
        pos = j*4;
        r = src_rgba[pos+0];
        g = src_rgba[pos+1];
        b = src_rgba[pos+2];
        a = src_rgba[pos+3];

        if (make_inactive)
          iupImageColorMakeInactive(&r, &g, &b, bg_r, bg_g, bg_b);

        line_r[j] = r;
        line_g[j] = g;
        line_b[j] = b;
        line_a[j] = a;
      }
    }
  }

  return image_buffer;
}

void cdIupDrawImage(cdCanvas *canvas, Ihandle *image, int x, int y, int w, int h, int make_inactive, long bgcolor)
{
  int plane_size, depth;
  int width = IupGetInt(image, "WIDTH");
  int height = IupGetInt(image, "HEIGHT");
  int bpp = IupGetInt(image, "BPP");
  unsigned char* image_buffer;

  depth = 1;
  if (bpp==24)
    depth = 3;
  else if (bpp==32)
    depth = 4;

  if (w == 0) w = width;
  if (h == 0) h = height;

  /* The buffer will be destroyed in IupImage unmap */
  if (depth!=1 && make_inactive)
    image_buffer = (unsigned char*)iupAttribGet(image, "_IUPIMAGE_BUFFER_INACTIVE");
  else
    image_buffer = (unsigned char*)iupAttribGet(image, "_IUPIMAGE_BUFFER");

  if (!image_buffer)
  {
    image_buffer = iUtilBuildImageBuffer(image, width, height, depth, make_inactive, bgcolor);

    if (depth!=1 && make_inactive)
      iupAttribSet(image, "_IUPIMAGE_BUFFER_INACTIVE", (char*)image_buffer);
    else
      iupAttribSet(image, "_IUPIMAGE_BUFFER", (char*)image_buffer);
  }

  if (!image_buffer)
    return;

  plane_size = width*height;

  if (depth==1)
  {
    long palette[256];
    iUtilInitPalette(image, palette, bgcolor, make_inactive);

    cdCanvasPutImageRectMap(canvas, width, height, 
                            image_buffer, palette, 
                            x, y, w, h, 0, 0, 0, 0);
  }
  else if (depth==3)
  {
    cdCanvasPutImageRectRGB(canvas, width, height,
                            image_buffer + 0*plane_size,
                            image_buffer + 1*plane_size,
                            image_buffer + 2*plane_size,
                            x, y, w, h, 0, 0, 0, 0);
  }
  else /* depth==4 */
  {
    cdCanvasPutImageRectRGBA(canvas, width, height,
                             image_buffer + 0*plane_size,
                             image_buffer + 1*plane_size,
                             image_buffer + 2*plane_size,
                             image_buffer + 3*plane_size,
                             x, y, w, h, 0, 0, 0, 0);
  }
}


/************************************************************************/


void IupCdSetFont(Ihandle* ih, cdCanvas *canvas, const char* font)
{
  char* lastfont = iupAttribGetStr(ih, "_IUPCD_LASTFONT");
  if (!lastfont || !iupStrEqual(lastfont, font))
  {
    iupAttribSetStr(ih, "_IUPCD_LASTFONT", font);
    cdCanvasNativeFont(canvas, font);
  }
}

void IupCdDrawFocusRect(Ihandle* ih, cdCanvas *canvas, int x1, int y1, int x2, int y2)
{
  int y, x, w, h;

#ifdef WIN32
  void* gc = cdCanvasGetAttribute(canvas, "HDC");
  if (!gc)
    gc = cdCanvasGetAttribute(canvas, "GC");  /* for Cairo running in Windows */
#else
  void* gc = cdCanvasGetAttribute(canvas, "GC");  /* works for X11, GDK and Cairo */
#endif

  cdCanvasUpdateYAxis(canvas, &y1);
  cdCanvasUpdateYAxis(canvas, &y2);
  y = y1;
  if (y2<y1) y = y2;
  x = x1;
  if (x2<x1) x = x2;

  w = abs(x2 - x1) + 1;
  h = abs(y2 - y1) + 1;

  iupdrvPaintFocusRect(ih, gc, x, y, w, h);
}

