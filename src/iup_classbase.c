/** \file
 * \brief Ihandle Class Base Functions
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>

#include "iup.h"
#include "iupcbs.h"

#include "iup_object.h"
#include "iup_drv.h"
#include "iup_drvinfo.h"
#include "iup_drvfont.h"
#include "iup_str.h"
#include "iup_attrib.h"
#include "iup_layout.h"
#include "iup_assert.h"


void iupBaseCallValueChangedCb(Ihandle* ih)
{
  IFn vc_cb = (IFn)IupGetCallback(ih, "VALUECHANGED_CB");
  if (vc_cb)
    vc_cb(ih);
}

int iupBaseTypeVoidMapMethod(Ihandle* ih)
{
  ih->handle = (InativeHandle*)-1;  /* fake value just to indicate that it is already mapped */
  return IUP_NOERROR;
}

char* iupBaseGetWidAttrib(Ihandle *ih)
{
  return (char*)ih->handle;
}

void iupBaseUpdateAttribFromFont(Ihandle* ih)
{
  char* value = iupAttribGet(ih, "SIZE");
  if (value)
    iupBaseSetSizeAttrib(ih, value);

  {
    Icallback cb = IupGetCallback(ih, "UPDATEATTRIBFROMFONT_CB");
    if (cb)
      cb(ih);
  }
}

int iupBaseSetSizeAttrib(Ihandle* ih, const char* value)
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
    iupdrvFontGetCharSize(ih, &charwidth, &charheight);
    iupStrToIntInt(value, &w, &h, 'x');
    if (w < 0) w = 0;
    if (h < 0) h = 0;

    ih->userwidth = iupWIDTH2RASTER(w, charwidth);
    ih->userheight = iupHEIGHT2RASTER(h, charheight);
  }
  return 1;  /* always save in the hash table, so when FONT is changed SIZE can be updated */
}

char* iupBaseGetSizeAttrib(Ihandle* ih)
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

  iupdrvFontGetCharSize(ih, &charwidth, &charheight);
  if (charwidth == 0 || charheight == 0)
    return NULL;  /* if font failed get from the hash table */

  if (width == 0 && height == 0)
    return NULL;
  return iupStrReturnIntInt(iupRASTER2WIDTH(width, charwidth), iupRASTER2HEIGHT(height, charheight), 'x');
}

int iupBaseSetRasterSizeAttrib(Ihandle* ih, const char* value)
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
    iupStrToIntInt(value, &w, &h, 'x');
    if (w < 0) w = 0;
    if (h < 0) h = 0;
    ih->userwidth = w;
    ih->userheight = h;
  }
  iupAttribSet(ih, "SIZE", NULL); /* clear SIZE in hash table */
  return 0;
}

char* iupBaseGetClientOffsetAttrib(Ihandle* ih)
{
  (void)ih;
  return "0x0";
}

char* iupBaseGetCurrentSizeAttrib(Ihandle* ih)
{
  int width = ih->currentwidth;
  int height = ih->currentheight;
  if (width < 0) width = 0;
  if (height < 0) height = 0;
  if (width == 0 && height == 0)
    return NULL;
  return iupStrReturnIntInt(width, height, 'x');
}

char* iupBaseGetRasterSizeAttrib(Ihandle* ih)
{
  int width, height;

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

  if (width < 0) width = 0;
  if (height < 0) height = 0;

  if (width == 0 && height == 0)
    return NULL;

  return iupStrReturnIntInt(width, height, 'x');
}

char* iupBaseGetCharSizeAttrib(Ihandle* ih)
{
  int charwidth, charheight;

  iupdrvFontGetCharSize(ih, &charwidth, &charheight);
  if (charwidth == 0 || charheight == 0)  /* if font failed get from the hash table */
    return NULL;

  return iupStrReturnIntInt(charwidth, charheight, 'x');
}

static char* iBaseGetNaturalSizeAttrib(Ihandle* ih)
{
  if (ih->naturalwidth == 0 && ih->naturalheight == 0)
    return NULL;
  return iupStrReturnIntInt(ih->naturalwidth, ih->naturalheight, 'x');
}

static char* iBaseGetUserSizeAttrib(Ihandle* ih)
{
  if (ih->userwidth == 0 && ih->userheight == 0)
    return NULL;
  return iupStrReturnIntInt(ih->userwidth, ih->userheight, 'x');
}

static char* iBaseGetPositionAttrib(Ihandle* ih)
{
  return iupStrReturnIntInt(ih->x, ih->y, ',');
}

