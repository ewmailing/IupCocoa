/** \file
* \brief Web Browser Control
*
* http://msdn.microsoft.com/en-us/library/aa752040(v=vs.85).aspx
*
* See Copyright Notice in "iup.h"
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <windows.h>
#include <exdisp.h>

#include "iup.h"
#include "iupcbs.h"
#include "iupole.h"

#include "iup_object.h"
#include "iup_register.h"
#include "iup_attrib.h"
#include "iup_stdcontrols.h"
#include "iup_str.h"
#include "iup_layout.h"
#include "iup_webbrowser.h"
#include "iup_drv.h"
#include "iup_drvfont.h"

/* Exported from "iupwin_str.c" */
extern "C" WCHAR* iupwinStrChar2Wide(const char* str);
extern "C" char*  iupwinStrWide2Char(const WCHAR* wstr);

#include <atlbase.h>
#include <atlcom.h>
#include <atlctl.h>

#include <shlguid.h>   /* IID_IWebBrowser2, DIID_DWebBrowserEvents2 */
#include <exdispid.h>  /* DISPID_*   */

using namespace ATL;

// Should have only one instance of a class
// derived from CAtlModule in a project.
static CComModule* iweb_module = NULL;

#if _MSC_VER < 1500  // VC9: VC8 does not defines this
#define OLECMDID_OPTICAL_ZOOM (OLECMDID)63
#endif

interface CSink:public IDispEventImpl<0, CSink, &DIID_DWebBrowserEvents2, &LIBID_SHDocVw, 1, 0>
{
public:
  Ihandle* ih;

  BEGIN_SINK_MAP(CSink)
    SINK_ENTRY_EX(0, DIID_DWebBrowserEvents2, DISPID_BEFORENAVIGATE2, BeforeNavigate2)
#ifdef DISPID_NEWWINDOW3
    SINK_ENTRY_EX(0, DIID_DWebBrowserEvents2, DISPID_NEWWINDOW3, NewWindow3)
#endif
    SINK_ENTRY_EX(0, DIID_DWebBrowserEvents2, DISPID_NAVIGATEERROR, NavigateError)
    SINK_ENTRY_EX(0, DIID_DWebBrowserEvents2, DISPID_DOCUMENTCOMPLETE, DocumentComplete)
    SINK_ENTRY_EX(0, DIID_DWebBrowserEvents2, DISPID_COMMANDSTATECHANGE, CommandStateChange)
    
  END_SINK_MAP()

  void STDMETHODCALLTYPE BeforeNavigate2(IDispatch *pDisp, VARIANT *url, VARIANT *Flags, VARIANT *TargetFrameName,
                                         VARIANT *PostData, VARIANT *Headers, VARIANT_BOOL *Cancel)
  {
    if (iupAttribGet(ih, "_IUPWEB_IGNORE_NAVIGATE"))
      return;

    IFns cb = (IFns)IupGetCallback(ih, "NAVIGATE_CB");
    if (cb)
    {
      char* urlString = iupwinStrWide2Char(url->bstrVal);
      int ret = cb(ih, urlString);
      free(urlString);
      if (ret == IUP_IGNORE)
        *Cancel = VARIANT_TRUE;
    }

    (void)Cancel;
    (void)Headers;
    (void)PostData;
    (void)TargetFrameName;
    (void)Flags;
    (void)pDisp;
  }

#ifdef DISPID_NEWWINDOW3
  void STDMETHODCALLTYPE NewWindow3(IDispatch **ppDisp, VARIANT_BOOL *Cancel, DWORD dwFlags,
                                    BSTR bstrUrlContext, BSTR bstrUrl)
  {
    IFns cb = (IFns)IupGetCallback(ih, "NEWWINDOW_CB");
    if (cb)
    {
      char* urlString = iupwinStrWide2Char(bstrUrl);
      cb(ih, urlString);
      free(urlString);
    }

    (void)bstrUrlContext;
    (void)dwFlags;
    (void)Cancel;
    (void)ppDisp;
  }
#endif

  void STDMETHODCALLTYPE NavigateError(IDispatch *pDisp, VARIANT *url, VARIANT *TargetFrameName, 
                     VARIANT *StatusCode, VARIANT_BOOL *Cancel)
  {
    iupAttribSet(ih, "_IUPWEB_FAILED", "1");
    IFns cb = (IFns)IupGetCallback(ih, "ERROR_CB");
    if (cb)
    {
      char* urlString = iupwinStrWide2Char(url->bstrVal);
      cb(ih, urlString);
      free(urlString);
    }

    (void)TargetFrameName;
    (void)StatusCode;
    (void)Cancel;
    (void)pDisp;
  }

