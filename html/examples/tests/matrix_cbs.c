#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "iup.h"
#include "iupcontrols.h"
#include <cd.h>


static int leave(Ihandle *self, int lin, int col)
{
  printf("leaveitem_cb(%d, %d)\n", lin, col);
  //if(lin == 3 && col ==2)
  //  return IUP_IGNORE;   /* notice that this will lock the matrix in this cell */
  return IUP_DEFAULT;
}

static char* value(Ihandle *self, int lin, int col)
{
  static char str[50];
  sprintf(str, "%d-%d", lin, col);
  return str;
}

static int enter(Ihandle *self, int lin, int col)
{
  printf("enteritem_cb(%d, %d)\n", lin, col);
  if(lin == 2 && col == 2)
  {
    IupSetAttribute(IupGetHandle("mat1"), "REDRAW", "ALL");
    IupSetAttribute(IupGetHandle("mat2"), "REDRAW", "ALL");
    //IupSetAttribute(IupGetHandle("mat3"), "REDRAW", "ALL");
    //IupSetAttribute(IupGetHandle("mat4"), "REDRAW", "ALL");
    //IupSetAttribute(IupGetHandle("mat5"), "REDRAW", "ALL");
    //IupSetAttribute(IupGetHandle("mat6"), "REDRAW", "ALL");
  }
  return IUP_DEFAULT;
}

static int dropselect(Ihandle *self, int lin, int col, Ihandle *drop, char *t, int i, int v)
{
  printf("dropselect_cb(%d, %d, %s, i=%d v=%d)\n", lin, col, t, i, v);
  return IUP_CONTINUE;
 // return IUP_DEFAULT;
}

static int dropcheck(Ihandle *self, int lin, int col)
{
  if(lin == 1 && col == 1)
    return IUP_DEFAULT;
  return IUP_IGNORE;
}

static int click(Ihandle *self, int lin, int col)
{
  char* value = IupGetAttributeId2(self, "", lin, col);
  if (!value) value = "NULL";
  printf("click_cb(%d, %d)\n", lin, col);
  printf("  VALUE%d:%d = %s\n", lin, col, value);
  return IUP_DEFAULT;
}

static int release(Ihandle *self, int lin, int col)
{
  char* value = IupGetAttributeId2(self, "", lin, col);
  if (!value) value = "NULL";
  printf("release_cb(%d, %d)\n", lin, col);
  printf("  VALUE%d:%d = %s\n", lin, col, value);
  return IUP_DEFAULT;
}

static int drop(Ihandle *self, Ihandle *drop, int lin, int col)
{
  printf("drop_cb(%d, %d)\n", lin, col);
  if(lin == 1 && col == 1)
  {
    IupSetAttribute(drop, "1", "A - Test of Very Big String for Dropdown!");
    IupSetAttribute(drop, "2", "B");
    IupSetAttribute(drop, "3", "C");
    IupSetAttribute(drop, "4", "XXX");
    IupSetAttribute(drop, "5", "5");
    IupSetAttribute(drop, "6", "6");
    IupSetAttribute(drop, "7", "7");
    IupSetAttribute(drop, "8", NULL);
    return IUP_DEFAULT;
  }
  return IUP_IGNORE;
}

static int edition(Ihandle *self, int lin, int col, int mode) 
{
  printf("edition_cb(lin=%d, col=%d, mode=%d)\n", lin, col, mode);
  if (mode==1)
  {
    IupSetAttribute(self, "CARET", "3");

    if(lin == 3 && col == 2)
      return IUP_IGNORE;
  }

  //if(lin == 1 && col == 1 && mode==0 && mdrop == 1)
  //{
  //  mdrop = 0;
  //  IupSetAttribute(self, "EDITMODE", "NO");
  //  IupSetAttribute(self, "EDITMODE", "YES");
  //  return IUP_IGNORE;
  //}

  return IUP_DEFAULT;
}

static int drawcb(Ihandle *h, int lin, int col,int x1, int x2, int y1, int y2, cdCanvas* canvas)
{
  if (lin < 5 || lin > 12 || col < 2 || col > 8)
    return IUP_IGNORE;

  cdCanvasForeground(canvas, CD_RED);
  cdCanvasLine(canvas, x1, y1, x2, y2);
  cdCanvasLine(canvas, x1, y2, x2, y1);

  {
    char s[50];
    sprintf(s, "%d:%d", lin, col);
    cdCanvasTextAlignment(canvas, CD_CENTER);
    cdCanvasText(canvas, (x1 + x2) / 2, (y1 + y2) / 2, s);
  }

  return IUP_DEFAULT;
}

