/** \file
 * \brief String Utilities
 *
 * See Copyright Notice in "iup.h"
 */

 
#include <string.h>  
#include <stdlib.h>  
#include <stdio.h>  
#include <limits.h>
#include <stdarg.h>
#include <locale.h>

#include "iup_str.h"


/* Line breaks can be:
\n - UNIX
\r - Mac
\r\n - DOS/Windows
*/


#define IUP_STR_EQUAL(str1, str2)      \
{                                      \
  if (str1 == str2)                    \
    return 1;                          \
                                       \
  if (!str1 || !str2)                  \
    return 0;                          \
                                       \
  while(*str1 && *str2 &&              \
        SF(*str1) == SF(*str2))        \
  {                                    \
    EXTRAINC(str1);                    \
    EXTRAINC(str2);                    \
    str1++;                            \
    str2++;                            \
  }                                    \
                                       \
  /* check also for terminator */      \
  if (*str1 == *str2) return 1;        \
}

int iupStrEqual(const char* str1, const char* str2) 
{
#define EXTRAINC(_x) (void)(_x)
#define SF(_x) (_x)
  IUP_STR_EQUAL(str1, str2);
#undef SF
#undef EXTRAINC
  return 0;
}

int iupStrEqualPartial(const char* str1, const char* str2) 
{
#define EXTRAINC(_x) (void)(_x)
#define SF(_x) (_x)
  IUP_STR_EQUAL(str1, str2);
#undef SF
#undef EXTRAINC
  if (*str2 == 0) 
    return 1;  /* if second string is at terminator, then it is partially equal */
  return 0;
}

int iupStrEqualNoCase(const char* str1, const char* str2) 
{
#define EXTRAINC(_x) (void)(_x)
#define SF(_x) iup_tolower(_x)
  IUP_STR_EQUAL(str1, str2);
#undef SF
#undef EXTRAINC
  return 0;
}

int iupStrEqualNoCasePartial(const char* str1, const char* str2) 
{
#define EXTRAINC(_x) (void)(_x)
#define SF(_x) iup_tolower(_x)
  IUP_STR_EQUAL(str1, str2);
#undef SF
#undef EXTRAINC
  if (*str2 == 0) 
    return 1;  /* if second string is at terminator, then it is partially equal */
  return 0;
}

int iupStrEqualNoCaseNoSpace(const char* str1, const char* str2) 
{
#define EXTRAINC(_x) { if (*_x == ' ') _x++; }  /* also ignore spaces */
#define SF(_x) iup_tolower(_x)
  IUP_STR_EQUAL(str1, str2);
#undef SF
#undef EXTRAINC
  return 0;
}

int iupStrFalse(const char* str)
{
  if (!str || str[0]==0) return 0;
  if (str[0]=='0' && str[1]==0) return 1;
  if (iupStrEqualNoCase(str, "NO")) return 1;
  if (iupStrEqualNoCase(str, "OFF")) return 1;
  return 0;
}

int iupStrBoolean(const char* str)
{
  if (!str || str[0]==0) return 0;
  if (str[0]=='1' && str[1]==0) return 1;
  if (iupStrEqualNoCase(str, "YES")) return 1;
  if (iupStrEqualNoCase(str, "ON")) return 1;
  return 0;
}

void iupStrUpper(char* dstr, const char* sstr)
{
  if (!sstr || sstr[0] == 0) return;
  for (; *sstr; sstr++, dstr++)
    *dstr = (char)iup_toupper(*sstr);
  *dstr = 0;
}

void iupStrLower(char* dstr, const char* sstr)
{
  if (!sstr || sstr[0] == 0) return;
  for (; *sstr; sstr++, dstr++)
    *dstr = (char)iup_tolower(*sstr);
  *dstr = 0;
}

int iupStrHasSpace(const char* str)
{
  if (!str) return 0;
  while (*str)
  {
    if (*str == ' ')
      return 1;
    str++;
  }
  return 0;
}

char *iupStrDup(const char *str)
{
  if (str)
  {
    int size = strlen(str)+1;
    char *newstr = malloc(size);
    if (newstr) memcpy(newstr, str, size);
    return newstr;
  }
  return NULL;
}

const char* iupStrNextLine(const char* str, int *len)
{
  *len = 0;

  if (!str) return NULL;

  while(*str!=0 && *str!='\n' && *str!='\r') 
  {
    (*len)++;
    str++;
  }

  if (*str=='\r' && *(str+1)=='\n')   /* DOS line end */
    return str+2;
  else if (*str=='\n' || *str=='\r')   /* UNIX or MAC line end */
    return str+1;
  else 
    return str;  /* no next line */
}

const char* iupStrNextValue(const char* str, int str_len, int *len, char sep)
{
  *len = 0;

  if (!str) return NULL;

  while(*str!=0 && *str!=sep && *len<str_len) 
  {
    (*len)++;
    str++;
  }

  if (*str==sep)
    return str+1;
  else 
    return str;  /* no next value */
}

int iupStrLineCount(const char* str)
{
  int num_lin = 1;

  if (!str)
    return num_lin;

  while(*str != 0)
  {
    while(*str!=0 && *str!='\n' && *str!='\r')
      str++;

    if (*str=='\r' && *(str+1)=='\n')   /* DOS line end */
    {
      num_lin++;
      str+=2;
    }
    else if (*str=='\n' || *str=='\r')   /* UNIX or MAC line end */
    {
      num_lin++;
      str++;
    }
  }

  return num_lin;
}

int iupStrCountChar(const char *str, char c)
{
  int n;
  if (!str) return 0;
  for (n=0; *str; str++)
  {
    if (*str==c)
      n++;
  }
  return n;
}

void iupStrCopyN(char* dst_str, int dst_max_size, const char* src_str)
{
  if (src_str)
  {
    int size = strlen(src_str) + 1;
    if (size > dst_max_size) size = dst_max_size;
    memcpy(dst_str, src_str, size - 1);
    dst_str[size - 1] = 0;
  }
}

