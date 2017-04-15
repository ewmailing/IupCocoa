#include <stdlib.h>
#include <stdio.h>
#include "iup.h"

void FlatFrameTest(void)
{
  Ihandle *dlg, *frame1, *frame2, *frame3;

  frame1 = IupFlatFrame
          (
            IupVbox
            (
              IupLabel("Label1"),
              IupSetAttributes(IupLabel("Label2"), "SIZE=70x"),
              IupLabel("Label3"),
              NULL
            )
          );

  frame2 = IupFlatFrame
          (
            IupVbox
            (
              IupSetAttributes(IupLabel("Label4"), "EXPAND=HORIZONTAL"),
              IupLabel("Label5"),
              IupLabel("Label6"),
              NULL
            )
          );

  frame3 = IupFlatFrame
          (
            IupVbox
            (
              IupLabel("Label7"),
              IupSetAttributes(IupLabel("Label8"), "SIZE=70x"),
              IupLabel("Label9"),
              NULL
            )
          );

  IupSetAttribute(frame1, "TITLE", "Title Text");
//  IupSetAttribute(frame1, "FGCOLOR", "255 0 0");
//  IupSetAttribute(frame1, "BGCOLOR", "0 128 0");
  IupSetAttribute(frame1, "BGCOLOR", "64 192 255");
  IupSetAttribute(frame1, "FRAMECOLOR", "255 255 255");
  IupSetAttribute(frame1, "TITLELINECOLOR", "255 255 255");
  IupSetAttribute(frame1, "TITLEBGCOLOR", "64 128 255");
  IupSetAttribute(frame1, "TITLECOLOR", "255 255 255");
//  IupSetAttribute(frame1, "TITLELINE", "No");
  IupSetAttribute(frame1, "TITLELINEWIDTH", "2");
  IupSetAttribute(frame1, "FRAMEWIDTH", "2");
  IupSetAttribute(frame1, "FRAMESPACE", "5");

  //IupSetAttribute(frame2, "BGCOLOR", "0 128 0");
  IupSetAttribute(frame2, "BGCOLOR", "0 128 0");
  IupSetAttribute(frame2, "FRAMEWIDTH", "5");
  IupSetAttribute(frame2, "FRAMESPACE", "20");
  IupSetAttribute(frame2, "FGCOLOR", "255 128 128");  /* recursive set for labels */
  IupSetAttribute(frame2, "FONTSTYLE", "Bold");
  IupSetAttribute(frame2, "FONTSIZE", "14");

  IupSetAttribute(frame3, "FRAME", "No");
  IupSetAttribute(frame3, "TITLE", "Title Text");
  IupSetAttribute(frame3, "TITLEPADDING", "10x10");
  IupSetAttribute(frame3, "TITLELINE", "No");
  IupSetAttribute(frame3, "TITLEBGCOLOR", "64 128 255");

  dlg = IupDialog(IupSetAttributes(IupHbox(frame1, frame2, frame3, NULL), "NMARGIN=10x10"));

  IupSetAttribute(dlg, "TITLE", "IupFlatFrame Test");
  IupSetAttribute(dlg, "GAP", "5");
  IupSetAttribute(dlg, "FONTSIZE", "14");
//  IupSetAttribute(dlg, "RASTERSIZE", "300x200");
//  IupSetAttribute(dlg, "BGCOLOR", "128 0 0");

  IupShow(dlg);
  printf("RASTERSIZE(%s)\n", IupGetAttribute(frame1, "RASTERSIZE"));
  printf("CLIENTSIZE(%s)\n", IupGetAttribute(frame1, "CLIENTSIZE"));
  printf("RASTERSIZE(%s)\n", IupGetAttribute(frame2, "RASTERSIZE"));
  printf("CLIENTSIZE(%s)\n", IupGetAttribute(frame2, "CLIENTSIZE"));
}

#ifndef BIG_TEST
int main(int argc, char* argv[])
{
  IupOpen(&argc, &argv);

  FlatFrameTest();

  IupMainLoop();

  IupClose();

  return EXIT_SUCCESS;
}
#endif
