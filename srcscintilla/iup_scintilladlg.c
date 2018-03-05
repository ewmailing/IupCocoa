#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "iup.h"
#include "iupcbs.h"
#include "iup_scintilla.h"
#include "iup_config.h"
#include "iup_linefile.h"

#include "iup_object.h"
#include "iup_attrib.h"
#include "iup_str.h"
#include "iup_strmessage.h"
#include "iup_register.h"
#include "iup_childtree.h"
#include "iup_predialogs.h"


#define LINENUMBER_MARGIN "50"
#define BOOKMARK_MARGIN "20"

#define WINDOWMENU_MAX 10


/********************************** Utilities *****************************************/


static void copyFilenameToClipboard(const char *filename)
{
  Ihandle *clipboard = IupClipboard();
  IupSetAttribute(clipboard, "TEXT", NULL); /* clear clipboard first */
  IupSetAttribute(clipboard, "TEXT", filename);
  IupDestroy(clipboard);
}

static void saveMarkers(Ihandle* config, Ihandle* multitext)
{
  char* m_filename = IupGetAttribute(multitext, "FILENAME");
  char filename[10240], line_str[30];
  const char* value;
  int i, lin, count;
  IFnn cb;

  count = IupConfigGetVariableInt(config, "SciBookmarks", "Count");

  /* clear all bookmarks for this filename and compact list */
  for (i = 1; i <= count;)
  {
    value = IupConfigGetVariableStrId(config, "SciBookmarks", "FileLine", i);
    iupStrToStrStr(value, filename, line_str, '#');
    if (iupStrEqual(filename, m_filename))
    {
      IupConfigSetVariableStrId(config, "SciBookmarks", "FileLine", i, IupConfigGetVariableStrId(config, "SciBookmarks", "FileLine", count));
      IupConfigSetVariableStrId(config, "SciBookmarks", "FileLine", count, NULL);
      count--;
    }
    else
      i++;
  }

  /* add current bookmarks at the end */
  lin = 0;
  while (lin >= 0)
  {
    IupSetAttributeId(multitext, "MARKERNEXT", lin, "1");  /* 0001 - marker=0 */
    lin = IupGetInt(multitext, "LASTMARKERFOUND");
    if (lin >= 0)
    {
      count++;
      sprintf(filename, "%s#%d", m_filename, lin + 1);  /* use the same origin for line numbers */
      IupConfigSetVariableStrId(config, "SciBookmarks", "FileLine", count, filename);

      lin++;
    }
  }

  IupConfigSetVariableInt(config, "SciBookmarks", "Count", count);

  /* make sure some older bookmarks are not saved in the configuration file (at least 10) */
  for (i = count + 1; i <= count + 1 + 10; i++)
    IupConfigSetVariableStrId(config, "SciBookmarks", "FileLine", i, NULL);


  cb = (IFnn)IupGetCallback(IupGetDialog(multitext), "SAVEMARKERS_CB");
  if (cb)
    cb(IupGetDialog(multitext), multitext);
}

static void saveAllMarkers(Ihandle* ih, Ihandle* config)
{
  Ihandle* tabs = IupGetDialogChild(ih, "TABS");
  Ihandle* multitext;

  for (multitext = tabs->firstchild; multitext; multitext = multitext->brother)
    saveMarkers(config, multitext);
}

static void restoreMarkers(Ihandle* config, Ihandle* multitext)
{
  char* m_filename = IupGetAttribute(multitext, "FILENAME");
  char filename[10240], line_str[30];
  const char* value;
  int i, line, count;
  IFnn cb;

  IupSetInt(multitext, "MARKERDELETEALL", 0);

  count = IupConfigGetVariableInt(config, "SciBookmarks", "Count");

  /* clear all bookmarks for this filename and compact list */
  for (i = 1; i <= count; i++)
  {
    value = IupConfigGetVariableStrId(config, "SciBookmarks", "FileLine", i);
    iupStrToStrStr(value, filename, line_str, '#');
    if (iupStrEqual(filename, m_filename))
    {
      unsigned int markerMask;
      int has_bookmark;

      iupStrToInt(line_str, &line);

      markerMask = (unsigned int)IupGetIntId(multitext, "MARKERGET", line);
      has_bookmark = markerMask & 0x0001; /* 0001 - marker=0 */

      if (!has_bookmark)
        IupSetIntId(multitext, "MARKERADD", line - 1, 0);
    }
  }

  cb = (IFnn)IupGetCallback(IupGetDialog(multitext), "RESTOREMARKERS_CB");
  if (cb)
    cb(IupGetDialog(multitext), multitext);
}

static void addBookmark(Ihandle* multitext, int lin)
{
  IupSetIntId(multitext, "MARKERADD", lin, 0); /* marker=0 */
}

static void removeBookmark(Ihandle* multitext, int lin)
{
  IupSetIntId(multitext, "MARKERDELETE", lin, 0); /* marker=0 */
}

static void removeAllBookmark(Ihandle* multitext)
{
  IupSetInt(multitext, "MARKERDELETEALL", 0); /* marker=0 */
}

static void toggleMarker(Ihandle* multitext, int lin, int margin)
{
  Ihandle* ih = IupGetDialog(multitext);
  IFnnii cb;

  /* bookmarks */
  if (margin == 1)
  {
    unsigned int markerMask = (unsigned int)IupGetIntId(multitext, "MARKERGET", lin);
    int has_bookmark = markerMask & 0x0001; /* 0001 - marker=0 */

    if (has_bookmark)
      removeBookmark(multitext, lin);
    else
      addBookmark(multitext, lin);
  }

  cb = (IFnnii)IupGetCallback(ih, "MARKERCHANGED_CB");
  if (cb)
    cb(ih, multitext, lin, margin);
}

static void copyMarkedLines(Ihandle *multitext)
{
  int size = IupGetInt(multitext, "COUNT");
  char *buffer = (char *)malloc(size);
  char *text;
  int lin = 0;

  buffer[0] = 0;
  while (lin >= 0)
  {
    IupSetAttributeId(multitext, "MARKERNEXT", lin, "1");  /* 0001 - marker=0 */
    lin = IupGetInt(multitext, "LASTMARKERFOUND");
    if (lin >= 0)
    {
      text = IupGetAttributeId(multitext, "LINE", lin);
      strcat(buffer, text);  size -= (int)strlen(text);
      lin++;
    }
  }

  if (strlen(buffer) > 0)
  {
    Ihandle *clipboard = IupClipboard();
    IupSetAttribute(clipboard, "TEXT", NULL); /* clear clipboard first */
    IupSetAttribute(clipboard, "TEXT", buffer);
    IupDestroy(clipboard);
  }

  free(buffer);
}

static void cutMarkedLines(Ihandle *multitext)
{
  int size = IupGetInt(multitext, "COUNT");
  char *buffer = (char *)malloc(size);
  char *text;
  int lin = 0, pos, len;

  buffer[0] = 0;
  while (lin >= 0 && size)
  {
    IupSetAttributeId(multitext, "MARKERNEXT", lin, "1");  /* 0001 - marker=0 */
    lin = IupGetInt(multitext, "LASTMARKERFOUND");
    if (lin >= 0)
    {
      text = IupGetAttributeId(multitext, "LINE", lin);
      len = (int)strlen(text);
      IupTextConvertLinColToPos(multitext, lin, 0, &pos);
      IupSetStrf(multitext, "DELETERANGE", "%d,%d", pos, len);
      strcat(buffer, text);  size -= len;
      removeBookmark(multitext, lin);
      lin--;
    }
  }

  if (strlen(buffer) > 0)
  {
    Ihandle *clipboard = IupClipboard();
    IupSetAttribute(clipboard, "TEXT", NULL); /* clear clipboard first */
    IupSetAttribute(clipboard, "TEXT", buffer);
    IupDestroy(clipboard);
  }

  free(buffer);
}

static void pasteToMarkedLines(Ihandle *multitext)
{
  char *text;
  int lin = 0, pos, len;
  Ihandle *clipboard;

  while (lin >= 0)
  {
    IupSetAttributeId(multitext, "MARKERNEXT", lin, "1");  /* 0001 - marker=0 */
    lin = IupGetInt(multitext, "LASTMARKERFOUND");
    if (lin >= 0)
    {
      text = IupGetAttributeId(multitext, "LINE", lin);
      len = (int)strlen(text);
      IupTextConvertLinColToPos(multitext, lin, 0, &pos);
      IupSetStrf(multitext, "DELETERANGE", "%d,%d", pos, len);
      removeBookmark(multitext, lin);
      clipboard = IupClipboard();
      IupSetAttributeId(multitext, "INSERT", pos, IupGetAttribute(clipboard, "TEXT"));
      IupDestroy(clipboard);
      lin--;
    }
  }
}

static void invertMarkedLines(Ihandle *multitext)
{
  int lin;
  for (lin = 0; lin < IupGetInt(multitext, "LINECOUNT"); lin++)
  {
    toggleMarker(multitext, lin, 1);
  }
}

static void removeMarkedLines(Ihandle *multitext)
{
  char *text;
  int lin = 0, pos, len;

  while (lin >= 0)
  {
    IupSetAttributeId(multitext, "MARKERNEXT", lin, "1");  /* 0001 - marker=0 */
    lin = IupGetInt(multitext, "LASTMARKERFOUND");
    if (lin >= 0)
    {
      text = IupGetAttributeId(multitext, "LINE", lin);
      len = (int)strlen(text);
      IupTextConvertLinColToPos(multitext, lin, 0, &pos);
      IupSetStrf(multitext, "DELETERANGE", "%d,%d", pos, len);
      removeBookmark(multitext, lin);
      lin--;
    }
  }
}

static void changeTabsToSpaces(Ihandle *multitext)
{
  char *text = IupGetAttribute(multitext, "VALUE");
  int count = IupGetInt(multitext, "COUNT");
  int tabSize = IupGetInt(multitext, "TABSIZE");
  int lin, col, i, j;

  if (tabSize == 0)
    return;

  for (i = count - 1; i >= 0; i--)
  {
    int spacesToNextTab;
    char c = text[i];

    if (c != '\t')
      continue;

    IupSetStrf(multitext, "DELETERANGE", "%d,%d", i, 1);

    IupTextConvertPosToLinCol(multitext, i, &lin, &col);
    spacesToNextTab = tabSize - (col + 1) % tabSize + 1;

    for (j = 0; j < spacesToNextTab; j++)
      IupSetAttributeId(multitext, "INSERT", i + j, " ");
  }
}

static void changeSpacesToTabs(Ihandle *multitext)
{
  char *text = IupGetAttribute(multitext, "VALUE");
  int count = IupGetInt(multitext, "COUNT");
  int tabSize = IupGetInt(multitext, "TABSIZE");
  int lin, col, i;

  if (tabSize == 0)
    return;

  for (i = count - 1; i >= 0; i--)
  {
    int nSpaces, tabStop;
    char c = text[i];

    IupTextConvertPosToLinCol(multitext, i, &lin, &col);

    tabStop = (col + 1) % tabSize == tabSize - 1 ? 1 : 0;

    if (!tabStop || c != ' ')
      continue;

    IupSetStrf(multitext, "DELETERANGE", "%d,%d", i + 1, 1);
    IupSetAttributeId(multitext, "INSERT", i + 1, "\t");

    nSpaces = 0;

    while (text[i - nSpaces] == ' ' && nSpaces < tabSize - 1)
      nSpaces++;

    if (nSpaces == 0)
      continue;

    i -= nSpaces;

    IupSetStrf(multitext, "DELETERANGE", "%d,%d", i + 1, nSpaces);
  }
}

static void changeLeadingSpacesToTabs(Ihandle *multitext)
{
  int lineCount = IupGetInt(multitext, "LINECOUNT");
  int tabSize = IupGetInt(multitext, "TABSIZE");
  int pos, i, j;

  if (tabSize == 0)
    return;

  for (i = 0; i < lineCount; i++)
  {
    int tabCount = 0;
    int spaceCount = 0;
    char *text = IupGetAttributeId(multitext, "LINE", i);

    int len = (int)strspn(text, " \t");
    if (len == 0)
      continue;

    for (j = 0; j < len; j++)
    {
      if (text[j] == '\t')
      {
        tabCount++;
        spaceCount = 0;
      }
      else
        spaceCount++;

      if (spaceCount == tabSize)
      {
        tabCount++;
        spaceCount = 0;
      }
    }
    IupTextConvertLinColToPos(multitext, i, 0, &pos);
    IupSetStrf(multitext, "DELETERANGE", "%d,%d", pos, len);
    for (j = 0; j < spaceCount; j++)
      IupSetAttributeId(multitext, "INSERT", pos, " ");
    for (j = 0; j < tabCount; j++)
      IupSetAttributeId(multitext, "INSERT", pos, "\t");
  }
}

static void removeLeadingSpaces(Ihandle *multitext)
{
  int lineCount = IupGetInt(multitext, "LINECOUNT");
  int pos, i;

  for (i = 0; i < lineCount; i++)
  {
    char *text = IupGetAttributeId(multitext, "LINE", i);
    int len = (int)strspn(text, " \t");
    if (len == 0)
      continue;

    IupTextConvertLinColToPos(multitext, i, 0, &pos);
    IupSetStrf(multitext, "DELETERANGE", "%d,%d", pos, len);
  }
}

static void removeTrailingSpaces(Ihandle *multitext)
{
  int lineCount = IupGetInt(multitext, "LINECOUNT");
  int pos, i, j;

  for (i = 0; i < lineCount; i++)
  {
    int count = 0;
    char *text = IupGetAttributeId(multitext, "LINE", i);
    int len = (int)strlen(text);
    if (len == 0)
      continue;

    if (text[len - 1] == '\n' || text[len - 1] == '\r')
    {
      len--;

      if (text[len - 1] == '\r')
        len--;
    }

    for (j = len - 1; j >= 0; j--)
    {
      if (text[j] != ' ' && text[j] != '\t')
        break;
      count++;
    }

    if (count == 0)
      continue;

    IupTextConvertLinColToPos(multitext, i, len - count, &pos);
    IupSetStrf(multitext, "DELETERANGE", "%d,%d", pos, count);
  }
}

static void changeEolToSpace(Ihandle *multitext)
{
  int lineCount = IupGetInt(multitext, "LINECOUNT");
  int i;

  for (i = lineCount-1; i >= 0; i--)
  {
    int pos, count;
    char *text = IupGetAttributeId(multitext, "LINE", i);
    int len = (int)strlen(text);
    if (len == 0)
      continue;

    count = 0;
    if (text[len - 1] == '\n' || text[len - 1] == '\r')
    {
      len--;
      count++;

      if (text[len - 1] == '\r')
      {
        len--;
        count++;
      }
    }

    if (count == 0)
      continue;

    IupTextConvertLinColToPos(multitext, i, len, &pos);
    IupSetStrf(multitext, "DELETERANGE", "%d,%d", pos, count);
    IupSetAttributeId(multitext, "INSERT", pos, " ");
  }
}

static const char* strFileTitle(const char *filename)
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

static char* readFile(const char* filename)
{
  long size;
  char* str;
  FILE* file = fopen(filename, "rb");
  if (!file)
    return NULL;

  /* calculate file size */
  fseek(file, 0, SEEK_END);
  size = ftell(file);
  if (size <= 0)
  {
    fclose(file);
    return NULL;
  }

  /* allocate memory for the file contents + nul terminator */
  str = malloc(size + 1);
  if (!str)
  {
    fclose(file);
    return NULL;
  }

  /* read all data at once */
  fseek(file, 0, SEEK_SET);
  fread(str, size, 1, file);

  /* set the nul terminator */
  str[size] = 0;

  fclose(file);
  return str;
}

static int writeFile(const char* filename, const char* str, int count)
{
  FILE* file = fopen(filename, "wb");
  if (!file)
    return 0;

  fwrite(str, 1, count, file);
  fclose(file);
  return 1;
}

static Ihandle* iScintillaDlgGetConfig(Ihandle* ih)
{
  return (Ihandle*)iupAttribGetInherit(ih, "_IUP_CONFIG");
}

static Ihandle* iScintillaDlgGetProjectConfig(Ihandle* ih)
{
  return (Ihandle*)iupAttribGetInherit(ih, "_IUP_PROJECT_CONFIG");
}

static void saveProjectOpenFiles(Ihandle *ih, Ihandle *projectConfig)
{
  Ihandle* tabs = IupGetDialogChild(ih, "TABS");
  Ihandle* multitext;
  char* filename;
  int i;
  int count = IupConfigGetVariableInt(projectConfig, "ProjectOpenFiles", "Count");

  /* clear everything before saving */
  for (i = 1; i <= count; i++)
    IupConfigSetVariableStrId(projectConfig, "ProjectOpenFiles", "File", i, NULL);
  IupConfigSetVariableStr(projectConfig, "ProjectOpenFiles", "Count", NULL);

  i = 1;
  for (multitext = tabs->firstchild; multitext; multitext = multitext->brother)
  {
    filename = IupGetAttribute(multitext, "FILENAME");
    if (!filename || iupStrEqual(filename, "Untitled"))
      continue;

    IupConfigSetVariableStrId(projectConfig, "ProjectOpenFiles", "File", i, filename);
    i++;
  }

  IupConfigSetVariableInt(projectConfig, "ProjectOpenFiles", "Count", i - 1);
}

static void saveProjectFiles(Ihandle *projectTree, Ihandle *projectConfig)
{
  int count = IupConfigGetVariableInt(projectConfig, "PojectFiles", "Count");
  char *filename;
  int i;

  /* clear everything before saving */
  for (i = 1; i <= count; i++)
    IupConfigSetVariableStrId(projectConfig, "ProjectFiles", "File", i, NULL);
  IupConfigSetVariableStr(projectConfig, "ProjectFiles", "Count", NULL);

  i = 1;
  filename = IupTreeGetUserId(projectTree, i);
  while (filename != NULL)
  {
    IupConfigSetVariableStrId(projectConfig, "ProjectFiles", "File", i, filename);

    i++;
    filename = IupTreeGetUserId(projectTree, i);
  }

  count = i - 1;
  IupConfigSetVariableInt(projectConfig, "ProjectFiles", "Count", count);
}

static void addFileToProjectTree(Ihandle *projectTree, const char *new_filename)
{
  Ihandle* ih = IupGetDialog(projectTree);
  Ihandle* projectConfig = iScintillaDlgGetProjectConfig(ih);
  int count = IupGetIntId(projectTree, "CHILDCOUNT", 0);
  const char *newTitle = strFileTitle(new_filename);
  int currentId = IupGetInt(projectTree, "VALUE");

  if (currentId == 0)
    IupSetStrAttributeId(projectTree, "ADDLEAF", 0, newTitle);
  else if (currentId == -1)
    IupSetStrAttributeId(projectTree, "ADDLEAF", count + 1, newTitle);
  else
    IupSetStrAttributeId(projectTree, "ADDLEAF", currentId, newTitle);

  IupTreeSetUserId(projectTree, IupGetInt(projectTree, "LASTADDNODE"), iupStrDup(new_filename));

  saveProjectFiles(projectTree, projectConfig);
}

static void new_file(Ihandle* ih_item);
static int iScintillaDlgCloseMultitext(Ihandle* multitext, int check);

static void removeFileFromProject(Ihandle *projectConfig, Ihandle *projectTree, int id)
{
  char *filename = IupTreeGetUserId(projectTree, id);
  Ihandle* tabs = IupGetDialogChild(projectTree, "TABS");
  Ihandle *multitext;
  char* m_filename;

  IupSetAttributeId(projectTree, "DELNODE", id, "SELECTED");

  for (multitext = tabs->firstchild; multitext; multitext = multitext->brother)
  {
    m_filename = IupGetAttribute(multitext, "FILENAME");

    if (!iupStrEqual(m_filename, filename))
      continue;

    iScintillaDlgCloseMultitext(multitext, 1);

    break;
  }

  if (IupGetChildCount(tabs) == 0)
    new_file(tabs);  /* always keep at least one multitext */

  IupSetAttribute(projectConfig, "MODIFIED", "YES");

  saveProjectFiles(projectTree, projectConfig);

  free(filename);
}

static void updateTitle(Ihandle* multitext, int is_dirty)
{
  Ihandle* ih = IupGetDialog(multitext);
  Ihandle* tabs = IupGetParent(multitext);
  int pos = IupGetChildPos(tabs, multitext);
  char* filename = IupGetAttribute(multitext, "FILENAME");
  char* subtitle = IupGetAttribute(ih, "SUBTITLE");
  const char* dirty_sign = "", *title;

  if (is_dirty)
    dirty_sign = "*";

  if (!filename) filename = "Untitled";

  title = strFileTitle(filename);

  IupSetfAttribute(ih, "TITLE", "%s%s - %s", title, dirty_sign, subtitle);
  IupSetfAttributeId(tabs, "TABTITLE", pos, "%s%s", title, dirty_sign);
  IupSetStrAttributeId(tabs, "TABTIP", pos, filename);
}

static int setparent_param_cb(Ihandle* param_dialog, int param_index, void* user_data)
{
  if (param_index == IUP_GETPARAM_MAP)
  {
    Ihandle* ih = (Ihandle*)user_data;
    IupSetAttributeHandle(param_dialog, "PARENTDIALOG", ih);
  }

  return 1;
}

static int renameFile(Ihandle *ih, Ihandle *config, Ihandle *multitext, const char *old_filename, const char *new_filename)
{
  IFnss cb;

  if (old_filename[0] != 0)
  {
    if (rename(old_filename, new_filename) != 0)
    {
      IupMessageError(ih, "Failed to rename file. File already exists.");
      return 0;
    }
  }

  if (multitext)
  {
    IupSetStrAttribute(multitext, "FILENAME", new_filename);

    updateTitle(multitext, IupGetInt(multitext, "MODIFIED"));

    IupSetAttribute(config, "RECENTNAME", "ScintillaRecent");
    IupConfigRecentUpdate(config, new_filename);

    if (old_filename[0] != 0)
    {
      cb = (IFnss)IupGetCallback(ih, "NEWFILENAME_CB");
      if (cb)
        cb(ih, (char*)old_filename, (char*)new_filename);
    }
  }

  return 1;
}

static void openFolder(char *filename)
{
  char folder[10240] = "\"";
  char* title, *exec;

  strcpy(folder + 1, filename);
  title = (char*)strFileTitle(folder + 1);
  title[0] = '"';
  title[1] = 0;

#ifdef WIN32
  exec = "explorer";
#else
  exec = "nautilus";
#endif

  IupExecute(exec, folder);
}

static int multitext_caret_cb(Ihandle* multitext, int lin, int col);

static int searchInFile(Ihandle* multitext, Ihandle *find_txt, int find_start, int find_end, int wrap, int down, int casesensitive, int whole_word, int regexp, int posix)
{
  char* str_to_find;
  char flags[80];
  int pos_start, pos_end;
  int found = 0;

  flags[0] = 0;
  if (casesensitive)
    strcpy(flags, "MATCHCASE");
  if (whole_word)
    strcat((flags[0] != 0 ? strcat(flags, " | ") : flags), "WHOLEWORD");
  if (regexp)
    strcat((flags[0] != 0 ? strcat(flags, " | ") : flags), "REGEXP");
  if (posix)
    strcat((flags[0] != 0 ? strcat(flags, " | ") : flags), "POSIX");

  if (flags[0] != 0)
    IupSetAttribute(multitext, "SEARCHFLAGS", flags);
  else
    IupSetAttribute(multitext, "SEARCHFLAGS", NULL);

  IupSetInt(multitext, "TARGETSTART", find_start);
  IupSetInt(multitext, "TARGETEND", find_end);

  str_to_find = IupGetAttribute(find_txt, "VALUE");
  IupSetAttribute(multitext, "SEARCHINTARGET", str_to_find);

  pos_start = IupGetInt(multitext, "TARGETSTART");
  pos_end = IupGetInt(multitext, "TARGETEND");

  if (pos_start == find_start && pos_end == find_end && wrap)
  {
    /* if not found and wrap search again in the complementary region */
    find_end = find_start;
    find_start = down ? 0 : IupGetInt(multitext, "COUNT");

    IupSetInt(multitext, "TARGETSTART", find_start);
    IupSetInt(multitext, "TARGETEND", find_end);

    str_to_find = IupGetAttribute(find_txt, "VALUE");
    IupSetAttribute(multitext, "SEARCHINTARGET", str_to_find);

    pos_start = IupGetInt(multitext, "TARGETSTART");
    pos_end = IupGetInt(multitext, "TARGETEND");
  }

  if (pos_start != find_start || pos_end != find_end)
  {
    int lin, col;

    IupSetFocus(multitext);
    IupSetfAttribute(multitext, "SELECTIONPOS", "%d:%d", pos_start, pos_end);

    /* update statusbar */
    IupTextConvertPosToLinCol(multitext, pos_end, &lin, &col);
    multitext_caret_cb(multitext, lin, col);
    found = 1;
  }

  return found;
}

/********************************** Interface Utilities *****************************************/

static Ihandle* get_project_tree(Ihandle* ih_item)
{
  Ihandle* projectTree = (Ihandle*)IupGetAttribute(ih_item, "PROJECTTREE");
  if (!projectTree)
    projectTree = IupGetDialogChild(ih_item, "PROJECTTREE");
  return projectTree;
}

static void update_dialog_title(Ihandle* multitext)
{
  Ihandle* ih = IupGetDialog(multitext);
  char* filename = IupGetAttribute(multitext, "FILENAME");
  char* subtitle = IupGetAttribute(ih, "SUBTITLE");
  char* dirty_sign = "";

  if (IupGetInt(multitext, "MODIFIED"))
    dirty_sign = "*";

  if (!filename) filename = "Untitled";

  IupSetfAttribute(ih, "TITLE", "%s%s - %s", strFileTitle(filename), dirty_sign, subtitle);
}

