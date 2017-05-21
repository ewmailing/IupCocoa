#include <iup.h>
#include <stdlib.h>

int fnSrcFile(void) { return IUP_DEFAULT; }
int fnDstFile(void) { return IUP_DEFAULT; }
int fnBtnOk(void) { return IUP_DEFAULT; }
int fnBtnQuit(void) { return IUP_CLOSE; }

int main(int argc, char **argv) 
{
 Ihandle *dlg, *gbox;
  Ihandle *lbl1;
  Ihandle *lbl2;
  Ihandle *txt1;
  Ihandle *btn1;
  Ihandle *lbl3;
  Ihandle *lbl4;
  Ihandle *txt2;
  Ihandle *btn2;
  Ihandle *lbl5;
  Ihandle *lbl6;
  Ihandle *btn3;
  Ihandle *btn4;
 
 IupOpen(&argc, &argv);

 IupSetGlobal("GLOBALLAYOUTDLGKEY", "Yes");

 lbl1 = IupLabel("SOURCE FILE");
 lbl2 = IupLabel(":");
 lbl3 = IupLabel("DST DIR");
 lbl4 = IupLabel(":");
 txt1 = IupText(NULL);
 IupSetAttribute(txt1,"RASTERSIZE","125");
 IupSetAttribute,(txt1,"MULTILINE","NO");
 txt2 = IupText(NULL);
 IupSetAttribute(txt2,"RASTERSIZE","125");
 IupSetAttribute(txt2,"MULTILINE","NO");
 btn1 = IupButton("SRC",NULL);
 IupSetCallback(btn1,"ACTION",(Icallback)fnSrcFile);
 btn2 = IupButton("DST",NULL);
 IupSetCallback(btn2, "ACTION", (Icallback)fnDstFile);
 lbl5 = IupLabel("");   
 lbl6 = IupLabel("");
 btn3 = IupButton("OK",NULL);
 IupSetCallback(btn3, "ACTION", (Icallback)fnBtnOk);
 btn4 = IupButton("QUIT",NULL);
 IupSetCallback(btn4, "ACTION", (Icallback)fnBtnQuit);
 
 gbox = IupGridBox(lbl1,
   lbl2,
   txt1,
   btn1,
   lbl3,
   lbl4,
   txt2,
   btn2,
   lbl5,
   lbl6,
   btn3,
   btn4,
   NULL);

 IupSetAttribute(gbox,"ORIENTATION","HORIZONTAL");
 IupSetAttribute(gbox,"NUMDIV","4");
// IupSetAttribute(gbox,"SIZELIN","0");
// IupSetAttribute(gbox,"SIZECOL","2");
 IupSetAttribute(gbox, "SIZELIN", "-1");
 IupSetAttribute(gbox, "SIZECOL", "-1");
 IupSetAttribute(gbox, "MARGIN", "30x30");
 IupSetAttribute(gbox,"GAPCOL","30");
 IupSetAttribute(gbox,"GAPLIN","30");
 IupSetAttribute(gbox,"ALIGNMENTLIN","ACENTER");
 IupSetAttribute(gbox,"ALIGNMENTCOL","ARIGHT");
// IupSetAttribute(gbox, "ALIGNMENTLIN", "ATOP");
// IupSetAttribute(gbox, "ALIGNMENTCOL", "ALEFT");

 dlg = IupDialog(gbox);
 IupSetAttribute(dlg,"TITLE","Hello World");
 IupShowXY(dlg,IUP_CENTER,IUP_CENTER);
 
 IupMainLoop();
 IupClose();
}
