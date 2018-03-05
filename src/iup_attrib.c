/** \file
 * \brief attributes environment management
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <limits.h>
#include <stdarg.h>

#include "iup.h"

#include "iup_object.h"
#include "iup_childtree.h"
#include "iup_str.h"
#include "iup_ledlex.h"
#include "iup_attrib.h"
#include "iup_assert.h"
#include "iup_varg.h"


#define iupATTRIB_LANGUAGE_STRING(_v)  (_v && _v[0] == '_' && _v[1] == '@') 
#define iupATTRIB_LANGUAGE_SHIFT 2

#define iupATTRIB_GET_LANGUAGE_STRING(_v, _s)                              \
  {                                                                        \
    char* new_value = IupGetLanguageString(_v+iupATTRIB_LANGUAGE_SHIFT);   \
    if (new_value != _v+iupATTRIB_LANGUAGE_SHIFT)                          \
    {                                                                      \
      _v = new_value;                                                      \
      _s = 0;  /* no need to store it again, already stored internally */  \
    }                                                                      \
  }


int IupGetAllAttributes(Ihandle* ih, char** names, int n)
{
  char *name;
  int i = 0;

  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return 0;

  if (!names || !n)
    return iupTableCount(ih->attrib);

  name = iupTableFirst(ih->attrib);
  while (name)
  {
    if (!iupATTRIB_ISINTERNAL(name))
    {
      names[i] = name;
      i++;

      if (i == n)
        break;
    }

    name = iupTableNext(ih->attrib);
  }

  return i;
}

char* IupGetAttributes(Ihandle *ih)
{
  char *buffer;
  char *name, *value;
  char sb[128];
  int size;

  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return NULL;

  buffer = iupStrGetLargeMem(&size);
  buffer[0] = 0;

  name = iupTableFirst(ih->attrib);
  while (name && size)
  {
    if (!iupATTRIB_ISINTERNAL(name))
    {
      if (buffer[0] != 0)
        { strcat(buffer,","); size--; }

      value = iupTableGetCurr(ih->attrib);
      if (iupAttribIsNotString(ih, name))
      {
        sprintf(sb, "%p", (void*) value);
        value = sb;
      }

      strcat(buffer, name);  size -= (int)strlen(name);
      strcat(buffer,"=\"");  size--;
      strcat(buffer, value);  size -= (int)strlen(value);
      strcat(buffer,"\"");  size--;
    }

    name = iupTableNext(ih->attrib);
  }

  return buffer;
}

void iupAttribUpdateFromParent(Ihandle* ih)
{
  Iclass* ic = ih->iclass;
  char *name = iupTableFirst(ic->attrib_func);
  while (name)
  {
    /* if inheritable and NOT defined at the element */
    if (iupClassObjectCurAttribIsInherit(ic) && !iupAttribGet(ih, name))
    {
      /* check in the parent tree if the attribute is defined */
      Ihandle* parent = ih->parent;
      while (parent)
      {
        char* value = iupTableGet(parent->attrib, name);
        if (value)
        {
          int inherit;
          /* set on the class */
          iupClassObjectSetAttribute(ih, name, value, &inherit);
          break;
        }
        parent = parent->parent;
      }
    }

    name = iupTableNext(ic->attrib_func);
  }
}

static int iAttribIsInherit(Ihandle* ih, const  char* name)
{
  int inherit;
  char *def_value;
  iupClassObjectGetAttributeInfo(ih, name, &def_value, &inherit);
  return inherit;
}

static void iAttribNotifyChildren(Ihandle *ih, const char* name, const char *value)
{
  int inherit;
  Ihandle* child = ih->firstchild;
  while (child)
  {
    if (!iupTableGet(child->attrib, name))
    {
      /* set only if an inheritable attribute at the child */
      if (iAttribIsInherit(child, name))
      {
        /* set on the class */
        iupClassObjectSetAttribute(child, name, value, &inherit);

        iAttribNotifyChildren(child, name, value);
      }
    }

    child = child->brother;
  }
}

void iupAttribUpdateChildren(Ihandle* ih)
{
  char *name = iupTableFirst(ih->attrib);
  while (name)
  {
    if (!iupATTRIB_ISINTERNAL(name) && iAttribIsInherit(ih, name))
    {
      /* retrieve from the table */
      char* value = iupTableGet(ih->attrib, name);
      iAttribNotifyChildren(ih, name, value);
    }

    name = iupTableNext(ih->attrib);
  }
}

