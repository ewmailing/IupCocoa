/** \file
 * \brief Image Resource.
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>

#include "iup.h"

#include "iup_object.h"
#include "iup_attrib.h"
#include "iup_str.h"
#include "iup_image.h"
#include "iup_assert.h"
#include "iup_stdcontrols.h"
#include "iup_drvinfo.h"


static void iDataResize(int src_width, int src_height, unsigned char *src_map, int dst_width, int dst_height, unsigned char *dst_map, int depth)
{
  /* Do bilinear interpolation */

  unsigned char *line_mapl, *line_maph;
  double t, u, src_x, src_y, factor;
  int xl, yl, xh, yh, x, y;
  unsigned char *fhh, *fll, *fhl, *flh;

  int *XL = (int*)malloc(dst_width * sizeof(int));
  double *T = (double*)malloc(dst_width * sizeof(double));

  factor = (double)src_width / (double)dst_width;
  for (x = 0; x < dst_width; x++)
  {
    src_x = x * factor;
    xl = (int)(src_x);
    T[x] = src_x - xl;
    XL[x] = xl;
  }

  factor = (double)src_height / (double)dst_height;

  for (y = 0; y < dst_height; y++)
  {
    src_y = y * factor;
    yl = (int)(src_y);
    yh = (yl == src_height - 1) ? yl : yl + 1;
    u = src_y - yl;

    line_mapl = src_map + yl * src_width * depth;
    line_maph = src_map + yh * src_width * depth;

    for (x = 0; x < dst_width; x++)
    {
      xl = XL[x];
      xh = (xl == src_width - 1) ? xl : xl + 1;
      t = T[x];

      fll = line_mapl + xl * depth;
      fhl = line_mapl + xh * depth;
      flh = line_maph + xl * depth;
      fhh = line_maph + xh * depth;

      dst_map[0] = (unsigned char)(u * t * (fhh[0] - flh[0] - fhl[0] + fll[0]) + t * (fhl[0] - fll[0]) + u * (flh[0] - fll[0]) + fll[0]);
      dst_map[1] = (unsigned char)(u * t * (fhh[1] - flh[1] - fhl[1] + fll[1]) + t * (fhl[1] - fll[1]) + u * (flh[1] - fll[1]) + fll[1]);
      dst_map[2] = (unsigned char)(u * t * (fhh[2] - flh[2] - fhl[2] + fll[2]) + t * (fhl[2] - fll[2]) + u * (flh[2] - fll[2]) + fll[2]);
      if (depth == 4)
        dst_map[3] = (unsigned char)(u * t * (fhh[3] - flh[3] - fhl[3] + fll[3]) + t * (fhl[3] - fll[3]) + u * (flh[3] - fll[3]) + fll[3]);

      dst_map += depth;
    }
  }

  free(XL);
  free(T);
}

static void iupImageResize(Ihandle* ih, int width, int height)
{
  unsigned char* imgdata = (unsigned char*)iupAttribGetStr(ih, "WID");
  int channels = iupAttribGetInt(ih, "CHANNELS");
  int count = width*height*channels;
  unsigned char* new_imgdata = (unsigned char *)malloc(count);

  iDataResize(ih->currentwidth, ih->currentheight, imgdata, width, height, new_imgdata, channels);

  ih->currentwidth = width;
  ih->currentheight = height;

  free(imgdata);
  iupAttribSet(ih, "WID", (char*)new_imgdata);
}


/**************************************************************************************************/


typedef struct _IimageStock
{
  iupImageStockCreateFunc func;
  Ihandle* image;            /* cache image */
  const char* native_name;   /* used to map to GTK stock images */
} IimageStock;

static Itable *istock_table = NULL;   /* the image hash table indexed by the name string */

void iupImageStockInit(void)
{
  istock_table = iupTableCreate(IUPTABLE_STRINGINDEXED);
}

void iupImageStockFinish(void)
{
  char* name = iupTableFirst(istock_table);
  while (name)
  {
    IimageStock* istock = (IimageStock*)iupTableGetCurr(istock_table);
    if (iupObjectCheck(istock->image))
      IupDestroy(istock->image);
    memset(istock, 0, sizeof(IimageStock));
    free(istock);
    name = iupTableNext(istock_table);
  }

  iupTableDestroy(istock_table);
  istock_table = NULL;
}

void iupImageStockSet(const char *name, iupImageStockCreateFunc func, const char* native_name)
{
  IimageStock* istock = (IimageStock*)iupTableGet(istock_table, name);
  if (istock)
    free(istock);  /* overwrite a previous registration */

  istock = (IimageStock*)malloc(sizeof(IimageStock));
  istock->func = func;
  istock->image = NULL;
  istock->native_name = native_name;

  iupTableSet(istock_table, name, (void*)istock, IUPTABLE_POINTER);
}

