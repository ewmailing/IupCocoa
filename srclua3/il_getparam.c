/** \file
 * \brief Bindig of iupgetparam to Lua 3.
 *
 * See Copyright Notice in "iup.h"
 */
 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <lua.h>
#include <lauxlib.h>

#include "iup.h"

#include "iuplua.h"
#include "il.h"
#include "il_controls.h"


/* Used only by the Lua binding */
int iupGetParamCount(const char *format, int *param_extra);
char iupGetParamType(const char* format, int *line_size);


typedef struct _getparam_data
{
  int has_func;
  int func_ref;
}getparam_data;

static int param_action(Ihandle* dialog, int param_index, void* user_data)
{
  int ret = 1;
  getparam_data* gp = (getparam_data*)user_data;
  if (gp->has_func)
  {
    lua_Object func;
    lua_beginblock();
    func = lua_getref(gp->func_ref);
    iuplua_pushihandle(dialog);
    lua_pushnumber(param_index);
    lua_callfunction(func);
    ret = (int)luaL_check_number(1);
    lua_endblock();
  }
  return ret;
}

static void GetParam(void)
{
  getparam_data gp;
  lua_Object func;
  const char* title = luaL_check_string(1);
  void* user_data = (void*)&gp;
  const char* format = luaL_check_string(3);
  int param_count, param_extra, i, size, ret,
      line_size = 0, lua_param_start = 4;
  const char* f = format;
  const char* s;
  void* param_data[50];
  char param_type[50];

  gp.has_func = 0;
  gp.func_ref = 0;

  memset(param_data, 0, sizeof(void*)*50);
  memset(param_type, 0, sizeof(char)*50);

  param_count = iupGetParamCount(format, &param_extra);

  for (i = 0; i < param_count; i++)
  {
    char t = iupGetParamType(f, &line_size);

    if (t == 't') /* if separator */
    {
      f += line_size;
      i--; /* compensate next increment */
      continue;
    }

    switch(t)
    {
    case 'b':
    case 'i':
    case 'l':
      param_data[i] = malloc(sizeof(int));
      *(int*)(param_data[i]) = (int)luaL_check_number(lua_param_start); lua_param_start++;
      break;
    case 'a':
    case 'r':
      param_data[i] = malloc(sizeof(float));
      *(float*)(param_data[i]) = (float)luaL_check_number(lua_param_start); lua_param_start++;
      break;
    case 'f':
    case 'c':
    case 's':
    case 'm':
      s = luaL_check_string(lua_param_start); lua_param_start++;
      size = strlen(s);
      if (size < 512)
        param_data[i] = malloc(512);
      else
        param_data[i] = malloc(2*size);
      memcpy(param_data[i], s, size+1);
      break;
    }

    param_type[i] = t;
    f += line_size;
  }

  func = lua_getparam(2);
  if (lua_isfunction(func))
  {
    lua_pushobject(func);
    gp.func_ref = lua_ref(1);
    gp.has_func = 1;
  }

  ret = IupGetParamv(title, param_action, user_data, format, param_count, param_extra, param_data);

  lua_pushnumber(ret);

  if (ret)
  {
    for (i = 0; i < param_count; i++)
    {
      switch(param_type[i])
      {
      case 'b':
      case 'i':
      case 'l':
        lua_pushnumber(*(int*)(param_data[i]));
        break;
      case 'a':
      case 'r':
        lua_pushnumber(*(float*)(param_data[i]));
        break;
      case 'f':
      case 'c':
      case 's':
      case 'm':
        lua_pushstring((char*)(param_data[i]));
        break;
      }
    }
  }

  for (i = 0; i < param_count; i++)
  {
    free(param_data[i]);
  }

  if (gp.has_func)
    lua_unref(gp.func_ref);
}

static void GetParamParam(void)
{
  Ihandle* param;
  Ihandle *dialog = iuplua_checkihandle(1);
  int param_index = luaL_check_int(2);
  char param_str[50];
  sprintf(param_str, "PARAM%d", param_index);
  param = (Ihandle*)IupGetAttribute(dialog, param_str);
  iuplua_pushihandle(param);
}

void getparamlua_open(void)
{
  iuplua_register("IupGetParam", GetParam);
  iuplua_register("IupGetParamParam", GetParamParam);
}
