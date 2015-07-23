/** \file
 * \brief GTK Driver Core
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdio.h>          
#include <stdlib.h>
#include <string.h>

#include <Application.h>
#include <Screen.h>
#include <View.h>

#define UNIMPLEMENTED printf("%s (%s %d) UNIMPLEMENTED\n",__func__,__FILE__,__LINE__);

#include "iup.h"

#include "iup_str.h"
#include "iup_drv.h"
#include "iup_drvinfo.h"
#include "iup_object.h"
#include "iup_globalattrib.h"

#include "iuphaiku_drv.h"

char* iupgtkGetNativeWindowHandle(Ihandle* ih)
{
  BView* view = (BView*)ih->handle;
  return (char*)view->Window();
}

void* iupdrvGetDisplay(void)
{
  return new BScreen(); // TODO Does this leak memory ?
}

void iupgtkPushVisualAndColormap(void* visual, void* colormap)
{
	UNIMPLEMENTED
}

extern void iuphaikuTimerFired(Ihandle* ih);

class IUPApplication: public BApplication
{
	public:
		IUPApplication()
			: BApplication("application/iup")
		{
		}

	  	void MessageReceived(BMessage* message)
		{
			Ihandle* ih = NULL;
			message->FindPointer("iHandle", (void**)&ih);

			switch(message->what) {
	  			case timerFire:
					iuphaikuTimerFired(ih);
					break;
				default:
					BApplication::MessageReceived(message);
					break;
			}
		}
};


static void haikuSetGlobalAttrib(void)
{
	// XDISPLAY, XSCREEN, XSERVERVENDOR, XVENDORRELEASE
}


void haikuSetGlobalColors(void)
{
	rgb_color color;
	color = ui_color(B_PANEL_BACKGROUND_COLOR); // or B_CONTROL_BACKGROUND_COLOR ?
	iupGlobalSetDefaultColorAttrib("DLGBGCOLOR", color.red, color.green, color.blue);
	color = ui_color(B_PANEL_TEXT_COLOR); // or B_CONTROL_TEXT_COLOR ?
	iupGlobalSetDefaultColorAttrib("DLFBGCOLOR", color.red, color.green, color.blue);
	color = ui_color(B_DOCUMENT_BACKGROUND_COLOR);
	iupGlobalSetDefaultColorAttrib("TXTBGCOLOR", color.red, color.green, color.blue);
	color = ui_color(B_DOCUMENT_TEXT_COLOR);
	iupGlobalSetDefaultColorAttrib("TXTFGCOLOR", color.red, color.green, color.blue);
}


int iupdrvOpen(int *argc, char ***argv)
{
  IupSetGlobal("DRIVER", "HAIKU");
  //IupStoreGlobal("SYSTEMLANGUAGE", pango_language_to_string(gtk_get_default_language()));
  //HAIKUVERSION ? (does that make any sense ?)
  
  haikuSetGlobalAttrib();

  haikuSetGlobalColors();

  // Here, create our BApplication.
  new IUPApplication();

  return IUP_NOERROR;
}

void iupdrvClose(void)
{
  // Here, destroy our BApplication.
  if (be_app)
  {
  	be_app->Lock();
  	be_app->Quit();
  }
}
