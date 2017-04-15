/** \file
 * \brief GLSubCanvas Control.
 *
 * See Copyright Notice in "iup.h"
 */

#ifdef WIN32
#include <windows.h>
#endif

#if defined (__APPLE__) || defined (OSX)
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include <stdio.h>
#include <stdlib.h>

#include "iup.h"
#include "iupcbs.h"
#include "iupgl.h"

#include "iup_assert.h"
#include "iup_object.h"
#include "iup_attrib.h"
#include "iup_str.h"
#include "iup_layout.h"

#include "iup_glcontrols.h"
#include "iup_glfont.h"
#include "iup_glsubcanvas.h"


static void iGLFixRectY(Ihandle* gl_parent, int *y, int h)
{
  int border = iupAttribGetBoolean(gl_parent, "BORDER");

  /* y is at bottom and oriented bottom to top in OpenGL, both different from IUP */

  *y = *y + h - 1;  /* First, move to bottom */
  *y = gl_parent->currentheight - 2 * border - 1 - *y; /* Second, orient bottom to top */
}

static void iGLSaveClientClipping(Ihandle* ih, int clip_x, int clip_y, int clip_w, int clip_h)
{
  int x = 0, y = 0, w = 0, h = 0;

  IupGetIntInt(ih, "CLIENTSIZE", &w, &h);
  IupGetIntInt(ih, "CLIP_MIN", &x, &y);
  x += ih->x;
  y += ih->y;

  if (w <= 0 || h <= 0)
    return;

  if (x > clip_x + clip_w)
    return;
  if (y > clip_y + clip_h)
    return;
  if (x < clip_x)
    x = clip_x;
  if (y < clip_y)
    y = clip_y;

  if (x + w < clip_x)
    return;
  if (y + h < clip_y)
    return;
  if (x + w > clip_x + clip_w)
    w = clip_x + clip_w - x;
  if (y + h > clip_y + clip_h)
    h = clip_y + clip_h - y;

  if (w < 0) w = 0;
  if (h < 0) h = 0;

  iupAttribSetInt(ih, "CLIP_X", x);
  iupAttribSetInt(ih, "CLIP_Y", y);
  iupAttribSetInt(ih, "CLIP_W", w);
  iupAttribSetInt(ih, "CLIP_H", h);
}

static int iGLSetClipping(Ihandle* ih, int x, int y, int w, int h, Ihandle* gl_parent)
{
  int clip_x = iupAttribGetInt(ih->parent, "CLIP_X");
  int clip_y = iupAttribGetInt(ih->parent, "CLIP_Y");
  int clip_w = iupAttribGetInt(ih->parent, "CLIP_W");
  int clip_h = iupAttribGetInt(ih->parent, "CLIP_H");

  iupAttribSet(ih, "CLIP_W", "0");
  iupAttribSet(ih, "CLIP_H", "0");

  if (clip_w <= 0 || clip_h <= 0)
    return 0;
  if (w <= 0 || h <= 0)
    return 0;

  if (x > clip_x + clip_w)
    return 0;
  if (y > clip_y + clip_h)
    return 0;
  if (x < clip_x)
    x = clip_x;
  if (y < clip_y)
    y = clip_y;

  if (x + w < clip_x)
    return 0;
  if (y + h < clip_y)
    return 0;
  if (x + w > clip_x + clip_w)
    w = clip_x + clip_w - x;
  if (y + h > clip_y + clip_h)
    h = clip_y + clip_h - y;

  if (w < 0) w = 0;
  if (h < 0) h = 0;

  if (w == 0 || h == 0)
    return 0;

  if (ih->firstchild)
    iGLSaveClientClipping(ih, clip_x, clip_y, clip_w, clip_h);

  iGLFixRectY(gl_parent, &y, h);
  glScissor(x, y, w, h);
  return 1;
}

int iupGLSubCanvasSetTransform(Ihandle* ih, Ihandle* gl_parent)
{
  int x = ih->x;
  int y = ih->y;
  int w = ih->currentwidth;
  int h = ih->currentheight;

  /* crop to parent's rectangle */
  if (!iGLSetClipping(ih, x, y, w, h, gl_parent))
    return 0;

  iGLFixRectY(gl_parent, &y, h);
  glViewport(x, y, w, h);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, w, 0, h, -1, 1);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef(0.375, 0.375, 0.0);  /* render all primitives at integer positions */

  return 1;
}

