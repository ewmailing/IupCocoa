/** \file
 * \brief GL Drawing functions.
 *
 * See Copyright Notice in "iup.h"
 */

#ifdef WIN32
#include <windows.h>
#endif

#if defined (__APPLE__) || defined (OSX)
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif


#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "iup.h"

#include "iup_assert.h"
#include "iup_object.h"
#include "iup_attrib.h"
#include "iup_str.h"
#include "iup_array.h"
#include "iup_image.h"

#include "iup_glcontrols.h"
#include "iup_glfont.h"
#include "iup_gldraw.h"
#include "iup_glicon.h"
#include "iup_glimage.h"


void iupGLDrawLine(Ihandle* ih, int x1, int y1, int x2, int y2, float linewidth, const char* color, int active)
{
  unsigned char r = 0, g = 0, b = 0, a = 255;

  if (!color || linewidth == 0)
    return;

  iupStrToRGBA(color, &r, &g, &b, &a);
  if (!active)
    iupGLColorMakeInactive(&r, &g, &b);

  glColor4ub(r, g, b, a);

  glLineWidth(linewidth);

  /* y is oriented top to bottom in IUP */
  y1 = ih->currentheight - 1 - y1;
  y2 = ih->currentheight - 1 - y2;

  glBegin(GL_LINES);
  glVertex2i(x1, y1);
  glVertex2i(x2, y2);
  glEnd();
}

void iupGLDrawFrameRect(Ihandle* ih, int xmin, int xmax, int ymin, int ymax, float linewidth, const char* color, int active, int title_x, int title_width, int title_height)
{
  unsigned char r = 0, g = 0, b = 0, a = 255;
  int d = 2;

  if (!color || linewidth == 0 || xmin == xmax || ymin == ymax)
    return;

  if (xmin > xmax) { int _t = xmin; xmin = xmax; xmax = _t; }
  if (ymin > ymax) { int _t = ymin; ymin = ymax; ymax = _t; }

  iupStrToRGBA(color, &r, &g, &b, &a);
  if (!active)
    iupGLColorMakeInactive(&r, &g, &b);

  glColor4ub(r, g, b, a);

  glLineWidth(linewidth);

  /* y is oriented top to bottom in IUP */
  ymin = ih->currentheight - 1 - ymin;
  ymax = ih->currentheight - 1 - ymax;

  /* position frame title at left-center */
  ymin -= title_height / 2;

  glBegin(GL_LINE_STRIP);

  glVertex2i(xmin + title_x + title_width, ymin);

  glVertex2i(xmax - d, ymin);
  glVertex2i(xmax, ymin - d);

  glVertex2i(xmax, ymax + d);
  glVertex2i(xmax - d, ymax);

  glVertex2i(xmin + d, ymax);
  glVertex2i(xmin, ymax + d);

  glVertex2i(xmin, ymin - d);
  glVertex2i(xmin + d, ymin);

  glVertex2i(xmin + title_x, ymin);

  glEnd();
}

void iupGLDrawRect(Ihandle* ih, int xmin, int xmax, int ymin, int ymax, float linewidth, const char* color, int active, int round)
{
  unsigned char r = 0, g = 0, b = 0, a = 255;

  if (!color || linewidth == 0 || xmin == xmax || ymin == ymax)
    return;

  if (xmin > xmax) { int _t = xmin; xmin = xmax; xmax = _t; }
  if (ymin > ymax) { int _t = ymin; ymin = ymax; ymax = _t; }

  iupStrToRGBA(color, &r, &g, &b, &a);
  if (!active)
    iupGLColorMakeInactive(&r, &g, &b);

  glColor4ub(r, g, b, a);

  glLineWidth(linewidth);

  /* y is oriented top to bottom in IUP */
  ymin = ih->currentheight - 1 - ymin;
  ymax = ih->currentheight - 1 - ymax;

  glBegin(GL_LINE_LOOP);

  if (round)
  {
    int r = 2;

    glVertex2i(xmin, ymin-r);
    glVertex2i(xmin+r, ymin);

    glVertex2i(xmax-r, ymin);
    glVertex2i(xmax, ymin-r);

    glVertex2i(xmax, ymax+r);
    glVertex2i(xmax-r, ymax);

    glVertex2i(xmin+r, ymax);
    glVertex2i(xmin, ymax+r);
  }
  else
  {
    glVertex2i(xmin, ymin);
    glVertex2i(xmax, ymin);
    glVertex2i(xmax, ymax);
    glVertex2i(xmin, ymax);
  }

  glEnd();
}

