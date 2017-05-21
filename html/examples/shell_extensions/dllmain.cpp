/****************************** Module Header ******************************\
Copyright (c) Microsoft Corporation.

The file implements DllMain, and the DllGetClassObject, DllCanUnloadNow, 
DllRegisterServer, DllUnregisterServer functions that are necessary for a COM 
DLL. 

DllGetClassObject invokes the class factory defined in ClassFactory.h/cpp and 
queries to the specific interface.

DllCanUnloadNow checks if we can unload the component from the memory.

DllRegisterServer registers the COM server and the preview handler in the 
registry by invoking the helper functions defined in Reg.h/cpp. The preview 
handler is associated with the .cgm file class.

DllUnregisterServer unregisters the COM server and the preview handler. 

This source is subject to the Microsoft Public License.
See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
All other rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#include <windows.h>
#include <Guiddef.h>
#include <shlobj.h>
#include <stdio.h>

#include "ClassFactory.h"
#include "Reg.h"

#include "Config.h"

#if defined(DEFINE_PREVIEWHANDLER)
#include <iup.h>
#endif

HINSTANCE  g_hInstDll = NULL;
long  g_cRefDll   = 0;


BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
  switch (dwReason)
  {
  case DLL_PROCESS_ATTACH:
    // Hold the instance of this DLL module, we will use it to get the 
    // path of the DLL to register the component.
    g_hInstDll = hModule;

#ifdef XX_DEBUG
    AllocConsole();
    AttachConsole(GetCurrentProcessId());
    freopen("CON", "w", stdout);
#endif

#if defined(DEFINE_PREVIEWHANDLER)
    IupOpen(NULL, NULL);
#endif

    // disable the DLL_THREAD_ATTACH and DLL_THREAD_DETACH notification calls. 
    // This can be a useful optimization for multi-threaded applications 
    // that have many DLLs, frequently create and delete threads, 
    // and whose DLLs do not need these thread-level notifications of attachment/detachment.
    DisableThreadLibraryCalls(hModule);
    break;
  case DLL_PROCESS_DETACH:
#if defined(DEFINE_PREVIEWHANDLER)
    IupClose();
#endif
    break;
  }
  return TRUE;
}


//
//   FUNCTION: DllGetClassObject
//
//   PURPOSE: Create the class factory and query to the specific interface.
//
//   PARAMETERS:
//   * rclsid - The CLSID that will associate the correct data and code.
//   * riid - A reference to the identifier of the interface that the caller 
//     is to use to communicate with the class object.
//   * ppv - The address of a pointer variable that receives the interface 
//     pointer requested in riid. Upon successful return, *ppv contains the 
//     requested interface pointer. If an error occurs, the interface pointer 
//     is NULL. 
//
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, void **ppv)
{
  if (ppv == NULL)
    return E_INVALIDARG;
    
  if (!IsEqualCLSID(CLSID_ShelExtensionHandler, rclsid))
    return CLASS_E_CLASSNOTAVAILABLE;
    
  ClassFactory *pClassFactory = new ClassFactory();
  if (!pClassFactory)
    return E_OUTOFMEMORY;
    
  HRESULT hr = pClassFactory->QueryInterface(riid, ppv);
  pClassFactory->Release();

  return hr;
}


//
//   FUNCTION: DllCanUnloadNow
//
//   PURPOSE: Check if we can unload the component from the memory.
//
//   NOTE: The component can be unloaded from the memory when its reference 
//   count is zero (i.e. nobody is still using the component).
// 
STDAPI DllCanUnloadNow(void)
{
  return g_cRefDll > 0 ? S_FALSE : S_OK;
}


//
//   FUNCTION: DllRegisterServer
//
//   PURPOSE: Register the COM server and the shell extension handler.
// 
STDAPI DllRegisterServer(void)
{
  HRESULT hr;

  wchar_t szModule[MAX_PATH];
  if (GetModuleFileName(g_hInstDll, szModule, ARRAYSIZE(szModule)) == 0)
  {
    hr = HRESULT_FROM_WIN32(GetLastError());
    return hr;
  }

  // Register the component.
  hr = RegisterInprocServer(szModule, CLSID_ShelExtensionHandler, 
                            APPICATION_DESCRIPTION L"." HANDLER_DESCRIPTION L" Class", 
                            L"Apartment", APPID_ShelExtensionHandler);
  if (SUCCEEDED(hr))
  {
#if defined(DEFINE_PREVIEWHANDLER)
    // Register the shell extension handler.
    hr = RegisterShellExtPreviewHandler(FORMAT_EXTENSION, CLSID_ShelExtensionHandler, HANDLER_DESCRIPTION);
#elif defined( DEFINE_THUMBNAILHANDLER)
    hr = RegisterShellExtThumbnailHandler(FORMAT_EXTENSION, CLSID_ShelExtensionHandler);
    if (SUCCEEDED(hr))
    {
      // This tells the shell to invalidate the thumbnail cache. It is 
      // important because any files of this format viewed before registering 
      // this handler would otherwise show cached blank thumbnails.
      SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, NULL, NULL);
    }
#endif
  }

  return hr;
}


//
//   FUNCTION: DllUnregisterServer
//
//   PURPOSE: Unregister the COM server and the shell extension handler.
// 
STDAPI DllUnregisterServer(void)
{
  // Unregister the component.
  HRESULT hr = UnregisterInprocServer(CLSID_ShelExtensionHandler, APPID_ShelExtensionHandler);
  if (SUCCEEDED(hr))
  {
    // Unregister the shell extension handler.
#if defined(DEFINE_PREVIEWHANDLER)
    hr = UnregisterShellExtPreviewHandler(FORMAT_EXTENSION, CLSID_ShelExtensionHandler);
#elif defined(DEFINE_THUMBNAILHANDLER)
    hr = UnregisterShellExtThumbnailHandler(FORMAT_EXTENSION);
#endif
  }

  return hr;
}
