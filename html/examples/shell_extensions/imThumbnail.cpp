#include <windows.h>

#include <im.h>
#include <im_image.h>
#include <im_process.h>
#include <im_convert.h>
#include <im_dib.h>


static void imImageViewFitRect(int cnv_width, int cnv_height, int img_width, int img_height, int *w, int *h)
{
  double rView, rImage;
  int correct = 0;

  *w = cnv_width;
  *h = cnv_height;

  rView = ((double)cnv_height) / cnv_width;
  rImage = ((double)img_height) / img_width;

  if ((rView <= 1 && rImage <= 1) || (rView >= 1 && rImage >= 1)) /* view and image are horizontal rectangles */
  {
    if (rView > rImage)
      correct = 2;
    else
      correct = 1;
  }
  else if (rView < 1 && rImage > 1) /* view is a horizontal rectangle and image is a vertical rectangle */
    correct = 1;
  else if (rView > 1 && rImage < 1) /* view is a vertical rectangle and image is a horizontal rectangle */
    correct = 2;

  if (correct == 1)
    *w = (int)(cnv_height / rImage);
  else if (correct == 2)
    *h = (int)(cnv_width * rImage);
}

HBITMAP CreateThumbnail(const char* filename, UINT thumb_size, BOOL &has_alpha)
{
  int error;
  imImage* image = imFileImageLoadBitmap(filename, 0, &error);
  if (!image)
    return NULL;

  if (image->color_space != IM_RGB)
  {
    imImage* rgb_image = imImageCreateBased(image, -1, -1, IM_RGB, -1);
    imConvertColorSpace(image, rgb_image);
    imImageDestroy(image);
    image = rgb_image;
  }

  int thumb_width, thumb_height;
  imImageViewFitRect(thumb_size, thumb_size, image->width, image->height, &thumb_width, &thumb_height);

  imImage* thumb_image = imImageCreateBased(image, thumb_width, thumb_height, -1, -1);
  imProcessResize(image, thumb_image, 1);
  imImageDestroy(image);

  HBITMAP hbmp;
  HDC hDC = GetDC(NULL);
  imDib* dib = imDibSectionFromImage(hDC, &hbmp, thumb_image);
  ReleaseDC(NULL, hDC);

  if (image->has_alpha)
    has_alpha = TRUE;
  else
    has_alpha = FALSE;

  imDibDestroy(dib);
  imImageDestroy(thumb_image);

  return hbmp;
}


#if 0
/* sample code using IM just to create the HBTIMAP */

HBITMAP CreateThumbnail(const char* filename, UINT thumb_size, BOOL &has_alpha)
{
  int plane_size = thumb_size*thumb_size;
  unsigned char *data = (unsigned char *)malloc(sizeof(unsigned char)*plane_size*4);
  unsigned char *red = data;
  unsigned char *green = red + plane_size;
  unsigned char *blue = green + plane_size;
  unsigned char *alpha = blue + plane_size;
  has_alpha = TRUE;

  // Fill data here
  // or use your own image data

  HBITMAP hbmp;
  HDC hDC = GetDC(NULL);
  imDib* dib = imDibCreateSection(hDC, &hbmp, thumb_size, thumb_size, 32);
  ReleaseDC(NULL, hDC);

  imDibEncodeFromRGBA(dib, red, green, blue, alpha);

  //imDibEncodeFromBitmap(dib, data); /* when data is packed (RGBARGBARGBA...) */

  imDibDestroy(dib);

  free(data);

  return hbmp;
}
#endif