static void iGLDrawBuildSmallCircle(int cx, int cy, int rd)
{
  /* Reference: http://slabode.exofire.net/circle_draw.shtml
  Copyright SiegeLord's Abode */
  int i, num_segments = 16;
  double theta = 2 * 3.1415926 / (double)num_segments;
  double c = cos(theta);  /* precalculate the sine and cosine */
  double s = sin(theta);
  double t, x, y;

  x = rd;  /* we start at angle = 0 */
  y = 0;

  for (i = 0; i < num_segments; i++)
  {
    glVertex2d(x + cx, y + cy);

    /* apply the rotation matrix */
    t = x;
    x = c * x - s * y;
    y = s * t + c * y;
  }
}

void iupGLDrawSmallCircle(Ihandle* ih, int cx, int cy, int rd, float linewidth, const char* color, int active)
{
  unsigned char r = 0, g = 0, b = 0, a = 255;

  if (!color || linewidth == 0 || rd == 0)
    return;

  iupStrToRGBA(color, &r, &g, &b, &a);
  if (!active)
    iupGLColorMakeInactive(&r, &g, &b);

  glColor4ub(r, g, b, a);

  glLineWidth(linewidth);

  /* y is oriented top to bottom in IUP */
  cy = ih->currentheight - 1 - cy;

  glBegin(GL_LINE_LOOP);

  iGLDrawBuildSmallCircle(cx, cy, rd);

  glEnd();
}

void iupGLDrawSmallDisc(Ihandle* ih, int cx, int cy, int rd, const char* color, int active)
{
  unsigned char r = 0, g = 0, b = 0, a = 255;

  if (!color || rd == 0)
    return;

  iupStrToRGBA(color, &r, &g, &b, &a);
  if (!active)
    iupGLColorMakeInactive(&r, &g, &b);

  glColor4ub(r, g, b, a);

  /* y is oriented top to bottom in IUP */
  cy = ih->currentheight - 1 - cy;

  glBegin(GL_POLYGON);

  iGLDrawBuildSmallCircle(cx, cy, rd);

  glEnd();
}

void iupGLDrawBox(Ihandle* ih, int xmin, int xmax, int ymin, int ymax, const char* color, int active)
{
  unsigned char r = 0, g = 0, b = 0, a = 255;
  int smooth;

  if (!color || xmin == xmax || ymin == ymax)
    return;

  iupStrToRGBA(color, &r, &g, &b, &a);
  if (!active)
    iupGLColorMakeInactive(&r, &g, &b);

  glColor4ub(r, g, b, a);

  /* y is oriented top to bottom in IUP */
  ymin = ih->currentheight - 1 - ymin;
  ymax = ih->currentheight - 1 - ymax;

  /* must disable polygon smooth or fill may get diagonal lines */
  smooth = glIsEnabled(GL_POLYGON_SMOOTH);
  if (smooth) glDisable(GL_POLYGON_SMOOTH);
  glBegin(GL_QUADS);
  glVertex2i(xmin, ymin);
  glVertex2i(xmax, ymin);
  glVertex2i(xmax, ymax);
  glVertex2i(xmin, ymax);
  glEnd();
  if (smooth) glEnable(GL_POLYGON_SMOOTH);
}

void iupGLDrawPolygon(Ihandle* ih, const int* points, int count, const char* color, int active)
{
  unsigned char r = 0, g = 0, b = 0, a = 255;
  int i, x, y;

  if (!color || count < 3)
    return;

  iupStrToRGBA(color, &r, &g, &b, &a);
  if (!active)
    iupGLColorMakeInactive(&r, &g, &b);

  glColor4ub(r, g, b, a);

  glBegin(GL_POLYGON);

  for (i = 0; i < count; i++)
  {
    x = points[2 * i + 0];
    y = points[2 * i + 1];

    /* y is oriented top to bottom in IUP */
    y = ih->currentheight - 1 - y;

    glVertex2i(x, y);
  }

  glEnd();
}

void iupGLDrawPolyline(Ihandle* ih, const int* points, int count, float linewidth, const char* color, int active, int loop)
{
  unsigned char r = 0, g = 0, b = 0, a = 255;
  int i, x, y;

  if (!color || count < 3)
    return;

  iupStrToRGBA(color, &r, &g, &b, &a);
  if (!active)
    iupGLColorMakeInactive(&r, &g, &b);

  glColor4ub(r, g, b, a);

  glLineWidth(linewidth);

  if (loop)
    glBegin(GL_LINE_LOOP);
  else
    glBegin(GL_LINE_STRIP);

  for (i = 0; i < count; i++)
  {
    x = points[2 * i + 0];
    y = points[2 * i + 1];

    /* y is oriented top to bottom in IUP */
    y = ih->currentheight - 1 - y;

    glVertex2i(x, y);
  }

  glEnd();
}

