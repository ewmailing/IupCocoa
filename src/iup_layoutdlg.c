/** \file
 * \brief IupLayoutDialog pre-defined dialog
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <stdarg.h>
#include <limits.h>
#include <ctype.h>

#include "iup.h"

#include "iup_object.h"
#include "iup_attrib.h"
#include "iup_str.h"
#include "iup_focus.h"
#include "iup_dlglist.h"
#include "iup_assert.h"
#include "iup_drvdraw.h"
#include "iup_draw.h"
#include "iup_image.h"
#include "iup_childtree.h"
#include "iup_drv.h"
#include "iup_func.h"
#include "iup_register.h"

enum {
  FIND_TYPE = 0,
  FIND_HANDLE = 1,
  FIND_NAME = 2,
  FIND_TITLE = 3,
  FIND_ATTRIBUTE = 4
};

typedef struct _iLayoutDialog {
  int destroy;  /* destroy the selected dialog, when the layout dialog is destroyed */
  int changed;
  Ihandle *dialog;  /* the selected dialog */
  Ihandle *tree, *status, *timer, *properties;  /* elements from the layout dialog */
  Ihandle *copy;
} iLayoutDialog;

static int iLayoutFindItemMatch(Ihandle *ih, const char *str, int searchType)
{
  if (searchType == FIND_TYPE)
    return iupStrEqualNoCase(IupGetClassName(ih), str);
  else if (searchType == FIND_TITLE)
  {
    char* title = iupAttribGetLocal(ih, "TITLE");
    return title ? iupStrEqualNoCase(title, str) : 0;
  }
  else if (searchType == FIND_ATTRIBUTE)
  {
    char att[80], val[80];
    char *attVal;
    iupStrToStrStr(str, att, val, '=');
    attVal = iupAttribGetLocal(ih, att);
    if (attVal == NULL)
      return 0;
    if (iupStrEqualNoCase(val, attVal))
      return 1;
  }

  return 0;
}

static Ihandle* iLayoutFindNode(Ihandle* tree, const char *str, int start_id, int end_id, int searchType)
{
  Ihandle *elem;
  int id;

  for (id = start_id; id <= end_id; id++)
  {
    elem = (Ihandle*)IupTreeGetUserId(tree, id);

    if (iLayoutFindItemMatch(elem, str, searchType))
      return elem;
  }

  return NULL;
}

static int iLayoutTreeSelection_CB(Ihandle* tree, int id, int status);

static int iLayoutFindDialogNext_CB(Ihandle* ih)
{
  Ihandle* find_dlg = (Ihandle*)IupGetAttribute(ih, "FIND_DIALOG");
  Ihandle* dialog = (Ihandle*)IupGetAttribute(find_dlg, "DIALOG");
  Ihandle* tree = (Ihandle*)IupGetAttribute(find_dlg, "TREE");
  Ihandle* lbl_result = IupGetDialogChild(find_dlg, "FIND_RESULT");

  Ihandle* target = IupGetDialogChild(find_dlg, "TARGET");
  Ihandle* targetValue = IupGetAttributeHandle(target, "VALUE");
  int searchType = IupGetInt(targetValue, "SEARCH_TYPE");

  char* str = IupGetAttribute(IupGetDialogChild(find_dlg, "FIND_TEXT"), "VALUE");

  int last_id = IupGetInt(tree, "VALUE");

  Ihandle* obj = NULL;

  if (searchType == FIND_HANDLE)
    obj = IupGetHandle(str);
  else if (searchType == FIND_NAME)
    obj = IupGetDialogChild(dialog, str);
  else 
  {
    int count = IupGetInt(tree, "COUNT");
    obj = iLayoutFindNode(tree, str, last_id + 1, count - 1, searchType);
    if (!obj)
      obj = iLayoutFindNode(tree, str, 0, last_id, searchType);
  }

  if (obj)
  {
    int id = IupTreeGetId(tree, obj);
    iLayoutTreeSelection_CB(tree, last_id, 0);
    IupSetInt(tree, "VALUE", id);
    iLayoutTreeSelection_CB(tree, id, 1);

    IupSetAttribute(lbl_result, "TITLE", "");
  }
  else
    IupSetAttribute(lbl_result, "TITLE", "Not Found!");

  return IUP_DEFAULT;
}

static int iLayoutFindDialogClose_CB(Ihandle* ih)
{
  Ihandle* find_dlg = IupGetDialog(ih);
  IupHide(find_dlg);  /* do not destroy, just hide */
  return IUP_DEFAULT;
}

static Ihandle* iLayoutCreateFindDialog(iLayoutDialog* layoutdlg, Ihandle *ih)
{
  Ihandle *txt, *box, *find_dlg;
  Ihandle *type, *handle_name, *name, *title, *attribute, *radio;
  Ihandle *bt_next, *bt_close, *lbl_result;

  txt = IupText(NULL);
  IupSetAttribute(txt, "NAME", "FIND_TEXT");
  IupSetAttribute(txt, "VISIBLECOLUMNS", "20");
  IupSetAttribute(txt, "EXPAND", "HORIZONTAL");

  type = IupToggle("Type", NULL);
  IupSetAttribute(type, "NAME", "FIND_TYPE");
  IupSetInt(type, "SEARCH_TYPE", 0);
  IupSetAttribute(type, "TIP", "Element Type (Class Name)");

  handle_name = IupToggle("Handle", NULL);
  IupSetAttribute(handle_name, "NAME", "FIND_HANDLE");
  IupSetInt(handle_name, "SEARCH_TYPE", 1);
  IupSetAttribute(handle_name, "TIP", "Handle Name");

  name = IupToggle("Name", NULL);
  IupSetAttribute(name, "NAME", "FIND_NAME");
  IupSetInt(name, "SEARCH_TYPE", 2);
  IupSetAttribute(name, "TIP", "NAME attribute");

  title = IupToggle("Title", NULL);
  IupSetAttribute(title, "NAME", "FIND_TITLE");
  IupSetInt(title, "SEARCH_TYPE", 3);
  IupSetAttribute(title, "TIP", "TITLE attribute");

  attribute = IupToggle("Atribute", NULL);
  IupSetAttribute(attribute, "NAME", "FIND_ATTRIBUTE");
  IupSetInt(attribute, "SEARCH_TYPE", 4);
  IupSetAttribute(attribute, "TIP", "Attribute=Value");

  bt_next = IupButton("Find Next", NULL);
  IupSetStrAttribute(bt_next, "PADDING", IupGetGlobal("DEFAULTBUTTONPADDING"));
  IupSetCallback(bt_next, "ACTION", (Icallback)iLayoutFindDialogNext_CB);
  bt_close = IupButton("Close", NULL);
  IupSetCallback(bt_close, "ACTION", (Icallback)iLayoutFindDialogClose_CB);
  IupSetStrAttribute(bt_close, "PADDING", IupGetGlobal("DEFAULTBUTTONPADDING"));

  lbl_result = IupLabel(NULL);
  IupSetAttribute(lbl_result, "SIZE", "30");
  IupSetAttribute(lbl_result, "NAME", "FIND_RESULT");

  radio = IupRadio(IupVbox(
    type,
    handle_name,
    name,
    title,
    attribute,
    NULL));

  IupSetAttribute(radio, "NAME", "TARGET");
  IupSetAttribute(radio, "VALUE_HANDLE", (char*)type);
  IupSetAttribute(radio, "MARGIN", "10x10");

  box = IupVbox(
    IupLabel("Find What:"),
    txt,
    IupHbox(
      IupSetAttributes(IupFrame(radio), "TITLE=\"What Kind:\""),
      IupFill(),
      IupSetAttributes(IupVbox(
        bt_next,
        bt_close,
        lbl_result,
        NULL), "NORMALIZESIZE=HORIZONTAL"),
      NULL),
    NULL);
  IupSetAttribute(box, "NMARGIN", "10x10");
  IupSetAttribute(box, "GAP", "5");

  find_dlg = IupDialog(box);
  IupSetAttribute(find_dlg, "TITLE", "Find in Layout");
  IupSetAttribute(find_dlg, "DIALOGFRAME", "Yes");
  IupSetAttributeHandle(find_dlg, "DEFAULTENTER", bt_next);
  IupSetAttributeHandle(find_dlg, "DEFAULTESC", bt_close);
  IupSetAttributeHandle(find_dlg, "PARENTDIALOG", IupGetDialog(ih));
  IupSetCallback(find_dlg, "CLOSE_CB", (Icallback)iLayoutFindDialogClose_CB);

  /* Save the multiline to access it from the callbacks */
  IupSetAttribute(find_dlg, "TREE", (char*)ih);

  /* Save the dialog to reuse it */
  IupSetAttribute(find_dlg, "FIND_DIALOG", (char*)find_dlg);  /* from itself */
  IupSetAttribute(IupGetDialog(ih), "FIND_DIALOG", (char*)find_dlg); /* from the main dialog */
  IupSetAttribute(find_dlg, "DIALOG", (char*)layoutdlg->dialog); /* from the main dialog */

  return find_dlg;
}

static char* iLayoutGetElementTreeTitle(Ihandle* ih)
{
  char* title = iupAttribGetLocal(ih, "TITLE");
  char* name = IupGetName(ih);
  char* str = iupStrGetMemory(200);
  if (title)
  {
    char buffer[51];

    if (iupStrLineCount(title) > 1)
    {
      int len;
      iupStrNextLine(title, &len); /* get the size of the first line */
      if (len > 50) len = 50;
      iupStrCopyN(buffer, len + 1, title);
      title = &buffer[0];
    }

    if (name)
      sprintf(str, "[%s] %.50s \"%.50s\"", IupGetClassName(ih), title, name);
    else
      sprintf(str, "[%s] %.50s", IupGetClassName(ih), title);
  }
  else
  {
    if (name)
      sprintf(str, "[%s] \"%.50s\"", IupGetClassName(ih), name);
    else
      sprintf(str, "[%s]", IupGetClassName(ih));
  }
  return str;
}

static void iLayoutRemoveExt(char* title, const char* ext)
{
  int len = (int)strlen(title);
  int len_ext = (int)strlen(ext);
  if (len_ext == 1)
  {
    if (tolower(title[len - 1]) == ext[0] &&
        title[len - 2] == '.')
        title[len - 2] = 0; /* place terminator at dot */
  }
  else
  {
    if (tolower(title[len - 1]) == ext[2] &&
        tolower(title[len - 2]) == ext[1] &&
        tolower(title[len - 3]) == ext[0] &&
        title[len - 4] == '.')
        title[len - 4] = 0; /* place terminator at dot */
  }
}

static int iLayoutHasDigit(const char* name)
{
  while (*name)
  {
    if (isdigit(*name))
      return 1;
    name++;
  }
  return 0;
}

static int iLayoutCompareStr(const void *a, const void *b)
{
  return strcmp(*(char**)a, *(char**)b);
}

static int iLayoutAttributeChanged(Ihandle* ih, const char* name, const char* value, const char* def_value, int flags)
{
  if ((flags&IUPAF_NO_STRING) || /* not a string */
      (flags&IUPAF_HAS_ID) ||  /* has id */
      (flags&(IUPAF_READONLY | IUPAF_WRITEONLY)))  /* can only read or only write */
      return 0;

  if (!value || value[0] == 0 || iupATTRIB_ISINTERNAL(value))
    return 0;

  if ((flags&IUPAF_NO_SAVE) && iupBaseNoSaveCheck(ih, name))  /* can not be saved */
    return 0;

  if (def_value && iupStrEqualNoCase(def_value, value))  /* equal to the default value */
    return 0;

  if (!def_value && iupStrFalse(value))  /* default=NULL and value=NO */
    return 0;

  if (!(flags&IUPAF_NO_INHERIT) && ih->parent) /* if inherit, check if the same value is defined at parent */
  {
    char* parent_value = iupAttribGetInherit(ih->parent, name);
    if (parent_value && iupStrEqualNoCase(value, parent_value))
      return 0;
  }

  return 1;
}

static char* iLayoutGetName(Ihandle* ih)
{
  char* name = IupGetName(ih);
  if (name && iupATTRIB_ISINTERNAL(name))
    name = NULL;
  if (!name && ih->iclass->nativetype == IUP_TYPEDIALOG)
    name = iupAttribGet(ih, "_IUP_DIALOG_NAME");
  return name;
}


/***************************************************************************
                          Tree Utilities
***************************************************************************/


static void iLayoutTreeSetNodeColor(Ihandle* tree, int id, Ihandle* ih)
{
  if (ih->handle != NULL)
    IupSetAttributeId(tree, "COLOR", id, "0 0 0");
  else
    IupSetAttributeId(tree, "COLOR", id, "128 128 128");
}

static void iLayoutTreeSetNodeInfo(Ihandle* tree, int id, Ihandle* ih)
{
  IupSetAttributeId(tree, "TITLE", id, iLayoutGetElementTreeTitle(ih));
  iLayoutTreeSetNodeColor(tree, id, ih);
  IupTreeSetUserId(tree, id, ih);
}

static Ihandle* iLayoutTreeGetFirstChild(Ihandle* ih)
{
  Ihandle* firstchild = ih->parent->firstchild;

  while (firstchild && firstchild->flags & IUP_INTERNAL)
    firstchild = firstchild->brother;

  return firstchild;
}

static int iLayoutTreeAddNode(Ihandle* tree, int id, Ihandle* ih)
{
  if (ih->iclass->childtype != IUP_CHILDNONE)
  {
    if (!ih->parent || ih == iLayoutTreeGetFirstChild(ih))
    {
      IupSetAttributeId(tree, "ADDBRANCH", id, "");
      id++;
    }
    else
    {
      IupSetAttributeId(tree, "INSERTBRANCH", id, "");
      id = IupGetInt(tree, "LASTADDNODE");
    }
  }
  else
  {
    if (!ih->parent || ih == iLayoutTreeGetFirstChild(ih))
    {
      IupSetAttributeId(tree, "ADDLEAF", id, "");
      id++;
    }
    else
    {
      IupSetAttributeId(tree, "INSERTLEAF", id, "");
      id = IupGetInt(tree, "LASTADDNODE");
    }
  }

  iLayoutTreeSetNodeInfo(tree, id, ih);
  return id;
}

static void iLayoutTreeAddChildren(Ihandle* tree, int parent_id, Ihandle* parent)
{
  Ihandle *child;
  int last_child_id = parent_id;

  for (child = parent->firstchild; child; child = child->brother)
  {
    if (!(child->flags & IUP_INTERNAL))
    {
      last_child_id = iLayoutTreeAddNode(tree, last_child_id, child);

      if (child->iclass->childtype != IUP_CHILDNONE)
        iLayoutTreeAddChildren(tree, last_child_id, child);
    }
  }
}

static void iLayoutUpdateLayout(iLayoutDialog* layoutdlg)
{
  Ihandle* canvas = IupGetBrother(layoutdlg->tree);
  int w = 0, h = 0;

  IupRefresh(layoutdlg->dialog);

  IupGetIntInt(layoutdlg->dialog, "CLIENTSIZE", &w, &h);
  IupSetInt(canvas, "XMAX", w);
  IupSetInt(canvas, "YMAX", h);

  IupGetIntInt(canvas, "DRAWSIZE", &w, &h);
  IupSetInt(canvas, "DX", w);
  IupSetInt(canvas, "DY", h);

  /* redraw canvas */
  IupUpdate(canvas);
}

static void iLayoutTreeRebuild(iLayoutDialog* layoutdlg)
{
  Ihandle* tree = layoutdlg->tree;
  IupSetAttribute(tree, "DELNODE0", "CHILDREN");

  layoutdlg->changed = 0;
  layoutdlg->copy = NULL;

  iLayoutTreeSetNodeInfo(tree, 0, layoutdlg->dialog);
  iLayoutTreeAddChildren(tree, 0, layoutdlg->dialog);

  iLayoutUpdateLayout(layoutdlg);
}