char* iupStrDupUntil(const char **str, char c)
{
  const char *p_str;
  char *new_str;
  if (!str || str[0]==0)
    return NULL;

  p_str = strchr(*str,c);
  if (!p_str) 
    return NULL;
  else
  {
    int i;
    int sl = (int)(p_str - (*str));

    new_str = (char *)malloc(sl + 1);
    if (!new_str) return NULL;

    for (i = 0; i < sl; ++i)
      new_str[i] = (*str)[i];

    new_str[sl] = 0;

    *str = p_str+1;
    return new_str;
  }
}

static char *iStrDupUntilNoCase(char **str, char sep)
{
  char *p_str,*new_str;
  if (!str || str[0]==0)
    return NULL;

  p_str=strchr(*str,sep); /* usually the lower case is enough */
  if (!p_str && (iup_toupper(sep)!=sep)) 
    p_str=strchr(*str, iup_toupper(sep));  /* but check also for upper case */

  /* if both fail, then abort */
  if (!p_str) 
    return NULL;
  else
  {
    int i;
    int sl=(int)(p_str - (*str));

    new_str = (char *) malloc (sl + 1);
    if (!new_str) return NULL;

    for (i = 0; i < sl; ++i)
      new_str[i] = (*str)[i];

    new_str[sl] = 0;

    *str = p_str+1;
    return new_str;
  }
}

char *iupStrGetLargeMem(int *size)
{
#define LARGE_MAX_BUFFERS 10
#define LARGE_SIZE SHRT_MAX
  static char buffers[LARGE_MAX_BUFFERS][LARGE_SIZE];
  static int buffers_index = -1;
  char* ret_str;

  /* init buffers array */
  if (buffers_index == -1)
  {
    int i;

    memset(buffers, 0, sizeof(char*)*LARGE_MAX_BUFFERS);
    buffers_index = 0;

    /* clear all memory only once */
    for (i=0; i<LARGE_MAX_BUFFERS; i++)
      memset(buffers[i], 0, sizeof(char)*LARGE_SIZE);
  }

  /* DON'T clear memory everytime because the buffer is too large */
  ret_str = buffers[buffers_index];
  ret_str[0] = 0;

  buffers_index++;
  if (buffers_index == LARGE_MAX_BUFFERS)
    buffers_index = 0;

  if (size) *size = LARGE_SIZE;
  return ret_str;
#undef LARGE_MAX_BUFFERS
#undef LARGE_SIZE 
}

static char* iupStrGetSmallMem(void)
{
#define SMALL_MAX_BUFFERS 100
#define SMALL_SIZE 80  /* maximum for iupStrReturnFloat */
  static char buffers[SMALL_MAX_BUFFERS][SMALL_SIZE];
  static int buffers_index = -1;
  char* ret_str;

  /* init buffers array */
  if (buffers_index == -1)
  {
    memset(buffers, 0, sizeof(char*)*SMALL_MAX_BUFFERS);
    buffers_index = 0;
  }

  /* always clear memory before returning a new buffer */
  memset(buffers[buffers_index], 0, SMALL_SIZE);
  ret_str = buffers[buffers_index];

  buffers_index++;
  if (buffers_index == SMALL_MAX_BUFFERS)
    buffers_index = 0;

  return ret_str;
#undef SMALL_MAX_BUFFERS
#undef SMALL_SIZE 
}

char *iupStrGetMemory(int size)
{
#define MAX_BUFFERS 50
  static char* buffers[MAX_BUFFERS];
  static int buffers_sizes[MAX_BUFFERS];
  static int buffers_index = -1;

  int i;

  if (size == -1) /* Frees memory */
  {
    buffers_index = -1;
    for (i = 0; i < MAX_BUFFERS; i++)
    {
      if (buffers[i]) 
      {
        free(buffers[i]);
        buffers[i] = NULL;
      }
      buffers_sizes[i] = 0;
    }
    return NULL;
  }
  else
  {
    char* ret_str;

    /* init buffers array */
    if (buffers_index == -1)
    {
      memset(buffers, 0, sizeof(char*)*MAX_BUFFERS);
      memset(buffers_sizes, 0, sizeof(int)*MAX_BUFFERS);
      buffers_index = 0;
    }

    /* first alocation */
    if (!(buffers[buffers_index]))
    {
      buffers_sizes[buffers_index] = size+1;
      buffers[buffers_index] = (char*)malloc(buffers_sizes[buffers_index]);
    }
    else if (buffers_sizes[buffers_index] < size+1)  /* reallocate if necessary */
    {
      buffers_sizes[buffers_index] = size+1;
      buffers[buffers_index] = (char*)realloc(buffers[buffers_index], buffers_sizes[buffers_index]);
    }

    /* always clear memory before returning a new buffer */
    memset(buffers[buffers_index], 0, buffers_sizes[buffers_index]);
    ret_str = buffers[buffers_index];

    buffers_index++;
    if (buffers_index == MAX_BUFFERS)
      buffers_index = 0;

    return ret_str;
  }
#undef MAX_BUFFERS
}

char* iupStrReturnStrf(const char* format, ...)
{
  char* str = iupStrGetMemory(1024);
  va_list arglist;
  va_start(arglist, format);
  vsnprintf(str, 1024, format, arglist);
  va_end(arglist);
  return str;
}

char* iupStrReturnStr(const char* str)
{
  if (str)
  {
    int size = strlen(str)+1;
    char* ret_str = iupStrGetMemory(size);
    memcpy(ret_str, str, size);
    return ret_str;
  }
  else
    return NULL;
}

char* iupStrReturnBoolean(int b)
{
  if (b)
    return "YES";
  else
    return "NO";
}

char* iupStrReturnChecked(int check)
{
  if (check == -1)
    return "NOTDEF";
  else if (check)
    return "ON";
  else
    return "OFF";
}

char* iupStrReturnInt(int i)
{
  char* str = iupStrGetSmallMem();  /* 20 */
  sprintf(str, "%d", i);
  return str;
}

char* iupStrReturnFloat(float f)
{
  char* str = iupStrGetSmallMem();  /* 80 */
  sprintf(str, IUP_FLOAT2STR, f);
  return str;
}

char* iupStrReturnDouble(double d)
{
  char* str = iupStrGetSmallMem();  /* 80 */
  sprintf(str, IUP_DOUBLE2STR, d);
  return str;
}

