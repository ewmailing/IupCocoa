/** \file
 * \brief Bindig of iupgl to Lua 3.
 *
 * See Copyright Notice in "iup.h"
 */
 
#include <stdlib.h>

#include <lua.h>
#include <lauxlib.h>

#include "iup.h"
#include "iupgl.h"

#include "iuplua.h"
#include "iupluagl.h"
#include "il.h"

#ifdef _WIN32
#include <windows.h>
#endif
#if defined (__APPLE__) || defined (OSX)
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif


static void createglcanvas(void)
{
  int tag = (int)lua_getnumber(lua_getglobal("iuplua_tag"));
  lua_pushusertag(IupGLCanvas(NULL), tag);
}

static void iscurrent(void)
{
  lua_pushnumber(IupGLIsCurrent(iuplua_checkihandle(1))); 
}

static void makecurrent(void)
{
  IupGLMakeCurrent(iuplua_checkihandle(1)); 

  iuplua_regstring_mask((char*)glGetString(GL_VENDOR), "IUPGL_VENDOR");
  iuplua_regstring_mask((char*)glGetString(GL_RENDERER), "IUPGL_RENDERER");
  iuplua_regstring_mask((char*)glGetString(GL_VERSION), "IUPGL_VERSION");
}

static void swapbuffers(void)
{
  IupGLSwapBuffers(iuplua_checkihandle(1)); 
}

static void palette(void)
{
  IupGLPalette(iuplua_checkihandle(1),
               luaL_check_int(2),
               (float)luaL_check_number(3),
               (float)luaL_check_number(4),
               (float)luaL_check_number(5));
}

static void usefont(void)
{
  IupGLUseFont(iuplua_checkihandle(1),
               luaL_check_int(2),
               luaL_check_int(3),
               luaL_check_int(4));
}

static void glwait(void)
{
  IupGLWait(luaL_check_int(1));
}

int iupgllua_open (void)
{
  iuplua_register("iupCreateGLCanvas",createglcanvas);
  iuplua_register("IupGLMakeCurrent",makecurrent);
  iuplua_register("IupGLIsCurrent",iscurrent);
  iuplua_register("IupGLSwapBuffers",swapbuffers);
  iuplua_register("IupGLPalette",palette);
  iuplua_register("IupGLUseFont",usefont);
  iuplua_register("IupGLWait",glwait);

  iuplua_regstring("BUFFER", "IUP_BUFFER");
  iuplua_regstring("STEREO", "IUP_STEREO");
  iuplua_regstring("COLOR", "IUP_COLOR");
  iuplua_regstring("BUFFER_SIZE", "IUP_BUFFER_SIZE");
  iuplua_regstring("RED_SIZE", "IUP_RED_SIZE");
  iuplua_regstring("GREEN_SIZE", "IUP_GREEN_SIZE");
  iuplua_regstring("BLUE_SIZE", "IUP_BLUE_SIZE");
  iuplua_regstring("ALPHA_SIZE", "IUP_ALPHA_SIZE");
  iuplua_regstring("DEPTH_SIZE", "IUP_DEPTH_SIZE");
  iuplua_regstring("STENCIL_SIZE", "IUP_STENCIL_SIZE");
  iuplua_regstring("ACCUM_RED_SIZE", "IUP_ACCUM_RED_SIZE");
  iuplua_regstring("ACCUM_GREEN_SIZE", "IUP_ACCUM_GREEN_SIZE");
  iuplua_regstring("ACCUM_BLUE_SIZE", "IUP_ACCUM_BLUE_SIZE");
  iuplua_regstring("ACCUM_ALPHA_SIZE", "IUP_ACCUM_ALPHA_SIZE");
  iuplua_regstring("DOUBLE", "IUP_DOUBLE");
  iuplua_regstring("SINGLE", "IUP_SINGLE");
  iuplua_regstring("INDEX", "IUP_INDEX");
  iuplua_regstring("RGBA", "IUP_RGBA");
  iuplua_regstring("YES", "IUP_YES");
  iuplua_regstring("NO", "IUP_NO");

  return lua_dostring("IUPGLCANVAS = {parent = IUPCANVAS}\n"
                      "function IUPGLCANVAS:CreateIUPelement (obj)\n"
                      "  return iupCreateGLCanvas ()\n"
                      "end\n"
                      "function iupglcanvas (o)\n"
                      "  return IUPGLCANVAS:Constructor (o)\n"
                      "end\n"
                      "iup.glcanvas = iupglcanvas\n"
                      "iup_callbacks.action.glcanvas = iup_action_canvas\n");
}