static int multitext_caret_cb(Ihandle* multitext, int lin, int col)
{
  Ihandle *lbl_statusbar = IupGetDialogChild(multitext, "STATUSBAR");
  IupSetfAttribute(lbl_statusbar, "TITLE", "Lin %d, Col %d", lin + 1, col + 1);  /* in Scintilla lin and col start at 0 */
  return IUP_DEFAULT;
}

static int multitext_marginclick_cb(Ihandle* multitext, int margin, int lin, char *status)
{
  (void)status;
  toggleMarker(multitext, lin, margin);
  return IUP_DEFAULT;
}

static int multitext_savepoint_cb(Ihandle* multitext, int state)
{
  if (state == 1) /* save point */
    updateTitle(multitext, 0);
  else
    updateTitle(multitext, 1);
  return IUP_DEFAULT;
}

static int multitext_updateselection_cb(Ihandle* multitext)
{
  /* Indicators 0-7 could be in use by a lexer
     so we'll use indicator 8 to highlight words. */
  const int id = 8;
  int find_start, find_end, startPos, endPos;
  IupGetIntInt(multitext, "SELECTIONPOS", &startPos, &endPos);

  IupSetfAttribute(multitext, "INDICATORCLEARRANGE", "%d:%d", 0, IupGetInt(multitext, "COUNT"));

  if (startPos == endPos)
    return IUP_IGNORE;

  IupSetInt(multitext, "INDICATORCURRENT", id);
  IupSetAttributeId(multitext, "INDICATORSTYLE", id, "STRAIGHTBOX");
  IupSetAttributeId(multitext, "INDICATORFGCOLOR", id, "0 255 0");
  IupSetIntId(multitext, "INDICATOROUTLINEALPHA", id, 50);
  IupSetIntId(multitext, "INDICATORALPHA", id, 50);

  IupSetfAttribute(multitext, "WORDRANGE", "%d:%d", startPos, endPos);

  if (IupGetInt(multitext, "ISWORD"))
  {
    char* find_txt = IupGetAttribute(multitext, "SELECTEDTEXT");

    IupSetAttribute(multitext, "SEARCHFLAGS", "WHOLEWORD");

    find_start = -1;
    find_end = IupGetInt(multitext, "COUNT") + 1;

    IupSetInt(multitext, "TARGETSTART", find_start);
    IupSetInt(multitext, "TARGETEND", find_end);

    IupSetAttribute(multitext, "SEARCHINTARGET", find_txt);

    startPos = IupGetInt(multitext, "TARGETSTART");
    endPos = IupGetInt(multitext, "TARGETEND");

    while (find_start != startPos && find_end != endPos)
    {
      IupSetfAttribute(multitext, "INDICATORFILLRANGE", "%d:%d", startPos, endPos - startPos);

      find_start = endPos;
      find_end = IupGetInt(multitext, "COUNT");
      IupSetInt(multitext, "TARGETSTART", find_start);
      IupSetInt(multitext, "TARGETEND", find_end + 1);

      IupSetAttribute(multitext, "SEARCHINTARGET", find_txt);

      startPos = IupGetInt(multitext, "TARGETSTART");
      endPos = IupGetInt(multitext, "TARGETEND");
    }
  }

  return IUP_DEFAULT;
}

static void iScintillaDlgSetConfigMultitext(Ihandle* ih, Ihandle* config, Ihandle* multitext);
static int item_paste_action_cb(Ihandle* item_paste);
static int item_case_action_cb(Ihandle* ih_item);
static int dropfiles_cb(Ihandle* multitext, const char* filename, int num, int x, int y);
static int item_windowN_action_cb(Ihandle* ih_item);

static void multitext_add_menuitem(Ihandle* item_window1)
{
  Ihandle* menu = IupGetParent(item_window1);
  int count = IupGetChildCount(menu);
  Ihandle* item_windowN = IupItem("N", NULL);
  IupSetCallback(item_windowN, "ACTION", (Icallback)item_windowN_action_cb);
  IupInsert(menu, IupGetChild(menu, count - 2), item_windowN); /* before the separator */
  if (item_window1->handle)
    IupMap(item_windowN);
}

static Ihandle* iScintillaDlgNewMultitext(Ihandle* ih_item)
{
  Ihandle* ih = IupGetDialog(ih_item);
  Ihandle* tabs = IupGetDialogChild(ih, "TABS");
  Ihandle* item_window1 = IupGetDialogChild(ih, "ITEM_WINDOW1");
  Ihandle* config = iScintillaDlgGetConfig(ih);
  int count;
  IFnn cb;

  Ihandle* multitext = IupScintilla();
  IupSetAttribute(multitext, "EXPAND", "YES");
  IupSetAttribute(multitext, "VISIBLELINES", "5");
  IupSetAttribute(multitext, "SAVEPOINT", NULL);
  IupSetCallback(multitext, "CARET_CB", (Icallback)multitext_caret_cb);
  IupSetCallback(multitext, "SAVEPOINT_CB", (Icallback)multitext_savepoint_cb);
  IupSetCallback(multitext, "DROPFILES_CB", (Icallback)dropfiles_cb);
  IupSetCallback(multitext, "MARGINCLICK_CB", (Icallback)multitext_marginclick_cb);
  IupSetCallback(multitext, "UPDATESELECTION_CB", (Icallback)multitext_updateselection_cb);

  /* highlight color for BRACEHIGHLIGHT */
  IupSetAttribute(multitext, "STYLEFGCOLOR34", "255 0 0");
  /* change the default for visibility of word warps */
  IupSetAttribute(multitext, "WORDWRAPVISUALFLAGS", "MARGIN");

  /* line numbers margin=0 */
  IupSetAttribute(multitext, "MARGINWIDTH0", LINENUMBER_MARGIN);
  IupSetAttribute(multitext, "MARGINSENSITIVE0", "YES");

  /* bookmarks margin=1 */
  IupSetAttribute(multitext, "MARGINWIDTH1", BOOKMARK_MARGIN);
  IupSetAttribute(multitext, "MARGINTYPE1", "SYMBOL");
  IupSetAttribute(multitext, "MARGINSENSITIVE1", "YES");
  IupSetAttribute(multitext, "MARGINMASKFOLDERS1", "NO"); /* (disable folding) */
  IupSetAttributeId(multitext, "MARGINMASK", 1, "1");  /* 0001 - marker=0 */

  /* bookmarks marker=0 */
  IupSetAttributeId(multitext, "MARKERFGCOLOR", 0, "0 0 255");
  IupSetAttributeId(multitext, "MARKERBGCOLOR", 0, "0 0 255");
  IupSetAttributeId(multitext, "MARKERALPHA", 0, "80");
  IupSetAttributeId(multitext, "MARKERSYMBOL", 0, "BOOKMARK");

  /* Ctrl+C, Ctrl+X, Ctrl+A, Del, already implemented inside IupScintilla */
  IupSetCallback(multitext, "K_cV", (Icallback)item_paste_action_cb);  /* replace system processing */
  IupSetCallback(multitext, "K_cU", (Icallback)item_case_action_cb);

  IupAppend(tabs, multitext);

  cb = (IFnn)IupGetCallback(ih, "NEWTEXT_CB");
  if (cb) cb(ih, multitext);

  if (tabs->handle)
  {
    IupMap(multitext);

    if (tabs->firstchild == multitext)
      IupRefresh(tabs);
    else
      IupRefreshChildren(tabs);
  }

  iScintillaDlgSetConfigMultitext(ih, config, multitext);

  IupSetAttribute(tabs, "VALUE_HANDLE", (char*)multitext);

  IupSetStrAttributeId(multitext, "STYLEBGCOLOR", 33, IupGetGlobal("DLGBGCOLOR"));  /* line number style color, reset by IupFlatTabs default BGCOLOR */

  count = IupGetChildCount(tabs);
  if (count > 1 && count <= WINDOWMENU_MAX)
    multitext_add_menuitem(item_window1);

  return multitext;
}

static int save_check(Ihandle* multitext);

static int iScintillaDlgCloseMultitext(Ihandle* multitext, int check)
{
  Ihandle* tabs = IupGetParent(multitext);
  Ihandle* item_window1 = IupGetDialogChild(tabs, "ITEM_WINDOW1");
  IFnn cb;
  int count;

  if (check)
  {
    Ihandle* projectConfig = iScintillaDlgGetProjectConfig(multitext);
    Ihandle* config = iScintillaDlgGetConfig(multitext);

    if (projectConfig)
      saveMarkers(projectConfig, multitext);
    else
      saveMarkers(config, multitext);

    if (!save_check(multitext))
      return 0;
  }

  count = IupGetChildCount(tabs);
  if (count > 1 && count <= WINDOWMENU_MAX)
  {
    /* remove the second menu item (anyone but the first) */
    Ihandle* item_windowN = IupGetBrother(item_window1);
    IupDestroy(item_windowN);
  }

  cb = (IFnn)IupGetCallback(IupGetDialog(tabs), "CLOSETEXT_CB");
  if (cb) cb(IupGetDialog(tabs), multitext);

  IupDestroy(multitext);
  return 1;
}

static Ihandle* iScintillaDlgGetCurrentMultitext(Ihandle* ih)
{
  Ihandle* tabs = IupGetDialogChild(ih, "TABS");
  if (!tabs) return (Ihandle*)iupAttribGetInherit(ih, "MULTITEXT"); /* from the context menu */
  return (Ihandle*)IupGetAttribute(tabs, "VALUE_HANDLE");
}

static void new_file(Ihandle* ih_item)
{
  Ihandle* multitext = iScintillaDlgNewMultitext(ih_item);

  IupSetAttribute(multitext, "FILENAME", NULL);
  IupSetAttribute(multitext, "VALUE", "");
  IupSetAttribute(multitext, "SAVEPOINT", NULL); /* this will update title */
  IupSetAttribute(multitext, "UNDO", NULL); /* clear undo */
}

static void open_file(Ihandle* ih_item, const char* filename, int check_empty)
{
  char* str = readFile(filename);
  if (str)
  {
    Ihandle* ih = IupGetDialog(ih_item);
    Ihandle* tabs = IupGetDialogChild(ih_item, "TABS");
    Ihandle* multitext = iScintillaDlgNewMultitext(ih_item);
    Ihandle* config = iScintillaDlgGetConfig(ih_item);
    Ihandle* projectConfig = iScintillaDlgGetProjectConfig(ih);

    IupSetAttribute(config, "RECENTNAME", "ScintillaRecent");
    IupConfigRecentUpdate(config, filename);

    IupSetStrAttribute(multitext, "FILENAME", filename);
    IupSetStrAttribute(multitext, "VALUE", str);
    IupSetAttribute(multitext, "SAVEPOINT", NULL); /* this will update title */
    IupSetAttribute(multitext, "UNDO", NULL); /* clear undo */

    if (projectConfig)
      restoreMarkers(projectConfig, multitext);
    else
      restoreMarkers(config, multitext);

    free(str);

    if (check_empty && IupGetChildCount(tabs) == 2)
    {
      multitext = IupGetChild(tabs, 0);
      filename = IupGetAttribute(multitext, "FILENAME");
      if (!filename && IupGetInt(multitext, "COUNT") == 0) /* an empty non saved single file is replaced by the open file */
        iScintillaDlgCloseMultitext(multitext, 0);
    }
  }
  else
    IupMessageError(IupGetDialog(ih_item), "IUP_ERRORFILEOPEN");
}

static void loadProjectFiles(Ihandle *projectConfig, Ihandle *projectTree);
static int item_close_proj_action_cb(Ihandle* ih_item);

static void open_proj(Ihandle *ih, const char *filename)
{
  IFnn cb = (IFnn)IupGetCallback(ih, "CONFIGLOAD_CB");
  Ihandle* projectTree = IupGetDialogChild(ih, "PROJECTTREE");
  Ihandle* projectSplit = IupGetDialogChild(ih, "PROJECTSPLIT");
  Ihandle* config = iScintillaDlgGetConfig(ih);
  Ihandle* projectConfig = iScintillaDlgGetProjectConfig(ih);

  if (projectConfig)
  {
    if (item_close_proj_action_cb(ih) == IUP_IGNORE)
      return;
  }

  IupSetAttribute(config, "RECENTNAME", "ScintillaRecentProject");
  IupConfigRecentUpdate(config, filename);

  projectConfig = IupConfig();
  iupAttribSet(ih, "_IUP_PROJECT_CONFIG", (char*)projectConfig);

  IupSetStrAttribute(projectConfig, "APP_FILENAME", filename);
  IupSetAttribute(projectTree, "TITLE0", strFileTitle(filename));

  IupConfigLoad(projectConfig);

  if (cb)
    cb(ih, projectConfig);

  loadProjectFiles(projectConfig, projectTree);

  if (IupGetInt(projectSplit, "VALUE") == 0)
  {
    IupSetAttribute(projectSplit, "VALUE", "200");
    IupRefresh(ih);
  }
}

static void revert_file(Ihandle* multitext)
{
  IupSetAttribute(multitext, "UNDO", "ALL");
}

static int item_saveas_action_cb(Ihandle* ih_item);

static void save_file(Ihandle* multitext)
{
  char* filename = IupGetAttribute(multitext, "FILENAME");
  if (!filename)
    item_saveas_action_cb(multitext);
  else
  {
    char* str = IupGetAttribute(multitext, "VALUE");
    int count = IupGetInt(multitext, "COUNT");
    if (writeFile(filename, str, count))
    {
      IupSetAttribute(multitext, "SAVEPOINT", NULL); /* this will update title */
      IupSetAttribute(multitext, "UNDO", NULL); /* clear undo */
    }
    else
      IupMessageError(IupGetDialog(multitext), "IUP_ERRORFILESAVE");
  }
}

static void saveas_file(Ihandle* multitext, const char* filename)
{
  char* str = IupGetAttribute(multitext, "VALUE");
  int count = IupGetInt(multitext, "COUNT");
  if (writeFile(filename, str, count))
  {
    Ihandle* config = iScintillaDlgGetConfig(multitext);
    char* old_filename = iupStrDup(IupGetAttribute(multitext, "FILENAME"));
    IFnss cb;

    IupSetAttribute(config, "RECENTNAME", "ScintillaRecent");
    IupConfigRecentUpdate(config, filename);

    IupSetStrAttribute(multitext, "FILENAME", filename);

    IupSetAttribute(multitext, "SAVEPOINT", NULL); /* this will update title */
    IupSetAttribute(multitext, "UNDO", NULL); /* clear undo */

    cb = (IFnss)IupGetCallback(IupGetDialog(multitext), "NEWFILENAME_CB");
    if (cb)
      cb(IupGetDialog(multitext), old_filename, (char*)filename);

    free(old_filename);
  }
  else
    IupMessageError(IupGetDialog(multitext), "IUP_ERRORFILESAVE");
}

static void savecopy_file(Ihandle* multitext, const char* filename)
{
  char* str = IupGetAttribute(multitext, "VALUE");
  int count = IupGetInt(multitext, "COUNT");

  if (writeFile(filename, str, count))
  {
    Ihandle* config = iScintillaDlgGetConfig(multitext);
    IupSetAttribute(config, "RECENTNAME", "ScintillaRecent");
    IupConfigRecentUpdate(config, filename);
  }
  else
    IupMessageError(IupGetDialog(multitext), "IUP_ERRORFILESAVE");
}

static int save_check(Ihandle* multitext)
{
  if (IupGetInt(multitext, "MODIFIED"))
  {
    switch (IupMessageAlarm(IupGetDialog(multitext), "Attention!", "File not saved. Save it now?", "YESNOCANCEL"))
    {
      case 1:  /* save the changes and continue */
        save_file(multitext);
        break;
      case 2:  /* ignore the changes and continue */
        break;
      case 3:  /* cancel */
        return 0;
    }
  }
  return 1;
}

static void saveProject(Ihandle *ih, Ihandle *projectConfig, int show_dialog);

static int save_project_check(Ihandle* ih)
{
  Ihandle* projectConfig = iScintillaDlgGetProjectConfig(ih);

  if (IupGetInt(projectConfig, "MODIFIED")) /* modifications in the file list only */
  {
    switch (IupMessageAlarm(ih, "Attention!", "Project not saved. Save it now?", "YESNOCANCEL"))
    {
      case 1:  /* save the changes and continue */
        saveProject(ih, projectConfig, 0);
        break;
      case 2:  /* ignore the changes and continue */
        break;
      case 3:  /* cancel */
        return 0;
    }
  }
  else /* project is automatically saved when not modified because of the other changes */
  {
    char *filename = IupGetAttribute(projectConfig, "APP_FILENAME");
    if (filename)
    {
      IFnn cb = (IFnn)IupGetCallback(ih, "CONFIGSAVE_CB");
      if (cb)
        cb(ih, projectConfig);
      saveAllMarkers(ih, projectConfig);
      saveProjectOpenFiles(ih, projectConfig);
      IupConfigSave(projectConfig);
    }
  }

  return 1;
}

static void toggle_bar_visibility(Ihandle* ih_item, Ihandle* bar)
{
  if (IupGetInt(ih_item, "VALUE"))
  {
    IupSetAttribute(bar, "FLOATING", "YES");
    IupSetAttribute(bar, "VISIBLE", "NO");
    IupSetAttribute(ih_item, "VALUE", "OFF");
  }
  else
  {
    IupSetAttribute(bar, "FLOATING", "NO");
    IupSetAttribute(bar, "VISIBLE", "YES");
    IupSetAttribute(ih_item, "VALUE", "ON");
  }

  IupRefresh(bar);  /* refresh the dialog layout */
}

static void set_find_replace_visibility(Ihandle* find_dlg, int show_replace)
{
  Ihandle* replace_txt = IupGetDialogChild(find_dlg, "REPLACE_TEXT");
  Ihandle* replace_lbl = IupGetDialogChild(find_dlg, "REPLACE_LABEL");
  Ihandle* replace_bt = IupGetDialogChild(find_dlg, "REPLACE_BUTTON");
  Ihandle* replace_all_bt = IupGetDialogChild(find_dlg, "REPLACE_ALL_BUTTON");

  if (show_replace)
  {
    IupSetAttribute(replace_txt, "VISIBLE", "Yes");
    IupSetAttribute(replace_lbl, "VISIBLE", "Yes");
    IupSetAttribute(replace_bt, "VISIBLE", "Yes");
    IupSetAttribute(replace_all_bt, "VISIBLE", "Yes");
    IupSetAttribute(replace_txt, "FLOATING", "No");
    IupSetAttribute(replace_lbl, "FLOATING", "No");
    IupSetAttribute(replace_bt, "FLOATING", "No");
    IupSetAttribute(replace_all_bt, "FLOATING", "No");

    IupSetAttribute(find_dlg, "TITLE", "Replace");
  }
  else
  {
    IupSetAttribute(replace_txt, "FLOATING", "Yes");
    IupSetAttribute(replace_lbl, "FLOATING", "Yes");
    IupSetAttribute(replace_bt, "FLOATING", "Yes");
    IupSetAttribute(replace_all_bt, "FLOATING", "Yes");
    IupSetAttribute(replace_txt, "VISIBLE", "No");
    IupSetAttribute(replace_lbl, "VISIBLE", "No");
    IupSetAttribute(replace_bt, "VISIBLE", "No");
    IupSetAttribute(replace_all_bt, "VISIBLE", "No");

    IupSetAttribute(find_dlg, "TITLE", "Find");
  }

  IupSetAttribute(find_dlg, "SIZE", NULL);  /* force a dialog resize on the IupRefresh */
  IupRefresh(find_dlg);
}


/********************************** Callbacks *****************************************/

static int check_inproject(Ihandle* projectTree, const char* check_filename)
{
  int i;
  int count = IupGetInt(projectTree, "CHILDCOUNT0");

  for (i = 1; i <= count; i++)
  {
    char* filename = IupTreeGetUserId(projectTree, i);
    if (iupStrEqual(filename, check_filename))
    {
      IupMessageError(IupGetDialog(projectTree), "File already in project!");
      return 1; /* exists, abort */
    }
  }

  return 0; /* does NOT exists, continue */
}

static Ihandle* check_open(Ihandle* ih, const char* check_filename, int save)
{
  Ihandle* tabs = IupGetDialogChild(ih, "TABS");
  Ihandle* multitext;

  for (multitext = tabs->firstchild; multitext; multitext = multitext->brother)
  {
    char* filename = IupGetAttribute(multitext, "FILENAME");
    if (filename && iupStrEqual(filename, check_filename))
    {
      if (save)
        IupMessageError(IupGetDialog(tabs), "File already opened. Can not save using this filename.");
      else if (IupGetInt(multitext, "MODIFIED") && IupMessageAlarm(IupGetDialog(tabs), "Attention!", "File already opened, but modified. Do you want to revert it?", "YESNO") == 1)
        revert_file(multitext);

      return multitext; /* exists, abort */
    }
  }

  return NULL; /* does NOT exists, continue */
}

static int dropfiles_cb(Ihandle* ih, const char* filename, int num, int x, int y)
{
  static int last = 1;
  static int remove_empty = 0;

  (void)x;
  (void)y;

  if (last)
  {
    Ihandle* tabs = IupGetDialogChild(ih, "TABS");
    if (IupGetChildCount(tabs) == 1)
    {
      Ihandle* multitext = IupGetChild(tabs, 0);
      char* m_filename = IupGetAttribute(multitext, "FILENAME");
      if (!m_filename && IupGetInt(multitext, "COUNT") == 0) /* an empty non saved single file is replaced by the open file */
        remove_empty = 1;
    }

    last = 0;
  }

  if (!check_open(ih, filename, 0))
    open_file(ih, filename, 0);

  if (num == 0)
  {
    if (remove_empty)
    {
      /* do it only after all files are dropped, to avoid destruction of the dropped target during callback calls */
      Ihandle* tabs = IupGetDialogChild(ih, "TABS");
      Ihandle* multitext = IupGetChild(tabs, 0);
      iScintillaDlgCloseMultitext(multitext, 0);
      remove_empty = 0;
    }

    last = 1;
  }

  return IUP_DEFAULT;
}

static int file_menu_open_cb(Ihandle* ih_menu)
{
  Ihandle* item_revert = IupGetDialogChild(ih_menu, "ITEM_REVERT");
  Ihandle* item_save = IupGetDialogChild(ih_menu, "ITEM_SAVE");
  Ihandle* multitext = iScintillaDlgGetCurrentMultitext(ih_menu);
  char* filename = IupGetAttribute(multitext, "FILENAME");
  int dirty = IupGetInt(multitext, "MODIFIED");

  if (dirty)
    IupSetAttribute(item_save, "ACTIVE", "YES");
  else
    IupSetAttribute(item_save, "ACTIVE", "NO");

  if (dirty && filename)
    IupSetAttribute(item_revert, "ACTIVE", "YES");
  else
    IupSetAttribute(item_revert, "ACTIVE", "NO");
  return IUP_DEFAULT;
}

static int edit_menu_open_cb(Ihandle* ih_menu)
{
  Ihandle* find_dlg = (Ihandle*)IupGetAttribute(ih_menu, "FIND_DIALOG");

  Ihandle *item_undo = IupGetDialogChild(ih_menu, "ITEM_UNDO");
  Ihandle *item_redo = IupGetDialogChild(ih_menu, "ITEM_REDO");
  Ihandle *item_paste = IupGetDialogChild(ih_menu, "ITEM_PASTE");
  Ihandle *item_cut = IupGetDialogChild(ih_menu, "ITEM_CUT");
  Ihandle *item_delete = IupGetDialogChild(ih_menu, "ITEM_DELETE");
  Ihandle *item_copy = IupGetDialogChild(ih_menu, "ITEM_COPY");
  Ihandle *item_find_next = IupGetDialogChild(ih_menu, "ITEM_FINDNEXT");
  Ihandle* multitext = iScintillaDlgGetCurrentMultitext(ih_menu);

  if (IupGetInt(multitext, "UNDO"))
    IupSetAttribute(item_undo, "ACTIVE", "YES");
  else
    IupSetAttribute(item_undo, "ACTIVE", "NO");

  if (IupGetInt(multitext, "REDO"))
    IupSetAttribute(item_redo, "ACTIVE", "YES");
  else
    IupSetAttribute(item_redo, "ACTIVE", "NO");

  if (IupGetInt(multitext, "CLIPBOARD"))
    IupSetAttribute(item_paste, "ACTIVE", "YES");
  else
    IupSetAttribute(item_paste, "ACTIVE", "NO");

  if (IupGetAttribute(multitext, "SELECTEDTEXT"))
  {
    IupSetAttribute(item_cut, "ACTIVE", "YES");
    IupSetAttribute(item_delete, "ACTIVE", "YES");
    IupSetAttribute(item_copy, "ACTIVE", "YES");
  }
  else
  {
    IupSetAttribute(item_cut, "ACTIVE", "NO");
    IupSetAttribute(item_delete, "ACTIVE", "NO");
    IupSetAttribute(item_copy, "ACTIVE", "NO");
  }

  if (find_dlg)
  {
    Ihandle* txt = IupGetDialogChild(find_dlg, "FIND_TEXT");
    char* str_to_find = IupGetAttribute(txt, "VALUE");

    if (!str_to_find || str_to_find[0] == 0)
      IupSetAttribute(item_find_next, "ACTIVE", "NO");
    else
      IupSetAttribute(item_find_next, "ACTIVE", "Yes");
  }
  else
    IupSetAttribute(item_find_next, "ACTIVE", "NO");

  return IUP_DEFAULT;
}

