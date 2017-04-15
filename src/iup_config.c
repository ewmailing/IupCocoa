/** \file
 * \brief Configuration file Utilities
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <iup.h>
#include <iupcbs.h>

#include "iup_object.h"
#include "iup_config.h"
#include "iup_linefile.h"
#include "iup_str.h"


#define GROUPKEYSIZE 100
#define MAX_LINES 500


static char* strGetGroupKeyName(const char* group, const char* key)
{
  static char str[GROUPKEYSIZE];
  strcpy(str, group);
  strcat(str, ".");
  strcat(str, key);
  return str;
}

static char* strSetGroupKeyName(const char* groupkey, char* group)
{
  int len;
  const char* key = strchr(groupkey, '.');
  if (!key) return NULL;
  len = (int)(key - groupkey);
  memcpy(group, groupkey, len);
  group[len] = 0;
  return (char*)(key + 1);
}

Ihandle* IupConfig(void)
{
  return IupUser();
}

static char* iConfigSetFilename(Ihandle* ih)
{
  char* app_name;
  char* app_path;
  int app_config;
  char* home;
  
  char filename[10240] = "";
  char* app_filename = IupGetAttribute(ih, "APP_FILENAME");
  if (app_filename)
    return app_filename;

  app_name = IupGetAttribute(ih, "APP_NAME");
  app_path = IupGetAttribute(ih, "APP_PATH");
  app_config = IupGetInt(ih, "APP_CONFIG");

  if (!app_name)
    return NULL;

  home = getenv("HOME");
  if (home && !app_config)
  {
    /* UNIX format */
    strcat(filename, home);
    strcat(filename, "/.");
    strcat(filename, app_name);
  }
  else
  {
    /* Windows format */
    char* homedrive = getenv("HOMEDRIVE");
    char* homepath = getenv("HOMEPATH");
    if (homedrive && homepath && !app_config)
    {
      strcat(filename, homedrive);
      strcat(filename, homepath);
      strcat(filename, "\\");
      strcat(filename, app_name);
      strcat(filename, ".cfg");
    }
    else
    {
      if (!app_path)
        return NULL;

      strcat(filename, app_path);
#ifndef WIN32
      strcat(filename, ".");
#endif
      strcat(filename, app_name);
#ifdef WIN32
      strcat(filename, ".cfg");
#endif
    }
  }

  IupSetStrAttribute(ih, "FILENAME", filename);

  return IupGetAttribute(ih, "FILENAME");
}


static int sort_names_cb(const void* elem1, const void* elem2)
{
  char* str1 = *((char**)elem1);
  char* str2 = *((char**)elem2);
  return strcmp(str1, str2);
}

int IupConfigLoad(Ihandle* ih)
{
  char group[GROUPKEYSIZE] = "";
  char key[GROUPKEYSIZE];
  IlineFile* line_file;
  
  char* filename = iConfigSetFilename(ih);
  if (!filename)
    return -3;

  line_file = iupLineFileOpen(filename);
  if (!line_file)
    return -1;

  do
  {
    const char* line_buffer;

    int line_len = iupLineFileReadLine(line_file);
    if (line_len == -1)
      return -2;

    line_buffer = iupLineFileGetBuffer(line_file);

    if (line_buffer[0] == 0)  /* skip empty line */
      continue;

    if (line_buffer[0] == '#')  /* "#" signifies a comment line */
      continue;

    if (line_buffer[0] == '[')  /* group start */
    {
      group[0] = 0;
      sscanf(line_buffer, "[%[^]]s]", group);
    }
    else
    {
      const char* value;

      key[0] = 0;
      sscanf(line_buffer, "%[^=]s", key);

      value = strstr(line_buffer, "=");
      if (!value)                    
        value = line_buffer;
      else
        value++;  // Skip =

      IupConfigSetVariableStr(ih, group, key, value);
    }
  } while (!iupLineFileEOF(line_file));

  iupLineFileClose(line_file);
  return 0;
}