char* iupStrReturnRGB(unsigned char r, unsigned char g, unsigned char b)
{
  char* str = iupStrGetSmallMem();  /* 3*20 */
  sprintf(str, "%d %d %d", (int)r, (int)g, (int)b);
  return str;
}

char* iupStrReturnRGBA(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
  char* str = iupStrGetSmallMem();  /* 4*20 */
  sprintf(str, "%d %d %d %d", (int)r, (int)g, (int)b, (int)a);
  return str;
}

char* iupStrReturnStrStr(const char *str1, const char *str2, char sep)
{
  if (str1 || str2)
  {
    char* ret_str;
    int size1=0, size2=0;
    if (str1) size1 = strlen(str1);
    if (str2) size2 = strlen(str2);
    ret_str = iupStrGetMemory(size1+size2+2);
    if (str1 && size1) memcpy(ret_str, str1, size1);
    ret_str[size1] = sep;
    if (str2 && size2) memcpy(ret_str+size1+1, str2, size2);
    ret_str[size1+1+size2] = 0;
    return ret_str;
  }
  else
    return NULL;
}

char* iupStrReturnIntInt(int i1, int i2, char sep)
{
  char* str = iupStrGetSmallMem();  /* 2*20 */
  sprintf(str, "%d%c%d", i1, sep, i2);
  return str;
}

int iupStrGetFormatPrecision(const char* format)
{
  int precision;
  while (*format)
  {
    if (*format == '.')
      break;
    format++;
  }

  if (*format != '.')
    return -1;

  format++;
  if (iupStrToInt(format, &precision))
    return precision;

  return -1;
}

int iupStrToRGB(const char *str, unsigned char *r, unsigned char *g, unsigned char *b)
{
  unsigned int ri = 0, gi = 0, bi = 0;
  if (!str) return 0;
  if (str[0]=='#')
  {
    str++;
    if (sscanf(str, "%2X%2X%2X", &ri, &gi, &bi) != 3) return 0;
  }
  else
  {
    if (sscanf(str, "%u %u %u", &ri, &gi, &bi) != 3) return 0;
  }
  if (ri > 255 || gi > 255 || bi > 255) return 0;
  *r = (unsigned char)ri;
  *g = (unsigned char)gi;
  *b = (unsigned char)bi;
  return 1;
}

int iupStrToRGBA(const char *str, unsigned char *r, unsigned char *g, unsigned char *b, unsigned char *a)
{
  unsigned int ri = 0, gi = 0, bi = 0, ai = 255;
  if (!str) return 0;
  if (str[0] == '#')
  {
    str++;
    if (sscanf(str, "%2X%2X%2X%2X", &ri, &gi, &bi, &ai) < 3) return 0;
  }
  else
  {
    if (sscanf(str, "%u %u %u %u", &ri, &gi, &bi, &ai) < 3) return 0;
  }
  if (ri > 255 || gi > 255 || bi > 255 || ai > 255) return 0;
  *r = (unsigned char)ri;
  *g = (unsigned char)gi;
  *b = (unsigned char)bi;
  *a = (unsigned char)ai;
  return 1;
}

/* TODO: are strtod/atof and strtol/atoi faster/better than sscanf? 
         must handle the 0 return value. */

int iupStrToInt(const char *str, int *i)
{
  if (!str) return 0;
  if (sscanf(str, "%d", i) != 1) return 0;
  return 1;
}

int iupStrToIntInt(const char *str, int *i1, int *i2, char sep)
{
  if (!str) return 0;
                         
  if (iup_tolower(*str) == sep) /* no first value */
  {
    str++; /* skip separator */
    if (sscanf(str, "%d", i2) != 1) return 0;
    return 1;
  }
  else 
  {
    char* p_str = iStrDupUntilNoCase((char**)&str, sep);
    
    if (!p_str)   /* no separator means no second value */
    {        
      if (sscanf(str, "%d", i1) != 1) return 0;
      return 1;
    }
    else if (*str==0)  /* separator exists, but second value empty, also means no second value */
    {        
      int ret = sscanf(p_str, "%d", i1);
      free(p_str);
      if (ret != 1) return 0;
      return 1;
    }
    else
    {
      int ret = 0;
      if (sscanf(p_str, "%d", i1) == 1) ret++;
      if (sscanf(str, "%d", i2) == 1) ret++;
      free(p_str);
      return ret;
    }
  }
}

int iupStrToFloatDef(const char *str, float *f, float def)
{
  if (!str) { *f = def;  return 1; }
  if (sscanf(str, "%f", f) != 1) return 0;
  return 1;
}

int iupStrToFloat(const char *str, float *f)
{
  if (!str) return 0;
  if (sscanf(str, "%f", f) != 1) return 0;
  return 1;
}

int iupStrToDouble(const char *str, double *d)
{
  if (!str) return 0;
  if (sscanf(str, "%lf", d) != 1) return 0;
  return 1;
}

int iupStrToDoubleDef(const char *str, double *d, double def)
{
  if (!str) { *d = def;  return 1; }
  if (sscanf(str, "%lf", d) != 1) return 0;
  return 1;
}

int iupStrToFloatFloat(const char *str, float *f1, float *f2, char sep)
{
  if (!str) return 0;

  if (iup_tolower(*str) == sep) /* no first value */
  {
    str++; /* skip separator */
    if (sscanf(str, "%f", f2) != 1) return 0;
    return 1;
  }
  else 
  {
    char* p_str = iStrDupUntilNoCase((char**)&str, sep);
    
    if (!p_str)   /* no separator means no second value */
    {        
      if (sscanf(str, "%f", f1) != 1) return 0;
      return 1;
    }
    else if (*str==0)    /* separator exists, but second value empty, also means no second value */
    {        
      int ret = sscanf(p_str, "%f", f1);
      free(p_str);
      if (ret != 1) return 0;
      return 1;
    }
    else
    {
      int ret = 0;
      if (sscanf(p_str, "%f", f1) == 1) ret++;
      if (sscanf(str, "%f", f2) == 1) ret++;
      free(p_str);
      return ret;
    }
  }
}