static int window_menu_open_cb(Ihandle* ih_menu)
{
  Ihandle* tabs = IupGetDialogChild(ih_menu, "TABS");
  Ihandle* item_window1 = IupGetDialogChild(tabs, "ITEM_WINDOW1");
  Ihandle* current_multitext = (Ihandle*)IupGetAttribute(tabs, "VALUE_HANDLE");
  Ihandle* m_multitext, *item_windowN;
  int pos;

  for (m_multitext = tabs->firstchild, pos = 0, item_windowN = item_window1; m_multitext && pos < WINDOWMENU_MAX && item_windowN; m_multitext = m_multitext->brother, pos++, item_windowN = item_windowN->brother)
  {
    char* filename = IupGetAttribute(m_multitext, "FILENAME");
    if (!filename) filename = "Untitled";

    IupSetfAttribute(item_windowN, "TITLE", "&%d %s", pos + 1, strFileTitle(filename));

    if (m_multitext == current_multitext)
      IupSetAttribute(item_windowN, "VALUE", "ON");
    else
      IupSetAttribute(item_windowN, "VALUE", "OFF");
  }

  return IUP_DEFAULT;
}

static int project_menu_open_cb(Ihandle* ih_menu)
{
  Ihandle* ih = IupGetDialog(ih_menu);
  Ihandle* item_save_proj = IupGetDialogChild(ih_menu, "ITEM_SAVE_PROJ");
  Ihandle* item_save_proj_as = IupGetDialogChild(ih_menu, "ITEM_SAVE_PROJ_AS");
  Ihandle* item_close_proj = IupGetDialogChild(ih_menu, "ITEM_CLOSE_PROJ");
  Ihandle* item_add_new_proj = IupGetDialogChild(ih_menu, "ITEM_ADD_NEW_FILE");
  Ihandle* item_add_exist_proj = IupGetDialogChild(ih_menu, "ITEM_ADD_EXIST_FILE");
  Ihandle* item_add_open_proj = IupGetDialogChild(ih_menu, "ITEM_ADD_OPEN_FILE");
  Ihandle* item_open_proj_file = IupGetDialogChild(ih_menu, "ITEM_OPEN_PROJ_FILE");
  Ihandle* item_open_all_proj_file = IupGetDialogChild(ih_menu, "ITEM_OPEN_ALL_PROJ_FILE");
  Ihandle* item_remove_proj_file = IupGetDialogChild(ih_menu, "ITEM_REMOVE_PROJ_FILE");
  Ihandle* projectConfig = iScintillaDlgGetProjectConfig(ih);

  if (projectConfig)
  {
    IupSetAttribute(item_close_proj, "ACTIVE", "YES");
    IupSetAttribute(item_add_new_proj, "ACTIVE", "YES");
    IupSetAttribute(item_add_exist_proj, "ACTIVE", "YES");
    IupSetAttribute(item_add_open_proj, "ACTIVE", "YES");
    IupSetAttribute(item_open_proj_file, "ACTIVE", "YES");
    IupSetAttribute(item_open_all_proj_file, "ACTIVE", "YES");
    IupSetAttribute(item_remove_proj_file, "ACTIVE", "YES");
    IupSetAttribute(item_save_proj_as, "ACTIVE", "YES");
    if (IupGetInt(projectConfig, "MODIFIED"))
      IupSetAttribute(item_save_proj, "ACTIVE", "YES");
    else
      IupSetAttribute(item_save_proj, "ACTIVE", "NO");
  }
  else
  {
    IupSetAttribute(item_close_proj, "ACTIVE", "NO");
    IupSetAttribute(item_add_new_proj, "ACTIVE", "NO");
    IupSetAttribute(item_save_proj, "ACTIVE", "NO");
    IupSetAttribute(item_save_proj_as, "ACTIVE", "NO");
    IupSetAttribute(item_add_exist_proj, "ACTIVE", "NO");
    IupSetAttribute(item_add_open_proj, "ACTIVE", "NO");
    IupSetAttribute(item_open_proj_file, "ACTIVE", "NO");
    IupSetAttribute(item_open_all_proj_file, "ACTIVE", "NO");
    IupSetAttribute(item_remove_proj_file, "ACTIVE", "NO");
  }

  return IUP_DEFAULT;
}

static int view_menu_open_cb(Ihandle *ih_menu)
{
  Ihandle* item_panel = IupGetDialogChild(ih_menu, "ITM_PANEL");
  Ihandle* panelSplit = IupGetDialogChild(ih_menu, "PANEL_SPLIT");

  if (IupGetInt(panelSplit, "VALUE") == 1000)
    IupSetAttribute(item_panel, "VALUE", "OFF");
  else
    IupSetAttribute(item_panel, "VALUE", "ON");

  return IUP_DEFAULT;
}

static int config_recent_cb(Ihandle* ih_item)
{
  char* filename = IupGetAttribute(ih_item, "TITLE");
  if (!check_open(ih_item, filename, 0))
    open_file(ih_item, filename, 1);
  return IUP_DEFAULT;
}

static int config_recent_proj_cb(Ihandle* ih_item)
{
  Ihandle* ih = IupGetDialog(ih_item);
  char* filename = IupGetAttribute(ih_item, "TITLE");
  open_proj(ih, filename);
  return IUP_DEFAULT;
}

static int item_new_action_cb(Ihandle* ih_item)
{
  new_file(ih_item);
  return IUP_IGNORE;  /* to avoid garbage in Scintilla when pressing the hot key */
}

static int item_open_action_cb(Ihandle* ih_item)
{
  Ihandle* ih = IupGetDialog(ih_item);
  Ihandle *filedlg;
  Ihandle* config;
  const char* dir = NULL;
  char* extra_filters = IupGetAttribute(ih, "EXTRAFILTERS");

  config = iScintillaDlgGetConfig(ih_item);
  dir = IupConfigGetVariableStr(config, IupGetAttribute(ih, "SUBTITLE"), "LastDirectory");

  filedlg = IupFileDlg();
  IupSetAttribute(filedlg, "DIALOGTYPE", "OPEN");
  if (extra_filters)
    IupSetStrf(filedlg, "EXTFILTER", "%sText Files|*.txt|All Files|*.*|", extra_filters);
  else
    IupSetAttribute(filedlg, "EXTFILTER", "Text Files|*.txt|All Files|*.*|");
  IupSetAttributeHandle(filedlg, "PARENTDIALOG", ih);
  IupSetStrAttribute(filedlg, "DIRECTORY", dir);

  IupPopup(filedlg, IUP_CENTERPARENT, IUP_CENTERPARENT);

  if (IupGetInt(filedlg, "STATUS") != -1)
  {
    char* filename = IupGetAttribute(filedlg, "VALUE");
    if (!check_open(ih, filename, 0))
      open_file(ih_item, filename, 1);

    dir = IupGetAttribute(filedlg, "DIRECTORY");
    IupConfigSetVariableStr(config, IupGetAttribute(ih, "SUBTITLE"), "LastDirectory", dir);
  }

  IupDestroy(filedlg);
  return IUP_DEFAULT;
}

static int item_saveas_action_cb(Ihandle* ih_item)
{
  Ihandle* ih = IupGetDialog(ih_item);
  const char* dir;
  Ihandle* multitext = iScintillaDlgGetCurrentMultitext(ih_item);
  Ihandle* config = iScintillaDlgGetConfig(multitext);
  Ihandle *filedlg = IupFileDlg();
  char* extra_filters = IupGetAttribute(ih, "EXTRAFILTERS");
  char* old_filename = IupGetAttribute(multitext, "FILENAME");

  dir = IupConfigGetVariableStr(config, IupGetAttribute(ih, "SUBTITLE"), "LastDirectory");

  IupSetAttribute(filedlg, "DIALOGTYPE", "SAVE");
  if (extra_filters)
    IupSetStrf(filedlg, "EXTFILTER", "%sText Files|*.txt|All Files|*.*|", extra_filters);
  else
    IupSetAttribute(filedlg, "EXTFILTER", "Text Files|*.txt|All Files|*.*|");
  IupSetAttributeHandle(filedlg, "PARENTDIALOG", ih);
  IupSetStrAttribute(filedlg, "FILE", old_filename);
  IupSetStrAttribute(filedlg, "DIRECTORY", dir);

  IupPopup(filedlg, IUP_CENTERPARENT, IUP_CENTERPARENT);

  if (IupGetInt(filedlg, "STATUS") != -1)
  {
    char* filename = IupGetAttribute(filedlg, "VALUE");
    if (iupStrEqual(old_filename, filename))
      save_file(multitext);
    else if (!check_open(ih, filename, 1))
      saveas_file(multitext, filename);

    dir = IupGetAttribute(filedlg, "DIRECTORY");
    IupConfigSetVariableStr(config, IupGetAttribute(ih, "SUBTITLE"), "LastDirectory", dir);
  }

  IupDestroy(filedlg);
  return IUP_DEFAULT;
}

static int item_savecopy_action_cb(Ihandle* ih_item)
{
  Ihandle* ih = IupGetDialog(ih_item);
  const char* dir;
  Ihandle* multitext = iScintillaDlgGetCurrentMultitext(ih_item);
  Ihandle* config = iScintillaDlgGetConfig(multitext);
  Ihandle *filedlg = IupFileDlg();
  char* extra_filters = IupGetAttribute(ih, "EXTRAFILTERS");

  dir = IupConfigGetVariableStr(config, IupGetAttribute(ih, "SUBTITLE"), "LastDirectory");

  IupSetAttribute(filedlg, "DIALOGTYPE", "SAVE");
  if (extra_filters)
    IupSetStrf(filedlg, "EXTFILTER", "%sText Files|*.txt|All Files|*.*|", extra_filters);
  else
    IupSetAttribute(filedlg, "EXTFILTER", "Text Files|*.txt|All Files|*.*|");
  IupSetAttributeHandle(filedlg, "PARENTDIALOG", ih);
  IupSetStrAttribute(filedlg, "FILE", IupGetAttribute(multitext, "FILENAME"));
  IupSetStrAttribute(filedlg, "DIRECTORY", dir);

  IupPopup(filedlg, IUP_CENTERPARENT, IUP_CENTERPARENT);

  if (IupGetInt(filedlg, "STATUS") != -1)
  {
    char* filename = IupGetAttribute(filedlg, "VALUE");
    savecopy_file(multitext, filename);

    dir = IupGetAttribute(filedlg, "DIRECTORY");
    IupConfigSetVariableStr(config, IupGetAttribute(ih, "SUBTITLE"), "LastDirectory", dir);
  }

  IupDestroy(filedlg);
  return IUP_DEFAULT;
}

static int item_saveall_action_cb(Ihandle* ih_item)
{
  Ihandle* tabs = IupGetDialogChild(ih_item, "TABS");
  Ihandle *projectConfig = iScintillaDlgGetProjectConfig(ih_item);
  Ihandle* multitext;

  for (multitext = tabs->firstchild; multitext; multitext = multitext->brother)
  {
    if (IupGetInt(multitext, "MODIFIED"))
      save_file(multitext);
  }

  if (projectConfig)
    saveProject(ih_item, projectConfig, 0);

  return IUP_DEFAULT;
}

static int item_save_action_cb(Ihandle* ih_item)
{
  if (IupGetInt(NULL, "SHIFTKEY"))
    item_saveall_action_cb(ih_item);
  else
  {
    Ihandle* multitext = iScintillaDlgGetCurrentMultitext(ih_item);  /* ih_item can be from the context menu */
    /* test again because in can be called using the hot key */
    if (IupGetInt(multitext, "MODIFIED"))
      save_file(multitext);
  }
  return IUP_IGNORE;  /* to avoid garbage in Scintilla when pressing the hot key */
}

static int item_closeall_action_cb(Ihandle* ih_item)
{
  Ihandle* multitext, *tmp;
  Ihandle* tabs = IupGetDialogChild(ih_item, "TABS");
  if (!tabs) tabs = (Ihandle*)iupAttribGetInherit(ih_item, "TABS");  /* from the context menu */

  for (multitext = tabs->firstchild; multitext; /* increment before destroy */)
  {
    /* increment before destroy */
    tmp = multitext;
    multitext = multitext->brother;

    if (!iScintillaDlgCloseMultitext(tmp, 1))
      return IUP_DEFAULT;
  }

  new_file(tabs);  /* always keep at least one multitext */

  return IUP_DEFAULT;
}

static int item_close_action_cb(Ihandle* ih_item)
{
  if (IupGetInt(NULL, "SHIFTKEY"))
    item_closeall_action_cb(ih_item);
  else
  {
    Ihandle* multitext = iScintillaDlgGetCurrentMultitext(ih_item); /* ih_item can be from the context menu */
    Ihandle* tabs = IupGetParent(multitext);

    if (!iScintillaDlgCloseMultitext(multitext, 1))
      return IUP_DEFAULT;

    if (IupGetChildCount(tabs) == 0)
      new_file(ih_item);  /* always keep at least one multitext */
  }

  return IUP_DEFAULT;
}

static int item_closeall_butthis_action_cb(Ihandle* ih_item)
{
  Ihandle* tabs = (Ihandle*)iupAttribGetInherit(ih_item, "TABS");  /* from the context menu */
  Ihandle* this_multitext = (Ihandle*)iupAttribGetInherit(ih_item, "MULTITEXT");  /* from the context menu */
  Ihandle* multitext, *tmp;

  for (multitext = tabs->firstchild; multitext; /* increment before destroy */)
  {
    if (this_multitext != multitext)
    {
      /* increment before destroy */
      tmp = multitext;
      multitext = multitext->brother;

      if (!iScintillaDlgCloseMultitext(tmp, 1))
        return IUP_DEFAULT;
    }
    else
      multitext = multitext->brother;
  }

  return IUP_DEFAULT;
}

static int item_rename_action_cb(Ihandle* ih_item)
{
  Ihandle* config = iScintillaDlgGetConfig(ih_item);
  Ihandle* multitext = iScintillaDlgGetCurrentMultitext(ih_item);
  Ihandle* ih = IupGetDialog(ih_item);

  char* old_filename = iupStrDup(IupGetAttribute(multitext, "FILENAME"));
  char new_name[512], new_filename[10240];

  if (!old_filename) old_filename = iupStrDup("");
  strcpy(new_name, strFileTitle(old_filename));

  if (!IupGetParam("Rename", setparent_param_cb, ih, "Name: %s\n", new_name, NULL))
  {
    free(old_filename);
    return IUP_DEFAULT;
  }

  strcpy(new_filename, old_filename);
  strcpy((char*)strFileTitle(new_filename), new_name);

  renameFile(ih, config, multitext, old_filename, new_filename);

  free(old_filename);
  return IUP_DEFAULT;
}

static int item_revert_action_cb(Ihandle* ih_item)
{
  Ihandle* multitext = iScintillaDlgGetCurrentMultitext(ih_item);  /* ih_item can be from the context menu */
  revert_file(multitext);
  return IUP_DEFAULT;
}

static int item_print_action_cb(Ihandle* ih_item)
{
  Ihandle* multitext = iScintillaDlgGetCurrentMultitext(ih_item);
  Ihandle* config = iScintillaDlgGetConfig(ih_item);
  const char* value;

  value = IupConfigGetVariableStr(config, "ScintillaPrint", "MarginLeft");
  if (value)
    IupSetStrAttribute(multitext, "PRINTMARGINLEFT", value);

  value = IupConfigGetVariableStr(config, "ScintillaPrint", "MarginRight");
  if (value)
    IupSetStrAttribute(multitext, "PRINTMARGINRIGHT", value);

  value = IupConfigGetVariableStr(config, "ScintillaPrint", "MarginTop");
  if (value)
    IupSetStrAttribute(multitext, "PRINTMARGINTOP", value);

  value = IupConfigGetVariableStr(config, "ScintillaPrint", "MarginBottom");
  if (value)
    IupSetStrAttribute(multitext, "PRINTMARGINBOTTOM", value);

  value = IupConfigGetVariableStr(config, "ScintillaPrint", "MarginUnits");
  if (value)
    IupSetStrAttribute(multitext, "PRINTMARGINUNITS", value);

  value = IupConfigGetVariableStr(config, "ScintillaPrint", "WordWrap");
  if (value)
    IupSetStrAttribute(multitext, "PRINTWORDWRAP", value);

  value = IupConfigGetVariableStr(config, "ScintillaPrint", "Color");
  if (value)
    IupSetStrAttribute(multitext, "PRINTCOLOR", value);

  value = IupConfigGetVariableStr(config, "ScintillaPrint", "Magnification");
  if (value)
    IupSetStrAttribute(multitext, "PRINTMAGNIFICATION", value);

  IupSetAttribute(multitext, "PRINT", IupGetAttribute(IupGetDialog(multitext), "TITLE"));
  return IUP_DEFAULT;
}

static int getListIndex(const char* value, const char* list[], int count)
{
  int i;

  if (!value)
    return 0;

  for (i = 0; i < count; i++)
  {
    if (iupStrEqualNoCase(list[i], value))
      return i;
  }
  return 0;
}

static const char* getListValue(int index, const char* list[])
{
  return list[index];
}

static int item_pagesetup_action_cb(Ihandle* ih_item)
{
  Ihandle* config = iScintillaDlgGetConfig(ih_item);
  double margin_left, margin_top, margin_right, margin_bottom;
  int margin_units_index, word_wrap_index, color_index;
  char* margin_units_list[] = { "PIXELS", "INCH", "CM" };
  char* word_wrap_list[] = { "NONE", "CHAR", "WORD" };
  char* color_list[] = { "NORMAL", "INVERTLIGHT", "BLACKONWHITE", "COLORONWHITE" };
  const char *margin_units, *word_wrap, *color;
  int magnification;

  margin_left = IupConfigGetVariableDouble(config, "ScintillaPrint", "MarginLeft");
  margin_right = IupConfigGetVariableDouble(config, "ScintillaPrint", "MarginRight");
  margin_top = IupConfigGetVariableDouble(config, "ScintillaPrint", "MarginTop");
  margin_bottom = IupConfigGetVariableDouble(config, "ScintillaPrint", "MarginBottom");
  margin_units = IupConfigGetVariableStr(config, "ScintillaPrint", "MarginUnits");
  word_wrap = IupConfigGetVariableStr(config, "ScintillaPrint", "WordWrap");
  color = IupConfigGetVariableStr(config, "ScintillaPrint", "Color");
  magnification = IupConfigGetVariableInt(config, "ScintillaPrint", "Magnification");

  margin_units_index = getListIndex(margin_units, margin_units_list, sizeof(margin_units_list));
  word_wrap_index = getListIndex(word_wrap, word_wrap_list, sizeof(word_wrap_list));
  color_index = getListIndex(color, color_list, sizeof(color_list));

  if (IupGetParam("Page Setup", setparent_param_cb, IupGetDialog(ih_item),
    "Margin Left: %R\n"
    "Margin Right: %R\n"
    "Margin Top: %R\n"
    "Margin Bottom: %R\n"
    "Margin Units: %l|Pixels|Inch|Cm|\n"
    "Word Wrap: %l|None|Char|Word|\n"
    "Color: %l|Normal|Invert Light|Black on White|Color on White|\n"
    "Magnification: %i\n",
    &margin_left, &margin_top, &margin_right, &margin_bottom,
    &margin_units_index, &word_wrap_index, &color_index,
    &magnification,
    NULL))
  {
    margin_units = getListValue(margin_units_index, margin_units_list);
    word_wrap = getListValue(word_wrap_index, word_wrap_list);
    color = getListValue(color_index, color_list);

    IupConfigSetVariableDouble(config, "ScintillaPrint", "MarginLeft", margin_left);
    IupConfigSetVariableDouble(config, "ScintillaPrint", "MarginRight", margin_right);
    IupConfigSetVariableDouble(config, "ScintillaPrint", "MarginTop", margin_top);
    IupConfigSetVariableDouble(config, "ScintillaPrint", "MarginBottom", margin_bottom);
    IupConfigSetVariableStr(config, "ScintillaPrint", "MarginUnits", margin_units);
    IupConfigSetVariableStr(config, "ScintillaPrint", "WordWrap", word_wrap);
    IupConfigSetVariableStr(config, "ScintillaPrint", "Color", color);
    IupConfigSetVariableInt(config, "ScintillaPrint", "Magnification", magnification);
  }
  return IUP_DEFAULT;
}

static int item_openfolder_action_cb(Ihandle* ih_item)
{
  Ihandle* multitext = (Ihandle*)iupAttribGetInherit(ih_item, "MULTITEXT");  /* from the context menu */
  char* filename = IupGetAttribute(multitext, "FILENAME");

  openFolder(filename);

  return IUP_DEFAULT;
}

static int item_copyfilename_action_cb(Ihandle* ih_item)
{
  Ihandle* multitext = (Ihandle*)iupAttribGetInherit(ih_item, "MULTITEXT");  /* from the context menu */
  char* filename = IupGetAttribute(multitext, "FILENAME");
  copyFilenameToClipboard(filename);
  return IUP_DEFAULT;
}

static int item_addtoproject_action_cb(Ihandle* ih_item)
{
  Ihandle* multitext = (Ihandle*)iupAttribGetInherit(ih_item, "MULTITEXT");  /* from the context menu */
  char* filename = IupGetAttribute(multitext, "FILENAME");
  Ihandle* projectConfig = iScintillaDlgGetProjectConfig(multitext);
  Ihandle *projectTree = get_project_tree(multitext);

  if (!check_inproject(projectTree, filename))
  {
    addFileToProjectTree(projectTree, filename);
    IupSetAttribute(projectConfig, "MODIFIED", "YES");
  }

  return IUP_DEFAULT;
}

static int tabs_rightclick_cb(Ihandle* tabs, int pos)
{
  Ihandle *menu, *item_close, *item_revert, *item_save, *item_closeall, *item_closeall_butthis,
    *item_openfolder, *item_copyfilename, *item_addtoproject;
  Ihandle* multitext = IupGetChild(tabs, pos);
  char* filename = IupGetAttribute(multitext, "FILENAME");
  int dirty = IupGetInt(multitext, "MODIFIED");
  Ihandle *projectConfig = iScintillaDlgGetProjectConfig(tabs);

  item_save = IupItem("Save\tCtrl+S", NULL);
  IupSetAttribute(item_save, "IMAGE", "IUP_FileSave");
  IupSetCallback(item_save, "ACTION", (Icallback)item_save_action_cb);

  item_close = IupItem("Close\tCtrl+F4", NULL);
  IupSetCallback(item_close, "ACTION", (Icallback)item_close_action_cb);

  item_closeall = IupItem("Close All\tCtrl+Shift+F4", NULL);
  IupSetCallback(item_closeall, "ACTION", (Icallback)item_closeall_action_cb);

  item_closeall_butthis = IupItem("Close All But This", NULL);
  IupSetCallback(item_closeall_butthis, "ACTION", (Icallback)item_closeall_butthis_action_cb);

  item_revert = IupItem("Revert", NULL);
  IupSetCallback(item_revert, "ACTION", (Icallback)item_revert_action_cb);

  item_openfolder = IupItem("Open Containing Folder", NULL);
  IupSetCallback(item_openfolder, "ACTION", (Icallback)item_openfolder_action_cb);

  item_copyfilename = IupItem("Copy File Name", NULL);
  IupSetCallback(item_copyfilename, "ACTION", (Icallback)item_copyfilename_action_cb);

  item_addtoproject = IupItem("Add To Project", NULL);
  IupSetCallback(item_addtoproject, "ACTION", (Icallback)item_addtoproject_action_cb);

  if (dirty)
    IupSetAttribute(item_save, "ACTIVE", "YES");
  else
    IupSetAttribute(item_save, "ACTIVE", "NO");

  if (dirty && filename)
    IupSetAttribute(item_revert, "ACTIVE", "YES");
  else
    IupSetAttribute(item_revert, "ACTIVE", "NO");

  if (IupGetChildCount(IupGetParent(multitext)) > 1)
    IupSetAttribute(item_closeall_butthis, "ACTIVE", "YES");
  else
    IupSetAttribute(item_closeall_butthis, "ACTIVE", "NO");

  if (filename)
  {
    IupSetAttribute(item_openfolder, "ACTIVE", "YES");
    IupSetAttribute(item_copyfilename, "ACTIVE", "YES");
  }
  else
  {
    IupSetAttribute(item_openfolder, "ACTIVE", "NO");
    IupSetAttribute(item_copyfilename, "ACTIVE", "NO");
  }

  if (projectConfig)
    IupSetAttribute(item_addtoproject, "ACTIVE", "YES");
  else
    IupSetAttribute(item_addtoproject, "ACTIVE", "NO");

  menu = IupMenu(
    item_revert,
    item_save,
    item_close,
    item_closeall,
    item_closeall_butthis,
    IupSeparator(),
    item_copyfilename,
    item_openfolder,
    IupSeparator(),
    item_addtoproject,
    NULL);

  iupAttribSet(menu, "TABS", (char*)tabs);
  iupAttribSet(menu, "MULTITEXT", (char*)multitext);

  IupPopup(menu, IUP_MOUSEPOS, IUP_MOUSEPOS);

  IupDestroy(menu);

  return IUP_DEFAULT;
}

static int tabs_change_cb(Ihandle* tabs, Ihandle* new_tab, Ihandle* old_tab)
{
  update_dialog_title(new_tab);

  (void)tabs;
  (void)old_tab;
  return IUP_DEFAULT;
}

static int tabs_close_cb(Ihandle* tabs, int pos)
{
  Ihandle* multitext = IupGetChild(tabs, pos);

  if (!iScintillaDlgCloseMultitext(multitext, 1))
    return IUP_IGNORE;  /* to abort the CLOSE_CB callback */

  if (IupGetChildCount(tabs) == 0)
    new_file(tabs);  /* always keep at least one multitext */

  return IUP_IGNORE;  /* we already destroyed the multitext */
}

static int find_close_action_cb(Ihandle* bt_close);