  void STDMETHODCALLTYPE DocumentComplete(IDispatch *pDisp, VARIANT *url)
  {
    IFns cb = (IFns)IupGetCallback(ih, "COMPLETED_CB");
    if (cb)
    {
      char* urlString = iupwinStrWide2Char(url->bstrVal);
      cb(ih, urlString);
      free(urlString);
    }
    (void)pDisp;
  }

  void STDMETHODCALLTYPE CommandStateChange(LONG Command, VARIANT_BOOL Enable)
  {
    if (Command == CSC_NAVIGATEFORWARD)
    {
      if (Enable == VARIANT_TRUE)
        iupAttribSet(ih, "CANGOFORWARD", "YES");
      else
        iupAttribSet(ih, "CANGOFORWARD", "NO");
    }
    else if (Command == CSC_NAVIGATEBACK)
    {
      if (Enable == VARIANT_TRUE)
        iupAttribSet(ih, "CANGOBACK", "YES");
      else
        iupAttribSet(ih, "CANGOBACK", "NO");
    }
  }
};

static void VariantBStr(VARIANT *var, WCHAR* wstr)
{
  VariantInit(var);
  var->vt = VT_BSTR;
  var->bstrVal = wstr;
}

static void VariantLong(VARIANT *var, LONG val)
{
  VariantInit(var);
  var->vt = VT_I4;
  var->lVal = val;
}

static SAFEARRAY* VariantSafeArray(const WCHAR* wstr)
{
  VARIANT *param;
  SAFEARRAY *sfArray;
  sfArray = SafeArrayCreateVector(VT_VARIANT, 0, 1);    // must call SafeArrayDestroy
  SafeArrayAccessData(sfArray,(LPVOID*) &param);
  param->vt = VT_BSTR;
  param->bstrVal = SysAllocString(wstr);
  SafeArrayUnaccessData(sfArray);
  return sfArray;
}

/********************************************************************************/

#if 0
static int winWebBrowserSetHTMLAttrib(Ihandle* ih, const char* value)
{
  if (!value)
    return 0;

  IWebBrowser2 *pweb = (IWebBrowser2*)iupAttribGet(ih, "_IUPWEB_BROWSER");
  int size = strlen(value)+1;

  /* Create the memory for the stream */
  HGLOBAL hHTMLText = GlobalAlloc(GPTR, size);
  strcpy((char*)hHTMLText, value); /* weird but we follow the tutorial on MSDN */

  /* Create the stream  */
  IStream* pStream = NULL;
  CreateStreamOnHGlobal(hHTMLText, FALSE, &pStream);

  /* Retrieve the document object. */
  IDispatch* lpDispatch = NULL;
  pweb->get_Document(&lpDispatch);
  if (!lpDispatch)
  {
    iupAttribSet(ih, "_IUPWEB_FAILED", NULL);

    pweb->Navigate(L"about:blank", NULL, NULL, NULL, NULL);
    IupFlush();

    pweb->get_Document(&lpDispatch);
  }

  IPersistStreamInit* pPersistStreamInit = NULL;
  lpDispatch->QueryInterface(IID_IPersistStreamInit, (void**)&pPersistStreamInit);

  /* Initialize the document. */
  pPersistStreamInit->InitNew();

  /* Load the contents of the stream.  */
  pPersistStreamInit->Load(pStream);

  /* Releases */
  pPersistStreamInit->Release();
  pStream->Release();
  lpDispatch->Release();
  GlobalFree(hHTMLText);
  
  return 0; /* do not store value in hash table */
}

#else