void iupAttribUpdate(Ihandle* ih)
{
  char** name_array;
  char *name, *value;
  int count, i = 0, inherit, store;

  count = iupTableCount(ih->attrib);
  if (!count)
    return;

  name_array = (char**)malloc(count * sizeof(char*));

  /* store the names before updating so we can add or remove attributes during the update */
  name = iupTableFirst(ih->attrib);
  while (name)
  {
    if (!iupATTRIB_ISINTERNAL(name))
    {
      name_array[i] = name;
      i++;
    }

    name = iupTableNext(ih->attrib);
  }
  count = i;

  /* for all defined attributes updates the native system */
  for (i = 0; i < count; i++)
  {
    name = name_array[i];

    /* retrieve from the table */
    value = iupTableGet(ih->attrib, name);

    /* set on the class */
    store = iupClassObjectSetAttribute(ih, name, value, &inherit);

    if (inherit)
      iAttribNotifyChildren(ih, name, value);

    if (store == 0)
      iupTableRemove(ih->attrib, name); /* remove from the table according to the class SetAttribute */
  }

  free(name_array);
}

void IupSetAttributeId(Ihandle *ih, const char* name, int id, const char *value)
{
  iupASSERT(name!=NULL);
  if (!name)
    return;

  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return;

  if (iupClassObjectSetAttributeId(ih, name, id, value)!=0) /* store strings and pointers */
    iupAttribSetId(ih, name, id, value);
}

void IupSetStrAttributeId(Ihandle *ih, const char* name, int id, const char *value)
{
  IupStoreAttributeId(ih, name, id, value);
}

void IupStoreAttributeId(Ihandle *ih, const char* name, int id, const char *value)
{
  int store = 1;

  iupASSERT(name!=NULL);
  if (!name)
    return;

  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return;

  if (iupATTRIB_LANGUAGE_STRING(value))
    iupATTRIB_GET_LANGUAGE_STRING(value, store);

  if (iupClassObjectSetAttributeId(ih, name, id, value)==1) /* store only strings */
  {
    if (store)
      iupAttribSetStrId(ih, name, id, value);
    else
      iupAttribSetId(ih, name, id, value);
  }
}

char* IupGetAttributeId(Ihandle *ih, const char* name, int id)
{
  char *value;

  iupASSERT(name!=NULL);
  if (!name)
    return NULL;

  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return NULL;

  value = iupClassObjectGetAttributeId(ih, name, id);
  if (!value)
    value = iupAttribGetId(ih, name, id);

  return value;
}

void IupSetAttributeId2(Ihandle* ih, const char* name, int lin, int col, const char* value)
{
  iupASSERT(name!=NULL);
  if (!name)
    return;

  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return;

  if (iupClassObjectSetAttributeId2(ih, name, lin, col, value)!=0) /* store strings and pointers */
    iupAttribSetId2(ih, name, lin, col, value);
}

void IupSetStrAttributeId2(Ihandle* ih, const char* name, int lin, int col, const char* value)
{
  IupStoreAttributeId2(ih, name, lin, col, value);
}

void IupStoreAttributeId2(Ihandle* ih, const char* name, int lin, int col, const char* value)
{
  int store = 1;

  iupASSERT(name!=NULL);
  if (!name)
    return;

  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return;

  if (iupATTRIB_LANGUAGE_STRING(value))
    iupATTRIB_GET_LANGUAGE_STRING(value, store);

  if (iupClassObjectSetAttributeId2(ih, name, lin, col, value)==1) /* store only strings */
  {
    if (store)
      iupAttribSetStrId2(ih, name, lin, col, value);
    else
      iupAttribSetId2(ih, name, lin, col, value);
  }
}

char* IupGetAttributeId2(Ihandle* ih, const char* name, int lin, int col)
{
  char *value;

  iupASSERT(name!=NULL);
  if (!name)
    return NULL;

  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return NULL;

  value = iupClassObjectGetAttributeId2(ih, name, lin, col);
  if (!value)
    value = iupAttribGetId2(ih, name, lin, col);

  return value;
}

int IupGetIntId(Ihandle *ih, const char* name, int id)
{
  int i = 0;
  char *value = IupGetAttributeId(ih, name, id);
  if (value)
  {
    if (!iupStrToInt(value, &i))
    {
      if (iupStrBoolean(value))
        i = 1;
    }
  }
  return i;
}

float IupGetFloatId(Ihandle *ih, const char* name, int id)
{
  float f = 0;
  iupStrToFloat(IupGetAttributeId(ih, name, id), &f);
  return f;
}

double IupGetDoubleId(Ihandle *ih, const char* name, int id)
{
  double f = 0;
  iupStrToDouble(IupGetAttributeId(ih, name, id), &f);
  return f;
}

void IupGetRGBId(Ihandle *ih, const char* name, int id, unsigned char *r, unsigned char *g, unsigned char *b)
{
  iupStrToRGB(IupGetAttributeId(ih, name, id), r, g, b);
}

void IupSetStrfIdV(Ihandle* ih, const char* name, int id, const char* f, va_list arglist)
{
  int size;
  char* value = iupStrGetLargeMem(&size);
  vsnprintf(value, size, f, arglist);
  IupStoreAttributeId(ih, name, id, value);
}

