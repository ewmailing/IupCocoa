/** \file
 * \brief IULUA core - Bindig of iup to Lua 5.
 *
 * See Copyright Notice in "iup.h"
 */

#include <im.h>
#include <im_image.h>
 
#include "iup.h"
#include "iupim.h"

#include <lua.h>
#include <lauxlib.h>

#include "iuplua.h"
#include "iupluaim.h"
#include "imlua.h"
#include "il.h"


static int GetNativeHandleImage(lua_State *L)
{
  void* native_handle;
  imImage* im_image;
  luaL_checktype (L, 1, LUA_TLIGHTUSERDATA);
  native_handle = lua_touserdata(L, 1);
  im_image = IupGetNativeHandleImage(native_handle);
  imlua_pushimage(L, im_image);
  return 1;
}

static int GetImageNativeHandle(lua_State *L)
{
  imImage* im_image = imlua_checkimage(L, 1);
  void* native_handle = IupGetImageNativeHandle(im_image);
  lua_pushlightuserdata(L, native_handle);
  return 1;
}

static int ImageFromImImage(lua_State *L)
{
  imImage* im_image = imlua_checkimage(L, 1);
  Ihandle* image = IupImageFromImImage(im_image);
  iuplua_pushihandle(L, image);
  return 1;
}

static int SaveImage(lua_State *L)
{
  Ihandle *image = iuplua_checkihandle(L,1);
  const char *filename = luaL_checkstring(L, 2);
  const char *format = luaL_checkstring(L, 3);
  lua_pushboolean(L, IupSaveImage(image, filename, format));
  return 1;
}

static int LoadImage(lua_State *L)
{
  const char *filename = luaL_checkstring(L, 1);
  Ihandle* image = IupLoadImage(filename);
  iuplua_pushihandle(L, image);
  return 1;
}

static int LoadAnimation(lua_State *L)
{
  const char *filename = luaL_checkstring(L, 1);
  Ihandle* animation = IupLoadAnimation(filename);
  iuplua_pushihandle(L, animation);
  return 1;
}

static int LoadAnimationFrames(lua_State *L)
{
  int file_count = (int)luaL_checkinteger(L, 2);
  char** filename_list = iuplua_checkstring_array(L, 1, file_count);
  Ihandle* animation = IupLoadAnimationFrames(filename_list, file_count);
  iuplua_pushihandle(L, animation);
  return 1;
}

int iupimlua_open(lua_State *L)
{
  iuplua_get_env(L);
  iuplua_register(L, LoadImage, "LoadImage");
  iuplua_register(L, SaveImage, "SaveImage");
  iuplua_register(L, LoadAnimation, "LoadAnimation");
  iuplua_register(L, LoadAnimationFrames, "LoadAnimationFrames");
  iuplua_register(L, GetNativeHandleImage, "GetNativeHandleImage");
  iuplua_register(L, GetImageNativeHandle, "GetImageNativeHandle");
  iuplua_register(L, ImageFromImImage, "ImageFromImImage");
  return 0; /* nothing in stack */
}

/* obligatory to use require"iupluaim" */
int luaopen_iupluaim(lua_State* L)
{
  return iupimlua_open(L);
}