/***************************************************************************
                         Layout Export
***************************************************************************/


static void iLayoutExportCountContainersRec(Ihandle* ih, int *index)
{
  Ihandle *child;
  for (child = ih->firstchild; child; child = child->brother)
  {
    if (child->iclass->childtype != IUP_CHILDNONE)
    {
      if (!(child->flags & IUP_INTERNAL))
      {
        (*index)++;
        iupAttribSetInt(child, "_IUP_CONTAINER_INDEX", *index);
        iLayoutExportCountContainersRec(child, index);
      }
    }
  }
}

static int iLayoutExportCountContainers(Ihandle* dialog)
{
  int index = 0;
  iupAttribSetInt(dialog, "_IUP_CONTAINER_INDEX", index);
  iLayoutExportCountContainersRec(dialog, &index);
  return index + 1;
}

static void iLayoutExportWriteAttrib(FILE* file, const char* name, const char* value, const char* indent, int type)
{
  char attribname[1024];
  if (type == 1)  /* Lua */
  {
    iupStrLower(attribname, name);
    if (iLayoutHasDigit(attribname))
      fprintf(file, "%s[\"%s\"] = \"%s\",\n", indent, attribname, value);
    else
      fprintf(file, "%s%s = \"%s\",\n", indent, attribname, value);
  }
  else if (type == -1) /* LED */
  {
    iupStrUpper(attribname, name);
    if (iupStrHasSpace(value))
      fprintf(file, "%s%s = \"%s\",\n", indent, attribname, value);
    else
      fprintf(file, "%s%s = %s,\n", indent, attribname, value);
  }
  else
    fprintf(file, "%s\"%s\", \"%s\",\n", indent, name, value);
}

static int iLayoutExportElementAttribs(FILE* file, Ihandle* ih, const char* indent, int type)
{
  int i, wcount = 0, attr_count, has_attrib_id = ih->iclass->has_attrib_id, start_id = 0,
    total_count = IupGetClassAttributes(ih->iclass->name, NULL, 0);
  char **attr_names = (char **)malloc(total_count * sizeof(char *));

  if (IupClassMatch(ih, "tree") || /* tree can only set id attributes after map, so they can not be saved */
      IupClassMatch(ih, "cells"))  /* cells does not have any savable id attributes */
      has_attrib_id = 0;

  if (IupClassMatch(ih, "list"))
    start_id = 1;

  attr_count = IupGetClassAttributes(ih->iclass->name, attr_names, total_count);
  for (i = 0; i < attr_count; i++)
  {
    char *name = attr_names[i];
    char* value = iupAttribGetLocal(ih, name);
    char* def_value;
    int flags;

    iupClassGetAttribNameInfo(ih->iclass, name, &def_value, &flags);

    if (iLayoutAttributeChanged(ih, name, value, def_value, flags))
    {
      char* str = iupStrConvertToC(value);

      iLayoutExportWriteAttrib(file, name, str, indent, type);

      if (str != value)
        free(str);

      wcount++;
    }

    if (has_attrib_id && flags&IUPAF_HAS_ID)
    {
      flags &= ~IUPAF_HAS_ID; /* clear flag so the next function call can work */
      if (iLayoutAttributeChanged(ih, name, "X", NULL, flags))
      {
        if (iupStrEqual(name, "IDVALUE"))
          name = "";

        if (flags&IUPAF_HAS_ID2)
        {
          int lin, col,
            numcol = IupGetInt(ih, "NUMCOL") + 1,
            numlin = IupGetInt(ih, "NUMLIN") + 1;
          for (lin = 0; lin < numlin; lin++)
          {
            for (col = 0; col < numcol; col++)
            {
              value = IupGetAttributeId2(ih, name, lin, col);
              if (value && value[0] && !iupATTRIB_ISINTERNAL(value))
              {
                char str[50];
                sprintf(str, "%s%d:%d", name, lin, col);
                iLayoutExportWriteAttrib(file, str, value, indent, type);
                wcount++;
              }
            }
          }
        }
        else
        {
          int id, count = IupGetInt(ih, "COUNT");
          for (id = start_id; id < count + start_id; id++)
          {
            value = IupGetAttributeId(ih, name, id);
            if (value && value[0] && !iupATTRIB_ISINTERNAL(value))
            {
              char str[50];
              sprintf(str, "%s%d", name, id);
              iLayoutExportWriteAttrib(file, str, value, indent, type);
              wcount++;
            }
          }
        }
      }
    }
  }

  if (type != 0)  /* LED or C */
  {
    int cb_count = total_count - attr_count;
    IupGetClassCallbacks(ih->iclass->name, attr_names, cb_count);
    for (i = 0; i < cb_count; i++)
    {
      char* cb_name = iupGetCallbackName(ih, attr_names[i]);
      if (cb_name && cb_name[0] && !iupATTRIB_ISINTERNAL(cb_name))
      {
        iLayoutExportWriteAttrib(file, attr_names[i], cb_name, indent, type);
        wcount++;
      }
    }
  }

  if (type == -1) /* LED */
  {
    /* remove last comma ',' and new line */
    /* if wcount==0, it will remove '[' and new line */
    fseek(file, -2, SEEK_CUR);
  }

  free(attr_names);
  return wcount;
}

static void iLayoutExportElementC(FILE* file, Ihandle* ih)
{
  char* name = IupGetName(ih);
  char* indent = "    ";
  if (ih->iclass->childtype == IUP_CHILDNONE)
    indent = "        ";
  if (name && iupATTRIB_ISINTERNAL(name))
    name = NULL;

  if (ih->iclass->childtype == IUP_CHILDNONE)
    fprintf(file, "      IupSetAtt(%s%s%s, IupCreate(\"%s\"), \n", name ? "\"" : "", name ? name : "NULL", name ? "\"" : "", ih->iclass->name);
  else
  {
    Ihandle *child;

    fprintf(file, "  containers[%s] = IupSetAtt(%s%s%s, IupCreatep(\"%s\", \n", iupAttribGet(ih, "_IUP_CONTAINER_INDEX"), name ? "\"" : "", name ? name : "NULL", name ? "\"" : "", ih->iclass->name);

    for (child = ih->firstchild; child; child = child->brother)
    {
      if (!(child->flags & IUP_INTERNAL))
      {
        if (child->iclass->childtype == IUP_CHILDNONE)
          iLayoutExportElementC(file, child);  /* only one level of recursion */
        else
        {
          fprintf(file, "      containers[%s],\n", iupAttribGet(child, "_IUP_CONTAINER_INDEX"));
          iupAttribSet(child, "_IUP_CONTAINER_INDEX", NULL);  /* clear when last used */
        }
      }
    }

    fprintf(file, "      NULL),\n"); /* end of IupCreatep */
  }

  iLayoutExportElementAttribs(file, ih, indent, 0);  /* C */

  /* end of IupSetAtt */
  if (ih->iclass->childtype != IUP_CHILDNONE)
    fprintf(file, "    NULL);\n\n");
  else
    fprintf(file, "        NULL),\n");
}

static void iLayoutExportContainerC(FILE* file, Ihandle* ih)
{
  Ihandle *child;
  /* export children first */
  for (child = ih->firstchild; child; child = child->brother)
  {
    if (!(child->flags & IUP_INTERNAL) && child->iclass->childtype != IUP_CHILDNONE)
      iLayoutExportContainerC(file, child);
  }

  iLayoutExportElementC(file, ih);
}

static void iLayoutExportDialogC(FILE* file, Ihandle* dialog, const char* filename)
{
  int count = iLayoutExportCountContainers(dialog);
  char* title = iupStrFileGetTitle(filename);
  iLayoutRemoveExt(title, "c");

  fprintf(file, "/*   Generated by IupLayoutDialog export to C.   */\n\n");
  fprintf(file, "#include <stdlib.h>\n");
  fprintf(file, "#include <iup.h>\n\n");
  fprintf(file, "Ihandle* create_dialog_%s(void)\n", title);
  fprintf(file, "{\n");
  fprintf(file, "  Ihandle* containers[%d];\n\n", count);

  iLayoutExportContainerC(file, dialog);
  iupAttribSet(dialog, "_IUP_CONTAINER_INDEX", NULL);  /* clear when last used */

  fprintf(file, "  return containers[0];\n");
  fprintf(file, "}\n");
  free(title);
}

static void iLayoutExportElementLua(FILE* file, Ihandle* ih)
{
  char* indent = "    ";

  if (ih->iclass->childtype == IUP_CHILDNONE)
  {
    indent = "      ";
    fprintf(file, "    iup.%s{\n", ih->iclass->name);
  }
  else
  {
    Ihandle *child;

    fprintf(file, "  containers[%d] = iup.%s{\n", iupAttribGetInt(ih, "_IUP_CONTAINER_INDEX") + 1, ih->iclass->name);

    for (child = ih->firstchild; child; child = child->brother)
    {
      if (!(child->flags & IUP_INTERNAL))
      {
        if (child->iclass->childtype == IUP_CHILDNONE)
          iLayoutExportElementLua(file, child);  /* only one level of recursion */
        else
        {
          fprintf(file, "    containers[%d],\n", iupAttribGetInt(child, "_IUP_CONTAINER_INDEX") + 1);
          iupAttribSet(child, "_IUP_CONTAINER_INDEX", NULL);  /* clear when last used */
        }
      }
    }
  }

  iLayoutExportElementAttribs(file, ih, indent, 1);  /* Lua */

  if (ih->iclass->childtype != IUP_CHILDNONE)
    fprintf(file, "  }\n\n");
  else
    fprintf(file, "    },\n");
}

static void iLayoutExportContainerLua(FILE* file, Ihandle* ih)
{
  Ihandle *child;
  /* export children first */
  for (child = ih->firstchild; child; child = child->brother)
  {
    if (!(child->flags & IUP_INTERNAL) && child->iclass->childtype != IUP_CHILDNONE)
      iLayoutExportContainerLua(file, child);
  }

  iLayoutExportElementLua(file, ih);
}

static void iLayoutExportDialogLua(FILE* file, Ihandle* dialog, const char* filename)
{
  char* title = iupStrFileGetTitle(filename);
  iLayoutRemoveExt(title, "lua");
  iLayoutExportCountContainers(dialog);

  fprintf(file, "--   Generated by IupLayoutDialog export to Lua.\n\n");
  fprintf(file, "function create_dialog_%s()\n", title);
  fprintf(file, "  local containers = {}\n\n");

  iLayoutExportContainerLua(file, dialog);
  iupAttribSet(dialog, "_IUP_CONTAINER_INDEX", NULL);  /* clear when last used */

  fprintf(file, "  return containers[1]\n");
  fprintf(file, "end\n");
  free(title);
}

static void iLayoutExportElementLED(FILE* file, Ihandle* ih, const char* name, int indent_level)
{
  int i, count, indent_count = 0;
  const char* format = ih->iclass->format;
  char classname[100];
  char indent[300] = "";

  /* constructor indentation */
  if (indent_level)
  {
    indent_count = indent_level * 4;
    for (i = 0; i < indent_count; i++)
      indent[i] = ' ';
  }

  iupStrUpper(classname, ih->iclass->name);
  if (name)
    fprintf(file, "%s = %s[\n", name, classname);  /* start of attributes */
  else
    fprintf(file, "%s%s[\n", indent, classname);

  /* attributes indentation */
  for (i = indent_count; i < indent_count + 2; i++)
    indent[i] = ' ';
  indent_count += 2;

  if (iLayoutExportElementAttribs(file, ih, indent, -1) != 0)  /* LED */
    fprintf(file, "]"); /* end of attributes (no new line) */

  if (!format)
    fprintf(file, "()");
  else
  {
    if (*format == 'h' || *format == 'g')
    {
      Ihandle *child;

      fprintf(file, "(\n");

      /* children indentation */
      for (i = indent_count; i < indent_count + 2; i++)
        indent[i] = ' ';
      indent_count += 2;

      for (child = ih->firstchild; child; child = child->brother)
      {
        if (!(child->flags & IUP_INTERNAL))
        {
          char* childname = iLayoutGetName(child);
          if (!childname)
            iLayoutExportElementLED(file, child, NULL, indent_level + 1);   /* here process the ones that does NOT have names */
          else
            fprintf(file, "%s%s", indent, childname);

          if (child->brother)
            fprintf(file, ",\n");
        }
      }

      fprintf(file, ")");
    }
    else
    {
      count = (int)strlen(format);

      fprintf(file, "(");

      for (i = 0; i < count; i++)
      {
        if (format[i] == 's')
          fprintf(file, "\"\"");  /* empty string, let the job to the attributes */
        else if (format[i] == 'a')
        {
          char* cb_name = iupGetCallbackName(ih, "ACTION");
          if (!cb_name)
            cb_name = iupGetCallbackName(ih, "ACTION_CB");
          if (cb_name && !iupATTRIB_ISINTERNAL(cb_name))
            fprintf(file, "%s", cb_name);
          else
            fprintf(file, "do_nothing");  /* dummy name */
        }
        if (i != count - 1)
          fprintf(file, ", ");
      }

      fprintf(file, ")");
    }
  }

  if (name)
    fprintf(file, "\n\n");
}

static void iLayoutExportChildrenLED(FILE* file, Ihandle* ih)
{
  Ihandle *child;
  char* name;

  /* export children first */
  for (child = ih->firstchild; child; child = child->brother)
  {
    if (!(child->flags & IUP_INTERNAL))
      iLayoutExportChildrenLED(file, child);
  }

  name = iLayoutGetName(ih);
  if (name)  /* here process only the ones that have names */
    iLayoutExportElementLED(file, ih, name, 0);
}

static void iLayoutExportDialogLED(FILE* file, Ihandle* dialog, const char* filename)
{
  char* title = NULL;
  char* name = IupGetName(dialog);
  if (!name)
  {
    title = iupStrFileGetTitle(filename);
    iLayoutRemoveExt(title, "led");
    iupAttribSet(dialog, "_IUP_DIALOG_NAME", title);
  }

  fprintf(file, "#   Generated by IupLayoutDialog export to LED.\n\n");

  iLayoutExportChildrenLED(file, dialog);

  if (title)
    free(title);
}

static void iLayoutExportDialog(Ihandle* dialog, const char* filename, const char* format)
{
  FILE* file = fopen(filename, "wb");
  if (!file)
    return;

  if (iupStrEqualNoCase(format, "LED"))
    iLayoutExportDialogLED(file, dialog, filename);
  else if (iupStrEqualNoCase(format, "LUA"))
    iLayoutExportDialogLua(file, dialog, filename);
  else if (iupStrEqualNoCase(format, "C"))
    iLayoutExportDialogC(file, dialog, filename);

  fclose(file);
}

static int iLayoutGetExportFile(Ihandle* parent, char* filename)
{
  Ihandle *file_dlg = 0;
  int ret;
  char filter[4096] = "*.*";
  static char dir[4096] = "";  /* static will make the dir persist from one call to another if not defined */

  file_dlg = IupFileDlg();

  iupStrFileNameSplit(filename, dir, filter);

  IupSetAttribute(file_dlg, "FILTER", filter);
  IupSetAttribute(file_dlg, "DIRECTORY", dir);
  IupSetAttribute(file_dlg, "DIALOGTYPE", "SAVE");
  IupSetAttribute(file_dlg, "ALLOWNEW", "YES");
  IupSetAttribute(file_dlg, "NOCHANGEDIR", "YES");
  IupSetAttributeHandle(file_dlg, "PARENTDIALOG", parent);
  IupSetAttribute(file_dlg, "ICON", IupGetGlobal("ICON"));

  IupPopup(file_dlg, IUP_CENTERPARENT, IUP_CENTERPARENT);

  ret = IupGetInt(file_dlg, "STATUS");
  if (ret != -1)
  {
    char* value = IupGetAttribute(file_dlg, "VALUE");
    if (value)
    {
      strcpy(filename, value);
      iupStrFileNameSplit(filename, dir, NULL);
    }
  }

  IupDestroy(file_dlg);

  return ret;
}