int iupImageStockGetSize(void)
{
  char* stock_size = IupGetGlobal("IMAGESTOCKSIZE");
  if (stock_size)
  {
    int size = 16;
    iupStrToInt(stock_size, &size);

    if (size <= 16) 
      return 16;
    else if (size <= 24) 
      return 24;
    else if (size <= 32)
      return 32;
    else
      return 48;
  }
  else
  {
    int dpi = (int)(iupdrvGetScreenDpi() + 0.6);

    if (dpi <= 96)
      return 16;
    else if (dpi <= 144)
      return 24;
    else if (dpi <= 192)
      return 32;
    else 
      return 48;
  }
}

static void iImageStockGet(const char* name, Ihandle* *ih, const char* *native_name)
{
  IimageStock* istock = (IimageStock*)iupTableGet(istock_table, name);
  if (istock)
  {
    if (istock->image)
      *ih = istock->image;
    else if (istock->native_name)
        *native_name = istock->native_name;
    else if (istock->func)
    {
      int stock_size, bpp;

      istock->image = istock->func();
      *ih = istock->image;

      stock_size = iupImageStockGetSize();
      bpp = IupGetInt(istock->image, "BPP");

      if (istock->image->currentheight != stock_size && bpp > 8)
      {
        int new_width = stock_size;
        if (istock->image->currentwidth != istock->image->currentheight)
          new_width = (stock_size * istock->image->currentwidth) / istock->image->currentheight;

        iupImageResize(istock->image, new_width, stock_size);
      }
    }
  }
}

static void iImageStockUnload(const char* name)
{
  IimageStock* istock = (IimageStock*)iupTableGet(istock_table, name);
  if (istock)
    istock->image = NULL;
}

static void iImageStockLoad(const char *name)
{
  /* Used only in iupImageStockLoadAll */
  const char* native_name = NULL;
  Ihandle* ih = NULL;
  iImageStockGet(name, &ih, &native_name);
  if (ih)
  {
    IupSetHandle(name, ih);
    iupAttribSetStr(ih, "_IUPSTOCK_LOAD", name);
  }
  else if (native_name)
  {
    /* dummy image to save the GTK stock name */
    void* handle = iupdrvImageLoad(native_name, IUPIMAGE_IMAGE);
    if (handle)
    {
      int w, h, bpp;
      iupdrvImageGetInfo(handle, &w, &h, &bpp);
      if (bpp == 32)
        ih = IupImageRGBA(w,h,NULL);
      else
        ih = IupImageRGB(w,h,NULL);
      iupAttribSet(ih, "_IUPSTOCK_LOAD_HANDLE", (char*)handle);
      IupSetHandle(name, ih);
    }
  }
}

static int iImageStockExtended(const char* name)
{
  int len = (int)strlen(name);
  if (len > 2 && iup_isdigit(name[len - 1]) && iup_isdigit(name[len - 2]))
    return 1;
  return 0;
}

void iupImageStockLoadAll(void)
{
  /* Used only in IupView */
  char* name = iupTableFirst(istock_table);
  while (name)
  {
    if (!iImageStockExtended(name))
      iImageStockLoad(name);
    name = iupTableNext(istock_table);
  }
}


/**************************************************************************************************/


int iupImageNormBpp(int bpp)
{
  if (bpp <= 8) return 8;
  if (bpp <= 24) return 24;
  return 32;
}

static void iupColorSet(iupColor *c, unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
  c->r = r;
  c->g = g;
  c->b = b;
  c->a = a;
}

int iupImageInitColorTable(Ihandle *ih, iupColor* colors, int *colors_count)
{
  char *value;
  unsigned char red, green, blue;
  int i, has_alpha = 0;
  static iupColor default_colors[] = {
    { 0,0,0,255 }, { 128,0,0,255 }, { 0,128,0,255 }, { 128,128,0,255 },    
    { 0,0,128,255 }, { 128,0,128,255 }, { 0,128,128,255 }, { 192,192,192,255 },    
    { 128,128,128,255 }, { 255,0,0,255 }, { 0,255,0,255 }, { 255,255,0,255 },
    { 0,0,255,255 }, { 255,0,255,255 }, { 0,255,255,255 }, { 255,255,255,255 } };

  memset(colors, 0, sizeof(iupColor)*256);

  for (i=0;i<16;i++)
  {
    value = iupAttribGetId(ih, "", i);

    if (value)
    {
      if (iupStrEqual(value, "BGCOLOR"))
      {
        iupColorSet(&colors[i], 0, 0, 0, 0);
        has_alpha = 1;
      }
      else
      {
        if (!iupStrToRGB(value, &red, &green, &blue))
          iupColorSet(&colors[i], default_colors[i].r, default_colors[i].g, default_colors[i].b, 255);
        else
          iupColorSet(&colors[i], red, green, blue, 255);
      }
    }
    else
    {
      iupColorSet(&colors[i], default_colors[i].r, default_colors[i].g, default_colors[i].b, 255);
    }
  }

  for (;i<256;i++)
  {
    value = iupAttribGetId(ih, "", i);
    if (!value)
      break;

    if (iupStrEqual(value, "BGCOLOR"))
    {
      iupColorSet(&colors[i], 0, 0, 0, 0);
      has_alpha = 1;
    }
    else
    {
      if (!iupStrToRGB(value, &red, &green, &blue))
        break;

      iupColorSet(&colors[i], red, green, blue, 255);
    }
  }

  if (colors_count) *colors_count = i;

  return has_alpha;
}