static int winWebBrowserSetHTMLAttrib(Ihandle* ih, const char* value)
{
  if (!value)
    return 0;

  IWebBrowser2 *pweb = (IWebBrowser2*)iupAttribGet(ih, "_IUPWEB_BROWSER");

  /* Retrieve the document object. */
  IDispatch* lpDispatch = NULL;
  pweb->get_Document(&lpDispatch);
  if (!lpDispatch)
  {
    iupAttribSet(ih, "_IUPWEB_FAILED", NULL);
    iupAttribSet(ih, "_IUPWEB_IGNORE_NAVIGATE", "1");
    pweb->Navigate(L"about:blank", NULL, NULL, NULL, NULL);
    IupFlush();
    iupAttribSet(ih, "_IUPWEB_IGNORE_NAVIGATE", NULL);

    pweb->get_Document(&lpDispatch);
  }

	IHTMLDocument2 *htmlDoc2;
  lpDispatch->QueryInterface(IID_IHTMLDocument2, (void**)&htmlDoc2);

  WCHAR* wvalue = iupwinStrChar2Wide(value);

  SAFEARRAY *sfArray = VariantSafeArray(wvalue);

	htmlDoc2->write(sfArray);
	htmlDoc2->close();

  /* Releases */
  SafeArrayDestroy(sfArray);
  htmlDoc2->Release();
  lpDispatch->Release();
  free(wvalue);
  
  return 0; /* do not store value in hash table */
}
#endif

static int winWebBrowserSetCopyAttrib(Ihandle* ih, const char* value)
{
  IWebBrowser2 *pweb = (IWebBrowser2*)iupAttribGet(ih, "_IUPWEB_BROWSER");
  pweb->ExecWB(OLECMDID_COPY, OLECMDEXECOPT_DONTPROMPTUSER, NULL, NULL);
  (void)value;
  return 0;
}

static int winWebBrowserSetSelectAllAttrib(Ihandle* ih, const char* value)
{
  IWebBrowser2 *pweb = (IWebBrowser2*)iupAttribGet(ih, "_IUPWEB_BROWSER");
  pweb->ExecWB(OLECMDID_SELECTALL, OLECMDEXECOPT_DONTPROMPTUSER, NULL, NULL);
  (void)value;
  return 0;
}

static int winWebBrowserSetPrintAttrib(Ihandle* ih, const char* value)
{
  IWebBrowser2 *pweb = (IWebBrowser2*)iupAttribGet(ih, "_IUPWEB_BROWSER");
  pweb->ExecWB(OLECMDID_PRINT2, OLECMDEXECOPT_PROMPTUSER, NULL, NULL);
  (void)value;
  return 0;
}

static int winWebBrowserSetZoomAttrib(Ihandle* ih, const char* value)
{
  int zoom;
  if (iupStrToInt(value, &zoom))
  {
    IWebBrowser2 *pweb = (IWebBrowser2*)iupAttribGet(ih, "_IUPWEB_BROWSER");

    VARIANT var;
    VariantLong(&var, (LONG)zoom);

    // OLECMDID_OPTICAL_ZOOM = VT_I4 (LONG) parameter in the range of 10 to 1000 (percent).
    pweb->ExecWB(OLECMDID_OPTICAL_ZOOM, OLECMDEXECOPT_DONTPROMPTUSER, &var, NULL);
  }
  return 0;
}

static char* winWebBrowserGetZoomAttrib(Ihandle* ih)
{
  IWebBrowser2 *pweb = (IWebBrowser2*)iupAttribGet(ih, "_IUPWEB_BROWSER");

  VARIANT var;
  VariantLong(&var, 0);

  pweb->ExecWB(OLECMDID_OPTICAL_ZOOM, OLECMDEXECOPT_DONTPROMPTUSER, NULL, &var);

  return iupStrReturnInt((int)var.lVal);
}