/***************************************************************************
                             Layout Dialog Menus
***************************************************************************/


static int iLayoutMenuNew_CB(Ihandle* ih)
{
  Ihandle* dlg = IupGetDialog(ih);
  iLayoutDialog* layoutdlg = (iLayoutDialog*)iupAttribGet(dlg, "_IUP_LAYOUTDIALOG");
  if (layoutdlg->destroy)
    IupDestroy(layoutdlg->dialog);
  layoutdlg->dialog = IupDialog(NULL);
  layoutdlg->destroy = 1;
  iLayoutTreeRebuild(layoutdlg);
  return IUP_DEFAULT;
}

static int iLayoutMenuReload_CB(Ihandle* ih)
{
  Ihandle* dlg = IupGetDialog(ih);
  iLayoutDialog* layoutdlg = (iLayoutDialog*)iupAttribGet(dlg, "_IUP_LAYOUTDIALOG");
  iLayoutTreeRebuild(layoutdlg);
  return IUP_DEFAULT;
}

static int iLayoutMenuExportLED_CB(Ihandle* ih)
{
  Ihandle* dlg = IupGetDialog(ih);
  iLayoutDialog* layoutdlg = (iLayoutDialog*)iupAttribGet(dlg, "_IUP_LAYOUTDIALOG");
  char filename[4096] = "*.led";
  int ret = iLayoutGetExportFile(dlg, filename);
  if (ret != -1) /* ret==0 existing file. TODO: replace existing contents. */
    iLayoutExportDialog(layoutdlg->dialog, filename, "LED");
  return IUP_DEFAULT;
}

static int iLayoutMenuExportLua_CB(Ihandle* ih)
{
  Ihandle* dlg = IupGetDialog(ih);
  iLayoutDialog* layoutdlg = (iLayoutDialog*)iupAttribGet(dlg, "_IUP_LAYOUTDIALOG");
  char filename[4096] = "*.lua";
  int ret = iLayoutGetExportFile(dlg, filename);
  if (ret != -1) /* ret==0 existing file. TODO: replace existing contents. */
    iLayoutExportDialog(layoutdlg->dialog, filename, "Lua");
  return IUP_DEFAULT;
}

static int iLayoutMenuExportC_CB(Ihandle* ih)
{
  Ihandle* dlg = IupGetDialog(ih);
  iLayoutDialog* layoutdlg = (iLayoutDialog*)iupAttribGet(dlg, "_IUP_LAYOUTDIALOG");
  char filename[4096] = "*.c";
  int ret = iLayoutGetExportFile(dlg, filename);
  if (ret != -1) /* ret==0 existing file. TODO: replace existing contents. */
    iLayoutExportDialog(layoutdlg->dialog, filename, "C");
  return IUP_DEFAULT;
}

static int iLayoutMenuClose_CB(Ihandle* ih)
{
  Ihandle* dlg = IupGetDialog(ih);
  if (IupGetInt(dlg, "DESTROYWHENCLOSED"))
  {
    IupDestroy(dlg);
    return IUP_IGNORE;
  }
  else
  {
    IupHide(dlg);
    return IUP_DEFAULT;
  }
}

static int iLayoutMenuHierarchy_CB(Ihandle* ih)
{
  Ihandle* dlg = IupGetDialog(ih);
  Ihandle* split = IupGetChild(IupGetChild(dlg, 0), 0);
  if (!IupGetInt(ih, "VALUE"))
    IupSetAttribute(split, "VALUE", "0");
  else
    IupSetAttribute(split, "VALUE", "300");
  return IUP_DEFAULT;
}

static int iLayoutMenuRefresh_CB(Ihandle* ih)
{
  Ihandle* dlg = IupGetDialog(ih);
  iLayoutDialog* layoutdlg = (iLayoutDialog*)iupAttribGet(dlg, "_IUP_LAYOUTDIALOG");
  iLayoutUpdateLayout(layoutdlg);
  return IUP_DEFAULT;
}

static int iLayoutTimerAutoUpdate_CB(Ihandle* ih)
{
  iLayoutDialog* layoutdlg = (iLayoutDialog*)iupAttribGet(ih, "_IUP_LAYOUTDIALOG");
  /* redraw canvas */
  IupUpdate(IupGetBrother(layoutdlg->tree));
  return IUP_DEFAULT;
}

static int iLayoutMenuShowHidden_CB(Ihandle* ih)
{
  Ihandle* dlg = IupGetDialog(ih);
  iLayoutDialog* layoutdlg = (iLayoutDialog*)iupAttribGet(dlg, "_IUP_LAYOUTDIALOG");
  if (IupGetInt(dlg, "SHOWHIDDEN"))
    iupAttribSet(dlg, "SHOWHIDDEN", "No");
  else
    iupAttribSet(dlg, "SHOWHIDDEN", "Yes");
  /* redraw canvas */
  IupUpdate(IupGetBrother(layoutdlg->tree));
  return IUP_DEFAULT;
}

static int iLayoutMenuShowInternal_CB(Ihandle* ih)
{
  Ihandle* dlg = IupGetDialog(ih);
  iLayoutDialog* layoutdlg = (iLayoutDialog*)iupAttribGet(dlg, "_IUP_LAYOUTDIALOG");
  if (IupGetInt(dlg, "SHOWINTERNAL"))
    iupAttribSet(dlg, "SHOWINTERNAL", "No");
  else
    iupAttribSet(dlg, "SHOWINTERNAL", "Yes");
  /* rebuild tree and redraw canvas */
  iLayoutTreeRebuild(layoutdlg);
  return IUP_DEFAULT;
}

static int iLayoutMenuAutoUpdate_CB(Ihandle* ih)
{
  Ihandle* dlg = IupGetDialog(ih);
  iLayoutDialog* layoutdlg = (iLayoutDialog*)iupAttribGet(dlg, "_IUP_LAYOUTDIALOG");
  if (IupGetInt(layoutdlg->timer, "RUN"))
    IupSetAttribute(layoutdlg->timer, "RUN", "No");
  else
    IupSetAttribute(layoutdlg->timer, "RUN", "Yes");
  return IUP_DEFAULT;
}

static int iLayoutMenuUpdate_CB(Ihandle* ih)
{
  Ihandle* dlg = IupGetDialog(ih);
  iLayoutDialog* layoutdlg = (iLayoutDialog*)iupAttribGet(dlg, "_IUP_LAYOUTDIALOG");
  /* rebuild tree and redraw canvas */
  iLayoutTreeRebuild(layoutdlg);
  return IUP_DEFAULT;
}

static int iLayoutMenuRedraw_CB(Ihandle* ih)
{
  Ihandle* dlg = IupGetDialog(ih);
  iLayoutDialog* layoutdlg = (iLayoutDialog*)iupAttribGet(dlg, "_IUP_LAYOUTDIALOG");
  IupRedraw(layoutdlg->dialog, 1);
  return IUP_DEFAULT;
}

static int iLayoutGetParamOpacity_CB(Ihandle* dialog, int param_index, void* user_data)
{
  if (param_index == 0)
  {
    Ihandle* dlg = (Ihandle*)user_data;
    Ihandle* param = (Ihandle*)IupGetAttribute(dialog, "PARAM0");
    int opacity = IupGetInt(param, "VALUE");
    IupSetInt(dlg, "OPACITY", opacity);
  }
  return 1;
}

static int iLayoutMenuOpacity_CB(Ihandle* ih)
{
  Ihandle* dlg = IupGetDialog(ih);
  int opacity = IupGetInt(dlg, "OPACITY");
  if (opacity == 0)
    opacity = 255;

  if (IupGetParam("Dialog Layout", iLayoutGetParamOpacity_CB, dlg,
                  "Opacity: %i[0,255]\n",
                  &opacity, NULL))
  {

    if (opacity == 0 || opacity == 255)
      IupSetAttribute(dlg, "OPACITY", NULL);
    else
      IupSetInt(dlg, "OPACITY", opacity);
  }

  return IUP_DEFAULT;
}

static int iLayoutMenuFind_CB(Ihandle* ih)
{
  Ihandle* dlg = IupGetDialog(ih);
  iLayoutDialog* layoutdlg = (iLayoutDialog*)iupAttribGet(dlg, "_IUP_LAYOUTDIALOG");
  Ihandle* find_dlg = (Ihandle*)IupGetAttribute(ih, "FIND_DIALOG");
  Ihandle* tree = IupGetDialogChild(ih, "TREE");

  if (!find_dlg)
    find_dlg = iLayoutCreateFindDialog(layoutdlg, tree);

  IupShow(find_dlg);

  return IUP_DEFAULT;
}

static int iLayoutMenuShow_CB(Ihandle* ih)
{
  Ihandle* dlg = IupGetDialog(ih);
  iLayoutDialog* layoutdlg = (iLayoutDialog*)iupAttribGet(dlg, "_IUP_LAYOUTDIALOG");
  IupShow(layoutdlg->dialog);
  return IUP_DEFAULT;
}

static int iLayoutMenuHide_CB(Ihandle* ih)
{
  Ihandle* dlg = IupGetDialog(ih);
  iLayoutDialog* layoutdlg = (iLayoutDialog*)iupAttribGet(dlg, "_IUP_LAYOUTDIALOG");
  IupHide(layoutdlg->dialog);
  return IUP_DEFAULT;
}

static void iLayoutDialogLoad(Ihandle* parent_dlg, iLayoutDialog* layoutdlg, int only_visible)
{
  int ret, count, i;
  Ihandle* *dlg_list;
  char* *dlg_list_str;
  Ihandle *dlg;

  if (only_visible)
    count = iupDlgListVisibleCount();
  else
    count = iupDlgListCount();

  dlg_list = (Ihandle**)malloc(count*sizeof(Ihandle*));
  dlg_list_str = (char**)malloc(count*sizeof(char*));

  for (dlg = iupDlgListFirst(), i = 0; dlg && i < count; dlg = iupDlgListNext())
  {
    if (!only_visible ||
        (dlg->handle && IupGetInt(dlg, "VISIBLE")))
    {
      dlg_list[i] = dlg;
      dlg_list_str[i] = iupStrDup(iLayoutGetElementTreeTitle(dlg));
      i++;
    }
  }

  iupASSERT(i == count);
  if (i != count)
    count = i;

  IupStoreGlobal("_IUP_OLD_PARENTDIALOG", IupGetGlobal("PARENTDIALOG"));
  IupSetAttributeHandle(NULL, "PARENTDIALOG", parent_dlg);

  ret = IupListDialog(1, "Dialogs", count, (const char**)dlg_list_str, 1, 15, count < 15 ? count + 1 : 15, NULL);

  IupStoreGlobal("PARENTDIALOG", IupGetGlobal("_IUP_OLD_PARENTDIALOG"));
  IupSetGlobal("_IUP_OLD_PARENTDIALOG", NULL);

  if (ret != -1)
  {
    int w = 0, h = 0;

    if (layoutdlg->destroy)
      IupDestroy(layoutdlg->dialog);
    layoutdlg->dialog = dlg_list[ret];
    layoutdlg->destroy = 0;

    IupGetIntInt(layoutdlg->dialog, "CLIENTSIZE", &w, &h);
    if (w && h)
    {
      int scrollbar = IupGetInt(NULL, "SCROLLBARSIZE");
      int pw = 0, ph = 0;
      int prop = IupGetInt(IupGetParent(layoutdlg->tree), "VALUE");
      int status = IupGetInt2(IupGetBrother(IupGetParent(layoutdlg->tree)), "RASTERSIZE");
      w = (w * 1000) / (1000 - prop);
      IupGetIntInt(parent_dlg, "CLIENTSIZE", &pw, &ph);
      ph -= status + scrollbar;
      pw -= scrollbar;
      if (w > pw || h > ph)
      {
        IupSetfAttribute(parent_dlg, "CLIENTSIZE", "%dx%d", w + scrollbar + 10, h + scrollbar + status + 10);
        IupShow(parent_dlg);
      }
    }
  }

  for (i = 0; i < count; i++)
    free(dlg_list_str[i]);

  free(dlg_list);
  free(dlg_list_str);
}

static int iLayoutMenuLoad_CB(Ihandle* ih)
{
  Ihandle* dlg = IupGetDialog(ih);
  iLayoutDialog* layoutdlg = (iLayoutDialog*)iupAttribGet(dlg, "_IUP_LAYOUTDIALOG");
  iLayoutDialogLoad(dlg, layoutdlg, 0);
  iLayoutTreeRebuild(layoutdlg);
  return IUP_DEFAULT;
}

static int iLayoutMenuLoadVisible_CB(Ihandle* ih)
{
  Ihandle* dlg = IupGetDialog(ih);
  iLayoutDialog* layoutdlg = (iLayoutDialog*)iupAttribGet(dlg, "_IUP_LAYOUTDIALOG");
  iLayoutDialogLoad(dlg, layoutdlg, 1);
  iLayoutTreeRebuild(layoutdlg);
  return IUP_DEFAULT;
}


/***************************************************************************
                               Canvas Drawing
***************************************************************************/


