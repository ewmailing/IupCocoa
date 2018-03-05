
#include "simple_paint_util.h"

#include <string.h>
#include <ctype.h>

#include <iup.h>
#include <cd.h>
#include <im_process.h>
#include <im_lib.h>


char* str_duplicate(const char* str)
{
  if (!str)
    return NULL;

  int size = (int)strlen(str);
  char* new_str = new char[size+1];
  memcpy(new_str, str, size + 1);
  return new_str;
}

char* str_filepath(const char *filename)
{
  /* Start at the last character */
  int len = (int)strlen(filename);
  int offset = len - 1;
  while (offset != 0)
  {
    if (filename[offset] == '\\' || filename[offset] == '/')
      break;

    offset--;
  }

  if (offset == 0)
    return NULL;
  else
  {
    char* file_path = str_duplicate(filename);
    file_path[offset] = 0;
    return file_path;
  }
}

const char* str_filetitle(const char *filename)
{
  /* Start at the last character */
  int len = (int)strlen(filename);
  int offset = len - 1;
  while (offset != 0)
  {
    if (filename[offset] == '\\' || filename[offset] == '/')
    {
      offset++;
      break;
    }
    offset--;
  }
  return filename + offset;
}

const char* str_fileext(const char *filename)
{
  /* Start at the last character */
  int len = (int)strlen(filename);
  int offset = len - 1;
  while (offset != 0)
  {
    if (filename[offset] == '\\' || filename[offset] == '/')
      break;

    if (filename[offset] == '.')
    {
      offset++;
      return filename + offset;
    }
    offset--;
  }
  return NULL;
}

int str_compare(const char *l, const char *r, int casesensitive)
{
  if (!l || !r)
    return 0;

  while (*l && *r)
  {
    int diff;
    char l_char = *l,
      r_char = *r;

    /* compute the difference of both characters */
    if (casesensitive)
      diff = l_char - r_char;
    else
      diff = tolower((int)l_char) - tolower((int)r_char);

    /* if they differ we have a result */
    if (diff != 0)
      return 0;

    /* otherwise process the next characters */
    ++l;
    ++r;
  }

  /* check also for terminator */
  if (*l == *r)
    return 1;

  if (*r == 0)
    return 1;  /* if second string is at terminator, then it is partially equal */

  return 0;
}


/********************************************************************/


void show_file_error(int error)
{
  switch (error)
  {
  case IM_ERR_OPEN:
    IupMessageError(NULL, "Error Opening File.");
    break;
  case IM_ERR_MEM:
    IupMessageError(NULL, "Insufficient memory.");
    break;
  case IM_ERR_ACCESS:
    IupMessageError(NULL, "Error Accessing File.");
    break;
  case IM_ERR_DATA:
    IupMessageError(NULL, "Image type not Supported.");
    break;
  case IM_ERR_FORMAT:
    IupMessageError(NULL, "Invalid Format.");
    break;
  case IM_ERR_COMPRESS:
    IupMessageError(NULL, "Invalid or unsupported compression.");
    break;
  default:
    IupMessageError(NULL, "Unknown Error.");
  }
}

void image_flood_fill(imImage* image, int start_x, int start_y, long replace_color, double tol_percent)
{
#if IM_VERSION_NUMBER > 312000
  double color[3];
  double tol;
#else
  float color[3];
  float tol;
#endif

  color[0] = cdRed(replace_color);
  color[1] = cdGreen(replace_color);
  color[2] = cdBlue(replace_color);

  /* max value = 255*255*3 = 195075 */
  /* sqrt(195075) = 441 */
  tol = (441 * tol_percent) / 100;

  /* still too high */
  tol = tol / 5;  /* empirical reduce. TODO: What is the best formula? */

  imProcessRenderFloodFill(image, start_x, start_y, color, tol);
}
