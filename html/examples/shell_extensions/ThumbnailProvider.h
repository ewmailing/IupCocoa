/****************************** Module Header ******************************\
Module Name:  ThumbnailProvider.h
Copyright (c) Microsoft Corporation.

The code sample demonstrates the C++ implementation of a thumbnail handler. 

A thumbnail image handler provides an image to represent the item. It lets 
you customize the thumbnail of files with a specific file extension. Windows 
Vista and newer operating systems make greater use of file-specific thumbnail 
images than earlier versions of Windows. Thumbnails of 32-bit resolution and 
as large as 256x256 pixels are often used. File format owners should be 
prepared to display their thumbnails at that size. 

This source is subject to the Microsoft Public License.
See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
All other rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#pragma once

#include <windows.h>
#include <thumbcache.h>


class ThumbnailProvider : 
  public IInitializeWithFile, 
  public IThumbnailProvider
{
public:
  // IUnknown
  IFACEMETHODIMP QueryInterface(REFIID riid, void **ppv);
  IFACEMETHODIMP_(ULONG) AddRef();
  IFACEMETHODIMP_(ULONG) Release();

  // IInitializeWithFile
  IFACEMETHODIMP Initialize(LPCWSTR pszFilePath, DWORD grfMode);

  // IThumbnailProvider
  IFACEMETHODIMP GetThumbnail(UINT cx, HBITMAP *phbmp, WTS_ALPHATYPE *pdwAlpha);

  ThumbnailProvider();

protected:
  ~ThumbnailProvider();

private:
  // Reference count of component.
  long m_cRef;

  // Provided during initialization.
  LPWSTR m_pPathFile;
};

HBITMAP CreateThumbnail(const char* filename, UINT thumb_size, BOOL &has_alpha);
