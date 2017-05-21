#include <stdlib.h>
#include <stdio.h>
#include <iup.h>

void GridBoxTest(void)
{
  Ihandle *dlg;
  Ihandle *fr1;
  Ihandle *gbox;

  fr1 = IupFrame
  (
    gbox = IupGridBox
    (
      IupSetAttributes(IupLabel(""), ""),
      IupSetAttributes(IupLabel("col1"), "FONTSTYLE=Bold"),
      IupSetAttributes(IupLabel("col2"), "FONTSTYLE=Bold"),

      IupSetAttributes(IupLabel("lin1"), "FONTSTYLE=Bold"),
      IupSetAttributes(IupLabel("lbl"), "XSIZE=50x12"),
      IupSetAttributes(IupButton("but", NULL), "XSIZE=50"),

      IupSetAttributes(IupLabel("lin2"), "FONTSTYLE=Bold"),
      IupSetAttributes(IupLabel("label"), "XSIZE=x12"),
      IupSetAttributes(IupButton("button", NULL), "XEXPAND=Horizontal"),

      IupSetAttributes(IupLabel("lin3"), "FONTSTYLE=Bold"),
      IupSetAttributes(IupLabel("label large"), "XSIZE=x12"),
      IupSetAttributes(IupButton("button large", NULL), ""),
      NULL
    )
  );
  
  IupSetAttribute(gbox, "SIZECOL", "2");
  IupSetAttribute(gbox, "SIZELIN", "3");
//  IupSetAttribute(gbox, "HOMOGENEOUSLIN", "Yes");
//  IupSetAttribute(gbox, "HOMOGENEOUSCOL", "Yes");
//  IupSetAttribute(gbox, "EXPANDCHILDREN", "HORIZONTAL");
//  IupSetAttribute(gbox, "NORMALIZESIZE", "BOTH");
//  IupSetAttribute(gbox, "NORMALIZESIZE", "HORIZONTAL");

  IupSetAttribute(gbox, "NUMDIV", "3");
//  IupSetAttribute(gbox, "NUMDIV", "2");

//  IupSetAttribute(gbox, "ORIENTATION", "VERTICAL");
//  IupSetAttribute(gbox, "SIZECOL", "3");
//  IupSetAttribute(gbox, "SIZELIN", "2");

//  IupSetAttribute(gbox, "SIZE", "70x");
//  IupSetAttribute(gbox, "NUMDIV", "AUTO");
//  IupSetAttribute(gbox, "NORMALIZESIZE", "BOTH");

  IupSetAttribute(gbox, "ALIGNMENTLIN", "ACENTER");
  IupSetAttribute(gbox, "MARGIN", "10x10");
  IupSetAttribute(gbox, "GAPLIN", "5");
  IupSetAttribute(gbox, "GAPCOL", "5");

  dlg = IupDialog
  (
    IupHbox
    (
      fr1,
      NULL
    )
  );

  IupSetAttribute(dlg, "TITLE", "IupGridBox Test");
  IupSetAttribute(dlg, "MARGIN", "10x10");
  IupSetAttribute(fr1, "MARGIN", "0x0");   /* avoid attribute propagation */

  /* Shows dlg in the center of the screen */
  IupShowXY(dlg, IUP_CENTER, IUP_CENTER);
}

#ifndef BIG_TEST
int main(int argc, char* argv[])
{
  IupOpen(&argc, &argv);

  GridBoxTest();

  IupMainLoop();

  IupClose();

  return EXIT_SUCCESS;
}
#endif