int IupConfigSave(Ihandle* ih)
{
  char* names[MAX_LINES];
  FILE* file;
  int i, count;
  char last_group[GROUPKEYSIZE] = "", group[GROUPKEYSIZE], *key;
  
  char* filename = iConfigSetFilename(ih);
  if (!filename)
    return -3;

  file = fopen(filename, "w");
  if (!file)
    return -1;

  count = IupGetAllAttributes(ih, names, MAX_LINES);

  qsort(names, count, sizeof(char*), sort_names_cb);

  for (i = 0; i<count; i++)
  {
    char* value;
    
    key = strSetGroupKeyName(names[i], group);
    if (!key)
      continue;

    if (!iupStrEqual(group, last_group))
    {
      /* write a new group */
      fprintf(file, "\n[%s]\n", group);
      strcpy(last_group, group);
    }

    value = IupGetAttribute(ih, names[i]);
    fprintf(file, "%s=%s\n", key, value);

    if (ferror(file))
    {
      fclose(file);
      return -2;
    }
  }

  fclose(file);
  return 0;
}

void IupConfigSetVariableStrId(Ihandle* ih, const char* group, const char* key, int id, const char* value)
{
  char key_id[GROUPKEYSIZE];
  sprintf(key_id, "%s%d", key, id);
  IupConfigSetVariableStr(ih, group, key_id, value);
}

void IupConfigSetVariableIntId(Ihandle* ih, const char* group, const char* key, int id, int value)
{
  char key_id[GROUPKEYSIZE];
  sprintf(key_id, "%s%d", key, id);
  IupConfigSetVariableInt(ih, group, key_id, value);
}

void IupConfigSetVariableDoubleId(Ihandle* ih, const char* group, const char* key, int id, double value)
{
  char key_id[GROUPKEYSIZE];
  sprintf(key_id, "%s%d", key, id);
  IupConfigSetVariableDouble(ih, group, key_id, value);
}

void IupConfigSetVariableStr(Ihandle* ih, const char* group, const char* key, const char* value)
{
  IupSetStrAttribute(ih, strGetGroupKeyName(group, key), value);
}

void IupConfigSetVariableInt(Ihandle* ih, const char* group, const char* key, int value)
{
  IupSetInt(ih, strGetGroupKeyName(group, key), value);
}

void IupConfigSetVariableDouble(Ihandle* ih, const char* group, const char* key, double value)
{
  IupSetDouble(ih, strGetGroupKeyName(group, key), value);
}

const char* IupConfigGetVariableStr(Ihandle* ih, const char* group, const char* key)
{
  return IupGetAttribute(ih, strGetGroupKeyName(group, key));
}

int IupConfigGetVariableInt(Ihandle* ih, const char* group, const char* key)
{
  return IupGetInt(ih, strGetGroupKeyName(group, key));
}

double IupConfigGetVariableDouble(Ihandle* ih, const char* group, const char* key)
{
  return IupGetDouble(ih, strGetGroupKeyName(group, key));
}

const char* IupConfigGetVariableStrDef(Ihandle* ih, const char* group, const char* key, const char* def)
{
  if (!IupGetAttribute(ih, strGetGroupKeyName(group, key)))
    return def;
  else
    return IupConfigGetVariableStr(ih, group, key);
}

const char* IupConfigGetVariableStrIdDef(Ihandle* ih, const char* group, const char* key, int id, const char* def)
{
  if (!IupGetAttributeId(ih, strGetGroupKeyName(group, key), id))
    return def;
  else
    return IupConfigGetVariableStrId(ih, group, key, id);
}

int IupConfigGetVariableIntDef(Ihandle* ih, const char* group, const char* key, int def)
{
  if (!IupGetAttribute(ih, strGetGroupKeyName(group, key)))
    return def;
  else
    return IupConfigGetVariableInt(ih, group, key);
}

int IupConfigGetVariableIntIdDef(Ihandle* ih, const char* group, const char* key, int id, int def)
{
  if (!IupGetAttributeId(ih, strGetGroupKeyName(group, key), id))
    return def;
  else
    return IupConfigGetVariableIntId(ih, group, key, id);
}

double IupConfigGetVariableDoubleDef(Ihandle* ih, const char* group, const char* key, double def)
{
  if (!IupGetAttribute(ih, strGetGroupKeyName(group, key)))
    return def;
  else
    return IupConfigGetVariableDouble(ih, group, key);
}

