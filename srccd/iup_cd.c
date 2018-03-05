/** \file
 * \brief IUP Driver
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdlib.h>
#include <stdio.h>

#include "iup.h"

#include <cd.h>
#include <cdiup.h>
#include <cdnative.h>
#include <cddbuf.h>
#include <cdirgb.h>


/* IMPORTANT: this module does NOT depends on the internal cdCanvas structure.
   It depends ONLY on the internal cdContext structure. 
   So no need to rebuild IUPCD when cdCanvas is changed. */
#include <cd_private.h>


static void (*cdcreatecanvasNATIVE)(cdCanvas* canvas, void* data) = NULL;

static void cdcreatecanvasIUP(cdCanvas* canvas, Ihandle *ih_canvas)
{
#ifndef WIN32
  char str[50] = "";
#endif
  char* data;

  if (cdBaseDriver() == CD_BASE_GDK || /* this is also used by the Cairo driver when in GTK3 */
      cdBaseDriver() == CD_BASE_HAIKU)
  {
    data = IupGetAttribute(ih_canvas, "DRAWABLE");  /* new IUP 3 attribute, works for GTK only */
    if (!data)
      return;
  }
  else  /* CD_BASE_WIN or CD_BASE_X */
  {
#ifdef WIN32
    data = IupGetAttribute(ih_canvas, "HWND");  /* new IUP 3 attribute, works for Windows and GTK */
    if (!data)
      data = IupGetAttribute(ih_canvas, "WID"); /* OLD IUP 2 attribute */
    if (!data)
      return;
#else
    void *dpy = IupGetAttribute(ih_canvas, "XDISPLAY");   /* works for Motif and GTK */
    unsigned long wnd = (unsigned long)IupGetAttribute(ih_canvas, "XWINDOW");
    if (!wnd || !dpy)
      return;
    data = str;
#ifdef SunOS_OLD
    sprintf(str, "%d %lu", (int)dpy, wnd); 
#else
    sprintf(str, "%p %lu", dpy, wnd); 
#endif
#endif
  }
  
  /* Inicializa driver NativeWindow */
  cdcreatecanvasNATIVE(canvas, data);

  IupSetAttribute(ih_canvas, "_CD_CANVAS", (char*)canvas);

  {
    int utf8mode = IupGetInt(NULL, "UTF8MODE");
    if (utf8mode)
      cdCanvasSetAttribute(canvas, "UTF8MODE", "1");
  }
}

static cdContext cdIupContext;

cdContext* cdContextIup(void)
{
  /* call this every time, because of ContextPlus */
  cdContext* ctx = cdContextNativeWindow();

  cdcreatecanvasNATIVE = ctx->cxCreateCanvas;

  cdIupContext = *ctx;
  cdIupContext.cxCreateCanvas = cdcreatecanvasIUP;

  return &cdIupContext;
}


/****************************************************************************/
/****************************************************************************/


static void(*cdcreatecanvasDBUFFER)(cdCanvas* canvas, void* data) = NULL;

static void cdcreatecanvasIUP_DBUFFER(cdCanvas* canvas, Ihandle *ih_canvas)
{
  cdCanvas* canvas_dbuffer = cdCreateCanvas(CD_IUP, ih_canvas);
  if (!canvas_dbuffer)
    return;

  cdcreatecanvasDBUFFER(canvas, canvas_dbuffer);

  IupSetAttribute(ih_canvas, "_CD_CANVAS_DBUFFER", (char*)canvas);

  {
    int utf8mode = IupGetInt(NULL, "UTF8MODE");
    if (utf8mode)
      cdCanvasSetAttribute(canvas, "UTF8MODE", "1");
  }

  cdCanvasSetAttribute(canvas, "KILLDBUFFER", "1");  /* automatic kill the canvas_dbuffer */
}

static cdContext cdIupContextDBuffer;

cdContext* cdContextIupDBuffer(void)
{
  /* call this every time, because of ContextPlus */
  cdContext* ctx = cdContextDBuffer();

  cdcreatecanvasDBUFFER = ctx->cxCreateCanvas;

  cdIupContextDBuffer = *ctx;
  cdIupContextDBuffer.cxCreateCanvas = cdcreatecanvasIUP_DBUFFER;

  return &cdIupContextDBuffer;
}


/****************************************************************************/
/****************************************************************************/


static void(*cdcreatecanvasDBUFFERRGB)(cdCanvas* canvas, void* data) = NULL;

static void cdcreatecanvasIUP_DBUFFERRGB(cdCanvas* canvas, Ihandle *ih_canvas)
{
  cdCanvas* canvas_dbuffer = cdCreateCanvas(CD_IUP, ih_canvas);
  if (!canvas_dbuffer)
    return;

  cdcreatecanvasDBUFFERRGB(canvas, canvas_dbuffer);

  IupSetAttribute(ih_canvas, "_CD_CANVAS_DBUFFER", (char*)canvas);

  {
    int utf8mode = IupGetInt(NULL, "UTF8MODE");
    if (utf8mode)
      cdCanvasSetAttribute(canvas, "UTF8MODE", "1");
  }

  cdCanvasSetAttribute(canvas, "KILLDBUFFER", "1");  /* automatic kill the canvas_dbuffer */
}

static cdContext cdIupContextDBufferRGB;

cdContext* cdContextIupDBufferRGB(void)
{
  /* call this every time, because of ContextPlus */
  cdContext* ctx = cdContextDBufferRGB();

  cdcreatecanvasDBUFFERRGB = ctx->cxCreateCanvas;

  cdIupContextDBufferRGB = *ctx;
  cdIupContextDBufferRGB.cxCreateCanvas = cdcreatecanvasIUP_DBUFFERRGB;

  return &cdIupContextDBufferRGB;
}
