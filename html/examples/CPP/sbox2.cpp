
#include <stdlib.h>
#include <stdio.h>
#include <iup_plus.h>
#include <iupcontrols.h>

static Iup::Tree createtree()
{
  Iup::Tree tree;
  tree.SetAttribute("SIZE", "80x80");
  tree.SetAttributes("FONT=COURIER_NORMAL_10, \
                          TITLE=Figures, \
                          ADDBRANCH=3D, \
                          ADDBRANCH=2D, \
                          ADDLEAF1=trapeze, \
                          ADDBRANCH1=parallelogram, \
                          ADDLEAF2=diamond, \
                          ADDLEAF2=square, \
                          ADDBRANCH4=triangle, \
                          ADDLEAF5=scalenus, \
                          ADDLEAF5=isoceles, \
                          ADDLEAF5=equilateral, \
                          VALUE=6, \
                          ADDEXPANDED=NO");
  return tree;
}

int main(int argc, char **argv)
{
  Iup::Open(argc, argv);
  Iup::Controls::Open();

  Iup::Tree tree = createtree();
  tree.SetAttribute("EXPAND", "YES");

  Iup::Sbox sbox(tree);
  sbox.SetAttribute("DIRECTION", "EAST");

  Iup::Canvas cv;
  cv.SetAttribute("EXPAND", "YES");

  Iup::Text ml;
  ml.SetAttribute("MULTILINE", "YES");
  ml.SetAttribute("EXPAND", "YES");
  Iup::Sbox sbox2(ml);
  sbox2.SetAttribute("DIRECTION", "WEST");

  Iup::Hbox vbox(sbox, cv, sbox2);

  Iup::Label lb("This is a label");
  lb.SetAttribute("EXPAND", "NO");
  Iup::Sbox sbox3(lb);
  sbox3.SetAttribute("DIRECTION", "NORTH");
  Iup::Dialog dg(Iup::Vbox(vbox, sbox3));
  dg.SetAttribute("TITLE", "IupSbox Example");

  dg.Show();

  Iup::MainLoop();

  Iup::Close();

  return 0;
}