static int iBaseSetPositionAttrib(Ihandle* ih, const char* value)
{
  iupStrToIntInt(value, &ih->x, &ih->y, ',');
  return 0;
}

static char* iBaseGetXAttrib(Ihandle *ih)
{
  int x = 0, y = 0;
  iupdrvClientToScreen(ih, &x, &y);
  iupdrvAddScreenOffset(&x, &y, -1);
  return iupStrReturnInt(x);
}

static char* iBaseGetYAttrib(Ihandle *ih)
{
  int x = 0, y = 0;
  iupdrvClientToScreen(ih, &x, &y);
  iupdrvAddScreenOffset(&x, &y, -1);
  return iupStrReturnInt(y);
}

static char* iBaseGetScreenPositionAttrib(Ihandle *ih)
{
  int x = 0, y = 0;
  iupdrvClientToScreen(ih, &x, &y);
  iupdrvAddScreenOffset(&x, &y, -1);
  return iupStrReturnIntInt(x, y, ',');
}

char* iupBaseGetActiveAttrib(Ihandle *ih)
{
  return iupStrReturnBoolean (iupdrvIsActive(ih)); 
}

static int iBaseNativeParentIsActive(Ihandle* ih)
{
  Ihandle* parent = ih->parent;

  if (!parent || parent->iclass->nativetype == IUP_TYPEDIALOG)
    return 1;

  if (parent->iclass->nativetype == IUP_TYPEVOID)
    return iBaseNativeParentIsActive(parent);
  else 
    return iupdrvIsActive(parent);
}

int iupBaseSetActiveAttrib(Ihandle* ih, const char* value)
{
  if (iupStrBoolean(value))
  {
    if (iBaseNativeParentIsActive(ih))
      iupdrvSetActive(ih, 1);
  }
  else
    iupdrvSetActive(ih, 0);
  return 0;
}

char* iupBaseGetVisibleAttrib(Ihandle* ih)
{
  return iupStrReturnBoolean (iupdrvIsVisible(ih)); 
}

int iupBaseSetVisibleAttrib(Ihandle* ih, const char* value)
{
  iupdrvSetVisible(ih, iupStrBoolean(value));
  return 1;  /* must be 1 to mark when set at the element */
}

char* iupBaseNativeParentGetBgColorAttrib(Ihandle* ih)
{
  /* Used only by those who need a transparent background */
  char* color = iupAttribGetInheritNativeParent(ih, "BGCOLOR");
  if (!color) color = iupAttribGetInheritNativeParent(ih, "BACKGROUND");
  if (!color) color = IupGetGlobal("DLGBGCOLOR");
  return color;
}

char* iupBaseNativeParentGetBgColor(Ihandle* ih)
{
  /* Used in SetBgColorAttrib */
  char* color = iupAttribGetInheritNativeParent(ih, "BGCOLOR");
  if (!color) color = IupGetGlobal("DLGBGCOLOR");
  return color;
}

int iupRound(double x)
{
  return iupROUND(x);
}

int iupBaseGetScrollbar(Ihandle* ih)
{
  int sb = IUP_SB_NONE;  /* NO scrollbar by default */
  char* value = IupGetAttribute(ih, "SCROLLBAR");
  if (value)
  {
    if (iupStrEqualNoCase(value, "YES"))
      sb = IUP_SB_HORIZ | IUP_SB_VERT;
    else if (iupStrEqualNoCase(value, "HORIZONTAL"))
      sb = IUP_SB_HORIZ;
    else if (iupStrEqualNoCase(value, "VERTICAL"))
      sb = IUP_SB_VERT;
  }
  return sb;
}

static int iBaseSetNormalizerGroupAttrib(Ihandle* ih, const char* value)
{
  Ihandle* ih_normalizer = IupGetHandle(value);
  if (!ih_normalizer)
  {
    ih_normalizer = IupNormalizer(NULL);
    IupSetHandle(value, ih_normalizer);
  }

  IupSetAttribute(ih_normalizer, "ADDCONTROL_HANDLE", (char*)ih);
  return 1;
}

static Ihandle* iBaseFindNamedChild(Ihandle* ih, const char* name)
{
  Ihandle* child = ih->firstchild;
  while (child)
  {
    char* child_name = iupAttribGet(child, "NAME");
    if (child_name && iupStrEqualNoCase(name, child_name))
      return child;

    if (child->firstchild)
    {
      Ihandle* c = iBaseFindNamedChild(child, name);
      if (c) return c;
    }

    child = child->brother;
  }
  return NULL;
}

