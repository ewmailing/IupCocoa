
#include <iup_plus.h>
#include <cd_plus.h>


static int nlines_cb(Ihandle*) 
{
   return 8;
}

static int ncols_cb(Ihandle*) 
{
   return 8;
}

static int height_cb(Ihandle*, int) 
{
   return 50;
}

static int width_cb(Ihandle*, int) 
{
   return 50;
}

static int draw_cb(Ihandle*, int i, int j, int xmin, int xmax, int ymin, int ymax, cdCanvas* _canvas) 
{
  cd::Canvas canvas(_canvas);

  if (((i%2) && (j%2)) || (((i+1)%2) && ((j+1)%2))) 
    canvas.Foreground(CD_WHITE);
  else 
    canvas.Foreground(CD_BLACK);
  canvas.Box(xmin, xmax, ymin, ymax);

  return IUP_DEFAULT;
}

static Iup::Cells create(void)
{
  Iup::Cells cells;
   
  cells.SetCallback( "DRAW_CB", (Icallback)draw_cb);
  cells.SetCallback( "WIDTH_CB", (Icallback)width_cb);
  cells.SetCallback( "HEIGHT_CB", (Icallback)height_cb);
  cells.SetCallback( "NLINES_CB", (Icallback)nlines_cb);
  cells.SetCallback( "NCOLS_CB", (Icallback)ncols_cb);

  return cells;
}

/* Main program */
int main(int argc, char **argv) 
{
   Iup::Open(argc, argv);
   Iup::Controls::Open();

   Iup::Cells cells = create();
   Iup::Dialog dlg(cells);

   dlg.SetAttribute("RASTERSIZE", "440x480");
   dlg.SetAttribute("TITLE", "IupCells");

   dlg.ShowXY(IUP_CENTER, IUP_CENTER);

   Iup::MainLoop();

   Iup::Close();

   return 0;
}