void iupGLSubCanvasSaveState(Ihandle* gl_parent)
{
  int border = iupAttribGetBoolean(gl_parent, "BORDER");
  char* saved = iupAttribGet(gl_parent, "_IUP_GLSUBCANVAS_SAVED");
  iupASSERT(saved == NULL);
  if (saved)
    return;

  /* save all changes, except transformation matrix */
  glPushAttrib(GL_ENABLE_BIT | GL_HINT_BIT | GL_VIEWPORT_BIT);

  /* save transformation matrix */
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();

  /* save transformation matrix */
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();

  /* clipping */
  glEnable(GL_SCISSOR_TEST);
  /* alpha enabled */
  glEnable(GL_BLEND);     /* GL_COLOR_BUFFER_BIT or GL_ENABLE_BIT */
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  /* image data alignment */
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);  /* not saved */
  /* disable Z axis */
  glDisable(GL_DEPTH_TEST);  /* GL_DEPTH_BUFFER_BIT or GL_ENABLE_BIT */
  /* disable lighting */
  glDisable(GL_LIGHTING);
  /* anti-alias */
  glEnable(GL_POINT_SMOOTH);     /* GL_LINE_BIT or GL_ENABLE_BIT */
  glEnable(GL_LINE_SMOOTH);      /* GL_POINT_BIT or GL_ENABLE_BIT */
  glEnable(GL_POLYGON_SMOOTH);    /* GL_POLYGON_BIT or GL_ENABLE_BIT */
  glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);   /* GL_HINT_BIT */
  glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
  glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

  iupAttribSet(gl_parent, "_IUP_GLSUBCANVAS_SAVED", "1");

  iupAttribSetInt(gl_parent, "CLIP_X", 0);
  iupAttribSetInt(gl_parent, "CLIP_Y", 0);
  iupAttribSetInt(gl_parent, "CLIP_W", gl_parent->currentwidth - 2 * border);
  iupAttribSetInt(gl_parent, "CLIP_H", gl_parent->currentheight - 2 * border);
}

void iupGLSubCanvasRestoreState(Ihandle* gl_parent)
{
  char* saved = iupAttribGet(gl_parent, "_IUP_GLSUBCANVAS_SAVED");
  iupASSERT(saved != NULL);
  if (!saved)
    return;

  /* restore transformation matrix */
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();

  /* restore transformation matrix */
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();

  /* restore everything else */
  glPopAttrib();

  iupAttribSet(gl_parent, "_IUP_GLSUBCANVAS_SAVED", NULL);
}

static void iGLSubCanvasRedrawFront(Ihandle* ih)
{
  IFn cb = (IFn)IupGetCallback(ih, "GL_ACTION");
  if (cb && iupAttribGetInt(ih, "VISIBLE"))
  {
    Ihandle* gl_parent = (Ihandle*)iupAttribGet(ih, "GL_CANVAS");
    IupGLMakeCurrent(gl_parent);
    glDrawBuffer(GL_FRONT);
    iupGLSubCanvasSaveState(gl_parent);
    if (iupGLSubCanvasSetTransform(ih, gl_parent))
      cb(ih);
    iupGLSubCanvasRestoreState(gl_parent);
    glDrawBuffer(GL_BACK);
  }
}

int iupGLSubCanvasRedraw(Ihandle* ih)
{
  Ihandle* gl_parent = (Ihandle*)iupAttribGet(ih, "GL_CANVAS");
  if (iupAttribGetInt(ih, "REDRAWALL"))
    IupSetAttribute(gl_parent, "REDRAW", NULL);  /* redraw the whole box */
  else
    iGLSubCanvasRedrawFront(ih);  /* redraw only the sub-canvas in the front buffer */
  return IUP_DEFAULT;  /* return IUP_DEFAULT so it can be used as a callback */
}

void iupGLSubCanvasStartMoving(Ihandle* ih, int x, int y)
{
  iupAttribSetInt(ih, "_IUP_START_X", ih->x + x);
  iupAttribSetInt(ih, "_IUP_START_Y", ih->y + y);

  if (iupAttribGetInt(ih, "MOVETOTOP"))
    IupSetAttribute(ih, "ZORDER", "TOP");
}

int iupGLSubCanvasMove(Ihandle* ih, int x, int y)
{
  int moved = 0;
  int start_x = iupAttribGetInt(ih, "_IUP_START_X");
  int start_y = iupAttribGetInt(ih, "_IUP_START_Y");

  x += ih->x;
  y += ih->y;

  if ((x != start_x) || (y != start_y))
  {
    Ihandle* gl_parent = (Ihandle*)iupAttribGet(ih, "GL_CANVAS");
    IFnii cb = (IFnii)IupGetCallback(ih, "MOVE_CB");

    /* clear canvas box alignment */
    iupAttribSet(ih, "VERTICALALIGN", NULL);
    iupAttribSet(ih, "HORIZONTALALIGN", NULL);

    iupBaseSetPosition(ih, ih->x + (x - start_x), ih->y + (y - start_y));
    iupLayoutUpdate(ih);

    IupSetAttribute(gl_parent, "REDRAW", NULL);  /* redraw the whole box */
    moved = 1;

    if (cb)
      cb(ih, ih->x, ih->y);
  }

  iupAttribSetInt(ih, "_IUP_START_X", x);
  iupAttribSetInt(ih, "_IUP_START_Y", y);
  return moved;
}