static void iLayoutDrawElement(IdrawCanvas* dc, Ihandle* ih, int marked, int native_parent_x, int native_parent_y)
{
  int x, y, w, h;
  char *bgcolor;
  long color, fg, fg_void, bg, fg_max;

  bg = iupDrawColor(255, 255, 255, 255);  /* background color */
  fg = iupDrawColor(0, 0, 0, 255);        /* foreground color */
  fg_void = iupDrawColor(164, 164, 164, 255);  /* foreground color for void elements */
  fg_max = iupDrawColor(255, 0, 0, 255);      /* foreground color for elements that are maximizing parent size */

  x = ih->x + native_parent_x;
  y = ih->y + native_parent_y;
  w = ih->currentwidth;
  h = ih->currentheight;
  if (w <= 0) w = 1;
  if (h <= 0) h = 1;

  bgcolor = IupGetAttribute(ih, "BGCOLOR");
  if (bgcolor && ih->iclass->nativetype != IUP_TYPEVOID)
  {
    color = iupDrawStrToColor(bgcolor, bg);
    iupdrvDrawRectangle(dc, x, y, x + w - 1, y + h - 1, color, IUP_DRAW_FILL, 1);
  }

  if (ih->iclass->nativetype == IUP_TYPEVOID)
    iupdrvDrawRectangle(dc, x, y, x + w - 1, y + h - 1, fg_void, IUP_DRAW_STROKE_DASH, 1);
  else
    iupdrvDrawRectangle(dc, x, y, x + w - 1, y + h - 1, fg, IUP_DRAW_STROKE, 1);

  iupdrvDrawSetClipRect(dc, x, y, x + w - 1, y + h - 1);

  if (ih->iclass->childtype == IUP_CHILDNONE)
  {
    int pw, ph;
    IupGetIntInt(ih->parent, "CLIENTSIZE", &pw, &ph);

    if (ih->currentwidth == pw && ih->currentwidth == ih->naturalwidth)
    {
      iupdrvDrawLine(dc, x + 1, y + 1, x + w - 2, y + 1, fg_max, IUP_DRAW_STROKE, 1);
      iupdrvDrawLine(dc, x + 1, y + h - 2, x + w - 2, y + h - 2, fg_max, IUP_DRAW_STROKE, 1);
    }

    if (ih->currentheight == ph && ih->currentheight == ih->naturalheight)
    {
      iupdrvDrawLine(dc, x + 1, y + 1, x + 1, y + h - 2, fg_max, IUP_DRAW_STROKE, 1);
      iupdrvDrawLine(dc, x + w - 2, y + 1, x + w - 2, y + h - 2, fg_max, IUP_DRAW_STROKE, 1);
    }
  }
  else if (ih->iclass->nativetype != IUP_TYPEVOID)
  {
    /* if ih is a Tabs, position the title accordingly */
    if (IupClassMatch(ih, "tabs"))
    {
      /* TABORIENTATION is ignored */
      char* tabtype = IupGetAttribute(ih, "TABTYPE");
      if (iupStrEqualNoCase(tabtype, "BOTTOM"))
      {
        int cw = 0, ch = 0;
        IupGetIntInt(ih, "CLIENTSIZE", &cw, &ch);
        y += ch;  /* position after the client area */
      }
      else if (iupStrEqualNoCase(tabtype, "RIGHT"))
      {
        int cw = 0, ch = 0;
        IupGetIntInt(ih, "CLIENTSIZE", &cw, &ch);
        x += cw;  /* position after the client area */
      }
    }
  }

  /* always draw the image first */
  if (ih->iclass->nativetype != IUP_TYPEVOID)
  {
    char *title, *image;

    image = IupGetAttribute(ih, "IMAGE0");  /* Tree root node title */
    if (!image)
      image = IupGetAttribute(ih, "TABIMAGE0");  /* Tabs first tab image */
    if (image)
    {
      /* returns the image of the active tab */
      int pos = IupGetInt(ih, "VALUEPOS");
      image = IupGetAttributeId(ih, "TABIMAGE", pos);
    }
    if (!image)
      image = IupGetAttribute(ih, "IMAGE");
    if (image)
    {
      char* position;
      int img_w, img_h;

      iupImageGetInfo(image, &img_w, &img_h, NULL);

      iupdrvDrawImage(dc, image, 0, x + 1, y + 1);

      position = IupGetAttribute(ih, "IMAGEPOSITION");  /* used only for buttons */
      if (position &&
          (iupStrEqualNoCase(position, "BOTTOM") ||
          iupStrEqualNoCase(position, "TOP")))
          y += img_h;
      else
        x += img_w;  /* position text usually at right */
    }

    title = IupGetAttribute(ih, "0:0");  /* Matrix title cell */
    if (!title)
      title = IupGetAttribute(ih, "1");  /* List first item */
    if (!title)
      title = IupGetAttribute(ih, "TITLE0");  /* Tree root node title */
    if (!title)
    {
      title = IupGetAttribute(ih, "TABTITLE0");  /* Tabs first tab title */
      if (title)
      {
        /* returns the title of the active tab */
        int pos = IupGetInt(ih, "VALUEPOS");
        title = IupGetAttributeId(ih, "TABTITLE", pos);
      }
    }
    if (!title)
      title = iupAttribGetLocal(ih, "TITLE");
    if (title)
    {
      int len;
      iupStrNextLine(title, &len);  /* get the size of the first line */
      color = iupDrawStrToColor(IupGetAttribute(ih, "FGCOLOR"), fg);
      iupdrvDrawText(dc, title, len, x + 1, y + 1, w, h, color, IupGetAttribute(ih, "FONT"), IUP_ALIGN_ALEFT);
    }

    if (ih->iclass->childtype == IUP_CHILDNONE &&
        !title && !image)
    {
      if (IupClassMatch(ih, "progressbar") || IupClassMatch(ih, "gauge"))
      {
        double min = IupGetDouble(ih, "MIN");
        double max = IupGetDouble(ih, "MAX");
        double val = IupGetDouble(ih, "VALUE");
        color = iupDrawStrToColor(IupGetAttribute(ih, "FGCOLOR"), fg);
        if (iupStrEqualNoCase(IupGetAttribute(ih, "ORIENTATION"), "VERTICAL"))
        {
          int ph = (int)(((max - val)*(h - 5)) / (max - min));
          iupdrvDrawRectangle(dc, x + 2, y + 2, x + w - 3, y + ph, color, IUP_DRAW_FILL, 1);
        }
        else
        {
          int pw = (int)(((val - min)*(w - 5)) / (max - min));
          iupdrvDrawRectangle(dc, x + 2, y + 2, x + pw, y + h - 3, color, IUP_DRAW_FILL, 1);
        }
      }
      else if (IupClassMatch(ih, "val"))
      {
        double min = IupGetDouble(ih, "MIN");
        double max = IupGetDouble(ih, "MAX");
        double val = IupGetDouble(ih, "VALUE");
        color = iupDrawStrToColor(IupGetAttribute(ih, "FGCOLOR"), fg);
        if (iupStrEqualNoCase(IupGetAttribute(ih, "ORIENTATION"), "VERTICAL"))
        {
          int ph = (int)(((max - val)*(h - 5)) / (max - min));
          iupdrvDrawRectangle(dc, x + 2, y + ph - 1, x + w - 3, y + ph + 1, color, IUP_DRAW_FILL, 1);
        }
        else
        {
          int pw = (int)(((val - min)*(w - 5)) / (max - min));
          iupdrvDrawRectangle(dc, x + pw - 1, y + 2, x + pw + 1, y + h - 3, color, IUP_DRAW_FILL, 1);
        }
      }
    }
  }

  iupdrvDrawResetClip(dc);

  if (marked)
  {
    x = ih->x + native_parent_x;
    y = ih->y + native_parent_y;
    w = ih->currentwidth;
    h = ih->currentheight;
    if (w <= 0) w = 1;
    if (h <= 0) h = 1;

    iupdrvDrawSelectRect(dc, x, y, x + w, y + h);
  }
}

static int iLayoutElementIsVisible(Ihandle* ih, int dlgvisible)
{
  if (dlgvisible)
    return iupStrBoolean(iupAttribGetLocal(ih, "VISIBLE"));
  else
  {
    /* can not check at native implementation because it will be always not visible */
    char* value = iupAttribGet(ih, "VISIBLE");
    if (!value)
      return 1; /* default is visible */
    else
      return iupStrBoolean(value);
  }
}

static void iLayoutDrawElementTree(IdrawCanvas* dc, int showhidden, int dlgvisible, int shownotmapped, int showinternal, Ihandle* mark, Ihandle* ih, int native_parent_x, int native_parent_y)
{
  Ihandle *child;
  int dx, dy;

  if ((showhidden || iLayoutElementIsVisible(ih, dlgvisible)) &&
      (shownotmapped || ih->handle))
  {
    /* draw the element */
    iLayoutDrawElement(dc, ih, ih == mark, native_parent_x, native_parent_y);

    if (ih->iclass->childtype != IUP_CHILDNONE)
    {
      /* if ih is a native parent, then update the offset */
      if (ih->iclass->nativetype != IUP_TYPEVOID)
      {
        dx = 0, dy = 0;
        IupGetIntInt(ih, "CLIENTOFFSET", &dx, &dy);
        native_parent_x += ih->x + dx;
        native_parent_y += ih->y + dy;

        /* if ih is a Tabs, then draw only the active child */
        if (IupClassMatch(ih, "tabs") || IupClassMatch(ih, "flattabs"))
        {
          child = (Ihandle*)IupGetAttribute(ih, "VALUE_HANDLE");
          if (child)
            iLayoutDrawElementTree(dc, showhidden, dlgvisible, shownotmapped, showinternal, mark, child, native_parent_x, native_parent_y);
          return;
        }
      }
    }

    /* draw its children */
    for (child = ih->firstchild; child; child = child->brother)
    {
      if (!(child->flags & IUP_INTERNAL) || showinternal)
        iLayoutDrawElementTree(dc, showhidden, dlgvisible, shownotmapped, showinternal, mark, child, native_parent_x, native_parent_y);
    }
  }
}

static void iLayoutDrawDialog(iLayoutDialog* layoutdlg, int showhidden, int showinternal, IdrawCanvas* dc, Ihandle* mark, int posx, int posy)
{
  int w, h;

  iupdrvDrawGetSize(dc, &w, &h);
  iupdrvDrawRectangle(dc, 0, 0, w - 1, h - 1, iupDrawColor(255, 255, 255, 255), IUP_DRAW_FILL, 1);

  /* draw the dialog */
  IupGetIntInt(layoutdlg->dialog, "CLIENTSIZE", &w, &h);
  iupdrvDrawRectangle(dc, 0, 0, w - 1, h - 1, iupDrawColor(0, 0, 0, 255), IUP_DRAW_STROKE, 1);

  if (layoutdlg->dialog->firstchild)
  {
    int native_parent_x = 0, native_parent_y = 0;
    int shownotmapped = layoutdlg->dialog->handle == NULL;  /* only show not mapped if dialog is also not mapped */
    int dlgvisible = IupGetInt(layoutdlg->dialog, "VISIBLE");
    IupGetIntInt(layoutdlg->dialog, "CLIENTOFFSET", &native_parent_x, &native_parent_y);
    native_parent_x -= posx;
    native_parent_y -= posy;
    iLayoutDrawElementTree(dc, showhidden, dlgvisible, shownotmapped, showinternal, mark, layoutdlg->dialog->firstchild, native_parent_x, native_parent_y);
  }
}

static int iLayoutCanvas_CB(Ihandle* canvas, float fposx, float fposy)
{
  Ihandle* dlg = IupGetDialog(canvas);
  iLayoutDialog* layoutdlg = (iLayoutDialog*)iupAttribGet(dlg, "_IUP_LAYOUTDIALOG");
  IdrawCanvas* dc = iupdrvDrawCreateCanvas(canvas);
  int showhidden = IupGetInt(dlg, "SHOWHIDDEN");
  int showinternal = IupGetInt(dlg, "SHOWINTERNAL");
  Ihandle* mark = (Ihandle*)iupAttribGet(dlg, "_IUPLAYOUT_MARK");

  iLayoutDrawDialog(layoutdlg, showhidden, showinternal, dc, mark, (int)fposx, (int)fposy);

  iupdrvDrawFlush(dc);

  iupdrvDrawKillCanvas(dc);

  return IUP_DEFAULT;
}


/***************************************************************************
                        Element Properties Dialog
***************************************************************************/


static void iLayoutPropertiesUpdate(Ihandle* properties, Ihandle* ih)
{
  int i, j, attr_count, cb_count, total_count = IupGetClassAttributes(ih->iclass->name, NULL, 0);
  char **attr_names = (char **)malloc(total_count * sizeof(char *));
  Ihandle* list1 = (Ihandle*)iupAttribGet(properties, "_IUP_PROPLIST1");
  Ihandle* list2 = (Ihandle*)iupAttribGet(properties, "_IUP_PROPLIST2");
  Ihandle* list3 = (Ihandle*)iupAttribGet(properties, "_IUP_PROPLIST3");

  /* Clear everything */
  IupSetAttribute(list1, "REMOVEITEM", NULL);
  IupSetAttribute(list2, "REMOVEITEM", NULL);
  IupSetAttribute(list3, "REMOVEITEM", NULL);
  IupSetAttribute(IupGetDialogChild(properties, "VALUE1A"), "VALUE", "");
  IupSetAttribute(IupGetDialogChild(properties, "VALUE1B"), "TITLE", "");
  IupSetAttribute(IupGetDialogChild(properties, "VALUE1C"), "TITLE", "");
  IupSetAttribute(IupGetDialogChild(properties, "VALUE2"), "VALUE", "");
  IupSetAttribute(IupGetDialogChild(properties, "VALUE3"), "TITLE", "");
  IupSetAttribute(IupGetDialogChild(properties, "SETBUT"), "ACTIVE", "No");
  IupSetAttribute(IupGetDialogChild(properties, "SETCOLORBUT"), "VISIBLE", "No");
  IupSetAttribute(IupGetDialogChild(properties, "SETFONTBUT"), "VISIBLE", "No");
  IupSetAttribute(IupGetDialogChild(properties, "SHOWIDLIST"), "VISIBLE", "No");

  attr_count = IupGetClassAttributes(ih->iclass->name, attr_names, total_count);
  for (i = 0; i < attr_count; i++)
    IupSetAttributeId(list1, "", i + 1, attr_names[i]);

  cb_count = total_count - attr_count;
  IupGetClassCallbacks(ih->iclass->name, attr_names, cb_count);
  for (i = 0; i<cb_count; i++)
    IupSetAttributeId(list3, "", i + 1, attr_names[i]);

  attr_count = IupGetAllAttributes(ih, NULL, 0);
  if (attr_count > total_count)
    attr_names = (char **)realloc(attr_names, attr_count * sizeof(char *));

  IupGetAllAttributes(ih, attr_names, attr_count);
  for (i = 0, j = 1; i < attr_count; i++)
  {
    if (!iupClassAttribIsRegistered(ih->iclass, attr_names[i]))
    {
      IupSetAttributeId(list2, "", j, attr_names[i]);
      j++;
    }
  }

  iupAttribSet(properties, "_IUP_PROPELEMENT", (char*)ih);

  IupStoreAttribute(IupGetDialogChild(properties, "ELEMTITLE"), "TITLE", iLayoutGetElementTreeTitle(ih));

  free(attr_names);
}

static int iLayoutPropertiesClose_CB(Ihandle* ih)
{
  IupHide(IupGetDialog(ih));
  return IUP_DEFAULT;
}

static int iLayoutPropertiesShowId_CB(Ihandle* showidlist, char *id, int item, int state)
{
  (void)item;
  if (state)
  {
    char* def_value;
    int flags;
    char *value, name[100];
    Ihandle* elem = (Ihandle*)iupAttribGetInherit(showidlist, "_IUP_PROPELEMENT");
    Ihandle* txt1 = IupGetDialogChild(showidlist, "VALUE1A");
    Ihandle* list1 = (Ihandle*)iupAttribGetInherit(showidlist, "_IUP_PROPLIST1");
    char* itemlist1 = IupGetAttribute(list1, "VALUE");
    if (!itemlist1)
      return IUP_DEFAULT;

    strcpy(name, IupGetAttribute(list1, itemlist1));

    iupClassGetAttribNameInfo(elem->iclass, name, &def_value, &flags);

    strcat(name, id);

    value = IupGetAttribute(elem, name);
    if (value)
    {
      if (flags&IUPAF_NO_STRING)
        IupSetfAttribute(txt1, "VALUE", "%p", value);
      else
        IupStoreAttribute(txt1, "VALUE", value);
    }
    else
      IupSetAttribute(txt1, "VALUE", "NULL");

    if (strstr(name, "COLOR") != NULL)
    {
      Ihandle* colorbut = IupGetDialogChild(showidlist, "SETCOLORBUT");
      IupStoreAttribute(colorbut, "BGCOLOR", value);
    }
  }
  return IUP_DEFAULT;
}