static IHTMLElement* winWebBrowserFindElement(Ihandle* ih, const char* element_id)
{
  HRESULT hr;
  IDispatch* pHtmlDocDispatch = NULL;
  IHTMLDocument2 * pHtmlDoc = NULL;
  IWebBrowser2 *m_pWebBrowser = (IWebBrowser2*)iupAttribGet(ih, "_IUPWEB_BROWSER");

  // Retrieve the document object.
  hr = m_pWebBrowser->get_Document(&pHtmlDocDispatch);
  if (SUCCEEDED(hr) && (pHtmlDocDispatch != NULL))
  {
    hr = pHtmlDocDispatch->QueryInterface(IID_IHTMLDocument2, (void**)&pHtmlDoc);
    if (SUCCEEDED(hr) && (pHtmlDoc != NULL))
    {
      IHTMLElementCollection* pColl = NULL;
      hr = pHtmlDoc->get_all(&pColl);

      if (SUCCEEDED(hr) && (pColl != NULL))
      {
        // Obtained the Anchor Collection...
        long nLength = 0;
        pColl->get_length(&nLength);

        for (int i = 0; i < nLength; i++)
        {
          VARIANT vIdx;
          VariantLong(&vIdx, (LONG)i);

          IDispatch* pElemDispatch = NULL;
          IHTMLElement * pElem = NULL;

          hr = pColl->item(vIdx, vIdx, &pElemDispatch);

          if (SUCCEEDED(hr) && (pElemDispatch != NULL))
          {
            hr = pElemDispatch->QueryInterface(IID_IHTMLElement, (void**)&pElem);

            if (SUCCEEDED(hr) && (pElem != NULL))
            {
              BSTR bstrId;
              if (!FAILED(pElem->get_id(&bstrId)))
              {
                char* str = iupwinStrWide2Char(bstrId);
                SysFreeString(bstrId);

                if (iupStrEqual(str, element_id))
                {
                  free(str);
                  pElemDispatch->Release();
                  pColl->Release();
                  pHtmlDoc->Release();
                  pHtmlDocDispatch->Release();

                  return pElem;
                }

                free(str);
              }

              pElem->Release();
            }
            pElemDispatch->Release();
          }
        }
        pColl->Release();
      }
      pHtmlDoc->Release();
    }
    pHtmlDocDispatch->Release();
  }

  return NULL;
}

static int winWebBrowserSetInnerTextAttrib(Ihandle* ih, const char* value)
{
  if (value)
  {
    char* element_id = iupAttribGet(ih, "ELEMENT_ID");
    if (element_id)
    {
      IHTMLElement* pElem = winWebBrowserFindElement(ih, element_id);
      if (pElem)
      {
        WCHAR* wvalue = iupwinStrChar2Wide(value);
        pElem->put_innerText(wvalue);
        free(wvalue);
        pElem->Release();
      }
    }
  }
  return 0;
}

static char* winWebBrowserGetInnerTextAttrib(Ihandle* ih)
{
  char* element_id = iupAttribGet(ih, "ELEMENT_ID");
  if (element_id)
  {
    IHTMLElement* pElem = winWebBrowserFindElement(ih, element_id);
    if (pElem)
    {
      WCHAR* wvalue = NULL;
      if (!FAILED(pElem->get_innerText(&wvalue)))
      {
        char* str = iupwinStrWide2Char(wvalue);
        char* value = iupStrReturnStr(str);
        SysFreeString(wvalue);
        free(str);
        pElem->Release();
        return value;
      }
      pElem->Release();
    }
  }

  return NULL;
}

static int winWebBrowserSetAttributeAttrib(Ihandle* ih, const char* value)
{
  if (value)
  {
    char* element_id = iupAttribGet(ih, "ELEMENT_ID");
    char* attribute_name = iupAttribGet(ih, "ATTRIBUTE_NAME");
    if (element_id && attribute_name)
    {
      IHTMLElement* pElem = winWebBrowserFindElement(ih, element_id);
      if (pElem)
      {
        WCHAR* wname = iupwinStrChar2Wide(attribute_name);
        WCHAR* wvalue = iupwinStrChar2Wide(value);

        VARIANT var;
        VariantBStr(&var, wvalue);

        pElem->setAttribute(wname, var, 1);  // case sensitive search

        free(wvalue);
        free(wname);
        pElem->Release();
      }
    }
  }
  return 0;
}

static char* winWebBrowserGetAttributeAttrib(Ihandle* ih)
{
  char* element_id = iupAttribGet(ih, "ELEMENT_ID");
  char* attribute_name = iupAttribGet(ih, "ATTRIBUTE_NAME");
  if (element_id && attribute_name)
  {
    IHTMLElement* pElem = winWebBrowserFindElement(ih, element_id);
    if (pElem)
    {
      WCHAR* wname = iupwinStrChar2Wide(attribute_name);
      VARIANT var;
      VariantInit(&var);
      if (!FAILED(pElem->getAttribute(wname, 1, &var)) && var.bstrVal)  // case sensitive search
      {
        char* str = iupwinStrWide2Char(var.bstrVal);
        char* value = iupStrReturnStr(str);
        free(str);
        free(wname);
        pElem->Release();
        return value;
      }
      free(wname);
      pElem->Release();
    }
  }

  return NULL;
}

