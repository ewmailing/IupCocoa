/** \file
 * \brief iupim utilities
 *
 * See Copyright Notice in "iup.h"
 */

#include <im.h>
#include <im_convert.h>
#include <im_counter.h>
#include <im_util.h>
#include <im_image.h>
#include <im_raw.h>

#include "iup.h"
#include "iupim.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "iup_object.h"
#include "iup_assert.h"
#include "iup_str.h"
#include "iup_image.h"


static void iSaveErrorMsg(int error)
{
  char* lang = IupGetLanguage();
  char* msg;
  if (iupStrEqualNoCase(lang, "ENGLISH"))
  {
    switch (error)
    {
    case IM_ERR_NONE:
      msg = NULL;
    case IM_ERR_OPEN:
      msg = "Error Opening Image File.";
      break;
    case IM_ERR_MEM:
      msg = "Insufficient memory.";
      break;
    case IM_ERR_ACCESS:
      msg = "Error Accessing Image File.";
      break;
    case IM_ERR_DATA:
      msg = "Image type not Supported.";
      break;
    case IM_ERR_FORMAT:
      msg = "Invalid Image File Format.";
      break;
    case IM_ERR_COMPRESS:
      msg = "Invalid or unsupported compression.";
      break;
    default:
      msg = "Unknown Error.";
    }
  }
  else
  {
    switch (error)
    {
    case IM_ERR_NONE:
      msg = NULL;
    case IM_ERR_OPEN:
      msg = "Erro Abrindo Arquivo de Imagem.";
      break;
    case IM_ERR_MEM:
      msg = "Memória Insuficiente.";
      break;
    case IM_ERR_ACCESS:
      msg = "Erro Acessando Arquivo de Imagem.";
      break;
    case IM_ERR_DATA:
      msg = "Tipo de Imagem não Suportado.";
      break;
    case IM_ERR_FORMAT:
      msg = "Formato de Arquivo de Imagem Inválido.";
      break;
    case IM_ERR_COMPRESS:
      msg = "Compressão Inválida ou não Suportada.";
      break;
    default:
      msg = "Erro Desconhecido.";
    }
  }

  IupSetGlobal("IUPIM_LASTERROR", msg);
}

static Ihandle* iupLoadImageFile(imFile* ifile)
{
  int i, error, width, height, color_mode, flags,
    data_type, has_alpha = 0;
  Ihandle* iup_image = NULL;
  void* image_data = NULL;

  error = imFileReadImageInfo(ifile, 0, &width, &height, &color_mode, &data_type);
  if (error)
    goto load_finish;

  flags = IM_TOPDOWN;
  flags |= IM_PACKED;
  if (imColorModeHasAlpha(color_mode))
  {
    has_alpha = 1;
    flags |= IM_ALPHA;
  }

  color_mode = imColorModeToBitmap(color_mode);
  data_type = IM_BYTE;

  image_data = malloc(imImageDataSize(width, height, flags | color_mode, data_type));
  if (!image_data)
    goto load_finish;

  error = imFileReadImageData(ifile, image_data, 1, flags);
  if (error)
    goto load_finish;

  if (color_mode == IM_RGB)
  {
    if (has_alpha)
      iup_image = IupImageRGBA(width, height, (unsigned char*)image_data);
    else
      iup_image = IupImageRGB(width, height, (unsigned char*)image_data);
  }
  else
  {
    const unsigned char* transp_index;
    int palette_count;
    long palette[256];

    imFileGetPalette(ifile, palette, &palette_count);
    iup_image = IupImage(width, height, (unsigned char*)image_data);

    for (i = 0; i < palette_count; i++)
    {
      unsigned char r, g, b;
      imColorDecode(&r, &g, &b, palette[i]);
      IupSetRGBId(iup_image, "", i, r, g, b);
    }

    transp_index = imFileGetAttribute(ifile, "TransparencyIndex", NULL, NULL);
    if (transp_index)
      IupSetAttributeId(iup_image, "", (int)(*transp_index), "BGCOLOR");
  }

load_finish:
  if (image_data) free(image_data);
  iSaveErrorMsg(error);
  return iup_image;
}