int iupStrToDoubleDouble(const char *str, double *f1, double *f2, char sep)
{
  if (!str) return 0;

  if (iup_tolower(*str) == sep) /* no first value */
  {
    str++; /* skip separator */
    if (sscanf(str, "%lf", f2) != 1) return 0;
    return 1;
  }
  else
  {
    char* p_str = iStrDupUntilNoCase((char**)&str, sep);

    if (!p_str)   /* no separator means no second value */
    {
      if (sscanf(str, "%lf", f1) != 1) return 0;
      return 1;
    }
    else if (*str == 0)    /* separator exists, but second value empty, also means no second value */
    {
      int ret = sscanf(p_str, "%lf", f1);
      free(p_str);
      if (ret != 1) return 0;
      return 1;
    }
    else
    {
      int ret = 0;
      if (sscanf(p_str, "%lf", f1) == 1) ret++;
      if (sscanf(str, "%lf", f2) == 1) ret++;
      free(p_str);
      return ret;
    }
  }
}

int iupStrToStrStr(const char *str, char *str1, char *str2, char sep)
{
  if (!str) 
    return 0;

  if (iup_tolower(*str) == sep) /* no first value */
  {
    str++; /* skip separator */
    str1[0] = 0;
    strcpy(str2, str);
    return 1;
  }
  else 
  {
    char* p_str = iStrDupUntilNoCase((char**)&str, sep);
    
    if (!p_str)   /* no separator means no second value */
    {        
      strcpy(str1, str);
      str2[0] = 0;
      return 1;
    }
    else if (*str==0)    /* separator exists, but second value empty, also means no second value */
    {        
      strcpy(str1, p_str);
      free(p_str);
      str2[0] = 0;
      return 1;
    }
    else
    {
      strcpy(str1, p_str);
      strcpy(str2, str);
      free(p_str);
      return 2;
    }
  }
}

char* iupStrFileGetPath(const char *file_name)
{
  if (!file_name)
    return NULL;
  else
  {
    /* Starts at the last character */
    int len = strlen(file_name) - 1;
    while (len != 0)
    {
      if (file_name[len] == '\\' || file_name[len] == '/')
      {
        len++;
        break;
      }

      len--;
    }
    if (len == 0)
      return NULL;

    {
      char* path = malloc(len + 1);
      memcpy(path, file_name, len);
      path[len] = 0;

      return path;
    }
  }
}

char* iupStrFileGetTitle(const char *file_name)
{
  if (!file_name)
    return NULL;
  else
  {
    /* Starts at the last character */
    int len = strlen(file_name);
    int offset = len - 1;
    while (offset != 0)
    {
      if (file_name[offset] == '\\' || file_name[offset] == '/')
      {
        offset++;
        break;
      }

      offset--;
    }

    {
      int title_size = len - offset + 1;
      char* file_title = malloc(title_size);
      memcpy(file_title, file_name + offset, title_size);
      return file_title;
    }
  }
}

char* iupStrFileGetExt(const char *file_name)
{
  if (!file_name)
    return NULL;
  else
  {
    /* Starts at the last character */
    int len = strlen(file_name);
    int offset = len - 1;
    while (offset != 0)
    {
      /* if found a path separator stop. */
      if (file_name[offset] == '\\' || file_name[offset] == '/')
        return NULL;

      if (file_name[offset] == '.')
      {
        offset++;
        break;
      }

      offset--;
    }

    if (offset == 0)
      return NULL;

    {
      int ext_size = len - offset + 1;
      char* file_ext = (char*)malloc(ext_size);
      memcpy(file_ext, file_name + offset, ext_size);
      return file_ext;
    }
  }
}

char* iupStrFileMakeFileName(const char* path, const char* title)
{
  if (!path || !title)
    return NULL;
  else
  {
    int size_path = strlen(path);
    int size_title = strlen(title);
    char *file_name = malloc(size_path + size_title + 2);
    memcpy(file_name, path, size_path);

    if (path[size_path - 1] != '/')
    {
      file_name[size_path] = '/';
      size_path++;
    }

    memcpy(file_name + size_path, title, size_title);
    file_name[size_path + size_title] = 0;

    return file_name;
  }
}

void iupStrFileNameSplit(const char* file_name, char *path, char *title)
{
  int i, n;

  if (!file_name)
    return;

  /* Look for last folder separator and split title from path */
  n = strlen(file_name);
  for (i = n - 1; i >= 0; i--)
  {
    if (file_name[i] == '\\' || file_name[i] == '/') 
    {
      if (path)
      {
        strncpy(path, file_name, i+1);
        path[i+1] = 0;
      }

      if (title)
      {
        strcpy(title, file_name+i+1);
        title[n-i] = 0;
      }

      return;
    }
  }
}

int iupStrReplace(char* str, char src, char dst)
{
  int i = 0;

  if (!str)
    return 0;

  while (*str)
  {
    if (*str == src)
    {
      *str = dst;
      i++;
    }
    str++;
  }
  return i;
}

void iupStrToUnix(char* str)
{
  char* pstr = str;

  if (!str) return;
  
  while (*str)
  {
    if (*str == '\r')
    {
      if (*(str+1) != '\n')  /* MAC line end */
        *pstr++ = '\n';
      str++;
    }
    else
      *pstr++ = *str++;
  }
  
  *pstr = *str;
}

void iupStrToMac(char* str)
{
  char* pstr = str;

  if (!str) return;
  
  while (*str)
  {
    if (*str == '\r')
    {
      if (*(++str) == '\n')  /* DOS line end */
        str++;
      *pstr++ = '\r';
    }
    else if (*str == '\n')  /* UNIX line end */
    {
      str++;
      *pstr++ = '\r';
    }
    else
      *pstr++ = *str++;
  }
  
  *pstr = *str;
}