/****************************************************************/


static char* iGLSubCanvasGetCharSizeAttrib(Ihandle* ih)
{
  int charwidth, charheight;

  iupGLFontGetCharSize(ih, &charwidth, &charheight);
  if (charwidth == 0 || charheight == 0)
    return NULL;

  return iupStrReturnIntInt(charwidth, charheight, 'x');
}

static int iGLSubCanvasSetSizeAttrib(Ihandle* ih, const char* value)
{
  if (!value)
  {
    ih->userwidth = 0;
    ih->userheight = 0;
  }
  else
  {
    /* if not specified, the value is 0 */
    int w = 0, h = 0;
    int charwidth, charheight;
    iupGLFontGetCharSize(ih, &charwidth, &charheight);
    iupStrToIntInt(value, &w, &h, 'x');
    if (w < 0) w = 0;
    if (h < 0) h = 0;

    ih->userwidth = iupWIDTH2RASTER(w, charwidth);
    ih->userheight = iupHEIGHT2RASTER(h, charheight);
  }
  return 1;  /* always save in the hash table, so when FONT is changed SIZE can be updated */
}

static char* iGLSubCanvasGetSizeAttrib(Ihandle* ih)
{
  int charwidth, charheight, width, height;

  if (ih->handle)
  {
    width = ih->currentwidth;
    height = ih->currentheight;
  }
  else
  {
    width = ih->userwidth;
    height = ih->userheight;
  }

  iupGLFontGetCharSize(ih, &charwidth, &charheight);
  if (charwidth == 0 || charheight == 0)
    return NULL;  /* if font failed get from the hash table */

  return iupStrReturnIntInt(iupRASTER2WIDTH(width, charwidth), iupRASTER2HEIGHT(height, charheight), 'x');
}

static int iGLSubCanvasSetRedrawFrontAttrib(Ihandle* ih, const char* value)
{
  iGLSubCanvasRedrawFront(ih);
  (void)value;
  return 0;
}

static int iGLSubCanvasSetZorder(Ihandle* parent, Ihandle* child, int top)
{
  Ihandle *c,
    *c_prev = NULL;

  if (top && child->brother == NULL)  /* already at the top */
    return 0;

  if (!top && parent->firstchild == child)  /* already at the bottom */
    return 0;

  /* Removes the child entry inside the parent's child list */
  for (c = parent->firstchild; c; c = c->brother)
  {
    if (c == child)
    {
      if (c_prev == NULL)
        parent->firstchild = child->brother;
      else
        c_prev->brother = child->brother;

      child->brother = NULL;
      break;
    }

    c_prev = c;
  }

  if (top)
  {
    /* insert at last element */
    for (c = parent->firstchild; c; c = c->brother)
    {
      if (!c->brother)
      {
        c->brother = child;
        break;
      }
    }
  }
  else
  {
    /* insert at first */
    child->brother = parent->firstchild;
    parent->firstchild = child;
  }

  return 1;
}

static int iGLSubCanvasSetZorderAttrib(Ihandle* ih, const char* value)
{
  Ihandle* gl_parent = (Ihandle*)iupAttribGet(ih, "GL_CANVAS");
  int redraw = 0;
  int top = 1;
  if (iupStrEqualNoCase(value, "BOTTOM"))
    top = 0;

  /* move everyone in the same hierachy */
  while (ih != gl_parent)
  {
    if (iGLSubCanvasSetZorder(ih->parent, ih, top))
      redraw = 1;
    ih = ih->parent;
  }

  if (redraw)
    IupSetAttribute(gl_parent, "REDRAW", NULL);  /* redraw the whole box */

  return 0;
}


/****************************************************************/


static int iGLSubCanvasMapMethod(Ihandle* ih)
{
  Ihandle* gl_parent;
  for (gl_parent = ih->parent; gl_parent; gl_parent = gl_parent->parent)
  {
    if (gl_parent->iclass->nativetype == IUP_TYPECANVAS)
      break;
  }

  /* must have a canvas parent, and it must be a glcanvasbox */
  if (!gl_parent || !iupClassMatch(gl_parent->iclass, "glcanvasbox"))
    return IUP_ERROR;

  iupAttribSet(ih, "GL_CANVAS", (char*)gl_parent);

  /* use the handle of the native parent */
  ih->handle = gl_parent->handle;
  return IUP_NOERROR;
}

static void iGLSubCanvasReleaseMethod(Iclass* ic)
{
  iupGLFontFinish();
  (void)ic;
}