double IupConfigGetVariableDoubleIdDef(Ihandle* ih, const char* group, const char* key, int id, double def)
{
  if (!IupGetAttributeId(ih, strGetGroupKeyName(group, key), id))
    return def;
  else
    return IupConfigGetVariableDoubleId(ih, group, key, id);
}

const char* IupConfigGetVariableStrId(Ihandle* ih, const char* group, const char* key, int id)
{
  char key_id[GROUPKEYSIZE];
  sprintf(key_id, "%s%d", key, id);
  return IupConfigGetVariableStr(ih, group, key_id);
}

int IupConfigGetVariableIntId(Ihandle* ih, const char* group, const char* key, int id)
{
  char key_id[GROUPKEYSIZE];
  sprintf(key_id, "%s%d", key, id);
  return IupConfigGetVariableInt(ih, group, key_id);
}

double IupConfigGetVariableDoubleId(Ihandle* ih, const char* group, const char* key, int id)
{
  char key_id[GROUPKEYSIZE];
  sprintf(key_id, "%s%d", key, id);
  return IupConfigGetVariableDouble(ih, group, key_id);
}


/******************************************************************/


static int iConfigItemRecent_CB(Ihandle* ih_item)
{
  Ihandle* ih = (Ihandle*)IupGetAttribute(ih_item, "_IUP_CONFIG");
  Icallback recent_cb = IupGetCallback(ih, "RECENT_CB");
  if (recent_cb)
  {
    IupSetStrAttribute(ih, "TITLE", IupGetAttribute(ih_item, "TITLE"));
    ih->parent = ih_item;
    recent_cb(ih);
    ih->parent = NULL;
  }
  return IUP_DEFAULT;
}

static void iConfigBuildRecent(Ihandle* ih)
{
  /* add the new items, reusing old ones */
  int max_recent = IupGetInt(ih, "RECENTMAX");
  Ihandle* menu = (Ihandle*)IupGetAttribute(ih, "RECENTMENU");

  int mapped = IupGetAttribute(menu, "WID") != NULL ? 1 : 0;
  const char* value;
  int i = 1;
  do
  {
    value = IupConfigGetVariableStrId(ih, "Recent", "File", i);
    if (value)
    {
      Ihandle* item = IupGetChild(menu, i - 1);
      if (item)
        IupSetStrAttribute(item, "TITLE", value);
      else
      {
        item = IupItem(value, NULL);
        IupSetAttribute(item, "_IUP_CONFIG", (char*)ih);
        IupSetCallback(item, "ACTION", iConfigItemRecent_CB);
        IupAppend(menu, item);
        if (mapped) IupMap(item);
      }
    }
    i++;
  } while (value && i <= max_recent);
}

void IupConfigRecentInit(Ihandle* ih, Ihandle* menu, Icallback recent_cb, int max_recent)
{
  IupSetAttribute(ih, "RECENTMENU", (char*)menu);
  IupSetCallback(ih, "RECENT_CB", recent_cb);
  IupSetInt(ih, "RECENTMAX", max_recent);

  iConfigBuildRecent(ih);
}

void IupConfigRecentUpdate(Ihandle* ih, const char* filename)
{
  int max_recent = IupGetInt(ih, "RECENTMAX");
  const char* value = IupConfigGetVariableStr(ih, "Recent", "File1");
  if (value && !iupStrEqual(value, filename))
  {
    /* must update the stack */
    int found = 0;

    // First search for the new filename to avoid duplicates
    int i = 1;
    do
    {
      value = IupConfigGetVariableStrId(ih, "Recent", "File", i);

      if (value && iupStrEqual(value, filename))
      {
        found = i;
        break;
      }

      i++;
    } while (value && i <= max_recent);

    // simply open space for the new filename
    if (found)
      i = found;
    else
      i = max_recent;
    do
    {
      value = IupConfigGetVariableStrId(ih, "Recent", "File", i - 1);
      IupConfigSetVariableStrId(ih, "Recent", "File", i, value);

      i--;
    } while (i > 1);
  }

  /* push new at start always */
  IupConfigSetVariableStr(ih, "Recent", "File1", filename);

  iConfigBuildRecent(ih);
}