Ihandle* IupGetDialogChild(Ihandle* ih, const char* name)
{
  Ihandle *child, *dialog;
  char attrib[1024] = "_IUP_DIALOG_CHILD_";

  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return NULL;

  if (!name)
    return NULL;

  dialog = IupGetDialog(ih);
  if (dialog) ih = dialog;
 
  strcat(attrib, name);
  child = (Ihandle*)iupAttribGet(ih, attrib);
  if (child) return child;

  if (ih->firstchild)
  {
    child = iBaseFindNamedChild(ih, name);
    if (child) return child;
  }

  ih = IupGetAttributeHandle(ih, "MENU");
  if (ih)
  {
    child = iBaseFindNamedChild(ih, name);
    if (child) return child;
  }
  return NULL;
}

int iupBaseSetNameAttrib(Ihandle* ih, const char* value)
{
  if (!value)
  {
    /* remove old name cache */
    value = iupAttribGet(ih, "NAME");
    if (value)
    {
      Ihandle* dialog;
      char attrib[1024] = "_IUP_DIALOG_CHILD_";
      strcat(attrib, value);
      dialog = (Ihandle*)iupAttribGet(ih, attrib);
      if (dialog)
        iupAttribSet(dialog, attrib, NULL);
    }
  }
  else
  {
    Ihandle* dialog = IupGetDialog(ih);
    if (dialog)
    {
      char attrib[1024] = "_IUP_DIALOG_CHILD_";
      strcat(attrib, value);
      iupAttribSet(dialog, attrib, (char*)ih);
      iupAttribSet(ih, attrib, (char*)dialog);
    }
  }
  return 1;
}

static int iBaseSetFloatingAttrib(Ihandle* ih, const char* value)
{
  if (iupStrEqualNoCase(value, "IGNORE"))
    ih->flags |= IUP_FLOATING_IGNORE|IUP_FLOATING;
  else if (iupStrBoolean(value))
    ih->flags |= IUP_FLOATING;
  else
    ih->flags &= ~(IUP_FLOATING_IGNORE|IUP_FLOATING);  /* clear both flags */
  return 0;
}

static char* iBaseGetFloatingAttrib(Ihandle* ih)
{
  if (ih->flags & IUP_FLOATING_IGNORE)
    return "IGNORE";
  else 
    return iupStrReturnBoolean (ih->flags & IUP_FLOATING); 
}

int iupBaseSetMaxSizeAttrib(Ihandle* ih, const char* value)
{
  if (value)
    ih->flags |= IUP_MAXSIZE;
  else
    ih->flags &= ~IUP_MAXSIZE;
  return 1;
}

int iupBaseSetMinSizeAttrib(Ihandle* ih, const char* value)
{
  if (value)
    ih->flags |= IUP_MINSIZE;
  else
    ih->flags &= ~IUP_MINSIZE;
  return 1;
}

int iupBaseSetExpandAttrib(Ihandle* ih, const char* value)
{
  if (iupStrEqualNoCase(value, "YES"))
    ih->expand = IUP_EXPAND_BOTH;
  else if (iupStrEqualNoCase(value, "HORIZONTAL"))
    ih->expand = IUP_EXPAND_WIDTH;
  else if (iupStrEqualNoCase(value, "VERTICAL"))
    ih->expand = IUP_EXPAND_HEIGHT;
  else if (iupStrEqualNoCase(value, "HORIZONTALFREE"))
    ih->expand = IUP_EXPAND_WFREE;
  else if (iupStrEqualNoCase(value, "VERTICALFREE"))
    ih->expand = IUP_EXPAND_HFREE;
  else
    ih->expand = IUP_EXPAND_NONE;
  return 0;
}

char* iupBaseGetExpandAttrib(Ihandle* ih)
{
  if ((ih->expand & IUP_EXPAND_WIDTH) && (ih->expand & IUP_EXPAND_HEIGHT))
    return "YES";
  else if (ih->expand & IUP_EXPAND_WIDTH)
    return "HORIZONTAL";
  else if (ih->expand & IUP_EXPAND_HEIGHT)
    return "VERTICAL";
  else if (ih->expand & IUP_EXPAND_WFREE)
    return "HORIZONTALFREE";
  else if (ih->expand & IUP_EXPAND_HFREE)
    return "VERTICALFREE";
  else
    return "NO";
}