void IupSetStrfId(Ihandle *ih, const char* name, int id, const char* f, ...)
{
  int size;
  char* value = iupStrGetLargeMem(&size);
  va_list arglist;
  va_start(arglist, f);
  vsnprintf(value, size, f, arglist);
  va_end(arglist);
  IupStoreAttributeId(ih, name, id, value);
}

void IupSetfAttributeId(Ihandle *ih, const char* name, int id, const char* f, ...)
{
  int size;
  char* value = iupStrGetLargeMem(&size);
  va_list arglist;
  va_start(arglist, f);
  vsnprintf(value, size, f, arglist);
  va_end(arglist);
  IupStoreAttributeId(ih, name, id, value);
}

void IupSetIntId(Ihandle* ih, const char* name, int id, int num)
{
  char value[20];  /* +4,294,967,296 */
  sprintf(value, "%d", num);
  IupStoreAttributeId(ih, name, id, value);
}

void IupSetFloatId(Ihandle* ih, const char* name, int id, float num)
{
  char value[80];
  sprintf(value, IUP_FLOAT2STR, num);
  IupStoreAttributeId(ih, name, id, value);
}

void IupSetDoubleId(Ihandle* ih, const char* name, int id, double num)
{
  char value[80];
  sprintf(value, IUP_DOUBLE2STR, num);
  IupStoreAttributeId(ih, name, id, value);
}

void IupSetRGBId(Ihandle *ih, const char* name, int id, unsigned char r, unsigned char g, unsigned char b)
{
  char value[60];
  sprintf(value, "%d %d %d", (int)r, (int)g, (int)b);
  IupStoreAttributeId(ih, name, id, value);
}

int IupGetIntId2(Ihandle* ih, const char* name, int lin, int col)
{
  int i = 0;
  char *value = IupGetAttributeId2(ih, name, lin, col);
  if (value)
  {
    if (!iupStrToInt(value, &i))
    {
      if (iupStrBoolean(value))
        i = 1;
    }
  }
  return i;
}

float IupGetFloatId2(Ihandle* ih, const char* name, int lin, int col)
{
  float f = 0;
  iupStrToFloat(IupGetAttributeId2(ih, name, lin, col), &f);
  return f;
}

double IupGetDoubleId2(Ihandle* ih, const char* name, int lin, int col)
{
  double f = 0;
  iupStrToDouble(IupGetAttributeId2(ih, name, lin, col), &f);
  return f;
}

void IupGetRGBId2(Ihandle *ih, const char* name, int lin, int col, unsigned char *r, unsigned char *g, unsigned char *b)
{
  iupStrToRGB(IupGetAttributeId2(ih, name, lin, col), r, g, b);
}

void IupSetStrfId2V(Ihandle* ih, const char* name, int lin, int col, const char* f, va_list arglist)
{
  int size;
  char* value = iupStrGetLargeMem(&size);
  vsnprintf(value, size, f, arglist);
  IupStoreAttributeId2(ih, name, lin, col, value);
}

void IupSetStrfId2(Ihandle* ih, const char* name, int lin, int col, const char* f, ...)
{
  int size;
  char* value = iupStrGetLargeMem(&size);
  va_list arglist;
  va_start(arglist, f);
  vsnprintf(value, size, f, arglist);
  va_end(arglist);
  IupStoreAttributeId2(ih, name, lin, col, value);
}

void IupSetfAttributeId2(Ihandle* ih, const char* name, int lin, int col, const char* f, ...)
{
  int size;
  char* value = iupStrGetLargeMem(&size);
  va_list arglist;
  va_start(arglist, f);
  vsnprintf(value, size, f, arglist);
  va_end(arglist);
  IupStoreAttributeId2(ih, name, lin, col, value);
}

void IupSetIntId2(Ihandle* ih, const char* name, int lin, int col, int num)
{
  char value[20];  /* +4,294,967,296 */
  sprintf(value, "%d", num);
  IupStoreAttributeId2(ih, name, lin, col, value);
}

void IupSetFloatId2(Ihandle* ih, const char* name, int lin, int col, float num)
{
  char value[80];
  sprintf(value, IUP_FLOAT2STR, num);
  IupStoreAttributeId2(ih, name, lin, col, value);
}

void IupSetDoubleId2(Ihandle* ih, const char* name, int lin, int col, double num)
{
  char value[80];
  sprintf(value, IUP_DOUBLE2STR, num);
  IupStoreAttributeId2(ih, name, lin, col, value);
}

void IupSetRGBId2(Ihandle *ih, const char* name, int lin, int col, unsigned char r, unsigned char g, unsigned char b)
{
  char value[60];
  sprintf(value, "%d %d %d", (int)r, (int)g, (int)b);
  IupStoreAttributeId2(ih, name, lin, col, value);
}

