#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iup_plus.h>
#include <iupcontrols.h>


static int resize_cb(Ihandle *ih)
{
  Iup::Container dlg(ih);
  Iup::Matrix mat = dlg.GetChild(0);
	mat.SetAttribute("RASTERWIDTH2", NULL);
	mat.SetAttribute("FITTOSIZE", "COLUMNS");
	return IUP_DEFAULT;
}

static int close_cb(Ihandle *ih)
{
  Iup::Control dlg(ih);
  int X = dlg.GetInteger("X");
  int Y = dlg.GetInteger("Y");
	printf("close_cb: X,Y=%d,%d\n", X, Y);

  FILE *fp = fopen("test.dat", "w");
  fwrite(&X, sizeof(int), 1, fp);
  fwrite(&Y, sizeof(int), 1, fp);
  fclose(fp);

	return IUP_DEFAULT;
}

static Iup::Matrix create_mat(void)
{
  Iup::Matrix mat;

  mat.SetAttribute("NUMCOL","3"); 
  mat.SetAttribute("NUMLIN","3"); 
  mat.SetAttribute("NUMCOL_VISIBLE","3");
  mat.SetAttribute("NUMLIN_VISIBLE","3");
  mat.SetAttribute("WIDTH0", "0");
  mat.SetAttribute("WIDTH1","100");
  mat.SetAttribute("WIDTH3","70");
  
  mat.SetAttribute("1:0","Medicine");
  mat.SetAttribute("2:0","Food"); 
  mat.SetAttribute("3:0","Energy"); 
  mat.SetAttribute("1:1","5.6");
  mat.SetAttribute("2:1","2.2");
  mat.SetAttribute("3:1","7.2");
  mat.SetAttribute("1:2","4.5");
  mat.SetAttribute("2:2","8.1");
  mat.SetAttribute("3:2","3.4");
  mat.SetAttribute("READONLY","YES");
  mat.SetAttribute("SCROLLBAR","VERTICAL");
  return mat;
}

int main(int argc, char **argv)
{
	int X=100, Y=200;

  FILE* fp = fopen("test.dat", "r");
	if (fp != NULL)
	{
		fread(&X,sizeof(int),1,fp);
		fread(&Y,sizeof(int),1,fp);
		fclose(fp);
	}

	printf("X,Y=%d,%d\n", X, Y);

  Iup::Open(argc, argv);       
  Iup::Controls::Open();

  Iup::Matrix mat = create_mat();
  Iup::Dialog dlg(mat);
  dlg.SetAttribute("SHRINK", "YES");
  dlg.SetAttribute("TITLE", "IupMatrix");
  dlg.SetCallback("RESIZE_CB", (Icallback)resize_cb);
  dlg.SetCallback("CLOSE_CB", (Icallback)close_cb);

  dlg.ShowXY(X, Y);

  Iup::MainLoop();

  Iup::Close();

  return 0;
}