void iupBaseContainerUpdateExpand(Ihandle* ih)
{
  char *expand = iupAttribGetInherit(ih, "EXPAND");
  if (!expand)
    ih->expand = IUP_EXPAND_BOTH;  /* default for containers is YES */
  else
  {
    if (iupStrEqualNoCase(expand, "NO"))
      ih->expand = IUP_EXPAND_NONE;
    else if (iupStrEqualNoCase(expand, "HORIZONTAL"))
      ih->expand = IUP_EXPAND_WIDTH;
    else if (iupStrEqualNoCase(expand, "VERTICAL"))
      ih->expand = IUP_EXPAND_HEIGHT;
    else if (iupStrEqualNoCase(expand, "HORIZONTALFREE"))
      ih->expand = IUP_EXPAND_WFREE;
    else if (iupStrEqualNoCase(expand, "VERTICALFREE"))
      ih->expand = IUP_EXPAND_HFREE;
    else
      ih->expand = IUP_EXPAND_BOTH;  /* default for containers is YES */
  }
}

/* EXPAND is registered with IUP_NO_INHERIT because it is not inheritable,
   but it is inheritable for containers. 
   So if you set at a container it will not affect its children,
   but children that are containers will get the same value. */
char* iupBaseContainerGetExpandAttrib(Ihandle* ih)
{
  return iupAttribGetInherit(ih, "EXPAND");
}

void iupBaseRegisterCommonAttrib(Iclass* ic)
{
  iupClassRegisterAttribute(ic, "WID", iupBaseGetWidAttrib, NULL, NULL, NULL, IUPAF_READONLY|IUPAF_NO_INHERIT|IUPAF_NO_STRING);
  iupClassRegisterAttribute(ic, "NAME", NULL, iupBaseSetNameAttrib, NULL, NULL, IUPAF_NO_DEFAULTVALUE|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "FLOATING", iBaseGetFloatingAttrib, iBaseSetFloatingAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "EXPAND", iupBaseGetExpandAttrib, iupBaseSetExpandAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "NORMALIZERGROUP", NULL, iBaseSetNormalizerGroupAttrib, NULL, NULL, IUPAF_IHANDLENAME|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "EXPANDWEIGHT", NULL, NULL, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "HANDLENAME", NULL, NULL, NULL, NULL, IUPAF_NOT_MAPPED | IUPAF_NO_INHERIT);

  /* make sure everyone has the correct default value */
  iupClassRegisterAttribute(ic, "ACTIVE", NULL, NULL, IUPAF_SAMEASSYSTEM, "YES", IUPAF_DEFAULT);  /* inherited */
  if (ic->is_interactive)
    iupClassRegisterAttribute(ic, "CANFOCUS", NULL, NULL, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NO_INHERIT);
  else
    iupClassRegisterAttribute(ic, "CANFOCUS", NULL, NULL, IUPAF_SAMEASSYSTEM, "NO", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "PROPAGATEFOCUS", NULL, NULL, IUPAF_SAMEASSYSTEM, "NO", IUPAF_NO_INHERIT);

  /* if not native container, must set at children,
     native container will automatically hide its children. */
  iupClassRegisterAttribute(ic, "VISIBLE", NULL, NULL, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NO_SAVE|IUPAF_DEFAULT);  /* inherited */

  iupClassRegisterAttribute(ic, "SIZE", iupBaseGetSizeAttrib, iupBaseSetSizeAttrib, NULL, NULL, IUPAF_NO_SAVE|IUPAF_NO_DEFAULTVALUE|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "RASTERSIZE", iupBaseGetRasterSizeAttrib, iupBaseSetRasterSizeAttrib, NULL, NULL, IUPAF_NO_SAVE|IUPAF_NO_DEFAULTVALUE|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "CHARSIZE", iupBaseGetCharSizeAttrib, NULL, NULL, NULL, IUPAF_NO_DEFAULTVALUE|IUPAF_READONLY|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "NATURALSIZE", iBaseGetNaturalSizeAttrib, NULL, NULL, NULL, IUPAF_NO_DEFAULTVALUE|IUPAF_READONLY|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "USERSIZE", iBaseGetUserSizeAttrib, iupBaseSetRasterSizeAttrib, NULL, NULL, IUPAF_NO_DEFAULTVALUE | IUPAF_NOT_MAPPED | IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "POSITION", iBaseGetPositionAttrib, iBaseSetPositionAttrib, NULL, NULL, IUPAF_NO_SAVE|IUPAF_NO_DEFAULTVALUE|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "MINSIZE", NULL, iupBaseSetMinSizeAttrib, IUPAF_SAMEASSYSTEM, "0x0", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "MAXSIZE", NULL, iupBaseSetMaxSizeAttrib, IUPAF_SAMEASSYSTEM, "65535x65535", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "FONT", NULL, iupdrvSetFontAttrib, IUPAF_SAMEASSYSTEM, "DEFAULTFONT", IUPAF_NOT_MAPPED);  /* inherited */

  iupClassRegisterAttribute(ic, "FONTSTYLE", iupGetFontStyleAttrib, iupSetFontStyleAttrib, NULL, NULL, IUPAF_NO_SAVE|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "FONTSIZE",  iupGetFontSizeAttrib,  iupSetFontSizeAttrib,  NULL, NULL, IUPAF_NO_SAVE|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "FONTFACE",  iupGetFontFaceAttrib,  iupSetFontFaceAttrib,  NULL, NULL, IUPAF_NO_SAVE|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);

  iupdrvBaseRegisterCommonAttrib(ic);
}