void IupSetAttribute(Ihandle *ih, const char* name, const char *value)
{
  int inherit;

  iupASSERT(name!=NULL);
  if (!name)
    return;

  if (!ih)
  {
    IupSetGlobal(name, value);
    return;
  }

  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return;

  if (iupATTRIB_ISINTERNAL(name))
    iupAttribSet(ih, name, value);
  else
  {
    if (iupClassObjectSetAttribute(ih, name, value, &inherit)!=0) /* store strings and pointers */
      iupAttribSet(ih, name, value);

    if (inherit)
      iAttribNotifyChildren(ih, name, value);
  }
}

void IupSetStrAttribute(Ihandle *ih, const char* name, const char *value)
{
  IupStoreAttribute(ih, name, value);
}

void IupStoreAttribute(Ihandle *ih, const char* name, const char *value)
{
  int inherit;

  if (!name)
    return;

  if (!ih)
  {
    IupStoreGlobal(name, value);
    return;
  }

  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return;

  if (iupATTRIB_ISINTERNAL(name))
    iupAttribSetStr(ih, name, value);
  else
  {
    int store = 1;

    if (iupATTRIB_LANGUAGE_STRING(value))
      iupATTRIB_GET_LANGUAGE_STRING(value, store);

    if (iupClassObjectSetAttribute(ih, name, value, &inherit)==1) /* store only strings */
    {
      if (store)
        iupAttribSetStr(ih, name, value);
      else
        iupAttribSet(ih, name, value);
    }

    if (inherit)
      iAttribNotifyChildren(ih, name, value);
  }
}

static void iAttribResetChildren(Ihandle *ih, const char* name)
{
  Ihandle* child = ih->firstchild;
  while (child)
  {
    /* set only if an inheritable attribute at the child */
    if (iAttribIsInherit(child, name))
    {
      iupAttribSet(child, name, NULL);

      iAttribResetChildren(child, name);
    }

    child = child->brother;
  }
}

void IupResetAttribute(Ihandle *ih, const char* name)
{
  iupASSERT(name!=NULL);
  if (!name)
    return;

  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return;

  iupAttribSet(ih, name, NULL);

  if (iAttribIsInherit(ih, name))
    iAttribResetChildren(ih, name);
}

char* IupGetAttribute(Ihandle *ih, const char* name)
{
  iupASSERT(name!=NULL);
  if (!name)
    return NULL;

  if (!ih)
    return IupGetGlobal(name);

  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return NULL;

  if (iupATTRIB_ISINTERNAL(name))
    return iupAttribGet(ih, name);
  else
  {
    int inherit;
    char *value, *def_value;

    value = iupClassObjectGetAttribute(ih, name, &def_value, &inherit);

    if (!value)
      value = iupAttribGet(ih, name);

    if (!value && inherit)
    {
      while (!value)
      {
        ih = ih->parent;
        if (!ih)
          break;

        value = iupAttribGet(ih, name);
      }
    }

    if (!value)
      value = def_value;

    return value;
  }
}

float IupGetFloat(Ihandle *ih, const char* name)
{
  float f = 0;
  iupStrToFloat(IupGetAttribute(ih, name), &f);
  return f;
}

double IupGetDouble(Ihandle *ih, const char* name)
{
  double f = 0;
  iupStrToDouble(IupGetAttribute(ih, name), &f);
  return f;
}

int IupGetInt(Ihandle *ih, const char* name)
{
  int i = 0;
  char *value = IupGetAttribute(ih, name);
  if (value)
  {
    if (!iupStrToInt(value, &i))
    {
      if (iupStrBoolean(value))
        i = 1;
    }
  }
  return i;
}

int IupGetInt2(Ihandle *ih, const char* name)
{
  int i1 = 0, i2 = 0;
  char *value = IupGetAttribute(ih, name);
  if (value)
  {
    char sep = 'x';
    if (strchr(value, ':')!=NULL)
      sep = ':';
    else if (strchr(value, ',')!=NULL)
      sep = ',';

    iupStrToIntInt(value, &i1, &i2, sep);
  }
  return i2;
}

int IupGetIntInt(Ihandle *ih, const char* name, int *i1, int *i2)
{
  int _i1 = 0, _i2 = 0;
  char *value = IupGetAttribute(ih, name);
  if (value)
  {
    int count;

    char sep = 'x';
    if (strchr(value, ':')!=NULL)
      sep = ':';
    else if (strchr(value, ',')!=NULL)
      sep = ',';

    count = iupStrToIntInt(value, &_i1, &_i2, sep);
    if (i1) *i1 = _i1;
    if (i2) *i2 = _i2;
    return count;
  }
  return 0;
}

void IupGetRGB(Ihandle *ih, const char* name, unsigned char *r, unsigned char *g, unsigned char *b)
{
  iupStrToRGB(IupGetAttribute(ih, name), r, g, b);
}

void IupSetStrfV(Ihandle* ih, const char* name, const char* f, va_list arglist)
{
  int size;
  char* value = iupStrGetLargeMem(&size);
  vsnprintf(value, size, f, arglist);
  IupStoreAttribute(ih, name, value);
}

