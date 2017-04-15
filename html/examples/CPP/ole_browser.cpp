/*
 * IupOleControl sample using C++
 */

#include <stdlib.h>
#include <stdio.h>

#include <windows.h>
#include <exdisp.h>

#include <iup_plus.h>


// Needed to use the WebBrowser OLE control if not using the <expdisp.h> header
//#import "progid:Shell.Explorer.2" no_namespace named_guids

static WCHAR* Char2Wide(char* str)
{
  if (str)
  {
    int n = (int)strlen(str)+1;
    WCHAR* wstr = (WCHAR*)malloc(n * sizeof(WCHAR));
    MultiByteToWideChar(CP_ACP, 0, str, -1, wstr, n);
    return wstr;
  }

  return NULL;
}

static int load_cb(Ihandle* ih)
{
  Iup::OleControl control(ih);
  Iup::Text txt((Ihandle*)control.GetAttribute("MY_TEXT"));
  IWebBrowser2 *pweb = (IWebBrowser2*)control.GetAttribute("MY_WEB");
  WCHAR* url = Char2Wide(txt.GetAttribute("VALUE"));

  // Uses the navigate method of the control
  pweb->Navigate(url, NULL, NULL, NULL, NULL);

  free(url);
  return IUP_DEFAULT;
}

int main(int argc, char **argv)
{
  Iup::Open(argc, argv);
  Iup::OleControl::Open();

  // Creates an instance of the WebBrowser control
  Iup::OleControl control("Shell.Explorer.2");

  // Sets production mode
  control.SetAttribute("DESIGNMODE", "NO");

  // Creates a dialog containing the OLE control
  Iup::Text txt;
  Iup::Button bt("Load");
  Iup::Dialog dlg(Iup::Vbox(Iup::Hbox(txt, bt), control));
  dlg.SetAttribute("TITLE", "IupOle");
  dlg.SetAttribute("MY_TEXT", (char*)txt.GetHandle());
  txt.SetAttribute("EXPAND", "HORIZONTAL");
  bt.SetCallback("ACTION", (Icallback)load_cb);

  // Maps the dialog to force the creation of the control
  dlg.Map();

  // Gathers the IUnknown pointer to access the control's interface

  IUnknown* punk = (IUnknown*)control.GetAttribute("IUNKNOWN");
  IWebBrowser2 *pweb = NULL;
  punk->QueryInterface(IID_IWebBrowser2, (void **)&pweb);
  punk->Release();
  dlg.SetAttribute("MY_WEB", (char*)pweb);

  // Shows dialog
  dlg.Show();

  Iup::MainLoop();

  // Releases the control interface
  pweb->Release();

  Iup::Close();

  return 0;
}
