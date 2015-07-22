#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "iup.h"
#include "iupkey.h"

//#define CUSTOM_FORMAT

static int copy_cb(Ihandle* ih)
{
#ifdef CUSTOM_FORMAT
  char* str;
#endif
  Ihandle* text = IupGetDialogChild(ih, "text");
  Ihandle* clipboard = IupClipboard();
#ifndef CUSTOM_FORMAT
  IupSetAttribute(clipboard, "TEXT", NULL);
  IupSetAttribute(clipboard, "TEXT", IupGetAttribute(text, "VALUE"));
#else
  IupSetAttribute(clipboard, "FORMAT", "TESTFORMAT");
  str = IupGetAttribute(text, "VALUE");
  IupSetfAttribute(clipboard, "FORMATDATASIZE", "%d", strlen(str)+1);
  IupSetAttribute(clipboard, "FORMATDATA", str);
#endif
  IupDestroy(clipboard);
  return IUP_DEFAULT;
}

static int paste_cb(Ihandle* ih)
{
  Ihandle* text = IupGetDialogChild(ih, "text");
  Ihandle* clipboard = IupClipboard();
#ifndef CUSTOM_FORMAT
  IupSetAttribute(text, "VALUE", IupGetAttribute(clipboard, "TEXT"));
#else
  IupSetAttribute(clipboard, "FORMAT", "TESTFORMAT");
  IupSetAttribute(text, "VALUE", IupGetAttribute(clipboard, "FORMATDATA"));
#endif
  IupDestroy(clipboard);
  return IUP_DEFAULT;
}

void ClipboardTest(void)
{
  Ihandle *dlg;
#ifdef CUSTOM_FORMAT
  Ihandle* clipboard = IupClipboard();
  IupSetAttribute(clipboard, "ADDFORMAT", "TESTFORMAT");
  IupDestroy(clipboard);
#endif

  dlg = IupDialog(IupVbox(
    IupSetAttributes(IupText(NULL), "SIZE=80x60, MULTILINE=YES, NAME=text, EXPAND=YES"),
    IupSetCallbacks(IupButton("Copy", NULL), "ACTION", copy_cb, NULL), 
    IupSetCallbacks(IupButton("Paste", NULL), "ACTION", paste_cb, NULL), 
    NULL));
  IupSetAttribute(dlg, "GAP", "10");
  IupSetAttribute(dlg, "MARGIN", "20x20");

  IupSetAttribute(dlg, "TITLE", "Clipboard Test");
  IupShow(dlg);
}

#ifndef BIG_TEST
int main(int argc, char* argv[])
{
  IupOpen(&argc, &argv);

  ClipboardTest();

  IupMainLoop();

  IupClose();

  return EXIT_SUCCESS;
}
#endif