void iupBaseRegisterVisualAttrib(Iclass* ic)
{
  iupClassRegisterAttribute(ic, "VISIBLE", iupBaseGetVisibleAttrib, iupBaseSetVisibleAttrib, "YES", "NO", IUPAF_NO_SAVE | IUPAF_DEFAULT);   /* must be inheritable to propagate visual state to children and from parent */
  iupClassRegisterAttribute(ic, "ACTIVE", iupBaseGetActiveAttrib, iupBaseSetActiveAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_DEFAULT);  /* must be inheritable to propagate visual state to children and from parent */

  iupClassRegisterAttribute(ic, "ZORDER", NULL, iupdrvBaseSetZorderAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "X", iBaseGetXAttrib, NULL, NULL, NULL, IUPAF_READONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "Y", iBaseGetYAttrib, NULL, NULL, NULL, IUPAF_READONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SCREENPOSITION", iBaseGetScreenPositionAttrib, NULL, NULL, NULL, IUPAF_READONLY|IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "TIP", NULL, iupdrvBaseSetTipAttrib, NULL, NULL, IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "TIPVISIBLE", iupdrvBaseGetTipVisibleAttrib, iupdrvBaseSetTipVisibleAttrib, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "TIPDELAY", NULL, NULL, IUPAF_SAMEASSYSTEM, "5000", IUPAF_NOT_MAPPED);          /* 5 seconds */
  iupClassRegisterAttribute(ic, "TIPBGCOLOR", NULL, NULL, IUPAF_SAMEASSYSTEM, "255 255 225", IUPAF_NOT_MAPPED); /* Light Yellow */
  iupClassRegisterAttribute(ic, "TIPFGCOLOR", NULL, NULL, IUPAF_SAMEASSYSTEM, "0 0 0", IUPAF_NOT_MAPPED);       /* black */
  iupClassRegisterAttribute(ic, "TIPRECT", NULL, NULL, IUPAF_SAMEASSYSTEM, NULL, IUPAF_NOT_MAPPED);

  iupdrvBaseRegisterVisualAttrib(ic);
}

void iupBaseRegisterCommonCallbacks(Iclass* ic)
{
  iupClassRegisterCallback(ic, "DESTROY_CB", "");
  iupClassRegisterCallback(ic, "LDESTROY_CB", "");
  iupClassRegisterCallback(ic, "MAP_CB", "");
  iupClassRegisterCallback(ic, "UNMAP_CB", "");
  iupClassRegisterCallback(ic, "GETFOCUS_CB", "");
  iupClassRegisterCallback(ic, "KILLFOCUS_CB", "");
  iupClassRegisterCallback(ic, "ENTERWINDOW_CB", "");
  iupClassRegisterCallback(ic, "LEAVEWINDOW_CB", "");
  iupClassRegisterCallback(ic, "HELP_CB", "");
  iupClassRegisterCallback(ic, "K_ANY", "i");
}

int iupBaseNoSaveCheck(Ihandle* ih, const char* name)
{
  if (iupStrEqual(name, "BGCOLOR") ||
      iupStrEqual(name, "VISIBLE") ||
      iupStrEqual(name, "SIZE"))
  {
    if (iupAttribGet(ih, name))  /* save if stored at the hash table */
      return 0;  /* save the attribute */
    else
      return 1;
  }
  if (iupStrEqual(name, "RASTERSIZE"))
  {
    if (!iupAttribGet(ih, "SIZE") &&   /* save if SIZE is not set, and user size is set */
        (ih->userwidth!=0 || ih->userheight!=0))
      return 0;
    else
      return 1;
  }
  if (iupStrEqual(name, "POSITION"))
  {
    if (ih->flags&IUP_FLOATING &&   /* save only if floating is set */
        (ih->x != 0 || ih->y != 0))
      return 0;
    else
      return 1;
  }

  return 1; /* default is NOT to save */
}
