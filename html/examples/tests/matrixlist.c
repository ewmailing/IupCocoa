#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <iup.h>
#include <iupcontrols.h>
#include <cd.h>


static int imagevaluechanged_cb(Ihandle *self, int item, int state)
{
	printf("imagevaluechanged_cb(item=%d, state=%d)\n", item, state);
	return IUP_DEFAULT;
}

static int listclick_cb(Ihandle *self, int lin, int col, char *status)
{
  char* value = IupGetAttributeId(self, "", lin);
  if (!value) value = "NULL";
  printf("listclick_cb(%d, %d)\n", lin, col);
  printf("  VALUE%d:%d = %s\n", lin, col, value);
  return IUP_DEFAULT;
}

static int listaction_cb(Ihandle *self, int item, int state)
{
  printf("listaction_cb(item=%d, state=%d)\n", item, state);
  return IUP_DEFAULT;
}

void MatrixListTest(void)
{
  Ihandle *dlg, *mlist;

  mlist = IupMatrixList();
  IupSetInt(mlist, "COUNT", 10);
  IupSetInt(mlist, "VISIBLELINES", 9);
    IupSetAttribute(mlist, "COLUMNORDER", "LABEL:COLOR:IMAGE");
//  IupSetAttribute(mlist, "COLUMNORDER", "LABEL:COLOR");
//  IupSetAttribute(mlist, "COLUMNORDER", "LABEL");
//  IupSetAttribute(mlist, "ACTIVE", "NO");
//  IupSetAttribute(mlist, "FOCUSCOLOR", "BGCOLOR");
  IupSetAttribute(mlist, "SHOWDELETE", "Yes");
//  IupSetAttribute(mlist, "EXPAND", "Yes");

  IupSetAttribute(mlist, "EDITABLE", "Yes");
  IupSetCallback(mlist,"LISTCLICK_CB",(Icallback)listclick_cb);
  IupSetCallback(mlist,"LISTACTION_CB",(Icallback)listaction_cb);
  IupSetCallback(mlist, "IMAGEVALUECHANGED_CB", (Icallback)imagevaluechanged_cb);
  
  /* Bluish style */
  if (1)
  {
    IupSetAttribute(mlist, "TITLE", "Test");
    IupSetAttribute(mlist, "BGCOLOR", "220 230 240");
    IupSetAttribute(mlist, "FRAMECOLOR", "120 140 160");
    IupSetAttribute(mlist, "ITEMBGCOLOR0", "120 140 160");
    IupSetAttribute(mlist, "ITEMFGCOLOR0", "255 255 255");
  }

  IupSetAttribute(mlist, "1", "AAA");
  IupSetAttribute(mlist, "2", "BBB");
  IupSetAttribute(mlist, "3", "CCC");
  IupSetAttribute(mlist, "4", "DDD");
  IupSetAttribute(mlist, "5", "EEE");
  IupSetAttribute(mlist, "6", "FFF");
  IupSetAttribute(mlist, "7", "GGG");
  IupSetAttribute(mlist, "8", "HHH");
  IupSetAttribute(mlist, "9", "III");
  IupSetAttribute(mlist, "10","JJJ");

  IupSetAttribute(mlist, "COLOR1", "255 0 0");
  IupSetAttribute(mlist, "COLOR2", "255 255 0");
  //IupSetAttribute(mlist, "COLOR3", "0 255 0");
  IupSetAttribute(mlist, "COLOR4", "0 255 255");
  IupSetAttribute(mlist, "COLOR5", "0 0 255");
  IupSetAttribute(mlist, "COLOR6", "255 0 255");
  IupSetAttribute(mlist, "COLOR7", "255 128 0");
  IupSetAttribute(mlist, "COLOR8", "255 128 128");
  IupSetAttribute(mlist, "COLOR9", "0 255 128");
  IupSetAttribute(mlist, "COLOR10", "128 255 128");

  IupSetAttribute(mlist, "ITEMACTIVE3", "NO");
  IupSetAttribute(mlist, "ITEMACTIVE7", "NO");
  IupSetAttribute(mlist, "ITEMACTIVE8", "NO");

  IupSetAttribute(mlist, "IMAGEACTIVE9", "No");

  IupSetAttribute(mlist, "IMAGEVALUE1", "ON");
  IupSetAttribute(mlist, "IMAGEVALUE2", "ON");
  IupSetAttribute(mlist, "IMAGEVALUE3", "ON");

  dlg = IupDialog(IupVbox(mlist, NULL));
  IupSetAttribute(dlg, "TITLE", "IupMatrixList");
  IupSetAttribute(dlg, "MARGIN", "10x10");
//  IupSetAttribute(dlg, "FONT", "Helvetica, 24");
  IupShowXY(dlg, IUP_CENTER, IUP_CENTER);

  IupSetAttribute(mlist, "APPENDITEM","KKK");
}

#ifndef BIG_TEST
int main(int argc, char* argv[])
{
  IupOpen(&argc, &argv);
  IupControlsOpen();

  MatrixListTest();

  IupMainLoop();

  IupClose();

  return EXIT_SUCCESS;
}
#endif
