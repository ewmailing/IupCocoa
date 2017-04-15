#include <windows.h>

#include <iup.h>
#include <stdio.h>

//#define USE_OPEN_GL
#define USE_IM

#ifdef USE_OPEN_GL
#include <GL/gl.h> 
#include <iupgl.h>
#else
#include <cd.h>
#include <cdiup.h>
#include <cdcgm.h>
#include <cdemf.h>
#endif

#ifdef USE_IM
#include <im.h>
#include <im_image.h>
#include <im_convert.h>
#endif

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

static int canvas_redraw(Ihandle* ih)
{
  char* filename = IupGetAttribute(ih, "PATHFILE");

#ifdef USE_OPEN_GL
  int cnv_w, cnv_h;
  IupGetIntInt(ih, "DRAWSIZE", &cnv_w, &cnv_h);

  IupGLMakeCurrent(ih);

  glViewport(0, 0, cnv_w, cnv_h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, cnv_w, 0, cnv_h, -1, 1);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glClearColor(1.0, 1.0, 1.0, 1.f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glColor3f(0.0,1.0,0.0);  /* green in OpenGL */

  glBegin(GL_LINES);
  glVertex2i(0, 0);
  glVertex2i(cnv_w - 1, cnv_h - 1);
  glEnd();

  glBegin(GL_LINES);
  glVertex2i(0, cnv_h - 1);
  glVertex2i(cnv_w - 1, 0);
  glEnd();

#ifdef USE_IM
  int error;
  imImage* image = imFileImageLoadBitmap(filename, 0, &error);
  if (!image)
    return IUP_DEFAULT;

  int glformat;
  void* glImage = imImageGetOpenGLData(image, &glformat);
  if (!glImage)
  {
    imImageDestroy(image);
    return IUP_DEFAULT;
  }

  int x, y, w, h;
  imImageViewFitRect(cnv_w, cnv_h, image->width, image->height, &w, &h);
  x = (cnv_w - w) / 2;
  y = (cnv_h - h) / 2;

  glPixelZoom((GLfloat)w / (GLfloat)image->width, (GLfloat)h / (GLfloat)image->height);
  glRasterPos2d(x, y);
  glDrawPixels(image->width, image->height, glformat, GL_UNSIGNED_BYTE, glImage);

  imImageDestroy(image);
#endif

  IupGLSwapBuffers(ih);
#else
  cdCanvas* canvas = (cdCanvas*)IupGetAttribute(ih, "_CD_CANVAS_DBUFFER");

  cdCanvasActivate(canvas);
  cdCanvasBackground(canvas, CD_WHITE);
  cdCanvasClear(canvas);

  int cnv_w, cnv_h;
  cdCanvasGetSize(canvas, &cnv_w, &cnv_h, 0, 0);

  cdCanvasForeground(canvas, CD_RED);
  cdCanvasLine(canvas, 0, 0, cnv_w - 1, cnv_h - 1);
  cdCanvasLine(canvas, 0, cnv_h - 1, cnv_w - 1, 0);

#ifdef USE_IM
  int error;
  imImage* image = imFileImageLoadBitmap(filename, 0, &error);

  int x, y, w, h;
  imImageViewFitRect(cnv_w, cnv_h, image->width, image->height, &w, &h);
  x = (cnv_w - w) / 2;
  y = (cnv_h - h) / 2;

  imcdCanvasPutImage(canvas, image, x, y, w, h, 0, 0, 0, 0);

  imImageDestroy(image);
#else
  //  cdCanvasPlay(canvas, CD_CGM, 0, cnv_w - 1, 0, cnv_h - 1, filename);
  cdCanvasPlay(canvas, CD_EMF, 0, cnv_w - 1, 0, cnv_h - 1, filename);
#endif

  cdCanvasText(canvas, 0, 0, "CD_IUPDBUFFER");

  cdCanvasFlush(canvas);
#endif
  return IUP_DEFAULT;
}

#ifndef USE_OPEN_GL
static int canvas_map(Ihandle* ih)
{
  cdCreateCanvas(CD_IUPDBUFFER, ih);
  return IUP_DEFAULT;
}

static int canvas_unmap(Ihandle* ih)
{
  cdCanvas* canvas = (cdCanvas*)IupGetAttribute(ih, "_CD_CANVAS_DBUFFER");
  cdKillCanvas(canvas);
  return IUP_DEFAULT;
}
#endif

static Ihandle* canvas_create()
{
#ifdef USE_OPEN_GL
  IupGLCanvasOpen();

  Ihandle* cnv = IupGLCanvas(NULL);
  IupSetAttribute(cnv, "BUFFER", "DOUBLE");
#else
  Ihandle* cnv = IupCanvas(0);
  IupSetCallback(cnv, "MAP_CB", canvas_map);
  IupSetCallback(cnv, "UNMAP_CB", canvas_unmap);
#endif

  IupSetCallback(cnv, "ACTION", canvas_redraw);

  return cnv;
}

HWND CreatePreviewWindow(HWND hNativeParent, int width, int height, const char* filename)
{
  Ihandle* cnv = canvas_create();

  Ihandle* dialog = IupDialog(cnv);
  IupSetAttribute(dialog, "BORDER", "NO");
  IupSetAttribute(dialog, "MAXBOX", "NO");
  IupSetAttribute(dialog, "MINBOX", "NO");
  IupSetAttribute(dialog, "MENUBOX", "NO");
  IupSetAttribute(dialog, "RESIZE", "NO");
  IupSetAttribute(dialog, "CONTROL", "YES");

  IupSetStrAttribute(dialog, "PATHFILE", filename);

  IupSetAttribute(dialog, "NATIVEPARENT", (char*)hNativeParent);

  if (width && height) IupSetStrf(dialog, "RASTERSIZE", "%dx%d", width, height);
  IupMap(dialog);
  if (width && height) IupSetAttribute(dialog, "RASTERSIZE", NULL);

#ifdef XXX_DEBUG
  // printf("CreatePreviewWindow(%d, %d)\n", width, height);
  // MessageBox(NULL, L"ShowWindow", L"IUP", MB_OK);
#endif

  return (HWND)IupGetAttribute(dialog, "HWND");
}