static int iGLSubCanvasCreateMethod(Ihandle* ih, void** params)
{
  (void)params;
  // to avoid system fonts that are not well processed by FreeType
  iupAttribSetClassObject(ih, "FONTFACE", "Helvetica");
  return IUP_NOERROR;
}

Iclass* iupGLSubCanvasNewClass(void)
{
  Iclass* ic = iupClassNew(NULL);

  ic->name = "glsubcanvas";
  ic->format = NULL;  /* no parameters */
  ic->nativetype = IUP_TYPEVOID;
  ic->childtype = IUP_CHILDNONE;
  ic->is_interactive = 0;

  /* Class functions */
  ic->New = iupGLSubCanvasNewClass;
  ic->Create = iGLSubCanvasCreateMethod;
  ic->Release = iGLSubCanvasReleaseMethod;
  ic->Map = iGLSubCanvasMapMethod;

  /* Callbacks */
  iupClassRegisterCallback(ic, "GL_ACTION", "");
  iupClassRegisterCallback(ic, "GL_BUTTON_CB", "iiiis");
  iupClassRegisterCallback(ic, "GL_ENTERWINDOW_CB", "ii");
  iupClassRegisterCallback(ic, "GL_LEAVEWINDOW_CB", "");
  iupClassRegisterCallback(ic, "GL_MOTION_CB", "iis");
  iupClassRegisterCallback(ic, "GL_WHEEL_CB", "fiis");

  /* Common */
  iupBaseRegisterCommonAttrib(ic);

  /* redefine common attributes */
  iupClassRegisterAttribute(ic, "SIZE", iGLSubCanvasGetSizeAttrib, iGLSubCanvasSetSizeAttrib, NULL, NULL, IUPAF_NO_SAVE | IUPAF_NO_DEFAULTVALUE | IUPAF_NOT_MAPPED | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "CHARSIZE", iGLSubCanvasGetCharSizeAttrib, NULL, NULL, NULL, IUPAF_NO_DEFAULTVALUE | IUPAF_READONLY | IUPAF_NOT_MAPPED | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "FONT", NULL, iupGLFontSetFontAttrib, IUPAF_SAMEASSYSTEM, "DEFAULTFONT", IUPAF_NOT_MAPPED);  /* inherited */

  /* Visual */
  iupBaseRegisterVisualAttrib(ic);

  /* redefine native visual attributes */
  iupClassRegisterAttribute(ic, "VISIBLE", NULL, NULL, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NO_SAVE | IUPAF_DEFAULT);  /* inheritable */
  iupClassRegisterAttribute(ic, "ACTIVE", NULL, NULL, IUPAF_SAMEASSYSTEM, "YES", IUPAF_DEFAULT);  /* inheritable */
  iupClassRegisterAttribute(ic, "ZORDER", NULL, iGLSubCanvasSetZorderAttrib, NULL, NULL, IUPAF_WRITEONLY | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "TIP", NULL, NULL, NULL, NULL, IUPAF_NO_DEFAULTVALUE | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "TIPVISIBLE", NULL, NULL, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "CURSOR", NULL, NULL, IUPAF_SAMEASSYSTEM, "ARROW", IUPAF_IHANDLENAME | IUPAF_NO_INHERIT);

  /* Common visual */
  iupClassRegisterAttribute(ic, "BORDERCOLOR", NULL, NULL, IUPAF_SAMEASSYSTEM, "50 150 255", IUPAF_DEFAULT);  /* inheritable */
  iupClassRegisterAttribute(ic, "BORDERWIDTH", NULL, NULL, IUPAF_SAMEASSYSTEM, "1", IUPAF_DEFAULT);  /* inheritable */
  iupClassRegisterAttribute(ic, "BGCOLOR", NULL, NULL, NULL, NULL, IUPAF_DEFAULT);  /* inheritable */
  iupClassRegisterAttribute(ic, "HLCOLOR", NULL, NULL, IUPAF_SAMEASSYSTEM, "200 225 245", IUPAF_DEFAULT);  /* inheritable */
  iupClassRegisterAttribute(ic, "PSCOLOR", NULL, NULL, IUPAF_SAMEASSYSTEM, "150 200 235", IUPAF_DEFAULT);  /* inheritable */

  iupClassRegisterAttribute(ic, "UNDERLINE", NULL, NULL, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "REDRAWFRONT", NULL, iGLSubCanvasSetRedrawFrontAttrib, NULL, NULL, IUPAF_WRITEONLY | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "REDRAWALL", NULL, NULL, IUPAF_SAMEASSYSTEM, "Yes", IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "HIGHLIGHT", NULL, NULL, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "PRESSED", NULL, NULL, NULL, NULL, IUPAF_NO_INHERIT);

  iupGLFontInit();

  return ic;
}

Ihandle* IupGLSubCanvas(void)
{
  return IupCreate("glsubcanvas");
}