void iupImageInitNonBgColors(Ihandle* ih, unsigned char *colors)
{
  char *value;
  int i;

  memset(colors, 0, 256);

  for (i=0;i<16;i++)
  {
    value = iupAttribGetId(ih, "", i);
    if (!iupStrEqual(value, "BGCOLOR"))
      colors[i] = 1;
  }

  for (;i<256;i++)
  {
    value = iupAttribGetId(ih, "", i);
    if (!value)
      break;

    if (!iupStrEqual(value, "BGCOLOR"))
      colors[i] = 1;
  }
}

void iupImageColorMakeInactive(unsigned char *r, unsigned char *g, unsigned char *b, unsigned char bg_r, unsigned char bg_g, unsigned char bg_b)
{
  if (*r==bg_r && *g==bg_g && *b==bg_b)  /* preserve colors identical to the background color */
  {
    *r = bg_r; 
    *g = bg_g; 
    *b = bg_b; 
  }
  else
  {
    int ir = 0, ig = 0, ib = 0, 
      i = (*r+*g+*b)/3,
      bg_i = (bg_r+bg_g+bg_b)/3;

    if (bg_i)
    {
      ir = (bg_r*i)/bg_i; 
      ig = (bg_g*i)/bg_i; 
      ib = (bg_b*i)/bg_i; 
    }

#define LIGHTER(_c) ((255 + _c)/2)
    ir = LIGHTER(ir);
    ig = LIGHTER(ig);
    ib = LIGHTER(ib);

    *r = iupBYTECROP(ir);
    *g = iupBYTECROP(ig);
    *b = iupBYTECROP(ib);
  }
}


/**************************************************************************************************/


static Ihandle* iImageGetImageFromName(const char* name)
{
  Ihandle* ih = IupGetHandle(name);
  if (ih)
  {
    int bpp = IupGetInt(ih, "BPP");
    char* autoscale = iupAttribGet(ih, "AUTOSCALE");
    if (!autoscale) autoscale = IupGetGlobal("IMAGEAUTOSCALE");
    if (autoscale && bpp > 8 && !iupAttribGet(ih, "SCALED"))
    {
      float scale = 0;

      if (iupStrEqualNoCase(autoscale, "DPI"))
      {
        int dpi = (int)(iupdrvGetScreenDpi() + 0.6);
        int images_dpi = IupGetInt(NULL, "IMAGESDPI");
        if (images_dpi == 0) images_dpi = 96;

        if (dpi <= 96)
          dpi = 96;
        else if (dpi <= 144)
          dpi = 144;
        else if (dpi <= 192)
          dpi = 192;
        else
          dpi = 288;

        scale = (float)dpi / (float)images_dpi;
      }
      else
        iupStrToFloat(autoscale, &scale);

      if (scale > 0 && (scale < 0.99f || scale > 1.01))
      {
        char* hotspot = iupAttribGet(ih, "HOTSPOT");

        int new_width = iupRound(scale*ih->currentwidth);
        int new_height = iupRound(scale*ih->currentheight);

        iupImageResize(ih, new_width, new_height);
        iupAttribSet(ih, "SCALED", "1");

        if (hotspot)
        {
          int x = 0, y = 0;
          iupStrToIntInt(hotspot, &x, &y, ':');

          x = iupRound(scale*x);
          y = iupRound(scale*y);

          iupAttribSetStrf(ih, "HOTSPOT", "%d:%d", x, y);
        }
      }
    }
  }

  return ih;
}

void* iupImageGetMask(const char* name)
{
  void* mask;
  Ihandle *ih;

  if (!name)
    return NULL;

  ih = iImageGetImageFromName(name);
  if (!ih)
    return NULL;
  
  /* Check for an already created icon */
  mask = iupAttribGet(ih, "_IUPIMAGE_MASK");
  if (mask)
    return mask;

  /* Not created, tries to create the mask */
  mask = iupdrvImageCreateMask(ih);

  /* save the mask */
  iupAttribSet(ih, "_IUPIMAGE_MASK", (char*)mask);

  return mask;
}

void* iupImageGetIcon(const char* name)
{
  void* icon;
  Ihandle *ih;

  if (!name)
    return NULL;

  ih = iImageGetImageFromName(name);
  if (!ih)
  {
    const char* native_name = NULL;

    /* Check in the system resources. */
    icon = iupdrvImageLoad(name, IUPIMAGE_ICON);
    if (icon) 
      return icon;

    /* Check in the stock images. */
    iImageStockGet(name, &ih, &native_name);
    if (native_name)
    {
      icon = iupdrvImageLoad(native_name, IUPIMAGE_ICON);
      if (icon)
        return icon;
    }

    return NULL;
  }
  
  /* Check for an already created icon */
  icon = iupAttribGet(ih, "_IUPIMAGE_ICON");
  if (icon)
    return icon;

  /* Not created, tries to create the icon */
  icon = iupdrvImageCreateIcon(ih);

  /* save the icon */
  iupAttribSet(ih, "_IUPIMAGE_ICON", (char*)icon);

  return icon;
}

