#include <stdlib.h>
#include <stdio.h>
#include <iup.h>

/* Initializes the dialog */
void init_dlg(void)
{
  Ihandle* tree1 = IupGetHandle("tree1");
  Ihandle* tree2 = IupGetHandle("tree2");
  Ihandle* box = IupVbox(IupHbox(tree1, tree2, NULL), NULL);
  Ihandle* dlg = IupDialog(box);
  IupSetAttribute(dlg, "TITLE", "IupTree Example");
  IupSetAttribute(box, "MARGIN", "20x20");
  IupSetHandle("dlg", dlg);
}

/* Initializes the IupTree TWO attributes */
void init_tree2_atributes(void)
{
  Ihandle* tree2 = IupGetHandle("tree2");

  IupSetAttribute(tree2, "TITLE", "Frutas");
  IupSetAttribute(tree2, "ADDBRANCH", "Outras");
  IupSetAttribute(tree2, "ADDLEAF", "morango");
  IupSetAttribute(tree2, "ADDLEAF", "maçã");
  IupSetAttribute(tree2, "ADDBRANCH1", "Cítricas");
  IupSetAttribute(tree2, "ADDLEAF2", "limão");
  IupSetAttribute(tree2, "ADDLEAF2", "laranja");
  IupSetAttribute(tree2, "ADDLEAF2", "cajá");
  IupSetAttribute(tree2, "VALUE", "3");
}

/* Initializes the IupTree ONE attributes */
void init_tree1_atributes(void)
{
  Ihandle* tree1 = IupGetHandle("tree1");

  IupSetAttribute(tree1, "TITLE", "Figures");
  IupSetAttribute(tree1, "ADDBRANCH", "3D");
  IupSetAttribute(tree1, "ADDBRANCH", "2D");
  IupSetAttribute(tree1, "ADDLEAF", "test");
  IupSetAttribute(tree1, "ADDBRANCH1", "parallelogram");
  IupSetAttribute(tree1, "ADDLEAF2", "diamond");
  IupSetAttribute(tree1, "ADDLEAF2", "square");
  IupSetAttribute(tree1, "ADDBRANCH1", "triangle");
  IupSetAttribute(tree1, "ADDLEAF2", "scalenus");
  IupSetAttribute(tree1, "ADDLEAF2", "isoceles");
  IupSetAttribute(tree1, "ADDLEAF2", "equilateral");
  IupSetAttribute(tree1, "ADDBRANCH2", "new branch");
  IupSetAttribute(tree1, "ADDLEAF3", "child");
  IupSetAttribute(tree1, "ADDBRANCH3", "nothing");
  IupSetAttribute(tree1, "VALUE", "6");
  IupSetAttribute(tree1, "COLOR5", "0 0 255");
  IupSetAttribute(tree1, "TITLEFONT6", "Courier, 12");
}

/* Main program */
int main(int argc, char **argv)
{
  Ihandle* dlg;
  Ihandle *tree1, *tree2;
  
  IupOpen(&argc, &argv);                  /* IUP initialization */

  IupSetGlobal("UTF8MODE", "Yes");

  tree1 = IupTree();
  IupSetHandle("tree1", tree1);
  
  tree2 = IupTree();
  IupSetHandle("tree2", tree2);

  //Generic DND
  //IupSetAttribute(tree1, "SHOWDRAGDROP", "YES");
  //IupSetAttribute(tree1, "DRAGTYPES", "TEXT,STRING");
  //Tree2Tree DND
  IupSetAttribute(tree1, "DRAGDROPTREE", "YES");
  //Common DND Attrib
  IupSetAttribute(tree1, "DRAGSOURCE", "YES");
  IupSetAttribute(tree1, "DRAGSOURCEMOVE", "YES");
  IupSetAttribute(tree1, "DRAGTYPES", "TREEBRANCH");

  //Generic DND
  //IupSetAttribute(tree2, "SHOWDRAGDROP", "YES");
  //IupSetAttribute(tree2, "DROPTYPES", "TEXT,STRING");  
  //Tree2Tree DND
  IupSetAttribute(tree2, "DRAGDROPTREE", "YES");
  //Common DND Attrib
  IupSetAttribute(tree2, "DROPTARGET", "YES");
  IupSetAttribute(tree2, "DROPTYPES", "TREEBRANCH");  
  
  init_dlg();                             /* Initializes the dialog */
  dlg = IupGetHandle("dlg");              /* Retrieves the dialog handle */
  IupShowXY(dlg, IUP_CENTER, IUP_CENTER); /* Displays the dialog */

  init_tree1_atributes();                 /* Initializes attributes, can be done here or anywhere */
  init_tree2_atributes();                 /* Initializes attributes, can be done here or anywhere */

  IupMainLoop();                          /* Main loop */
  IupClose();                             /* Ends IUP */

  return EXIT_SUCCESS;

}