static int close_exit_action_cb(Ihandle* ih_item)
{
  Ihandle* ih = IupGetDialog(ih_item);
  Ihandle* tabs = IupGetDialogChild(ih, "TABS");
  Ihandle* config = iScintillaDlgGetConfig(ih);
  Ihandle* find_dlg = (Ihandle*)IupGetAttribute(ih, "FIND_DIALOG");
  Ihandle* panelSplit = IupGetDialogChild(ih, "PANEL_SPLIT");

  int pos;
  Ihandle* multitext, *tmp;
  IFnn configsave_cb;
  Icallback cb;
  Ihandle *projectConfig = iScintillaDlgGetProjectConfig(ih);

  IupConfigSetVariableStr(config, IupGetAttribute(ih, "SUBTITLE"), "Split", IupGetAttribute(panelSplit, "VALUE"));

  if (projectConfig)
  {
    if (item_close_proj_action_cb(ih) == IUP_IGNORE)
      return IUP_IGNORE;
  }

  for (multitext = tabs->firstchild, pos = 0; multitext; pos++ /* increment before destroy */)
  {
    /* increment before destroy */
    tmp = multitext;
    multitext = multitext->brother;

    if (!iScintillaDlgCloseMultitext(tmp, 1))
      return IUP_IGNORE;  /* to abort the CLOSE_CB callback */
  }

  if (find_dlg)
    find_close_action_cb(find_dlg);

  configsave_cb = (IFnn)IupGetCallback(ih, "CONFIGSAVE_CB");
  if (configsave_cb)
    configsave_cb(ih, config);

  IupConfigDialogClosed(config, ih, IupGetAttribute(ih, "SUBTITLE"));
  IupConfigSave(config);

  iupAttribSet(ih, "_IUP_CLOSING", "1");
  IupHide(ih);
  iupAttribSet(ih, "_IUP_CLOSING", NULL);

  /* after hide, at the last moment */
  cb = IupGetCallback(ih, "EXIT_CB");
  if (cb)
    cb(ih);

  return IUP_DEFAULT;
}

static int tree_executeleaf_cb(Ihandle* projectTree, int id)
{
  char* filename = IupTreeGetUserId(projectTree, id);
  if (id == 0)
    return IUP_DEFAULT;

  if (!check_open(projectTree, filename, 0))
    open_file(projectTree, filename, 1);
  return IUP_DEFAULT;
}

static int tree_showrename_cb(Ihandle* projectTree, int id)
{
  (void)projectTree;
  if (id == 0)
    return IUP_IGNORE;
  return IUP_DEFAULT;
}

static int tree_rename_cb(Ihandle* projectTree, int id, char* new_name)
{
  Ihandle* tabs = IupGetDialogChild(projectTree, "TABS");
  Ihandle* multitext;
  char* filename, *m_filename;
  Ihandle* config = iScintillaDlgGetConfig(projectTree);
  char new_filename[10240];

  if (id == 0)
    return IUP_IGNORE;

  filename = IupTreeGetUserId(projectTree, id);

  strcpy(new_filename, filename);
  strcpy((char*)strFileTitle(new_filename), new_name);

  for (multitext = tabs->firstchild; multitext; multitext = multitext->brother)
  {
    m_filename = IupGetAttribute(multitext, "FILENAME");
    if (iupStrEqual(m_filename, filename))
      break;
  }

  if (renameFile(IupGetDialog(projectTree), config, multitext, filename, new_filename))
  {
    Ihandle* projectConfig = iScintillaDlgGetProjectConfig(projectTree);

    IupSetAttribute(projectConfig, "MODIFIED", "YES");

    IupSetStrAttributeId(projectTree, "TITLE", id, strFileTitle(new_filename));
    IupTreeSetUserId(projectTree, id, iupStrDup(new_filename));

    saveMarkers(projectConfig, multitext);
    saveProjectFiles(projectTree, projectConfig);
    saveProjectOpenFiles(projectTree, projectConfig);

    free(filename);
  }

  return IUP_DEFAULT;
}

static int leaf_open_action_cb(Ihandle* ih_item)
{
  Ihandle *projectTree = get_project_tree(ih_item);
  int id = IupGetInt(projectTree, "VALUE");
  char* filename;

  /* can be called from the hot key */
  if (id == 0)
    return IUP_DEFAULT;

  filename = IupTreeGetUserId(projectTree, id);
  if (!check_open(projectTree, filename, 0))
    open_file(projectTree, filename, 1);
  return IUP_DEFAULT;
}

static int leaf_remove_action_cb(Ihandle* ih_item)
{
  Ihandle *projectTree = get_project_tree(ih_item);
  int id = IupGetInt(projectTree, "VALUE");
  Ihandle* ih = IupGetDialog(projectTree);
  Ihandle* projectConfig = iScintillaDlgGetProjectConfig(ih);

  /* can be called from the hot key */
  if (id == 0)
    return IUP_DEFAULT;

  removeFileFromProject(projectConfig, projectTree, id);
  return IUP_DEFAULT;
}

static int leaf_rename_action_cb(Ihandle* ih_item)
{
  Ihandle *projectTree = get_project_tree(ih_item);
  int id = IupGetInt(projectTree, "VALUE");

  /* can be called from the hot key */
  if (id == 0)
    return IUP_DEFAULT;

  IupSetAttribute(projectTree, "RENAME", "Yes");

  return IUP_DEFAULT;
}

static int leaf_copyfilename_action_cb(Ihandle* ih_item)
{
  Ihandle *projectTree = get_project_tree(ih_item);
  int id = IupGetInt(projectTree, "VALUE");
  char* filename;

  if (id == 0)
    return IUP_DEFAULT;

  filename = IupTreeGetUserId(projectTree, id);
  copyFilenameToClipboard(filename);
  return IUP_DEFAULT;
}

static int leaf_openfolder_action_cb(Ihandle* ih_item)
{
  Ihandle *projectTree = get_project_tree(ih_item);
  int id = IupGetInt(projectTree, "VALUE");
  char* filename;

  if (id == 0)
    return IUP_DEFAULT;

  filename = IupTreeGetUserId(projectTree, id);
  openFolder(filename);
  return IUP_DEFAULT;
}

static int tree_dropfiles_cb(Ihandle* projectTree, const char* filename, int num, int x, int y)
{
  Ihandle* projectConfig = iScintillaDlgGetProjectConfig(projectTree);
  int id = IupConvertXYToPos(projectTree, x, y);
  IupSetInt(projectTree, "VALUE", id);

  if (!check_inproject(projectTree, filename))
  {
    addFileToProjectTree(projectTree, filename);
    IupSetAttribute(projectConfig, "MODIFIED", "YES");
  }

  (void)num;
  return IUP_DEFAULT;
}

static int tree_rightclick_cb(Ihandle* projectTree, int id)
{
  Ihandle *menu, *item_open, *item_remove, *item_rename, *item_copyfilename, *item_openfolder;

  if (id == 0)
    return IUP_DEFAULT;

  IupSetInt(projectTree, "VALUE", id);

  item_open = IupItem("&Open\tEnter", NULL);
  IupSetAttribute(item_open, "IMAGE", "IUP_FileSave");
  IupSetCallback(item_open, "ACTION", (Icallback)leaf_open_action_cb);

  item_remove = IupItem("&Remove\tDel", NULL);
  IupSetAttribute(item_remove, "IMAGE", "IUP_EditErase");
  IupSetCallback(item_remove, "ACTION", (Icallback)leaf_remove_action_cb);

  item_rename = IupItem("R&ename...\tF2", NULL);
  IupSetCallback(item_rename, "ACTION", (Icallback)leaf_rename_action_cb);

  item_copyfilename = IupItem("&Copy File Name", NULL);
  IupSetCallback(item_copyfilename, "ACTION", (Icallback)leaf_copyfilename_action_cb);

  item_openfolder = IupItem("Open Containing &Folder", NULL);
  IupSetCallback(item_openfolder, "ACTION", (Icallback)leaf_openfolder_action_cb);

  menu = IupMenu(
    item_open,
    item_remove,
    item_rename,
    IupSeparator(),
    item_copyfilename,
    item_openfolder,
    NULL);

  iupAttribSet(menu, "PROJECTTREE", (char*)projectTree);

  IupPopup(menu, IUP_MOUSEPOS, IUP_MOUSEPOS);

  IupDestroy(menu);

  return IUP_DEFAULT;
}

static void tree_project_clear(Ihandle* projectTree)
{
  int i = 1;
  char* filename = IupTreeGetUserId(projectTree, i);
  while (filename != NULL)
  {
    free(filename);

    i++;
    filename = IupTreeGetUserId(projectTree, i);
  }

  IupSetAttribute(projectTree, "DELNODE0", "CHILDREN");
}

static int list_search_dblclick_cb(Ihandle *ih, int index, char *t)
{
  Ihandle* projectTree = IupGetDialogChild(ih, "PROJECTTREE");
  Ihandle* tabs = IupGetDialogChild(ih, "TABS");
  char *filename = IupGetAttributeId(ih, "FILENAME", index);
  int lin = IupGetIntId(ih, "LINE", index);
  int col = IupGetIntId(ih, "COL", index);
  int start_pos = IupGetIntId(ih, "POSSTART", index);
  int end_pos = IupGetIntId(ih, "POSEND", index);
  Ihandle *multitext = check_open(projectTree, filename, 0);

  if (!multitext)
  {
    open_file(projectTree, filename, 1);
    multitext = iScintillaDlgGetCurrentMultitext(ih);
  }
  else
    IupSetAttribute(tabs, "VALUE_HANDLE", (char*)multitext);

  IupSetFocus(multitext);
  IupSetfAttribute(multitext, "SELECTIONPOS", "%d:%d", start_pos, end_pos);

  /* update statusbar */
  IupTextConvertPosToLinCol(multitext, end_pos, &lin, &col);
  multitext_caret_cb(multitext, lin, col);

  (void)t;
  return IUP_DEFAULT;
}

static int item_new_blank_proj_action_cb(Ihandle* ih_item)
{
  Ihandle* ih = IupGetDialog(ih_item);
  IFnn cb = (IFnn)IupGetCallback(ih, "CONFIGLOAD_CB");
  Ihandle* projectSplit = IupGetDialogChild(ih, "PROJECTSPLIT");
  Ihandle* projectTree = IupGetDialogChild(ih, "PROJECTTREE");
  Ihandle *projectConfig = iScintillaDlgGetProjectConfig(ih);

  if (projectConfig)
  {
    if (item_close_proj_action_cb(ih) == IUP_IGNORE)
      return IUP_IGNORE;
  }

  projectConfig = IupConfig();
  iupAttribSet(ih, "_IUP_PROJECT_CONFIG", (char*)projectConfig);

  tree_project_clear(projectTree);

  IupSetAttribute(projectTree, "TITLE0", "Untitled");

  if (cb)
    cb(ih, projectConfig);

  if (IupGetInt(projectSplit, "VALUE") == 0)
  {
    IupSetAttribute(projectSplit, "VALUE", "200");
    IupRefresh(ih);
  }

  return IUP_DEFAULT;
}

static int item_new_proj_action_cb(Ihandle* ih_item)
{
  Ihandle* ih = IupGetDialog(ih_item);
  IFnn cb = (IFnn)IupGetCallback(ih, "CONFIGLOAD_CB");
  Ihandle* tabs = IupGetDialogChild(ih_item, "TABS");
  Ihandle* projectTree = IupGetDialogChild(ih_item, "PROJECTTREE");
  Ihandle* multitext;
  char* filename;
  Ihandle *config, *projectConfig;

  if (item_new_blank_proj_action_cb(ih_item) == IUP_IGNORE)
    return IUP_IGNORE;

  config = iScintillaDlgGetConfig(ih_item);
  projectConfig = iScintillaDlgGetProjectConfig(ih_item);

  IupConfigCopy(config, projectConfig, "Scintilla");  /* do not copy "Scintilla" prefixed groups */

  if (cb)
    cb(ih, projectConfig);

  for (multitext = tabs->firstchild; multitext; multitext = multitext->brother)
  {
    filename = IupGetAttribute(multitext, "FILENAME");

    if (!filename || iupStrEqual(filename, "Untitled"))
      continue;

    if (!check_inproject(projectTree, filename))
    {
      addFileToProjectTree(projectTree, filename);
      IupSetAttribute(projectConfig, "MODIFIED", "YES");
    }
  }

  return IUP_DEFAULT;
}

static void loadProjectFiles(Ihandle *projectConfig, Ihandle *projectTree)
{
  const char *value;
  int count, i;

  count = IupConfigGetVariableInt(projectConfig, "ProjectFiles", "Count");

  for (i = 1; i <= count; i++)
  {
    value = IupConfigGetVariableStrId(projectConfig, "ProjectFiles", "File", i);
    if (!check_inproject(projectTree, value))
      addFileToProjectTree(projectTree, value);
  }

  count = IupConfigGetVariableInt(projectConfig, "ProjectOpenFiles", "Count");

  for (i = 1; i <= count; i++)
  {
    value = IupConfigGetVariableStrId(projectConfig, "ProjectOpenFiles", "File", i);
    if (!check_open(projectTree, value, 0))
      open_file(projectTree, value, 1);
  }
}

static int item_open_proj_action_cb(Ihandle* ih_item)
{
  Ihandle* ih = IupGetDialog(ih_item);
  Ihandle *filedlg;
  Ihandle* config;
  const char* dir = NULL;

  config = iScintillaDlgGetConfig(ih_item);
  dir = IupConfigGetVariableStr(config, IupGetAttribute(ih, "SUBTITLE"), "LastDirectory");

  filedlg = IupFileDlg();
  IupSetAttribute(filedlg, "DIALOGTYPE", "OPEN");
  IupSetAttribute(filedlg, "EXTFILTER", "Project Files|*.cfg|All Files|*.*|");
  IupSetAttributeHandle(filedlg, "PARENTDIALOG", ih);
  IupSetStrAttribute(filedlg, "DIRECTORY", dir);
  IupSetAttribute(filedlg, "TITLE", "Open Project");

  IupPopup(filedlg, IUP_CENTERPARENT, IUP_CENTERPARENT);

  if (IupGetInt(filedlg, "STATUS") != -1)
  {
    char* filename = IupGetAttribute(filedlg, "VALUE");
    open_proj(ih, filename);

    dir = IupGetAttribute(filedlg, "DIRECTORY");
    IupConfigSetVariableStr(config, IupGetAttribute(ih, "SUBTITLE"), "LastDirectory", dir);
  }

  IupDestroy(filedlg);
  return IUP_DEFAULT;
}

static int item_save_proj_action_cb(Ihandle* ih_item)
{
  Ihandle* ih = IupGetDialog(ih_item);
  Ihandle *projectConfig = iScintillaDlgGetProjectConfig(ih);

  saveProject(ih, projectConfig, 0);

  return IUP_DEFAULT;
}

static void saveProject(Ihandle *ih_item, Ihandle *projectConfig, int show_dialog)
{
  IFnn cb;
  char *filename = IupGetAttribute(projectConfig, "APP_FILENAME");
  Ihandle* ih = IupGetDialog(ih_item);
  if (!filename || show_dialog)
  {
    Ihandle *config = iScintillaDlgGetConfig(ih);
    const char *dir = IupConfigGetVariableStr(config, IupGetAttribute(ih, "SUBTITLE"), "LastDirectory");
    Ihandle *filedlg = IupFileDlg();

    IupSetAttribute(filedlg, "DIALOGTYPE", "SAVE");

    IupSetAttribute(filedlg, "EXTFILTER", "Project Files|*.cfg|All Files|*.*|");
    IupSetAttributeHandle(filedlg, "PARENTDIALOG", ih);
    IupSetStrAttribute(filedlg, "FILE", filename);
    IupSetStrAttribute(filedlg, "DIRECTORY", dir);
    IupSetAttribute(filedlg, "TITLE", "Save Project As");

    IupPopup(filedlg, IUP_CENTERPARENT, IUP_CENTERPARENT);

    if (IupGetInt(filedlg, "STATUS") != -1)
    {
      Ihandle* projectTree = IupGetDialogChild(ih, "PROJECTTREE");

      filename = IupGetAttribute(filedlg, "VALUE");
      IupSetStrAttribute(projectConfig, "APP_FILENAME", filename);
      IupSetStrAttribute(projectTree, "TITLE0", strFileTitle(filename));

      IupSetAttribute(config, "RECENTNAME", "ScintillaRecentProject");
      IupConfigRecentUpdate(config, filename);

      dir = IupGetAttribute(filedlg, "DIRECTORY");
      IupConfigSetVariableStr(config, IupGetAttribute(ih, "SUBTITLE"), "LastDirectory", dir);
    }
    else
      return;
  }

  cb = (IFnn)IupGetCallback(ih, "CONFIGSAVE_CB");
  if (cb)
    cb(ih, projectConfig);

  saveAllMarkers(ih, projectConfig);
  saveProjectOpenFiles(ih, projectConfig);
  IupConfigSave(projectConfig);

  IupSetAttribute(projectConfig, "MODIFIED", "NO");
}

static int item_save_proj_as_action_cb(Ihandle* ih_item)
{
  Ihandle *projectConfig = iScintillaDlgGetProjectConfig(ih_item);
  saveProject(ih_item, projectConfig, 1);
  return IUP_DEFAULT;
}

static int item_close_proj_action_cb(Ihandle* ih_item)
{
  Ihandle* ih = IupGetDialog(ih_item);
  IFnn cb = (IFnn)IupGetCallback(ih, "CONFIGLOAD_CB");
  Ihandle* projectSplit = IupGetDialogChild(ih, "PROJECTSPLIT");
  Ihandle* projectTree = IupGetDialogChild(ih, "PROJECTTREE");
  Ihandle* projectConfig = iScintillaDlgGetProjectConfig(ih);
  Ihandle *config = iScintillaDlgGetConfig(ih);

  if (!save_project_check(ih))
    return IUP_IGNORE;

  item_closeall_action_cb(ih_item);

  tree_project_clear(projectTree);

  IupSetAttribute(projectTree, "TITLE0", "Untitled");

  IupSetAttribute(projectSplit, "VALUE", "0");

  IupDestroy(projectConfig);

  iupAttribSet(ih, "_IUP_PROJECT_CONFIG", NULL);

  if (cb)
    cb(ih, config);

  return IUP_DEFAULT;
}

static int item_add_new_file_action_cb(Ihandle* ih_item)
{
  Ihandle* ih = IupGetDialog(ih_item);
  Ihandle* config = iScintillaDlgGetConfig(ih_item);
  const char *dir = IupConfigGetVariableStr(config, IupGetAttribute(ih, "SUBTITLE"), "LastDirectory");
  Ihandle *filedlg = IupFileDlg();

  IupSetAttribute(filedlg, "DIALOGTYPE", "SAVE");

  IupSetAttributeHandle(filedlg, "PARENTDIALOG", ih);
  IupSetStrAttribute(filedlg, "DIRECTORY", dir);

  IupPopup(filedlg, IUP_CENTERPARENT, IUP_CENTERPARENT);

  if (IupGetInt(filedlg, "STATUS") != -1)
  {
    Ihandle* multitext;
    Ihandle* projectTree = IupGetDialogChild(ih_item, "PROJECTTREE");
    Ihandle* projectConfig = iScintillaDlgGetProjectConfig(ih);

    char *filename = IupGetAttribute(filedlg, "VALUE");
    if (!check_inproject(projectTree, filename))
    {
      addFileToProjectTree(projectTree, filename);
      IupSetAttribute(projectConfig, "MODIFIED", "YES");
    }

    dir = IupGetAttribute(filedlg, "DIRECTORY");
    IupConfigSetVariableStr(config, IupGetAttribute(ih, "SUBTITLE"), "LastDirectory", dir);

    multitext = iScintillaDlgNewMultitext(ih_item);

    IupSetAttribute(multitext, "FILENAME", filename);
    IupSetAttribute(multitext, "VALUE", " ");
    IupSetAttribute(multitext, "SAVEPOINT", NULL); /* this will update title */
    IupSetAttribute(multitext, "UNDO", NULL); /* clear undo */

    save_file(multitext);
  }

  return IUP_DEFAULT;
}

static int item_add_exist_file_action_cb(Ihandle* ih_item)
{
  Ihandle* ih = IupGetDialog(ih_item);
  Ihandle *filedlg;
  Ihandle *config, *projectConfig;
  const char* dir = NULL;
  char* extra_filters = IupGetAttribute(ih, "EXTRAFILTERS");
  Ihandle* projectTree = IupGetDialogChild(ih, "PROJECTTREE");

  config = iScintillaDlgGetConfig(ih_item);
  projectConfig = iScintillaDlgGetProjectConfig(ih_item);
  dir = IupConfigGetVariableStr(config, IupGetAttribute(ih, "SUBTITLE"), "LastDirectory");

  filedlg = IupFileDlg();
  IupSetAttribute(filedlg, "DIALOGTYPE", "OPEN");
  if (extra_filters)
    IupSetStrf(filedlg, "EXTFILTER", "%sText Files|*.txt|All Files|*.*|", extra_filters);
  else
    IupSetAttribute(filedlg, "EXTFILTER", "Text Files|*.txt|All Files|*.*|");
  IupSetAttributeHandle(filedlg, "PARENTDIALOG", ih);
  IupSetStrAttribute(filedlg, "DIRECTORY", dir);

  IupPopup(filedlg, IUP_CENTERPARENT, IUP_CENTERPARENT);

  if (IupGetInt(filedlg, "STATUS") != -1)
  {
    char* filename = IupGetAttribute(filedlg, "VALUE");
    if (!check_inproject(projectTree, filename))
    {
      addFileToProjectTree(projectTree, filename);
      IupSetAttribute(projectConfig, "MODIFIED", "YES");
    }

    dir = IupGetAttribute(filedlg, "DIRECTORY");
    IupConfigSetVariableStr(config, IupGetAttribute(ih, "SUBTITLE"), "LastDirectory", dir);
  }

  IupDestroy(filedlg);
  return IUP_DEFAULT;
}

static int item_add_open_file_action_cb(Ihandle* ih_item)
{
  Ihandle* ih = IupGetDialog(ih_item);
  Ihandle* projectTree = IupGetDialogChild(ih, "PROJECTTREE");
  Ihandle* projectConfig = iScintillaDlgGetProjectConfig(ih_item);
  Ihandle* multitext = iScintillaDlgGetCurrentMultitext(ih_item);
  char* filename = IupGetAttribute(multitext, "FILENAME");

  if (!check_inproject(projectTree, filename))
  {
    addFileToProjectTree(projectTree, filename);
    IupSetAttribute(projectConfig, "MODIFIED", "YES");
  }

  return IUP_DEFAULT;
}

static int item_open_proj_file_action_cb(Ihandle* ih_item)
{
  char* filename;
  Ihandle* projectTree = IupGetDialogChild(ih_item, "PROJECTTREE");

  int id = IupGetInt(projectTree, "VALUE");
  if (id < 1)
    return IUP_DEFAULT;

  filename = IupTreeGetUserId(projectTree, id);
  if (!check_open(projectTree, filename, 0))
    open_file(projectTree, filename, 1);

  return IUP_DEFAULT;
}

static int item_open_all_proj_file_action_cb(Ihandle* ih_item)
{
  Ihandle* projectTree = IupGetDialogChild(ih_item, "PROJECTTREE");
  int count = IupGetInt(projectTree, "COUNT");
  int i;

  for (i = 1; i < count; i++)
  {
    char* filename = IupTreeGetUserId(projectTree, i);
    if (!check_open(projectTree, filename, 0))
      open_file(projectTree, filename, 1);
  }

  return IUP_DEFAULT;
}

static int item_remove_proj_file_action_cb(Ihandle* ih_item)
{
  Ihandle* ih = IupGetDialog(ih_item);
  Ihandle* projectTree = IupGetDialogChild(ih, "PROJECTTREE");
  int id = IupGetInt(projectTree, "VALUE");
  Ihandle* projectConfig = iScintillaDlgGetProjectConfig(ih);

  removeFileFromProject(projectConfig, projectTree, id);

  return IUP_DEFAULT;
}

static int item_loadsession_action_cb(Ihandle *ih_item)
{
  Ihandle* ih = IupGetDialog(ih_item);
  const char* dir;
  Ihandle* config;
  Ihandle *filedlg;

  config = iScintillaDlgGetConfig(ih_item);
  dir = IupConfigGetVariableStr(config, IupGetAttribute(ih, "SUBTITLE"), "LastDirectory");

  filedlg = IupFileDlg();

  IupSetAttribute(filedlg, "DIALOGTYPE", "OPEN");
  IupSetAttribute(filedlg, "EXTFILTER", "Text Files|*.txt|All Files|*.*|");
  IupSetAttributeHandle(filedlg, "PARENTDIALOG", ih);
  IupSetStrAttribute(filedlg, "DIRECTORY", dir);

  IupPopup(filedlg, IUP_CENTERPARENT, IUP_CENTERPARENT);

  if (IupGetInt(filedlg, "STATUS") != -1)
  {
    char* filename = IupGetAttribute(filedlg, "VALUE");
    IlineFile* line_file = iupLineFileOpen(filename);
    if (!line_file)
    {
      IupMessageError(ih, "IUP_ERRORFILEOPEN");
      return IUP_DEFAULT;
    }

    do
    {
      const char* line_buffer;

      int line_len = iupLineFileReadLine(line_file);
      if (line_len == -1)
        break;

      line_buffer = iupLineFileGetBuffer(line_file);

      if (!check_open(ih, line_buffer, 0))
        open_file(ih_item, line_buffer, 1);

    } while (!iupLineFileEOF(line_file));

    iupLineFileClose(line_file);

    dir = IupGetAttribute(filedlg, "DIRECTORY");
    IupConfigSetVariableStr(config, IupGetAttribute(ih, "SUBTITLE"), "LastDirectory", dir);
  }

  return IUP_DEFAULT;
}

