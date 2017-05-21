
#include <iup_plus.h>

int main(int argc, char **argv)
{
  Iup::Open(argc, argv);
  Iup::MglPlot::Open();

  Iup::MglLabel lbl("\\int \\alpha \\sqrt{sin(\\pi x)^2 + \\gamma_{i_k}} dx");
  lbl.SetAttribute("RASTERSIZE", "400x80");
  lbl.SetAttribute("LABELFONTSIZE", "10");

//  lbl.SetAttribute("BGCOLOR", "255 255 0");
//  lbl.SetAttribute("FGCOLOR", "0 0 255");

  Iup::Dialog dlg(Iup::Vbox(lbl, (Ihandle*)0));
  dlg.SetAttribute("MARGIN", "10x10");
  dlg.SetAttribute("TITLE", "IupMglLabel Example");

  dlg.ShowXY(IUP_CENTER, IUP_CENTER);

  Iup::MainLoop();

  Iup::Close();

  return 0;
}