static int winWebBrowserSetBackForwardAttrib(Ihandle* ih, const char* value)
{
  int i, val;
  if (iupStrToInt(value, &val))
  {
    IWebBrowser2 *pweb = (IWebBrowser2*)iupAttribGet(ih, "_IUPWEB_BROWSER");

    /* Negative values represent steps backward while positive values represent steps forward. */
    if(val > 0)
    {
      for(i = 0; i < val; i++)
        pweb->GoForward();
    }
    else if(val < 0)
    {
      for(i = 0; i < -(val); i++)
        pweb->GoBack();
    }
  }

  return 0; /* do not store value in hash table */
}

static int winWebBrowserSetGoBackAttrib(Ihandle* ih, const char* value)
{
  IWebBrowser2 *pweb = (IWebBrowser2*)iupAttribGet(ih, "_IUPWEB_BROWSER");
  pweb->GoBack();
  (void)value;
  return 0; /* do not store value in hash table */
}

static int winWebBrowserSetGoForwardAttrib(Ihandle* ih, const char* value)
{
  IWebBrowser2 *pweb = (IWebBrowser2*)iupAttribGet(ih, "_IUPWEB_BROWSER");
  pweb->GoForward();
  (void)value;
  return 0; /* do not store value in hash table */
}

static int winWebBrowserSetReloadAttrib(Ihandle* ih, const char* value)
{
  IWebBrowser2 *pweb = (IWebBrowser2*)iupAttribGet(ih, "_IUPWEB_BROWSER");
  pweb->Refresh();

  (void)value;
  return 0; /* do not store value in hash table */
}

static int winWebBrowserSetStopAttrib(Ihandle* ih, const char* value)
{
  IWebBrowser2 *pweb = (IWebBrowser2*)iupAttribGet(ih, "_IUPWEB_BROWSER");
  pweb->Stop();

  (void)value;
  return 0; /* do not store value in hash table */
}

static char* winWebBrowserGetStatusAttrib(Ihandle* ih)
{
  IWebBrowser2 *pweb = (IWebBrowser2*)iupAttribGet(ih, "_IUPWEB_BROWSER");
  READYSTATE plReadyState;
  pweb->get_ReadyState(&plReadyState);
  if (iupAttribGet(ih, "_IUPWEB_FAILED"))
    return "FAILED";
  else if (plReadyState == READYSTATE_COMPLETE)
    return "COMPLETED";
  else
    return "LOADING";
}

static int winWebBrowserSetValueAttrib(Ihandle* ih, const char* value)
{
  if (value)
  {
    IWebBrowser2 *pweb = (IWebBrowser2*)iupAttribGet(ih, "_IUPWEB_BROWSER");
    WCHAR* wvalue = iupwinStrChar2Wide(value);

    VARIANT var;
    VariantInit(&var);  /* Initialize our variant */
    var.vt = VT_ARRAY | VT_UI1;
    var.bstrVal = L"_top";

    iupAttribSet(ih, "_IUPWEB_FAILED", NULL);

    pweb->Navigate(wvalue, NULL, &var, NULL, NULL);
    free(wvalue);
  }
  return 0;
}

static char* winWebBrowserGetValueAttrib(Ihandle* ih)
{
  IWebBrowser2 *pweb = (IWebBrowser2*)iupAttribGet(ih, "_IUPWEB_BROWSER");
  BSTR pbstrLocationURL = NULL;
  if (pweb->get_LocationURL(&pbstrLocationURL)==S_OK && pbstrLocationURL)
  {
    char* str = iupwinStrWide2Char(pbstrLocationURL);
    SysFreeString(pbstrLocationURL);
    char* value = iupStrReturnStr(str);
    free(str);
    return value;
  }
  return NULL;
}

static int winWebBrowserCreateMethod(Ihandle* ih, void **params)
{
  (void)params;
  IupSetAttribute(ih, "PROGID", "Shell.Explorer.2");
  IupSetAttribute(ih, "DESIGNMODE", "NO");

  /* Get the current IUnknown* */
  IUnknown *punk = (IUnknown*)IupGetAttribute(ih, "IUNKNOWN");

  IWebBrowser2 *pweb = NULL;
  punk->QueryInterface(IID_IWebBrowser2, (void **)&pweb);
  iupAttribSet(ih, "_IUPWEB_BROWSER", (char*)pweb);

  /* CSink object to capture events */
  CSink* sink = new CSink();

  /* Set handle to use in CSink Interface */
  sink->ih = ih;

  /* Connecting to the server's outgoing interface */
  sink->DispEventAdvise(punk);

  iupAttribSet(ih, "_IUPWEB_SINK", (char*)sink);
  punk->Release();

  return IUP_NOERROR; 
}

