/** \file
 * \brief Image Resource.
 *
 * See Copyright Notice in "iup.h"
 */

#include <Bitmap.h>
#include <GraphicsDefs.h>
#include <Screen.h>
#include <TranslationUtils.h>

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#include "iup.h"

#include "iup_object.h"
#include "iup_attrib.h"
#include "iup_str.h"
#include "iup_image.h"
#include "iup_drvinfo.h"

#include "iuphaiku_drv.h"

#define UNIMPLEMENTED printf("%s (%s %d) UNIMPLEMENTED\n",__func__,__FILE__,__LINE__);

void iupdrvImageGetRawData(void* handle, unsigned char* imgdata)
{
	UNIMPLEMENTED
}

void* iupdrvImageCreateImageRaw(int width, int height, int bpp, iupColor* colors, int colors_count, unsigned char *imgdata)
{
	UNIMPLEMENTED
		return NULL;
}

void* iupdrvImageCreateImage(Ihandle *ih, const char* bgcolor, int make_inactive)
{
  int bpp = iupAttribGetInt(ih, "BPP");
  color_space space;
  if (bpp == 8)
    space = B_CMAP8;
  else if (bpp == 32)
	space = B_RGBA32;
  else if(bpp == 24)
	space = B_RGB24;
  else
	UNIMPLEMENTED
  BBitmap* bitmap = new BBitmap(BRect(0, 0, ih->currentwidth - 1, ih->currentheight - 1), space);


  unsigned char* imgdata = (unsigned char*)iupAttribGetStr(ih, "WID");

  if (make_inactive)
  {
    unsigned char bg_r = 0, bg_g = 0, bg_b = 0;
    iupStrToRGB(bgcolor, &bg_r, &bg_g, &bg_b);

	if (bpp >= 24)
    for (int y=0; y<ih->currentheight; y++)
    for (int x=0; x<ih->currentwidth; x++)
	{
      rgb_color* c = (rgb_color*)(imgdata + (x + y * ih->currentwidth) * bpp / 8);
	  if (bpp == 32) {
	    c->red = iupALPHABLEND(c->red, bg_r, c->alpha);
	    c->green = iupALPHABLEND(c->green, bg_r, c->alpha);
	    c->blue = iupALPHABLEND(c->blue, bg_r, c->alpha);
	  }
	  iupImageColorMakeInactive(&c->red, &c->blue, &c->green, bg_r, bg_g, bg_b);
	} else if(bpp == 8) {
	  // FIXME this isn't working too well...
	  rgb_color disable = make_color(bg_r, bg_g, bg_b);
	  unsigned char cid = BScreen().IndexForColor(disable);
      for (int y=0; y<ih->currentheight; y++)
      for (int x=y&1; x<ih->currentwidth; x+=2) {
        *(imgdata + x + y * ih->currentwidth) = cid;
	  }
	} else
	  UNIMPLEMENTED // Not sure how to do that for 16 bit pics ?
	
  }

  bitmap->SetBits(imgdata, ih->currentwidth * ih->currentheight * bpp / 8, 0, space);
	  
  return bitmap;
}

void* iupdrvImageCreateIcon(Ihandle *ih)
{
  return iupdrvImageCreateImage(ih, NULL, 0);
}

void* iupdrvImageCreateCursor(Ihandle *ih)
{
	UNIMPLEMENTED
		return NULL;
}

void* iupdrvImageCreateMask(Ihandle *ih)
{
	UNIMPLEMENTED
		return NULL;
}

void* iupdrvImageLoad(const char* name, int type)
{
	if(type == IUPIMAGE_CURSOR) {
		UNIMPLEMENTED
	} else {
		// TODO the GTK driver attempts to load icons and cursors from the GTK theme
		// Try using the translation kit
		/*
		BBitmap* bitmap = BTranslationUtils::GetBitmap(name);
		if (bitmap == NULL)
		{
			// likely an image from iupimglib ?
			printf("Loading image %s\n", name);
			UNIMPLEMENTED
		}
		return bitmap;
		*/

		// TODO try loading from resources ?
		// UNIMPLEMENTED
	}

	return NULL;
}

int iupdrvImageGetInfo(void* handle, int *w, int *h, int *bpp)
{
  BBitmap* bmp = (BBitmap*)handle;
  BRect r = bmp->Bounds();

  if(w) *w = r.Width();
  if(h) *h = r.Height();
  if(bpp) {
	switch(bmp->ColorSpace()) {
	  case B_RGB32: *bpp=32; break;
	  case B_CMAP8: *bpp=8; break;
	  default: UNIMPLEMENTED
	}
  }
  return 1;
}

int iupdrvImageGetRawInfo(void* handle, int *w, int *h, int *bpp, iupColor* colors, int *colors_count)
{
  /* GdkPixbuf are only 24 bpp or 32 bpp */
  (void)colors;
  (void)colors_count;
  return iupdrvImageGetInfo(handle, w, h, bpp);
}

void iupdrvImageDestroy(void* handle, int type)
{
	UNIMPLEMENTED
}