void* iupImageGetCursor(const char* name)
{
  void* cursor;
  Ihandle *ih;

  if (!name)
    return NULL;

  ih = iImageGetImageFromName(name);
  if (!ih)
  {
    /* Check in the system resources. */
    cursor = iupdrvImageLoad(name, IUPIMAGE_CURSOR);
    if (cursor) 
      return cursor;

    return NULL;
  }
  
  /* Check for an already created cursor */
  cursor = iupAttribGet(ih, "_IUPIMAGE_CURSOR");
  if (cursor)
    return cursor;

  /* Not created, tries to create the cursor */
  cursor = iupdrvImageCreateCursor(ih);

  /* save the cursor */
  iupAttribSet(ih, "_IUPIMAGE_CURSOR", (char*)cursor);

  return cursor;
}

void iupImageGetInfo(const char* name, int *w, int *h, int *bpp)
{
  void* handle;
  Ihandle *ih;

  if (!name)
    return;

  ih = iImageGetImageFromName(name);
  if (!ih)
  {
    const char* native_name = NULL;

    /* Check in the system resources. */
    handle = iupdrvImageLoad(name, IUPIMAGE_IMAGE);
    if (handle)
    {
      iupdrvImageGetInfo(handle, w, h, bpp);
      return;
    }

    /* Check in the stock images. */
    iImageStockGet(name, &ih, &native_name);
    if (native_name) 
    {
      handle = iupdrvImageLoad(native_name, IUPIMAGE_IMAGE);
      if (handle) 
      {
        iupdrvImageGetInfo(handle, w, h, bpp);
        return;
      }
    }

    if (!ih)
      return;
  }

  if (w) *w = ih->currentwidth;
  if (h) *h = ih->currentheight;
  if (bpp) *bpp = IupGetInt(ih, "BPP");
}

void* iupImageGetImage(const char* name, Ihandle* ih_parent, int make_inactive)
{
  char cache_name[100] = "_IUPIMAGE_IMAGE";
  char* bgcolor;
  void* handle;
  Ihandle *ih;
  int bg_concat = 0;

  if (!name)
    return NULL;

  ih = iImageGetImageFromName(name);
  if (!ih)
  {
    const char* native_name = NULL;

    /* Check in the system resources. */
    handle = iupdrvImageLoad(name, IUPIMAGE_IMAGE);
    if (handle) 
      return handle;

    /* Check in the stock images. */
    iImageStockGet(name, &ih, &native_name);
    if (native_name) 
    {
      handle = iupdrvImageLoad(native_name, IUPIMAGE_IMAGE);
      if (handle) 
        return handle;
    }

    if (!ih)
      return NULL;
  }

  handle = iupAttribGet(ih, "_IUPSTOCK_LOAD_HANDLE");
  if (handle)
    return handle;

  bgcolor = iupAttribGet(ih, "BGCOLOR");
  if (ih_parent && !bgcolor)
    bgcolor = IupGetAttribute(ih_parent, "BGCOLOR"); /* Use IupGetAttribute to use inheritance and native implementation */

  if (make_inactive)
    strcat(cache_name, "_INACTIVE");

  if (iupAttribGet(ih, "_IUP_BGCOLOR_DEPEND") && bgcolor)
  {
    strcat(cache_name, "(");
    strcat(cache_name, bgcolor);
    strcat(cache_name, ")");
    bg_concat = 1;
  }
  
  /* Check for an already created native image */
  handle = (void*)iupAttribGet(ih, cache_name);
  if (handle)
    return handle;

  if (ih_parent && iupAttribGetStr(ih_parent, "FLAT_ALPHA"))
    iupAttribSet(ih, "FLAT_ALPHA", "1");

  /* Creates the native image */
  handle = iupdrvImageCreateImage(ih, bgcolor, make_inactive);

  if (ih_parent && iupAttribGetStr(ih_parent, "FLAT_ALPHA"))
    iupAttribSet(ih, "FLAT_ALPHA", NULL);

  if (iupAttribGet(ih, "_IUP_BGCOLOR_DEPEND") && bgcolor && !bg_concat)  /* _IUP_BGCOLOR_DEPEND could be set during creation */
  {
    strcat(cache_name, "(");
    strcat(cache_name, bgcolor);
    strcat(cache_name, ")");
  }

  /* save the native image in the cache */
  iupAttribSet(ih, cache_name, (char*)handle);

  return handle;
}

static Ihandle* iImageGetHandleFromImage(void* handle)
{
  Ihandle* ih = NULL;
  int w, h, bpp, i;
  iupColor colors[256];
  int colors_count = 0;
  if (iupdrvImageGetRawInfo(handle, &w, &h, &bpp, colors, &colors_count))
  {
    unsigned char* imgdata;

    if (bpp == 32)
      ih = IupImageRGBA(w,h,NULL);
    else if (bpp > 8)
      ih = IupImageRGB(w,h,NULL);
    else
      ih = IupImage(w,h,NULL);

    if (bpp <= 8 && colors_count)
    {
      for (i = 0; i < colors_count; i++)
        IupSetRGBId(ih, "", i, colors[i].r, colors[i].g, colors[i].b);
    }

    imgdata = (unsigned char*)iupAttribGetStr(ih, "WID");
    iupdrvImageGetData(handle, imgdata);

    iupdrvImageDestroy(handle, IUPIMAGE_IMAGE);
  }

  return ih;
}

