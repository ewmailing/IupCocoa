#include <stdlib.h>
#include <stdio.h>
#include "iup.h"
#include "cd.h"
#include "cdiup.h"
#include "wd.h"


/* World:
   The canvas will be a window into that space.
   If canvas is smaller than the virtual space, scrollbars are active.
   The drawing is a red X connecting the corners of the world,
   plus a box inside the borders.

   Remember that:
   XMIN<=POSX<=XMAX-DX
*/
#define WORLD_XMAX 599
#define WORLD_YMAX 399

static void update_scrollbar(Ihandle* ih, int canvas_w, int canvas_h)
{
  /* update page size, it is always the client size of the canvas,
     If you change canvas size must call this function. */
  IupSetFloat(ih, "DX", (float)canvas_w/((float)WORLD_XMAX+1));
  IupSetFloat(ih, "DY", (float)canvas_h/((float)WORLD_YMAX+1));
}

/************************************************************************************/

static int action(Ihandle *ih, float fposx, float fposy)
{
  int posx, posy;
  cdCanvas *canvas = (cdCanvas*)IupGetAttribute(ih, "_CD_CANVAS");

  cdCanvasActivate(canvas);
  cdCanvasClear(canvas);

  /* posy is top-bottom, CD is bottom-top.
     invert posy reference (YMAX-DY - POSY) */
  fposy = IupGetFloat(ih, "YMAX")-IupGetFloat(ih, "DY") - fposy;
  if (fposy < 0) fposy = 0;

  posx = (int)(fposx*(WORLD_XMAX+1) + 0.5);
  posy = (int)(fposy*(WORLD_YMAX+1) + 0.5);

  posx = -posx;
  posy = -posy;

  cdCanvasOrigin(canvas, posx, posy);
  posx = 0; posy = 0;

  cdCanvasForeground(canvas, CD_RED);
  cdCanvasLine(canvas, posx+0, posy+0, posx+WORLD_XMAX, posy+WORLD_YMAX);
  cdCanvasLine(canvas, posx+0, posy+WORLD_YMAX, posx+WORLD_XMAX, posy+0);

  cdCanvasArc(canvas, posx+WORLD_XMAX/2, posy+WORLD_YMAX/2+WORLD_YMAX/10, WORLD_XMAX/10, WORLD_YMAX/10, 0, 360);

  cdCanvasLine(canvas, posx+0, posy+0, posx+WORLD_XMAX, posy+0);
  cdCanvasLine(canvas, posx+0, posy+WORLD_YMAX, posx+WORLD_XMAX, posy+WORLD_YMAX);
  cdCanvasLine(canvas, posx+0, posy+0, posx+0, posy+WORLD_YMAX);
  cdCanvasLine(canvas, posx+WORLD_XMAX, posy+0, posx+WORLD_XMAX, posy+WORLD_YMAX);

  return IUP_DEFAULT;
}

static int resize_cb(Ihandle *ih, int canvas_w, int canvas_h)
{
  cdCanvas *canvas = (cdCanvas*)IupGetAttribute(ih, "_CD_CANVAS");

printf("RESIZE_CB(%d, %d) RASTERSIZE=%s DRAWSIZE=%s \n", canvas_w, canvas_h, IupGetAttribute(ih, "RASTERSIZE"), IupGetAttribute(ih, "DRAWSIZE"));
  /* When *AUTOHIDE=Yes, this can hide a scrollbar and so change the canvas drawsize */
  update_scrollbar(ih, canvas_w, canvas_h);  
printf("                                DRAWSIZE=%s \n", IupGetAttribute(ih, "DRAWSIZE"));
  /* update the canvas size */
  IupGetIntInt(ih, "DRAWSIZE", &canvas_w, &canvas_h);
  update_scrollbar(ih, canvas_w, canvas_h);  

  /* update the application */
  cdCanvasActivate(canvas);

  return IUP_DEFAULT;
}

static int scroll_cb(Ihandle *ih, int op, float posx, float posy)
{
  cdCanvas *canvas = (cdCanvas*)IupGetAttribute(ih, "_CD_CANVAS");
printf("SCROLL_CB(%g, %g)\n", posx, posy);
  cdCanvasActivate(canvas);
  IupRedraw(ih, 0);
  (void)op;
  return IUP_DEFAULT;
}

static int map_cb(Ihandle *ih)
{
  /* canvas will be automatically saved in "_CD_CANVAS" attribute */
  cdCanvas *canvas = cdCreateCanvas(CD_IUP, ih);

  /* handle scrollbar in 0.0-1.0 interval */

  return IUP_DEFAULT;
}

static int unmap_cb(Ihandle *ih)
{
  cdCanvas *canvas = (cdCanvas*)IupGetAttribute(ih, "_CD_CANVAS");
  cdKillCanvas(canvas);
  return IUP_DEFAULT;
}

void CanvasScrollbarTest3(void)
{
  Ihandle *dlg, *cnv;

  cnv = IupCanvas(NULL);
  IupSetAttribute(cnv, "RASTERSIZE", "300x200"); /* initial size */
  IupSetAttribute(cnv, "SCROLLBAR", "YES");
//  IupSetAttribute(cnv, "EXPAND", "NO");

  IupSetCallback(cnv, "RESIZE_CB",  (Icallback)resize_cb);
  IupSetCallback(cnv, "ACTION",  (Icallback)action);
  IupSetCallback(cnv, "MAP_CB",  (Icallback)map_cb);
  IupSetCallback(cnv, "UNMAP_CB",  (Icallback)unmap_cb);
  IupSetCallback(cnv, "SCROLL_CB",  (Icallback)scroll_cb);
                   
  dlg = IupDialog(IupVbox(cnv, NULL));
  IupSetAttribute(dlg, "TITLE", "Scrollbar Test");
  IupSetAttribute(dlg, "MARGIN", "10x10");

  IupMap(dlg);
  IupSetAttribute(cnv, "RASTERSIZE", NULL);  /* release the minimum limitation */
 
  IupShowXY(dlg,IUP_CENTER,IUP_CENTER);
}

#ifndef BIG_TEST
int main(int argc, char* argv[])
{
  IupOpen(&argc, &argv);

  CanvasScrollbarTest();

  IupMainLoop();

  IupClose();

  return EXIT_SUCCESS;
}
#endif
