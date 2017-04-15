/******************************** Module Header ********************************\
Module Name:  ThumbnailProvider.cpp
Copyright (c) Microsoft Corporation.

The code sample demonstrates the C++ implementation of a thumbnail handler 
for a new file type registered with the given extension. 

A thumbnail image handler provides an image to represent the item. It lets you 
customize the thumbnail of files with a specific file extension. Windows Vista 
and newer operating systems make greater use of file-specific thumbnail images 
than earlier versions of Windows. Thumbnails of 32-bit resolution and as large 
as 256x256 pixels are often used. File format owners should be prepared to 
display their thumbnails at that size. 

This source is subject to the Microsoft Public License.
See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
All other rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\*******************************************************************************/

#include "ThumbnailProvider.h"

#include <Shlwapi.h>
#include <stdio.h>


extern HINSTANCE g_hInstDll;
extern long      g_cRefDll;


ThumbnailProvider::ThumbnailProvider()
  : m_cRef(1), m_pPathFile(NULL)
{
  InterlockedIncrement(&g_cRefDll);
}


ThumbnailProvider::~ThumbnailProvider()
{
  if (m_pPathFile)
  {
    LocalFree(m_pPathFile);
    m_pPathFile = NULL;
  }

  InterlockedDecrement(&g_cRefDll);
}


#pragma region IUnknown

// Query to the interface the component supported.
IFACEMETHODIMP ThumbnailProvider::QueryInterface(REFIID riid, void **ppv)
{
  static const QITAB qit[] =
  {
    QITABENT(ThumbnailProvider, IThumbnailProvider),
    QITABENT(ThumbnailProvider, IInitializeWithFile),
    { 0 },
  };
  return QISearch(this, qit, riid, ppv);
}

// Increase the reference count for an interface on an object.
IFACEMETHODIMP_(ULONG) ThumbnailProvider::AddRef()
{
  return InterlockedIncrement(&m_cRef);
}

// Decrease the reference count for an interface on an object.
IFACEMETHODIMP_(ULONG) ThumbnailProvider::Release()
{
  ULONG cRef = InterlockedDecrement(&m_cRef);
  if (0 == cRef)
  {
    delete this;
  }

  return cRef;
}

#pragma endregion


#pragma region IInitializeWithFile

IFACEMETHODIMP ThumbnailProvider::Initialize(LPCWSTR pszFilePath, DWORD grfMode)
{
  HRESULT hr = E_INVALIDARG;

  if (pszFilePath)
  {
    // Initialize can be called more than once, so release existing valid 
    // m_pStream.
    if (m_pPathFile)
    {
      LocalFree(m_pPathFile);
      m_pPathFile = NULL;
    }

    m_pPathFile = StrDup(pszFilePath);
    hr = S_OK;
  }
  return hr;
}

#pragma endregion


#pragma region IThumbnailProvider

// Gets a thumbnail image and alpha type. The GetThumbnail is called with the 
// largest desired size of the image, in pixels. Although the parameter is 
// called cx, this is used as the maximum size of both the x and y dimensions. 
// If the retrieved thumbnail is not square, then the longer axis is limited 
// by cx and the aspect ratio of the original image respected. On exit, 
// GetThumbnail provides a handle to the retrieved image. It also provides a 
// value that indicates the color format of the image and whether it has 
// valid alpha information.
IFACEMETHODIMP ThumbnailProvider::GetThumbnail(UINT thumb_size, HBITMAP *phbmp, WTS_ALPHATYPE *pdwAlpha)
{
  char filename[10240];
  size_t size;
  wcstombs_s(&size, filename, 10240, m_pPathFile, 10240);

  BOOL has_alpha;
  *phbmp = CreateThumbnail(filename, thumb_size, has_alpha);
  if (!(*phbmp))
    return E_FAIL;

  if (has_alpha)
    *pdwAlpha = WTSAT_ARGB;
  else
    *pdwAlpha = WTSAT_RGB;

  return S_OK;
}

#pragma endregion
