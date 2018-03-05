#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#undef __IUPDEF_H
#include "iup.h"
#include "iupcontrols.h"
#include "iupcbs.h"


#define TEST_IMAGE_SIZE 20

static unsigned char image_data_32 [TEST_IMAGE_SIZE*TEST_IMAGE_SIZE*4] = 
{
  000,000,000,255,000,000,000,255,000,000,000,255,000,000,000,255,000,000,000,255,000,000,000,255,000,000,000,255,000,000,000,255,000,000,000,255,000,000,000,255,000,000,000,255,000,000,000,255,000,000,000,255,000,000,000,255,000,000,000,255,000,000,000,255,000,000,000,255,000,000,000,255,000,000,000,255,000,000,000,255,
  000,000,000,255,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,255,
  000,000,000,255,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,255,
  000,000,000,255,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,255,
  000,000,000,255,000,000,000,000,000,000,000,000,000,000,000,000,255,000,000,255,255,000,000,255,255,000,000,255,255,000,000,255,255,000,000,255,255,000,000,255,000,255,000,255,000,255,000,255,000,255,000,255,000,255,000,255,000,255,000,255,000,255,000,255,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,255, 
  000,000,000,255,000,000,000,000,000,000,000,000,000,000,000,000,255,000,000,255,255,000,000,255,255,000,000,255,255,000,000,255,255,000,000,255,255,000,000,255,000,255,000,255,000,255,000,255,000,255,000,255,000,255,000,255,000,255,000,255,000,255,000,255,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,255, 
  000,000,000,255,000,000,000,000,000,000,000,000,000,000,000,000,255,000,000,255,255,000,000,255,255,000,000,255,255,000,000,255,255,000,000,255,255,000,000,255,000,255,000,255,000,255,000,255,000,255,000,255,000,255,000,255,000,255,000,255,000,255,000,255,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,255, 
  000,000,000,255,000,000,000,000,000,000,000,000,000,000,000,000,255,000,000,255,255,000,000,255,255,000,000,255,255,000,000,255,255,000,000,255,255,000,000,255,000,255,000,255,000,255,000,255,000,255,000,255,000,255,000,255,000,255,000,255,000,255,000,255,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,255, 
  000,000,000,255,000,000,000,000,000,000,000,000,000,000,000,000,255,000,000,255,255,000,000,255,255,000,000,255,255,000,000,255,255,000,000,255,255,000,000,255,000,255,000,255,000,255,000,255,000,255,000,255,000,255,000,255,000,255,000,255,000,255,000,255,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,255, 
  000,000,000,255,000,000,000,000,000,000,000,000,000,000,000,000,255,000,000,255,255,000,000,255,255,000,000,255,255,000,000,255,255,000,000,255,255,000,000,255,000,255,000,255,000,255,000,255,000,255,000,255,000,255,000,255,000,255,000,255,000,255,000,255,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,255, 
  000,000,000,255,000,000,000,000,000,000,000,000,000,000,000,000,000,000,255,255,000,000,255,255,000,000,255,255,000,000,255,255,000,000,255,255,000,000,255,255,255,255,255,192,255,255,255,192,255,255,255,192,255,255,255,192,255,255,255,192,255,255,255,192,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,255, 
  000,000,000,255,000,000,000,000,000,000,000,000,000,000,000,000,000,000,255,255,000,000,255,255,000,000,255,255,000,000,255,255,000,000,255,255,000,000,255,255,255,255,255,192,255,255,255,192,255,255,255,192,255,255,255,192,255,255,255,192,255,255,255,192,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,255, 
  000,000,000,255,000,000,000,000,000,000,000,000,000,000,000,000,000,000,255,255,000,000,255,255,000,000,255,255,000,000,255,255,000,000,255,255,000,000,255,255,255,255,255,192,255,255,255,192,255,255,255,192,255,255,255,192,255,255,255,192,255,255,255,192,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,255, 
  000,000,000,255,000,000,000,000,000,000,000,000,000,000,000,000,000,000,255,255,000,000,255,255,000,000,255,255,000,000,255,255,000,000,255,255,000,000,255,255,255,255,255,192,255,255,255,192,255,255,255,192,255,255,255,192,255,255,255,192,255,255,255,192,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,255, 
  000,000,000,255,000,000,000,000,000,000,000,000,000,000,000,000,000,000,255,255,000,000,255,255,000,000,255,255,000,000,255,255,000,000,255,255,000,000,255,255,255,255,255,192,255,255,255,192,255,255,255,192,255,255,255,192,255,255,255,192,255,255,255,192,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,255, 
  000,000,000,255,000,000,000,000,000,000,000,000,000,000,000,000,000,000,255,255,000,000,255,255,000,000,255,255,000,000,255,255,000,000,255,255,000,000,255,255,255,255,255,192,255,255,255,192,255,255,255,192,255,255,255,192,255,255,255,192,255,255,255,192,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,255,
  000,000,000,255,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,255,
  000,000,000,255,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,255,
  000,000,000,255,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,255,
  000,000,000,255,000,000,000,255,000,000,000,255,000,000,000,255,000,000,000,255,000,000,000,255,000,000,000,255,000,000,000,255,000,000,000,255,000,000,000,255,000,000,000,255,000,000,000,255,000,000,000,255,000,000,000,255,000,000,000,255,000,000,000,255,000,000,000,255,000,000,000,255,000,000,000,255,000,000,000,255,
};

