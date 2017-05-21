#include <stdlib.h>
#include <stdio.h>

#include <iup.h>
#include <iup_mglplot.h>

int main(int argc, char **argv)
{
  Ihandle *dlg;
  Ihandle *lbl;

  IupOpen(&argc, &argv);
  IupMglPlotOpen();

  lbl = IupMglLabel("\\int \\alpha \\sqrt{sin(\\pi x)^2 + \\gamma_{i_k}} dx");
  IupSetAttribute(lbl, "RASTERSIZE", "400x80");
  IupSetAttribute(lbl, "LABELFONTSIZE", "10");

//  IupSetAttribute(lbl, "BGCOLOR", "255 255 0");
//  IupSetAttribute(lbl, "FGCOLOR", "0 0 255");

  dlg = IupDialog(IupVbox(lbl, NULL));
  IupSetAttribute(dlg, "MARGIN", "10x10");
  IupSetAttribute(dlg, "TITLE", "IupMglLabel Example");

  IupShowXY(dlg, IUP_CENTER, IUP_CENTER);

  IupMainLoop();

  IupClose();

  return EXIT_SUCCESS;
}