char* iupStrToDos(const char* str)
{
	char *auxstr, *newstr;
	int num_lin;

  if (!str) return NULL;

  num_lin = iupStrLineCount(str);
  if (num_lin == 1)
    return (char*)str;

	newstr = malloc(num_lin + strlen(str) + 1);
  auxstr = newstr;
	while(*str)
	{
		if (*str == '\r' && *(str+1)=='\n')  /* DOS line end */
    {
      *auxstr++ = *str++;
      *auxstr++ = *str++;
    }
    else if (*str == '\r')   /* MAC line end */
    {
		  *auxstr++ = *str++;
			*auxstr++ = '\n';
    }
    else if (*str == '\n')  /* UNIX line end */
    {
			*auxstr++ = '\r';
		  *auxstr++ = *str++;
    }
    else
		  *auxstr++ = *str++;
	}
	*auxstr = 0;

	return newstr;	
}

#define IUP_ISRESERVED(_c) (_c=='\n' || _c=='\r' || _c=='\t')

char* iupStrConvertToC(const char* str)
{
  char* new_str, *pnstr;
  const char* pstr = str;
  int len, count=0;

  if (!str)
    return NULL;

  while(*pstr)
  {
    if (IUP_ISRESERVED(*pstr))
      count++;
    pstr++;
  }
  if (!count)
    return (char*)str;

  len = (int)(pstr-str);
  new_str = malloc(len+count+1);
  pstr = str;
  pnstr = new_str;
  while(*pstr)
  {
    if (IUP_ISRESERVED(*pstr))
    {
      *pnstr = '\\';
      pnstr++;

      switch(*pstr)
      {
      case '\n':
        *pnstr = 'n';
        break;
      case '\r':
        *pnstr = 'r';
        break;
      case '\t':
        *pnstr = 't';
        break;
      }
    }
    else
      *pnstr = *pstr;

    pnstr++;
    pstr++;
  }
  *pnstr = 0;
  return new_str;
}

char* iupStrProcessMnemonic(const char* str, char *c, int action)
{
  int i = 0, found = 0;
  char* new_str, *orig_str = (char*)str;

  if (!str) 
    return NULL;

  if (!strchr(str, '&'))
    return (char*)str;

  new_str = malloc(strlen(str)+1);
  while (*str)
  {
    if (*str == '&')
    {
      if (*(str+1) == '&') /* remove & from the string, add next & to the string */
      {
        found = -1;

        str++;
        new_str[i++] = *str;
      }
      else if (found!=1) /* mnemonic found */
      {
        found = 1;

        if (action == 1) /* replace & by c */
          new_str[i++] = *c;
        else if (action == -1)  /* remove & and return in c */
          *c = *(str+1);  /* next is mnemonic */
        /* else -- only remove & */
      }
    }
    else
    {
      new_str[i++] = *str;
    }

    str++;
  }
  new_str[i] = 0;

  if (found==0)
  {
    free(new_str);
    return orig_str;
  }

  return new_str;
}

int iupStrFindMnemonic(const char* str)
{
  int c = 0, found = 0;

  if (!str) 
    return 0;

  if (!strchr(str, '&'))
    return 0;

  while (*str)
  {
    if (*str == '&')
    {
      if (*(str+1) == '&')
      {
        found = -1;
        str++;
      }
      else if (found!=1) /* mnemonic found */
      {
        found = 1;
        c = *(str+1);  /* next is mnemonic */
      }
    }

    str++;
  }

  if (found==0)
    return 0;
  else
    return c;
}

static unsigned char* Latin1_map = NULL;
static unsigned char* Latin1_map_nocase = NULL;