static int mousemove_cb(Ihandle *ih, int lin, int col)
{
  printf("mousemove_cb(%d, %d)\n", lin, col);

#if 0
  {
    char* value = IupGetAttributeId2(ih, "", lin, col);
    char* tip = IupGetAttribute(ih, "TIP");
    if (value && ((tip && strcmp(value, tip) != 0) || !tip))
    {
      IupSetStrAttribute(ih, "TIP", value);
      IupSetAttribute(ih, "TIPVISIBLE", "Yes");
    }
   }
#endif

  return IUP_DEFAULT;
}

static int drop(Ihandle *self, Ihandle *drop, int lin, int col)
{
  printf("drop_cb(%d, %d)\n", lin, col);
  if(lin == 3 && col == 1)
  {
    IupSetAttribute(drop, "1", "A - Test of Very Big String for Dropdown!");
    IupSetAttribute(drop, "2", "B");
    IupSetAttribute(drop, "3", "C");
    IupSetAttribute(drop, "4", "XXX");
    IupSetAttribute(drop, "5", "5");
    IupSetAttribute(drop, "6", "6");
    IupSetAttribute(drop, "7", "7");
    IupSetAttribute(drop, "8", NULL);
    IupSetAttribute(drop, "VALUE", "4");
    return IUP_DEFAULT;
  }
  return IUP_IGNORE;
}

static int dropcheck_cb(Ihandle *self, int lin, int col)
{
  if (lin == 3 && col == 1)
    return IUP_DEFAULT;
  if (lin == 4 && col == 4)
    return IUP_CONTINUE;
  return IUP_IGNORE;
}

static int togglevalue_cb(Ihandle *self, int lin, int col, int value)
{
  printf("togglevalue_cb(%d, %d)=%d\n", lin, col, value);
  return IUP_DEFAULT;
}

static int click(Ihandle *self, int lin, int col)
{
  printf("click_cb(%d, %d)\n", lin, col);
  IupSetAttribute(self,"MARKED", NULL);  /* clear all marks */
  IupSetAttributeId2(self,"MARK", lin, 0, "1");
  IupSetfAttribute(self,"REDRAW", "L%d", lin);
  return IUP_DEFAULT;
}

static int enteritem_cb(Ihandle *ih, int lin, int col)
{
  IupSetAttribute(ih,"MARKED", NULL);  /* clear all marks */
  IupSetAttributeId2(ih,"MARK", lin, 0, "1");
  IupSetfAttribute(ih,"REDRAW", "L%d", lin);
  return IUP_DEFAULT;
}