static int actioncb(Ihandle *h, int key, int lin, int col, int active, char* after)
{
  printf("action_cb(key=%d, lin=%d, col=%d, active=%d, after=%s)\n", key, lin, col, active, after);

  if (lin == 2 && col == 3 && active && after)
  {
    char str[100];
    strcpy(str, after);
    strcat(str, "xxx");
    IupStoreAttribute(h,"VALUE", str);

    IupSetAttribute(h,"CARET","1");

    IupSetAttribute(h,"REDRAW","ALL");
  }

  /*
  if (key == 45 || (key >= 48 && key <= 57) || (key >= 65 && key <= 90) ||
      (key >= 97 && key <= 122) || key == 95 || key == 8 || key > 255 || key == 13)
  {
    printf("DEFAULT\n");
    return IUP_DEFAULT;
  }

  printf("IGNORE\n");
  return IUP_IGNORE;
  */

  return IUP_DEFAULT;
}

static Ihandle *create_mat(int mati)
{
  Ihandle *mat; 
  char name[30];
  sprintf(name, "mat%d", mati);

  if (mati==1)
  {
    mat = IupMatrixEx();
    IupSetAttribute(mat,"UNDOREDO","Yes"); 
  }
  else
    mat = IupMatrix(NULL);

  IupSetHandle(name, mat);
  
  IupSetAttribute(mat,"NUMCOL","15"); 
  IupSetAttribute(mat,"NUMLIN","18"); 
  
  IupSetAttribute(mat,"NUMCOL_VISIBLE","5");
  IupSetAttribute(mat,"NUMLIN_VISIBLE","8");

//  IupSetAttribute(mat,"EXPAND", "NO");
//  IupSetAttribute(mat,"SCROLLBAR", "NO");
  IupSetAttribute(mat,"RESIZEMATRIX", "YES");

  IupSetAttribute(mat,"MARKMODE", "CELL");
//  IupSetAttribute(mat,"MARKMODE", "LINCOL");
  IupSetAttribute(mat,"MARKMULTIPLE", "YES");
  IupSetAttribute(mat,"MARKAREA", "NOT_CONTINUOUS");
// IupSetAttribute(mat, "MARKAREA", "CONTINUOUS");

  IupSetAttribute(mat,"0:0","Test");
  IupSetAttribute(mat,"1:0","Medicine");
  IupSetAttribute(mat,"2:0","Food"); 
  IupSetAttribute(mat,"3:0","Energy"); 
  IupSetAttribute(mat,"0:1","January 2000"); 
  IupSetAttribute(mat,"0:2","February 2000"); 
  IupSetAttribute(mat,"1:1","5.6");
  IupSetAttribute(mat,"2:1","2.2");
  IupSetAttribute(mat,"3:1","7.2");
  IupSetAttribute(mat,"1:2","4.5");
  IupSetAttribute(mat,"2:2","8.1");
  IupSetAttribute(mat,"3:2","3.4 (RO)");

  IupSetAttribute(mat, "PASTEFILEAT", "1:1");

  IupSetAttribute(mat, "NUMERICFORMATTITLE3", "%s (%s)");
  IupSetAttribute(mat, "NUMERICQUANTITY3", "Time");
  IupSetAttribute(mat, "NUMERICUNIT3", "day");
  IupSetAttribute(mat, "NUMERICUNITSHOWN3", "day");
  IupSetAttribute(mat,"0:3","Time");
  IupSetAttribute(mat,"1:3","1");
  IupSetAttribute(mat,"2:3","1.5");
  IupSetAttribute(mat,"3:3","2");
  IupSetAttribute(mat, "NUMERICDECIMALSYMBOL", ",");
  
//  IupSetAttribute(mat,"BGCOLOR1:*","255 128 0");
  IupSetAttribute(mat,"BGCOLOR2:1","255 128 0");
  IupSetAttribute(mat,"FGCOLOR2:0","255 0 128");
//  IupSetAttribute(mat,"BGCOLOR0:*","255 0 128");
  IupSetAttribute(mat,"FGCOLOR1:1","255 0 128");
  IupSetAttribute(mat,"BGCOLOR3:*","255 128 0");
  IupSetAttribute(mat,"BGCOLOR*:4","255 128 0");
  //IupSetAttribute(mat,"FONT2:*", "Times:BOLD:8");
  //IupSetAttribute(mat,"FONT*:2", "Courier::12");
  IupSetAttribute(mat,"SORTSIGN1","UP");
//  IupSetAttribute(mat,"SORTSIGN2","DOWN");
  IupSetAttribute(mat,"FRAMEVERTCOLOR2:2","255 255 255");
  IupSetAttribute(mat, "CELLBYTITLE", "Yes");

//  IupSetAttribute(mat,"MARKAREA","NOT_CONTINUOUS");
//  IupSetAttribute(mat,"MARKMULTIPLE","YES");

  IupSetCallback(mat,"LEAVEITEM_CB",(Icallback)leave);
  IupSetCallback(mat,"ENTERITEM_CB",(Icallback)enter);
  IupSetCallback(mat,"DROPSELECT_CB",(Icallback)dropselect);
  IupSetCallback(mat,"DROP_CB",(Icallback)drop);
  IupSetCallback(mat,"DROPCHECK_CB",(Icallback)dropcheck);
  IupSetCallback(mat,"EDITION_CB",(Icallback)edition);
  IupSetCallback(mat,"CLICK_CB",(Icallback)click);
  IupSetCallback(mat,"RELEASE_CB",(Icallback)release);
  IupSetCallback(mat,"DRAW_CB",(Icallback)drawcb);
  IupSetCallback(mat,"ACTION_CB",(Icallback)actioncb);

//  IupSetCallback(mat,"VALUE_CB",(Icallback)value);
//  IupSetAttribute(mat,"WIDTH0","24");
//  IupSetAttribute(mat,"HEIGHT0","8");

  IupSetAttribute(mat, "FLATSCROLLBAR", "Yes");
//  IupSetAttribute(mat, "SHOWFLOATING", "Yes");

  return mat;
}