static void iStrInitLatin1_map(void)
{
  static unsigned char map[256];
  static unsigned char map_nocase[256];

  Latin1_map = map;
  Latin1_map_nocase = map_nocase;

#define M(_x) (map[(unsigned char)_x])

  /* these characters are sorted in the same order as Excel would sort them */

  M(  0)=  0;  M(  1)=  1; M(  2)=  2; M(  3)=  3; M(  4)=  4; M(  5)=  5; M(  6)=  6; M(  7)=  7;  M(  8)=  8; M(  9)=  9; M( 10)= 10; M( 11)= 11; M( 12)= 12; M( 13)= 13; M( 14)= 14; M( 15)= 15; 
  M( 16)= 16;  M( 17)= 17; M( 18)= 18; M( 19)= 19; M( 20)= 20; M( 21)= 21; M( 22)= 22; M( 23)= 23;  M( 24)= 24; M( 25)= 25; M( 26)= 26; M( 27)= 27; M( 28)= 28; M( 29)= 29; M( 30)= 30; M( 31)= 31; 
  M('\'')= 32; M('-')= 33; M('–')= 34; M('—')= 35; M(' ')= 36; M('!')= 37; M('"')= 38; M('#')= 39;  M('$')= 40; M('%')= 41; M('&')= 42; M('(')= 43; M(')')= 44; M('*')= 45; M(',')= 46; M('.')= 47; 
  M('/')= 48;  M(':')= 49; M(';')= 50; M('?')= 51; M('@')= 52; M('(')= 53; M(')')= 54; M('^')= 55;  M('ˆ')= 56; M('_')= 57; M('`')= 58; M('{')= 59; M('|')= 60; M('}')= 61; M('~')= 62; M('¡')= 63; 
  M('¦')= 64;  M('¨')= 65; M('¯')= 66; M('´')= 67; M('¸')= 68; M('¿')= 69; M('˜')= 70; M('‘')= 71;  M('’')= 72; M('‚')= 73; M('“')= 74; M('”')= 75; M('„')= 76; M('‹')= 77; M('›')= 78; M('¢')= 79; 
  M('£')= 80;  M('¤')= 81; M('¥')= 82; M('€')= 83; M('+')= 84; M('<')= 85; M('=')= 86; M('>')= 87;  M('±')= 88; M('«')= 89; M('»')= 90; M('×')= 91; M('÷')= 92; M('§')= 93; M('©')= 94; M('¬')= 95; 
  M('®')= 96;  M('°')= 97; M('µ')= 98; M('¶')= 99; M('…')=100; M('†')=101; M('‡')=102; M('•')=103;  M('•')=104; M('‰')=105; M('0')=106; M('¼')=107; M('½')=108; M('¾')=109; M('1')=110; M('¹')=111; 
  M('2')=112;  M('²')=113; M('3')=114; M('³')=115; M('4')=116; M('5')=117; M('6')=118; M('7')=119;  M('8')=120; M('9')=121; M('a')=122; M('A')=123; M('ª')=124; M('á')=125; M('Á')=126; M('à')=127; 
  M('À')=128;  M('â')=129; M('Â')=130; M('ä')=131; M('Ä')=132; M('ã')=133; M('Ã')=134; M('å')=135;  M('Å')=136; M('æ')=137; M('Æ')=138; M('b')=139; M('B')=140; M('c')=141; M('C')=142; M('ç')=143; 
  M('Ç')=144;  M('d')=145; M('D')=146; M('ð')=147; M('Ð')=148; M('e')=149; M('E')=150; M('é')=151;  M('É')=152; M('è')=153; M('È')=154; M('ê')=155; M('Ê')=156; M('ë')=157; M('Ë')=158; M('f')=159; 
  M('F')=160;  M('ƒ')=161; M('g')=162; M('G')=163; M('h')=164; M('H')=165; M('i')=166; M('I')=167;  M('í')=168; M('Í')=169; M('ì')=170; M('Ì')=171; M('î')=172; M('Î')=173; M('ï')=174; M('Ï')=175; 
  M('j')=176;  M('J')=177; M('k')=178; M('K')=179; M('l')=180; M('L')=181; M('m')=182; M('M')=183;  M('n')=184; M('N')=185; M('ñ')=186; M('Ñ')=187; M('o')=188; M('O')=189; M('º')=190; M('ó')=191; 
  M('Ó')=192;  M('ò')=193; M('Ò')=194; M('ô')=195; M('Ô')=196; M('ö')=197; M('Ö')=198; M('õ')=199;  M('Õ')=200; M('ø')=201; M('Ø')=202; M('œ')=203; M('Œ')=204; M('p')=205; M('P')=206; M('q')=207; 
  M('Q')=208;  M('r')=209; M('R')=210; M('s')=211; M('S')=212; M('š')=213; M('Š')=214; M('ß')=215;  M('t')=216; M('T')=217; M('þ')=218; M('Þ')=219; M('™')=220; M('u')=221; M('U')=222; M('ú')=223; 
  M('Ú')=224;  M('ù')=225; M('Ù')=226; M('û')=227; M('Û')=228; M('ü')=229; M('Ü')=230; M('v')=231;  M('V')=232; M('w')=233; M('W')=234; M('x')=235; M('X')=236; M('y')=237; M('Y')=238; M('ý')=239; 
  M('Ý')=240;  M('ÿ')=241; M('Ÿ')=242; M('z')=243; M('Z')=244; M('ž')=245; M('Ž')=246; M('\\')=247; M(127)=248; M(129)=249; M(141)=250; M(143)=251; M(144)=252; M(157)=253; M(160)=254; M(173)=255; 

#undef M

#define M(_x) (map_nocase[(unsigned char)_x])

  /* here case differences use the same code */
  M(  0)=  0;  M(  1)=  1; M(  2)=  2; M(  3)=  3; M(  4)=  4; M(  5)=  5; M(  6)=  6; M(  7)=  7;  M(  8)=  8; M(  9)=  9; M( 10)= 10; M( 11)= 11; M( 12)= 12; M( 13)= 13; M( 14)= 14; M( 15)= 15; 
  M( 16)= 16;  M( 17)= 17; M( 18)= 18; M( 19)= 19; M( 20)= 20; M( 21)= 21; M( 22)= 22; M( 23)= 23;  M( 24)= 24; M( 25)= 25; M( 26)= 26; M( 27)= 27; M( 28)= 28; M( 29)= 29; M( 30)= 30; M( 31)= 31; 
  M('\'')= 32; M('-')= 33; M('–')= 34; M('—')= 35; M(' ')= 36; M('!')= 37; M('"')= 38; M('#')= 39;  M('$')= 40; M('%')= 41; M('&')= 42; M('(')= 43; M(')')= 44; M('*')= 45; M(',')= 46; M('.')= 47; 
  M('/')= 48;  M(':')= 49; M(';')= 50; M('?')= 51; M('@')= 52; M('(')= 53; M(')')= 54; M('^')= 55;  M('ˆ')= 56; M('_')= 57; M('`')= 58; M('{')= 59; M('|')= 60; M('}')= 61; M('~')= 62; M('¡')= 63; 
  M('¦')= 64;  M('¨')= 65; M('¯')= 66; M('´')= 67; M('¸')= 68; M('¿')= 69; M('˜')= 70; M('‘')= 71;  M('’')= 72; M('‚')= 73; M('“')= 74; M('”')= 75; M('„')= 76; M('‹')= 77; M('›')= 78; M('¢')= 79; 
  M('£')= 80;  M('¤')= 81; M('¥')= 82; M('€')= 83; M('+')= 84; M('<')= 85; M('=')= 86; M('>')= 87;  M('±')= 88; M('«')= 89; M('»')= 90; M('×')= 91; M('÷')= 92; M('§')= 93; M('©')= 94; M('¬')= 95; 
  M('®')= 96;  M('°')= 97; M('µ')= 98; M('¶')= 99; M('…')=100; M('†')=101; M('‡')=102; M('•')=103;  M('•')=104; M('‰')=105; M('0')=106; M('¼')=107; M('½')=108; M('¾')=109; M('1')=110; M('¹')=111; 
  M('2')=112;  M('²')=113; M('3')=114; M('³')=115; M('4')=116; M('5')=117; M('6')=118; M('7')=119;  M('8')=120; M('9')=121; M('a')=122; M('A')=122; M('ª')=124; M('á')=125; M('Á')=125; M('à')=127; 
  M('À')=127;  M('â')=129; M('Â')=129; M('ä')=131; M('Ä')=131; M('ã')=133; M('Ã')=133; M('å')=135;  M('Å')=135; M('æ')=137; M('Æ')=137; M('b')=139; M('B')=139; M('c')=141; M('C')=141; M('ç')=143; 
  M('Ç')=143;  M('d')=145; M('D')=145; M('ð')=147; M('Ð')=147; M('e')=149; M('E')=149; M('é')=151;  M('É')=151; M('è')=153; M('È')=153; M('ê')=155; M('Ê')=155; M('ë')=157; M('Ë')=157; M('f')=159; 
  M('F')=159;  M('ƒ')=161; M('g')=162; M('G')=162; M('h')=164; M('H')=164; M('i')=166; M('I')=166;  M('í')=168; M('Í')=168; M('ì')=170; M('Ì')=170; M('î')=172; M('Î')=172; M('ï')=174; M('Ï')=174; 
  M('j')=176;  M('J')=176; M('k')=178; M('K')=178; M('l')=180; M('L')=180; M('M')=182; M('M')=182;  M('n')=184; M('N')=184; M('ñ')=186; M('Ñ')=186; M('o')=188; M('O')=188; M('º')=190; M('ó')=191; 
  M('Ó')=191;  M('ò')=193; M('Ò')=194; M('ô')=195; M('Ô')=196; M('ö')=197; M('Ö')=198; M('õ')=199;  M('Õ')=200; M('ø')=201; M('Ø')=202; M('œ')=203; M('Œ')=204; M('p')=205; M('P')=206; M('q')=207; 
  M('Q')=208;  M('r')=209; M('R')=209; M('s')=211; M('S')=211; M('š')=213; M('Š')=213; M('ß')=215;  M('t')=216; M('T')=216; M('þ')=218; M('Þ')=218; M('™')=220; M('u')=221; M('U')=221; M('ú')=223; 
  M('Ú')=223;  M('ù')=225; M('Ù')=225; M('û')=227; M('Û')=227; M('ü')=229; M('Ü')=229; M('v')=231;  M('V')=231; M('w')=233; M('W')=233; M('x')=235; M('X')=235; M('y')=237; M('Y')=237; M('ý')=239; 
  M('Ý')=239;  M('ÿ')=241; M('Ÿ')=241; M('z')=243; M('Z')=243; M('ž')=245; M('Ž')=245; M('\\')=247; M(127)=248; M(129)=249; M(141)=250; M(143)=251; M(144)=252; M(157)=253; M(160)=254; M(173)=255; 

#undef M
}