static int iLayoutPropertiesSet_CB(Ihandle* button)
{
  Ihandle* list1 = (Ihandle*)iupAttribGetInherit(button, "_IUP_PROPLIST1");
  char* item = IupGetAttribute(list1, "VALUE");
  if (item)
  {
    iLayoutDialog* layoutdlg = (iLayoutDialog*)iupAttribGetInherit(button, "_IUP_LAYOUTDIALOG");
    Ihandle* elem = (Ihandle*)iupAttribGetInherit(button, "_IUP_PROPELEMENT");
    Ihandle* txt1 = IupGetDialogChild(button, "VALUE1A");
    char* value = IupGetAttribute(txt1, "VALUE");
    char* name = IupGetAttribute(list1, item);
    Ihandle* showidlist = IupGetDialogChild(button, "SHOWIDLIST");

    if (IupGetInt(showidlist, "VISIBLE"))
    {
      char* item = IupGetAttribute(showidlist, "VALUE");
      if (item)
      {
        char* id = IupGetAttribute(showidlist, item);
        char nameid[100];
        sprintf(nameid, "%s%s", name, id);

        if (!value || iupStrEqual(value, "NULL"))
          IupSetAttribute(elem, nameid, NULL);
        else
          IupStoreAttribute(elem, nameid, value);
      }
    }
    else
    {
      if (!value || iupStrEqual(value, "NULL"))
        IupSetAttribute(elem, name, NULL);
      else
        IupStoreAttribute(elem, name, value);
    }

    if (strstr(name, "COLOR") != NULL)
    {
      Ihandle* colorbut = IupGetDialogChild(button, "SETCOLORBUT");
      IupStoreAttribute(colorbut, "BGCOLOR", value);
    }

    if (layoutdlg)
    {
      layoutdlg->changed = 1;

      /* redraw canvas */
      IupUpdate(IupGetBrother(layoutdlg->tree));
    }
  }
  return IUP_DEFAULT;
}

static int iLayoutPropertiesSetColor_CB(Ihandle *colorbut)
{
  Ihandle* color_dlg = IupColorDlg();
  IupSetAttributeHandle(color_dlg, "PARENTDIALOG", IupGetDialog(colorbut));
  IupSetAttribute(color_dlg, "TITLE", "Choose Color");
  IupStoreAttribute(color_dlg, "VALUE", IupGetAttribute(colorbut, "BGCOLOR"));

  IupPopup(color_dlg, IUP_CENTER, IUP_CENTER);

  if (IupGetInt(color_dlg, "STATUS") == 1)
  {
    iLayoutDialog* layoutdlg = (iLayoutDialog*)iupAttribGetInherit(colorbut, "_IUP_LAYOUTDIALOG");
    Ihandle* elem = (Ihandle*)iupAttribGetInherit(colorbut, "_IUP_PROPELEMENT");
    Ihandle* list1 = (Ihandle*)iupAttribGetInherit(colorbut, "_IUP_PROPLIST1");
    Ihandle* txt1 = IupGetDialogChild(colorbut, "VALUE1A");
    char* value = IupGetAttribute(color_dlg, "VALUE");
    char* name = IupGetAttribute(list1, IupGetAttribute(list1, "VALUE"));
    Ihandle* showidlist = IupGetDialogChild(colorbut, "SHOWIDLIST");

    IupStoreAttribute(txt1, "VALUE", value);
    IupStoreAttribute(colorbut, "BGCOLOR", value);

    if (IupGetInt(showidlist, "VISIBLE"))
    {
      char* item = IupGetAttribute(showidlist, "VALUE");
      if (item)
      {
        char* id = IupGetAttribute(showidlist, item);
        char nameid[100];
        sprintf(nameid, "%s%s", name, id);
        IupStoreAttribute(elem, nameid, value);
      }
    }
    else
      IupStoreAttribute(elem, name, value);

    if (layoutdlg)
    {
      layoutdlg->changed = 1;

      /* redraw canvas */
      IupUpdate(IupGetBrother(layoutdlg->tree));
    }
  }

  IupDestroy(color_dlg);

  return IUP_DEFAULT;
}

static int iLayoutPropertiesSetFont_CB(Ihandle *fontbut)
{
  Ihandle* font_dlg = IupFontDlg();
  Ihandle* txt1 = IupGetDialogChild(fontbut, "VALUE1A");
  IupSetAttributeHandle(font_dlg, "PARENTDIALOG", IupGetDialog(fontbut));
  IupSetAttribute(font_dlg, "TITLE", "Choose Font");
  IupStoreAttribute(font_dlg, "VALUE", IupGetAttribute(txt1, "VALUE"));

  IupPopup(font_dlg, IUP_CENTER, IUP_CENTER);

  if (IupGetInt(font_dlg, "STATUS") == 1)
  {
    iLayoutDialog* layoutdlg = (iLayoutDialog*)iupAttribGetInherit(fontbut, "_IUP_LAYOUTDIALOG");
    Ihandle* elem = (Ihandle*)iupAttribGetInherit(fontbut, "_IUP_PROPELEMENT");
    char* value = IupGetAttribute(font_dlg, "VALUE");

    IupStoreAttribute(txt1, "VALUE", value);
    IupStoreAttribute(elem, "FONT", value);

    if (layoutdlg)
    {
      layoutdlg->changed = 1;

      /* redraw canvas */
      IupUpdate(IupGetBrother(layoutdlg->tree));
    }
  }

  IupDestroy(font_dlg);

  return IUP_DEFAULT;
}

static void iLayoutPropertiesUpdateIdList(Ihandle *showidlist, Ihandle *ih, int has_id2)
{
  IupSetAttribute(showidlist, "REMOVEITEM", NULL);
  IupSetAttribute(showidlist, "VALUE", NULL);

  if (has_id2)
  {
    int lin, col,
      numcol = IupGetInt(ih, "NUMCOL") + 1,
      numlin = IupGetInt(ih, "NUMLIN") + 1;
    for (lin = 0; lin < numlin; lin++)
    {
      for (col = 0; col < numcol; col++)
        IupSetfAttributeId(showidlist, "", lin*numcol + col + 1, "%d:%d", lin, col);
    }
  }
  else
  {
    int id, start_id = 0,
      count = IupGetInt(ih, "COUNT");

    if (IupClassMatch(ih, "list"))
      start_id = 1;

    for (id = start_id; id < count + start_id; id++)
      IupSetIntId(showidlist, "", id - start_id + 1, id);
  }
}

static int iLayoutPropertiesList1_CB(Ihandle *list1, char *name, int item, int state)
{
  (void)item;
  if (state)
  {
    char* def_value;
    int flags;
    Ihandle* elem = (Ihandle*)iupAttribGetInherit(list1, "_IUP_PROPELEMENT");
    char* value = IupGetAttribute(elem, name);
    Ihandle* txt1 = IupGetDialogChild(list1, "VALUE1A");
    Ihandle* lbl2 = IupGetDialogChild(list1, "VALUE1B");
    Ihandle* lbl3 = IupGetDialogChild(list1, "VALUE1C");
    Ihandle* setbut = IupGetDialogChild(list1, "SETBUT");
    Ihandle* colorbut = IupGetDialogChild(list1, "SETCOLORBUT");
    Ihandle* fontbut = IupGetDialogChild(list1, "SETFONTBUT");
    Ihandle* showidlist = IupGetDialogChild(list1, "SHOWIDLIST");

    iupClassGetAttribNameInfo(elem->iclass, name, &def_value, &flags);

    if (value)
    {
      if (flags&IUPAF_NO_STRING)
        IupSetfAttribute(txt1, "VALUE", "%p", value);
      else
        IupStoreAttribute(txt1, "VALUE", value);
    }
    else
      IupSetAttribute(txt1, "VALUE", "NULL");

    if (def_value)
      IupStoreAttribute(lbl2, "TITLE", def_value);
    else
      IupSetAttribute(lbl2, "TITLE", "NULL");

    IupSetfAttribute(lbl3, "TITLE", "%s\n%s%s%s%s%s", flags&(IUPAF_NO_INHERIT | IUPAF_NO_STRING) ? "NON Inheritable" : "Inheritable",
                     flags&IUPAF_NO_STRING ? "NOT a String\n" : "",
                     flags&IUPAF_HAS_ID ? "Has ID\n" : "",
                     flags&IUPAF_READONLY ? "Read-Only\n" : (flags&IUPAF_WRITEONLY ? "Write-Only\n" : ""),
                     flags&IUPAF_IHANDLENAME ? "Ihandle* name\n" : "",
                     flags&IUPAF_NOT_SUPPORTED ? "NOT SUPPORTED in this driver" : "");

    if (iLayoutAttributeChanged(elem, name, value, def_value, flags))
      IupSetAttribute(txt1, "FGCOLOR", "255 0 0");
    else
      IupSetAttribute(txt1, "FGCOLOR", "0 0 0");

    if (!(flags&IUPAF_READONLY) &&
        !(flags&IUPAF_NO_STRING))
    {
      IupSetAttribute(setbut, "ACTIVE", "Yes");
      IupSetAttribute(txt1, "READONLY", "No");

      if (strstr(name, "COLOR") != NULL) /* if COLOR in attribute name, show the color selection button */
      {
        IupStoreAttribute(colorbut, "BGCOLOR", value);  /* set it even if it is NULL */
        IupSetAttribute(colorbut, "VISIBLE", "Yes");
      }
      else
        IupSetAttribute(colorbut, "VISIBLE", "No");

      if (iupStrEqual(name, "FONT"))
        IupSetAttribute(fontbut, "VISIBLE", "Yes");
      else
        IupSetAttribute(fontbut, "VISIBLE", "No");
    }
    else
    {
      IupSetAttribute(txt1, "READONLY", "Yes");
      IupSetAttribute(setbut, "ACTIVE", "No");
      IupSetAttribute(colorbut, "VISIBLE", "No");
      IupSetAttribute(fontbut, "VISIBLE", "No");
    }

    if (flags&IUPAF_HAS_ID)
    {
      IupSetAttribute(showidlist, "VISIBLE", "Yes");
      iLayoutPropertiesUpdateIdList(showidlist, elem, flags&IUPAF_HAS_ID2);
    }
    else
      IupSetAttribute(showidlist, "VISIBLE", "No");
  }

  return IUP_DEFAULT;
}

static int iLayoutPropertiesList2_CB(Ihandle *list2, char *name, int item, int state)
{
  (void)item;
  if (state)
  {
    Ihandle* elem = (Ihandle*)iupAttribGetInherit(list2, "_IUP_PROPELEMENT");
    char* value = iupAttribGet(elem, name);
    Ihandle* lbl = IupGetDialogChild(list2, "VALUE2");
    if (value)
      IupSetfAttribute(lbl, "VALUE", "%p", value);
    else
      IupSetAttribute(lbl, "VALUE", "NULL");
  }
  return IUP_DEFAULT;
}

static int iLayoutPropertiesGetAsString_CB(Ihandle *button)
{
  Ihandle* elem = (Ihandle*)iupAttribGetInherit(button, "_IUP_PROPELEMENT");
  Ihandle* list2 = (Ihandle*)iupAttribGetInherit(button, "_IUP_PROPLIST2");
  char* item = IupGetAttribute(list2, "VALUE");
  if (item)
  {
    char* value = iupAttribGet(elem, IupGetAttribute(list2, item));
    Ihandle* lbl = IupGetDialogChild(button, "VALUE2");
    if (value)
      IupStoreAttribute(lbl, "VALUE", value);
    else
      IupSetAttribute(lbl, "VALUE", "NULL");
  }
  return IUP_DEFAULT;
}

static int iLayoutPropertiesSetStr_CB(Ihandle* button)
{
  Ihandle* elem = (Ihandle*)iupAttribGetInherit(button, "_IUP_PROPELEMENT");
  char* name = IupGetAttribute(IupGetDialogChild(button, "NAME22"), "VALUE");
  char* value = IupGetAttribute(IupGetDialogChild(button, "VALUE22"), "VALUE");
  if (!value || iupStrEqual(value, "NULL"))
    IupSetAttribute(elem, name, NULL);
  else
    IupStoreAttribute(elem, name, value);

  iLayoutPropertiesUpdate(IupGetDialog(button), elem);

  return IUP_DEFAULT;
}

static int iLayoutPropertiesList3_CB(Ihandle *list3, char *text, int item, int state)
{
  (void)item;
  if (state)
  {
    Ihandle* elem = (Ihandle*)iupAttribGetInherit(list3, "_IUP_PROPELEMENT");
    Icallback cb = IupGetCallback(elem, text);
    Ihandle* lbl = IupGetDialogChild(list3, "VALUE3");
    if (cb)
    {
      char* name = iupGetCallbackName(elem, text);
      if (name)
        IupSetfAttribute(lbl, "TITLE", "%p\n\"%s\"", cb, name);
      else
        IupSetfAttribute(lbl, "TITLE", "%p", cb);
    }
    else
      IupSetAttribute(lbl, "TITLE", "NULL");
  }
  return IUP_DEFAULT;
}

static int iLayoutPropertiesTabChangePos_CB(Ihandle* ih, int new_pos, int old_pos)
{
  (void)old_pos;
  switch (new_pos)
  {
  case 0:
    IupSetAttribute(ih, "TIP", "All attributes that are known by the element.");
    break;
  case 1:
    IupSetAttribute(ih, "TIP", "Custom attributes set by the application.");
    break;
  case 2:
    IupSetAttribute(ih, "TIP", "All callbacks that are known by the element.");
    break;
  }

  /* In GTK the TIP appears for all children */
  /* TODO: move this code to iupgtk_tabs.c */
  if (iupStrEqualNoCase(IupGetGlobal("DRIVER"), "GTK"))
  {
    char* tabtype = IupGetAttribute(ih, "TABTYPE");
    int x = 0;
    int y = 0;
    int w = ih->currentwidth;
    int h = ih->currentheight;
    int cw = 0, ch = 0;

    IupGetIntInt(ih, "CLIENTSIZE", &cw, &ch);

    /* TABORIENTATION is ignored */
    if (iupStrEqualNoCase(tabtype, "BOTTOM"))
    {
      y += ch;  /* position after the client area */
      h -= ch;
    }
    else if (iupStrEqualNoCase(tabtype, "RIGHT"))
    {
      x += cw;  /* position after the client area */
      w -= cw;
    }
    else if (iupStrEqualNoCase(tabtype, "LEFT"))
      w -= cw;
    else  /* TOP */
      h -= ch;

    IupSetfAttribute(ih, "TIPRECT", "%d %d %d %d", x, y, x + w, y + h);
  }

  IupSetAttribute(ih, "TIPVISIBLE", "YES");
  return IUP_DEFAULT;
}

