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

float iupdrvGetScreenDpi(void)
{
  float dpi;
  HDC hDCDisplay = GetDC(NULL);
  dpi = (float)GetDeviceCaps(hDCDisplay, LOGPIXELSY);
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

int iupdrvGetPreferencePath(char *filename, int str_len, const char *app_name)
{
  /*
  The implementation IUP originally used does not correctly follow the official Windows conventions.
  Also remember that the official location changed starting from Windows Vista.
  SHGetFolderPath fortunately works on both. (It calls the new Vista SHGetKnownFolderPath under the hood.)
  SHGetKnownFolderPath(FOLDERID_RoamingAppData, KF_FLAG_CREATE, NULL, &winPath);
  Vista+ directory: SHGetFolderPath CSIDL_APPDATA  C:\Users\patricka\AppData\Roaming
  XP directory: SHGetFolderPath CSIDL_APPDATA  C:\Documents and Settings\XPMUser\Application Data
  */


  /* TODO: My version uses the 'A' version instead of the 'W'. This means we're affected by the MAX_PATH character limit. */
  /* This is technically deprecated, but this is also Microsoft... */
  BOOL ret_flag;
  const char path_separator[] = "\\";
  HRESULT the_result;

  /*
  To preserve backwards compatibility
  We will use the old algorithm and look for the app_name.cfg (because this was the only thing in IUP using this information).
  If it exists, it means the app is already using the old system and we want to preserve the legacy behavior.
  If the file does not exist, nothing depends on the legacy code and we are free to use the santioned way.
  */
  BOOL use_legacy_mode = 0;
  {
    // 10240 was the hardcoded number IUP used originally.
    char legacy_config_filename[10240] = "";
    HANDLE find_handle;
    WIN32_FIND_DATA find_file_data;
  
    /* This implementation is copied from the original iConfigSetFilename */
    char* homedrive = getenv("HOMEDRIVE");
    char* homepath = getenv("HOMEPATH");
    if (homedrive && homepath)
    {
      /* bounds checks are skipped because Visual Studio has a bad track record of supporting safe versions. */
      strcpy(filename, homedrive);
      strcat(filename, homepath);
      strcat(filename, path_separator);
      /* filename is now the legacy path we should return if we are in legacy mode. */


      /* Now test to see if the app_name.cfg is there. 
         If it is, we need to use legacy mode.
         Otherwise, we can go to non-legacy mode.
      */
      strcpy(legacy_config_filename, filename);
      strcat(legacy_config_filename, app_name);
      strcat(legacy_config_filename, ".cfg");      

      find_handle = FindFirstFileA(legacy_config_filename, &find_file_data);
      if (INVALID_HANDLE_VALUE == find_handle) 
      {
          use_legacy_mode = 0;
      } 
      else 
      {
          use_legacy_mode = 1;
          return 1;
      }
    }
  }
  
  /* If we get here, we are NOT in legacy mode. */

  the_result = SHGetFolderPathA(NULL, CSIDL_APPDATA | CSIDL_FLAG_CREATE, NULL, SHGFP_TYPE_CURRENT, filename);
  if (S_OK != the_result)
  {
    filename[0] = '\0';
    return 0;
  }

  /* Let's create a new subdirectory using the app_name because all programs dump to this same location. */
  /* bounds checks are skipped because Visual Studio has a bad track record of supporting safe versions. */
  strcat(filename, path_separator);
  strcat(filename, app_name);
  strcat(filename, path_separator);

  ret_flag = CreateDirectoryA(filename, NULL);
  if (0 == ret_flag)
  {
      if (GetLastError() != ERROR_ALREADY_EXISTS)
      {
          filename[0] = '\0';
          return 0;
      }
  }

  return 1;

}