static char iStrUTF8toLatin1(const char* *l)
{
  char c = **l;

  if (c >= 0) 
    return c;   /* ASCII */

  if ((c & 0x20) == 0)       /* Use 00100000 to detect 110XXXXX */
  {
    short u;
    u  = (c & 0x1F) << 6;    /* first part + make room for second part */
    (*l)++;
    c = **l;
    u |= (c & 0x3F);         /* second part (10XXXXXX) */
    if (u >= -128 && u < 128)
      return (char)u;
    else
      return 0;
  }

  /* only increment the pointer for the remaining codes */
  if ((c & 0x10) == 0)       /* Use 00010000 to detect 1110XXXX */
    *l += 3-1;  
  else if ((c & 0x08) == 0)  /* Use 00001000 to detect 11110XXX */
    *l += 4-1;

  return 0;
}

/*
The Alphanum Algorithm is an improved sorting algorithm for strings
containing numbers.  Instead of sorting numbers in ASCII order like a
standard sort, this algorithm sorts numbers in numeric order.

The Alphanum Algorithm is discussed at http://www.DaveKoelle.com/alphanum.html

This implementation is Copyright (c) 2008 Dirk Jagdmann <doj@cubic.org>.
It is a cleanroom implementation of the algorithm and not derived by
other's works. In contrast to the versions written by Dave Koelle this
source code is distributed with the libpng/zlib license.

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you
       must not claim that you wrote the original software. If you use
       this software in a product, an acknowledgment in the product
       documentation would be appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and
       must not be misrepresented as being the original software.

    3. This notice may not be removed or altered from any source
       distribution.

***************************************************************************

The following code is based on the "alphanum.hpp" code 
downloaded from the Dave Koelle page and implemented by Dirk Jagdmann.

It was modified to the C language and simplified to IUP needs.
*/

int iupStrCompare(const char *l, const char *r, int casesensitive, int utf8)
{
  enum mode_t { STRING, NUMBER } mode=STRING;

  if (!l || !r)
    return 0;

  if (!Latin1_map)
    iStrInitLatin1_map();

  while(*l && *r)
  {
    if (mode == STRING)
    {
      while((*l) && (*r))
      {
        int diff;
        char l_char = *l, 
             r_char = *r;

        /* check if this are digit characters */
        int l_digit = iup_isdigit(l_char), 
            r_digit = iup_isdigit(r_char);

        /* if both characters are digits, we continue in NUMBER mode */
        if(l_digit && r_digit)
        {
          mode = NUMBER;
          break;
        }

        /* if only the left character is a digit, we have a result */
        if(l_digit) return -1;

        /* if only the right character is a digit, we have a result */
        if(r_digit) return +1;

        if (utf8)
        {
          l_char = iStrUTF8toLatin1(&l);  /* increment n-1 an utf8 character */
          r_char = iStrUTF8toLatin1(&r);
        }

        /* compute the difference of both characters */
        if (casesensitive)
          diff = Latin1_map[(unsigned char)l_char] - Latin1_map[(unsigned char)r_char];
        else
          diff = Latin1_map_nocase[(unsigned char)l_char] - Latin1_map_nocase[(unsigned char)r_char];

        /* if they differ we have a result */
        if(diff != 0) return diff;

        /* otherwise process the next characters */
        ++l;
        ++r;
      }
    }
    else /* mode==NUMBER */
    {
      unsigned long r_int;
      long diff;

      /* get the left number */
      unsigned long l_int=0;
      while(*l && iup_isdigit(*l))
      {
        /* TODO: this can overflow */
        l_int = l_int*10 + *l-'0';
        ++l;
      }

      /* get the right number */
      r_int=0;
      while(*r && iup_isdigit(*r))
      {
        /* TODO: this can overflow */
        r_int = r_int*10 + *r-'0';
        ++r;
      }

      /* if the difference is not equal to zero, we have a comparison result */
      diff = l_int-r_int;
      if (diff != 0)
        return (int)diff;

      /* otherwise we process the next substring in STRING mode */
      mode=STRING;
    }
  }

  if (*r) return -1;
  if (*l) return +1;
  return 0;
}

