/** \file
 * \brief GL Image functions.
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>

#include "iup.h"

#include "iup_object.h"
#include "iup_attrib.h"
#include "iup_str.h"
#include "iup_image.h"

#include "iup_glcontrols.h"
#include "iup_glimage.h"


void iupGLColorMakeInactive(unsigned char *r, unsigned char *g, unsigned char *b)
{
  /* 50% blend with white */
  *r = iupALPHABLEND(*r, 255, 128);
  *g = iupALPHABLEND(*g, 255, 128);
  *b = iupALPHABLEND(*b, 255, 128);
}

void iupGLImageGetInfo(const char* name, int *w, int *h, int *bpp)
{
  Ihandle* ih = iupImageGetHandle(name);
  if (!ih)
    return;

  if (w) *w = ih->currentwidth;
  if (h) *h = ih->currentheight;
  if (bpp) *bpp = IupGetInt(ih, "BPP");
}

static unsigned char* iGLImageCreateImage(Ihandle* ih, int make_inactive)
{
  int x, y;
  int w = ih->currentwidth;
  int h = ih->currentheight;
  int channels = IupGetInt(ih, "CHANNELS");
  int depth = channels==1? 3: channels;
  unsigned char* data = (unsigned char*)iupAttribGet(ih, "WID");
  unsigned char* gldata;
  int linesize;

  if (channels == 1)
  {
    iupColor colors[256];
    int colors_count;
    int has_alpha = iupImageInitColorTable(ih, colors, &colors_count);
    if (has_alpha)
      depth = 4;

    gldata = malloc(w*h*depth);
    if (!gldata)
      return NULL;
    linesize = w * depth;

    for (y = 0; y < h; y++)
    {
      unsigned char* line_data = data + y*w;
      unsigned char* line_gldata = gldata + (h - 1 - y)*linesize;

      for (x = 0; x < w; x++)
      {
        unsigned char index = line_data[x];
        iupColor* c = colors + index;
        int offset = x*depth;

        line_gldata[offset + 0] = c->r;
        line_gldata[offset + 1] = c->g;
        line_gldata[offset + 2] = c->b;
        if (has_alpha)
          line_gldata[offset + 3] = c->a;

        if (make_inactive)
        {
          iupGLColorMakeInactive(&line_gldata[offset + 0],
                                 &line_gldata[offset + 1],
                                 &line_gldata[offset + 2]);
        }
      }
    }
  }
  else
  {
    gldata = malloc(w*h*depth);
    if (!gldata)
      return NULL;
    linesize = w * depth;

    for (y = 0; y < h; y++)
    {
      unsigned char* line_data = data + y*linesize;
      unsigned char* line_gldata = gldata + (h-1 - y)*linesize;
      memcpy(line_gldata, line_data, linesize);

      if (make_inactive)
      {
        for (x = 0; x < w; x++)
        {
          int offset = x*depth;

          iupGLColorMakeInactive(&line_gldata[offset + 0],
                                 &line_gldata[offset + 1],
                                 &line_gldata[offset + 2]);
        }
      }
    }
  }

  iupAttribSetInt(ih, "GL_DEPTH", depth);
  return gldata;
}

unsigned char* iupGLImageGetData(Ihandle* ih, int active)
{
  void* data;

  const char *cache_name = "_IUPIMAGE_BUFFER";
  int make_inactive = !active && iupAttribGetInt(ih, "MAKEINACTIVE");
  if (make_inactive)
    cache_name = "_IUPIMAGE_BUFFER_INACTIVE";

  /* Check for an already created native image */
  data = (void*)iupAttribGet(ih, cache_name);
  if (data)
    return data;

  /* Creates the native image */
  data = iGLImageCreateImage(ih, make_inactive);

  /* save the native image in the cache */
  iupAttribSet(ih, cache_name, (char*)data);

  return data;
}

