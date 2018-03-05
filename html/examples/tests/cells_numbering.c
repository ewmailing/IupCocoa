#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "iup.h"
#include "cd.h"
#include "iupcontrols.h"


static int nlines_cb(Ihandle* ih)
{
  return 20;
}

static int ncols_cb(Ihandle* ih)
{
  return 50;
}

static int height_cb(Ihandle* ih, int i)
{
  return 30;
}

static int width_cb(Ihandle* ih, int j)
{
  return 70;
}

static int mouseclick_cb(Ihandle* ih, int b, int p, int i, int j, int x, int y, char* r)
{
  printf("CLICK: %d,%d (%d,%d)\n", i, j, x, y);
  return IUP_DEFAULT;
}

static int mousemotion_cb(Ihandle *ih, int i, int j, int x, int y, char *r)
{
  printf("MOTION: %d,%d (%d,%d)\n", i, j, x, y);
  return IUP_DEFAULT;
}

static int draw_cb(Ihandle* ih, int i, int j, int xmin, int xmax, int ymin, int ymax, cdCanvas* canvas)
{
  char buffer[64];
  int xm = (xmax + xmin) / 2;
  int ym = (ymax + ymin) / 2;

  cdCanvasForeground(canvas, cdEncodeColor((unsigned char)(i*20), (unsigned char)(j*100), (unsigned char)(i+100)));

  cdCanvasFont(canvas, "Courier", CD_PLAIN, 9);
  cdCanvasBox(canvas, xmin, xmax, ymin, ymax);
  cdCanvasTextAlignment(canvas, CD_CENTER);
  cdCanvasForeground(canvas, CD_BLACK);
  sprintf(buffer, "%d,%d", i, j);
  cdCanvasText(canvas, xm, ym, buffer);

  return IUP_DEFAULT;
}

static Ihandle* create(void)
{
  Ihandle* cells = IupCells(); 

  IupSetCallback(cells, "MOUSECLICK_CB", (Icallback)mouseclick_cb);
  IupSetCallback(cells, "MOUSEMOTION_CB", (Icallback)mousemotion_cb);
  IupSetCallback(cells, "DRAW_CB", (Icallback)draw_cb);
  IupSetCallback(cells, "WIDTH_CB",      (Icallback)width_cb);
  IupSetCallback(cells, "HEIGHT_CB",     (Icallback)height_cb);
  IupSetCallback(cells, "NLINES_CB",     (Icallback)nlines_cb);
  IupSetCallback(cells, "NCOLS_CB",      (Icallback)ncols_cb);

  IupSetAttribute(cells, "BOXED", "NO");

  IupSetAttribute(cells, "RASTERSIZE", "500x400");

  return cells;
}

void CellsNumberingTest(void)
{
  Ihandle* dlg = NULL;
  Ihandle* cells  = NULL;

  cells  = create();

  dlg = IupDialog(cells);
  IupSetAttribute(dlg, "TITLE", "IupCells");

  IupShowXY(dlg, IUP_CENTER, IUP_CENTER) ;
  IupSetAttribute(cells, "RASTERSIZE", NULL);
}

#ifndef BIG_TEST
int main(int argc, char* argv[])
{
  IupOpen(&argc, &argv);
  IupControlsOpen();

  CellsNumberingTest();

  IupMainLoop();

  IupClose();

  return EXIT_SUCCESS;
}
#endif
