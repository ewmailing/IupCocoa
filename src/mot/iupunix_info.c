/** \file
 * \brief UNIX System Information
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdio.h>
#include <stdlib.h> 
#include <string.h> 
#include <limits.h>
#include <sys/stat.h>

/* This module should depend only on IUP core headers 
   and UNIX system headers. NO Motif headers allowed. */

#include <sys/utsname.h>
#include <unistd.h>
#include <errno.h>
#include <langinfo.h>
#include <syslog.h>

#include "iup_str.h"
#include "iup_drvinfo.h"
#include "iup_varg.h"


char* iupdrvLocaleInfo(void)
{
  return iupStrReturnStr(nl_langinfo(CODESET));
}

char *iupdrvGetSystemName(void)
{
  struct utsname un;
  uname(&un);
  if (iupStrEqualNoCase(un.sysname, "Darwin"))
    return iupStrReturnStr("MacOS");
  else
    return iupStrReturnStr(un.sysname);
}

char *iupdrvGetSystemVersion(void)
{
  struct utsname un;
  uname(&un);
  if (iupStrEqualNoCase(un.sysname, "Darwin"))
  {
    int release = atoi(un.release);
    return iupStrReturnInt(release-4);
  }
  else
    return iupStrReturnStrf("%s.%s", un.release, un.version);
}

int iupdrvSetCurrentDirectory(const char* dir)
{
  return chdir(dir) == 0 ? 1 : 0;
}

char* iupdrvGetCurrentDirectory(void)
{
  size_t size = 256;
  char *buffer = (char *)iupStrGetMemory(size);

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
    buffer = (char *)iupStrGetMemory(size);
  }

  return NULL;
}


/**************************************************************************/


int iupUnixIsFile(const char* name)
{
  struct stat status;
  if (stat(name, &status) != 0)
    return 0;
  if (S_ISDIR(status.st_mode))
    return 0;
  return 1;
}

int iupUnixIsDirectory(const char* name)
{
  struct stat status;
  if (stat(name, &status) != 0)
    return 0;
  if (S_ISDIR(status.st_mode))
    return 1;
  return 0;
}            

int iupUnixMakeDirectory(const char* name) 
{
  mode_t oldmask = umask((mode_t)0);
  int fail =  mkdir(name, S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP |
                          S_IWGRP | S_IXGRP | S_IROTH | S_IXOTH);
  umask (oldmask);
  if (fail)
    return 0;
  return 1;
}


/**************************************************************************/


void IupLogV(const char* type, const char* format, va_list arglist)
{
  int options = LOG_CONS | LOG_PID;
  int priority = 0;

  if (iupStrEqualNoCase(type, "DEBUG"))
  {
    priority = LOG_DEBUG;
#ifdef LOG_PERROR
    options |= LOG_PERROR;
#endif
  }
  else if (iupStrEqualNoCase(type, "ERROR"))
    priority = LOG_ERR;
  else if (iupStrEqualNoCase(type, "WARNING"))
    priority = LOG_WARNING;
  else if (iupStrEqualNoCase(type, "INFO"))
    priority = LOG_INFO;

  openlog(NULL, options, LOG_USER);

  vsyslog(priority, format, arglist);

  closelog();
}

void IupLog(const char* type, const char* format, ...)
{
  va_list arglist;
  va_start(arglist, format);
  IupLogV(type, format, arglist);
  va_end(arglist);
}