void IupSetStrf(Ihandle *ih, const char* name, const char* f, ...)
{
  int size;
  char* value = iupStrGetLargeMem(&size);
  va_list arglist;
  va_start(arglist, f);
  vsnprintf(value, size, f, arglist);
  va_end(arglist);
  IupStoreAttribute(ih, name, value);
}

void IupSetfAttribute(Ihandle *ih, const char* name, const char* f, ...)
{
  int size;
  char* value = iupStrGetLargeMem(&size);
  va_list arglist;
  va_start(arglist, f);
  vsnprintf(value, size, f, arglist);
  va_end(arglist);
  IupStoreAttribute(ih, name, value);
}

void IupSetInt(Ihandle* ih, const char* name, int num)
{
  char value[20];  /* +4,294,967,296 */
  sprintf(value, "%d", num);
  IupStoreAttribute(ih, name, value);
}

void IupSetFloat(Ihandle* ih, const char* name, float num)
{
  char value[80];
  sprintf(value, IUP_FLOAT2STR, num);
  IupStoreAttribute(ih, name, value);
}

void IupSetDouble(Ihandle* ih, const char* name, double num)
{
  char value[80];
  sprintf(value, IUP_DOUBLE2STR, num);
  IupStoreAttribute(ih, name, value);
}

void IupSetRGB(Ihandle *ih, const char* name, unsigned char r, unsigned char g, unsigned char b)
{
  char value[60];
  sprintf(value, "%d %d %d", (int)r, (int)g, (int)b);
  IupStoreAttribute(ih, name, value);
}

void iupAttribSetHandleName(Ihandle *ih)
{
  char str_name[100];
  sprintf(str_name, "_IUP_NAME(%p)", ih);
  IupSetHandle(str_name, ih);
}

char* iupAttribGetHandleName(Ihandle *ih)
{
  char* name = iupAttribGet(ih, "HANDLENAME");  /* IupSetHandle was called at least once */
  if (name)
    return iupStrReturnStr(name);
  else
    return NULL;
}

void IupSetAttributeHandle(Ihandle* ih, const char* name, Ihandle* ih_named)
{
  char* handle_name;

  iupASSERT(name!=NULL);
  if (!name)
    return;

  iupASSERT(ih_named!=NULL);
  if (!ih_named)
    return;

  /* make sure it has at least one name */
  handle_name = iupAttribGetHandleName(ih_named);
  if (!handle_name)
  {
    iupAttribSetHandleName(ih_named);
    handle_name = iupAttribGetHandleName(ih_named);
  }

  IupStoreAttribute(ih, name, handle_name);
}

void IupSetAttributeHandleId(Ihandle* ih, const char* name, int id, Ihandle* ih_named)
{
  char nameid[100];
  sprintf(nameid, "%s%d", name, id);
  IupSetAttributeHandle(ih, nameid, ih_named);
}

Ihandle* IupGetAttributeHandle(Ihandle *ih, const char* name)
{
  char* handle_name;

  iupASSERT(name!=NULL);
  if (!name)
    return NULL;

  handle_name = IupGetAttribute(ih, name);
  return IupGetHandle(handle_name);
}

Ihandle* IupGetAttributeHandleId(Ihandle *ih, const char* name, int id)
{
  char nameid[100];
  sprintf(nameid, "%s%d", name, id);
  return IupGetAttributeHandle(ih, nameid);
}

static void iAttribSetNameId2(char* nameid, const char* name, int lin, int col)
{
  if (lin == IUP_INVALID_ID)
    sprintf(nameid, "%s*:%d", name, col);
  else if (col == IUP_INVALID_ID)
    sprintf(nameid, "%s%d:*", name, lin);
  else
    sprintf(nameid, "%s%d:%d", name, lin, col);
}

void IupSetAttributeHandleId2(Ihandle* ih, const char* name, int lin, int col, Ihandle* ih_named)
{
  char nameid[100];
  iAttribSetNameId2(nameid, name, lin, col);
  IupSetAttributeHandle(ih, nameid, ih_named);
}

Ihandle* IupGetAttributeHandleId2(Ihandle *ih, const char* name, int lin, int col)
{
  char nameid[100];
  iAttribSetNameId2(nameid, name, lin, col);
  return IupGetAttributeHandle(ih, nameid);
}

Ihandle* IupSetAttV(const char* handle_name, Ihandle* ih, const char* name, va_list arglist)
{
  const char *attr, *val;
  attr = name;
  while (attr)
  {
    val = va_arg(arglist, const char*);
    IupSetAttribute(ih, attr, val);
    attr = va_arg(arglist, const char*);
  }
  if (handle_name)
    IupSetHandle(handle_name, ih);
  return ih;
}

Ihandle* IupSetAtt(const char* handle_name, Ihandle* ih, const char* name, ...)
{
  va_list arglist;
  va_start(arglist, name);
  IupSetAttV(handle_name, ih, name, arglist);
  va_end(arglist);
  return ih;
}