static Ihandle* iLayoutPropertiesCreateDialog(iLayoutDialog* layoutdlg, Ihandle* parent)
{
  Ihandle *list1, *list2, *list3, *close, *dlg, *dlg_box, *button_box, *colorbut, *fontbut,
    *tabs, *box1, *box11, *box2, *box22, *box3, *box33, *set, *showidlist;

  close = IupButton("Close", NULL);
  IupSetStrAttribute(close, "PADDING", IupGetGlobal("DEFAULTBUTTONPADDING"));
  IupSetCallback(close, "ACTION", (Icallback)iLayoutPropertiesClose_CB);

  button_box = IupHbox(
    IupFill(),
    close,
    NULL);
  IupSetAttribute(button_box, "MARGIN", "0x0");

  list1 = IupList(NULL);
  IupSetCallback(list1, "ACTION", (Icallback)iLayoutPropertiesList1_CB);
  IupSetAttribute(list1, "VISIBLELINES", "15");
  IupSetAttribute(list1, "VISIBLECOLUMNS", "11");
  IupSetAttribute(list1, "SORT", "Yes");
  IupSetAttribute(list1, "EXPAND", "VERTICAL");

  list2 = IupList(NULL);
  IupSetCallback(list2, "ACTION", (Icallback)iLayoutPropertiesList2_CB);
  IupSetAttribute(list2, "VISIBLELINES", "15");
  IupSetAttribute(list2, "VISIBLECOLUMNS", "11");
  IupSetAttribute(list2, "SORT", "Yes");
  IupSetAttribute(list2, "EXPAND", "VERTICAL");

  list3 = IupList(NULL);
  IupSetCallback(list3, "ACTION", (Icallback)iLayoutPropertiesList3_CB);
  IupSetAttribute(list3, "VISIBLELINES", "15");
  IupSetAttribute(list3, "VISIBLECOLUMNS", "14");
  IupSetAttribute(list3, "SORT", "Yes");
  IupSetAttribute(list3, "EXPAND", "VERTICAL");

  set = IupButton("Set", NULL);
  IupSetCallback(set, "ACTION", iLayoutPropertiesSet_CB);
  IupSetStrAttribute(set, "PADDING", IupGetGlobal("DEFAULTBUTTONPADDING"));
  IupSetAttribute(set, "NAME", "SETBUT");

  showidlist = IupList(NULL);
  IupSetCallback(showidlist, "ACTION", (Icallback)iLayoutPropertiesShowId_CB);
  IupSetAttribute(showidlist, "DROPDOWN", "Yes");
  IupSetAttribute(showidlist, "VISIBLECOLUMNS", "5");
  IupSetAttribute(showidlist, "NAME", "SHOWIDLIST");

  colorbut = IupButton(NULL, NULL);
  IupSetAttribute(colorbut, "SIZE", "20x10");
  IupStoreAttribute(colorbut, "BGCOLOR", "0 0 0");
  IupSetCallback(colorbut, "ACTION", (Icallback)iLayoutPropertiesSetColor_CB);
  IupSetAttribute(colorbut, "NAME", "SETCOLORBUT");
  IupSetAttribute(colorbut, "VISIBLE", "NO");

  fontbut = IupButton("F", NULL);
  IupSetAttribute(fontbut, "SIZE", "20x10");
  IupStoreAttribute(fontbut, "FONT", "Times, Bold Italic 12");
  IupSetCallback(fontbut, "ACTION", (Icallback)iLayoutPropertiesSetFont_CB);
  IupSetAttribute(fontbut, "NAME", "SETFONTBUT");
  IupSetAttribute(fontbut, "VISIBLE", "NO");

  box11 = IupVbox(
    IupLabel("Value:"),
    IupHbox(IupSetAttributes(IupText(NULL), "MULTILINE=Yes, ALIGNMENT=ALEFT:ATOP, EXPAND=YES, NAME=VALUE1A"), IupVbox(set, colorbut, fontbut, showidlist, NULL), NULL),
    IupSetAttributes(IupFill(), "RASTERSIZE=10"),
    IupLabel("Default Value:"),
    IupFrame(IupSetAttributes(IupLabel(NULL), "ALIGNMENT=ALEFT:ATOP, EXPAND=HORIZONTAL, NAME=VALUE1B")),
    IupSetAttributes(IupFill(), "RASTERSIZE=10"),
    IupLabel("Other Info:"),
    IupFrame(IupSetAttributes(IupLabel(NULL), "SIZE=90x48, ALIGNMENT=ALEFT:ATOP, NAME=VALUE1C")),
    NULL);
  IupSetAttribute(box11, "MARGIN", "0x0");
  IupSetAttribute(box11, "GAP", "0");

  box22 = IupVbox(
    IupLabel("Value:"),
    IupSetAttributes(IupText(NULL), "MULTILINE=Yes, ALIGNMENT=ALEFT:ATOP, EXPAND=YES, NAME=VALUE2, READONLY=Yes"),
    IupSetAttributes(IupFill(), "RASTERSIZE=10"),
    IupSetCallbacks(IupSetAttributes(IupButton("Get as String", NULL), "PADDING=3x3"), "ACTION", iLayoutPropertiesGetAsString_CB, NULL),
    IupLabel("IMPORTANT: if the attribute is not a string\nthis can crash the application."),
    IupSetAttributes(IupFill(), "SIZE=60"),
    NULL);
  IupSetAttribute(box22, "MARGIN", "0x0");
  IupSetAttribute(box22, "GAP", "0");

box33 = IupVbox(
  IupLabel("Value:"),
  IupFrame(IupSetAttributes(IupLabel(""), "SIZE=x20, ALIGNMENT=ALEFT:ATOP, EXPAND=HORIZONTAL, NAME=VALUE3")),
  NULL);
IupSetAttribute(box33, "MARGIN", "0x0");
IupSetAttribute(box33, "GAP", "0");

box1 = IupHbox(IupSetAttributes(IupVbox(IupLabel("Name:"), list1, NULL), "MARGIN=0x0, GAP=0"), box11, NULL);
box2 = IupHbox(IupSetAttributes(IupVbox(IupLabel("Name:"), list2, NULL), "MARGIN=0x0, GAP=0"), box22, NULL);
box3 = IupHbox(IupSetAttributes(IupVbox(IupLabel("Name:"), list3, NULL), "MARGIN=0x0, GAP=0"), box33, NULL);

box2 = IupSetAttributes(IupVbox(
  box2,
  IupSetAttributes(IupFrame(IupSetAttributes(IupHbox(IupSetAttributes(IupVbox(IupLabel("Name:"), IupSetAttributes(IupText(NULL), "VISIBLECOLUMNS=9, NAME=NAME22"), NULL), "GAP=0"),
  IupSetAttributes(IupVbox(IupLabel("Value:"), IupSetAttributes(IupText(NULL), "EXPAND=HORIZONTAL, NAME=VALUE22"), NULL), "GAP=0"),
  IupSetAttributes(IupVbox(IupLabel(""), IupSetCallbacks(IupSetAttributes(IupButton("Set", NULL), "PADDING=3x0"), "ACTION", iLayoutPropertiesSetStr_CB, NULL), NULL), "GAP=0"),
  NULL), "ALIGNMENT=ACENTER, NMARGIN=5x5")), "TITLE=\"New Attribute:\""),
  NULL), "NMARGIN=0x0");

tabs = IupTabs(box1, box2, box3, NULL);
IupSetAttribute(tabs, "TABTITLE0", "Registered Attributes");
IupSetAttribute(tabs, "TABTITLE1", "Hash Table Only");
IupSetAttribute(tabs, "TABTITLE2", "Callbacks");
IupSetCallback(tabs, "TABCHANGEPOS_CB", (Icallback)iLayoutPropertiesTabChangePos_CB);
iLayoutPropertiesTabChangePos_CB(tabs, 0, 0);

dlg_box = IupVbox(
  IupSetAttributes(IupLabel(""), "EXPAND=HORIZONTAL, NAME=ELEMTITLE"),
  tabs,
  button_box,
  NULL);

IupSetAttribute(dlg_box, "MARGIN", "10x10");
IupSetAttribute(dlg_box, "GAP", "10");

dlg = IupDialog(dlg_box);
IupSetAttribute(dlg, "TITLE", "Element Properties");
IupSetAttribute(dlg, "MINBOX", "NO");
IupSetAttribute(dlg, "MAXBOX", "NO");
IupSetAttributeHandle(dlg, "DEFAULTENTER", close);
IupSetAttributeHandle(dlg, "DEFAULTESC", close);
IupSetAttributeHandle(dlg, "PARENTDIALOG", parent);
IupSetAttribute(dlg, "ICON", IupGetGlobal("ICON"));
iupAttribSet(dlg, "_IUP_PROPLIST1", (char*)list1);
iupAttribSet(dlg, "_IUP_PROPLIST2", (char*)list2);
iupAttribSet(dlg, "_IUP_PROPLIST3", (char*)list3);
iupAttribSet(dlg, "_IUP_LAYOUTDIALOG", (char*)layoutdlg);

if (layoutdlg)
layoutdlg->properties = dlg;

return dlg;
}

Ihandle* IupElementPropertiesDialog(Ihandle* elem)
{
  Ihandle* dlg = iLayoutPropertiesCreateDialog(NULL, NULL);
  iLayoutPropertiesUpdate(dlg, elem);
  return dlg;
}


/***************************************************************************
                          Context Menu
***************************************************************************/


static int iLayoutContextMenuProperties_CB(Ihandle* menu)
{
  iLayoutDialog* layoutdlg = (iLayoutDialog*)iupAttribGetInherit(menu, "_IUP_LAYOUTDIALOG");
  Ihandle* elem = (Ihandle*)iupAttribGetInherit(menu, "_IUP_LAYOUTCONTEXTELEMENT");
  Ihandle* dlg = (Ihandle*)iupAttribGetInherit(menu, "_IUP_LAYOUTDLG");

  if (!layoutdlg->properties)
    iLayoutPropertiesCreateDialog(layoutdlg, dlg);

  iLayoutPropertiesUpdate(layoutdlg->properties, elem);

  IupShow(layoutdlg->properties);

  return IUP_DEFAULT;
}

static void iLayoutTreeUpdateTitle(iLayoutDialog* layoutdlg, Ihandle* ih)
{
  int id = IupTreeGetId(layoutdlg->tree, ih);
  IupSetAttributeId(layoutdlg->tree, "TITLE", id, iLayoutGetElementTreeTitle(ih));
}

static int iLayoutContextMenuHandleName_CB(Ihandle* menu)
{
  iLayoutDialog* layoutdlg = (iLayoutDialog*)iupAttribGetInherit(menu, "_IUP_LAYOUTDIALOG");
  Ihandle* elem = (Ihandle*)iupAttribGetInherit(menu, "_IUP_LAYOUTCONTEXTELEMENT");
  char name[256] = "";

  char* elem_name = IupGetName(elem);
  if (elem_name)
    strcpy(name, elem_name);

  if (IupGetParam("Handle Name", NULL, NULL,
                  "Name: %s\n",
                  name, NULL))
  {
    if (name[0] == 0 || name[0] == ' ')
    {
      if (elem_name)
      {
        IupSetHandle(elem_name, NULL);
        iLayoutTreeUpdateTitle(layoutdlg, elem);
      }
    }
    else
    {
      IupSetHandle(name, elem);
      iLayoutTreeUpdateTitle(layoutdlg, elem);
    }
  }

  return IUP_DEFAULT;
}

static int iLayoutContextMenuAdd_CB(Ihandle* menu)
{
  iLayoutDialog* layoutdlg = (iLayoutDialog*)iupAttribGetInherit(menu, "_IUP_LAYOUTDIALOG");
  Ihandle* ref_elem = (Ihandle*)iupAttribGetInherit(menu, "_IUP_LAYOUTCONTEXTELEMENT");
  Ihandle* dlg = (Ihandle*)iupAttribGetInherit(menu, "_IUP_LAYOUTDLG");
  int ret, count, i;
  char** class_list_str, **p_str;

  count = IupGetAllClasses(NULL, 0);
  class_list_str = (char**)malloc(count*sizeof(char*));

  IupGetAllClasses(class_list_str, count);
  qsort(class_list_str, count, sizeof(char*), iLayoutCompareStr);

  /* filter the list of classes */
  p_str = class_list_str;
  for (i = 0; i < count; i++)
  {
    Iclass* iclass = iupRegisterFindClass(class_list_str[i]);
    if (iclass->nativetype == IUP_TYPEVOID ||
        iclass->nativetype == IUP_TYPECONTROL ||
        iclass->nativetype == IUP_TYPECANVAS)
        *p_str++ = class_list_str[i];
  }
  count = (int)(p_str - class_list_str);

  IupStoreGlobal("_IUP_OLD_PARENTDIALOG", IupGetGlobal("PARENTDIALOG"));
  IupSetAttributeHandle(NULL, "PARENTDIALOG", dlg);

  ret = IupListDialog(1, "Available Classes", count, (const char**)class_list_str, 1, 10, count < 15 ? count + 1 : 15, NULL);

  IupStoreGlobal("PARENTDIALOG", IupGetGlobal("_IUP_OLD_PARENTDIALOG"));
  IupSetGlobal("_IUP_OLD_PARENTDIALOG", NULL);

  if (ret != -1)
  {
    Ihandle* ret_ih = NULL;
    int add_child = IupGetInt(menu, "_IUP_ADDCHILD");
    Ihandle* new_ih = IupCreate(class_list_str[ret]);
    int ref_id = IupTreeGetId(layoutdlg->tree, ref_elem);

    if (add_child)
    {
      if (ref_elem->firstchild && (ref_elem->firstchild->flags & IUP_INTERNAL))
      {
        /* the first child is internal, so add after it */
        if (ref_elem->firstchild->brother)
          ret_ih = IupInsert(ref_elem, ref_elem->firstchild->brother, new_ih);
        else
          ret_ih = IupAppend(ref_elem, new_ih);
      }
      else
      {
        /* add as first child */
        ret_ih = IupInsert(ref_elem, NULL, new_ih);
      }
    }
    else
    {
      if (!ref_elem->parent)
      {
        IupMessage("Error", "Can NOT add here as brother.");
        return IUP_DEFAULT;
      }

      /* add as brother after reference */
      if (ref_elem->brother)
        /* add before the brother, so it will be the brother */
        ret_ih = IupInsert(ref_elem->parent, ref_elem->brother, new_ih);
      else
        ret_ih = IupAppend(ref_elem->parent, new_ih);
    }

    if (!ret_ih)
    {
      IupMessage("Error", "Add failed. Invalid operation for this node.");
      return IUP_DEFAULT;
    }

    layoutdlg->changed = 1;

    /* add to the tree */
    iLayoutTreeAddNode(layoutdlg->tree, ref_id, new_ih);

    iLayoutUpdateLayout(layoutdlg);
  }

  free(class_list_str);

  return IUP_DEFAULT;
}

static void iLayoutUpdateColors(Ihandle* tree, Ihandle* ih)
{
  iLayoutTreeSetNodeColor(tree, IupTreeGetId(tree, ih), ih);

  if (ih->iclass->childtype != IUP_CHILDNONE)
  {
    Ihandle *child;
    for (child = ih->firstchild; child; child = child->brother)
    {
      iLayoutUpdateColors(tree, child);
    }
  }
}

static int iLayoutContextMenuMap_CB(Ihandle* menu)
{
  iLayoutDialog* layoutdlg = (iLayoutDialog*)iupAttribGetInherit(menu, "_IUP_LAYOUTDIALOG");
  Ihandle* elem = (Ihandle*)iupAttribGetInherit(menu, "_IUP_LAYOUTCONTEXTELEMENT");

  if (IupMap(elem) == IUP_ERROR)
  {
    IupMessage("Error", "IupMap failed.");
    return IUP_DEFAULT;
  }

  iLayoutUpdateColors(layoutdlg->tree, elem);

  iLayoutUpdateLayout(layoutdlg);

  return IUP_DEFAULT;
}

static int iLayoutContextMenuRefreshChildren_CB(Ihandle* menu)
{
  iLayoutDialog* layoutdlg = (iLayoutDialog*)iupAttribGetInherit(menu, "_IUP_LAYOUTDIALOG");

  IupRefreshChildren(layoutdlg->dialog);

  /* redraw canvas */
  IupUpdate(IupGetBrother(layoutdlg->tree));

  return IUP_DEFAULT;
}

static void iLayoutSelectTreeItem(iLayoutDialog* layoutdlg, Ihandle* elem);

static int iLayoutContextMenuGoToParent_CB(Ihandle* menu)
{
  iLayoutDialog* layoutdlg = (iLayoutDialog*)iupAttribGetInherit(menu, "_IUP_LAYOUTDIALOG");
  Ihandle* elem = (Ihandle*)iupAttribGetInherit(menu, "_IUP_LAYOUTCONTEXTELEMENT");

  if (elem->parent)
    iLayoutSelectTreeItem(layoutdlg, elem->parent);

  return IUP_DEFAULT;
}

static void iLayoutSaveAttributes(Ihandle* ih)
{
  IupSaveClassAttributes(ih);

  if (ih->iclass->childtype != IUP_CHILDNONE)
  {
    Ihandle *child;
    for (child = ih->firstchild; child; child = child->brother)
    {
      iLayoutSaveAttributes(child);
    }
  }
}