int iupStrCompareEqual(const char *l, const char *r, int casesensitive, int utf8, int partial)
{
  if (!l || !r)
    return 0;

  if (!Latin1_map)
    iStrInitLatin1_map();

  while(*l && *r)
  {
    int diff;
    char l_char = *l, 
         r_char = *r;

    if (utf8)
    {
      l_char = iStrUTF8toLatin1(&l);  /* increment n-1 an utf8 character */
      r_char = iStrUTF8toLatin1(&r);
    }

    /* compute the difference of both characters */
    if (casesensitive)
      diff = l_char - r_char;
    else
      diff = Latin1_map_nocase[(unsigned char)l_char] - Latin1_map_nocase[(unsigned char)r_char];

    /* if they differ we have a result */
    if(diff != 0) 
      return 0;

    /* otherwise process the next characters */
    ++l;
    ++r;
  }

  /* check also for terminator */
  if (*l == *r) 
    return 1;

  if (partial && *r == 0) 
    return 1;  /* if second string is at terminator, then it is partially equal */

  return 0;
}

static int iStrIncUTF8(const char* str)
{
  if (*str >= 0)      /* ASCII */
    return 1;  
  else if ((*str & 0x20) == 0)  /* Use 00100000 to detect 110XXXXX */
    return 2;  
  else if ((*str & 0x10) == 0)  /* Use 00010000 to detect 1110XXXX */
    return 3;  
  else if ((*str & 0x08) == 0)  /* Use 00001000 to detect 11110XXX */
    return 4;
  return 1;
}

int iupStrCompareFind(const char *l, const char *r, int casesensitive, int utf8)
{
  int i, inc, l_len, r_len, count;

  if (!l || !r)
    return 0;

  l_len = strlen(l);
  r_len = strlen(r);
  count = l_len - r_len;
  if (count < 0)
    return 0;

  count++;

  for (i=0; i<count; i++)
  {
    if (iupStrCompareEqual(l, r, casesensitive, utf8, 1))
      return 1;

    if (utf8)
    {
      inc = iStrIncUTF8(l);
      l += inc;
      i += inc-1;
    }
    else
      l++;
  }

  return 0;
}

static void iStrFixPosUTF8(const char* str, int *start, int *end)
{
  int p = 0, i = 0, find = 0, inc;
  while (*(str + i))
  {
    if (find == 0 && p == *start)
    {
      *start = i;
      find = 1;
    }
    if (find == 1 && p == *end)
    {
      *end = i;
      return;
    }

    inc = iStrIncUTF8(str + i);
    i += inc;
    p++;
  }

  if (find == 0 && p == *start)
  {
    *start = i;
    find = 1;
  }

  if (find == 1 && p == *end)
    *end = i;
}

void iupStrRemove(char* str, int start, int end, int dir, int utf8)
{
  int len;

  if (end < start || !str || str[0] == 0)
    return;

  if (start == end)
  {
    if (dir == 1)  /* (forward) */
      end++;
    else  /* dir==-1 (backward) */
    {
      if (start == 0) /* there is nothing to remove before */
        return;
      else
        start--;
    }
  }

  if (utf8)
    iStrFixPosUTF8(str, &start, &end);

  /* from "start" remove up to "end", but not including "end" */
  len = strlen(str);
  if (start >= len) { start = len - 1; end = len; }
  if (end > len) end = len;

  memmove(str + start, str + end, len - end + 1);
}

char* iupStrInsert(const char* str, const char* insert_str, int start, int end, int utf8)
{
  char* new_str = (char*)str;
  int insert_len, len;

  if (!str || !insert_str)
    return NULL;

  insert_len = strlen(insert_str);
  len = strlen(str);

  if (utf8)
    iStrFixPosUTF8(str, &start, &end);

  if (end == start || insert_len > end - start)
  {
    new_str = malloc(len - (end - start) + insert_len + 1);
    memcpy(new_str, str, start);
    memcpy(new_str + start, insert_str, insert_len);
    memcpy(new_str + start + insert_len, str + end, len - end + 1);
  }
  else
  {
    memcpy(new_str + start, insert_str, insert_len);
    memcpy(new_str + start + insert_len, str + end, len - end + 1);
  }

  return new_str;
}

int iupStrIsAscii(const char* str)
{
  if (!str)
    return 0;

  while (*str)
  {
    int c = *str;
    if (c < 0)
      return 0;
    str++;
  }
  return 1;
}

static char* iStrSetLocale(const char* decimal_symbol)
{
  if (decimal_symbol)
  {
    struct lconv* locale_info = localeconv();
    if (locale_info->decimal_point[0] != decimal_symbol[0])
    {
      char* old_locale = setlocale(LC_NUMERIC, NULL);

      if (decimal_symbol[0] == '.')
      {
        old_locale = iupStrDup(old_locale);  /* must be before another setlocale */
        setlocale(LC_NUMERIC, "en-US");
        return old_locale;
      }
      else if (decimal_symbol[0] == ',')
      {
        old_locale = iupStrDup(old_locale);  /* must be before another setlocale */
        setlocale(LC_NUMERIC, "pt-BR");
        return old_locale;
      }
    }
  }

  return NULL;
}

static void iStrResetLocale(char* old_locale)
{
  if (old_locale)
  {
    setlocale(LC_NUMERIC, old_locale);
    free(old_locale);
  }
}

int iupStrToDoubleLocale(const char *str, double *d, const char* decimal_symbol)
{
  int ret, locale_set = 0;
  char* old_locale;

  if (!str) 
    return 0;

  old_locale = iStrSetLocale(decimal_symbol);
  if (old_locale) locale_set = 1;

  ret = sscanf(str, "%lf", d);

  iStrResetLocale(old_locale);

  if (ret != 1) 
    return 0;

  if (locale_set)
    return 2;
  else
    return 1;
}

void iupStrPrintfDoubleLocale(char *str, const char *format, double d, const char* decimal_symbol)
{
  char* old_locale = iStrSetLocale(decimal_symbol);

  sprintf(str, format, d);

  iStrResetLocale(old_locale);
}