Ihandle* iupImageGetHandle(const char* name)
{
  Ihandle *ih;
  const char* native_name = NULL;
  void* handle;

  if (!name)
    return NULL;

  ih = iImageGetImageFromName(name);
  if (ih)
    return ih;

  /* Check in the system resources. */
  handle = iupdrvImageLoad(name, IUPIMAGE_IMAGE);
  if (handle)
  {
    ih = iImageGetHandleFromImage(handle);
    if (ih)
    {
      IupSetHandle(name, ih);
      return ih;
    }
  }

  /* Check in the stock images. */
  iImageStockGet(name, &ih, &native_name);
  if (ih)
  {
    IupSetHandle(name, ih);
    return ih;
  }

  if (native_name)
  {
    handle = iupdrvImageLoad(native_name, IUPIMAGE_IMAGE);
    if (handle)
    {
      ih = iImageGetHandleFromImage(handle);
      if (ih)
      {
        IupSetHandle(name, ih);
        return ih;
      }
    }
  }

  return NULL;
}

void iupImageUpdateParent(Ihandle *ih)  /* ih here is the element that contains images */
{
  /* Called when BGCOLOR is changed */
  /* it will re-create the image, if the case */

  char* value = iupAttribGet(ih, "IMAGE");
  if (value) 
    iupAttribSetClassObject(ih, "IMAGE", value);

  value = iupAttribGet(ih, "IMINACTIVE");
  if (value) 
    iupAttribSetClassObject(ih, "IMINACTIVE", value);

  value = iupAttribGet(ih, "IMPRESS");
  if (value) 
    iupAttribSetClassObject(ih, "IMPRESS", value);
}

void iupImageClearFromCache(Ihandle* ih, void* handle)
{
  char *name;
  void* cur_handle;

  name = iupTableFirst(ih->attrib);
  while (name)
  {
    if (iupStrEqualPartial(name, "_IUPIMAGE_"))
    {
      cur_handle = iupTableGetCurr(ih->attrib);
      if (cur_handle == handle)
      {
        iupTableRemoveCurr(ih->attrib);
        return;
      }
    }

    name = iupTableNext(ih->attrib);
  }
}

static void iImageClearCache(Ihandle* ih)
{
  char *name;
  void* handle;

  handle = iupAttribGet(ih, "_IUPIMAGE_MASK");
  if (handle)
  {
    iupdrvImageDestroy(handle, IUPIMAGE_IMAGE);
    iupAttribSet(ih, "_IUPIMAGE_MASK", NULL);
  }

  handle = iupAttribGet(ih, "_IUPIMAGE_ICON");
  if (handle)
  {
    iupdrvImageDestroy(handle, IUPIMAGE_ICON);
    iupAttribSet(ih, "_IUPIMAGE_ICON", NULL);
  }

  handle = iupAttribGet(ih, "_IUPIMAGE_CURSOR");
  if (handle)
  {
    iupdrvImageDestroy(handle, IUPIMAGE_CURSOR);
    iupAttribSet(ih, "_IUPIMAGE_CURSOR", NULL);
  }

  /* the remaining native images are all IUPIMAGE_IMAGE(*) - depends on BGCOLOR/INACTIVE/etc  */
  name = iupTableFirst(ih->attrib);
  while (name)
  {
    if (iupStrEqualPartial(name, "_IUPIMAGE_IMAGE"))
    {
      handle = iupTableGetCurr(ih->attrib);
      if (handle)
      {
        iupdrvImageDestroy(handle, IUPIMAGE_IMAGE);
        iupTableSetCurr(ih->attrib, NULL, IUPTABLE_POINTER);
      }
    }

    name = iupTableNext(ih->attrib);
  }

  /* additional image buffer when an IupImage is converted to one (CD, OpenGL, etc) */
  handle = iupAttribGet(ih, "_IUPIMAGE_BUFFER");
  if (handle)
  {
    iupAttribSet(ih, "_IUPIMAGE_BUFFER", NULL);
    free(handle);
  }

  handle = iupAttribGet(ih, "_IUPIMAGE_BUFFER_INACTIVE");
  if (handle)
  {
    iupAttribSet(ih, "_IUPIMAGE_BUFFER_INACTIVE", NULL);
    free(handle);
  }
}


/******************************************************************************/


static char* iImageGetWidthAttrib(Ihandle *ih)
{
  return iupStrReturnInt(ih->currentwidth);
}

static char* iImageGetHeightAttrib(Ihandle *ih)
{
  return iupStrReturnInt(ih->currentheight);
}

