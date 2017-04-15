/*
 * IupCanvas IDLE Redraw example
 */

#include <cd_plus.h>
#include <iup_plus.h>

static int need_redraw = 1, redraw_count = 0;
static Iup::ProgressBar gauge;
static cd::CanvasIup *cdcanvas = 0;

int toggle_redraw(void)
{
  need_redraw = !need_redraw;
  return IUP_DEFAULT;
}

int redraw(void)
{
  if (need_redraw)
  {
    if (redraw_count == 0)
    {
      cdcanvas->Background(CD_BLUE);
      cdcanvas->Clear();
    }

    cdcanvas->Foreground(CD_RED);
    cdcanvas->Box(0, 300, 0, redraw_count / 100);
    gauge.SetNumber("VALUE", (double)redraw_count / 30000.0f);

    redraw_count++;
    if (redraw_count == 30000)
    {
      redraw_count = 0;
      need_redraw = 0;
    }
  }
  return IUP_DEFAULT;
}

int main(int argc, char **argv) 
{
  Iup::Open(argc, argv);
  
  Iup::Canvas cv;
  Iup::Button bt("Start/Stop");
  bt.SetAttribute("SIZE", "50x50");
  gauge.SetAttribute("SIZE", "200x15");
  cv.SetAttribute("SIZE", "200x200");
  Iup::Dialog dlg(Iup::Vbox(cv, Iup::Hbox(gauge, bt)));
  dlg.SetAttribute("TITLE", "Redraw test");

  Iup::SetFunction("IDLE_ACTION", (Icallback)redraw);

  dlg.Map();
  
  cdcanvas = new cd::CanvasIup(cv);
  
  bt.SetCallback("ACTION", (Icallback)toggle_redraw);

  dlg.ShowXY(IUP_CENTER, IUP_CENTER);

  Iup::MainLoop();

  delete cdcanvas;

  Iup::Close();

  return 0;
}