static int item_savesession_action_cb(Ihandle *ih_item)
{
  Ihandle* ih = IupGetDialog(ih_item);
  const char* dir;
  Ihandle* config;
  Ihandle *filedlg;

  config = iScintillaDlgGetConfig(ih_item);
  dir = IupConfigGetVariableStr(config, IupGetAttribute(ih, "SUBTITLE"), "LastDirectory");

  filedlg = IupFileDlg();

  IupSetAttribute(filedlg, "DIALOGTYPE", "SAVE");
  IupSetAttribute(filedlg, "EXTFILTER", "Text Files|*.txt|All Files|*.*|");
  IupSetAttributeHandle(filedlg, "PARENTDIALOG", ih);
  IupSetStrAttribute(filedlg, "DIRECTORY", dir);

  IupPopup(filedlg, IUP_CENTERPARENT, IUP_CENTERPARENT);

  if (IupGetInt(filedlg, "STATUS") != -1)
  {
    Ihandle* tabs = IupGetDialogChild(ih, "TABS");
    char* filename = IupGetAttribute(filedlg, "VALUE");
    int i, count = IupGetChildCount(tabs);

    FILE* file = fopen(filename, "wb");
    if (!file)
    {
      IupMessageError(ih, "IUP_ERRORFILESAVE");
      return IUP_DEFAULT;
    }

    for (i = 0; i < count; i++)
    {
      Ihandle* multitext = IupGetChild(tabs, i);
      char* m_filename = IupGetAttribute(multitext, "FILENAME");
      if (m_filename && IupGetInt(multitext, "COUNT") > 0)
      {
        if (i > 0)
          fprintf(file, "\n");
        fprintf(file, "%s", m_filename);
      }
    }

    fclose(file);

    dir = IupGetAttribute(filedlg, "DIRECTORY");
    IupConfigSetVariableStr(config, IupGetAttribute(ih, "SUBTITLE"), "LastDirectory", dir);
  }

  IupDestroy(filedlg);
  return IUP_DEFAULT;
}
static int show_cb(Ihandle* ih, int state)
{
  if (state == IUP_HIDE && !iupAttribGet(ih, "_IUP_CLOSING"))
    close_exit_action_cb(ih);

  return IUP_DEFAULT;
}

static int goto_ok_action_cb(Ihandle* bt_ok)
{
  int line_count = IupGetInt(bt_ok, "TEXT_LINECOUNT");
  Ihandle* txt = IupGetDialogChild(bt_ok, "LINE_TEXT");
  int line = IupGetInt(txt, "VALUE");
  if (line < 1 || line > line_count)
  {
    IupMessageError(IupGetDialog(bt_ok), "Invalid line number.");
    return IUP_DEFAULT;
  }

  IupSetAttribute(IupGetDialog(bt_ok), "STATUS", "1");
  return IUP_CLOSE;
}

static int goto_cancel_action_cb(Ihandle* bt_ok)
{
  IupSetAttribute(IupGetDialog(bt_ok), "STATUS", "0");
  return IUP_CLOSE;
}

static int item_goto_action_cb(Ihandle* ih_item)
{
  Ihandle* multitext = iScintillaDlgGetCurrentMultitext(ih_item);
  Ihandle *goto_dlg, *box, *bt_ok, *bt_cancel, *txt, *lbl;
  int line_count = IupGetInt(multitext, "LINECOUNT");

  lbl = IupLabel(NULL);
  IupSetfAttribute(lbl, "TITLE", "Line Number [1-%d]:", line_count);
  txt = IupText(NULL);
  IupSetAttribute(txt, "MASK", IUP_MASK_UINT);  /* unsigned integer numbers only */
  IupSetAttribute(txt, "NAME", "LINE_TEXT");
  IupSetAttribute(txt, "VISIBLECOLUMNS", "20");
  bt_ok = IupButton("OK", NULL);
  IupSetInt(bt_ok, "TEXT_LINECOUNT", line_count);
  IupSetAttribute(bt_ok, "PADDING", "10x2");
  IupSetCallback(bt_ok, "ACTION", (Icallback)goto_ok_action_cb);
  bt_cancel = IupButton("Cancel", NULL);
  IupSetCallback(bt_cancel, "ACTION", (Icallback)goto_cancel_action_cb);
  IupSetAttribute(bt_cancel, "PADDING", "10x2");

  box = IupVbox(
    lbl,
    txt,
    IupSetAttributes(IupHbox(
    IupFill(),
    bt_ok,
    bt_cancel,
    NULL), "NORMALIZESIZE=HORIZONTAL"),
    NULL);
  IupSetAttribute(box, "MARGIN", "10x10");
  IupSetAttribute(box, "GAP", "5");

  goto_dlg = IupDialog(box);
  IupSetAttribute(goto_dlg, "TITLE", "Go To Line");
  IupSetAttribute(goto_dlg, "DIALOGFRAME", "Yes");
  IupSetAttributeHandle(goto_dlg, "DEFAULTENTER", bt_ok);
  IupSetAttributeHandle(goto_dlg, "DEFAULTESC", bt_cancel);
  IupSetAttributeHandle(goto_dlg, "PARENTDIALOG", IupGetDialog(ih_item));

  IupPopup(goto_dlg, IUP_CENTERPARENT, IUP_CENTERPARENT);

  if (IupGetInt(goto_dlg, "STATUS") == 1)
  {
    int line = IupGetInt(txt, "VALUE");
    int pos;
    IupTextConvertLinColToPos(multitext, line - 1, 0, &pos);  /* in Scintilla lin and col start at 0 */
    IupSetAttributeId(multitext, "ENSUREVISIBLE", line - 1, NULL);
    IupSetInt(multitext, "CARETPOS", pos);
    IupSetInt(multitext, "SCROLLTOPOS", pos);
  }

  IupDestroy(goto_dlg);

  return IUP_DEFAULT;
}

static int item_gotombrace_action_cb(Ihandle* ih_item)
{
  Ihandle* multitext = iScintillaDlgGetCurrentMultitext(ih_item);

  int pos = IupGetInt(multitext, "CARETPOS");

  int newpos = IupGetIntId(multitext, "BRACEMATCH", pos);

  if (newpos != -1)
  {
    IupSetStrf(multitext, "BRACEHIGHLIGHT", "%d:%d", pos, newpos);

    IupSetInt(multitext, "CARETPOS", newpos);
    IupSetInt(multitext, "SCROLLTOPOS", newpos);
  }

  return IUP_IGNORE;
}

static int item_togglemark_action_cb(Ihandle* ih_item)
{
  Ihandle* multitext = iScintillaDlgGetCurrentMultitext(ih_item);

  int pos = IupGetInt(multitext, "CARETPOS");

  int lin, col;
  IupTextConvertPosToLinCol(multitext, pos, &lin, &col);

  toggleMarker(multitext, lin, 1);

  return IUP_IGNORE;
}

static int item_nextmark_action_cb(Ihandle* ih_item)
{
  Ihandle* multitext = iScintillaDlgGetCurrentMultitext(ih_item);

  int pos = IupGetInt(multitext, "CARETPOS");

  int lin, col;
  IupTextConvertPosToLinCol(multitext, pos, &lin, &col);

  IupSetAttributeId(multitext, "MARKERNEXT", lin + 1, "1");  /* 0001 - marker=0 */

  lin = IupGetInt(multitext, "LASTMARKERFOUND");

  if (lin == -1)
    return IUP_IGNORE;

  IupTextConvertLinColToPos(multitext, lin, 0, &pos);

  IupSetInt(multitext, "CARETPOS", pos);

  return IUP_DEFAULT;
}

static int item_previousmark_action_cb(Ihandle* ih_item)
{
  Ihandle* multitext = iScintillaDlgGetCurrentMultitext(ih_item);

  int pos = IupGetInt(multitext, "CARETPOS");

  int lin, col;
  IupTextConvertPosToLinCol(multitext, pos, &lin, &col);

  IupSetAttributeId(multitext, "MARKERPREVIOUS", lin - 1, "1");  /* 0001 - marker=0 */

  lin = IupGetInt(multitext, "LASTMARKERFOUND");

  if (lin == -1)
    return IUP_IGNORE;

  IupTextConvertLinColToPos(multitext, lin, 0, &pos);

  IupSetInt(multitext, "CARETPOS", pos);

  return IUP_DEFAULT;
}

static int item_clearmarks_action_cb(Ihandle* ih_item)
{
  Ihandle* multitext = iScintillaDlgGetCurrentMultitext(ih_item);
  removeAllBookmark(multitext);
  return IUP_DEFAULT;
}

static int item_copymarked_action_cb(Ihandle* ih_item)
{
  Ihandle* multitext = iScintillaDlgGetCurrentMultitext(ih_item);
  copyMarkedLines(multitext);
  return IUP_DEFAULT;
}

static int item_cutmarked_action_cb(Ihandle* ih_item)
{
  Ihandle* multitext = iScintillaDlgGetCurrentMultitext(ih_item);

  IupSetAttribute(multitext, "UNDOACTION", "BEGIN");

  cutMarkedLines(multitext);

  IupSetAttribute(multitext, "UNDOACTION", "END");

  return IUP_DEFAULT;
}

static int item_pastetomarked_action_cb(Ihandle* ih_item)
{
  Ihandle* multitext = iScintillaDlgGetCurrentMultitext(ih_item);

  IupSetAttribute(multitext, "UNDOACTION", "BEGIN");

  pasteToMarkedLines(multitext);

  IupSetAttribute(multitext, "UNDOACTION", "END");

  return IUP_DEFAULT;
}

static int item_removemarked_action_cb(Ihandle* ih_item)
{
  Ihandle* multitext = iScintillaDlgGetCurrentMultitext(ih_item);

  IupSetAttribute(multitext, "UNDOACTION", "BEGIN");

  removeMarkedLines(multitext);

  IupSetAttribute(multitext, "UNDOACTION", "END");

  return IUP_DEFAULT;
}

static int item_invertmarks_action_cb(Ihandle* ih_item)
{
  Ihandle* multitext = iScintillaDlgGetCurrentMultitext(ih_item);
  invertMarkedLines(multitext);
  return IUP_DEFAULT;
}

static int item_eoltospace_action_cb(Ihandle* ih_item)
{
  Ihandle* multitext = iScintillaDlgGetCurrentMultitext(ih_item);

  IupSetAttribute(multitext, "UNDOACTION", "BEGIN");

  changeEolToSpace(multitext);

  IupSetAttribute(multitext, "UNDOACTION", "END");

  return IUP_DEFAULT;
}

static int item_eol_action_cb(Ihandle* ih_item)
{
  Ihandle* multitext = iScintillaDlgGetCurrentMultitext(ih_item);
  char* eol = iupAttribGet(ih_item, "EOL");
  IupSetStrAttribute(multitext, "EOLMODE", eol);
  IupSetStrAttribute(multitext, "FIXEOL", eol);
  return IUP_DEFAULT;
}

static int item_removespaceeol_action_cb(Ihandle* ih_item)
{
  Ihandle* multitext = iScintillaDlgGetCurrentMultitext(ih_item);

  IupSetAttribute(multitext, "UNDOACTION", "BEGIN");

  removeTrailingSpaces(multitext);

  removeLeadingSpaces(multitext);

  changeEolToSpace(multitext);

  IupSetAttribute(multitext, "UNDOACTION", "END");

  return IUP_DEFAULT;
}

static int item_trimtrailing_action_cb(Ihandle* ih_item)
{
  Ihandle* multitext = iScintillaDlgGetCurrentMultitext(ih_item);

  IupSetAttribute(multitext, "UNDOACTION", "BEGIN");

  removeTrailingSpaces(multitext);

  IupSetAttribute(multitext, "UNDOACTION", "END");

  return IUP_DEFAULT;
}

static int item_trimleading_action_cb(Ihandle* ih_item)
{
  Ihandle* multitext = iScintillaDlgGetCurrentMultitext(ih_item);

  IupSetAttribute(multitext, "UNDOACTION", "BEGIN");

  removeLeadingSpaces(multitext);

  IupSetAttribute(multitext, "UNDOACTION", "END");

  return IUP_DEFAULT;
}

static int item_trimtraillead_action_cb(Ihandle* ih_item)
{
  Ihandle* multitext = iScintillaDlgGetCurrentMultitext(ih_item);

  IupSetAttribute(multitext, "UNDOACTION", "BEGIN");

  removeTrailingSpaces(multitext);

  removeLeadingSpaces(multitext);

  IupSetAttribute(multitext, "UNDOACTION", "END");

  return IUP_DEFAULT;
}

static int item_tabtospace_action_cb(Ihandle* ih_item)
{
  Ihandle* multitext = iScintillaDlgGetCurrentMultitext(ih_item);

  IupSetAttribute(multitext, "UNDOACTION", "BEGIN");

  changeTabsToSpaces(multitext);

  IupSetAttribute(multitext, "UNDOACTION", "END");

  return IUP_DEFAULT;
}

static int item_allspacetotab_action_cb(Ihandle* ih_item)
{
  Ihandle* multitext = iScintillaDlgGetCurrentMultitext(ih_item);

  IupSetAttribute(multitext, "UNDOACTION", "BEGIN");

  changeSpacesToTabs(multitext);

  IupSetAttribute(multitext, "UNDOACTION", "END");

  return IUP_DEFAULT;
}

static int item_leadingspacetotab_action_cb(Ihandle* ih_item)
{
  Ihandle* multitext = iScintillaDlgGetCurrentMultitext(ih_item);

  IupSetAttribute(multitext, "UNDOACTION", "BEGIN");

  changeLeadingSpacesToTabs(multitext);

  IupSetAttribute(multitext, "UNDOACTION", "END");

  return IUP_DEFAULT;
}

static int lst_search_in_action_cb(Ihandle* ih, char *text, int item, int state)
{
  Ihandle* find_dlg = (Ihandle*)IupGetAttribute(ih, "FIND_DIALOG");
  Ihandle* dirMode = IupGetDialogChild(find_dlg, "DIRECTION_RADIO");
  Ihandle* wrap = IupGetDialogChild(find_dlg, "WRAP");

  IupSetAttribute(dirMode, "VALUE_HANDLE", (char*)IupGetDialogChild(find_dlg, "DOWN"));
  IupSetAttribute(wrap, "VALUE", "OFF");

  if (item == 2 && state == 1)
  {
    IupSetAttribute(dirMode, "ACTIVE", "NO");
    IupSetAttribute(wrap, "ACTIVE", "NO");
  }
  else
  {
    IupSetAttribute(dirMode, "ACTIVE", "YES");
    IupSetAttribute(wrap, "ACTIVE", "YES");
  }

  (void)text;
  return IUP_DEFAULT;
}

static int find_next_action_cb(Ihandle* ih_item)
{
  /* this callback can be called from the main dialog also */
  Ihandle* find_dlg = (Ihandle*)IupGetAttribute(ih_item, "FIND_DIALOG");
  if (find_dlg)
  {
    char* str_to_find;
    Ihandle* ih = IupGetAttributeHandle(find_dlg, "PARENTDIALOG");
    Ihandle* tabs = IupGetDialogChild(ih, "TABS");
    Ihandle* projectTree = IupGetDialogChild(ih, "PROJECTTREE");
    Ihandle* sciDummy = IupGetDialogChild(find_dlg, "SCI_DUMMY");
    Ihandle* currentMultitext = iScintillaDlgGetCurrentMultitext(ih);
    Ihandle *multitext = currentMultitext;
    Ihandle* find_txt = IupGetDialogChild(find_dlg, "FIND_TEXT");
    int count = IupGetInt(tabs, "COUNT");
    int find_start, find_end;
    int found = 0;
    int i;

    /* test again, because it can be called from the hot key */
    str_to_find = IupGetAttribute(find_txt, "VALUE");
    if (str_to_find && str_to_find[0] != 0)
    {
      int searchIn = IupGetInt(IupGetDialogChild(find_dlg, "LST_SEARCH_IN"), "VALUE");
      int wrap = IupGetInt(IupGetDialogChild(find_dlg, "WRAP"), "VALUE");
      int down = IupGetInt(IupGetDialogChild(find_dlg, "DOWN"), "VALUE");
      int casesensitive = IupGetInt(IupGetDialogChild(find_dlg, "FIND_CASE"), "VALUE");
      int whole_word = IupGetInt(IupGetDialogChild(find_dlg, "WHOLE_WORD"), "VALUE");
      int regexp = IupGetInt(IupGetDialogChild(find_dlg, "REG_EXP"), "VALUE");
      int posix = IupGetInt(IupGetDialogChild(find_dlg, "POSIX"), "VALUE");

      if (!down && IupGetAttribute(currentMultitext, "SELECTIONPOS"))
      {
        int st, ed;
        IupGetIntInt(currentMultitext, "SELECTIONPOS", &st, &ed);
        find_start = st;
      }
      else
        find_start = IupGetInt(currentMultitext, "CARETPOS");

      find_end = down ? IupGetInt(currentMultitext, "COUNT") : 0;

      if (searchIn == 1)
        count = 1;

      for (i = 0; i < count; i++)
      {
        found = searchInFile(multitext, find_txt, find_start, find_end, wrap, down, casesensitive, whole_word, regexp, posix);

        if (found)
        {
          if ((Ihandle*)IupGetAttribute(tabs, "VALUE_HANDLE") != multitext)
            IupSetAttribute(tabs, "VALUE_HANDLE", (char *)multitext);
          break;
        }

        if (multitext->brother)
          multitext = multitext->brother;
        else
          multitext = tabs->firstchild;

        find_start = 0;
        find_end = IupGetInt(multitext, "COUNT");
      }

      if (searchIn == 3)
      {
        count = IupGetInt(projectTree, "CHILDCOUNT0");

        for (i = 1; i <= count; i++)
        {
          char* str;
          char* filename = IupTreeGetUserId(projectTree, i);
          if (check_open(projectTree, filename, 0))
            continue;

          str = readFile(filename);
          if (str)
          {
            int st1, ed1, st2, ed2;
            IupSetStrAttribute(sciDummy, "FILENAME", filename);
            IupSetStrAttribute(sciDummy, "VALUE", str);
            st1 = 0;
            ed1 = IupGetInt(sciDummy, "COUNT");
            IupSetInt(sciDummy, "TARGETSTART", st1);
            IupSetInt(sciDummy, "TARGETEND", ed1);
            IupSetAttribute(sciDummy, "SEARCHINTARGET", str_to_find);
            st2 = IupGetInt(sciDummy, "TARGETSTART");
            ed2 = IupGetInt(sciDummy, "TARGETEND");
            if (st2 == st1 && ed2 == ed1)
              continue;
          }
          open_file(projectTree, filename, 1);

          multitext = iScintillaDlgGetCurrentMultitext(ih);

          found = searchInFile(multitext, find_txt, 0, IupGetInt(sciDummy, "COUNT"), wrap, down, casesensitive, whole_word, regexp, posix);

          if (found)
          {
            if ((Ihandle*)IupGetAttribute(tabs, "VALUE_HANDLE") != multitext)
              IupSetAttribute(tabs, "VALUE_HANDLE", (char *)multitext);
            break;
          }
        }
      }

      if (!found)
      {
        Ihandle* multitext = iScintillaDlgGetCurrentMultitext(ih);
        /* update statusbar */
        Ihandle *lbl_statusbar = IupGetDialogChild(multitext, "STATUSBAR");
        IupSetfAttribute(lbl_statusbar, "TITLE", "Text \"%s\" not found.", str_to_find);
      }
    }
  }

  return IUP_DEFAULT;
}

static int find_replace_action_cb(Ihandle* bt_replace)
{
  Ihandle* find_dlg = (Ihandle*)IupGetAttribute(bt_replace, "FIND_DIALOG");
  Ihandle* ih = IupGetAttributeHandle(find_dlg, "PARENTDIALOG");
  Ihandle* multitext = iScintillaDlgGetCurrentMultitext(ih);
  char* selectionpos = IupGetAttribute(multitext, "SELECTIONPOS");

  if (!selectionpos)
    find_next_action_cb(bt_replace);
  else
  {
    Ihandle* replace_txt = IupGetDialogChild(find_dlg, "REPLACE_TEXT");
    char* str_to_replace = IupGetAttribute(replace_txt, "VALUE");
    if (IupGetAttribute(multitext, "SELECTIONPOS"))
    {
      int init, fim;
      IupGetIntInt(multitext, "SELECTIONPOS", &init, &fim);
      IupSetInt(multitext, "TARGETSTART", init);
      IupSetInt(multitext, "TARGETEND", fim);
      IupSetAttribute(multitext, "REPLACETARGET", str_to_replace);
    }

    /* then find next */
    find_next_action_cb(bt_replace);
  }

  return IUP_DEFAULT;
}

static int find_all_action_cb(Ihandle* bt_replace)
{
  Ihandle* find_dlg = (Ihandle*)IupGetAttribute(bt_replace, "FIND_DIALOG");
  if (find_dlg)
  {
    char* str_to_find;
    Ihandle* ih = IupGetAttributeHandle(find_dlg, "PARENTDIALOG");
    Ihandle* tabs = IupGetDialogChild(ih, "TABS");
    Ihandle* projectTree = IupGetDialogChild(ih, "PROJECTTREE");
    Ihandle* panelTabs = IupGetDialogChild(ih, "PANEL_TABS");
    Ihandle* listSearch = IupGetDialogChild(ih, "LIST_SEARCH");
    Ihandle* panelFrame = IupGetDialogChild(ih, "PANEL_FRAME");
    Ihandle* currentMultitext = iScintillaDlgGetCurrentMultitext(ih);
    Ihandle* multitext = NULL;
    Ihandle* find_txt = IupGetDialogChild(find_dlg, "FIND_TEXT");
    int i, count;

    IupSetAttribute(listSearch, "REMOVEITEM", "ALL");
    IupSetAttribute(panelTabs, "VALUEPOS", "0");

    /* test again, because it can be called from the hot key */
    str_to_find = IupGetAttribute(find_txt, "VALUE");
    if (str_to_find && str_to_find[0] != 0)
    {
      char flags[80];
      int find_start, find_end;
      int pos_start, pos_end;

      int searchIn = IupGetInt(IupGetDialogChild(find_dlg, "LST_SEARCH_IN"), "VALUE");
      int casesensitive = IupGetInt(IupGetDialogChild(find_dlg, "FIND_CASE"), "VALUE");
      int whole_word = IupGetInt(IupGetDialogChild(find_dlg, "WHOLE_WORD"), "VALUE");
      int regexp = IupGetInt(IupGetDialogChild(find_dlg, "REG_EXP"), "VALUE");
      int posix = IupGetInt(IupGetDialogChild(find_dlg, "POSIX"), "VALUE");

      flags[0] = 0;
      if (casesensitive)
        strcpy(flags, "MATCHCASE");
      if (whole_word)
        strcat((flags[0] != 0 ? strcat(flags, " | ") : flags), "WHOLEWORD");
      if (regexp)
        strcat((flags[0] != 0 ? strcat(flags, " | ") : flags), "REGEXP");
      if (posix)
        strcat((flags[0] != 0 ? strcat(flags, " | ") : flags), "POSIX");

      if (searchIn == 1)
        count = 1;
      else if (searchIn == 2)
        count = IupGetInt(tabs, "COUNT");
      else
        count = IupGetIntId(projectTree, "CHILDCOUNT", 0);

      for (i = 0; i < count; i++)
      {
        if (searchIn == 1)
          multitext = currentMultitext;
        else if (searchIn == 2)
          multitext = IupGetChild(tabs, i);
        else
        {
          char *filename = IupTreeGetUserId(projectTree, i+1);
          char* str = readFile(filename);
          if (str)
          {
            multitext = IupGetDialogChild(find_dlg, "SCI_DUMMY");
            IupSetStrAttribute(multitext, "FILENAME", filename);
            IupSetStrAttribute(multitext, "VALUE", str);
          }
        }

        if (flags[0] != 0)
          IupSetAttribute(multitext, "SEARCHFLAGS", flags);
        else
          IupSetAttribute(multitext, "SEARCHFLAGS", NULL);

        find_start = 0;
        find_end = IupGetInt(multitext, "COUNT");

        IupSetInt(multitext, "TARGETSTART", find_start);
        IupSetInt(multitext, "TARGETEND", find_end);

        str_to_find = IupGetAttribute(find_txt, "VALUE");
        IupSetAttribute(multitext, "SEARCHINTARGET", str_to_find);

        pos_start = IupGetInt(multitext, "TARGETSTART");
        pos_end = IupGetInt(multitext, "TARGETEND");

        while (find_start != pos_start || find_end != pos_end)
        {
          int lin, col;
          int count;
          char *filename = IupGetAttribute(multitext, "FILENAME");

          IupTextConvertPosToLinCol(multitext, pos_start, &lin, &col);

          IupSetStrf(listSearch, "APPENDITEM", "%s(%d): %s", filename, lin+1, IupGetAttributeId(multitext, "LINE", lin));

          count = IupGetInt(listSearch, "COUNT");

          IupSetStrAttributeId(listSearch, "FILENAME", count, filename);
          IupSetIntId(listSearch, "LINE", count, lin);
          IupSetIntId(listSearch, "COL", count, col);
          IupSetIntId(listSearch, "POSSTART", count, pos_start);
          IupSetIntId(listSearch, "POSEND", count, pos_end);

          find_start = IupGetInt(multitext, "TARGETEND");
          find_end = IupGetInt(multitext, "COUNT");

          IupSetInt(multitext, "TARGETSTART", find_start);
          IupSetInt(multitext, "TARGETEND", find_end);

          str_to_find = IupGetAttribute(find_txt, "VALUE");
          IupSetAttribute(multitext, "SEARCHINTARGET", str_to_find);

          pos_start = IupGetInt(multitext, "TARGETSTART");
          pos_end = IupGetInt(multitext, "TARGETEND");
        }

        if (multitext->brother)
          multitext = multitext->brother;
        else
          multitext = tabs->firstchild;
      }

      IupSetStrf(panelFrame, "TITLE", "Find Results: (%s)", str_to_find);
    }
    else
      IupSetAttribute(panelFrame, "TITLE", "Find Results:");
  }

  return IUP_DEFAULT;
}