Ihandle* IupLoadImage(const char* file_name)
{
  int error;
  Ihandle* iup_image = NULL;
  imCounterCallback old_callback;
  imFile* ifile;

  iupASSERT(file_name);
  if (!file_name)
    return NULL;

  old_callback = imCounterSetCallback(NULL, NULL);

  ifile = imFileOpen(file_name, &error);
  if (!error)
    iup_image = iupLoadImageFile(ifile);
  else
    iSaveErrorMsg(error);

  imCounterSetCallback(NULL, old_callback);

  if (ifile) imFileClose(ifile);
  return iup_image;
}

Ihandle* IupLoadAnimation(const char* file_name)
{
  int error;
  Ihandle* animation = NULL;
  imCounterCallback old_callback;
  imFile* ifile;

  iupASSERT(file_name);
  if (!file_name)
    return NULL;

  old_callback = imCounterSetCallback(NULL, NULL);

  ifile = imFileOpen(file_name, &error);
  if (!error)
  {
    double fps;
    int image_count, i;
    imFileGetInfo(ifile, NULL, NULL, &image_count);

    fps = imFileGetAttribReal(ifile, "FPS", 0);
    if (fps)
    {
      int frametime = iupRound(1000.0 / fps);
      IupSetInt(animation, "FRAMETIME", frametime);
    }

    for (i = 0; i < image_count; i++)
    {
      Ihandle* iup_image = iupLoadImageFile(ifile);
      if (!iup_image)
        break;

      if (!animation)
        animation = IupUser();

      IupAppend(animation, iup_image);
    }
  }
  else
    iSaveErrorMsg(error);

  imCounterSetCallback(NULL, old_callback);

  if (ifile) imFileClose(ifile);
  return animation;
}

Ihandle* IupLoadAnimationFrames(const char** file_name_list, int file_count)
{
  Ihandle* animation = NULL;
  int i;

  iupASSERT(file_name_list);
  if (!file_name_list)
    return NULL;

  for (i = 0; i < file_count; i++)
  {
    Ihandle* iup_image = IupLoadImage(file_name_list[i]);
    if (!iup_image)
      break;

    if (!animation)
      animation = IupUser();

    IupAppend(animation, iup_image);
  }

  return animation;
}

/* Study. Not public yet */
Ihandle* IupLoadImageRaw(const char* file_name, 
                         int width, int height, int data_type, int color_mode,
                         int switch_type, int byte_order, int padding, int start_offset, int ascii)
{
  int error, has_alpha = 0, i, flags, color_space;
  Ihandle* iup_image = NULL;
  void* image_data = NULL;
  imCounterCallback old_callback;
  imFile* ifile;

  old_callback = imCounterSetCallback(NULL, NULL);

  ifile = imFileOpenRaw(file_name, &error);
  if (error)
    goto load_raw_finish;

  /* the bitmap data configuration */
  flags = IM_TOPDOWN;
  flags |= IM_PACKED;
  if (imColorModeHasAlpha(color_mode))
  {
    has_alpha = 1;
    flags |= IM_ALPHA;
  }

  color_space = imColorModeToBitmap(color_mode);

  image_data = malloc(imImageDataSize(width, height, flags|color_space, IM_BYTE));
  if (!image_data)
    goto load_raw_finish;

  /* the original data configuration */
  imFileSetAttribute(ifile, "Width", IM_INT, 1, &width);
  imFileSetAttribute(ifile, "Height", IM_INT, 1, &height);
  imFileSetAttribute(ifile, "ColorMode", IM_INT, 1, &color_mode);
  imFileSetAttribute(ifile, "DataType", IM_INT, 1, &data_type);

  imFileSetAttribute(ifile, "SwitchType", IM_INT, 1, &switch_type);
  imFileSetAttribute(ifile, "ByteOrder", IM_INT, 1, &byte_order);
  imFileSetAttribute(ifile, "Padding", IM_INT, 1, &padding);
  imFileSetAttribute(ifile, "StartOffset", IM_INT, 1, &start_offset);

  if (ascii)
    imFileSetInfo(ifile, "ASCII");

  error = imFileReadImageInfo(ifile, 0, NULL, NULL, NULL, NULL);
  if (error)
    goto load_raw_finish;
  
  error = imFileReadImageData(ifile, image_data, 1, flags);
  if (error)
    goto load_raw_finish;

  if (color_mode == IM_RGB)
  {
    if (has_alpha)
      iup_image = IupImageRGBA(width, height, (unsigned char*)image_data);
    else
      iup_image = IupImageRGB(width, height, (unsigned char*)image_data);
  }
  else
  {
    int palette_count;
    long palette[256];

    imFileGetPalette(ifile, palette, &palette_count);
    iup_image = IupImage(width, height, (unsigned char*)image_data);

    for (i = 0; i < palette_count; i++)
    {
      unsigned char r, g, b;
      imColorDecode(&r, &g, &b, palette[i]);
      IupSetRGBId(iup_image, "", i, r, g, b); 
    }
  }

load_raw_finish:
  imCounterSetCallback(NULL, old_callback);
  if (ifile) imFileClose(ifile);
  if (image_data) free(image_data);
  iSaveErrorMsg(error);
  return iup_image;
}