static void winWebBrowserDestroyMethod(Ihandle* ih)
{
  IWebBrowser2 *pweb = (IWebBrowser2*)iupAttribGet(ih, "_IUPWEB_BROWSER");
  pweb->Release();

  CSink* sink = (CSink*)iupAttribGet(ih, "_IUPWEB_SINK");

  /* Get the current IUnknown* */
  IUnknown *punk = (IUnknown*)IupGetAttribute(ih, "IUNKNOWN");

  /* Disconnecting from the server's outgoing interface */
  sink->DispEventUnadvise(punk);
  delete sink;

  punk->Release();
}

static void winWebBrowserRelease(Iclass* ic)
{
  /* Terminating ATL support */
  if (iweb_module)
  {
    iweb_module->Term();
    delete iweb_module;
    iweb_module = NULL;
  }

  (void)ic;
}

Iclass* iupWebBrowserNewClass(void)
{
  Iclass* ic = iupClassNew(iupRegisterFindClass("olecontrol"));

  ic->name = "webbrowser";
  ic->format = NULL; /* no parameters */
  ic->nativetype = IUP_TYPECANVAS;
  ic->childtype  = IUP_CHILDNONE;
  ic->is_interactive = 1;

  /* Class functions */
  ic->New = iupWebBrowserNewClass;
  ic->Create = winWebBrowserCreateMethod;
  ic->Destroy = winWebBrowserDestroyMethod;
  ic->Release = winWebBrowserRelease;

  /* Callbacks */
  iupClassRegisterCallback(ic, "NEWWINDOW_CB", "s");
  iupClassRegisterCallback(ic, "NAVIGATE_CB", "s");
  iupClassRegisterCallback(ic, "ERROR_CB", "s");

  /* Attributes */
  iupClassRegisterAttribute(ic, "VALUE", winWebBrowserGetValueAttrib, winWebBrowserSetValueAttrib, NULL, NULL, IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "BACKFORWARD", NULL, winWebBrowserSetBackForwardAttrib, NULL, NULL, IUPAF_WRITEONLY | IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "GOBACK", NULL, winWebBrowserSetGoBackAttrib, NULL, NULL, IUPAF_WRITEONLY | IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "GOFORWARD", NULL, winWebBrowserSetGoForwardAttrib, NULL, NULL, IUPAF_WRITEONLY | IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "STOP", NULL, winWebBrowserSetStopAttrib, NULL, NULL, IUPAF_WRITEONLY | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "RELOAD", NULL, winWebBrowserSetReloadAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "HTML", NULL, winWebBrowserSetHTMLAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "STATUS", winWebBrowserGetStatusAttrib, NULL, NULL, NULL, IUPAF_NO_DEFAULTVALUE|IUPAF_READONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "COPY", NULL, winWebBrowserSetCopyAttrib, NULL, NULL, IUPAF_WRITEONLY | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SELECTALL", NULL, winWebBrowserSetSelectAllAttrib, NULL, NULL, IUPAF_WRITEONLY | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "ZOOM", winWebBrowserGetZoomAttrib, winWebBrowserSetZoomAttrib, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "PRINT", NULL, winWebBrowserSetPrintAttrib, NULL, NULL, IUPAF_WRITEONLY | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "CANGOBACK", NULL, NULL, NULL, NULL, IUPAF_READONLY | IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "CANGOFORWARD", NULL, NULL, NULL, NULL, IUPAF_READONLY | IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "ELEMENT_ID", NULL, NULL, NULL, NULL, IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "INNERTEXT", winWebBrowserGetInnerTextAttrib, winWebBrowserSetInnerTextAttrib, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "ATTRIBUTE_NAME", NULL, NULL, NULL, NULL, IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "ATTRIBUTE", winWebBrowserGetAttributeAttrib, winWebBrowserSetAttributeAttrib, NULL, NULL, IUPAF_NO_INHERIT);

  if (!iweb_module)
  {
    /* CComModule implements a COM server module,
       allowing a client to access the module's components  */
    iweb_module = new CComModule();

    /* Initializing ATL Support */
    iweb_module->Init(NULL, (HINSTANCE)IupGetGlobal("HINSTANCE"));
  }

  return ic;
}