void iupGLDrawArrow(Ihandle *ih, int x, int y, int size, const char* color, int active, int dir)
{
  int points[6];

  int off1 = iupRound((double)size * 0.13);
  int off2   = iupRound((double)size * 0.87);
  int half = size / 2;

  switch (dir)
  {
  case IUPGL_ARROW_LEFT:  /* arrow points left */
    points[0] = x + off2;
    points[1] = y;
    points[2] = x + off2;
    points[3] = y + size;
    points[4] = x + off1;
    points[5] = y + half;
    break;
  case IUPGL_ARROW_TOP:    /* arrow points top */
    points[0] = x;
    points[1] = y + off2;
    points[2] = x + size;
    points[3] = y + off2;
    points[4] = x + half;
    points[5] = y + off1;
    break;
  case IUPGL_ARROW_RIGHT:  /* arrow points right */
    points[0] = x + off1;
    points[1] = y;
    points[2] = x + off1;
    points[3] = y + size;
    points[4] = x + size - off1;
    points[5] = y + half;
    break;
  case IUPGL_ARROW_BOTTOM:  /* arrow points bottom */
    points[0] = x;
    points[1] = y + off1;
    points[2] = x + size;
    points[3] = y + off1;
    points[4] = x + half;
    points[5] = y + size - off1;
    break;
  }

  iupGLDrawPolygon(ih, points, 3, color, active);
  iupGLDrawPolyline(ih, points, 3, 1, color, active, 1);
}

static int iGLIsOpenGL2orMore(void)
{
  const char* glversion = (const char*)glGetString(GL_VERSION);
  int major = 1, minor = 0;
  iupStrToIntInt(glversion, &major, &minor, '.');
  if (major > 1)
    return 1;
  else
    return 0;
}

static int iGLDestroyTexture_CB(Ihandle* image)
{
  GLuint texture = (GLuint)iupAttribGetInt(image, "GL_TEXTURE");
  if (texture)
    glDeleteTextures(1, &texture);

  texture = (GLuint)iupAttribGetInt(image, "GL_TEXTURE_INACTIVE");
  if (texture)
    glDeleteTextures(1, &texture);

  return IUP_DEFAULT;
}

static GLuint iGLDrawGenTexture(Ihandle* ih, Ihandle* image, int active)
{
  GLuint texture;
  const char *texture_name = "GL_TEXTURE";
  int make_inactive = !active && iupAttribGetInt(ih, "MAKEINACTIVE");
  if (make_inactive)
    texture_name = "GL_TEXTURE_INACTIVE";

  texture = (GLuint)iupAttribGetInt(image, texture_name);
  if (texture)
    return texture;
  else
  {
    unsigned char* gldata = iupGLImageGetData(image, 1);
    int depth = iupAttribGetInt(image, "GL_DEPTH");
    int format = GL_RGB;
    if (depth == 4)
      format = GL_RGBA;

    glGenTextures(1, &texture);

    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, format, image->currentwidth, image->currentheight, 0, format, GL_UNSIGNED_BYTE, gldata);

    iupAttribSetInt(image, texture_name, (int)texture);
    IupSetCallback(image, "DESTROY_CB", iGLDestroyTexture_CB);

    return texture;
  }
}

static void iGLDrawImage(Ihandle* ih, int xmin, int xmax, int ymin, int ymax, Ihandle* image, int active)
{
  if (iGLIsOpenGL2orMore())
  {
    GLuint texture = iGLDrawGenTexture(ih, image, active);
    if (texture)
    {
      int smooth = glIsEnabled(GL_POLYGON_SMOOTH);
      if (smooth) glDisable(GL_POLYGON_SMOOTH);
      glEnable(GL_TEXTURE_2D);

      glBindTexture(GL_TEXTURE_2D, texture);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

      /* y is oriented top to bottom in IUP */
      ymin = ih->currentheight - 1 - ymin;
      ymax = ih->currentheight - 1 - ymax;

      /* y is at image bottom and oriented bottom to top in OpenGL */
      { int tmp = ymin; ymin = ymax; ymax = tmp; }

      glBegin(GL_QUADS);
      glTexCoord2d(0.0, 0.0); glVertex2d(xmin, ymin);
      glTexCoord2d(1.0, 0.0); glVertex2d(xmax + 0.375, ymin);
      glTexCoord2d(1.0, 1.0); glVertex2d(xmax + 0.375, ymax + 0.375);
      glTexCoord2d(0.0, 1.0); glVertex2d(xmin, ymax + 0.375);
      glEnd();

      glDisable(GL_TEXTURE_2D);
      if (smooth) glEnable(GL_POLYGON_SMOOTH);
    }
  }
  else
  {
    /* glDrawPixels is not affected by transformations.
       glRasterPos2i will be clipped if outside the canvas
       and the image will not be drawn.
       So we will avoid these two functions and use textures by default. */
    int rw = xmax - xmin + 1;
    int rh = ymax - ymin + 1;
    unsigned char* gldata = iupGLImageGetData(image, active);
    int depth = iupAttribGetInt(image, "GL_DEPTH");
    int format = GL_RGB;
    if (depth == 4)
      format = GL_RGBA;

    /* y is at image bottom and oriented bottom to top in OpenGL */
    ymin = ymax;

    /* y is oriented top to bottom in IUP */
    ymin = ih->currentheight - 1 - ymin;

    if (image->currentwidth != rw || image->currentheight != rh)
      glPixelZoom((GLfloat)rw / (GLfloat)image->currentwidth, (GLfloat)rh / (GLfloat)image->currentheight);

    glRasterPos2i(xmin, ymin);
    glDrawPixels(image->currentwidth, image->currentheight, format, GL_UNSIGNED_BYTE, gldata);

    if (image->currentwidth != rw || image->currentheight != rh)
      glPixelZoom(1.0f, 1.0f);
  }
}