int IupSaveImage(Ihandle* ih, const char* file_name, const char* format)
{
  int width, height, i, bpp;
  unsigned char* data;
  int error;
  long palette[256];
  imFile* ifile;

  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return 0;

  iupASSERT(file_name);
  if (!file_name)
    return 0;

  ifile = imFileNew(file_name, format, &error);
  if (!ifile)
  {
    iSaveErrorMsg(error);
    return 0;
  }

  data = (unsigned char*)IupGetAttribute(ih, "WID");
  if (!data)
  {
    iSaveErrorMsg(IM_ERR_DATA);
    return 0;
  }

  width = IupGetInt(ih, "WIDTH");
  height = IupGetInt(ih, "HEIGHT");
  bpp = IupGetInt(ih, "BPP");

  if (bpp == 24)
    error = imFileWriteImageInfo(ifile, width, height, IM_RGB|IM_TOPDOWN|IM_PACKED, IM_BYTE);
  else if (bpp == 32)
    error = imFileWriteImageInfo(ifile, width, height, IM_RGB|IM_TOPDOWN|IM_PACKED|IM_ALPHA, IM_BYTE);
  else /* bpp == 8 */
  {
    for(i = 0; i < 256; i++)
    {
      char* color = IupGetAttributeId(ih, "", i);
      if (!color)
        break;

      if (iupStrEqualNoCase(color, "BGCOLOR"))
      {
        unsigned char transp_index = (unsigned char)i;
        imFileSetAttribute(ifile, "TransparencyIndex", IM_BYTE, 1, &transp_index);
        palette[i] = imColorEncode(0, 0, 0);
      }
      else
      {
        unsigned char r, g, b;
        iupStrToRGB(color, &r, &g, &b);
        palette[i] = imColorEncode(r, g, b);
      }
    }

    imFileSetPalette(ifile, palette, i);

    error = imFileWriteImageInfo(ifile, width, height, IM_MAP|IM_TOPDOWN, IM_BYTE);
  }

  if (error == IM_ERR_NONE)
    error = imFileWriteImageData(ifile, data);

  imFileClose(ifile); 

  iSaveErrorMsg(error);

  return error == IM_ERR_NONE? 1: 0;
}

/******************************************************************************/

static void iInitColors(iupColor* colors, int count, long* palette)
{
  int i;
  for (i=0; i< count; i++)
  {
    imColorDecode(&(colors[i].r), &(colors[i].g), &(colors[i].b), palette[i]);
    colors[i].a = 0;
  }
}