static int iImageCreate(Ihandle* ih, void** params, int bpp)
{
  int width, height, channels, count;
  unsigned char *imgdata;

  iupASSERT(params!=NULL);
  if (!params)
    return IUP_ERROR;

  width = (int)(params[0]);
  height = (int)(params[1]);

  iupASSERT(width>0);
  iupASSERT(height>0);

  if (width <= 0 || height <= 0)
    return IUP_ERROR;

  ih->currentwidth = width;
  ih->currentheight = height;

  channels = 1;  /* bpp == 8 */
  if (bpp == 24)
    channels = 3;
  else if (bpp == 32)
    channels = 4;

  count = width*height*channels;
  imgdata = (unsigned char *)malloc(count);

  if (((int)(params[2])==-1) || ((int)(params[3])==-1)) /* compacted in one pointer */
  {
    if ((int)(params[2])!=-1)
      memcpy(imgdata, params[2], count);
  }
  else /* one param for each pixel/plane */
  {
    int i;
    for(i=0; i<count; i++)
    {
      imgdata[i] = (unsigned char)((int)(params[i+2]));
    }
  }

  iupAttribSet(ih, "WID", (char*)imgdata);
  iupAttribSetInt(ih, "BPP", bpp);
  iupAttribSetInt(ih, "CHANNELS", channels);

  return IUP_NOERROR;
}

static int iImageCreateMethod(Ihandle* ih, void** params)
{
  return iImageCreate(ih, params, 8);
}

static int iImageRGBCreateMethod(Ihandle* ih, void** params)
{
  return iImageCreate(ih, params, 24);
}

static int iImageRGBACreateMethod(Ihandle* ih, void** params)
{
  return iImageCreate(ih, params, 32);
}

static void iImageDestroyMethod(Ihandle* ih)
{
  char* stock_name;

  unsigned char* imgdata = (unsigned char*)iupAttribGetStr(ih, "WID");
  if (imgdata)
  {
    iupAttribSet(ih, "WID", NULL);
    free(imgdata);
  }

  stock_name = iupAttribGet(ih, "_IUPSTOCK_LOAD");
  if (stock_name)
    iImageStockUnload(stock_name);

  iImageClearCache(ih);
}


/******************************************************************************/


Ihandle* IupImage(int width, int height, const unsigned char *imgdata)
{
  void *params[4];
  params[0] = (void*)width;
  params[1] = (void*)height;
  params[2] = imgdata? (void*)imgdata: (void*)-1;
  params[3] = (void*)-1;
  return IupCreatev("image", params);
}

Ihandle* IupImageRGB(int width, int height, const unsigned char *imgdata)
{
  void *params[4];
  params[0] = (void*)width;
  params[1] = (void*)height;
  params[2] = imgdata? (void*)imgdata: (void*)-1;
  params[3] = (void*)-1;
  return IupCreatev("imagergb", params);
}

Ihandle* IupImageRGBA(int width, int height, const unsigned char *imgdata)
{
  void *params[4];
  params[0] = (void*)width;
  params[1] = (void*)height;
  params[2] = imgdata? (void*)imgdata: (void*)-1;
  params[3] = (void*)-1;
  return IupCreatev("imagergba", params);
}

static Iclass* iImageNewClassBase(char* name)
{
  Iclass* ic = iupClassNew(NULL);

  ic->name = name;
  ic->format = "iic"; /* (int,int,unsigned char*) */
  ic->nativetype = IUP_TYPEIMAGE;
  ic->childtype = IUP_CHILDNONE;
  ic->is_interactive = 0;

  /* Class functions */
  ic->Destroy = iImageDestroyMethod;

  /* Attribute functions */
  iupClassRegisterAttribute(ic, "WID", NULL, NULL, NULL, NULL, IUPAF_READONLY|IUPAF_NO_INHERIT|IUPAF_NO_STRING);
  iupClassRegisterAttribute(ic, "WIDTH", iImageGetWidthAttrib, NULL, NULL, NULL, IUPAF_READONLY|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "HEIGHT", iImageGetHeightAttrib, NULL, NULL, NULL, IUPAF_READONLY|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "RASTERSIZE", iupBaseGetRasterSizeAttrib, NULL, NULL, NULL, IUPAF_READONLY|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "BGCOLOR", NULL, NULL, IUPAF_SAMEASSYSTEM, "DLGBGCOLOR", IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "BPP",      NULL, NULL, NULL, NULL, IUPAF_READONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "CHANNELS", NULL, NULL, NULL, NULL, IUPAF_READONLY|IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "HOTSPOT", NULL, NULL, "0:0", NULL, IUPAF_NO_INHERIT);

  return ic;
}

Iclass* iupImageNewClass(void)
{
  Iclass* ic = iImageNewClassBase("image");
  ic->New = iupImageNewClass;
  ic->Create = iImageCreateMethod;
  return ic;
}

Iclass* iupImageRGBNewClass(void)
{
  Iclass* ic = iImageNewClassBase("imagergb");
  ic->New = iupImageRGBNewClass;
  ic->Create = iImageRGBCreateMethod;
  return ic;
}

Iclass* iupImageRGBANewClass(void)
{
  Iclass* ic = iImageNewClassBase("imagergba");
  ic->New = iupImageRGBANewClass;
  ic->Create = iImageRGBACreateMethod;
  return ic;
}