static int redraw(Ihandle *self) 
{
  IupSetAttribute(IupGetHandle("mat1"),"REDRAW","ALL"); 
  IupSetAttribute(IupGetHandle("mat2"),"REDRAW","ALL"); 
  //IupSetAttribute(IupGetHandle("mat3"),"REDRAW","ALL"); 
  //IupSetAttribute(IupGetHandle("mat4"),"REDRAW","ALL"); 
  //IupSetAttribute(IupGetHandle("mat5"),"REDRAW","ALL"); 
  //IupSetAttribute(IupGetHandle("mat6"),"REDRAW","ALL"); 

  //Ihandle* mat = IupGetHandle("mat1");
  //if (IupGetInt(mat, "VISIBLE"))
  //{
  //  IupSetAttribute(mat,"VISIBLE","NO"); 
  //  IupStoreAttribute(mat, "OLD_SIZE", IupGetAttribute(mat, "RASTERSIZE"));
  //  IupSetAttribute(mat, "RASTERSIZE", "1x1");
  //}
  //else
  //{
  //  IupStoreAttribute(mat, "RASTERSIZE", IupGetAttribute(mat, "OLD_SIZE"));
  //  IupSetAttribute(mat,"VISIBLE","YES"); 
  //}

  return IUP_DEFAULT;
}

static int removeline(Ihandle *self) 
{
//  IupSetAttribute(IupGetHandle("mat1"),"ORIGIN","5:1"); 
    IupSetAttribute(IupGetHandle("mat1"),"DELLIN","1"); 
//  IupSetAttribute(IupGetHandle("mat1"),"NUMLIN","0"); 
  return IUP_DEFAULT;
}

static int addline(Ihandle *self) 
{
  IupSetAttribute(IupGetHandle("mat1"),"ADDLIN","0"); 
//  IupSetAttribute(IupGetHandle("mat1"),"ADDLIN","0-5"); 
//  IupSetAttribute(IupGetHandle("mat1"),"NUMCOL_NOSCROLL","2");
//  IupSetAttribute(IupGetHandle("mat1"),"NUMLIN_NOSCROLL","2");
  return IUP_DEFAULT;
}

