#include <stdlib.h>
#include <stdio.h>
#include <iup.h>

static int restored_cb(Ihandle *ih, Ihandle* old_parent)
{
  IupSetAttribute(IupGetHandle("restore"), "ACTIVE", "NO");
  IupSetAttribute(IupGetHandle("detach"), "ACTIVE", "YES");
  printf("Restored!\n");

  (void)ih;
  (void)old_parent;
  return IUP_DEFAULT;
}

static int detached_cb(Ihandle *ih, Ihandle* new_parent, int x, int y)
{
  IupSetAttribute(new_parent, "TITLE", "New Dialog");

  IupSetAttribute(IupGetHandle("restore"), "ACTIVE", "YES");
  IupSetAttribute(IupGetHandle("detach"), "ACTIVE", "NO");
  printf("Detached!\n");

  (void)ih;
  (void)x;
  (void)y;
  return IUP_DEFAULT;
}

static int btn_restore_cb(Ihandle *bt)
{
  Ihandle *dbox = IupGetHandle("dbox");

#if 0
  Ihandle *dlg = IupGetDialog(dbox);
  Ihandle *old_parent = (Ihandle*)IupGetAttribute(dbox, "OLDPARENT_HANDLE");
  Ihandle *ref_child = (Ihandle*)IupGetAttribute(dbox, "OLDBROTHER_HANDLE");
  IupReparent(dbox, old_parent, ref_child);
  IupRefresh(old_parent);
  IupDestroy(dlg);
#else
  IupSetAttribute(dbox, "RESTORE", NULL);
#endif
  
  IupSetAttribute(bt, "ACTIVE", "NO");
  IupSetAttribute(IupGetHandle("detach"), "ACTIVE", "Yes");

  return IUP_DEFAULT;
}

static int btn_detach_cb(Ihandle *bt)
{
  Ihandle *dbox = IupGetHandle("dbox");

  IupSetAttribute(dbox, "DETACH", NULL);

  IupSetAttribute(bt, "ACTIVE", "NO");
  IupSetAttribute(IupGetHandle("restore"), "ACTIVE", "Yes");

  return IUP_DEFAULT;
}

void DetachBoxTest(void)
{
  Ihandle *dlg, *bt, *dbox, *lbl, *ml, *hbox, *bt2, *txt;

  bt = IupButton("Detache Me!", NULL);
  IupSetCallback(bt, "ACTION", (Icallback)btn_detach_cb);
  IupSetHandle("detach", bt);

  ml = IupMultiLine(NULL);
  IupSetAttribute(ml, "EXPAND", "YES");
  IupSetAttribute(ml, "VISIBLELINES", "5");

  hbox = IupHbox(bt, ml, NULL);
  IupSetAttribute(hbox, "MARGIN", "10x0");

  dbox = IupDetachBox(hbox);
  IupSetAttribute(dbox, "ORIENTATION", "VERTICAL");
  //IupSetAttribute(dbox, "SHOWGRIP", "NO");
  //IupSetAttribute(dbox, "BARSIZE", "0");
  //IupSetAttribute(dbox, "COLOR", "255 0 0");
  IupSetAttribute(dbox, "RESTOREWHENCLOSED", "Yes");
  IupSetCallback(dbox, "RESTORED_CB", (Icallback)restored_cb);
  IupSetCallback(dbox, "DETACHED_CB", (Icallback)detached_cb);
  IupSetHandle("dbox", dbox);

  lbl = IupLabel("Label");
  IupSetAttribute(lbl, "EXPAND", "VERTICAL");

  bt2 = IupButton("Restore me!", NULL);
  IupSetAttribute(bt2, "EXPAND", "YES");
  IupSetAttribute(bt2, "ACTIVE", "NO");
  IupSetCallback(bt2, "ACTION", (Icallback)btn_restore_cb);
  IupSetHandle("restore", bt2);

  txt = IupText(NULL);
  IupSetAttribute(txt, "EXPAND", "HORIZONTAL");

  dlg = IupDialog(IupVbox(dbox, lbl, bt2, txt, NULL));
  IupSetAttribute(dlg, "TITLE", "IupDetachBox Example");
  IupSetAttribute(dlg, "MARGIN", "10x10");
  IupSetAttribute(dlg, "GAP", "10");
//  IupSetAttribute(dlg, "RASTERSIZE", "300x300");

  IupShow(dlg);
}

#ifndef BIG_TEST
int main(int argc, char* argv[])
{
  IupOpen(&argc, &argv);

  DetachBoxTest();

  IupMainLoop();

  IupClose();

  return EXIT_SUCCESS;
}
#endif
