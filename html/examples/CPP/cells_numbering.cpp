#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <iup_plus.h>
#include <cd_plus.h>


static int nlines_cb(Ihandle*) 
{
  return 20;
}

static int ncols_cb(Ihandle*) 
{
  return 50;
}

static int height_cb(Ihandle*, int) 
{
  return 30;
}

static int width_cb(Ihandle*, int) 
{
  return 70;
}

static int mouseclick_cb(Ihandle*, int b, int, int i, int j, int, int, char*)
{
  char buffer[128];
  sprintf(buffer, "CLICK: %d: (%02d, %02d)\n", b, i, j);
  Iup::Message("Hi!", buffer);
  return IUP_DEFAULT;
}

static int draw_cb(Ihandle*, int i, int j, int xmin, int xmax, int ymin, int ymax, cdCanvas* _canvas) 
{
  cd::Canvas canvas(_canvas);

  int xm = (xmax + xmin) / 2;
  int ym = (ymax + ymin) / 2;
  char buffer[64];

  canvas.Foreground(cdEncodeColor(
    (unsigned char)(i*20), 
    (unsigned char)(j*100), 
    (unsigned char)(i+100)
  ));

  canvas.Box(xmin, xmax, ymin, ymax);
  canvas.TextAlignment(CD_CENTER);
  canvas.Foreground(CD_BLACK);
  sprintf(buffer, "(%02d, %02d)", i, j);
  canvas.Text(xm, ym, buffer);

  return IUP_DEFAULT;
}

static Iup::Cells create(void)
{
  Iup::Cells cells;

  cells.SetCallback("MOUSECLICK_CB", (Icallback)mouseclick_cb);
  cells.SetCallback( "DRAW_CB",   (Icallback)draw_cb);
  cells.SetCallback( "WIDTH_CB",  (Icallback)width_cb);
  cells.SetCallback( "HEIGHT_CB", (Icallback)height_cb);
  cells.SetCallback( "NLINES_CB", (Icallback)nlines_cb);
  cells.SetCallback( "NCOLS_CB",  (Icallback)ncols_cb);
  cells.SetAttribute("BOXED", "NO");

  return cells;
}

/* Main program */
int main(int argc, char **argv) 
{
  Iup::Open(argc, argv);       
  Iup::Controls::Open();

  Iup::Cells cells = create();
  Iup::Dialog dlg(cells);

  dlg.SetAttribute("RASTERSIZE", "500x500");
  dlg.SetAttribute("TITLE", "IupCells");

  dlg.ShowXY(IUP_CENTER, IUP_CENTER);

  Iup::MainLoop();

  Iup::Close();  

  return 0;
}