static Ihandle* create_matrix(void)
{
  Ihandle* mat = IupMatrix(NULL); 
  
//  IupSetAttribute(mat, "NUMLIN", "3"); 
  IupSetAttribute(mat, "NUMLIN", "20"); 
  IupSetAttribute(mat, "NUMCOL", "8"); 
//  IupSetAttribute(mat, "NUMCOL", "15"); 
//  IupSetAttribute(mat, "NUMLIN", "3"); 
//  IupSetAttribute(mat, "NUMCOL", "2"); 
//  IupSetAttribute(mat, "NUMLIN", "8"); 
//  IupSetAttribute(mat, "NUMCOL", "5"); 
//  IupSetAttribute(mat, "ACTIVE", "NO");
//  IupSetAttribute(mat, "EDITHIDEONFOCUS", "NO");
//  IupSetAttribute(mat, "EDITALIGN", "Yes");
//  IupSetAttribute(mat, "EDITFITVALUE", "Yes");
//  IupSetAttribute(mat, "READONLY", "Yes");
  
  IupSetAttribute(mat, "0:0", "Inflation");
  IupSetAttribute(mat, "1:0", "Medicine\nPharma");
  IupSetAttribute(mat, "2:0", "Food"); 
  IupSetAttribute(mat, "3:0", "Energy"); 
  IupSetAttribute(mat, "0:1", "January 2000"); 
  IupSetAttribute(mat, "0:2", "February 2000"); 
  IupSetAttribute(mat, "1:1", "5.6\n3.33");
  IupSetAttribute(mat, "2:1", "2.2");
  IupSetAttribute(mat, "3:2", "Very Very Very Very Very Large Text");
  IupSetAttribute(mat, "1:2", "4.5");
  if (IupGetInt(NULL, "UTF8MODE"))
    IupSetAttribute(mat, "2:2", "(çãõáóé)");
  else
    IupSetAttribute(mat, "2:2", "(������)");

  IupSetAttribute(mat, "3:1", "3.4");
  IupSetAttribute(mat, "3:3", "Font Test");
//  IupSetAttribute(mat, "HEIGHT2", "30");
//  IupSetAttribute(mat, "WIDTH2", "190");
//  IupSetAttributeId(mat, "WIDTH", 2, "190");
  IupSetAttribute(mat,"SORTSIGN2","DOWN");
//  IupSetAttribute(mat, "WIDTHDEF", "34");
//  IupSetAttribute(mat,"MULTILINE", "YES");
  IupSetAttribute(mat,"RESIZEMATRIX", "YES");
//  IupSetAttribute(mat,"HIDDENTEXTMARKS", "YES");
//  IupSetAttribute(mat,"USETITLESIZE", "YES");
  //IupSetAttribute(mat,"SCROLLBAR", "NO");
  //IupSetAttribute(mat, "BGCOLOR1:2", "255 92 255");
  //IupSetAttribute(mat, "BGCOLOR2:*", "92 92 255");
  //IupSetAttribute(mat, "BGCOLOR*:3", "255 92 92");
  //IupSetAttribute(mat, "FGCOLOR1:2", "255 0 0");
  //IupSetAttribute(mat, "FGCOLOR2:*", "0 128 0");
  //IupSetAttribute(mat, "FGCOLOR*:3", "0 0 255");
  IupSetAttribute(mat, "FONT3:3", "Helvetica, 24");
  //IupSetAttribute(mat, "FONT2:*", "Courier, 14");
  //IupSetAttribute(mat, "FONT*:3", "Times, Bold 14");
//  IupSetAttribute(mat, "ALIGNMENT1", "ALEFT");
//  IupSetAttribute(mat, "ALIGNMENT3", "ARIGHT");
//  IupSetAttribute(mat, "ALIGN2:1", ":ARIGHT");
//  IupSetAttribute(mat, "LINEALIGNMENT1", "ATOP");
  //  IupSetAttribute(mat, "ACTIVE", "NO");
//  IupSetAttribute(mat, "EXPAND", "NO");
//  IupSetAttribute(mat, "ALIGNMENT", "ALEFT");

  IupSetAttribute(mat, "FONTSTYLE0:1", "Bold");

//  IupSetAttribute(mat, "MASK1:3", IUP_MASK_FLOAT);
//  IupSetAttribute(mat, "MASK1:3", "[a-zA-Z][0-9a-zA-Z_]*");
//  IupSetAttribute(mat, "MASKFLOAT1:3", "0.0:10.0");
  IupSetAttribute(mat, "MASK*:3", "[a-zA-Z][0-9a-zA-Z_]*");

  IupSetAttribute(mat, "TYPE4:1", "COLOR");
  IupSetAttribute(mat, "4:1", "255 0 128");

  IupSetAttribute(mat, "TYPE4:2", "FILL");
  IupSetAttribute(mat, "4:2", "60");
  IupSetAttribute(mat, "SHOWFILLVALUE", "Yes");
//  IupSetAttribute(mat, "FLAT", "Yes");

  {
    Ihandle* image = IupImageRGBA(TEST_IMAGE_SIZE, TEST_IMAGE_SIZE, image_data_32);
    IupSetAttribute(mat, "TYPE4:3", "IMAGE");
    IupSetAttributeHandle(mat, "4:3", image);
  }

//  IupSetAttribute(mat, "TOGGLEVALUE4:4", "ON");
//  IupSetAttribute(mat, "VALUE4:4", "1");
  IupSetAttribute(mat, "TOGGLECENTERED", "Yes");

  IupSetAttribute(mat,"MARKMODE","CELL");
//  IupSetAttribute(mat,"MARKMODE","LIN");
//  IupSetAttribute(mat,"MARKMULTIPLE","NO");
  IupSetAttribute(mat,"MARKMULTIPLE","YES");
//  IupSetAttribute(mat,"MARKAREA","NOT_CONTINUOUS");
//  IupSetAttribute(mat,"MARK2:2","YES");
//  IupSetAttribute(mat,"MARK2:3","YES");
//  IupSetAttribute(mat,"MARK3:3","YES");

  IupSetAttribute(mat,"FRAMEVERTCOLOR1:2","BGCOLOR");
  IupSetAttribute(mat,"FRAMEHORIZCOLOR1:2","0 0 255");
  IupSetAttribute(mat,"FRAMEHORIZCOLOR1:3","0 255 0");
  IupSetAttribute(mat,"FRAMEVERTCOLOR2:2","255 255 0");
  IupSetAttribute(mat,"FRAMEVERTCOLOR*:4","0 255 0");
  IupSetAttribute(mat,"FRAMEVERTCOLOR*:5","BGCOLOR");

//  IupSetAttribute(mat, "FRAMETITLEVERTCOLOR*:0", "0 255 0");
//  IupSetAttribute(mat, "FRAMETITLEHORIZCOLOR0:4", "0 255 0");
//  IupSetAttribute(mat, "FRAMEHORIZCOLOR0:4", "255 0 0");
//  IupSetAttribute(mat, "FRAMETITLEHORIZCOLOR0:*", "0 255 0");

//  IupSetAttribute(mat,"MARKMODE","LINCOL");

  //IupSetAttribute(mat, "NUMCOL_VISIBLE_LAST", "YES");
  //IupSetAttribute(mat, "NUMLIN_VISIBLE_LAST", "YES");
//  IupSetAttribute(mat, "WIDTHDEF", "15");
  IupSetAttribute(mat, "20:8", "The End");
  IupSetAttribute(mat, "10:0", "Middle Line");
  IupSetAttribute(mat, "15:0", "Middle Line");
  IupSetAttribute(mat, "0:4", "Middle Column");
  IupSetAttribute(mat, "20:0", "Line Title Test");
  IupSetAttribute(mat, "0:8", "Column Title Test");
  IupSetAttribute(mat, "NUMCOL_VISIBLE", "3");
  IupSetAttribute(mat, "NUMLIN_VISIBLE", "5");
//  IupSetAttribute(mat,"EDITNEXT","COLCR");

  IupSetAttribute(mat, "NUMCOL_NOSCROLL", "1");
  IupSetAttribute(mat, "NOSCROLLASTITLE", "Yes");

  IupSetAttributeId2(mat, "MERGE", 1, 2, "1:3");
  IupSetAttributeId2(mat, "MERGE", 5, 2, "7:4");

//  IupSetAttribute(mat, "LIMITEXPAND", "Yes");
//  IupSetAttribute(mat, "XAUTOHIDE", "NO");
//  IupSetAttribute(mat, "YAUTOHIDE", "NO");

//  IupSetAttribute(mat,"RASTERSIZE","x300");
//  IupSetAttribute(mat,"FITTOSIZE","LINES");
  
//  IupSetAttribute(mat,"TYPECOLORINACTIVE","No");
//  IupSetAttribute(mat, "ACTIVE", "No");

//  IupSetAttribute(mat, "FRAMEBORDER", "Yes");

//  IupSetAttribute(mat, "FLATSCROLLBAR", "Yes");
//  IupSetAttribute(mat, "SHOWFLOATING", "Yes");

  /* test for custom matrix attributes */
  //{
  //  char* v;
  //  IupSetAttribute(mat, "MTX_LINE_ACTIVE_FLAG3:4", "Test1");
  //  IupSetAttributeId2(mat, "MTX_LINE_ACTIVE_FLAG", 5, 7, "Test2");
  //  printf("Test1=%s\n", IupGetAttribute(mat, "MTX_LINE_ACTIVE_FLAG3:4"));
  //  printf("Test2=%s\n", IupGetAttributeId2(mat, "MTX_LINE_ACTIVE_FLAG", 5, 7));
  //}

  IupSetCallback(mat, "DROPCHECK_CB", (Icallback)dropcheck_cb);
  IupSetCallback(mat,"DROP_CB",(Icallback)drop);
//  IupSetCallback(mat,"MENUDROP_CB",(Icallback)drop);
  IupSetCallback(mat, "MOUSEMOVE_CB", (Icallback)mousemove_cb);
//  IupSetCallback(mat,"CLICK_CB",(Icallback)click);
//  IupSetCallback(mat,"ENTERITEM_CB",(Icallback)enteritem_cb);
  IupSetCallback(mat,"TOGGLEVALUE_CB",(Icallback)togglevalue_cb);

  return mat;
}

void MatrixTest(void)
{
  Ihandle* dlg, *box, *mat;

  box = IupVbox(mat = create_matrix(), NULL);
  IupSetAttribute(box, "MARGIN", "10x10");
//  IupSetAttribute(box, "FONT", "Arial, 7");

  dlg = IupDialog(box);
  IupSetAttribute(dlg, "TITLE", "IupMatrix Simple Test");


  IupShowXY(dlg, IUP_CENTER, IUP_CENTER);

//  IupSetAttributeId2(mat, "MERGE", 1, 2, "1:3");
  //        IupSetAttribute(mat, "ADDLIN", "1");
//        IupSetAttribute(mat,"4:0","Teste");
//        IupSetAttribute(mat, "FONTSTYLE0:1", "Bold");
//       IupSetAttribute(mat, "REDRAW", "ALL");
}

#ifndef BIG_TEST
int main(int argc, char* argv[])
{
  IupOpen(&argc, &argv);
  IupControlsOpen();

  MatrixTest();

  IupMainLoop();

  IupClose();

  return EXIT_SUCCESS;
}
#endif