void iupAttribSetClassObject(Ihandle* ih, const char* name, const char* value)
{
  int inherit;
  iupClassObjectSetAttribute(ih, name, value, &inherit);
}

void iupAttribSetClassObjectId(Ihandle* ih, const char* name, int id, const char* value)
{
  iupClassObjectSetAttributeId(ih, name, id, value);
}

void iupAttribSetClassObjectId2(Ihandle* ih, const char* name, int lin, int col, const char* value)
{
  iupClassObjectSetAttributeId2(ih, name, lin, col, value);
}

char* iupAttribGetClassObject(Ihandle* ih, const char* name)
{
  int inherit;
  char *def_value;
  return iupClassObjectGetAttribute(ih, name, &def_value, &inherit);
}

char* iupAttribGetClassObjectId(Ihandle* ih, const char* name, int id)
{
  return iupClassObjectGetAttributeId(ih, name, id);
}

char* iupAttribGetClassObjectId2(Ihandle* ih, const char* name, int lin, int col)
{
  return iupClassObjectGetAttributeId2(ih, name, lin, col);
}

void iupAttribSet(Ihandle* ih, const char* name, const char* value)
{
  if (!value)
    iupTableRemove(ih->attrib, name);
  else
    iupTableSet(ih->attrib, name, (void*)value, IUPTABLE_POINTER);
}

void iupAttribSetStr(Ihandle* ih, const char* name, const char* value)
{
  if (!value)
    iupTableRemove(ih->attrib, name);
  else
  {
    int store = 1;

    if (iupATTRIB_LANGUAGE_STRING(value))
      iupATTRIB_GET_LANGUAGE_STRING(value, store);

    if (store)
      iupTableSet(ih->attrib, name, (void*)value, IUPTABLE_STRING);
    else
      iupTableSet(ih->attrib, name, (void*)value, IUPTABLE_POINTER);
  }
}

void iupAttribSetStrf(Ihandle *ih, const char* name, const char* f, ...)
{
  char* value = iupStrGetMemory(1024);
  va_list arglist;
  va_start(arglist, f);
  vsnprintf(value, 1024, f, arglist);
  va_end(arglist);
  iupAttribSetStr(ih, name, value);
}

void iupAttribSetInt(Ihandle *ih, const char* name, int num)
{
  char value[20];  /* +4,294,967,296 */
  sprintf(value, "%d", num);
  iupAttribSetStr(ih, name, value);
}

void iupAttribSetFloat(Ihandle *ih, const char* name, float num)
{
  char value[80];
  sprintf(value, IUP_FLOAT2STR, num);
  iupAttribSetStr(ih, name, value);
}

void iupAttribSetDouble(Ihandle *ih, const char* name, double num)
{
  char value[80];
  sprintf(value, IUP_DOUBLE2STR, num);
  iupAttribSetStr(ih, name, value);
}

void iupAttribSetId(Ihandle *ih, const char* name, int id, const char* value)
{
  char nameid[100];
  sprintf(nameid, "%s%d", name, id);
  iupAttribSet(ih, nameid, value);
}

void iupAttribSetStrId(Ihandle *ih, const char* name, int id, const char* value)
{
  char nameid[100];
  sprintf(nameid, "%s%d", name, id);
  iupAttribSetStr(ih, nameid, value);
}

void iupAttribSetId2(Ihandle *ih, const char* name, int lin, int col, const char* value)
{
  char nameid[100];
  iAttribSetNameId2(nameid, name, lin, col);
  iupAttribSet(ih, nameid, value);
}

void iupAttribSetStrId2(Ihandle *ih, const char* name, int lin, int col, const char* value)
{
  char nameid[100];
  iAttribSetNameId2(nameid, name, lin, col);
  iupAttribSetStr(ih, nameid, value);
}

void iupAttribSetIntId(Ihandle *ih, const char* name, int id, int num)
{
  char nameid[100];
  sprintf(nameid, "%s%d", name, id);
  iupAttribSetInt(ih, nameid, num);
}

void iupAttribSetIntId2(Ihandle *ih, const char* name, int lin, int col, int num)
{
  char nameid[100];
  iAttribSetNameId2(nameid, name, lin, col);
  iupAttribSetInt(ih, nameid, num);
}

void iupAttribSetFloatId(Ihandle *ih, const char* name, int id, float num)
{
  char nameid[100];
  sprintf(nameid, "%s%d", name, id);
  iupAttribSetFloat(ih, nameid, num);
}

void iupAttribSetDoubleId(Ihandle *ih, const char* name, int id, double num)
{
  char nameid[100];
  sprintf(nameid, "%s%d", name, id);
  iupAttribSetDouble(ih, nameid, num);
}

void iupAttribSetFloatId2(Ihandle *ih, const char* name, int lin, int col, float num)
{
  char nameid[100];
  iAttribSetNameId2(nameid, name, lin, col);
  iupAttribSetFloat(ih, nameid, num);
}