static int iLayoutContextMenuUnmap_CB(Ihandle* menu)
{
  iLayoutDialog* layoutdlg = (iLayoutDialog*)iupAttribGetInherit(menu, "_IUP_LAYOUTDIALOG");
  Ihandle* elem = (Ihandle*)iupAttribGetInherit(menu, "_IUP_LAYOUTCONTEXTELEMENT");

  iLayoutSaveAttributes(elem);

  IupUnmap(elem);

  iLayoutUpdateColors(layoutdlg->tree, elem);

  iLayoutUpdateLayout(layoutdlg);

  return IUP_DEFAULT;
}

static int iLayoutContextMenuSetFocus_CB(Ihandle* menu)
{
  Ihandle* elem = (Ihandle*)iupAttribGetInherit(menu, "_IUP_LAYOUTCONTEXTELEMENT");

  IupShow(IupGetDialog(elem)); /* must be the active dialog to get the focus */

  IupSetFocus(elem);

  return IUP_DEFAULT;
}

static void iLayoutBlink(Ihandle* ih);

static int iLayoutContextMenuBlink_CB(Ihandle* menu)
{
  Ihandle* elem = (Ihandle*)iupAttribGetInherit(menu, "_IUP_LAYOUTCONTEXTELEMENT");

  iLayoutBlink(elem);

  return IUP_DEFAULT;
}

static int iLayoutContextMenuRemove_CB(Ihandle* menu)
{
  Ihandle* msg_dlg;
  iLayoutDialog* layoutdlg = (iLayoutDialog*)iupAttribGetInherit(menu, "_IUP_LAYOUTDIALOG");
  Ihandle* elem = (Ihandle*)iupAttribGetInherit(menu, "_IUP_LAYOUTCONTEXTELEMENT");
  if (!elem)  /* can be called from a key press */
    elem = (Ihandle*)IupTreeGetUserId(layoutdlg->tree, IupGetInt(layoutdlg->tree, "VALUE"));
  if (!elem)
    return IUP_DEFAULT;

  if (elem->flags & IUP_INTERNAL)
  {
    IupMessage("Error", "Can NOT remove this child. It is an internal element of the container.");
    return IUP_DEFAULT;
  }

  msg_dlg = IupMessageDlg();
  IupSetAttribute(msg_dlg, "DIALOGTYPE", "QUESTION");
  IupSetAttribute(msg_dlg, "BUTTONS", "OKCANCEL");
  IupSetAttribute(msg_dlg, "TITLE", "Element Remove");
  IupSetAttribute(msg_dlg, "VALUE", "Remove the selected element?");

  IupPopup(msg_dlg, IUP_MOUSEPOS, IUP_MOUSEPOS);

  if (IupGetInt(msg_dlg, "BUTTONRESPONSE") == 1)
  {
    int id = IupTreeGetId(layoutdlg->tree, elem);

    layoutdlg->changed = 1;

    /* remove from the tree */
    IupSetAttributeId(layoutdlg->tree, "DELNODE", id, "SELECTED");

    /* update properties if necessary */
    if (layoutdlg->properties && IupGetInt(layoutdlg->properties, "VISIBLE"))
    {
      Ihandle* propelem = (Ihandle*)iupAttribGetInherit(layoutdlg->properties, "_IUP_PROPELEMENT");
      if (iupChildTreeIsChild(elem, propelem))
      {
        /* if current element will be removed, then use the previous element on the tree |*/
        iLayoutPropertiesUpdate(layoutdlg->properties, (Ihandle*)IupTreeGetUserId(layoutdlg->tree, id - 1));
      }
    }

    IupDestroy(elem);

    iLayoutUpdateLayout(layoutdlg);
  }

  IupDestroy(msg_dlg);

  return IUP_DEFAULT;
}

static int iLayoutContextMenuCopy_CB(Ihandle* menu)
{
  iLayoutDialog* layoutdlg = (iLayoutDialog*)iupAttribGetInherit(menu, "_IUP_LAYOUTDIALOG");
  Ihandle* elem = (Ihandle*)iupAttribGetInherit(menu, "_IUP_LAYOUTCONTEXTELEMENT");
  layoutdlg->copy = elem;
  return IUP_DEFAULT;
}

static int iLayoutContextMenuPaste_CB(Ihandle* menu)
{
  Ihandle* new_ih, *ret_ih = NULL;
  iLayoutDialog* layoutdlg = (iLayoutDialog*)iupAttribGetInherit(menu, "_IUP_LAYOUTDIALOG");
  Ihandle* ref_elem = (Ihandle*)iupAttribGetInherit(menu, "_IUP_LAYOUTCONTEXTELEMENT");
  int paste_child = IupGetInt(menu, "_IUP_PASTECHILD");
  int ref_id = IupTreeGetId(layoutdlg->tree, ref_elem);
  if (!iupObjectCheck(layoutdlg->copy))
    return IUP_DEFAULT;

  new_ih = IupCreate(layoutdlg->copy->iclass->name);
  IupCopyClassAttributes(layoutdlg->copy, new_ih);

  if (paste_child)
  {
    /* add as first child */
    ret_ih = IupInsert(ref_elem, NULL, new_ih);
  }
  else
  {
    if (!ref_elem->parent)
    {
      IupMessage("Error", "Can NOT paste here as brother.");
      return IUP_DEFAULT;
    }

    /* add as brother after reference */
    if (ref_elem->brother)
      /* add before the brother, so it will be the brother */
      ret_ih = IupInsert(ref_elem->parent, ref_elem->brother, new_ih);
    else
      ret_ih = IupAppend(ref_elem->parent, new_ih);
  }

  if (!ret_ih)
  {
    IupMessage("Error", "Paste failed. Invalid operation for this node.");
    return IUP_DEFAULT;
  }

  layoutdlg->changed = 1;

  /* add to the tree */
  iLayoutTreeAddNode(layoutdlg->tree, ref_id, new_ih);

  iLayoutUpdateLayout(layoutdlg);

  /* since we are only moving existing nodes,
     title, map state, and user data was not changed.
     there is no need to update the node info */

  return IUP_DEFAULT;
}

static void iLayoutContextMenu(iLayoutDialog* layoutdlg, Ihandle* ih, Ihandle* dlg)
{
  Ihandle* menu;
  int is_container = ih->iclass->childtype != IUP_CHILDNONE;
  int can_copy = !is_container || ih->firstchild == NULL;
  int can_paste = layoutdlg->copy != NULL;
  int can_map = (ih->handle == NULL) && (ih->parent == NULL || ih->parent->handle != NULL);
  int can_unmap = ih->handle != NULL;
  int can_blink = (ih->iclass->nativetype != IUP_TYPEVOID && IupGetInt(ih, "VISIBLE"));
  int can_focus = iupFocusCanAccept(ih);

  menu = IupMenu(
    IupSetCallbacks(IupItem("Properties...", NULL), "ACTION", iLayoutContextMenuProperties_CB, NULL),
    IupSetCallbacks(IupItem("Handle Name...", NULL), "ACTION", iLayoutContextMenuHandleName_CB, NULL),
    IupSetCallbacks(IupSetAttributes(IupItem("Map", NULL), can_map ? "ACTIVE=Yes" : "ACTIVE=No"), "ACTION", iLayoutContextMenuMap_CB, NULL),
    IupSetCallbacks(IupSetAttributes(IupItem("Unmap", NULL), can_unmap ? "ACTIVE=Yes" : "ACTIVE=No"), "ACTION", iLayoutContextMenuUnmap_CB, NULL),
    IupSetCallbacks(IupItem("Refresh Children", NULL), "ACTION", iLayoutContextMenuRefreshChildren_CB, NULL),
    IupSeparator(),
    IupSetCallbacks(IupItem("Go to Parent", NULL), "ACTION", iLayoutContextMenuGoToParent_CB, NULL),
    IupSetCallbacks(IupSetAttributes(IupItem("Blink", NULL), can_blink ? "ACTIVE=Yes" : "ACTIVE=No"), "ACTION", iLayoutContextMenuBlink_CB, NULL),
    IupSetCallbacks(IupSetAttributes(IupItem("Set Focus", NULL), can_focus ? "ACTIVE=Yes" : "ACTIVE=No"), "ACTION", iLayoutContextMenuSetFocus_CB, NULL),
    IupSeparator(),
    IupSetCallbacks(IupSetAttributes(IupItem("Copy", NULL), can_copy ? "ACTIVE=Yes" : "ACTIVE=No"), "ACTION", iLayoutContextMenuCopy_CB, NULL),
    IupSetCallbacks(IupSetAttributes(IupItem("Paste Child", NULL), can_paste && is_container ? "ACTIVE=Yes, _IUP_PASTECHILD=1" : "ACTIVE=No, _IUP_PASTECHILD=1"), "ACTION", iLayoutContextMenuPaste_CB, NULL),
    IupSetCallbacks(IupSetAttributes(IupItem("Paste Brother", NULL), can_paste ? "ACTIVE=Yes" : "ACTIVE=No"), "ACTION", iLayoutContextMenuPaste_CB, NULL),
    IupSeparator(),
    IupSetCallbacks(IupSetAttributes(IupItem("Add Child...", NULL), is_container ? "ACTIVE=Yes, _IUP_ADDCHILD=1" : "ACTIVE=No, _IUP_ADDCHILD=1"), "ACTION", iLayoutContextMenuAdd_CB, NULL),
    IupSetCallbacks(IupSetAttributes(IupItem("Add Brother...", NULL), "_IUP_ADDCHILD=0"), "ACTION", iLayoutContextMenuAdd_CB, NULL),
    IupSetCallbacks(IupItem("Remove...\tDel", NULL), "ACTION", iLayoutContextMenuRemove_CB, NULL),
    NULL);

  iupAttribSet(menu, "_IUP_LAYOUTCONTEXTELEMENT", (char*)ih);
  iupAttribSet(menu, "_IUP_LAYOUTDIALOG", (char*)layoutdlg);
  iupAttribSet(menu, "_IUP_LAYOUTDLG", (char*)dlg);

  IupPopup(menu, IUP_MOUSEPOS, IUP_MOUSEPOS);
}


/***************************************************************************
                       Layout Canvas Interaction
***************************************************************************/


static void iLayoutBlink(Ihandle* ih)
{
  if (ih->iclass->nativetype != IUP_TYPEVOID && IupGetInt(ih, "VISIBLE"))
  {
    int i;
    for (i = 0; i < 3; i++)
    {
      IupSetAttribute(ih, "VISIBLE", "NO");
      IupFlush();
      iupdrvSleep(100);
      IupSetAttribute(ih, "VISIBLE", "Yes");
      IupFlush();
      iupdrvSleep(100);
    }
  }
}

static void iLayoutUpdateMark(iLayoutDialog* layoutdlg, Ihandle* ih, int id)
{
  IupSetfAttribute(layoutdlg->status, "TITLE", "Position:%4d,%4d | User:%4d,%4d | Natural:%4d,%4d | Current:%4d,%4d", ih->x, ih->y, ih->userwidth, ih->userheight, ih->naturalwidth, ih->naturalheight, ih->currentwidth, ih->currentheight);

  if (!ih->handle)
    IupSetAttributeId(layoutdlg->tree, "COLOR", id, "128 0 0");
  else
    IupSetAttributeId(layoutdlg->tree, "COLOR", id, "255 0 0");

  iupAttribSet(IupGetDialog(layoutdlg->tree), "_IUPLAYOUT_MARK", (char*)ih);
  IupUpdate(IupGetBrother(layoutdlg->tree));

  if (layoutdlg->properties && IupGetInt(layoutdlg->properties, "VISIBLE"))
    iLayoutPropertiesUpdate(layoutdlg->properties, ih);
}

static Ihandle* iLayoutGetElementByPos(Ihandle* ih, int native_parent_x, int native_parent_y, int x, int y, int dlgvisible, int shownotmapped)
{
  Ihandle *child, *elem;
  int dx, dy;

  /* can only click in elements that are visible,
     independent from showhidden */
  if (iLayoutElementIsVisible(ih, dlgvisible) &&
      (shownotmapped || ih->handle))
  {
    /* check the element */
    if (x >= ih->x + native_parent_x &&
        y >= ih->y + native_parent_y &&
        x < ih->x + native_parent_x + ih->currentwidth &&
        y < ih->y + native_parent_y + ih->currentheight)
    {
      if (ih->iclass->childtype != IUP_CHILDNONE)
      {
        /* if ih is a native parent, then update the offset */
        if (ih->iclass->nativetype != IUP_TYPEVOID)
        {
          dx = 0, dy = 0;
          IupGetIntInt(ih, "CLIENTOFFSET", &dx, &dy);
          native_parent_x += ih->x + dx;
          native_parent_y += ih->y + dy;

          /* if ih is a Tabs, then find only the active child */
          if (IupClassMatch(ih, "tabs") || IupClassMatch(ih, "flattabs"))
          {
            child = (Ihandle*)IupGetAttribute(ih, "VALUE_HANDLE");
            if (child)
            {
              elem = iLayoutGetElementByPos(child, native_parent_x, native_parent_y, x, y, dlgvisible, shownotmapped);
              if (elem)
                return elem;
            }
            return ih;
          }
        }
      }

      /* check its children */
      for (child = ih->firstchild; child; child = child->brother)
      {
        elem = iLayoutGetElementByPos(child, native_parent_x, native_parent_y, x, y, dlgvisible, shownotmapped);
        if (elem)
          return elem;
      }

      return ih;
    }
  }
  return NULL;
}

static Ihandle* iLayoutGetDialogElementByPos(iLayoutDialog* layoutdlg, int x, int y)
{
  int w, h;

  /* check the dialog */
  IupGetIntInt(layoutdlg->dialog, "CLIENTSIZE", &w, &h);
  if (layoutdlg->dialog->firstchild &&
      x >= 0 && y >= 0 &&
      x < w && y < h)
  {
    Ihandle* elem;
    Ihandle* canvas = IupGetBrother(layoutdlg->tree);
    int native_parent_x = 0, native_parent_y = 0;
    int shownotmapped = layoutdlg->dialog->handle == NULL;  /* only check not mapped if dialog is also not mapped */
    int dlgvisible = IupGetInt(layoutdlg->dialog, "VISIBLE");
    IupGetIntInt(layoutdlg->dialog, "CLIENTOFFSET", &native_parent_x, &native_parent_y);
    native_parent_x -= IupGetInt(canvas, "POSX");
    native_parent_y -= IupGetInt(canvas, "POSY");
    elem = iLayoutGetElementByPos(layoutdlg->dialog->firstchild, native_parent_x, native_parent_y, x, y, dlgvisible, shownotmapped);
    if (elem)
      return elem;
    return layoutdlg->dialog;
  }
  return NULL;
}

static void iLayoutSelectTreeItem(iLayoutDialog* layoutdlg, Ihandle* elem)
{
  int id = IupTreeGetId(layoutdlg->tree, elem);
  int old_id = IupGetInt(layoutdlg->tree, "VALUE");
  Ihandle* old_elem = (Ihandle*)IupTreeGetUserId(layoutdlg->tree, old_id);
  iLayoutTreeSetNodeColor(layoutdlg->tree, old_id, old_elem);
  IupSetInt(layoutdlg->tree, "VALUE", id);
  iLayoutUpdateMark(layoutdlg, elem, id);
}