/*******************************************************************/


void IupConfigDialogShow(Ihandle* ih, Ihandle* dialog, const char* name)
{
  int shown = 0;
  int set_size = 0;

  /* does nothing if already visible */
  if (IupGetInt(dialog, "VISIBLE"))
  {
    IupShow(dialog);
    return;
  }

  if (IupConfigGetVariableIntDef(ih, name, "Maximized", 1) && IupGetInt(dialog, "RESIZE"))
  {
    IupSetAttribute(dialog, "PLACEMENT", "MAXIMIZED");
  }
  else
  {
    /* set size only if dialog is resizable */
    if (IupGetInt(dialog, "RESIZE"))
    {
      /* dialog size from Config */
      int width = IupConfigGetVariableInt(ih, name, "Width");
      int height = IupConfigGetVariableInt(ih, name, "Height");
      if (width != 0 || height != 0)
      {
        int screen_width = 0, screen_height = 0;
        IupGetIntInt(NULL, "SCREENSIZE", &screen_width, &screen_height);
        if (width == 0)
          width = screen_width;
        if (height == 0)
          height = screen_height;

#ifdef WIN32
        IupSetfAttribute(dialog, "RASTERSIZE", "%dx%d", width, height);
#else
        IupMap(dialog);
        IupSetfAttribute(dialog, "CLIENTSIZE", "%dx%d", width, height);
#endif
        set_size = 1;
      }
    }

    /* dialog position from Config */
    if (IupConfigGetVariableStr(ih, name, "X"))
    {
      int x = IupConfigGetVariableInt(ih, name, "X");
      int y = IupConfigGetVariableInt(ih, name, "Y");
      int virtual_x, virtual_y, virtual_w, virtual_h;
      int monitors_count = IupGetInt(NULL, "MONITORSCOUNT");
      if (monitors_count > 1)
      {
        char* virtual_screen = IupGetGlobal("VIRTUALSCREEN");
        sscanf(virtual_screen, "%d %d %d %d", &virtual_x, &virtual_y, &virtual_w, &virtual_h);
      }
      else
      {
        virtual_x = 0;
        virtual_y = 0;
        IupGetIntInt(NULL, "SCREENSIZE", &virtual_w, &virtual_h);
      }

      if (x < virtual_x) x = virtual_x;
      if (y < virtual_y) y = virtual_y;
      if (x > virtual_x + virtual_w) x = virtual_x;
      if (y > virtual_y + virtual_h) y = virtual_y;

      IupShowXY(dialog, x, y);
      shown = 1;
    }
  }

  if (!shown)
    IupShow(dialog);

  if (set_size)
    IupSetAttribute(dialog, "USERSIZE", NULL);  /* clear minimum restriction without reseting the current size */
}

void IupConfigDialogClosed(Ihandle* ih, Ihandle* dialog, const char* name)
{
  int x, y;
  int width, height;
  int maximized;
  int screen_width = 0, screen_height = 0;
  
  IupGetIntInt(dialog, "SCREENPOSITION", &x, &y);
  IupConfigSetVariableInt(ih, name, "X", x);
  IupConfigSetVariableInt(ih, name, "Y", y);

  /* save size only if dialog is resizable */
  if (IupGetInt(dialog, "RESIZE"))
  {
#ifdef WIN32
    IupGetIntInt(dialog, "RASTERSIZE", &width, &height);
#else
    IupGetIntInt(dialog, "CLIENTSIZE", &width, &height);
#endif
    IupConfigSetVariableInt(ih, name, "Width", width);
    IupConfigSetVariableInt(ih, name, "Height", height);

    maximized = IupGetInt(dialog, "MAXIMIZED"); // Windows Only
    IupGetIntInt(NULL, "SCREENSIZE", &screen_width, &screen_height);
    if (maximized ||
        ((x < 0 && (2 * x + width == screen_width)) &&    // Works only for the main screen
        (y < 0 && (2 * y + height == screen_height))))
        IupConfigSetVariableInt(ih, name, "Maximized", 1);
    else
      IupConfigSetVariableInt(ih, name, "Maximized", 0);
  }
}