void iupGLDrawIconImageZoom(Ihandle *ih, int xmin, int xmax, int ymin, int ymax, const char* baseattrib, const char* imagename, int active)
{
  Ihandle* image;

  if (xmin == xmax || ymin == ymax)
    return;

  image = iupGLIconGetImageHandle(ih, baseattrib, imagename, active);
  if (image)
    iGLDrawImage(ih, xmin, xmax, ymin, ymax, image, active);
}

void iupGLDrawIconImage(Ihandle* ih, int x, int y, const char* baseattrib, const char* imagename, int active)
{
  Ihandle* image = iupGLIconGetImageHandle(ih, baseattrib, imagename, active);
  if (image)
    iGLDrawImage(ih, x, x + image->currentwidth - 1, y, y + image->currentheight - 1, image, active);
}

static void iGLDrawText(Ihandle* ih, const char* str, int max_len, int x, int y)
{
  int len, cur_len, lineheight, ascent, baseline;
  const char *nextstr;
  const char *curstr = str;
  int underline = iupAttribGetInt(ih, "UNDERLINE");

  iupGLFontGetDim(ih, NULL, &lineheight, &ascent, NULL);
  baseline = lineheight - ascent;

  /* y is at text baseline and oriented bottom to top in OpenGL */
  y = y + lineheight - baseline;  /* move to baseline */

  /* y is oriented top to bottom in IUP */
  y = ih->currentheight - 1 - y;

  if (underline)
    glLineWidth(1.0f);

  glPushMatrix();
  glTranslated((double)x, (double)y, 0.0);

  do
  {
    nextstr = iupStrNextLine(curstr, &len);
    if (len)
    {
      if (max_len > 0)
      {
        if (nextstr)
          cur_len = (int)(nextstr - str);
        else
          cur_len = (int)(curstr - str) + len;
        if (cur_len > max_len)
        {
          len -= cur_len - max_len;
          if (len < 0)
            break;
          nextstr = NULL;
        }
      }

      iupGLFontRenderString(ih, curstr, len);

      if (underline)
      {
        int width = iupGLFontGetStringWidth(ih, curstr, len);
        glBegin(GL_LINES);
        glVertex2i(0, -2);
        glVertex2i(width - 1, -2);
        glEnd();
      }
    }

    glTranslated(0.0, (double)-lineheight, 0.0);

    curstr = nextstr;
  } while (*nextstr);

  glPopMatrix();
}

void iupGLDrawMultilineText(Ihandle* ih, int x, int y, const char* str, const char* color, int active)
{
  unsigned char r = 0, g = 0, b = 0, a = 255;

  if (!color || !str)
    return;

  iupStrToRGBA(color, &r, &g, &b, &a);
  if (!active)
    iupGLColorMakeInactive(&r, &g, &b);

  glColor4ub(r, g, b, a);

  if (str[0])
    iGLDrawText(ih, str, -1, x, y);
}


/***************************************************************/
/* Utilities */

void IupGLDrawImage(Ihandle* ih, const char* name, int x, int y, int active)
{
  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return;

  iupGLDrawIconImage(ih, x, y, NULL, name, active);
}

void IupGLDrawText(Ihandle* ih, const char* str, int len, int x, int y)
{
  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return;

  if (!str || str[0] == 0)
    return;

  iGLDrawText(ih, str, len, x, y);
}

void IupGLDrawGetTextSize(Ihandle* ih, const char* str, int *w, int *h)
{
  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return;

  iupGLFontGetMultiLineStringSize(ih, str, w, h);
}

void IupGLDrawGetImageInfo(const char* name, int *w, int *h, int *bpp)
{
  iupASSERT(name);
  if (!name)
    return;

  iupGLImageGetInfo(name, w, h, bpp);
}