static int find_replace_all_action_cb(Ihandle* bt_replace)
{
  Ihandle* find_dlg = (Ihandle*)IupGetAttribute(bt_replace, "FIND_DIALOG");
  if (find_dlg)
  {
    char* str_to_find;
    char* str_to_replace;
    Ihandle* ih = IupGetAttributeHandle(find_dlg, "PARENTDIALOG");
    Ihandle* tabs = IupGetDialogChild(ih, "TABS");
    Ihandle* currentMultitext = iScintillaDlgGetCurrentMultitext(ih);
    Ihandle* multitext = currentMultitext;
    Ihandle* replace_txt = IupGetDialogChild(find_dlg, "REPLACE_TEXT");
    Ihandle* find_txt = IupGetDialogChild(find_dlg, "FIND_TEXT");
    int count = IupGetInt(tabs, "COUNT");
    int i;

    /* test again, because it can be called from the hot key */
    str_to_find = IupGetAttribute(find_txt, "VALUE");
    if (str_to_find && str_to_find[0] != 0)
    {
      char flags[80];
      int find_start, find_end;
      int pos_start, pos_end;

      int searchIn = IupGetInt(IupGetDialogChild(find_dlg, "LST_SEARCH_IN"), "VALUE");
      int casesensitive = IupGetInt(IupGetDialogChild(find_dlg, "FIND_CASE"), "VALUE");
      int whole_word = IupGetInt(IupGetDialogChild(find_dlg, "WHOLE_WORD"), "VALUE");
      int regexp = IupGetInt(IupGetDialogChild(find_dlg, "REG_EXP"), "VALUE");
      int posix = IupGetInt(IupGetDialogChild(find_dlg, "POSIX"), "VALUE");

      flags[0] = 0;
      if (casesensitive)
        strcpy(flags, "MATCHCASE");
      if (whole_word)
        strcat((flags[0] != 0 ? strcat(flags, " | ") : flags), "WHOLEWORD");
      if (regexp)
        strcat((flags[0] != 0 ? strcat(flags, " | ") : flags), "REGEXP");
      if (posix)
        strcat((flags[0] != 0 ? strcat(flags, " | ") : flags), "POSIX");

      if (searchIn == 1)
        count = 1;

      for (i = 0; i < count; i++)
      {
        if (flags[0] != 0)
          IupSetAttribute(multitext, "SEARCHFLAGS", flags);
        else
          IupSetAttribute(multitext, "SEARCHFLAGS", NULL);

        find_start = 0;
        find_end = IupGetInt(multitext, "COUNT");

        IupSetInt(multitext, "TARGETSTART", find_start);
        IupSetInt(multitext, "TARGETEND", find_end);

        str_to_find = IupGetAttribute(find_txt, "VALUE");
        IupSetAttribute(multitext, "SEARCHINTARGET", str_to_find);

        pos_start = IupGetInt(multitext, "TARGETSTART");
        pos_end = IupGetInt(multitext, "TARGETEND");

        IupSetAttribute(multitext, "UNDOACTION", "BEGIN");

        while (find_start != pos_start || find_end != pos_end)
        {
          str_to_replace = IupGetAttribute(replace_txt, "VALUE");
          IupSetAttribute(multitext, "REPLACETARGET", str_to_replace);

          find_start = IupGetInt(multitext, "TARGETEND");
          find_end = IupGetInt(multitext, "COUNT");

          IupSetInt(multitext, "TARGETSTART", find_start);
          IupSetInt(multitext, "TARGETEND", find_end);

          str_to_find = IupGetAttribute(find_txt, "VALUE");
          IupSetAttribute(multitext, "SEARCHINTARGET", str_to_find);

          pos_start = IupGetInt(multitext, "TARGETSTART");
          pos_end = IupGetInt(multitext, "TARGETEND");
        }

        IupSetAttribute(multitext, "UNDOACTION", "END");

        if (multitext->brother)
          multitext = multitext->brother;
        else
          multitext = tabs->firstchild;
      }
    }
  }

  return IUP_DEFAULT;
}

static int find_close_action_cb(Ihandle* bt_close)
{
  Ihandle* find_dlg = IupGetDialog(bt_close);
  Ihandle* find_text = IupGetDialogChild(find_dlg, "FIND_TEXT");
  Ihandle* replace_text = IupGetDialogChild(find_dlg, "REPLACE_TEXT");
  Ihandle* find_case = IupGetDialogChild(find_dlg, "FIND_CASE");
  Ihandle* whole_word = IupGetDialogChild(find_dlg, "WHOLE_WORD");
  Ihandle* wrap = IupGetDialogChild(find_dlg, "WRAP");
  Ihandle* searchMode = IupGetDialogChild(find_dlg, "SEARCH_RADIO");
  Ihandle* dirMode = IupGetDialogChild(find_dlg, "DIRECTION_RADIO");
  Ihandle* ih = IupGetAttributeHandle(find_dlg, "PARENTDIALOG");
  Ihandle* config = iScintillaDlgGetConfig(ih);

  IupConfigSetVariableStr(config, "ScintillaFind", "FindText", IupGetAttribute(find_text, "VALUE"));
  IupConfigSetVariableStr(config, "ScintillaFind", "ReplaceText", IupGetAttribute(replace_text, "VALUE"));
  IupConfigSetVariableStr(config, "ScintillaFind", "FindCase", IupGetAttribute(find_case, "VALUE"));
  IupConfigSetVariableStr(config, "ScintillaFind", "WholeWord", IupGetAttribute(whole_word, "VALUE"));
  IupConfigSetVariableStr(config, "ScintillaFind", "Wrap", IupGetAttribute(wrap, "VALUE"));
  IupConfigSetVariableStr(config, "ScintillaFind", "SearchMode", IupGetAttribute((Ihandle*)IupGetAttribute(searchMode, "VALUE_HANDLE"), "NAME"));
  IupConfigSetVariableStr(config, "ScintillaFind", "SearchDir", IupGetAttribute((Ihandle*)IupGetAttribute(dirMode, "VALUE_HANDLE"), "NAME"));

  IupConfigDialogClosed(config, find_dlg, "ScintillaFind");

  IupHide(find_dlg);  /* do not destroy, just hide */
  return IUP_DEFAULT;
}

static Ihandle* create_find_dialog(Ihandle* ih_item)
{
  Ihandle *box, *bt_next, *bt_findall, *bt_close, *txt, *lst_search_in, *find_dlg;
  Ihandle *find_case, *whole_word, *mode, *normal, *reg_exp, *posix, *wrap, *up, *down;
  Ihandle *flags, *direction, *searchRadio, *directionRadio;
  Ihandle *txt_replace, *bt_replace, *bt_replace_all;
  Ihandle* config = iScintillaDlgGetConfig(ih_item);
  Ihandle *sci_dummy;
  const char* value;

  txt = IupText(NULL);
  IupSetAttribute(txt, "NAME", "FIND_TEXT");
  IupSetAttribute(txt, "EXPAND", "HORIZONTAL");
  txt_replace = IupText(NULL);
  IupSetAttribute(txt_replace, "NAME", "REPLACE_TEXT");
  IupSetAttribute(txt_replace, "EXPAND", "HORIZONTAL");
  lst_search_in = IupList(NULL);
  IupSetAttribute(lst_search_in, "NAME", "LST_SEARCH_IN");
  IupSetAttribute(lst_search_in, "EXPAND", "HORIZONTAL");
  IupSetAttribute(lst_search_in, "DROPDOWN", "YES");
  IupSetAttribute(lst_search_in, "1", "Current Document");
  IupSetAttribute(lst_search_in, "2", "All Open Documents");
  IupSetAttribute(lst_search_in, "VALUE", "1");
  IupSetCallback(lst_search_in, "ACTION", (Icallback)lst_search_in_action_cb);
  find_case = IupToggle("Match Case", NULL);
  IupSetAttribute(find_case, "NAME", "FIND_CASE");
  whole_word = IupToggle("Match Whole Word", NULL);
  IupSetAttribute(whole_word, "NAME", "WHOLE_WORD");
  normal = IupToggle("Normal", NULL);
  IupSetAttribute(normal, "NAME", "NORMAL");
  reg_exp = IupToggle("Reg. Expression", NULL);
  IupSetAttribute(reg_exp, "NAME", "REG_EXP");
  posix = IupToggle("Posix Reg. Expr.", NULL);
  IupSetAttribute(posix, "NAME", "POSIX");
  wrap = IupToggle("Wrap Around", NULL);
  IupSetAttribute(wrap, "NAME", "WRAP");
  up = IupToggle("Up", NULL);
  IupSetAttribute(up, "NAME", "UP");
  down = IupToggle("Down", NULL);
  IupSetAttribute(down, "NAME", "DOWN");
  bt_next = IupButton("Find Next", NULL);
  IupSetAttribute(bt_next, "PADDING", "10x2");
  IupSetCallback(bt_next, "ACTION", (Icallback)find_next_action_cb);
  bt_findall = IupButton("Find All", NULL);
  IupSetAttribute(bt_findall, "PADDING", "10x2");
  IupSetCallback(bt_findall, "ACTION", (Icallback)find_all_action_cb);
  bt_replace = IupButton("Replace", NULL);
  IupSetAttribute(bt_replace, "PADDING", "10x2");
  IupSetCallback(bt_replace, "ACTION", (Icallback)find_replace_action_cb);
  IupSetAttribute(bt_replace, "NAME", "REPLACE_BUTTON");
  bt_replace_all = IupButton("Replace All", NULL);
  IupSetAttribute(bt_replace_all, "PADDING", "10x2");
  IupSetCallback(bt_replace_all, "ACTION", (Icallback)find_replace_all_action_cb);
  IupSetAttribute(bt_replace_all, "NAME", "REPLACE_ALL_BUTTON");
  bt_close = IupButton("Close", NULL);
  IupSetCallback(bt_close, "ACTION", (Icallback)find_close_action_cb);
  IupSetAttribute(bt_close, "PADDING", "10x2");

  flags = IupVbox(find_case,
                  whole_word,
                  wrap,
                  NULL);

  searchRadio = IupRadio(IupVbox(normal, reg_exp, posix, NULL));
  IupSetAttribute(searchRadio, "MARGIN", "10x10");
  IupSetAttribute(searchRadio, "VALUE_HANDLE", (char*)normal);
  IupSetAttribute(searchRadio, "NAME", "SEARCH_RADIO");
  mode = IupFrame(searchRadio);
  IupSetAttribute(mode, "TITLE", "Search Mode");

  directionRadio = IupRadio(IupVbox(up, down, NULL));
  IupSetAttribute(directionRadio, "MARGIN", "10x10");
  IupSetAttribute(directionRadio, "VALUE_HANDLE", (char*)down);
  IupSetAttribute(directionRadio, "NAME", "DIRECTION_RADIO");
  direction = IupFrame(directionRadio);
  IupSetAttribute(direction, "TITLE", "Direction");

  box = IupVbox(
    IupLabel("Find What:"),
    txt,
    IupSetAttributes(IupLabel("Replace with:"), "NAME=REPLACE_LABEL"),
    txt_replace,
    IupLabel("Search in:"),
    lst_search_in,
    IupHbox(
    flags,
    direction,
    mode,
    NULL),
    IupSetAttributes(IupLabel(NULL), "SEPARATOR=HORIZONTAL"),
    IupSetAttributes(IupHbox(
    IupFill(),
    bt_next,
    bt_findall,
    bt_replace,
    bt_replace_all,
    bt_close,
    NULL), "NORMALIZESIZE=HORIZONTAL"),
    NULL);
  IupSetAttribute(box, "NMARGIN", "10x10");
  IupSetAttribute(box, "GAP", "10");

  sci_dummy = IupScintilla();
  IupSetAttribute(sci_dummy, "NAME", "SCI_DUMMY");
  IupSetAttribute(sci_dummy, "VISIBLE", "NO");
  IupSetAttribute(sci_dummy, "EXPAND", "NO");

  find_dlg = IupDialog(IupZbox(box, sci_dummy, NULL));
  IupSetAttribute(find_dlg, "TITLE", "Find");
  IupSetAttribute(find_dlg, "DIALOGFRAME", "Yes");
  IupSetAttributeHandle(find_dlg, "DEFAULTENTER", bt_next);
  IupSetAttributeHandle(find_dlg, "DEFAULTESC", bt_close);
  IupSetAttributeHandle(find_dlg, "PARENTDIALOG", IupGetDialog(ih_item));
  IupSetCallback(find_dlg, "CLOSE_CB", (Icallback)find_close_action_cb);

  /* Save the dialog to reuse it */
  IupSetAttribute(find_dlg, "FIND_DIALOG", (char*)find_dlg);  /* from itself */
  IupSetAttribute(IupGetDialog(ih_item), "FIND_DIALOG", (char*)find_dlg); /* from the main dialog */

  IupMap(find_dlg);

  value = IupConfigGetVariableStr(config, "ScintillaFind", "FindText");
  if (value)
    IupSetStrAttribute(txt, "VALUE", value);

  value = IupConfigGetVariableStr(config, "ScintillaFind", "ReplaceText");
  if (value)
    IupSetStrAttribute(txt_replace, "VALUE", value);

  value = IupConfigGetVariableStr(config, "ScintillaFind", "FindCase");
  if (value)
    IupSetStrAttribute(find_case, "VALUE", value);

  value = IupConfigGetVariableStr(config, "ScintillaFind", "WholeWord");
  if (value)
    IupSetStrAttribute(whole_word, "VALUE", value);

  value = IupConfigGetVariableStr(config, "ScintillaFind", "Wrap");
  if (value)
    IupSetStrAttribute(wrap, "VALUE", value);

  value = IupConfigGetVariableStr(config, "ScintillaFind", "SearchMode");
  if (value)
  {
    Ihandle* search_mode = IupGetDialogChild(find_dlg, value);
    if (search_mode)
      IupSetAttribute(searchRadio, "VALUE_HANDLE", (char*)search_mode);
  }

  value = IupConfigGetVariableStr(config, "ScintillaFind", "SearchDir");
  if (value)
  {
    Ihandle* search_dir = IupGetDialogChild(find_dlg, value);
    if (search_dir)
      IupSetAttribute(directionRadio, "VALUE_HANDLE", (char*)search_dir);
  }

  return find_dlg;
}

static int item_find_action_cb(Ihandle* ih_item)
{
  Ihandle* find_dlg = (Ihandle*)IupGetAttribute(ih_item, "FIND_DIALOG");
  Ihandle* multitext = iScintillaDlgGetCurrentMultitext(ih_item);
  Ihandle* config = iScintillaDlgGetConfig(ih_item);
  Ihandle* projectConfig = iScintillaDlgGetProjectConfig(ih_item);
  Ihandle* searchIn = NULL;
  char* str;

  if (!find_dlg)
    find_dlg = create_find_dialog(ih_item);

  searchIn = IupGetDialogChild(find_dlg, "LST_SEARCH_IN");

  if (projectConfig)
    IupSetAttribute(searchIn, "3", "Project Documents");
  else
    IupSetAttribute(searchIn, "3", NULL);

  set_find_replace_visibility(find_dlg, 0);

  IupConfigDialogShow(config, find_dlg, "ScintillaFind");

  str = IupGetAttribute(multitext, "SELECTEDTEXT");
  if (str && str[0] != 0)
  {
    Ihandle* txt = IupGetDialogChild(find_dlg, "FIND_TEXT");
    IupSetStrAttribute(txt, "VALUE", str);
  }

  return IUP_DEFAULT;
}

static int item_replace_action_cb(Ihandle* ih_item)
{
  Ihandle* find_dlg = (Ihandle*)IupGetAttribute(ih_item, "FIND_DIALOG");
  Ihandle* multitext = iScintillaDlgGetCurrentMultitext(ih_item);
  Ihandle* config = iScintillaDlgGetConfig(ih_item);
  char* str;

  if (!find_dlg)
    find_dlg = create_find_dialog(ih_item);

  set_find_replace_visibility(find_dlg, 1);

  IupConfigDialogShow(config, find_dlg, "ScintillaFind");

  str = IupGetAttribute(multitext, "SELECTEDTEXT");
  if (str && str[0] != 0)
  {
    Ihandle* txt = IupGetDialogChild(find_dlg, "FIND_TEXT");
    IupSetStrAttribute(txt, "VALUE", str);
  }

  return IUP_IGNORE;  /* replace system processing for the hot key */
}

static int selection_find_next_action_cb(Ihandle* ih)
{
  Ihandle* multitext = iScintillaDlgGetCurrentMultitext(ih);

  char* str = IupGetAttribute(multitext, "SELECTEDTEXT");
  if (str && str[0] != 0)
  {
    Ihandle* txt;
    Ihandle* find_dlg = (Ihandle*)IupGetAttribute(ih, "FIND_DIALOG");

    if (!find_dlg)
      find_dlg = create_find_dialog(multitext);

    txt = IupGetDialogChild(find_dlg, "FIND_TEXT");
    IupSetStrAttribute(txt, "VALUE", str);

    find_next_action_cb(ih);
  }

  return IUP_DEFAULT;
}

static int item_copy_action_cb(Ihandle* ih_item)
{
  Ihandle* multitext = iScintillaDlgGetCurrentMultitext(ih_item);
  IupSetAttribute(multitext, "CLIPBOARD", "COPY");
  return IUP_DEFAULT;
}

static int item_paste_action_cb(Ihandle* ih_item)
{
  Ihandle* multitext = iScintillaDlgGetCurrentMultitext(ih_item);
  IupSetAttribute(multitext, "CLIPBOARD", "PASTE");
  return IUP_IGNORE;  /* replace system processing for the hot key, to correctly parse line feed */
}

static int item_cut_action_cb(Ihandle* ih_item)
{
  Ihandle* multitext = iScintillaDlgGetCurrentMultitext(ih_item);
  IupSetAttribute(multitext, "CLIPBOARD", "CUT");
  return IUP_DEFAULT;
}

static int item_delete_action_cb(Ihandle* ih_item)
{
  Ihandle* multitext = iScintillaDlgGetCurrentMultitext(ih_item);
  IupSetAttribute(multitext, "CLIPBOARD", "CLEAR");
  return IUP_DEFAULT;
}

static int item_select_all_action_cb(Ihandle* ih_item)
{
  int count;
  Ihandle* multitext = iScintillaDlgGetCurrentMultitext(ih_item);
  IupSetFocus(multitext);
  count = IupGetInt(multitext, "COUNT");
  IupSetStrf(multitext, "SELECTIONPOS", "%d:%d", 0, count - 1);
  return IUP_DEFAULT;
}

static int item_undo_action_cb(Ihandle* ih_item)
{
  Ihandle* multitext = iScintillaDlgGetCurrentMultitext(ih_item);
  IupSetAttribute(multitext, "UNDO", "YES");
  return IUP_DEFAULT;
}

static int item_redo_action_cb(Ihandle* ih_item)
{
  Ihandle* multitext = iScintillaDlgGetCurrentMultitext(ih_item);
  IupSetAttribute(multitext, "REDO", "YES");
  return IUP_DEFAULT;
}

static int item_uppercase_action_cb(Ihandle* ih_item)
{
  char *text;
  int start, end;
  Ihandle* multitext = iScintillaDlgGetCurrentMultitext(ih_item);
  IupGetIntInt(multitext, "SELECTIONPOS", &start, &end);
  text = IupGetAttribute(multitext, "SELECTEDTEXT");
  text = iupStrDup(text);
  iupStrUpper(text, text);
  IupSetAttribute(multitext, "SELECTEDTEXT", text);
  IupSetStrf(multitext, "SELECTIONPOS", "%d:%d", start, end);
  free(text);
  return IUP_DEFAULT;
}

static int item_lowercase_action_cb(Ihandle* ih_item)
{
  char *text;
  int start, end;
  Ihandle* multitext = iScintillaDlgGetCurrentMultitext(ih_item);
  IupGetIntInt(multitext, "SELECTIONPOS", &start, &end);
  text = IupGetAttribute(multitext, "SELECTEDTEXT");
  text = iupStrDup(text);
  iupStrLower(text, text);
  IupSetAttribute(multitext, "SELECTEDTEXT", text);
  IupSetStrf(multitext, "SELECTIONPOS", "%d:%d", start, end);
  free(text);
  return IUP_DEFAULT;
}

static int item_case_action_cb(Ihandle* ih_item)
{
  char* shift = IupGetGlobal("SHIFTKEY");

  if (strcmp(shift, "ON") == 0)
    item_uppercase_action_cb(ih_item);
  else
    item_lowercase_action_cb(ih_item);

  return IUP_DEFAULT;
}

static int item_font_action_cb(Ihandle* ih_item)
{
  Ihandle* tabs = IupGetDialogChild(ih_item, "TABS");
  Ihandle* multitext;
  Ihandle* config = iScintillaDlgGetConfig(ih_item);
  Ihandle* fontdlg = IupFontDlg();

  const char* font = IupConfigGetVariableStr(config, "ScintillaFormat", "Font");

  IupSetStrAttribute(fontdlg, "VALUE", font);
  IupSetAttributeHandle(fontdlg, "PARENTDIALOG", IupGetDialog(ih_item));

  IupPopup(fontdlg, IUP_CENTERPARENT, IUP_CENTERPARENT);

  if (IupGetInt(fontdlg, "STATUS") == 1)
  {
    font = IupGetAttribute(fontdlg, "VALUE");

    for (multitext = tabs->firstchild; multitext; multitext = multitext->brother)
      IupSetStrAttribute(multitext, "FONT", font);

    IupConfigSetVariableStr(config, "ScintillaFormat", "Font", font);
  }

  IupDestroy(fontdlg);
  return IUP_DEFAULT;
}

static int item_tab_action_cb(Ihandle* ih_item)
{
  Ihandle* tabs = IupGetDialogChild(ih_item, "TABS");
  Ihandle* multitext;
  Ihandle* config = iScintillaDlgGetConfig(ih_item);

  int replaceBySpace = !IupConfigGetVariableIntDef(config, "ScintillaFormat", "UseTabs", 1);
  int tabSize = IupConfigGetVariableIntDef(config, "ScintillaFormat", "TabSize", 8);

  if (IupGetParam("Tab Settings", setparent_param_cb, IupGetDialog(ih_item),
    "Size: %i[1,]\n"
    "Replace by Whitespace: %b\n",
    &tabSize, &replaceBySpace, NULL))
  {

    for (multitext = tabs->firstchild; multitext; multitext = multitext->brother)
    {
      IupSetInt(multitext, "TABSIZE", tabSize);
      IupSetInt(multitext, "USETABS", !replaceBySpace);
    }

    IupConfigSetVariableInt(config, "ScintillaFormat", "TabSize", tabSize);
    IupConfigSetVariableInt(config, "ScintillaFormat", "UseTabs", !replaceBySpace);
  }

  return IUP_DEFAULT;
}

static int item_zoomin_action_cb(Ihandle* ih_item)
{
  Ihandle* multitext = iScintillaDlgGetCurrentMultitext(ih_item);

  IupSetAttribute(multitext, "ZOOMIN", "10");

  return IUP_DEFAULT;
}

static int item_zoomout_action_cb(Ihandle* ih_item)
{
  Ihandle* multitext = iScintillaDlgGetCurrentMultitext(ih_item);

  IupSetAttribute(multitext, "ZOOMOUT", "10");

  return IUP_IGNORE;  /* to avoid garbage in Scintilla when pressing the hot key */
}

static int item_restorezoom_action_cb(Ihandle* ih_item)
{
  Ihandle* multitext = iScintillaDlgGetCurrentMultitext(ih_item);

  IupSetAttribute(multitext, "ZOOM", "0");

  return IUP_DEFAULT;
}

static int item_wordwrap_action_cb(Ihandle* ih_item)
{
  Ihandle* tabs = IupGetDialogChild(ih_item, "TABS");
  Ihandle* multitext;
  Ihandle* config = iScintillaDlgGetConfig(ih_item);
  char *value = IupGetAttribute(ih_item, "VALUE");

  for (multitext = tabs->firstchild; multitext; multitext = multitext->brother)
  {
    if (iupStrBoolean(value))
      IupSetAttribute(multitext, "WORDWRAP", "WORD");
    else
      IupSetAttribute(multitext, "WORDWRAP", "NONE");
  }

  IupConfigSetVariableStr(config, "ScintillaView", "WordWrap", value);

  return IUP_DEFAULT;
}

static int item_showwhite_action_cb(Ihandle* ih_item)
{
  Ihandle* tabs = IupGetDialogChild(ih_item, "TABS");
  Ihandle* multitext;
  Ihandle* config = iScintillaDlgGetConfig(ih_item);
  char *value = IupGetAttribute(ih_item, "VALUE");

  for (multitext = tabs->firstchild; multitext; multitext = multitext->brother)
  {
    if (iupStrBoolean(value))
      IupSetAttribute(multitext, "WHITESPACEVIEW", "VISIBLEALWAYS");
    else
      IupSetAttribute(multitext, "WHITESPACEVIEW", "INVISIBLE");
  }

  IupConfigSetVariableStr(config, "ScintillaView", "ShowWhite", value);
  return IUP_DEFAULT;
}

static int item_panel_action_cb(Ihandle* ih_item)
{
  Ihandle* panelSplit = IupGetDialogChild(ih_item, "PANEL_SPLIT");

  if (IupGetInt(panelSplit, "VALUE") == 1000)
    IupSetAttribute(panelSplit, "VALUE", "800");
  else
    IupSetAttribute(panelSplit, "VALUE", "1000");

  return IUP_DEFAULT;
}

