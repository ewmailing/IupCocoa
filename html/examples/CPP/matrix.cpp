
#include <iup_plus.h>

static Iup::Matrix create_mat(void)
{
  Iup::Matrix mat;
  
  mat.SetAttribute("NUMCOL","5"); 
  mat.SetAttribute("NUMLIN","10"); 
  
  mat.SetAttribute("NUMCOL_VISIBLE","2");
  mat.SetAttribute("NUMLIN_VISIBLE","3");
  
  mat.SetAttribute("0:0","Inflation");
  mat.SetAttribute("1:0","Medicine");
  mat.SetAttribute("2:0","Food"); 
  mat.SetAttribute("3:0","Energy"); 
  mat.SetAttribute("0:1","January 2000"); 
  mat.SetAttribute("0:2","February 2000"); 
  mat.SetAttribute("1:1","5.6");
  mat.SetAttribute("2:1","2.2");
  mat.SetAttribute("3:1","7.2");
  mat.SetAttribute("1:2","4.5");
  mat.SetAttribute("2:2","8.1");
  mat.SetAttribute("3:2","3.4");
//  mat.SetAttribute("WIDTHDEF","34");
  mat.SetAttribute("RESIZEMATRIX","YES");
//  mat.SetAttribute("MARKMODE","CELL");
  mat.SetAttribute("MARKMODE","LINCOL");
  mat.SetAttribute("MULTIPLE","YES");
  mat.SetAttribute("AREA","NOT_CONTINUOUS");

  return mat;
}

int main(int argc, char **argv)
{
  Iup::Open(argc, argv);       
  Iup::Controls::Open();

  Iup::Dialog dlg(create_mat());
  dlg.SetAttribute("TITLE", "IupMatrix");

  dlg.ShowXY(IUP_CENTER, IUP_CENTER);

  Iup::MainLoop ();

  Iup::Close();

  return 0;
}
