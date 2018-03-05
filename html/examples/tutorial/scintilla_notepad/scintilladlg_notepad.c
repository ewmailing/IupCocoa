#include <stdio.h>
#include <stdlib.h>

#include <iup.h>
#include <iup_scintilla.h>
#include <iup_config.h>


static int item_help_action_cb(void)
{
  IupHelp("http://www.tecgraf.puc-rio.br/iup");
  return IUP_DEFAULT;
}

static int item_about_action_cb(void)
{
  IupMessage("About", "   Scintilla Notepad\n\nAutors:\n   Camilo Freire\n   Antonio Scuri");
  return IUP_DEFAULT;
}

int main(int argc, char **argv)
{
  Ihandle *main_dialog;
  Ihandle *config;
  Ihandle *menu;
  int i;

  IupOpen(&argc, &argv);
  IupImageLibOpen();
  IupScintillaOpen();

#ifdef _DEBUG
  IupSetGlobal("GLOBALLAYOUTDLGKEY", "Yes");
#endif

  config = IupConfig();
  IupSetAttribute(config, "APP_NAME", "scintilla_notepad");
  IupConfigLoad(config);

  main_dialog = IupScintillaDlg();

  IupSetAttribute(main_dialog, "SUBTITLE", "Scintilla Notepad");
  IupSetAttributeHandle(main_dialog, "CONFIG", config);

  menu = IupGetAttributeHandle(main_dialog, "MENU");
  IupAppend(menu, IupSubmenu("&Help", IupMenu(
    IupSetCallbacks(IupItem("&Help...", NULL), "ACTION", (Icallback)item_help_action_cb, NULL),
    IupSetCallbacks(IupItem("&About...", NULL), "ACTION", (Icallback)item_about_action_cb, NULL),
    NULL)));

  /* show the dialog at the last position, with the last size */
  IupConfigDialogShow(config, main_dialog, IupGetAttribute(main_dialog, "SUBTITLE"));

  /* open a file from the command line (allow file association in Windows) */
  for (i = 1; i < argc; i++)
  {
    const char* filename = argv[i];
    IupSetStrAttribute(main_dialog, "OPENFILE", filename);
  }

  IupMainLoop();

  IupDestroy(config);
  IupDestroy(main_dialog);

  IupClose();
  return EXIT_SUCCESS;
}

