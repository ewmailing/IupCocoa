/** \file
 * \brief MAC OS System Information
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdio.h>
#include <stdlib.h> 
#include <string.h>

#include <Entry.h>
#include <OS.h>
#include <Screen.h>

/* This module should depend only on IUP core headers */

#include <sys/utsname.h>
#include <unistd.h>
#include <limits.h>
#include <errno.h>
#include <sys/stat.h>
#include <langinfo.h>

#include "iup.h" 

#include "iup_str.h"
#include "iup_drv.h"
#include "iup_drvinfo.h"

#define IUP_MAC_ERROR -1
#define UNIMPLEMENTED printf("%s (%s %d) UNIMPLEMENTED\n",__func__,__FILE__,__LINE__);

int iupdrvMakeDirectory(const char* name) 
{
  mode_t oldmask = umask((mode_t)0);
  int fail =  mkdir(name, S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP |
                          S_IWGRP | S_IXGRP | S_IROTH | S_IXOTH);
  umask (oldmask);
  if (fail)
    return 0;
  return 1;
}

int iupdrvIsFile(const char* name)
{
  return BEntry(name).IsFile();
}

int iupdrvIsDirectory(const char* name)
{
  return BEntry(name).IsDirectory();
}            

char* iupdrvGetCurrentDirectory(void)
{
  size_t size = 256;
  char *buffer = (char *)malloc(size);

  for (;;)
  {
    if (getcwd(buffer, size) != NULL)
      return buffer;

    if (errno != ERANGE)
    {
      free(buffer);
      return NULL;
    }

    size += size;
    buffer = (char *)realloc(buffer, size);
  }

  return NULL;
}

int iupdrvSetCurrentDirectory(const char* dir)
{
  return chdir(dir) == 0? 1: 0;
}

int iupdrvGetWindowDecor(void* wnd, int *border, int *caption)
{
	UNIMPLEMENTED

  *border  = 0;
  *caption = 0;

  return 0;
}

void iupdrvGetScreenSize(int *width, int *height)
{
	BScreen screen;
	BRect frame = screen.Frame();

	if(width) *width = (int)frame.right + 1;
	if(height) *height = (int)frame.bottom + 1;
}

void iupdrvGetFullSize(int *width, int *height)
{
	// TODO is this only useful in mutli-monitor situations ?
	iupdrvGetScreenSize(width, height);
}

int iupdrvGetScreenDepth(void)
{
	BScreen screen;

	switch(screen.ColorSpace())
	{
		case B_CMAP8: return 8;
		case B_RGB15: return 15;
		case B_RGB32: return 32;
		default:
			printf("%s (%s:%d) FIXME unknown colorspace\n", __func__, __FILE__, __LINE__);
			return 0;
	}
}

float iupdrvGetScreenDpi(void)
{
	UNIMPLEMENTED
		return 0;
}

void iupdrvGetCursorPos(int *x, int *y)
{
	UNIMPLEMENTED
}

void iupdrvGetKeyState(char* key)
{
	UNIMPLEMENTED
}

char* iupdrvLocaleInfo(void)
{
  return iupStrGetMemoryCopy(nl_langinfo(CODESET));
}

/* Everything below is copypasted from mot/iupunix.c, but that one depends on X11 :( */
char *iupdrvGetSystemName(void)
{
  struct utsname un;
  char *str = iupStrGetMemory(50); 

  uname(&un);
  if (iupStrEqualNoCase(un.sysname, "Darwin"))
    strcpy(str, "MacOS");
  else
    strcpy(str, un.sysname);

  return str;
}

char *iupdrvGetSystemVersion(void)
{
  struct utsname un;
  char *str = iupStrGetMemory(100); 

  uname(&un);
  if (iupStrEqualNoCase(un.sysname, "Darwin"))
  {
    int release = atoi(un.release);
    sprintf(str, "%d", release-4);
  }
  else
  {
    strcpy(str, un.release);
    strcat(str, ".");
    strcat(str, un.version);
  }

  return str;
}

char *iupdrvGetComputerName(void)
{
  char* str = iupStrGetMemory(50);
  gethostname(str, 50);
  return str;
}

char *iupdrvGetUserName(void)
{
  return (char*)getlogin();
}

