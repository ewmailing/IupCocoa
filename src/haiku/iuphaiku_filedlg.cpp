/** \file
 * \brief IupFileDlg pre-defined dialog
 *
 * See Copyright Notice in "iup.h"
 */

#define UNIMPLEMENTED printf("%s (%s %d) UNIMPLEMENTED\n",__func__,__FILE__,__LINE__);

#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <stdio.h>

#include <FilePanel.h>
#include <Looper.h>
#include <Messenger.h>
#include <Path.h>
#include <Window.h>

#include "iup.h"
#include "iupcbs.h"

#include "iup_object.h"
#include "iup_attrib.h"
#include "iup_str.h"
#include "iup_drvinfo.h"
#include "iup_dialog.h"
#include "iup_strmessage.h"
#include "iup_array.h"
#include "iup_drvinfo.h"

#include "iuphaiku_drv.h"


class FileHandler: public BLooper
{
	public:

	FileHandler()
		: BLooper("filepanel listener")
	{
		lock = create_sem(0, "filepanel sem");
		path = NULL;

		Run();
	}

	~FileHandler()
	{
		delete_sem(lock);
		free(path);
	}

	void MessageReceived(BMessage* message)
	{
		switch(message->what)
		{
			case B_REFS_RECEIVED:
			{
				entry_ref ref;

				message->FindRef("refs", 0, &ref);
				BEntry entry(&ref);
				BPath xpath;
				entry.GetPath(&xpath);
				path = strdup(xpath.Path());
				break;
			}

			case B_SAVE_REQUESTED:
			{
				entry_ref ref;

				message->FindRef("directory", 0, &ref);
				BEntry entry(&ref);
				BPath xpath;
				entry.GetPath(&xpath);
				const char* dir = xpath.Path();

				const char* name;
			    message->FindString("name", 0, &name);
				path = (char*)malloc(strlen(name) + strlen(dir) + 1);
				strcpy(path, dir);
				strcat(path, name);
				break;
			}

			case B_CANCEL:
			{
				// If the path was set, do not change it.
				// If it was not set, leave it null and HT will not load anything
				break;
			}

			default:
				printf("Unhandled message %lx\n", message->what);
				BHandler::MessageReceived(message);
				return;
		}

		release_sem(lock);
	}

	void Wait() {
		acquire_sem(lock);
	}

	char* path;

	private:
		sem_id lock;
};

static int gtkFileDlgPopup(Ihandle* ih, int x, int y)
{
  file_panel_mode mode;
  uint32 flavour;
  char* value = iupAttribGetStr(ih, "DIALOGTYPE");
  if (iupStrEqualNoCase(value, "SAVE"))
    mode = B_SAVE_PANEL;
  else {
    mode = B_OPEN_PANEL;
	if (iupStrEqualNoCase(value, "DIR"))
	  flavour = B_DIRECTORY_NODE;
    else
	  flavour = B_FILE_NODE;
  }

  
  //if (IupGetCallback(ih, "HELP_CB"))
  //if (iupAttribGetBoolean(ih, "MULTIPLEFILES") && action == GTK_FILE_CHOOSER_ACTION_OPEN)
  //if (!iupAttribGetBoolean(ih, "NOOVERWRITEPROMPT") && action == GTK_FILE_CHOOSER_ACTION_SAVE)

  FileHandler* handler = new FileHandler();
  BMessenger messenger(handler);
  BFilePanel* panel = new BFilePanel(mode, &messenger, NULL, flavour);
  
  /* just check for the path inside FILE */
  value = iupAttribGet(ih, "FILE");
  if (value && (value[0] == '/' || value[1] == ':'))
  {
    char* dir = iupStrFileGetPath(value);
    int len = strlen(dir);
    iupAttribSetStr(ih, "DIRECTORY", dir);
    free(dir);
    iupAttribSetStr(ih, "FILE", value+len);
  }

  value = iupAttribGet(ih, "DIRECTORY");
  if (value)
    panel->SetPanelDirectory(new BDirectory(value));
    //iupgtkStrConvertToFilename(value);
  value = iupAttribGet(ih, "FILE");
  // TODO preset the textfield with the file name
  
  value = iupAttribGet(ih, "TITLE");
  BWindow* win = panel->Window();
  win->LockLooper();
  win->SetTitle(value);
  win->UnlockLooper();
  
  //value = iupAttribGet(ih, "EXTFILTER");
    //value = iupAttribGet(ih, "FILTER");
    //char* info = iupAttribGet(ih, "FILTERINFO");
  
  //file_cb = (IFnss)IupGetCallback(ih, "FILE_CB");
  //if (iupAttribGetBoolean(ih, "SHOWPREVIEW"))

  panel->Show();
  handler->Wait();
  delete panel;

  if (iupAttribGetBoolean(ih, "MULTIPLEFILES"))
	UNIMPLEMENTED

  iupAttribSetStr(ih, "VALUE", handler->path);

  int file_exist = iupdrvIsFile(handler->path);
  int dir_exist = iupdrvIsDirectory(handler->path);
  
  if (file_exist)
  {
    char* dir = iupStrFileGetPath(handler->path);
    iupAttribSetStr(ih, "DIRECTORY", dir);
    free(dir);
  }

  if (dir_exist)
  {
    iupAttribSet(ih, "FILEEXIST", NULL);
    iupAttribSet(ih, "STATUS", "0");
  }
  else
  {
    if (file_exist)  /* check if file exists */
    {
      iupAttribSet(ih, "FILEEXIST", "YES");
      iupAttribSet(ih, "STATUS", "0");
    }
    else
    {
      iupAttribSet(ih, "FILEEXIST", "NO");
      iupAttribSet(ih, "STATUS", "1");
    }
  }
  // put results
  //if (action != GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER && !iupAttribGetBoolean(ih, "NOCHANGEDIR"))  /* do change the current directory */
  messenger.SendMessage(B_QUIT_REQUESTED);
  return IUP_NOERROR;
}

extern "C" void iupdrvFileDlgInitClass(Iclass* ic)
{
  ic->DlgPopup = gtkFileDlgPopup;

  /* IupFileDialog Windows and GTK Only */
  iupClassRegisterAttribute(ic, "EXTFILTER", NULL, NULL, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "FILTERINFO", NULL, NULL, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "FILTERUSED", NULL, NULL, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "MULTIPLEFILES", NULL, NULL, NULL, NULL, IUPAF_NO_INHERIT);
}