static int removecol(Ihandle *self) 
{
  IupSetAttribute(IupGetHandle("mat1"),"DELCOL","1"); 
  return IUP_DEFAULT;
}

static int addcol(Ihandle *self) 
{
  IupSetAttribute(IupGetHandle("mat1"),"ADDCOL","0"); 
  return IUP_DEFAULT;
}

static int bt_cb(Ihandle *self) 
{
  printf("DEFAULTENTER\n"); 

//  IupHide(IupGetHandle("mat1")); 

//  IupSetAttribute(IupGetHandle("mat1"),"CLEARVALUE*:2","1-10"); 
//  IupSetAttribute(IupGetHandle("mat1"),"CLEARVALUE2:*","1-10"); 
//  IupSetAttribute(IupGetHandle("mat1"),"CLEARVALUE2:2","10-10"); 
//  IupSetAttribute(IupGetHandle("mat1"),"CLEARVALUE","ALL"); 
//  IupSetAttribute(IupGetHandle("mat1"),"CLEARVALUE","CONTENTS"); 

//  IupSetAttribute(IupGetHandle("mat1"),"CLEARATTRIB2:*","ALL"); 
//  IupSetAttribute(IupGetHandle("mat1"),"CLEARATTRIB3:*","ALL"); 
  return IUP_DEFAULT;
}

static void createmenu(void)
{
  Ihandle* menu = IupMenu(
    IupSubmenu("submenu", IupMenu(IupItem("item1","x"), IupItem("item2","x"), NULL)),
    IupItem("remove line","removeline"), 
    IupItem("add line","addline"), 
    IupItem("remove col","removecol"), 
    IupItem("add col","addcol"), 
    IupItem("redraw","redraw"), 
    NULL);
  IupSetHandle("mymenu", menu);
}

void MatrixCbsTest(void)
{
  Ihandle *dlg, *bt;

  IupSetFunction("removeline", (Icallback)removeline);
  IupSetFunction("addline", (Icallback)addline);
  IupSetFunction("removecol", (Icallback)removecol);
  IupSetFunction("addcol", (Icallback)addcol);
  IupSetFunction("redraw", (Icallback)redraw);

  createmenu();

  bt = IupButton("Button", NULL);
  IupSetCallback(bt, "ACTION", bt_cb);
  IupSetAttribute(bt, "CANFOCUS", "NO");

  dlg = IupDialog(
//          IupZbox(
          IupTabs(
            IupSetAttributes(
              IupVbox((create_mat(1)), bt, IupText(""), IupLabel("Label Text"), IupFrame(IupVal("HORIZONTAL")), 
                NULL), "MARGIN=10x10, GAP=10, TABTITLE=Test1"),
            IupSetAttributes(
              IupVbox(IupFrame(create_mat(2)), IupText(""), IupLabel("Label Text"), IupVal("HORIZONTAL"), 
//                NULL), "BGCOLOR=\"0 255 255\", MARGIN=10x10, GAP=10, TABTITLE=Test2,FONT=HELVETICA_ITALIC_14"), 
//                NULL), "FONT=HELVETICA_NORMAL_12, BGCOLOR=\"0 255 255\", MARGIN=10x10, GAP=10, TABTITLE=Test2"), 
                NULL), "BGCOLOR=\"0 255 255\", MARGIN=10x10, GAP=10, TABTITLE=Test2"), 
            NULL)); 
  IupSetAttribute(dlg,"TITLE", "IupMatrix");
  IupSetAttribute(dlg,"MENU", "mymenu");
  IupSetAttributeHandle(dlg,"DEFAULTENTER", bt);
//  IupSetAttribute(dlg,"BGCOLOR", "255 0 255");

  //IupSetAttribute(dlg,"COMPOSITED", "YES");
  //IupSetAttribute(dlg,"OPACITY", "192");

  IupShowXY(dlg,IUP_CENTER,IUP_CENTER);
}

#ifndef BIG_TEST
int main(int argc, char* argv[])
{
  IupOpen(&argc, &argv);
  IupControlsOpen();
  IupImageLibOpen();

  MatrixCbsTest();

  IupMainLoop();

  IupClose();

  return EXIT_SUCCESS;
}
#endif