void iupAttribSetDoubleId2(Ihandle *ih, const char* name, int lin, int col, double num)
{
  char nameid[100];
  iAttribSetNameId2(nameid, name, lin, col);
  iupAttribSetDouble(ih, nameid, num);
}

char* iupAttribGetId(Ihandle* ih, const char* name, int id)
{
  char nameid[100];
  sprintf(nameid, "%s%d", name, id);
  return iupAttribGet(ih, nameid);
}

int iupAttribGetIntId(Ihandle* ih, const char* name, int id)
{
  char nameid[100];
  sprintf(nameid, "%s%d", name, id);
  return iupAttribGetInt(ih, nameid);
}

int iupAttribGetBooleanId(Ihandle* ih, const char* name, int id)
{
  char nameid[100];
  sprintf(nameid, "%s%d", name, id);
  return iupAttribGetBoolean(ih, nameid);
}

float iupAttribGetFloatId(Ihandle* ih, const char* name, int id)
{
  char nameid[100];
  sprintf(nameid, "%s%d", name, id);
  return iupAttribGetFloat(ih, nameid);
}

double iupAttribGetDoubleId(Ihandle* ih, const char* name, int id)
{
  char nameid[100];
  sprintf(nameid, "%s%d", name, id);
  return iupAttribGetDouble(ih, nameid);
}

char* iupAttribGetId2(Ihandle* ih, const char* name, int lin, int col)
{
  char nameid[100];
  iAttribSetNameId2(nameid, name, lin, col);
  return iupAttribGet(ih, nameid);
}

int iupAttribGetIntId2(Ihandle* ih, const char* name, int lin, int col)
{
  char nameid[100];
  iAttribSetNameId2(nameid, name, lin, col);
  return iupAttribGetInt(ih, nameid);
}

int iupAttribGetBooleanId2(Ihandle* ih, const char* name, int lin, int col)
{
  char nameid[100];
  iAttribSetNameId2(nameid, name, lin, col);
  return iupAttribGetBoolean(ih, nameid);
}

float iupAttribGetFloatId2(Ihandle* ih, const char* name, int lin, int col)
{
  char nameid[100];
  iAttribSetNameId2(nameid, name, lin, col);
  return iupAttribGetFloat(ih, nameid);
}

double iupAttribGetDoubleId2(Ihandle* ih, const char* name, int lin, int col)
{
  char nameid[100];
  iAttribSetNameId2(nameid, name, lin, col);
  return iupAttribGetDouble(ih, nameid);
}

int iupAttribGetBoolean(Ihandle* ih, const char* name)
{
  char *value = iupAttribGetStr(ih, name);
  if (value)
  {
    if (iupStrBoolean(value))
      return 1;
  }
  return 0;
}

int iupAttribGetInt(Ihandle* ih, const char* name)
{
  int i = 0;
  char *value = iupAttribGetStr(ih, name);
  if (value)
  {
    if (!iupStrToInt(value, &i))
    {
      if (iupStrBoolean(value))
        i = 1;
    }
  }
  return i;
}

int iupAttribGetIntInt(Ihandle *ih, const char* name, int *i1, int *i2, char sep)
{
  int _i1 = 0, _i2 = 0;
  char *value = iupAttribGetStr(ih, name);
  if (value)
  {
    int count = iupStrToIntInt(value, &_i1, &_i2, sep);
    if (i1) *i1 = _i1;
    if (i2) *i2 = _i2;
    return count;
  }
  return 0;
}

float iupAttribGetFloat(Ihandle* ih, const char* name)
{
  float f = 0;
  char *value = iupAttribGetStr(ih, name);
  if (value)
    iupStrToFloat(value, &f);
  return f;
}

double iupAttribGetDouble(Ihandle* ih, const char* name)
{
  double f = 0;
  char *value = iupAttribGetStr(ih, name);
  if (value)
    iupStrToDouble(value, &f);
  return f;
}

char* iupAttribGet(Ihandle* ih, const char* name)
{
  if (!ih || !name)
    return NULL;
  return iupTableGet(ih->attrib, name);
}

char* iupAttribGetStr(Ihandle* ih, const char* name)
{
  char* value;
  if (!ih || !name)
    return NULL;

  value = iupTableGet(ih->attrib, name);

  if (!value && !iupATTRIB_ISINTERNAL(name))
  {
    int inherit;
    char *def_value;
    iupClassObjectGetAttributeInfo(ih, name, &def_value, &inherit);

    if (inherit)
    {
      while (!value)
      {
        ih = ih->parent;
        if (!ih)
          break;

        value = iupAttribGet(ih, name);
      }
    }

    if (!value)
      value = def_value;
  }

  return value;
}

char* iupAttribGetLocal(Ihandle* ih, const char* name)
{
  char* value;
  if (!ih || !name)
    return NULL;

  value = iupTableGet(ih->attrib, name);

  if (!value && !iupATTRIB_ISINTERNAL(name))
  {
    int inherit;
    char *def_value;
    value = iupClassObjectGetAttribute(ih, name, &def_value, &inherit);

    if (!value)
      value = def_value;
  }

  return value;
}