static int item_showeol_action_cb(Ihandle* ih_item)
{
  Ihandle* tabs = IupGetDialogChild(ih_item, "TABS");
  Ihandle* multitext;
  Ihandle* config = iScintillaDlgGetConfig(ih_item);
  char *value = IupGetAttribute(ih_item, "VALUE");

  for (multitext = tabs->firstchild; multitext; multitext = multitext->brother)
  {
    if (iupStrBoolean(value))
      IupSetAttribute(multitext, "EOLVISIBLE", "YES");
    else
      IupSetAttribute(multitext, "EOLVISIBLE", "NO");
  }

  IupConfigSetVariableStr(config, "ScintillaView", "ShowEol", value);
  return IUP_DEFAULT;
}

static int item_toolbar_action_cb(Ihandle* ih_item)
{
  Ihandle* tabs = IupGetDialogChild(ih_item, "TABS");
  Ihandle* toolbar = IupGetChild(IupGetParent(tabs), 0);
  Ihandle* config = iScintillaDlgGetConfig(tabs);

  toggle_bar_visibility(ih_item, toolbar);

  IupConfigSetVariableStr(config, "ScintillaView", "Toolbar", IupGetAttribute(ih_item, "VALUE"));
  return IUP_DEFAULT;
}

static int item_statusbar_action_cb(Ihandle* ih_item)
{
  Ihandle* tabs = IupGetDialogChild(ih_item, "TABS");
  Ihandle* statusbar = IupGetBrother(tabs);
  Ihandle* config = iScintillaDlgGetConfig(tabs);

  toggle_bar_visibility(ih_item, statusbar);

  IupConfigSetVariableStr(config, "ScintillaView", "Statusbar", IupGetAttribute(ih_item, "VALUE"));
  return IUP_DEFAULT;
}

static int item_linenumber_action_cb(Ihandle* ih_item)
{
  Ihandle* tabs = IupGetDialogChild(ih_item, "TABS");
  Ihandle* multitext;
  Ihandle* config = iScintillaDlgGetConfig(ih_item);
  char *value = IupGetAttribute(ih_item, "VALUE");

  for (multitext = tabs->firstchild; multitext; multitext = multitext->brother)
  {
    if (iupStrBoolean(value))
      IupSetAttribute(multitext, "MARGINWIDTH0", LINENUMBER_MARGIN);
    else
      IupSetAttribute(multitext, "MARGINWIDTH0", "0");
  }

  IupConfigSetVariableStr(config, "ScintillaView", "LineNumber", value);
  return IUP_DEFAULT;
}

static int item_bookmark_action_cb(Ihandle* ih_item)
{
  Ihandle* tabs = IupGetDialogChild(ih_item, "TABS");
  Ihandle* multitext;
  Ihandle* config = iScintillaDlgGetConfig(ih_item);
  char *value = IupGetAttribute(ih_item, "VALUE");

  for (multitext = tabs->firstchild; multitext; multitext = multitext->brother)
  {
    if (iupStrBoolean(value))
      IupSetAttribute(multitext, "MARGINWIDTH1", BOOKMARK_MARGIN);
    else
      IupSetAttribute(multitext, "MARGINWIDTH1", "0");
  }

  IupConfigSetVariableStr(config, "ScintillaView", "Bookmark", value);
  return IUP_DEFAULT;
}

static int item_windowN_action_cb(Ihandle* ih_item)
{
  Ihandle* tabs = IupGetDialogChild(ih_item, "TABS");
  int pos = IupGetChildPos(IupGetParent(ih_item), ih_item);
  IupSetInt(tabs, "VALUEPOS", pos);
  return IUP_DEFAULT;
}

static int item_windows_action_cb(Ihandle* ih_item)
{
  Ihandle* tabs = IupGetDialogChild(ih_item, "TABS");
  int pos = IupGetInt(tabs, "VALUEPOS");
  Ihandle* multitext;
  int ret, count = 0, max_lin;
  char* win_list_str[300];

  for (multitext = tabs->firstchild; multitext; multitext = multitext->brother)
  {
    char* filename = IupGetAttribute(multitext, "FILENAME");
    win_list_str[count] = filename;
    count++;
  }

  IupStoreGlobal("_IUP_OLD_PARENTDIALOG", IupGetGlobal("PARENTDIALOG"));
  IupSetAttributeHandle(NULL, "PARENTDIALOG", IupGetDialog(tabs));

  max_lin = count < 15 ? count + 1 : 15;

  ret = IupListDialog(1, "Windows", count, (const char**)win_list_str, pos + 1, 30, max_lin, NULL);

  IupStoreGlobal("PARENTDIALOG", IupGetGlobal("_IUP_OLD_PARENTDIALOG"));
  IupSetGlobal("_IUP_OLD_PARENTDIALOG", NULL);

  if (ret != -1)
    IupSetInt(tabs, "VALUEPOS", ret);

  return IUP_DEFAULT;
}


/********************************** Attributes *****************************************/


static void iScintillaDlgSetConfigMultitext(Ihandle* ih, Ihandle* config, Ihandle* multitext)
{
  const char* value;

  value = IupConfigGetVariableStr(config, "ScintillaFormat", "Font");
  if (value)
    IupSetStrAttribute(multitext, "FONT", value);
  else
  {
    value = IupGetAttribute(multitext, "FONT");
    IupConfigSetVariableStr(config, "ScintillaFormat", "Font", value);
  }

  value = IupConfigGetVariableStr(config, "ScintillaFormat", "TabSize");
  if (value)
    IupSetStrAttribute(multitext, "TABSIZE", value);

  value = IupConfigGetVariableStr(config, "ScintillaFormat", "UseTabs");
  if (value)
    IupSetStrAttribute(multitext, "USETABS", value);

  value = IupConfigGetVariableStr(config, "ScintillaView", "WordWrap");
  if (value)
  {
    Ihandle* item_wordwrap = IupGetDialogChild(ih, "ITEM_WORDWRAP");
    IupSetAttribute(item_wordwrap, "VALUE", value);
    if (iupStrBoolean(value))
      IupSetAttribute(multitext, "WORDWRAP", "WORD");
    else
      IupSetAttribute(multitext, "WORDWRAP", "NONE");
  }

  value = IupConfigGetVariableStr(config, "ScintillaView", "ShowWhite");
  if (value)
  {
    Ihandle* item_showwhite = IupGetDialogChild(ih, "ITEM_SHOWWHITE");
    IupSetAttribute(item_showwhite, "VALUE", value);
    if (iupStrBoolean(value))
      IupSetAttribute(multitext, "WHITESPACEVIEW", "VISIBLEALWAYS");
    else
      IupSetAttribute(multitext, "WHITESPACEVIEW", "INVISIBLE");
  }

  value = IupConfigGetVariableStr(config, "ScintillaView", "ShowEol");
  if (value)
  {
    Ihandle* item_showeol = IupGetDialogChild(ih, "ITEM_SHOWEOL");
    IupSetAttribute(item_showeol, "VALUE", value);
    if (iupStrBoolean(value))
      IupSetAttribute(multitext, "EOLVISIBLE", "YES");
    else
      IupSetAttribute(multitext, "EOLVISIBLE", "NO");
  }

  value = IupConfigGetVariableStr(config, "ScintillaView", "LineNumber");
  if (value)
  {
    Ihandle* item_linenumber = IupGetDialogChild(ih, "ITEM_LINENUMBER");
    IupSetAttribute(item_linenumber, "VALUE", value);
    if (iupStrBoolean(value))
      IupSetAttribute(multitext, "MARGINWIDTH0", LINENUMBER_MARGIN);
    else
      IupSetAttribute(multitext, "MARGINWIDTH0", "0");
  }

  value = IupConfigGetVariableStr(config, "ScintillaView", "Bookmark");
  if (value)
  {
    Ihandle* item_bookmark = IupGetDialogChild(ih, "ITEM_BOOKMARK");
    IupSetAttribute(item_bookmark, "VALUE", value);
    if (iupStrBoolean(value))
      IupSetAttribute(multitext, "MARGINWIDTH1", BOOKMARK_MARGIN);
    else
      IupSetAttribute(multitext, "MARGINWIDTH1", "0");
  }
}

static void iScintillaDlgSetConfig(Ihandle* ih, Ihandle* config)
{
  IFnn cb = (IFnn)IupGetCallback(ih, "CONFIGLOAD_CB");
  Ihandle* recent_menu = (Ihandle*)iupAttribGet(ih, "_IUP_RECENTMENU");
  Ihandle* recent_proj_menu = (Ihandle*)iupAttribGet(ih, "_IUP_PROJ_RECENTMENU");
  Ihandle* tabs = IupGetDialogChild(ih, "TABS");
  Ihandle* multitext;
  const char* value;

  value = IupConfigGetVariableStr(config, "ScintillaView", "Toolbar");
  if (value && !iupStrBoolean(value))
  {
    Ihandle* item_toolbar = IupGetDialogChild(ih, "ITEM_TOOLBAR");
    Ihandle* toolbar = IupGetChild(IupGetParent(tabs), 0);
    /* default is visible */
    IupSetAttribute(toolbar, "FLOATING", "YES");
    IupSetAttribute(toolbar, "VISIBLE", "NO");
    IupSetAttribute(item_toolbar, "VALUE", "OFF");
  }

  value = IupConfigGetVariableStr(config, "ScintillaView", "Statusbar");
  if (value && !iupStrBoolean(value))
  {
    Ihandle* item_statusbar = IupGetDialogChild(ih, "ITEM_STATUSBAR");
    Ihandle* statusbar = IupGetBrother(tabs);
    /* default is visible */
    IupSetAttribute(statusbar, "FLOATING", "YES");
    IupSetAttribute(statusbar, "VISIBLE", "NO");
    IupSetAttribute(item_statusbar, "VALUE", "OFF");
  }

  for (multitext = tabs->firstchild; multitext; multitext = multitext->brother)
    iScintillaDlgSetConfigMultitext(ih, config, multitext);

  IupSetAttribute(config, "RECENTNAME", "ScintillaRecent");
  IupConfigRecentInit(config, recent_menu, config_recent_cb, 10);

  IupSetAttribute(config, "RECENTNAME", "ScintillaRecentProject");
  IupConfigRecentInit(config, recent_proj_menu, config_recent_proj_cb, 10);

  value = IupConfigGetVariableStr(config, IupGetAttribute(ih, "SUBTITLE"), "Split");
  if (value)
  {
    Ihandle* panelSplit = IupGetDialogChild(ih, "PANEL_SPLIT");
    IupSetStrAttribute(panelSplit, "VALUE", value);
  }

  if (cb)
    cb(ih, config);
}

static int iScintillaDlgSetConfigHandleAttrib(Ihandle* ih, const char* value)
{
  Ihandle *old_config, *config = (Ihandle*)value;
  if (!iupObjectCheck(config))
    return 0;

  /* only valid before map */
  if (ih->handle)
    return 0;

  old_config = iScintillaDlgGetConfig(ih);
  IupDestroy(old_config);

  iupAttribSet(ih, "_IUP_CONFIG", (char*)config);

  /* make sure it has at least one name */
  if (!iupAttribGetHandleName(config))
    iupAttribSetHandleName(config);

  iScintillaDlgSetConfig(ih, config);

  return 0;
}

static char* iScintillaDlgGetConfigHandleAttrib(Ihandle* ih)
{
  Ihandle* projectConfig = iScintillaDlgGetProjectConfig(ih);
  if (projectConfig)
    return (char*)projectConfig;
  else
  {
    Ihandle* config = iScintillaDlgGetConfig(ih);
    return (char*)config;
  }
}

static int iScintillaDlgSetConfigAttrib(Ihandle* ih, const char* value)
{
  Ihandle* config = IupGetHandle(value);
  iScintillaDlgSetConfigHandleAttrib(ih, (char*)config);
  return 0;
}

static char* iScintillaDlgGetConfigAttrib(Ihandle* ih)
{
  Ihandle* config = iScintillaDlgGetConfig(ih);
  return IupGetName(config);
}

static int iScintillaDlgSetSubTitleAttrib(Ihandle* ih, const char* value)
{
  Ihandle* multitext = iScintillaDlgGetCurrentMultitext(ih);
  iupAttribSetStr(ih, "SUBTITLE", value);
  update_dialog_title(multitext);
  return 1;
}

static int iScintillaDlgSetUpdateTitleAttrib(Ihandle* ih, const char* value)
{
  Ihandle* multitext = iScintillaDlgGetCurrentMultitext(ih);
  update_dialog_title(multitext);
  (void)value;
  return 0;
}

static int iScintillaDlgSetOpenFileAttrib(Ihandle* ih, const char* value)
{
  if (value)
  {
    if (!check_open(ih, value, 0))
      open_file(ih, value, 1);
  }
  else
    new_file(ih);
  return 0;
}

static int iScintillaDlgSetSaveFileAttrib(Ihandle* ih, const char* value)
{
  Ihandle* multitext = iScintillaDlgGetCurrentMultitext(ih);
  if (value)
  {
    if (!check_open(ih, value, 1))
      saveas_file(multitext, value);
  }
  else
    save_file(multitext);
  return 0;
}

static int iScintillaDlgSetForceCloseFileAttrib(Ihandle* ih, const char* value)
{
  int pos;
  if (iupStrToInt(value, &pos))
  {
    Ihandle* tabs = IupGetDialogChild(ih, "TABS");
    Ihandle* multitext = IupGetChild(tabs, pos);

    iScintillaDlgCloseMultitext(multitext, 0);

    if (IupGetChildCount(tabs) == 0)
      new_file(ih);  /* always keep at least one multitext */
  }
  return 0;
}

static int iScintillaDlgSetCloseFileAttrib(Ihandle* ih, const char* value)
{
  int pos;
  if (iupStrToInt(value, &pos))
  {
    Ihandle* tabs = IupGetDialogChild(ih, "TABS");
    Ihandle* multitext = IupGetChild(tabs, pos);

    iScintillaDlgCloseMultitext(multitext, 1);

    if (IupGetChildCount(tabs) == 0)
      new_file(ih);  /* always keep at least one multitext */
  }
  return 0;
}


static int iScintillaDlgSetToggleMarkerAttribId(Ihandle* ih, int id, const char* value)
{
  int margin;
  if (iupStrToInt(value, &margin))
  {
    Ihandle* multitext = iScintillaDlgGetCurrentMultitext(ih);
    toggleMarker(multitext, id, margin);
  }
  return 0;
}


/********************************** Main *****************************************/