static void iInitPalette(long* palette, int count, iupColor* colors)
{
  int i;
  for (i=0; i< count; i++)
  {
    palette[i] = imColorEncode(colors[i].r, colors[i].g, colors[i].b);
  }
}

void* IupGetImageNativeHandle(const imImage* image)
{
  int bpp = image->depth*8;
  iupColor colors[256];
  iInitColors(colors, image->palette_count, image->palette);
  if (image->has_alpha && bpp == 24)
    bpp = 32;
  return iupdrvImageCreateImageRaw(image->width, image->height, bpp, colors, image->palette_count, image->data[0]);
}

imImage* IupGetNativeHandleImage(void* handle)
{
  int width, height, bpp;
  iupColor colors[256];
  int colors_count = 0;
  if (iupdrvImageGetRawInfo(handle, &width, &height, &bpp, colors, &colors_count))
  {
    imImage* image = imImageCreate(width, height, bpp>8? IM_RGB: IM_MAP, IM_BYTE);
    if (image)
    {
      if (bpp==32)
        imImageAddAlpha(image);

      if (bpp<=8 && colors_count)
      {
        long* palette = (long*)malloc(256*sizeof(long));
        iInitPalette(palette, colors_count, colors);
        imImageSetPalette(image, palette, colors_count);
      }

      iupdrvImageGetRawData(handle, image->data[0]);
        
      return image;
    }

    iupdrvImageDestroy(handle, IUPIMAGE_IMAGE);
  }
  return NULL;
}

static void iFlipData(unsigned char* data, int width, int height, int depth)
{
  int line_size = depth*width;
  unsigned char* temp_line = (unsigned char*)malloc(line_size);
  int y, half_height = height / 2;

  for (y = 0; y < half_height; y++)
  {
    int yd = height - 1 - y;
    memcpy(temp_line, data + yd*line_size, line_size);
    memcpy(data + yd*line_size, data + y*line_size, line_size);
    memcpy(data + y*line_size, temp_line, line_size);
  }

  free(temp_line);
}

Ihandle* IupImageFromImImage(const imImage* image)
{
  Ihandle* iup_image = NULL;
  void* image_data;

  if (!imImageIsBitmap(image))
    return NULL;

  if (image->color_space == IM_RGB)
  {
    int color_mode = image->color_space;
    int depth = image->depth;

    if (image->has_alpha)
    {
      color_mode |= IM_ALPHA;
      depth++;
    }

    image_data = malloc(imImageDataSize(image->width, image->height, color_mode, IM_BYTE));
    if (!image_data)
      return NULL;

    /* imImage is always unpacked, IUP is always packed */
    imConvertPacking(image->data[0], image_data, image->width, image->height, depth, depth, IM_BYTE, 0);

    /* imImage is always bottom top, IUP is always top bottom */
    iFlipData(image_data, image->width, image->height, depth);

    if (image->has_alpha)
      iup_image = IupImageRGBA(image->width, image->height, (unsigned char*)image_data);
    else
      iup_image = IupImageRGB(image->width, image->height, (unsigned char*)image_data);

    free(image_data);
  }
  else
  {
    int i;
    const unsigned char* transp_index;

    image_data = image->data[0];

    iup_image = IupImage(image->width, image->height, (unsigned char*)image_data);

    image_data = (unsigned char*)IupGetAttribute(iup_image, "WID");

    /* imImage is always bottom top, IUP is always top bottom */
    iFlipData(image_data, image->width, image->height, 1);

    for (i = 0; i < image->palette_count; i++)
    {
      unsigned char r, g, b;
      imColorDecode(&r, &g, &b, image->palette[i]);
      IupSetRGBId(iup_image, "", i, r, g, b);
    }

    transp_index = imImageGetAttribute(image, "TransparencyIndex", NULL, NULL);
    if (transp_index)
      IupSetAttributeId(iup_image, "", (int)(*transp_index), "BGCOLOR");
  }

  return iup_image;
}