char* iupAttribGetInherit(Ihandle* ih, const char* name)
{
  char* value;
  if (!ih || !name)
    return NULL;

  value = iupAttribGet(ih, name);   /* Check on the element first */
  while (!value)
  {
    ih = ih->parent;   /* iheritance here independs on the attribute */
    if (!ih)
      return NULL;

    value = iupAttribGet(ih, name);
  }
  return value;
}

char* iupAttribGetInheritNativeParent(Ihandle* ih, const char* name)
{
  char* value;
  if (!ih || !name)
    return NULL;

  value = NULL;    /* Do NOT check on the element first */
  while (!value)
  {
    ih = iupChildTreeGetNativeParent(ih);
    if (!ih)
      return NULL;

    value = iupAttribGet(ih, name);
  }

  return value;
}

static const char* env_str = NULL;
static void iAttribCapture(char* env_buffer, char* dlm)
{
  int i=0;
  int c;
  do
  {
    c = *env_str; ++env_str;
    if (i < 256)
      env_buffer[i++] = (char) c;
  } while (c && !strchr(dlm,c));
  env_buffer[i-1]='\0';                                /* discard delimiter */
}

static void iAttribSkipComment(void)
{
  int c;
  do
  {
    c = *env_str; ++env_str;
  } while ((c > 0) && (c != '\n'));
}

static int iAttribToken(char* env_buffer)
{
  for (;;)
  {
    int c = *env_str; ++env_str;
    switch (c)
    {
    case 0:
      return IUPLEX_TK_END;

    case '#':          /* Skip comment */
    case '%':          /* Skip comment */
      iAttribSkipComment();
      continue;

    case ' ':          /* ignore whitespace */
    case '\t':
    case '\n':
    case '\r':
    case '\f':
    case '\v':
      continue;

    case '=':          /* attribuicao */
      return IUPLEX_TK_SET;

    case ',':
      return IUPLEX_TK_COMMA;

    case '\"':          /* string */
      iAttribCapture(env_buffer, "\"");
      return IUPLEX_TK_NAME;

    default:
      if (c > 32)          /* identifier */
      {
        --env_str;                     /* unget first character of env_buffer */
        iAttribCapture(env_buffer, "=, \t\n\r\f\v"); /* get env_buffer until delimiter */
        --env_str;                     /* unget delimiter */
        return IUPLEX_TK_NAME;
      }
    }
  }
}

static void iAttribParse(Ihandle *ih, const char* str)
{
  char env_buffer[256];
  char* name=NULL;
  char* value=NULL;
  char state = 'a';               /* get attribute */
  int end = 0;

  env_str = str;

  for (;;)
  {
    switch (iAttribToken(env_buffer))
    {
    case IUPLEX_TK_END:           /* same as IUPLEX_TK_COMMA */
      end = 1;
    case IUPLEX_TK_COMMA:
      if (name)
      {
        IupStoreAttribute(ih, name, value);
        free(name);
      }
      if (end)
        return;
      name = value = NULL;
      state = 'a';
      break;

    case IUPLEX_TK_SET:
      state = 'v';                /* get value */
      break;

    case IUPLEX_TK_NAME:
      if (state == 'a')
        name = iupStrDup(env_buffer);
      else
        value = env_buffer;
      break;
    }
  }
}

Ihandle* IupSetAttributes(Ihandle *ih, const char* str)
{
  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return ih;
  if (str)
    iAttribParse(ih, str);
  return ih;
}

int iupAttribIsNotString(Ihandle* ih, const char* name)
{
  return iupClassObjectAttribIsNotString(ih, name);
}

int iupAttribIsIhandle(Ihandle* ih, const char* name)
{
  return iupClassObjectAttribIsIhandle(ih, name);
}

typedef int (*Iconvertxytopos)(Ihandle* ih, int x, int y);

int IupConvertXYToPos(Ihandle* ih, int x, int y)
{
  Iconvertxytopos drvConvertXYToPos;

  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return -1;

  if (!ih->handle)
    return -1;

  drvConvertXYToPos = (Iconvertxytopos)IupGetCallback(ih, "_IUP_XY2POS_CB");
  if (drvConvertXYToPos)
    return drvConvertXYToPos(ih, x, y);

  return -1;
}

int IupStringCompare(const char* str1, const char* str2, int casesensitive, int lexicographic)
{
  if (lexicographic)
  {
    int utf8 = IupGetInt(NULL, "UTF8MODE");
    return iupStrCompare(str1, str2, casesensitive, utf8);
  }
  else
  {
    if (casesensitive)
      return !iupStrEqual(str1, str2);  /* return 0 if equal */
    else
      return !iupStrEqualNoCase(str1, str2);  /* return 0 if equal */
  }
}