static int iScintillaDlgCreateMethod(Ihandle* ih, void** params)
{
  Ihandle *vbox, *projectSplit, *tabs, *projectTree, *menu;
  Ihandle *sub_menu_file, *file_menu, *item_exit, *item_new, *item_open, *item_save, *item_saveas, *item_revert;
  Ihandle *sub_menu_edit, *edit_menu, *item_find, *item_find_next, *item_goto, *item_gotombrace, *item_copy, *item_paste, *item_cut, *item_delete, *item_select_all;
  Ihandle *sub_menu_project, *project_menu, *item_new_proj, *item_new_blank_proj, *item_open_proj, *item_save_proj, *item_save_proj_as,
    *item_close_proj, *item_add_new_file, *item_add_exist_file, *item_add_open_file, *item_open_proj_file, *item_open_all_proj_file, *item_remove_proj_file;
  Ihandle *item_togglemark, *item_nextmark, *item_previousmark, *item_clearmarks, *item_cutmarked, *item_copymarked, *item_pastetomarked, *item_removemarked,
    *item_invertmarks, *item_tabtospace, *item_allspacetotab, *item_leadingspacetotab;
  Ihandle *item_trimleading, *item_trimtrailing, *item_trimtraillead, *item_eoltospace, *item_eol_cr, *item_eol_crlf, *item_eol_lf, *item_removespaceeol;
  Ihandle *item_undo, *item_redo, *item_pagesetup, *item_print;
  Ihandle *case_menu, *item_uppercase, *item_lowercase;
  Ihandle *btn_cut, *btn_copy, *btn_paste, *btn_find, *btn_new, *btn_open, *btn_save;
  Ihandle *sub_menu_format, *format_menu, *item_font, *item_tab, *item_replace;
  Ihandle *sub_menu_view, *view_menu, *item_panel, *item_toolbar, *item_statusbar, *item_linenumber, *item_bookmark;
  Ihandle *zoom_menu, *item_zoomin, *item_zoomout, *item_restorezoom;
  Ihandle *item_savecopy, *item_saveall, *item_closeall, *item_close, *item_rename, *item_windows, *item_loadsession, *item_savesession;
  Ihandle *lbl_statusbar, *toolbar_hb, *recent_menu, *recent_proj_menu, *window_menu, *sub_menu_window, *item_window1;
  Ihandle *item_wordwrap, *item_showwhite, *item_showeol;
  Ihandle *panelFrame, *panelTabs, *listSearch, *panelSplit;

  tabs = IupFlatTabs(NULL);
  IupSetAttribute(tabs, "NAME", "TABS");
  IupSetAttribute(tabs, "SHOWCLOSE", "YES");
  IupSetAttribute(tabs, "TABCHANGEONCHECK", "YES");
  IupSetAttribute(tabs, "TABTITLE0", "Untitled");
  IupSetCallback(tabs, "TABCLOSE_CB", (Icallback)tabs_close_cb);
  IupSetCallback(tabs, "TABCHANGE_CB", (Icallback)tabs_change_cb);
  IupSetCallback(tabs, "RIGHTCLICK_CB", (Icallback)tabs_rightclick_cb);

  projectTree = IupTree();
  IupSetAttribute(projectTree, "NAME", "PROJECTTREE");
  IupSetAttribute(projectTree, "TITLE0", "Untitled");
  IupSetAttribute(projectTree, "SHOWRENAME", "Yes");
  IupSetAttribute(projectTree, "IMAGELEAF", "IMGPAPER");
  IupSetCallback(projectTree, "EXECUTELEAF_CB", (Icallback)tree_executeleaf_cb);
  IupSetCallback(projectTree, "RIGHTCLICK_CB", (Icallback)tree_rightclick_cb);
  IupSetCallback(projectTree, "SHOWRENAME_CB", (Icallback)tree_showrename_cb);
  IupSetCallback(projectTree, "RENAME_CB", (Icallback)tree_rename_cb);
  IupSetCallback(projectTree, "DROPFILES_CB", (Icallback)tree_dropfiles_cb);
  IupSetCallback(projectTree, "K_DEL", (Icallback)leaf_remove_action_cb);
  IupSetCallback(projectTree, "K_F2", (Icallback)leaf_rename_action_cb);
  IupSetCallback(projectTree, "K_CR", (Icallback)leaf_open_action_cb);

  projectSplit = IupSplit(projectTree, tabs);
  IupSetAttribute(projectSplit, "NAME", "PROJECTSPLIT");
  IupSetAttribute(projectSplit, "ORIENTATION", "VERTICAL");
  IupSetAttribute(projectSplit, "LAYOUTDRAG", "NO");
  IupSetAttribute(projectSplit, "AUTOHIDE", "YES");
  IupSetAttribute(projectSplit, "COLOR", "50 150 255");
  IupSetAttribute(projectSplit, "VALUE", "0");

  listSearch = IupList(NULL);
  IupSetAttribute(listSearch, "EXPAND", "YES");
  IupSetAttribute(listSearch, "NAME", "LIST_SEARCH");
  IupSetCallback(listSearch, "DBLCLICK_CB", (Icallback)list_search_dblclick_cb);
  IupSetAttribute(listSearch, "VISIBLELINES", "3");

  panelFrame = IupFrame(listSearch);
  IupSetAttribute(panelFrame, "NAME", "PANEL_FRAME");
  IupSetAttribute(panelFrame, "MARGIN", "4x4");
  IupSetAttribute(panelFrame, "GAP", "4");
  IupSetAttribute(panelFrame, "TITLE", "Find Results:");
  IupSetAttribute(panelFrame, "TABTITLE", "Find");

  panelTabs = IupTabs(panelFrame, NULL);
  IupSetAttribute(panelTabs, "MARGIN", "0x0");
  IupSetAttribute(panelTabs, "GAP", "4");
  IupSetAttribute(panelTabs, "TABTYPE", "BOTTOM");
  IupSetAttribute(panelTabs, "NAME", "PANEL_TABS");

  panelSplit = IupSplit(projectSplit, panelTabs);
  IupSetAttribute(panelSplit, "NAME", "PANEL_SPLIT");
  IupSetAttribute(panelSplit, "ORIENTATION", "HORIZONTAL");
  IupSetAttribute(panelSplit, "LAYOUTDRAG", "NO");
  IupSetAttribute(panelSplit, "AUTOHIDE", "YES");
  IupSetAttribute(panelSplit, "MINMAX", "100:1000");
  IupSetAttribute(panelSplit, "COLOR", "50 150 255");

  lbl_statusbar = IupLabel("Lin 1, Col 1");
  IupSetAttribute(lbl_statusbar, "NAME", "STATUSBAR");
  IupSetAttribute(lbl_statusbar, "EXPAND", "HORIZONTAL");
  IupSetAttribute(lbl_statusbar, "PADDING", "10x5");

  item_new = IupItem("&New\tCtrl+N", NULL);
  IupSetAttribute(item_new, "IMAGE", "IUP_FileNew");
  IupSetCallback(item_new, "ACTION", (Icallback)item_new_action_cb);
  btn_new = IupButton(NULL, NULL);
  IupSetAttribute(btn_new, "IMAGE", "IUP_FileNew");
  IupSetAttribute(btn_new, "FLAT", "Yes");
  IupSetCallback(btn_new, "ACTION", (Icallback)item_new_action_cb);
  IupSetAttribute(btn_new, "TIP", "New (Ctrl+N)");
  IupSetAttribute(btn_new, "CANFOCUS", "No");

  item_open = IupItem("&Open...\tCtrl+O", NULL);
  IupSetAttribute(item_open, "IMAGE", "IUP_FileOpen");
  IupSetCallback(item_open, "ACTION", (Icallback)item_open_action_cb);
  btn_open = IupButton(NULL, NULL);
  IupSetAttribute(btn_open, "IMAGE", "IUP_FileOpen");
  IupSetAttribute(btn_open, "FLAT", "Yes");
  IupSetCallback(btn_open, "ACTION", (Icallback)item_open_action_cb);
  IupSetAttribute(btn_open, "TIP", "Open (Ctrl+O)");
  IupSetAttribute(btn_open, "CANFOCUS", "No");

  item_save = IupItem("&Save\tCtrl+S", NULL);
  IupSetAttribute(item_save, "NAME", "ITEM_SAVE");
  IupSetAttribute(item_save, "IMAGE", "IUP_FileSave");
  IupSetCallback(item_save, "ACTION", (Icallback)item_save_action_cb);
  btn_save = IupButton(NULL, NULL);
  IupSetAttribute(btn_save, "IMAGE", "IUP_FileSave");
  IupSetAttribute(btn_save, "FLAT", "Yes");
  IupSetCallback(btn_save, "ACTION", (Icallback)item_save_action_cb);
  IupSetAttribute(btn_save, "TIP", "Save (Ctrl+S)\nPress <Shift> to save all open files.");
  IupSetAttribute(btn_save, "CANFOCUS", "No");

  item_saveas = IupItem("Save &As...", NULL);
  IupSetCallback(item_saveas, "ACTION", (Icallback)item_saveas_action_cb);

  item_savecopy = IupItem("Save Cop&y...", NULL);
  IupSetCallback(item_savecopy, "ACTION", (Icallback)item_savecopy_action_cb);

  item_saveall = IupItem("Save A&ll\tCtrl+Shift+S", NULL);
  IupSetCallback(item_saveall, "ACTION", (Icallback)item_saveall_action_cb);

  item_closeall = IupItem("Cl&ose All\tCtrl+Shift+F4", NULL);
  IupSetCallback(item_closeall, "ACTION", (Icallback)item_closeall_action_cb);

  item_close = IupItem("&Close\tCtrl+F4", NULL);
  IupSetCallback(item_close, "ACTION", (Icallback)item_close_action_cb);

  item_rename = IupItem("R&ename...", NULL);
  IupSetCallback(item_rename, "ACTION", (Icallback)item_rename_action_cb);

  item_revert = IupItem("&Revert", NULL);
  IupSetAttribute(item_revert, "NAME", "ITEM_REVERT");
  IupSetCallback(item_revert, "ACTION", (Icallback)item_revert_action_cb);

  item_print = IupItem("&Print...\tCtrl+P", NULL);
  IupSetCallback(item_print, "ACTION", (Icallback)item_print_action_cb);
  item_pagesetup = IupItem("Page Set&up...", NULL);
  IupSetCallback(item_pagesetup, "ACTION", (Icallback)item_pagesetup_action_cb);

  item_loadsession = IupItem("Load Session...", NULL);
  IupSetCallback(item_loadsession, "ACTION", (Icallback)item_loadsession_action_cb);
  item_savesession = IupItem("Save Session...", NULL);
  IupSetCallback(item_savesession, "ACTION", (Icallback)item_savesession_action_cb);
  item_exit = IupItem("E&xit", NULL);
  IupSetCallback(item_exit, "ACTION", (Icallback)close_exit_action_cb);

  item_new_proj = IupItem("&New Project", NULL);
  IupSetCallback(item_new_proj, "ACTION", (Icallback)item_new_proj_action_cb);

  item_new_blank_proj = IupItem("New &Blank Project", NULL);
  IupSetCallback(item_new_blank_proj, "ACTION", (Icallback)item_new_blank_proj_action_cb);

  item_open_proj = IupItem("&Open Project...", NULL);
  IupSetCallback(item_open_proj, "ACTION", (Icallback)item_open_proj_action_cb);

  item_save_proj = IupItem("&Save Project", NULL);
  IupSetAttribute(item_save_proj, "NAME", "ITEM_SAVE_PROJ");
  IupSetCallback(item_save_proj, "ACTION", (Icallback)item_save_proj_action_cb);

  item_save_proj_as = IupItem("Save Project &As...", NULL);
  IupSetAttribute(item_save_proj_as, "NAME", "ITEM_SAVE_PROJ_AS");
  IupSetCallback(item_save_proj_as, "ACTION", (Icallback)item_save_proj_as_action_cb);

  item_close_proj = IupItem("&Close Project", NULL);
  IupSetAttribute(item_close_proj, "NAME", "ITEM_CLOSE_PROJ");
  IupSetCallback(item_close_proj, "ACTION", (Icallback)item_close_proj_action_cb);

  item_add_new_file = IupItem("&Add New File...", NULL);
  IupSetAttribute(item_add_new_file, "NAME", "ITEM_ADD_NEW_FILE");
  IupSetCallback(item_add_new_file, "ACTION", (Icallback)item_add_new_file_action_cb);

  item_add_exist_file = IupItem("Add &Existing File...", NULL);
  IupSetAttribute(item_add_exist_file, "NAME", "ITEM_ADD_EXIST_FILE");
  IupSetCallback(item_add_exist_file, "ACTION", (Icallback)item_add_exist_file_action_cb);

  item_add_open_file = IupItem("Add O&pened File...", NULL);
  IupSetAttribute(item_add_open_file, "NAME", "ITEM_ADD_OPEN_FILE");
  IupSetCallback(item_add_open_file, "ACTION", (Icallback)item_add_open_file_action_cb);

  item_open_proj_file = IupItem("Open Selected &File", NULL);
  IupSetAttribute(item_open_proj_file, "NAME", "ITEM_OPEN_PROJ_FILE");
  IupSetCallback(item_open_proj_file, "ACTION", (Icallback)item_open_proj_file_action_cb);

  item_open_all_proj_file = IupItem("Open A&ll Files", NULL);
  IupSetAttribute(item_open_all_proj_file, "NAME", "ITEM_OPEN_ALL_PROJ_FILE");
  IupSetCallback(item_open_all_proj_file, "ACTION", (Icallback)item_open_all_proj_file_action_cb);

  item_remove_proj_file = IupItem("&Remove Selected File", NULL);
  IupSetAttribute(item_remove_proj_file, "NAME", "ITEM_REMOVE_PROJ_FILE");
  IupSetCallback(item_remove_proj_file, "ACTION", (Icallback)item_remove_proj_file_action_cb);

  item_find = IupItem("&Find...\tCtrl+F", NULL);
  IupSetAttribute(item_find, "IMAGE", "IUP_EditFind");
  IupSetCallback(item_find, "ACTION", (Icallback)item_find_action_cb);
  btn_find = IupButton(NULL, NULL);
  IupSetAttribute(btn_find, "IMAGE", "IUP_EditFind");
  IupSetAttribute(btn_find, "FLAT", "Yes");
  IupSetCallback(btn_find, "ACTION", (Icallback)item_find_action_cb);
  IupSetAttribute(btn_find, "TIP", "Find (Ctrl+F)");
  IupSetAttribute(btn_find, "CANFOCUS", "No");

  item_find_next = IupItem("Find &Next\tF3", NULL);
  IupSetAttribute(item_find_next, "NAME", "ITEM_FINDNEXT");
  IupSetCallback(item_find_next, "ACTION", (Icallback)find_next_action_cb);

  item_replace = IupItem("&Replace...\tCtrl+H", NULL);
  IupSetCallback(item_replace, "ACTION", (Icallback)item_replace_action_cb);

  item_cut = IupItem("Cu&t\tCtrl+X", NULL);
  IupSetAttribute(item_cut, "NAME", "ITEM_CUT");
  IupSetAttribute(item_cut, "IMAGE", "IUP_EditCut");
  IupSetCallback(item_cut, "ACTION", (Icallback)item_cut_action_cb);
  btn_cut = IupButton(NULL, NULL);
  IupSetAttribute(btn_cut, "IMAGE", "IUP_EditCut");
  IupSetAttribute(btn_cut, "FLAT", "Yes");
  IupSetCallback(btn_cut, "ACTION", (Icallback)item_cut_action_cb);
  IupSetAttribute(btn_cut, "TIP", "Cut (Ctrl+X)");
  IupSetAttribute(btn_cut, "CANFOCUS", "No");

  item_copy = IupItem("&Copy\tCtrl+C", NULL);
  IupSetAttribute(item_copy, "NAME", "ITEM_COPY");
  IupSetAttribute(item_copy, "IMAGE", "IUP_EditCopy");
  IupSetCallback(item_copy, "ACTION", (Icallback)item_copy_action_cb);
  btn_copy = IupButton(NULL, NULL);
  IupSetAttribute(btn_copy, "IMAGE", "IUP_EditCopy");
  IupSetAttribute(btn_copy, "FLAT", "Yes");
  IupSetCallback(btn_copy, "ACTION", (Icallback)item_copy_action_cb);
  IupSetAttribute(btn_copy, "TIP", "Copy (Ctrl+C)");
  IupSetAttribute(btn_copy, "CANFOCUS", "No");

  item_paste = IupItem("&Paste\tCtrl+V", NULL);
  IupSetAttribute(item_paste, "NAME", "ITEM_PASTE");
  IupSetAttribute(item_paste, "IMAGE", "IUP_EditPaste");
  IupSetCallback(item_paste, "ACTION", (Icallback)item_paste_action_cb);
  btn_paste = IupButton(NULL, NULL);
  IupSetAttribute(btn_paste, "IMAGE", "IUP_EditPaste");
  IupSetAttribute(btn_paste, "FLAT", "Yes");
  IupSetCallback(btn_paste, "ACTION", (Icallback)item_paste_action_cb);
  IupSetAttribute(btn_paste, "TIP", "Paste (Ctrl+V)");
  IupSetAttribute(btn_paste, "CANFOCUS", "No");

  item_delete = IupItem("&Delete\tDel", NULL);
  IupSetAttribute(item_delete, "IMAGE", "IUP_EditErase");
  IupSetAttribute(item_delete, "NAME", "ITEM_DELETE");
  IupSetCallback(item_delete, "ACTION", (Icallback)item_delete_action_cb);

  item_select_all = IupItem("Select &All\tCtrl+A", NULL);
  IupSetCallback(item_select_all, "ACTION", (Icallback)item_select_all_action_cb);

  item_undo = IupItem("Undo\tCtrl+Z", NULL);
  IupSetAttribute(item_undo, "NAME", "ITEM_UNDO");
  IupSetCallback(item_undo, "ACTION", (Icallback)item_undo_action_cb);

  item_redo = IupItem("Redo\tCtrl+Y", NULL);
  IupSetAttribute(item_redo, "NAME", "ITEM_REDO");
  IupSetCallback(item_redo, "ACTION", (Icallback)item_redo_action_cb);

  item_uppercase = IupItem("UPPERCASE\tCtrl+Shift+U", NULL);
  IupSetCallback(item_uppercase, "ACTION", (Icallback)item_uppercase_action_cb);

  item_lowercase = IupItem("lowercase\tCtrl+U", NULL);
  IupSetCallback(item_lowercase, "ACTION", (Icallback)item_lowercase_action_cb);

  item_goto = IupItem("&Go To...\tCtrl+G", NULL);
  IupSetCallback(item_goto, "ACTION", (Icallback)item_goto_action_cb);

  item_gotombrace = IupItem("Go To Matching Brace\tCtrl+B", NULL);
  IupSetCallback(item_gotombrace, "ACTION", (Icallback)item_gotombrace_action_cb);

  item_togglemark = IupItem("Toggle Bookmark\tCtrl+F2", NULL);
  IupSetCallback(item_togglemark, "ACTION", (Icallback)item_togglemark_action_cb);

  item_nextmark = IupItem("Next Bookmark\tF2", NULL);
  IupSetCallback(item_nextmark, "ACTION", (Icallback)item_nextmark_action_cb);

  item_previousmark = IupItem("Previous Bookmark\tShift+F2", NULL);
  IupSetCallback(item_previousmark, "ACTION", (Icallback)item_previousmark_action_cb);

  item_clearmarks = IupItem("Clear All Bookmarks", NULL);
  IupSetCallback(item_clearmarks, "ACTION", (Icallback)item_clearmarks_action_cb);

  item_copymarked = IupItem("Copy Bookmarked Lines", NULL);
  IupSetCallback(item_copymarked, "ACTION", (Icallback)item_copymarked_action_cb);

  item_cutmarked = IupItem("Cut Bookmarked Lines", NULL);
  IupSetCallback(item_cutmarked, "ACTION", (Icallback)item_cutmarked_action_cb);

  item_pastetomarked = IupItem("Paste to (Replace) Bookmarked Lines", NULL);
  IupSetCallback(item_pastetomarked, "ACTION", (Icallback)item_pastetomarked_action_cb);

  item_removemarked = IupItem("Remove Bookmarked Lines", NULL);
  IupSetCallback(item_removemarked, "ACTION", (Icallback)item_removemarked_action_cb);

  item_invertmarks = IupItem("Invert Bookmarks", NULL);
  IupSetCallback(item_invertmarks, "ACTION", (Icallback)item_invertmarks_action_cb);

  item_trimtrailing = IupItem("Trim Trailing Space", NULL);
  IupSetCallback(item_trimtrailing, "ACTION", (Icallback)item_trimtrailing_action_cb);

  item_trimtraillead = IupItem("Trim Trailing and Leading Space", NULL);
  IupSetCallback(item_trimtraillead, "ACTION", (Icallback)item_trimtraillead_action_cb);

  item_eoltospace = IupItem("End of Lines to Spaces", NULL);
  IupSetCallback(item_eoltospace, "ACTION", (Icallback)item_eoltospace_action_cb);

  item_eol_cr = IupItem("CR (Macintosh)", NULL);
  IupSetAttribute(item_eol_cr, "EOL", "CR");
  IupSetCallback(item_eol_cr, "ACTION", (Icallback)item_eol_action_cb);

  item_eol_crlf = IupItem("CRLF (Windows)", NULL);
  IupSetAttribute(item_eol_crlf, "EOL", "CRLF");
  IupSetCallback(item_eol_crlf, "ACTION", (Icallback)item_eol_action_cb);

  item_eol_lf = IupItem("LF (UNIX)", NULL);
  IupSetAttribute(item_eol_lf, "EOL", "LF");
  IupSetCallback(item_eol_lf, "ACTION", (Icallback)item_eol_action_cb);

  item_removespaceeol = IupItem("Remove Unnecessary Blanks and EOL", NULL);
  IupSetCallback(item_removespaceeol, "ACTION", (Icallback)item_removespaceeol_action_cb);

  item_trimleading = IupItem("Trim Leading Space", NULL);
  IupSetCallback(item_trimleading, "ACTION", (Icallback)item_trimleading_action_cb);

  item_tabtospace = IupItem("TABs to Spaces", NULL);
  IupSetCallback(item_tabtospace, "ACTION", (Icallback)item_tabtospace_action_cb);

  item_allspacetotab = IupItem("Spaces to TABs (All)", NULL);
  IupSetCallback(item_allspacetotab, "ACTION", (Icallback)item_allspacetotab_action_cb);

  item_leadingspacetotab = IupItem("Spaces to TABs (Leading)", NULL);
  IupSetCallback(item_leadingspacetotab, "ACTION", (Icallback)item_leadingspacetotab_action_cb);

  item_zoomin = IupItem("Zoom In\tCtrl+'+'", NULL);
  IupSetCallback(item_zoomin, "ACTION", (Icallback)item_zoomin_action_cb);

  item_zoomout = IupItem("Zoom Out\tCtrl+'-'", NULL);
  IupSetCallback(item_zoomout, "ACTION", (Icallback)item_zoomout_action_cb);

  item_restorezoom = IupItem("Reset Zoom\tCtrl+/", NULL);
  IupSetCallback(item_restorezoom, "ACTION", (Icallback)item_restorezoom_action_cb);

  item_wordwrap = IupItem("Word Wrap", NULL);
  IupSetCallback(item_wordwrap, "ACTION", (Icallback)item_wordwrap_action_cb);
  IupSetAttribute(item_wordwrap, "AUTOTOGGLE", "YES");
  IupSetAttribute(item_wordwrap, "NAME", "ITEM_WORDWRAP");

  item_showwhite = IupItem("Show White Spaces", NULL);
  IupSetCallback(item_showwhite, "ACTION", (Icallback)item_showwhite_action_cb);
  IupSetAttribute(item_showwhite, "AUTOTOGGLE", "YES");
  IupSetAttribute(item_showwhite, "NAME", "ITEM_SHOWWHITE");

  item_showeol = IupItem("Show End of Lines", NULL);
  IupSetCallback(item_showeol, "ACTION", (Icallback)item_showeol_action_cb);
  IupSetAttribute(item_showeol, "AUTOTOGGLE", "YES");
  IupSetAttribute(item_showeol, "NAME", "ITEM_SHOWEOL");

  item_panel = IupItem("Panel", NULL);
  IupSetAttribute(item_panel, "NAME", "ITM_PANEL");
  IupSetCallback(item_panel, "ACTION", (Icallback)item_panel_action_cb);
  IupSetAttribute(item_panel, "VALUE", "ON");

  item_toolbar = IupItem("&Toolbar", NULL);
  IupSetCallback(item_toolbar, "ACTION", (Icallback)item_toolbar_action_cb);
  IupSetAttribute(item_toolbar, "VALUE", "ON");
  IupSetAttribute(item_toolbar, "NAME", "ITEM_TOOLBAR");

  item_statusbar = IupItem("&Statusbar", NULL);
  IupSetCallback(item_statusbar, "ACTION", (Icallback)item_statusbar_action_cb);
  IupSetAttribute(item_statusbar, "VALUE", "ON");
  IupSetAttribute(item_statusbar, "NAME", "ITEM_STATUSBAR");

  item_linenumber = IupItem("Display Line Numbers", NULL);
  IupSetCallback(item_linenumber, "ACTION", (Icallback)item_linenumber_action_cb);
  IupSetAttribute(item_linenumber, "AUTOTOGGLE", "YES");
  IupSetAttribute(item_linenumber, "VALUE", "ON");
  IupSetAttribute(item_linenumber, "NAME", "ITEM_LINENUMBER");

  item_bookmark = IupItem("Display Bookmarks", NULL);
  IupSetCallback(item_bookmark, "ACTION", (Icallback)item_bookmark_action_cb);
  IupSetAttribute(item_bookmark, "AUTOTOGGLE", "YES");
  IupSetAttribute(item_bookmark, "VALUE", "ON");
  IupSetAttribute(item_bookmark, "NAME", "ITEM_BOOKMARK");

  item_font = IupItem("&Font...", NULL);
  IupSetCallback(item_font, "ACTION", (Icallback)item_font_action_cb);

  item_tab = IupItem("Tab...", NULL);
  IupSetCallback(item_tab, "ACTION", (Icallback)item_tab_action_cb);

  item_window1 = IupItem("1 Untitled", NULL);
  IupSetAttribute(item_window1, "NAME", "ITEM_WINDOW1");
  IupSetCallback(item_window1, "ACTION", (Icallback)item_windowN_action_cb);
  IupSetAttribute(item_window1, "VALUE", "ON");

  item_windows = IupItem("Windows...", NULL);
  IupSetCallback(item_windows, "ACTION", (Icallback)item_windows_action_cb);

  recent_menu = IupMenu(NULL);
  iupAttribSet(ih, "_IUP_RECENTMENU", (char*)recent_menu);

  recent_proj_menu = IupMenu(NULL);
  iupAttribSet(ih, "_IUP_PROJ_RECENTMENU", (char*)recent_proj_menu);

  file_menu = IupMenu(
    item_new,
    item_open,
    item_close,
    item_closeall,
    item_revert,
    IupSeparator(),
    item_save,
    item_saveas,
    item_savecopy,
    item_saveall,
    item_rename,
    IupSeparator(),
    item_pagesetup,
    item_print,
    IupSeparator(),
    item_loadsession,
    item_savesession,
    IupSeparator(),
    IupSubmenu("Recent &Files", recent_menu),
    item_exit,
    NULL);
  project_menu = IupMenu(
    item_new_blank_proj,
    item_new_proj,
    item_open_proj,
    item_close_proj,
    IupSeparator(),
    item_save_proj,
    item_save_proj_as,
    IupSeparator(),
    item_add_new_file,
    item_add_exist_file,
    item_add_open_file,
    item_remove_proj_file,
    IupSeparator(),
    item_open_proj_file,
    item_open_all_proj_file,
    IupSeparator(),
    IupSubmenu("Recent &Projects", recent_proj_menu),
    NULL);
  edit_menu = IupMenu(
    item_undo,
    item_redo,
    IupSeparator(),
    item_cut,
    item_copy,
    item_paste,
    item_delete,
    IupSeparator(),
    item_find,
    item_find_next,
    item_replace,
    item_goto,
    item_gotombrace,
    IupSeparator(),
    IupSubmenu("Bookmarks", IupMenu(item_togglemark,
    item_nextmark,
    item_previousmark,
    item_clearmarks,
    item_cutmarked,
    item_copymarked,
    item_pastetomarked,
    item_removemarked,
    item_invertmarks,
    NULL)),
    IupSubmenu("Blank Operations", IupMenu(
      item_trimtrailing,
      item_trimleading,
      item_trimtraillead,
      item_eoltospace,
      item_removespaceeol,
      IupSeparator(),
      item_tabtospace,
      item_allspacetotab,
      item_leadingspacetotab,
      NULL)),
    IupSubmenu("End of Lines", IupMenu(
      item_eol_lf,
      item_eol_crlf,
      item_eol_cr,
      NULL)),
    IupSubmenu("Convert Case to", case_menu = IupMenu(
      item_uppercase,
      item_lowercase,
      NULL)),
    IupSeparator(),
    item_select_all,
    NULL);
  format_menu = IupMenu(
    item_font,
    item_tab,
    NULL);
  view_menu = IupMenu(
    IupSubmenu("Zoom", zoom_menu = IupMenu(
    item_zoomin,
    item_zoomout,
    item_restorezoom,
    NULL)),
    item_wordwrap,
    item_showwhite,
    item_showeol,
    IupSeparator(),
    item_panel,
    item_toolbar,
    item_statusbar,
    item_linenumber,
    item_bookmark,
    NULL);
  window_menu = IupMenu(
    item_window1,
    IupSeparator(),
    item_windows,
    NULL);

  IupSetCallback(file_menu, "OPEN_CB", (Icallback)file_menu_open_cb);
  IupSetCallback(edit_menu, "OPEN_CB", (Icallback)edit_menu_open_cb);
  IupSetCallback(window_menu, "OPEN_CB", (Icallback)window_menu_open_cb);
  IupSetCallback(project_menu, "OPEN_CB", (Icallback)project_menu_open_cb);
  IupSetCallback(view_menu, "OPEN_CB", (Icallback)view_menu_open_cb);

  sub_menu_file = IupSubmenu("&File", file_menu);
  sub_menu_project = IupSubmenu("&Project", project_menu);
  sub_menu_edit = IupSubmenu("&Edit", edit_menu);
  sub_menu_format = IupSubmenu("F&ormat", format_menu);
  sub_menu_view = IupSubmenu("&View", view_menu);
  sub_menu_window = IupSubmenu("&Window", window_menu);

  menu = IupMenu(
    sub_menu_file,
    sub_menu_project,
    sub_menu_edit,
    sub_menu_format,
    sub_menu_view,
    sub_menu_window,
    NULL);

  toolbar_hb = IupHbox(
    btn_new,
    btn_open,
    btn_save,
    IupSetAttributes(IupLabel(NULL), "SEPARATOR=VERTICAL"),
    btn_cut,
    btn_copy,
    btn_paste,
    IupSetAttributes(IupLabel(NULL), "SEPARATOR=VERTICAL"),
    btn_find,
    NULL);
  IupSetAttribute(toolbar_hb, "MARGIN", "5x5");
  IupSetAttribute(toolbar_hb, "GAP", "2");

  vbox = IupVbox(
    toolbar_hb,
    panelSplit,
    lbl_statusbar,
    NULL);

  /* Do not use IupAppend because we set childtype=IUP_CHILDNONE */
  iupChildTreeAppend(ih, vbox);

  IupSetAttributeHandle(ih, "MENU", menu);
  IupSetCallback(ih, "CLOSE_CB", (Icallback)close_exit_action_cb);
  IupSetCallback(ih, "SHOW_CB", (Icallback)show_cb);
  IupSetCallback(ih, "DROPFILES_CB", (Icallback)dropfiles_cb);

  IupSetCallback(ih, "K_cN", (Icallback)item_new_action_cb);
  IupSetCallback(ih, "K_cO", (Icallback)item_open_action_cb);
  IupSetCallback(ih, "K_cS", (Icallback)item_save_action_cb);
  IupSetCallback(ih, "K_cP", (Icallback)item_print_action_cb);
  IupSetCallback(ih, "K_cF", (Icallback)item_find_action_cb);
  IupSetCallback(ih, "K_cH", (Icallback)item_replace_action_cb);  /* replace system processing */
  IupSetCallback(ih, "K_cG", (Icallback)item_goto_action_cb);
  IupSetCallback(ih, "K_cB", (Icallback)item_gotombrace_action_cb);
  IupSetCallback(ih, "K_cF2", (Icallback)item_togglemark_action_cb);
  IupSetCallback(ih, "K_F2", (Icallback)item_nextmark_action_cb);
  IupSetCallback(ih, "K_sF2", (Icallback)item_previousmark_action_cb);
  IupSetCallback(ih, "K_F3", (Icallback)find_next_action_cb);
  IupSetCallback(ih, "K_cF3", (Icallback)selection_find_next_action_cb);
  IupSetCallback(ih, "K_c+", (Icallback)item_zoomin_action_cb);
  IupSetCallback(ih, "K_c-", (Icallback)item_zoomout_action_cb);
  IupSetCallback(ih, "K_c/", (Icallback)item_restorezoom_action_cb);
  IupSetCallback(ih, "K_cEqual", (Icallback)item_zoomin_action_cb);
  IupSetCallback(ih, "K_cMinus", (Icallback)item_zoomout_action_cb);
  IupSetCallback(ih, "K_cSlash", (Icallback)item_restorezoom_action_cb);
  IupSetCallback(ih, "K_cF4", (Icallback)item_close_action_cb);
  IupSetCallback(ih, "K_sF4", (Icallback)item_close_action_cb);

  iupAttribSet(ih, "_IUP_CONFIG", (char*)IupConfig());

  new_file(tabs);  /* always keep at least one multitext */

  (void)params;
  return IUP_NOERROR;
}

Iclass* iupScintillaDlgNewClass(void)
{
  Iclass* ic = iupClassNew(iupRegisterFindClass("dialog"));

  ic->New = iupScintillaDlgNewClass;
  ic->Create = iScintillaDlgCreateMethod;

  ic->name = "scintilladlg";
  ic->nativetype = IUP_TYPEDIALOG;
  ic->is_interactive = 1;
  ic->childtype = IUP_CHILDNONE;
  ic->has_attrib_id = 1;   /* has attributes with IDs that must be parsed */

  iupClassRegisterCallback(ic, "MARKERCHANGED_CB", "nii");
  iupClassRegisterCallback(ic, "SAVEMARKERS_CB", "i");
  iupClassRegisterCallback(ic, "RESTOREMARKERS_CB", "i");
  iupClassRegisterCallback(ic, "NEWTEXT_CB", "i");
  iupClassRegisterCallback(ic, "CLOSETEXT_CB", "i");
  iupClassRegisterCallback(ic, "NEWFILENAME_CB", "ss");
  iupClassRegisterCallback(ic, "CONFIGSAVE_CB", "i");
  iupClassRegisterCallback(ic, "CONFIGLOAD_CB", "i");
  iupClassRegisterCallback(ic, "EXIT_CB", "");

  iupClassRegisterAttribute(ic, "SUBTITLE", NULL, iScintillaDlgSetSubTitleAttrib, IUPAF_SAMEASSYSTEM, "Notepad", IUPAF_NOT_MAPPED | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "UPDATETITLE", NULL, iScintillaDlgSetUpdateTitleAttrib, NULL, NULL, IUPAF_WRITEONLY | IUPAF_NOT_MAPPED | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "CONFIG", iScintillaDlgGetConfigAttrib, iScintillaDlgSetConfigAttrib, NULL, NULL, IUPAF_IHANDLENAME | IUPAF_NOT_MAPPED | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "CONFIG_HANDLE", iScintillaDlgGetConfigHandleAttrib, iScintillaDlgSetConfigHandleAttrib, NULL, NULL, IUPAF_IHANDLE | IUPAF_NOT_MAPPED | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "NEWFILE", NULL, iScintillaDlgSetOpenFileAttrib, NULL, NULL, IUPAF_WRITEONLY | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "OPENFILE", NULL, iScintillaDlgSetOpenFileAttrib, NULL, NULL, IUPAF_WRITEONLY | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SAVEFILE", NULL, iScintillaDlgSetSaveFileAttrib, NULL, NULL, IUPAF_WRITEONLY | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "CLOSEFILE", NULL, iScintillaDlgSetCloseFileAttrib, NULL, NULL, IUPAF_WRITEONLY | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "FORCECLOSEFILE", NULL, iScintillaDlgSetForceCloseFileAttrib, NULL, NULL, IUPAF_WRITEONLY | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "EXTRAFILTERS", NULL, NULL, NULL, NULL, IUPAF_NOT_MAPPED | IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "TOGGLEMARKER", NULL, iScintillaDlgSetToggleMarkerAttribId, IUPAF_WRITEONLY | IUPAF_NO_INHERIT);

  return ic;
}

Ihandle* IupScintillaDlg(void)
{
  return IupCreate("scintilladlg");
}