/******************************************************************************/


#if 0
/*****************************************************/
/* This strategy generates libraries that are bigger */
/*****************************************************/
static int SaveImageC(const char* file_name, Ihandle* ih, const char* name, FILE* packfile)
{
  int y, x, width, height, channels, linesize;
  unsigned char* data;
  FILE* file;

  if (packfile)
    file = packfile;
  else
    file = fopen(file_name, "wb");

  if (!file)
    return 0;

  width = IupGetInt(ih, "WIDTH");
  height = IupGetInt(ih, "HEIGHT");
  channels = IupGetInt(ih, "CHANNELS");
  linesize = width*channels;

  data = (unsigned char*)IupGetAttribute(ih, "WID");

  if (fprintf(file, "static Ihandle* load_image_%s(void)\n", name)<0)
  {
    if (!packfile)
      fclose(file);
    return 0;
  }

  fprintf(file, "{\n");

  if (channels == 1)
  {
    int c;
    char* color;

    fprintf(file, "  Ihandle* image = IupImage(%d, %d, NULL);\n\n", width, height);

    for (c = 0; c < 256; c++)
    {
      color = IupGetAttributeId(ih, "", c);
      if (!color)
        break;

      fprintf(file, "  IupSetAttribute(image, \"%d\", \"%s\");\n", c, color);
    }

    fprintf(file, "\n");
  }
  else if (channels == 3)
    fprintf(file, "  Ihandle* image = IupImageRGB(%d, %d, NULL);\n", width, height);
  else /* channels == 4 */
    fprintf(file, "  Ihandle* image = IupImageRGBA(%d, %d, NULL);\n", width, height);

  fprintf(file, "  unsigned char* d = (unsigned char*)IupGetAttribute(image, \"WID\");\n");

  for (y = 0; y < height; y++)
  {
    fprintf(file, "    ");

    for (x = 0; x < linesize; x++)
      fprintf(file, "*d++ = %d; ", (int)(*data++));

    fprintf(file, "\n");
  }

  fprintf(file, "  return image;\n");
  fprintf(file, "}\n\n");

  if (!packfile)
    fclose(file);

  return 1;
}
#endif

static int SaveImageC(const char* file_name, Ihandle* ih, const char* name, FILE* packfile)
{
  int y, x, width, height, channels, linesize;
  unsigned char* data;
  FILE* file;

  if (packfile)
    file = packfile;
  else
    file = fopen(file_name, "wb");

  if (!file)
    return 0;

  width = IupGetInt(ih, "WIDTH");
  height = IupGetInt(ih, "HEIGHT");
  channels = IupGetInt(ih, "CHANNELS");
  linesize = width*channels;

  data = (unsigned char*)IupGetAttribute(ih, "WID");

  if (fprintf(file, "static Ihandle* load_image_%s(void)\n", name)<0)
  {
    if (!packfile)
      fclose(file);
    return 0;
  }

  fprintf(file, "{\n");
  fprintf(file, "  unsigned char imgdata[] = {\n");

  for (y = 0; y < height; y++)
  {
    fprintf(file, "    ");

    for (x = 0; x < linesize; x++)
    {
      if (x != 0)
        fprintf(file, ", ");

      fprintf(file, "%d", (int)data[y*linesize+x]);
    }

    if (y == height-1)
      fprintf(file, "};\n\n");
    else
      fprintf(file, ",\n");
  }

  if (channels == 1)
  {
    int c;
    char* color;

    fprintf(file, "  Ihandle* image = IupImage(%d, %d, imgdata);\n\n", width, height);

    for (c = 0; c < 256; c++)
    {
      color = IupGetAttributeId(ih, "", c);
      if (!color)
        break;

      fprintf(file, "  IupSetAttribute(image, \"%d\", \"%s\");\n", c, color);
    }

    fprintf(file, "\n");
  }
  else if (channels == 3)
    fprintf(file, "  Ihandle* image = IupImageRGB(%d, %d, imgdata);\n", width, height);
  else /* channels == 4 */
    fprintf(file, "  Ihandle* image = IupImageRGBA(%d, %d, imgdata);\n", width, height);

  fprintf(file, "  return image;\n");
  fprintf(file, "}\n\n");

  if (!packfile)
    fclose(file);

  return 1;
}

