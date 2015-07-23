/** \file
 * \brief GTK Base Functions
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdio.h>              
#include <stdlib.h>
#include <string.h>             
#include <limits.h>             

#include <gtk/gtk.h>

#include "iup.h"

#include "iup_object.h"
#include "iup_str.h"

#include "iupgtk_drv.h"


static int iupgtk_utf8mode = 0;

void iupgtkStrSetUTF8Mode(int utf8mode)
{
  iupgtk_utf8mode = utf8mode;
}

int iupgtkStrGetUTF8Mode(void)
{
  return iupgtk_utf8mode;
}

static char* gtkStrToUTF8(const char *str, int len, const char* charset)
{
  return g_convert(str, len, "UTF-8", charset, NULL, NULL, NULL);
}

static char* gtkStrFromUTF8(const char *str, const char* charset)
{
  return g_convert(str, -1, charset, "UTF-8", NULL, NULL, NULL);
}

static char* gtkLastConvertUTF8 = NULL;

void iupgtkStrRelease(void)
{
  if (gtkLastConvertUTF8)
    g_free(gtkLastConvertUTF8);
}

char* iupgtkStrConvertToSystemLen(const char* str, int *len)
{
  if (!str || *str == 0)
    return (char*)str;

  if (!iupgtk_utf8mode)  
  {
    const char *charset = NULL;
    if (g_get_charset(&charset)==TRUE)  /* current locale is already UTF-8 */
    {
      if (g_utf8_validate(str, *len, NULL))
        return (char*)str;
      else
      {
        if (gtkLastConvertUTF8)
          g_free(gtkLastConvertUTF8);
        gtkLastConvertUTF8 = gtkStrToUTF8(str, *len, "ISO8859-1");   /* if string is not UTF-8, assume ISO8859-1 */
        if (!gtkLastConvertUTF8) return (char*)str;
        *len = strlen(gtkLastConvertUTF8);
        return gtkLastConvertUTF8;
      }
    }
    else
    {
      if (iupStrIsAscii(str) || !charset)
        return (char*)str;
      else if (charset)
      {
        if (gtkLastConvertUTF8)
          g_free(gtkLastConvertUTF8);
        gtkLastConvertUTF8 = gtkStrToUTF8(str, *len, charset);
        if (!gtkLastConvertUTF8) return (char*)str;
        *len = strlen(gtkLastConvertUTF8);
        return gtkLastConvertUTF8;
      }
    }
  }
  return (char*)str;
}

char* iupgtkStrConvertToSystem(const char* str)  /* From IUP to GTK */
{
  if (!str || *str == 0)
    return (char*)str;

  if (!iupgtk_utf8mode)  
  {
    const char *charset = NULL;
    if (g_get_charset(&charset)==TRUE)  /* current locale is already UTF-8 */
    {
      if (g_utf8_validate(str, -1, NULL))
        return (char*)str;
      else
      {
        if (gtkLastConvertUTF8)
          g_free(gtkLastConvertUTF8);
        gtkLastConvertUTF8 = gtkStrToUTF8(str, -1, "ISO8859-1");   /* if string is not UTF-8, assume ISO8859-1 */
        if (!gtkLastConvertUTF8) return (char*)str;
        return gtkLastConvertUTF8;
      }
    }
    else
    {
      if (iupStrIsAscii(str) || !charset)
        return (char*)str;
      else if (charset)
      {
        if (gtkLastConvertUTF8)
          g_free(gtkLastConvertUTF8);
        gtkLastConvertUTF8 = gtkStrToUTF8(str, -1, charset);
        if (!gtkLastConvertUTF8) return (char*)str;
        return gtkLastConvertUTF8;
      }
    }
  }
  return (char*)str;
}

