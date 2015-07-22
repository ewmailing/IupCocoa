/** \file
 * \brief Iup API in Lua
 *
 * See Copyright Notice in "iup.h"
 */
 
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <lua.h>
#include <lauxlib.h>

#include "iup.h"

#include "iuplua.h"
#include "il.h"

#include "iup_str.h"
#include "iup_predialogs.h"

#define ALLOC(n,t)  ((t *)calloc((n),sizeof(t)))
#define REQUIRE(b)  {if (!(b)) goto cleanup;}

void iupluaScanf(void)
{
  char *format;
  int i;
  int fields;
  int *width = NULL;
  int *scroll = NULL;
  char **prompt = NULL;
  char **text = NULL;
  char *title = NULL;
  char *s = NULL;
  char *s1 = NULL;
  char *outf = NULL;
  int indParam;                /* va_list va; */

  format = luaL_check_string(1);
  fields = iupStrCountChar(format, '\n') - 1;
  REQUIRE(fields > 0);
  width = ALLOC(fields, int);
  REQUIRE(width != NULL);
  scroll = ALLOC(fields, int);
  REQUIRE(scroll != NULL);
  prompt = ALLOC(fields, char *);
  REQUIRE(prompt != NULL);
  text = ALLOC(fields, char *);
  REQUIRE(text != NULL);

  indParam = 2;                /* va_start(va,format); */
  REQUIRE((s1 = s = (char *) iupStrDup(format)) != NULL);
  title = iupStrDupUntil(&s, '\n');
  REQUIRE(title != NULL);
  for (i = 0; i < fields; ++i) {
    int n;
    prompt[i] = iupStrDupUntil(&s, '%');
    REQUIRE(prompt[i] != NULL);
    n = sscanf(s, "%d.%d", width + i, scroll + i);
    REQUIRE(n == 2);
    s = strchr(s, '%');
    REQUIRE(s != NULL);
    if (outf) free(outf);
    outf = iupStrDupUntil(&s, '\n');
    text[i] = ALLOC(width[i] + 1, char);
    REQUIRE(text[i] != NULL);

    switch (s[-2]) {
        case 'd':
        case 'i':
        case 'o':
        case 'u':
        case 'x':
        case 'X':
          if (s[-3] == 'l')
            sprintf(text[i], outf, luaL_check_long(indParam++));
          else if (s[-3] == 'h')
            sprintf(text[i], outf, (short)luaL_check_int(indParam++));
          else
            sprintf(text[i], outf, luaL_check_int(indParam++));
          break;
        case 'e':
        case 'f':
        case 'g':
        case 'E':
        case 'G':
          if (s[-3] == 'l')
            sprintf(text[i], outf, luaL_check_number(indParam++));
          else
            sprintf(text[i], outf, (float)luaL_check_number(indParam++));
          break;
        case 's':
          sprintf(text[i], outf, (char *)luaL_check_string(indParam++));
          break;
        default:
          goto cleanup;
    }
  }
  /* va_end(va); */

  REQUIRE(iupDataEntry(fields, width, scroll, title, prompt, text)>0);

  /* va_start(va,format); */
  s = strchr(format, '\n') + 1;
  for (i = 0; i < fields; ++i) {
    s = strchr(s, '\n') + 1;
    switch (s[-2]) {
        case 'd':
        case 'u':
          if (s[-3] == 'l') {
            long l = 0;
            sscanf(text[i], "%ld", &l);
            lua_pushnumber(l);
          } else if (s[-3] == 'h') {
            short l = 0;
            sscanf(text[i], "%hd", &l);
            lua_pushnumber(l);
          } else {
            int l = 0;
            sscanf(text[i], "%d", &l);
            lua_pushnumber(l);
          }
          break;
        case 'i':
        case 'o':
        case 'x':
        case 'X':
          if (s[-3] == 'l') {
            long l = 0;
            sscanf(text[i], "%li", &l);
            lua_pushnumber(l);
          } else if (s[-3] == 'h') {
            short l = 0;
            sscanf(text[i], "%hi", &l);
            lua_pushnumber(l);
          } else {
            int l = 0;
            sscanf(text[i], "%i", &l);
            lua_pushnumber(l);
          }
          break;
        case 'e':
        case 'f':
        case 'g':
        case 'E':
        case 'G':
          if (s[-3] == 'l') {
            double l = 0;
            sscanf(text[i], "%lg", &l);
            lua_pushnumber(l);
          } else {
            float l = 0;
            sscanf(text[i], "%g", &l);
            lua_pushnumber(l);
          }
          break;
        case 's':
          lua_pushstring(text[i]);
          break;
    }
  }
  /* va_end(va); */

cleanup:
  if (s1) free(s1);
  if (title) free(title);
  if (width) free(width);
  if (scroll) free(scroll);
  if (outf) free(outf);
  if (prompt) {
    for (i = 0; i < fields; ++i)
      if (prompt[i]) free(prompt[i]);
    free(prompt);
  }
  if (text) {
    for (i = 0; i < fields; ++i)
      if (text[i]) free(text[i]);
    free(text);
  }
}