static int SaveImageLua(const char* file_name, Ihandle* ih, const char* name, FILE* packfile)
{
  int y, x, width, height, channels, linesize;
  unsigned char* data;
  FILE* file;

  if (packfile)
    file = packfile;
  else
    file = fopen(file_name, "wb");

  if (!file)
    return 0;
  
  width = IupGetInt(ih, "WIDTH");
  height = IupGetInt(ih, "HEIGHT");
  channels = IupGetInt(ih, "CHANNELS");
  linesize = width*channels;

  data = (unsigned char*)IupGetAttribute(ih, "WID");

  if (fprintf(file, "function load_image_%s()\n", name)<0)
  {
    if (!packfile)
      fclose(file);
    return 0;
  }

  if (channels == 1)
    fprintf(file, "  local %s = iup.image\n", name);
  else if (channels == 3)
    fprintf(file, "  local %s = iup.imagergb\n", name);
  else /* channels == 4 */
    fprintf(file, "  local %s = iup.imagergba\n", name);

  fprintf(file, "  {\n");

  fprintf(file, "    width = %d,\n", width);
  fprintf(file, "    height = %d,\n", height);
  fprintf(file, "    pixels = {\n");

  for (y = 0; y < height; y++)
  {
    fprintf(file, "      ");
    for (x = 0; x < linesize; x++)
    {
      fprintf(file, "%d, ", (int)data[y*linesize+x]);
    }
    fprintf(file, "\n");
  }

  fprintf(file, "    },\n");

  if (channels == 1)
  {
    int c;
    char* color;
    unsigned char r, g, b;

    fprintf(file, "    colors = {\n");

    for(c = 0; c < 256; c++)
    {
      color = IupGetAttributeId(ih, "", c);
      if (!color)
        break;

      if (iupStrEqualNoCase(color, "BGCOLOR"))
        fprintf(file, "      \"BGCOLOR\",\n");
      else
      {
        iupStrToRGB(color, &r, &g, &b);
        fprintf(file, "      \"%d %d %d\",\n", (int)r, (int)g, (int)b);
      }
    }

    fprintf(file, "    }\n");
  }

  fprintf(file, "  }\n");

  fprintf(file, "  return %s\n", name);
  fprintf(file, "end\n\n");

  if (!packfile)
    fclose(file);

  return 1;
}

static int SaveImageLED(const char* file_name, Ihandle* ih, const char* name, FILE* packfile)
{
  int y, x, width, height, channels, linesize;
  unsigned char* data;
  FILE* file;

  if (packfile)
    file = packfile;
  else
    file = fopen(file_name, "wb");

  if (!file)
    return 0;

  width = IupGetInt(ih, "WIDTH");
  height = IupGetInt(ih, "HEIGHT");
  channels = IupGetInt(ih, "CHANNELS");
  linesize = width*channels;

  data = (unsigned char*)IupGetAttribute(ih, "WID");

  if (channels == 1)
  {
    int c;
    unsigned char r, g, b;
    char* color;

    if (fprintf(file, "%s = IMAGE\n", name)<0)
    {
      if (!packfile)
        fclose(file);
      return 0;
    }

    fprintf(file, "[\n");
    for(c = 0; c < 256; c++)
    {
      color = IupGetAttributeId(ih, "", c);
      if (!color)
      {
        if (c < 16)
          continue;
        else
          break;
      }

      if (c != 0)
        fprintf(file, ",\n");

      if (iupStrEqualNoCase(color, "BGCOLOR"))
        fprintf(file, "  %d = \"BGCOLOR\"", c);
      else
      {
        iupStrToRGB(color, &r, &g, &b);
        fprintf(file, "  %d = \"%d %d %d\"", c, (int)r, (int)g, (int)b);
      }
    }
    fprintf(file, "\n]\n");
  }
  else if (channels == 3)
  {
    if (fprintf(file, "%s = IMAGERGB\n", name)<0)
    {
      if (!packfile)
        fclose(file);
      return 0;
    }
  }
  else /* channels == 4 */
  {
    if (fprintf(file, "%s = IMAGERGBA\n", name)<0)
    {
      if (!packfile)
        fclose(file);
      return 0;
    }
  }

  fprintf(file, "(%d, %d,\n", width, height);

  for (y = 0; y < height; y++)
  {
    fprintf(file, "  ");
    for (x = 0; x < linesize; x++)
    {
      if (y == height-1 && x==linesize-1)
        fprintf(file, "%d", (int)data[y*linesize+x]);
      else
        fprintf(file, "%d, ", (int)data[y*linesize+x]);
    }
    fprintf(file, "\n");
  }

  fprintf(file, ")\n\n");

  if (!packfile)
    fclose(file);

  return 1;
}

int iupSaveImageAsText(Ihandle* ih, FILE* packfile, const char* format, const char* name)
{
  int ret = 0;
  if (iupStrEqualNoCase(format, "LED"))
    ret = SaveImageLED(NULL, ih, name, packfile);
  else if (iupStrEqualNoCase(format, "LUA"))
    ret = SaveImageLua(NULL, ih, name, packfile);
  else if (iupStrEqualNoCase(format, "C"))
    ret = SaveImageC(NULL, ih, name, packfile);
  return ret;
}

int IupSaveImageAsText(Ihandle* ih, const char* file_name, const char* format, const char* name)
{
  int ret = 0;
  if (!name)
  {
    name = IupGetName(ih);
    if (!name)
      name = "image";
  }
  if (iupStrEqualNoCase(format, "LED"))
    ret = SaveImageLED(file_name, ih, name, NULL);
  else if (iupStrEqualNoCase(format, "LUA"))
    ret = SaveImageLua(file_name, ih, name, NULL);
  else if (iupStrEqualNoCase(format, "C"))
    ret = SaveImageC(file_name, ih, name, NULL);
  return ret;
}