static int iLayoutCanvasButton_CB(Ihandle* canvas, int but, int pressed, int x, int y, char* status)
{
  (void)status;
  if (but == IUP_BUTTON1 && pressed)
  {
    Ihandle* dlg = IupGetDialog(canvas);
    iLayoutDialog* layoutdlg = (iLayoutDialog*)iupAttribGet(dlg, "_IUP_LAYOUTDIALOG");
    Ihandle* elem = iLayoutGetDialogElementByPos(layoutdlg, x, y);
    if (elem)
    {
      if (iup_isdouble(status))
      {
        iLayoutBlink(elem);
        IupUpdate(canvas);
      }
      else
        iLayoutSelectTreeItem(layoutdlg, elem);
    }
    else if (iupAttribGet(dlg, "_IUPLAYOUT_MARK"))
    {
      iupAttribSet(dlg, "_IUPLAYOUT_MARK", NULL);
      IupUpdate(canvas);
    }
  }
  else if (but == IUP_BUTTON3 && pressed)
  {
    Ihandle* dlg = IupGetDialog(canvas);
    iLayoutDialog* layoutdlg = (iLayoutDialog*)iupAttribGet(dlg, "_IUP_LAYOUTDIALOG");
    Ihandle* elem = iLayoutGetDialogElementByPos(layoutdlg, x, y);
    if (elem && elem != layoutdlg->dialog)
      iLayoutContextMenu(layoutdlg, elem, dlg);
  }
  return IUP_DEFAULT;
}

static int iLayoutCanvasResize_CB(Ihandle* canvas, int canvas_w, int canvas_h)
{
  IupSetInt(canvas, "DX", canvas_w);
  IupSetInt(canvas, "DY", canvas_h);
  return IUP_DEFAULT;
}

/***************************************************************************
                              Layout Tree
***************************************************************************/


static int iLayoutTreeExecuteLeaf_CB(Ihandle* tree, int id)
{
  Ihandle* elem = (Ihandle*)IupTreeGetUserId(tree, id);
  iLayoutBlink(elem);
  return IUP_DEFAULT;
}

static int iLayoutTreeRightClick_CB(Ihandle* tree, int id)
{
  Ihandle* dlg = IupGetDialog(tree);
  iLayoutDialog* layoutdlg = (iLayoutDialog*)iupAttribGet(dlg, "_IUP_LAYOUTDIALOG");
  Ihandle* elem = (Ihandle*)IupTreeGetUserId(tree, id);
  iLayoutContextMenu(layoutdlg, elem, dlg);
  return IUP_DEFAULT;
}

static int iLayoutTreeDragDrop_CB(Ihandle* tree, int drag_id, int drop_id, int isshift, int iscontrol)
{
  Ihandle* dlg = IupGetDialog(tree);
  iLayoutDialog* layoutdlg = (iLayoutDialog*)iupAttribGet(dlg, "_IUP_LAYOUTDIALOG");
  Ihandle* drag_elem = (Ihandle*)IupTreeGetUserId(tree, drag_id);
  Ihandle* drop_elem = (Ihandle*)IupTreeGetUserId(tree, drop_id);
  int error;

  /* no support for copy */
  if (iscontrol)
  {
    IupMessage("Error", "Copy not supported for drag&drop.");
    return IUP_IGNORE;
  }

  if (drag_elem->flags & IUP_INTERNAL)
  {
    IupMessage("Error", "Can NOT drag an internal element. This element exists only inside this container.");
    return IUP_IGNORE;
  }

  if (iupStrEqualNoCase(IupGetAttributeId(tree, "KIND", drop_id), "BRANCH") &&
      iupStrEqualNoCase(IupGetAttributeId(tree, "STATE", drop_id), "EXPANDED"))
  {
    Ihandle* ref_child = drop_elem->firstchild;   /* the first child as reference */

    /* if first element is internal, use the next one. */
    if (drop_elem->firstchild && (drop_elem->firstchild->flags & IUP_INTERNAL))
    {
      /* the first child is internal, so use brother as reference */
      if (drop_elem->firstchild->brother)
        ref_child = drop_elem->firstchild->brother;
    }

    if (drop_elem == drag_elem->parent && ref_child == drag_elem)
    {
      /* dropped at the same place, just ignore it */
      return IUP_IGNORE;
    }

    /* If the drop node is a branch and it is expanded, */
    /* add as first child */
    error = IupReparent(drag_elem, drop_elem, ref_child);  /* add before the reference */
  }
  else
  {
    if (!drop_elem->parent)
    {
      IupMessage("Error", "Can NOT drop here as brother.");
      return IUP_IGNORE;
    }

    if (drop_elem->parent == drag_elem->parent && drop_elem->brother == drag_elem)
    {
      /* dropped at the same place, just ignore it */
      return IUP_IGNORE;
    }

    /* If the branch is not expanded or the drop node is a leaf, */
    /* add as brother after reference */
    error = IupReparent(drag_elem, drop_elem->parent, drop_elem->brother);  /* drop_elem->brother can be NULL here */
  }

  if (error == IUP_ERROR)
  {
    IupMessage("Error", "Drop failed. Invalid operation for this node.");
    return IUP_IGNORE;
  }

  layoutdlg->changed = 1;

  iLayoutUpdateLayout(layoutdlg);

  /* since we are only moving existing nodes,
     title, map state, and user data was not changed.
     there is no need to update the node info */

  (void)isshift;
  return IUP_CONTINUE;  /* the nodes of the tree will be automatically moved */
}

static int iLayoutTreeSelection_CB(Ihandle* tree, int id, int status)
{
  Ihandle* elem = (Ihandle*)IupTreeGetUserId(tree, id);
  if (status == 1)
  {
    Ihandle* dlg = IupGetDialog(tree);
    iLayoutDialog* layoutdlg = (iLayoutDialog*)iupAttribGet(dlg, "_IUP_LAYOUTDIALOG");
    iLayoutUpdateMark(layoutdlg, elem, id);
  }
  else
    iLayoutTreeSetNodeColor(tree, id, elem);
  return IUP_DEFAULT;
}


/***************************************************************************
                            Layout Dialog Callbacks
***************************************************************************/


static int iLayoutDialogKAny_CB(Ihandle* dlg, int key)
{
  switch (key)
  {
  case K_DEL:
    return iLayoutContextMenuRemove_CB(dlg);
  case K_F5:
    return iLayoutMenuUpdate_CB(dlg);
  case K_ESC:
    return iLayoutMenuClose_CB(dlg);
  case K_cO:
    return iLayoutMenuLoad_CB(dlg);
  case K_cF5:
    return iLayoutMenuRefresh_CB(dlg);
  case K_F3:
  {
    Ihandle* find_dlg = (Ihandle*)IupGetAttribute(dlg, "FIND_DIALOG");
    if (!find_dlg)
      return iLayoutMenuFind_CB(dlg);
    else
      return iLayoutFindDialogNext_CB(find_dlg);
  }
  case K_cF:
    return iLayoutMenuFind_CB(dlg);
  case K_cMinus:
  case K_cPlus:
    {
      int opacity = IupGetInt(dlg, "OPACITY");
      if (opacity == 0)
        opacity = 255;
      if (key == K_cPlus)
        opacity++;
      else
        opacity--;
      if (opacity == 0 || opacity == 255)
        IupSetAttribute(dlg, "OPACITY", NULL);
      else
        IupSetInt(dlg, "OPACITY", opacity);
      break;
    }
  }

  return IUP_DEFAULT;
}

static int iLayoutDialogShow_CB(Ihandle* dlg, int state)
{
  if (state == IUP_SHOW)
    IupSetAttribute(dlg, "RASTERSIZE", NULL);
  return IUP_DEFAULT;
}

static int iLayoutDialogClose_CB(Ihandle* dlg)
{
  if (IupGetInt(dlg, "DESTROYWHENCLOSED"))
  {
    IupDestroy(dlg);
    return IUP_IGNORE;
  }
  return IUP_DEFAULT;
}

static int iLayoutDialogDestroy_CB(Ihandle* dlg)
{
  iLayoutDialog* layoutdlg = (iLayoutDialog*)iupAttribGet(dlg, "_IUP_LAYOUTDIALOG");
  IupDestroy(layoutdlg->timer);
  if (iupObjectCheck(layoutdlg->properties))
    IupDestroy(layoutdlg->properties);
  if (layoutdlg->destroy && iupObjectCheck(layoutdlg->dialog))
    IupDestroy(layoutdlg->dialog);
  free(layoutdlg);
  return IUP_DEFAULT;
}

Ihandle* IupLayoutDialog(Ihandle* dialog)
{
  Ihandle *tree, *canvas, *dlg, *menu, *status, *split;
  iLayoutDialog* layoutdlg;

  layoutdlg = calloc(1, sizeof(iLayoutDialog));
  if (dialog)
    layoutdlg->dialog = dialog;
  else
  {
    layoutdlg->dialog = IupDialog(NULL);
    layoutdlg->destroy = 1;
  }

  layoutdlg->timer = IupTimer();
  IupSetCallback(layoutdlg->timer, "ACTION_CB", iLayoutTimerAutoUpdate_CB);
  IupSetAttribute(layoutdlg->timer, "TIME", "300");
  IupSetAttribute(layoutdlg->timer, "_IUP_LAYOUTDIALOG", (char*)layoutdlg);

  canvas = IupCanvas(NULL);
  IupSetCallback(canvas, "ACTION", (Icallback)iLayoutCanvas_CB);
  IupSetCallback(canvas, "BUTTON_CB", (Icallback)iLayoutCanvasButton_CB);
  IupSetCallback(canvas, "RESIZE_CB", (Icallback)iLayoutCanvasResize_CB);
  IupSetAttribute(canvas, "SCROLLBAR", "YES");
  IupSetAttribute(canvas, "XAUTOHIDE", "NO");
  IupSetAttribute(canvas, "YAUTOHIDE", "NO");

  tree = IupTree();
  layoutdlg->tree = tree;
  IupSetAttribute(tree, "NAME", "TREE");
  IupSetAttribute(tree, "RASTERSIZE", NULL);
  IupSetAttribute(tree, "SHOWDRAGDROP", "Yes");
  IupSetCallback(tree, "SELECTION_CB", (Icallback)iLayoutTreeSelection_CB);
  IupSetCallback(tree, "EXECUTELEAF_CB", (Icallback)iLayoutTreeExecuteLeaf_CB);
  IupSetCallback(tree, "RIGHTCLICK_CB", (Icallback)iLayoutTreeRightClick_CB);
  IupSetCallback(tree, "DRAGDROP_CB", (Icallback)iLayoutTreeDragDrop_CB);

  status = IupLabel(NULL);
  IupSetAttribute(status, "EXPAND", "HORIZONTAL");
  IupSetAttribute(status, "FONT", "Courier, 11");
  IupSetAttribute(status, "SIZE", "x12");
  layoutdlg->status = status;

  split = IupSplit(tree, canvas);
  IupSetAttribute(split, "VALUE", "300");
  IupSetAttribute(split, "AUTOHIDE", "Yes");

  menu = IupMenu(
    IupSubmenu("&Dialog", IupMenu(
    IupSetCallbacks(IupItem("New", NULL), "ACTION", iLayoutMenuNew_CB, NULL),
    IupSetCallbacks(IupItem("Load...\tCtrl+O", NULL), "ACTION", iLayoutMenuLoad_CB, NULL),
    IupSetCallbacks(IupItem("Load Visible...", NULL), "ACTION", iLayoutMenuLoadVisible_CB, NULL),
    IupSetCallbacks(IupItem("Reload", NULL), "ACTION", iLayoutMenuReload_CB, NULL),
    IupSubmenu("&Export", IupMenu(
    IupSetCallbacks(IupItem("C...", NULL), "ACTION", iLayoutMenuExportC_CB, NULL),
    IupSetCallbacks(IupItem("LED...", NULL), "ACTION", iLayoutMenuExportLED_CB, NULL),
    IupSetCallbacks(IupItem("Lua...", NULL), "ACTION", iLayoutMenuExportLua_CB, NULL),
    NULL)),
    IupSeparator(),
    IupSetCallbacks(IupItem("Refresh\tCtrl+F5", NULL), "ACTION", iLayoutMenuRefresh_CB, NULL),
    IupSetCallbacks(IupItem("Redraw", NULL), "ACTION", iLayoutMenuRedraw_CB, NULL),
    IupSetCallbacks(IupItem("Show", NULL), "ACTION", iLayoutMenuShow_CB, NULL),
    IupSetCallbacks(IupItem("Hide", NULL), "ACTION", iLayoutMenuHide_CB, NULL),
    IupSeparator(),
    IupSetCallbacks(IupItem("&Close\tEsc", NULL), "ACTION", iLayoutMenuClose_CB, NULL),
    NULL)),
    IupSubmenu("&Layout", IupMenu(
    IupSetCallbacks(IupSetAttributes(IupItem("&Hierarchy", NULL), "AUTOTOGGLE=YES, VALUE=ON"), "ACTION", iLayoutMenuHierarchy_CB, NULL),
    IupSeparator(),
    IupSetCallbacks(IupItem("Update (Tree and Draw)\tF5", NULL), "ACTION", iLayoutMenuUpdate_CB, NULL),
    IupSetCallbacks(IupSetAttributes(IupItem("Auto Update (Draw Only)", NULL), "AUTOTOGGLE=YES, VALUE=OFF"), "ACTION", iLayoutMenuAutoUpdate_CB, NULL),
    IupSetCallbacks(IupSetAttributes(IupItem("Show Hidden", NULL), "AUTOTOGGLE=YES, VALUE=OFF"), "ACTION", iLayoutMenuShowHidden_CB, NULL),
    IupSetCallbacks(IupSetAttributes(IupItem("Show Internal", NULL), "AUTOTOGGLE=YES, VALUE=OFF"), "ACTION", iLayoutMenuShowInternal_CB, NULL),
    IupSeparator(),
    IupSetCallbacks(IupItem("Opacity\tCtrl+/Ctrl-", NULL), "ACTION", iLayoutMenuOpacity_CB, NULL),
    IupSeparator(),
    IupSetCallbacks(IupItem("Find...\tCtrl+F", NULL), "ACTION", iLayoutMenuFind_CB, NULL),
    NULL)),
    NULL);

  dlg = IupDialog(IupVbox(split, status, NULL));
  IupSetAttribute(dlg, "TITLE", "Dialog Layout");
  IupSetAttribute(dlg, "PARENTDIALOG", IupGetGlobal("PARENTDIALOG"));
  IupSetAttribute(dlg, "TREE", (char*)tree);
  IupSetAttribute(dlg, "ICON", IupGetGlobal("ICON"));
  IupSetCallback(dlg, "DESTROY_CB", iLayoutDialogDestroy_CB);
  IupSetCallback(dlg, "SHOW_CB", (Icallback)iLayoutDialogShow_CB);
  IupSetCallback(dlg, "K_ANY", (Icallback)iLayoutDialogKAny_CB);
  IupSetCallback(dlg, "CLOSE_CB", iLayoutDialogClose_CB);
  iupAttribSet(dlg, "_IUP_LAYOUTDIALOG", (char*)layoutdlg);
  IupSetAttributeHandle(dlg, "MENU", menu);
  iupAttribSet(dlg, "OPACITY", "255");

  iupAttribSet(dlg, "DESTROYWHENCLOSED", "Yes");

  {
    int w = 0, h = 0;
    IupGetIntInt(layoutdlg->dialog, "RASTERSIZE", &w, &h);
    if (w && h)
      IupSetfAttribute(dlg, "RASTERSIZE", "%dx%d", (int)(w*1.3), h);
    else
      IupSetAttribute(dlg, "SIZE", "HALFxHALF");
  }

  IupMap(dlg);

  iLayoutTreeRebuild(layoutdlg);

  return dlg;
}
