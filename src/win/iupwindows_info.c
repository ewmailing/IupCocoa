/** \file
 * \brief Windows System Information
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdio.h>
#include <stdlib.h> 

/* This module should depend only on IUP core headers 
   and Windows system headers. */

#include <windows.h>

#include "iup_str.h"
#include "iup_drvinfo.h"
#include "iup_varg.h"


char* iupdrvLocaleInfo(void)
{
  CPINFOEXA info;
  GetCPInfoExA(CP_ACP, 0, &info);
  return iupStrReturnStr(info.CodePageName);
}

void iupdrvGetScreenSize(int *width, int *height)
{
  RECT area;
  SystemParametersInfoA(SPI_GETWORKAREA, 0, &area, 0);
  *width = (int)(area.right - area.left);
  *height =  (int)(area.bottom - area.top);
}

void iupdrvAddScreenOffset(int *x, int *y, int add)
{
  RECT area;
  SystemParametersInfoA(SPI_GETWORKAREA, 0, &area, 0);
  if (add==1)
  {
    if (x) *x += area.left;
    if (y) *y += area.top;
  }
  else
  {
    if (x) *x -= area.left;
    if (y) *y -= area.top;
  }
}

void iupdrvGetFullSize(int *width, int *height)
{
  RECT rect;
  GetWindowRect(GetDesktopWindow(), &rect);
  *width = rect.right - rect.left;
  *height = rect.bottom - rect.top;
}

int iupdrvGetScreenDepth(void)
{
  int bpp;
  HDC hDCDisplay = GetDC(NULL);
  bpp = GetDeviceCaps(hDCDisplay, BITSPIXEL);
  ReleaseDC(NULL, hDCDisplay);
  return bpp;
}

double iupdrvGetScreenDpi(void)
{
  double dpi;
  HDC hDCDisplay = GetDC(NULL);
  dpi = (double)GetDeviceCaps(hDCDisplay, LOGPIXELSY);
  ReleaseDC(NULL, hDCDisplay);
  return dpi;
}

void iupdrvGetCursorPos(int *x, int *y)
{
  POINT CursorPoint;
  GetCursorPos(&CursorPoint);
  *x = (int)CursorPoint.x;
  *y = (int)CursorPoint.y;

  iupdrvAddScreenOffset(x, y, -1);
}

void iupdrvGetKeyState(char* key)
{
  if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
    key[0] = 'S';
  else
    key[0] = ' ';
  if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
    key[1] = 'C';
  else
    key[1] = ' ';
  if (GetAsyncKeyState(VK_MENU) & 0x8000)
    key[2] = 'A';
  else
    key[2] = ' ';
  if ((GetAsyncKeyState(VK_LWIN) & 0x8000) || (GetAsyncKeyState(VK_RWIN) & 0x8000))
    key[3] = 'Y';
  else
    key[3] = ' ';
  
  key[4] = 0;
}

/* TODO: Since Windows 8.1/Visual Studio 2013 GetVersionEx is deprecated. 
         We can replace it using GetProductInfo. But for now leave it. */

char *iupdrvGetSystemName(void)
{
  OSVERSIONINFOA osvi;
  osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOA);
  GetVersionExA(&osvi);

  if (osvi.dwPlatformId == VER_PLATFORM_WIN32_NT)
  {
    if (osvi.dwMajorVersion <= 4)
      return "WinNT";

    if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0)
      return "Win2K";

    if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion > 0)
      return "WinXP";

    if (osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 0)
      return "Vista";

    if (osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 1)
      return "Win7";

    if (osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 2)
      return "Win8";

    /* IMPORTANT: starting here will work only if the Manifest has been changed 
       to include Windows 8+ support. Otherwise GetVersionEx will report 6.2 */

    if (osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 3)
      return "Win81";

    if (osvi.dwMajorVersion == 10 && osvi.dwMinorVersion == 0)
      return "Win10";
  }

  return "Windows";
}

char *iupdrvGetSystemVersion(void)
{
  char *str = iupStrGetMemory(256);
  OSVERSIONINFOEXA osvi;
  SYSTEM_INFO si;

  ZeroMemory(&si, sizeof(SYSTEM_INFO));
  GetSystemInfo(&si);

  ZeroMemory(&osvi, sizeof(OSVERSIONINFOEXA));
  osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEXA);
  GetVersionExA((OSVERSIONINFOA*)&osvi);

  sprintf(str, "%d.%d.%d", (int)osvi.dwMajorVersion, (int)osvi.dwMinorVersion, (int)osvi.dwBuildNumber);

  /* Display service pack (if any). */
  if (osvi.szCSDVersion && osvi.szCSDVersion[0]!=0)
  {
    strcat(str, " ");
    strcat(str, osvi.szCSDVersion);
  }

  if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64)
    strcat(str, " (IA64)");
  else if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
    strcat(str, " (x64)");
  else
    strcat(str, " (x86)");

  return str;
}

char *iupdrvGetComputerName(void)
{
  DWORD size = MAX_COMPUTERNAME_LENGTH + 1;
  char* str = iupStrGetMemory(size);
  GetComputerNameA((LPSTR)str, &size);
  return str;
}

char *iupdrvGetUserName(void)
{
  DWORD size = 256;
  char* str = iupStrGetMemory(size);
  GetUserNameA((LPSTR)str, &size);
  return (char*)str;
}

int iupdrvSetCurrentDirectory(const char* path)
{
  return SetCurrentDirectoryA(path);
}

char* iupdrvGetCurrentDirectory(void)
{
  char* cur_dir = NULL;

  int len = GetCurrentDirectoryA(0, NULL);
  if (len == 0) return NULL;

  cur_dir = iupStrGetMemory(len + 2);
  GetCurrentDirectoryA(len + 1, cur_dir);
  cur_dir[len] = '\\';
  cur_dir[len + 1] = 0;

  return cur_dir;
}

void IupLogV(const char* type, const char* format, va_list arglist)
{
  HANDLE EventSource;
  WORD wtype = 0;

  int size;
  char* value = iupStrGetLargeMem(&size);
  vsnprintf(value, size, format, arglist);

  if (iupStrEqualNoCase(type, "DEBUG"))
  {
    OutputDebugStringA(value);
    return;
  }
  else if (iupStrEqualNoCase(type, "ERROR"))
    wtype = EVENTLOG_ERROR_TYPE;
  else if (iupStrEqualNoCase(type, "WARNING"))
    wtype = EVENTLOG_WARNING_TYPE;
  else if (iupStrEqualNoCase(type, "INFO"))
    wtype = EVENTLOG_INFORMATION_TYPE;

  EventSource = RegisterEventSourceA(NULL, "Application");
  if (EventSource)
  {
    ReportEventA(EventSource, wtype, 0, 0, NULL, 1, 0, &value, NULL);
    DeregisterEventSource(EventSource);
  }
}

void IupLog(const char* type, const char* format, ...)
{
  va_list arglist;
  va_start(arglist, format);
  IupLogV(type, format, arglist);
  va_end(arglist);
}