char* iupgtkStrConvertFromSystem(const char* str)  /* From GTK to IUP */
{
  if (!str || *str == 0)
    return (char*)str;

  if (!iupgtk_utf8mode)  
  {
    const gchar *charset = NULL;
    if (g_get_charset(&charset)==TRUE)  /* current locale is already UTF-8 */
    {
      if (g_utf8_validate(str, -1, NULL))
        return (char*)str;
      else
      {
        if (gtkLastConvertUTF8)
          g_free(gtkLastConvertUTF8);
        gtkLastConvertUTF8 = gtkStrFromUTF8(str, "ISO8859-1");  /* if string is not UTF-8, assume ISO8859-1 */
        if (!gtkLastConvertUTF8) return (char*)str;
        return gtkLastConvertUTF8;
      }
    }
    else
    {
      if (iupStrIsAscii(str) || !charset)
        return (char*)str;
      else if (charset)
      {
        if (gtkLastConvertUTF8)
          g_free(gtkLastConvertUTF8);
        gtkLastConvertUTF8 = gtkStrFromUTF8(str, charset);
        if (!gtkLastConvertUTF8) return (char*)str;
        return gtkLastConvertUTF8;
      }
    }
  }
  return (char*)str;
}

static gboolean gtkGetFilenameCharset(const gchar **filename_charset)
{
  const gchar **charsets = NULL;
  gboolean is_utf8 = FALSE;
  
#if GTK_CHECK_VERSION(2, 6, 0)
  is_utf8 = g_get_filename_charsets (&charsets);
#endif

  if (filename_charset && charsets)
    *filename_charset = charsets[0];
  
  return is_utf8;
}

char* iupgtkStrConvertToFilename(const char* str)   /* From IUP to Filename */
{
  if (!str || *str == 0)
    return (char*)str;

  if (!iupgtk_utf8mode)  
    return (char*)str;
  else
  {
    const gchar *charset = NULL;
    if (gtkGetFilenameCharset(&charset)==TRUE)  /* current locale is already UTF-8 */
    {
      if (g_utf8_validate(str, -1, NULL))
        return (char*)str;
      else
      {
        if (gtkLastConvertUTF8)
          g_free(gtkLastConvertUTF8);
        gtkLastConvertUTF8 = gtkStrFromUTF8(str, "ISO8859-1");  /* if string is not UTF-8, assume ISO8859-1 */
        if (!gtkLastConvertUTF8) return (char*)str;
        return gtkLastConvertUTF8;
      }
    }
    else
    {
      if (iupStrIsAscii(str) || !charset)
        return (char*)str;
      else if (charset)
      {
        if (gtkLastConvertUTF8)
          g_free(gtkLastConvertUTF8);
        gtkLastConvertUTF8 = gtkStrFromUTF8(str, charset);
        if (!gtkLastConvertUTF8) return (char*)str;
        return gtkLastConvertUTF8;
      }
    }
  }
  return (char*)str;
}

char* iupgtkStrConvertFromFilename(const char* str)   /* From Filename to IUP */
{
  if (!str || *str == 0)
    return (char*)str;

  if (!iupgtk_utf8mode)  
    return (char*)str;
  else
  {
    const char *charset = NULL;
    if (gtkGetFilenameCharset(&charset)==TRUE)  /* current locale is already UTF-8 */
    {
      if (g_utf8_validate(str, -1, NULL))
        return (char*)str;
      else
      {
        if (gtkLastConvertUTF8)
          g_free(gtkLastConvertUTF8);
        gtkLastConvertUTF8 = gtkStrToUTF8(str, -1, "ISO8859-1");   /* if string is not UTF-8, assume ISO8859-1 */
        if (!gtkLastConvertUTF8) return (char*)str;
        return gtkLastConvertUTF8;
      }
    }
    else
    {
      if (iupStrIsAscii(str) || !charset)
        return (char*)str;
      else if (charset)
      {
        if (gtkLastConvertUTF8)
          g_free(gtkLastConvertUTF8);
        gtkLastConvertUTF8 = gtkStrToUTF8(str, -1, charset);
        if (!gtkLastConvertUTF8) return (char*)str;
        return gtkLastConvertUTF8;
      }
    }
  }
  return (char*)str;
}

