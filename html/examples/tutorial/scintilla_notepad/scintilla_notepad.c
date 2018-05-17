#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <iup.h>
#include <iup_scintilla.h>
#include <iup_config.h>


/********************************** Utilities *****************************************/

void toggleMarker(Ihandle *ih, int lin, int margin)
{
  long int value = IupGetIntId(ih, "MARKERGET", lin);

  if (margin == 1)
    value = value & 0x000001;
  else
    value = value & 0x000002;

  if (value)
    IupSetIntId(ih, "MARKERDELETE", lin, margin - 1);
  else
    IupSetIntId(ih, "MARKERADD", lin, margin - 1);
}

long int setMarkerMask(int markNumber)
{
  long int mask = 0x000000;
  long int mark = 0x00001 << markNumber;
  return mask | mark;
}

void copyMarkedLines(Ihandle *multitext)
{
  int size = IupGetInt(multitext, "COUNT");
  char *buffer = (char *) malloc(size);
  char *text;
  int lin = 0;

  buffer[0] = 0;
  while (lin >= 0)
  {
    IupSetIntId(multitext, "MARKERNEXT", lin, setMarkerMask(0));
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
    IupSetAttribute(clipboard, "TEXT", buffer);
    IupDestroy(clipboard);
  }

  free(buffer);
}

void cutMarkedLines(Ihandle *multitext)
{
  int size = IupGetInt(multitext, "COUNT");
  char *buffer = (char *)malloc(size);
  char *text;
  int lin = 0, pos, len;

  buffer[0] = 0;
  while (lin >= 0 && size)
  {
    IupSetIntId(multitext, "MARKERNEXT", lin, setMarkerMask(0));
    lin = IupGetInt(multitext, "LASTMARKERFOUND");
    if (lin >= 0)
    {
      text = IupGetAttributeId(multitext, "LINE", lin);
      len = (int)strlen(text);
      IupTextConvertLinColToPos(multitext, lin, 0, &pos);
      IupSetStrf(multitext, "DELETERANGE", "%d,%d", pos, len);
      strcat(buffer, text);  size -= len;
      IupSetIntId(multitext, "MARKERDELETE", lin, 0);
      lin--;
    }
  }

  if (strlen(buffer) > 0)
  {
    Ihandle *clipboard = IupClipboard();
    IupSetAttribute(clipboard, "TEXT", buffer);
    IupDestroy(clipboard);
  }

  free(buffer);
}

void pasteToMarkedLines(Ihandle *multitext)
{
  char *text;
  int lin = 0, pos, len;

  while (lin >= 0)
  {
    IupSetIntId(multitext, "MARKERNEXT", lin, setMarkerMask(0));
    lin = IupGetInt(multitext, "LASTMARKERFOUND");
    if (lin >= 0)
    {
      Ihandle *clipboard;
      text = IupGetAttributeId(multitext, "LINE", lin);
      len = (int)strlen(text);
      IupTextConvertLinColToPos(multitext, lin, 0, &pos);
      IupSetStrf(multitext, "DELETERANGE", "%d,%d", pos, len);
      IupSetIntId(multitext, "MARKERDELETE", lin, 0);
      clipboard = IupClipboard();
      IupSetAttributeId(multitext, "INSERT", pos, IupGetAttribute(clipboard, "TEXT"));
      IupDestroy(clipboard);
      lin--;
    }
  }
}

void invertMarkedLines(Ihandle *multitext)
{
  int lin;
  for (lin = 0; lin < IupGetInt(multitext, "LINECOUNT"); lin++)
  {
    toggleMarker(multitext, lin, 1);
  }
}

void removeMarkedLines(Ihandle *multitext)
{
  char *text;
  int lin = 0, pos, len;

  while (lin >= 0)
  {
    IupSetIntId(multitext, "MARKERNEXT", lin, setMarkerMask(0));
    lin = IupGetInt(multitext, "LASTMARKERFOUND");
    if (lin >= 0)
    {
      text = IupGetAttributeId(multitext, "LINE", lin);
      len = (int)strlen(text);
      IupTextConvertLinColToPos(multitext, lin, 0, &pos);
      IupSetStrf(multitext, "DELETERANGE", "%d,%d", pos, len);
      IupSetIntId(multitext, "MARKERDELETE", lin, 0);
      lin--;
    }
  }
}

void removeUnmarkedLines(Ihandle *multitext)
{
  char *text;
  int len, start = IupGetInt(multitext, "LINECOUNT") - 1, end, posStart, posEnd;

  while (start >= 0)
  {
    text = IupGetAttributeId(multitext, "LINE", start);
    len = (int)strlen(text);
    IupSetIntId(multitext, "MARKERPREVIOUS", start, setMarkerMask(0));
    end = IupGetInt(multitext, "LASTMARKERFOUND");
    IupTextConvertLinColToPos(multitext, start, len + 1, &posEnd);
    if (end >= 0)
    {
      text = IupGetAttributeId(multitext, "LINE", end);
      len = (int)strlen(text);
      IupTextConvertLinColToPos(multitext, end, len + 1, &posStart);
    }
    else
    {
      posStart = 0;
      posEnd++;
    }
    IupSetStrf(multitext, "DELETERANGE", "%d,%d", posStart, posEnd - posStart);
    end--;
    start = end;
  }
}

void changeTabsToSpaces(Ihandle *multitext)
{
  char *text = IupGetAttribute(multitext, "VALUE");
  int count = IupGetInt(multitext, "COUNT");
  int tabSize = IupGetInt(multitext, "TABSIZE");
  int i, j, lin, col, spacesToNextTab;

  for (i = count - 1; i >= 0; i--)
  {
    char c = text[i];

    if (c != '\t')
      continue;

    IupTextConvertPosToLinCol(multitext, i, &lin, &col);

    spacesToNextTab = tabSize - (col + 1) % tabSize + 1;

    IupSetStrf(multitext, "DELETERANGE", "%d,%d", i, 1);

    for (j = 0; j < spacesToNextTab; j++)
      IupSetAttributeId(multitext, "INSERT", i + j, " ");
  }
}

void changeSpacesToTabs(Ihandle *multitext)
{
  char *text = IupGetAttribute(multitext, "VALUE");
  int count = IupGetInt(multitext, "COUNT");
  int tabSize = IupGetInt(multitext, "TABSIZE");
  int i, lin, col, nSpaces;

  for (i = count - 1; i >= 0; i--)
  {
    char c = text[i];

    IupTextConvertPosToLinCol(multitext, i, &lin, &col);

    int tabStop = (col + 1) % tabSize == tabSize - 1 ? 1 : 0;

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

void changeLeadingSpacesToTabs(Ihandle *multitext)
{
  int lineCount = IupGetInt(multitext, "LINECOUNT");
  int tabSize = IupGetInt(multitext, "TABSIZE");
  int i, j, pos, tabCount, spaceCount;

  for (i = 0; i < lineCount; i++)
  {
    char *text = IupGetAttributeId(multitext, "LINE", i);

    int len = (int)strspn(text, " \t");
    if (len == 0)
      continue;

    tabCount = 0;
    spaceCount = 0;
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

void removeLeadingSpaces(Ihandle *multitext)
{
  int lineCount = IupGetInt(multitext, "LINECOUNT");
  int i, pos;

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

void removeTrailingSpaces(Ihandle *multitext)
{
  int lineCount = IupGetInt(multitext, "LINECOUNT");
  int i, j, pos, count;

  for (i = 0; i < lineCount; i++)
  {
    char *text = IupGetAttributeId(multitext, "LINE", i);

    int len = (int)strlen(text);
    if (len == 0)
      continue;

    if (text[len - 1] == '\n')
      len--;

    count = 0;
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

void changeEolToSpace(Ihandle *multitext)
{
  while (1)
  {
    int pos;
    char *text = IupGetAttribute(multitext, "VALUE");

    char *c = strchr(text, '\n');
    if (c==NULL)
      break;

    pos = (int)(c - text);

    IupSetStrf(multitext, "DELETERANGE", "%d,%d", pos, 1);
    IupSetAttributeId(multitext, "INSERT", pos, " ");
  }
}

const char* str_filetitle(const char *filename)
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

char* read_file(const char* filename)
{
  int size;
  char* str;
  FILE* file = fopen(filename, "rb");
  if (!file)
  {
    IupMessagef("Error", "Can't open file: %s", filename);
    return NULL;
  }

  /* calculate file size */
  fseek(file, 0, SEEK_END);
  size = ftell(file);
  fseek(file, 0, SEEK_SET);

  /* allocate memory for the file contents + nul terminator */
  str = malloc(size + 1);
  /* read all data at once */
  fread(str, size, 1, file);
  /* set the nul terminator */
  str[size] = 0;

  if (ferror(file))
    IupMessagef("Error", "Fail when reading from file: %s", filename);

  fclose(file);
  return str;
}

int write_file(const char* filename, const char* str, int count)
{
  FILE* file = fopen(filename, "w");
  if (!file)
  {
    IupMessagef("Error", "Can't open file: %s", filename);
    return 0;
  }

  fwrite(str, 1, count, file);

  if (ferror(file))
    IupMessagef("Error", "Fail when writing to file: %s", filename);

  fclose(file);
  return 1;
}

void new_file(Ihandle* ih)
{
  Ihandle* dlg = IupGetDialog(ih);
  Ihandle* multitext = IupGetDialogChild(dlg, "MULTITEXT");

  IupSetAttribute(dlg, "TITLE", "Untitled - Scintilla Notepad");
  IupSetAttribute(multitext, "FILENAME", NULL);
  IupSetAttribute(multitext, "DIRTY", "NO");
  IupSetAttribute(multitext, "VALUE", "");
}

void open_file(Ihandle* ih, const char* filename)
{
  char* str = read_file(filename);
  if (str)
  {
    Ihandle* dlg = IupGetDialog(ih);
    Ihandle* multitext = IupGetDialogChild(dlg, "MULTITEXT");
    Ihandle* config = (Ihandle*)IupGetAttribute(multitext, "CONFIG");

    IupSetfAttribute(dlg, "TITLE", "%s - Scintilla Notepad", str_filetitle(filename));
    IupSetStrAttribute(multitext, "FILENAME", filename);
    IupSetAttribute(multitext, "DIRTY", "NO");
    IupSetStrAttribute(multitext, "VALUE", str);

    IupConfigRecentUpdate(config, filename);

    free(str);
  }
}

void save_file(Ihandle* multitext)
{
  char* filename = IupGetAttribute(multitext, "FILENAME");
  char* str = IupGetAttribute(multitext, "VALUE");
  int count = IupGetInt(multitext, "COUNT");
  if (write_file(filename, str, count))
    IupSetAttribute(multitext, "DIRTY", "NO");
}

void saveas_file(Ihandle* multitext, const char* filename)
{
  char* str = IupGetAttribute(multitext, "VALUE");
  int count = IupGetInt(multitext, "COUNT");
  if (write_file(filename, str, count))
  {
    Ihandle* config = (Ihandle*)IupGetAttribute(multitext, "CONFIG");

    IupSetfAttribute(IupGetDialog(multitext), "TITLE", "%s - Scintilla Notepad", str_filetitle(filename));
    IupSetStrAttribute(multitext, "FILENAME", filename);
    IupSetAttribute(multitext, "DIRTY", "NO");

    IupConfigRecentUpdate(config, filename);
  }
}

int save_check(Ihandle* ih)
{
  Ihandle* multitext = IupGetDialogChild(ih, "MULTITEXT");
  if (IupGetInt(multitext, "DIRTY"))
  {
    switch (IupAlarm("Warning", "File not saved! Save it now?", "Yes", "No", "Cancel"))
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

void toggle_bar_visibility(Ihandle* item, Ihandle* ih)
{
  if (IupGetInt(item, "VALUE"))
  {
    IupSetAttribute(ih, "FLOATING", "YES");
    IupSetAttribute(ih, "VISIBLE", "NO");
    IupSetAttribute(item, "VALUE", "OFF");
  }
  else
  {
    IupSetAttribute(ih, "FLOATING", "NO");
    IupSetAttribute(ih, "VISIBLE", "YES");
    IupSetAttribute(item, "VALUE", "ON");
  }

  IupRefresh(ih);  /* refresh the dialog layout */
}

void set_find_replace_visibility(Ihandle* find_dlg, int show_replace)
{
  Ihandle* replace_txt = IupGetDialogChild(find_dlg, "REPLACE_TEXT");
  Ihandle* replace_lbl = IupGetDialogChild(find_dlg, "REPLACE_LABEL");
  Ihandle* replace_bt = IupGetDialogChild(find_dlg, "REPLACE_BUTTON");

  if (show_replace)
  {
    IupSetAttribute(replace_txt, "VISIBLE", "Yes");
    IupSetAttribute(replace_lbl, "VISIBLE", "Yes");
    IupSetAttribute(replace_bt, "VISIBLE", "Yes");
    IupSetAttribute(replace_txt, "FLOATING", "No");
    IupSetAttribute(replace_lbl, "FLOATING", "No");
    IupSetAttribute(replace_bt, "FLOATING", "No");

    IupSetAttribute(find_dlg, "TITLE", "Replace");
  }
  else
  {
    IupSetAttribute(replace_txt, "FLOATING", "Yes");
    IupSetAttribute(replace_lbl, "FLOATING", "Yes");
    IupSetAttribute(replace_bt, "FLOATING", "Yes");
    IupSetAttribute(replace_txt, "VISIBLE", "No");
    IupSetAttribute(replace_lbl, "VISIBLE", "No");
    IupSetAttribute(replace_bt, "VISIBLE", "No");

    IupSetAttribute(find_dlg, "TITLE", "Find");
  }

  IupSetAttribute(find_dlg, "SIZE", NULL);  /* force a dialog resize on the IupRefresh */
  IupRefresh(find_dlg);
}


/********************************** Callbacks *****************************************/


int dropfiles_cb(Ihandle* ih, const char* filename)
{
  if (save_check(ih))
    open_file(ih, filename);

  return IUP_DEFAULT;
}

int marginclick_cb(Ihandle* ih, int margin, int lin, char *status)
{
  (void)status;

  if (margin < 1 || margin > 2)
    return IUP_IGNORE;

  toggleMarker(ih, lin, margin);

  return IUP_DEFAULT;
}

int multitext_valuechanged_cb(Ihandle* multitext)
{
  IupSetAttribute(multitext, "DIRTY", "YES");
  return IUP_DEFAULT;
}

int file_menu_open_cb(Ihandle* ih)
{
  Ihandle* item_revert = IupGetDialogChild(ih, "ITEM_REVERT");
  Ihandle* item_save = IupGetDialogChild(ih, "ITEM_SAVE");
  Ihandle* multitext = IupGetDialogChild(ih, "MULTITEXT");
  char* filename = IupGetAttribute(multitext, "FILENAME");
  int dirty = IupGetInt(multitext, "DIRTY");

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

int edit_menu_open_cb(Ihandle* ih)
{
  Ihandle *clipboard = IupClipboard();
  Ihandle* find_dlg = (Ihandle*)IupGetAttribute(ih, "FIND_DIALOG");

  Ihandle *item_paste = IupGetDialogChild(ih, "ITEM_PASTE");
  Ihandle *item_cut = IupGetDialogChild(ih, "ITEM_CUT");
  Ihandle *item_delete = IupGetDialogChild(ih, "ITEM_DELETE");
  Ihandle *item_copy = IupGetDialogChild(ih, "ITEM_COPY");
  Ihandle *item_find_next = IupGetDialogChild(ih, "ITEM_FINDNEXT");
  Ihandle* multitext = IupGetDialogChild(ih, "MULTITEXT");

  if (!IupGetInt(clipboard, "TEXTAVAILABLE"))
    IupSetAttribute(item_paste, "ACTIVE", "NO");
  else
    IupSetAttribute(item_paste, "ACTIVE", "YES");

  if (!IupGetAttribute(multitext, "SELECTEDTEXT"))
  {
    IupSetAttribute(item_cut, "ACTIVE", "NO");
    IupSetAttribute(item_delete, "ACTIVE", "NO");
    IupSetAttribute(item_copy, "ACTIVE", "NO");
  }
  else
  {
    IupSetAttribute(item_cut, "ACTIVE", "YES");
    IupSetAttribute(item_delete, "ACTIVE", "YES");
    IupSetAttribute(item_copy, "ACTIVE", "YES");
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

  IupDestroy(clipboard);
  return IUP_DEFAULT;
}

int config_recent_cb(Ihandle* ih)
{
  if (save_check(ih))
  {
    char* filename = IupGetAttribute(ih, "TITLE");
    open_file(ih, filename);
  }
  return IUP_DEFAULT;
}

int multitext_caret_cb(Ihandle *ih, int lin, int col)
{
  Ihandle *lbl_statusbar = IupGetDialogChild(ih, "STATUSBAR");
  IupSetfAttribute(lbl_statusbar, "TITLE", "Lin %d, Col %d", lin, col);
  return IUP_DEFAULT;
}

int item_new_action_cb(Ihandle* item_new)
{
  if (save_check(item_new))
    new_file(item_new);

  return IUP_DEFAULT;
}

int item_open_action_cb(Ihandle* item_open)
{
  Ihandle *filedlg;
  Ihandle* config;
  const char* dir;

  if (!save_check(item_open))
    return IUP_DEFAULT;

  config = (Ihandle*)IupGetAttribute(item_open, "CONFIG");
  dir = IupConfigGetVariableStr(config, "MainWindow", "LastDirectory");

  filedlg = IupFileDlg();
  IupSetAttribute(filedlg, "DIALOGTYPE", "OPEN");
  IupSetAttribute(filedlg, "EXTFILTER", "Text Files|*.txt|All Files|*.*|");
  IupSetAttributeHandle(filedlg, "PARENTDIALOG", IupGetDialog(item_open));
  IupSetStrAttribute(filedlg, "DIRECTORY", dir);

  IupPopup(filedlg, IUP_CENTERPARENT, IUP_CENTERPARENT);
  if (IupGetInt(filedlg, "STATUS") != -1)
  {
    char* filename = IupGetAttribute(filedlg, "VALUE");
    open_file(item_open, filename);

    dir = IupGetAttribute(filedlg, "DIRECTORY");
    IupConfigSetVariableStr(config, "MainWindow", "LastDirectory", dir);
  }

  IupDestroy(filedlg);
  return IUP_DEFAULT;
}

int item_saveas_action_cb(Ihandle* item_saveas)
{
  Ihandle* multitext = IupGetDialogChild(item_saveas, "MULTITEXT");
  Ihandle* config = (Ihandle*)IupGetAttribute(multitext, "CONFIG");
  const char* dir = IupConfigGetVariableStr(config, "MainWindow", "LastDirectory");
  Ihandle *filedlg = IupFileDlg();
  IupSetAttribute(filedlg, "DIALOGTYPE", "SAVE");
  IupSetAttribute(filedlg, "EXTFILTER", "Text Files|*.txt|All Files|*.*|");
  IupSetAttributeHandle(filedlg, "PARENTDIALOG", IupGetDialog(item_saveas));
  IupSetStrAttribute(filedlg, "FILE", IupGetAttribute(multitext, "FILENAME"));
  IupSetStrAttribute(filedlg, "DIRECTORY", dir);

  IupPopup(filedlg, IUP_CENTERPARENT, IUP_CENTERPARENT);

  if (IupGetInt(filedlg, "STATUS") != -1)
  {
    char* filename = IupGetAttribute(filedlg, "VALUE");
    saveas_file(multitext, filename);

    dir = IupGetAttribute(filedlg, "DIRECTORY");
    IupConfigSetVariableStr(config, "MainWindow", "LastDirectory", dir);
  }

  IupDestroy(filedlg);
  return IUP_DEFAULT;
}

int item_save_action_cb(Ihandle* item_save)
{
  Ihandle* multitext = IupGetDialogChild(item_save, "MULTITEXT");
  char* filename = IupGetAttribute(multitext, "FILENAME");
  if (!filename)
    item_saveas_action_cb(item_save);
  else
  {
    /* test again because in can be called using the hot key */
    int dirty = IupGetInt(multitext, "DIRTY");
    if (dirty)
      save_file(multitext);
  }
  return IUP_DEFAULT;
}

int item_revert_action_cb(Ihandle* item_revert)
{
  Ihandle* multitext = IupGetDialogChild(item_revert, "MULTITEXT");
  char* filename = IupGetAttribute(multitext, "FILENAME");
  open_file(item_revert, filename);
  return IUP_DEFAULT;
}

int item_exit_action_cb(Ihandle* item_exit)
{
  Ihandle* dlg = IupGetDialog(item_exit);
  Ihandle* config = (Ihandle*)IupGetAttribute(dlg, "CONFIG");

  if (!save_check(item_exit))
    return IUP_IGNORE;  /* to abort the CLOSE_CB callback */

  IupConfigDialogClosed(config, dlg, "MainWindow");
  IupConfigSave(config);
  IupDestroy(config);
  return IUP_CLOSE;
}

int goto_ok_action_cb(Ihandle* bt_ok)
{
  int line_count = IupGetInt(bt_ok, "TEXT_LINECOUNT");
  Ihandle* txt = IupGetDialogChild(bt_ok, "LINE_TEXT");
  int line = IupGetInt(txt, "VALUE");
  if (line < 1 || line >= line_count)
  {
    IupMessage("Error", "Invalid line number.");
    return IUP_DEFAULT;
  }

  IupSetAttribute(IupGetDialog(bt_ok), "STATUS", "1");
  return IUP_CLOSE;
}

int goto_cancel_action_cb(Ihandle* bt_ok)
{
  IupSetAttribute(IupGetDialog(bt_ok), "STATUS", "0");
  return IUP_CLOSE;
}

int item_goto_action_cb(Ihandle* item_goto)
{
  Ihandle* multitext = IupGetDialogChild(item_goto, "MULTITEXT");
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
  IupSetAttributeHandle(goto_dlg, "PARENTDIALOG", IupGetDialog(item_goto));

  IupPopup(goto_dlg, IUP_CENTERPARENT, IUP_CENTERPARENT);

  if (IupGetInt(goto_dlg, "STATUS") == 1)
  {
    int line = IupGetInt(txt, "VALUE");
    int pos;
    IupTextConvertLinColToPos(multitext, line, 0, &pos);
    IupSetInt(multitext, "CARETPOS", pos);
    IupSetInt(multitext, "SCROLLTOPOS", pos);
  }

  IupDestroy(goto_dlg);

  return IUP_DEFAULT;
}

int item_gotombrace_action_cb(Ihandle* ih)
{
  Ihandle* multitext = IupGetDialogChild(ih, "MULTITEXT");

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

int item_togglemark_action_cb(Ihandle* ih)
{
  Ihandle* multitext = IupGetDialogChild(ih, "MULTITEXT");

  int pos = IupGetInt(multitext, "CARETPOS");

  int lin, col;
  IupTextConvertPosToLinCol(multitext, pos, &lin, &col);

  toggleMarker(multitext, lin, 1);

  return IUP_IGNORE;
}

int item_nextmark_action_cb(Ihandle* ih)
{
  Ihandle* multitext = IupGetDialogChild(ih, "MULTITEXT");

  int pos = IupGetInt(multitext, "CARETPOS");

  int lin, col;
  IupTextConvertPosToLinCol(multitext, pos, &lin, &col);

  IupSetIntId(multitext, "MARKERNEXT", lin + 1, setMarkerMask(0));

  lin = IupGetInt(multitext, "LASTMARKERFOUND");

  if (lin == -1)
    return IUP_IGNORE;

  IupTextConvertLinColToPos(multitext, lin, 0, &pos);

  IupSetInt(multitext, "CARETPOS", pos);

  return IUP_DEFAULT;
}

int item_previousmark_action_cb(Ihandle* ih)
{
  Ihandle* multitext = IupGetDialogChild(ih, "MULTITEXT");

  int pos = IupGetInt(multitext, "CARETPOS");

  int lin, col;
  IupTextConvertPosToLinCol(multitext, pos, &lin, &col);

  IupSetIntId(multitext, "MARKERPREVIOUS", lin - 1, setMarkerMask(0));

  lin = IupGetInt(multitext, "LASTMARKERFOUND");

  if (lin == -1)
    return IUP_IGNORE;

  IupTextConvertLinColToPos(multitext, lin, 0, &pos);

  IupSetInt(multitext, "CARETPOS", pos);

  return IUP_DEFAULT;
}

int item_clearmarks_action_cb(Ihandle* ih)
{
  Ihandle* multitext = IupGetDialogChild(ih, "MULTITEXT");
  IupSetInt(multitext, "MARKERDELETEALL", 0);
  return IUP_DEFAULT;
}

int item_copymarked_action_cb(Ihandle* ih)
{
  Ihandle* multitext = IupGetDialogChild(ih, "MULTITEXT");

  copyMarkedLines(multitext);

  return IUP_DEFAULT;
}

int item_cutmarked_action_cb(Ihandle* ih)
{
  Ihandle* multitext = IupGetDialogChild(ih, "MULTITEXT");

  cutMarkedLines(multitext);

  return IUP_DEFAULT;
}

int item_pastetomarked_action_cb(Ihandle* ih)
{
  Ihandle* multitext = IupGetDialogChild(ih, "MULTITEXT");

  pasteToMarkedLines(multitext);

  return IUP_DEFAULT;
}

int item_removemarked_action_cb(Ihandle* ih)
{
  Ihandle* multitext = IupGetDialogChild(ih, "MULTITEXT");

  removeMarkedLines(multitext);

  return IUP_DEFAULT;
}

int item_removeunmarked_action_cb(Ihandle* ih)
{
  Ihandle* multitext = IupGetDialogChild(ih, "MULTITEXT");

  removeUnmarkedLines(multitext);

  return IUP_DEFAULT;
}

int item_invertmarks_action_cb(Ihandle* ih)
{
  Ihandle* multitext = IupGetDialogChild(ih, "MULTITEXT");

  invertMarkedLines(multitext);

  return IUP_DEFAULT;
}

int item_eoltospace_action_cb(Ihandle* ih)
{
  Ihandle* multitext = IupGetDialogChild(ih, "MULTITEXT");

  IupSetAttribute(multitext, "UNDOACTION", "BEGIN");

  changeEolToSpace(multitext);

  IupSetAttribute(multitext, "UNDOACTION", "END");

  return IUP_DEFAULT;
}

int item_removespaceeol_action_cb(Ihandle* ih)
{
  Ihandle* multitext = IupGetDialogChild(ih, "MULTITEXT");

  IupSetAttribute(multitext, "UNDOACTION", "BEGIN");

  removeTrailingSpaces(multitext);

  removeLeadingSpaces(multitext);

  changeEolToSpace(multitext);

  IupSetAttribute(multitext, "UNDOACTION", "END");

  return IUP_DEFAULT;
}

int item_trimtrailing_action_cb(Ihandle* ih)
{
  Ihandle* multitext = IupGetDialogChild(ih, "MULTITEXT");

  IupSetAttribute(multitext, "UNDOACTION", "BEGIN");

  removeTrailingSpaces(multitext);

  IupSetAttribute(multitext, "UNDOACTION", "END");

  return IUP_DEFAULT;
}

int item_trimleading_action_cb(Ihandle* ih)
{
  Ihandle* multitext = IupGetDialogChild(ih, "MULTITEXT");

  IupSetAttribute(multitext, "UNDOACTION", "BEGIN");

  removeLeadingSpaces(multitext);

  IupSetAttribute(multitext, "UNDOACTION", "END");

  return IUP_DEFAULT;
}

int item_trimtraillead_action_cb(Ihandle* ih)
{
  Ihandle* multitext = IupGetDialogChild(ih, "MULTITEXT");

  IupSetAttribute(multitext, "UNDOACTION", "BEGIN");

  removeTrailingSpaces(multitext);

  removeLeadingSpaces(multitext);

  IupSetAttribute(multitext, "UNDOACTION", "END");

  return IUP_DEFAULT;
}

int item_tabtospace_action_cb(Ihandle* ih)
{
  Ihandle* multitext = IupGetDialogChild(ih, "MULTITEXT");

  IupSetAttribute(multitext, "UNDOACTION", "BEGIN");

  changeTabsToSpaces(multitext);

  IupSetAttribute(multitext, "UNDOACTION", "END");

  return IUP_DEFAULT;
}

int item_allspacetotab_action_cb(Ihandle* ih)
{
  Ihandle* multitext = IupGetDialogChild(ih, "MULTITEXT");

  IupSetAttribute(multitext, "UNDOACTION", "BEGIN");

  changeTabsToSpaces(multitext);

  changeSpacesToTabs(multitext);

  IupSetAttribute(multitext, "UNDOACTION", "END");

  return IUP_DEFAULT;
}

int item_leadingspacetotab_action_cb(Ihandle* ih)
{
  Ihandle* multitext = IupGetDialogChild(ih, "MULTITEXT");

  IupSetAttribute(multitext, "UNDOACTION", "BEGIN");

  changeLeadingSpacesToTabs(multitext);

  IupSetAttribute(multitext, "UNDOACTION", "END");

  return IUP_DEFAULT;
}

int find_next_action_cb(Ihandle* ih)
{
  /* this callback can be called from the main dialog also */
  Ihandle* find_dlg = (Ihandle*)IupGetAttribute(ih, "FIND_DIALOG");
  if (find_dlg)
  {
    char* str;
    int pos;
    Ihandle* multitext = (Ihandle*)IupGetAttribute(find_dlg, "MULTITEXT");
    str = IupGetAttribute(multitext, "VALUE");

    Ihandle* txt = IupGetDialogChild(find_dlg, "FIND_TEXT");
    char* str_to_find = IupGetAttribute(txt, "VALUE");

    Ihandle* wrapHnd = IupGetDialogChild(find_dlg, "WRAP");
    int wrap = IupGetInt(wrapHnd, "VALUE");

    Ihandle* downHnd = IupGetDialogChild(find_dlg, "DOWN");
    int down = IupGetInt(downHnd, "VALUE");

    Ihandle* findHnd = IupGetDialogChild(find_dlg, "FIND_CASE");
    int casesensitive = IupGetInt(findHnd, "VALUE");
    Ihandle* wholeHnd = IupGetDialogChild(find_dlg, "WHOLE_WORD");
    int whole_word = IupGetInt(wholeHnd, "VALUE");
    Ihandle* wordHnd = IupGetDialogChild(find_dlg, "WORD_START");
    int word_start = IupGetInt(wordHnd, "VALUE");
    Ihandle* regexpHnd = IupGetDialogChild(find_dlg, "REGEXP");
    int regexp = IupGetInt(regexpHnd, "VALUE");
    Ihandle* posixHnd = IupGetDialogChild(find_dlg, "POSIX");
    int posix = IupGetInt(posixHnd, "VALUE");

    char flags[80];
    flags[0] = 0;
    IupSetAttribute(multitext, "SEARCHFLAGS", NULL);
    if (casesensitive)
      strcpy(flags, "MATCHCASE");
    if (whole_word)
      strcat((flags[0] != 0 ? strcat(flags, " | ") : flags), "WHOLEWORD");
    if (word_start)
      strcat((flags[0] != 0 ? strcat(flags, " | ") : flags), "WORDSTART");
    if (regexp)
      strcat((flags[0] != 0 ? strcat(flags, " | ") : flags), "REGEXP");
    if (posix)
      strcat((flags[0] != 0 ? strcat(flags, " | ") : flags), "POSIX");

    if (flags[0] != 0)
      IupSetAttribute(multitext, "SEARCHFLAGS", flags);

    /* test again, because it can be called from the hot key */
    if (!str_to_find || str_to_find[0] == 0)
      return IUP_DEFAULT;

    char *sel;
    int find_pos = IupGetInt(multitext, "CARETPOS");
    sel = IupGetAttribute(multitext, "SELECTIONPOS");

    if (!down && sel)
    {
      int st, ed;
      IupGetIntInt(multitext, "SELECTIONPOS", &st, &ed);
      find_pos = st;
    }

    IupSetInt(multitext, "TARGETSTART", find_pos);
    IupSetInt(multitext, "TARGETEND", down ? IupGetInt(multitext, "COUNT") - 1 : 1);

    IupSetAttribute(multitext, "SEARCHINTARGET", str_to_find);

    pos = IupGetInt(multitext, "TARGETSTART");

    if (pos == find_pos && wrap)
    {
      IupSetInt(multitext, "TARGETSTART", down ? 1 : IupGetInt(multitext, "COUNT") - 1);
      IupSetInt(multitext, "TARGETEND", find_pos);

      IupSetAttribute(multitext, "SEARCHINTARGET", str_to_find);

      pos = IupGetInt(multitext, "TARGETSTART");
    }

    if (pos != find_pos)
    {
      int lin, col, end_pos, start_pos;

      start_pos = IupGetInt(multitext, "TARGETSTART");
      end_pos = IupGetInt(multitext, "TARGETEND");

      IupSetFocus(multitext);
      IupSetfAttribute(multitext, "SELECTIONPOS", "%d:%d", pos, end_pos);

      //IupSetInt(multitext, "TARGETSTART", down ? end_pos : start_pos);
      //IupSetInt(multitext, "TARGETEND", down ? IupGetInt(multitext, "COUNT") - 1 : end_pos);

      IupTextConvertPosToLinCol(multitext, end_pos, &lin, &col);
      multitext_caret_cb(multitext, lin, col);
    }
    else
      IupMessage("Warning", "Text not found.");
  }

  return IUP_DEFAULT;
}

int find_replace_action_cb(Ihandle* bt_replace)
{
  Ihandle* find_dlg = (Ihandle*)IupGetAttribute(bt_replace, "FIND_DIALOG");
  Ihandle* multitext = (Ihandle*)IupGetAttribute(find_dlg, "MULTITEXT");
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

int find_close_action_cb(Ihandle* bt_close)
{
  Ihandle* find_dlg = IupGetDialog(bt_close);
  Ihandle* multitext = (Ihandle*)IupGetAttribute(find_dlg, "MULTITEXT");
  Ihandle* config = (Ihandle*)IupGetAttribute(multitext, "CONFIG");
  IupConfigDialogClosed(config, find_dlg, "FindDialog");
  IupHide(find_dlg);  /* do not destroy, just hide */
  return IUP_DEFAULT;
}

Ihandle* create_find_dialog(Ihandle *multitext)
{
  Ihandle *box, *bt_next, *bt_close, *txt, *find_dlg;
  Ihandle *find_case, *whole_word, *word_start, *reg_exp, *posix, *wrap, *up, *down;
  Ihandle *flags, *direction, *radio;
  Ihandle *txt_replace, *bt_replace;

  txt = IupText(NULL);
  IupSetAttribute(txt, "NAME", "FIND_TEXT");
  IupSetAttribute(txt, "VISIBLECOLUMNS", "20");
  txt_replace = IupText(NULL);
  IupSetAttribute(txt_replace, "NAME", "REPLACE_TEXT");
  IupSetAttribute(txt_replace, "VISIBLECOLUMNS", "20");
  find_case = IupToggle("Case Sensitive", NULL);
  IupSetAttribute(find_case, "NAME", "FIND_CASE");
  whole_word = IupToggle("Whole Word", NULL);
  IupSetAttribute(whole_word, "NAME", "WHOLE_WORD");
  word_start = IupToggle("Word Start", NULL);
  IupSetAttribute(word_start, "NAME", "WORD_START");
  reg_exp = IupToggle("Regular Expression", NULL);
  IupSetAttribute(reg_exp, "NAME", "REG_EXP");
  posix = IupToggle("Posix", NULL);
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
  bt_replace = IupButton("Replace", NULL);
  IupSetAttribute(bt_replace, "PADDING", "10x2");
  IupSetCallback(bt_replace, "ACTION", (Icallback)find_replace_action_cb);
  IupSetAttribute(bt_replace, "NAME", "REPLACE_BUTTON");
  bt_close = IupButton("Close", NULL);
  IupSetCallback(bt_close, "ACTION", (Icallback)find_close_action_cb);
  IupSetAttribute(bt_close, "PADDING", "10x2");

  flags = IupVbox(find_case,
                  whole_word,
                  word_start,
                  reg_exp,
                  posix,
                  wrap,
                  NULL);

  radio = IupRadio(IupVbox(up,
    down,
    NULL));

  IupSetAttribute(radio, "VALUE_HANDLE", (char*)down);

  direction = IupFrame(radio);

  IupSetAttribute(direction, "TITLE", "Direction");

  box = IupVbox(
    IupLabel("Find What:"),
    txt,
    IupSetAttributes(IupLabel("Replace with:"), "NAME=REPLACE_LABEL"),
    txt_replace,
    IupHbox(
    flags,
    direction,
    NULL),
    IupSetAttributes(IupHbox(
    IupFill(),
    bt_next,
    bt_replace,
    bt_close,
    NULL), "NORMALIZESIZE=HORIZONTAL"),
    NULL);
  IupSetAttribute(box, "NMARGIN", "10x10");
  IupSetAttribute(box, "GAP", "5");

  find_dlg = IupDialog(box);
  IupSetAttribute(find_dlg, "TITLE", "Find");
  IupSetAttribute(find_dlg, "DIALOGFRAME", "Yes");
  IupSetAttributeHandle(find_dlg, "DEFAULTENTER", bt_next);
  IupSetAttributeHandle(find_dlg, "DEFAULTESC", bt_close);
  IupSetAttributeHandle(find_dlg, "PARENTDIALOG", IupGetDialog(multitext));
  IupSetCallback(find_dlg, "CLOSE_CB", (Icallback)find_close_action_cb);

  /* Save the multiline to access it from the callbacks */
  IupSetAttribute(find_dlg, "MULTITEXT", (char*)multitext);

  IupSetInt(multitext, "TARGETSTART", IupGetInt(multitext, "CARETPOS"));
  IupSetInt(multitext, "TARGETEND", IupGetInt(multitext, "COUNT") - 1);

  /* Save the dialog to reuse it */
  IupSetAttribute(find_dlg, "FIND_DIALOG", (char*)find_dlg);  /* from itself */
  IupSetAttribute(IupGetDialog(multitext), "FIND_DIALOG", (char*)find_dlg); /* from the main dialog */

  return find_dlg;
}

int item_find_action_cb(Ihandle* item_find)
{
  Ihandle* find_dlg = (Ihandle*)IupGetAttribute(item_find, "FIND_DIALOG");
  Ihandle* multitext = IupGetDialogChild(item_find, "MULTITEXT");
  Ihandle* config = (Ihandle*)IupGetAttribute(multitext, "CONFIG");
  char* str;

  if (!find_dlg)
    find_dlg = create_find_dialog(multitext);

  set_find_replace_visibility(find_dlg, 0);

  IupConfigDialogShow(config, find_dlg, "FindDialog");

  str = IupGetAttribute(multitext, "SELECTEDTEXT");
  if (str && str[0] != 0)
  {
    Ihandle* txt = IupGetDialogChild(find_dlg, "FIND_TEXT");
    IupSetStrAttribute(txt, "VALUE", str);
  }

  return IUP_DEFAULT;
}

int item_replace_action_cb(Ihandle* item_replace)
{
  Ihandle* find_dlg = (Ihandle*)IupGetAttribute(item_replace, "FIND_DIALOG");
  Ihandle* multitext = IupGetDialogChild(item_replace, "MULTITEXT");
  Ihandle* config = (Ihandle*)IupGetAttribute(multitext, "CONFIG");
  char* str;

  if (!find_dlg)
    find_dlg = create_find_dialog(multitext);

  set_find_replace_visibility(find_dlg, 1);

  IupConfigDialogShow(config, find_dlg, "FindDialog");

  str = IupGetAttribute(multitext, "SELECTEDTEXT");
  if (str && str[0] != 0)
  {
    Ihandle* txt = IupGetDialogChild(find_dlg, "FIND_TEXT");
    IupSetStrAttribute(txt, "VALUE", str);
  }

  return IUP_IGNORE;  /* replace system processing for the hot key */
}

int selection_find_next_action_cb(Ihandle* ih)
{
  Ihandle* multitext = IupGetDialogChild(ih, "MULTITEXT");

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

int item_copy_action_cb(Ihandle* item_copy)
{
  Ihandle* multitext = IupGetDialogChild(item_copy, "MULTITEXT");
  Ihandle *clipboard = IupClipboard();
  IupSetAttribute(clipboard, "TEXT", IupGetAttribute(multitext, "SELECTEDTEXT"));
  IupDestroy(clipboard);
  return IUP_DEFAULT;
}

int item_paste_action_cb(Ihandle* item_paste)
{
  Ihandle* multitext = IupGetDialogChild(item_paste, "MULTITEXT");
  Ihandle *clipboard = IupClipboard();
  IupSetAttribute(multitext, "INSERT", IupGetAttribute(clipboard, "TEXT"));
  IupDestroy(clipboard);
  return IUP_IGNORE;  /* replace system processing for the hot key, to correctly parse line feed */
}

int item_cut_action_cb(Ihandle* item_cut)
{
  Ihandle* multitext = IupGetDialogChild(item_cut, "MULTITEXT");
  Ihandle *clipboard = IupClipboard();
  IupSetAttribute(clipboard, "TEXT", IupGetAttribute(multitext, "SELECTEDTEXT"));
  IupSetAttribute(multitext, "SELECTEDTEXT", "");
  IupDestroy(clipboard);
  return IUP_DEFAULT;
}

int item_delete_action_cb(Ihandle* item_delete)
{
  Ihandle* multitext = IupGetDialogChild(item_delete, "MULTITEXT");
  IupSetAttribute(multitext, "SELECTEDTEXT", "");
  return IUP_DEFAULT;
}

int item_select_all_action_cb(Ihandle* item_select_all)
{
  Ihandle* multitext = IupGetDialogChild(item_select_all, "MULTITEXT");
  IupSetFocus(multitext);
  int count = IupGetInt(multitext, "COUNT");
  IupSetStrf(multitext, "SELECTIONPOS", "%d:%d", 0, count - 1);
  return IUP_DEFAULT;
}

int item_undo_action_cb(Ihandle* item_select_all)
{
  Ihandle* multitext = IupGetDialogChild(item_select_all, "MULTITEXT");
  IupSetAttribute(multitext, "UNDO", "YES");
  return IUP_DEFAULT;
}

int item_redo_action_cb(Ihandle* item_select_all)
{
  Ihandle* multitext = IupGetDialogChild(item_select_all, "MULTITEXT");
  IupSetAttribute(multitext, "REDO", "YES");
  return IUP_DEFAULT;
}

void StrUpper(char* sstr)
{
  if (!sstr || sstr[0] == 0) return;
  for (; *sstr; sstr++)
    *sstr = (char)toupper(*sstr);
}

void StrLower(char* sstr)
{
  if (!sstr || sstr[0] == 0) return;
  for (; *sstr; sstr++)
    *sstr = (char)tolower(*sstr);
}

char* StrDup(const char *str)
{
  if (str)
  {
    int size = (int)strlen(str) + 1;
    char *newstr = malloc(size);
    if (newstr) memcpy(newstr, str, size);
    return newstr;
  }
  return NULL;
}

int item_uppercase_action_cb(Ihandle* item)
{
  char *text;
  int start, end;
  Ihandle* multitext = IupGetDialogChild(item, "MULTITEXT");
  IupGetIntInt(multitext, "SELECTIONPOS", &start, &end);
  text = IupGetAttribute(multitext, "SELECTEDTEXT");
  text = StrDup(text);
  StrUpper(text);
  IupSetAttribute(multitext, "SELECTEDTEXT", text);
  IupSetStrf(multitext, "SELECTIONPOS", "%d:%d", start, end);
  free(text);
  return IUP_DEFAULT;
}

int item_lowercase_action_cb(Ihandle* item)
{
  char *text;
  int start, end;
  Ihandle* multitext = IupGetDialogChild(item, "MULTITEXT");
  IupGetIntInt(multitext, "SELECTIONPOS", &start, &end);
  text = IupGetAttribute(multitext, "SELECTEDTEXT");
  text = StrDup(text);
  StrLower(text);
  IupSetAttribute(multitext, "SELECTEDTEXT", text);
  IupSetStrf(multitext, "SELECTIONPOS", "%d:%d", start, end);
  free(text);
  return IUP_DEFAULT;
}

int item_case_action_cb(Ihandle* item)
{
  char* shift = IupGetGlobal("SHIFTKEY");

  if (strcmp(shift, "ON") == 0)
    item_uppercase_action_cb(item);
  else
    item_lowercase_action_cb(item);

  return IUP_DEFAULT;
}

int item_font_action_cb(Ihandle* item_font)
{
  Ihandle* multitext = IupGetDialogChild(item_font, "MULTITEXT");
  Ihandle* fontdlg = IupFontDlg();
  char* font = IupGetAttribute(multitext, "FONT");
  IupSetStrAttribute(fontdlg, "VALUE", font);
  IupSetAttributeHandle(fontdlg, "PARENTDIALOG", IupGetDialog(item_font));

  IupPopup(fontdlg, IUP_CENTERPARENT, IUP_CENTERPARENT);

  if (IupGetInt(fontdlg, "STATUS") == 1)
  {
    Ihandle* config = (Ihandle*)IupGetAttribute(multitext, "CONFIG");
    font = IupGetAttribute(fontdlg, "VALUE");
    IupSetStrAttribute(multitext, "FONT", font);

    IupConfigSetVariableStr(config, "MainWindow", "Font", font);
  }

  IupDestroy(fontdlg);
  return IUP_DEFAULT;
}

int item_tab_action_cb(Ihandle* item_font)
{
  Ihandle* multitext = IupGetDialogChild(item_font, "MULTITEXT");

  int replaceBySpace = !IupGetInt(multitext, "USETABS");
  int tabSize = IupGetInt(multitext, "TABSIZE");

  if (!IupGetParam("Tab Settings", NULL,
    0,
    "Size: %i\n"
    "Replace by Whitespace: %b\n", &tabSize, &replaceBySpace))
    return IUP_IGNORE;

  IupSetInt(multitext, "TABSIZE", tabSize);
  IupSetInt(multitext, "USETABS", !replaceBySpace);

  return IUP_DEFAULT;
}

int item_zoomin_action_cb(Ihandle* item_toolbar)
{
  Ihandle* multitext = IupGetDialogChild(item_toolbar, "MULTITEXT");

  IupSetAttribute(multitext, "ZOOMIN", "10");

  return IUP_DEFAULT;
}

int item_zoomout_action_cb(Ihandle* item_toolbar)
{
  Ihandle* multitext = IupGetDialogChild(item_toolbar, "MULTITEXT");

  IupSetAttribute(multitext, "ZOOMOUT", "10");

  return IUP_DEFAULT;
}

int item_restorezoom_action_cb(Ihandle* item_toolbar)
{
  Ihandle* multitext = IupGetDialogChild(item_toolbar, "MULTITEXT");

  IupSetAttribute(multitext, "ZOOM", "0");

  return IUP_DEFAULT;
}

int item_wordwrap_action_cb(Ihandle* item_wordwrap)
{
  Ihandle* multitext = IupGetDialogChild(item_wordwrap, "MULTITEXT");

  if (IupGetInt(item_wordwrap, "VALUE"))
    IupSetAttribute(multitext, "WORDWRAP", "WORD");
  else
    IupSetAttribute(multitext, "WORDWRAP", "NONE");

  return IUP_DEFAULT;
}

int item_showwhite_action_cb(Ihandle* item_showwhite)
{
  Ihandle* multitext = IupGetDialogChild(item_showwhite, "MULTITEXT");

  if (IupGetInt(item_showwhite, "VALUE"))
    IupSetAttribute(multitext, "WHITESPACEVIEW", "VISIBLEALWAYS");
  else
    IupSetAttribute(multitext, "WHITESPACEVIEW", "INVISIBLE");

  return IUP_DEFAULT;
}

int item_toolbar_action_cb(Ihandle* item_toolbar)
{
  Ihandle* multitext = IupGetDialogChild(item_toolbar, "MULTITEXT");
  Ihandle* toolbar = IupGetChild(IupGetParent(multitext), 0);
  Ihandle* config = (Ihandle*)IupGetAttribute(multitext, "CONFIG");

  toggle_bar_visibility(item_toolbar, toolbar);

  IupConfigSetVariableStr(config, "MainWindow", "Toolbar", IupGetAttribute(item_toolbar, "VALUE"));
  return IUP_DEFAULT;
}

int item_statusbar_action_cb(Ihandle* item_statusbar)
{
  Ihandle* multitext = IupGetDialogChild(item_statusbar, "MULTITEXT");
  Ihandle* statusbar = IupGetBrother(multitext);
  Ihandle* config = (Ihandle*)IupGetAttribute(multitext, "CONFIG");

  toggle_bar_visibility(item_statusbar, statusbar);

  IupConfigSetVariableStr(config, "MainWindow", "Statusbar", IupGetAttribute(item_statusbar, "VALUE"));
  return IUP_DEFAULT;
}

int item_linenumber_action_cb(Ihandle* item_linenumber)
{
  Ihandle* multitext = IupGetDialogChild(item_linenumber, "MULTITEXT");

  if (IupGetInt(item_linenumber, "VALUE"))
  {
    IupSetInt(multitext, "MARGINWIDTH0", 0);
    IupSetAttribute(item_linenumber, "VALUE", "OFF");
  }
  else
  {
    IupSetInt(multitext, "MARGINWIDTH0", 50);
    IupSetAttribute(item_linenumber, "VALUE", "ON");
  }

  return IUP_DEFAULT;
}

int item_bookmark_action_cb(Ihandle* item_bookmark)
{
  Ihandle* multitext = IupGetDialogChild(item_bookmark, "MULTITEXT");

  if (IupGetInt(item_bookmark, "VALUE"))
  {
    IupSetInt(multitext, "MARGINWIDTH1", 0);
    IupSetAttribute(item_bookmark, "VALUE", "OFF");
  }
  else
  {
    IupSetInt(multitext, "MARGINWIDTH1", 20);
    IupSetAttribute(item_bookmark, "VALUE", "ON");
  }

  return IUP_DEFAULT;
}

int item_help_action_cb(void)
{
  IupHelp("http://www.tecgraf.puc-rio.br/iup");
  return IUP_DEFAULT;
}

int item_about_action_cb(void)
{
  IupMessage("About", "   Scintilla Notepad\n\nAutors:\n   Camilo Freire\n   Antonio Scuri");
  return IUP_DEFAULT;
}


/********************************** Main *****************************************/


Ihandle* create_main_dialog(Ihandle *config)
{
  Ihandle *dlg, *vbox, *multitext, *menu;
  Ihandle *sub_menu_file, *file_menu, *item_exit, *item_new, *item_open, *item_save, *item_saveas, *item_revert;
  Ihandle *sub_menu_edit, *edit_menu, *item_find, *item_find_next, *item_goto, *item_gotombrace, *item_copy, *item_paste, *item_cut, *item_delete, *item_select_all;
  Ihandle *item_togglemark, *item_nextmark, *item_previousmark, *item_clearmarks, *item_cutmarked, *item_copymarked, *item_pastetomarked, *item_removemarked,
    *item_removeunmarked, *item_invertmarks, *item_tabtospace, *item_allspacetotab, *item_leadingspacetotab;
  Ihandle *item_trimleading, *item_trimtrailing, *item_trimtraillead, *item_eoltospace, *item_removespaceeol;
  Ihandle *item_undo, *item_redo;
  Ihandle *case_menu, *item_uppercase, *item_lowercase;
  Ihandle *btn_cut, *btn_copy, *btn_paste, *btn_find, *btn_new, *btn_open, *btn_save;
  Ihandle *sub_menu_format, *format_menu, *item_font, *item_tab, *item_replace;
  Ihandle *sub_menu_help, *help_menu, *item_help, *item_about;
  Ihandle *sub_menu_view, *view_menu, *item_toolbar, *item_statusbar, *item_linenumber, *item_bookmark;
  Ihandle *zoom_menu, *item_zoomin, *item_zoomout, *item_restorezoom;
  Ihandle *lbl_statusbar, *toolbar_hb, *recent_menu;
  Ihandle *item_wordwrap, *item_showwhite;
  const char* font;

  multitext = IupScintilla();
  IupSetAttribute(multitext, "MULTILINE", "YES");
  IupSetAttribute(multitext, "EXPAND", "YES");
  IupSetAttribute(multitext, "NAME", "MULTITEXT");
  IupSetAttribute(multitext, "DIRTY", "NO");
  IupSetCallback(multitext, "CARET_CB", (Icallback)multitext_caret_cb);
  IupSetCallback(multitext, "VALUECHANGED_CB", (Icallback)multitext_valuechanged_cb);
  IupSetCallback(multitext, "DROPFILES_CB", (Icallback)dropfiles_cb);
  IupSetCallback(multitext, "MARGINCLICK_CB", (Icallback)marginclick_cb);

  IupSetAttribute(multitext, "STYLEFGCOLOR34", "255 0 0");
  /* line numbers */
  IupSetInt(multitext, "MARGINWIDTH0", 30);
  IupSetAttribute(multitext, "MARGINSENSITIVE0", "YES");
  /* bookmarks */
  IupSetInt(multitext, "MARGINWIDTH1", 15);
  IupSetAttribute(multitext, "MARGINTYPE1", "SYMBOL");
  IupSetAttribute(multitext, "MARGINSENSITIVE1", "YES");
  IupSetAttribute(multitext, "MARGINMASKFOLDERS1", "NO");

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
  IupSetAttribute(btn_save, "TIP", "Save (Ctrl+S)");
  IupSetAttribute(btn_save, "CANFOCUS", "No");

  item_saveas = IupItem("Save &As...", NULL);
  IupSetAttribute(item_saveas, "NAME", "ITEM_SAVEAS");
  IupSetCallback(item_saveas, "ACTION", (Icallback)item_saveas_action_cb);

  item_revert = IupItem("&Revert", NULL);
  IupSetAttribute(item_revert, "NAME", "ITEM_REVERT");
  IupSetCallback(item_revert, "ACTION", (Icallback)item_revert_action_cb);

  item_exit = IupItem("E&xit", NULL);
  IupSetCallback(item_exit, "ACTION", (Icallback)item_exit_action_cb);

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
  IupSetCallback(item_undo, "ACTION", (Icallback)item_undo_action_cb);

  item_redo = IupItem("Redo\tCtrl+Y", NULL);
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

  item_removeunmarked = IupItem("Remove unmarked Lines", NULL);
  IupSetCallback(item_removeunmarked, "ACTION", (Icallback)item_removeunmarked_action_cb);

  item_invertmarks = IupItem("Inverse Bookmark", NULL);
  IupSetCallback(item_invertmarks, "ACTION", (Icallback)item_invertmarks_action_cb);

  item_trimtrailing = IupItem("Trim Trailing Space", NULL);
  IupSetCallback(item_trimtrailing, "ACTION", (Icallback)item_trimtrailing_action_cb);

  item_trimtraillead = IupItem("Trim Trailing and Leading Space", NULL);
  IupSetCallback(item_trimtraillead, "ACTION", (Icallback)item_trimtraillead_action_cb);

  item_eoltospace = IupItem("EOL to Space", NULL);
  IupSetCallback(item_eoltospace, "ACTION", (Icallback)item_eoltospace_action_cb);

  item_removespaceeol = IupItem("Remove Unnecessary Blanks and EOL", NULL);
  IupSetCallback(item_removespaceeol, "ACTION", (Icallback)item_removespaceeol_action_cb);

  item_trimleading = IupItem("Trim Leading Space", NULL);
  IupSetCallback(item_trimleading, "ACTION", (Icallback)item_trimleading_action_cb);

  item_tabtospace = IupItem("TAB to Space", NULL);
  IupSetCallback(item_tabtospace, "ACTION", (Icallback)item_tabtospace_action_cb);

  item_allspacetotab = IupItem("Space to TAB (All)", NULL);
  IupSetCallback(item_allspacetotab, "ACTION", (Icallback)item_allspacetotab_action_cb);

  item_leadingspacetotab = IupItem("Space to TAB (Leading)", NULL);
  IupSetCallback(item_leadingspacetotab, "ACTION", (Icallback)item_leadingspacetotab_action_cb);

  item_zoomin = IupItem("Zoom In\tCtrl_Num +", NULL);
  IupSetCallback(item_zoomin, "ACTION", (Icallback)item_zoomin_action_cb);

  item_zoomout = IupItem("Zoom Out\tCtrl_Num -", NULL);
  IupSetCallback(item_zoomout, "ACTION", (Icallback)item_zoomout_action_cb);

  item_restorezoom = IupItem("Restore Default Zoom\tCtrl_Num /", NULL);
  IupSetCallback(item_restorezoom, "ACTION", (Icallback)item_restorezoom_action_cb);

  item_wordwrap = IupItem("Word Wrap", NULL);
  IupSetCallback(item_wordwrap, "ACTION", (Icallback)item_wordwrap_action_cb);
  IupSetAttribute(item_wordwrap, "AUTOTOGGLE", "YES");

  item_showwhite = IupItem("Show White Spaces", NULL);
  IupSetCallback(item_showwhite, "ACTION", (Icallback)item_showwhite_action_cb);
  IupSetAttribute(item_showwhite, "AUTOTOGGLE", "YES");

  item_toolbar = IupItem("&Toobar", NULL);
  IupSetCallback(item_toolbar, "ACTION", (Icallback)item_toolbar_action_cb);
  IupSetAttribute(item_toolbar, "VALUE", "ON");

  item_statusbar = IupItem("&Statusbar", NULL);
  IupSetCallback(item_statusbar, "ACTION", (Icallback)item_statusbar_action_cb);
  IupSetAttribute(item_statusbar, "VALUE", "ON");

  item_linenumber = IupItem("Display Line Numbers", NULL);
  IupSetCallback(item_linenumber, "ACTION", (Icallback)item_linenumber_action_cb);
  IupSetAttribute(item_linenumber, "VALUE", "ON");

  item_bookmark = IupItem("Display Bookmarks", NULL);
  IupSetCallback(item_bookmark, "ACTION", (Icallback)item_bookmark_action_cb);
  IupSetAttribute(item_bookmark, "VALUE", "ON");

  item_font = IupItem("&Font...", NULL);
  IupSetCallback(item_font, "ACTION", (Icallback)item_font_action_cb);

  item_tab = IupItem("Tab...", NULL);
  IupSetCallback(item_tab, "ACTION", (Icallback)item_tab_action_cb);

  item_help = IupItem("&Help...", NULL);
  IupSetCallback(item_help, "ACTION", (Icallback)item_help_action_cb);

  item_about = IupItem("&About...", NULL);
  IupSetCallback(item_about, "ACTION", (Icallback)item_about_action_cb);

  recent_menu = IupMenu(NULL);

  file_menu = IupMenu(
    item_new,
    item_open,
    item_save,
    item_saveas,
    item_revert,
    IupSeparator(),
    IupSubmenu("Recent &Files", recent_menu),
    item_exit,
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
      item_removeunmarked,
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
    IupSeparator(),
    item_toolbar,
    item_statusbar,
    item_linenumber,
    item_bookmark,
    NULL);
  help_menu = IupMenu(
    item_help,
    item_about,
    NULL);

  IupSetCallback(file_menu, "OPEN_CB", (Icallback)file_menu_open_cb);
  IupSetCallback(edit_menu, "OPEN_CB", (Icallback)edit_menu_open_cb);

  sub_menu_file = IupSubmenu("&File", file_menu);
  sub_menu_edit = IupSubmenu("&Edit", edit_menu);
  sub_menu_format = IupSubmenu("F&ormat", format_menu);
  sub_menu_view = IupSubmenu("&View", view_menu);
  sub_menu_help = IupSubmenu("&Help", help_menu);

  menu = IupMenu(
    sub_menu_file,
    sub_menu_edit,
    sub_menu_format,
    sub_menu_view,
    sub_menu_help,
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
    multitext,
    lbl_statusbar,
    NULL);

  dlg = IupDialog(vbox);
  IupSetAttributeHandle(dlg, "MENU", menu);
  IupSetAttribute(dlg, "SIZE", "HALFxHALF");
  IupSetCallback(dlg, "CLOSE_CB", (Icallback)item_exit_action_cb);
  IupSetCallback(dlg, "DROPFILES_CB", (Icallback)dropfiles_cb);

  IupSetCallback(dlg, "K_cN", (Icallback)item_new_action_cb);
  IupSetCallback(dlg, "K_cO", (Icallback)item_open_action_cb);
  IupSetCallback(dlg, "K_cS", (Icallback)item_save_action_cb);
  IupSetCallback(dlg, "K_cF", (Icallback)item_find_action_cb);
  IupSetCallback(dlg, "K_cH", (Icallback)item_replace_action_cb);  /* replace system processing */
  IupSetCallback(dlg, "K_cG", (Icallback)item_goto_action_cb);
  IupSetCallback(dlg, "K_cB", (Icallback)item_gotombrace_action_cb);
  IupSetCallback(dlg, "K_cF2", (Icallback)item_togglemark_action_cb);
  IupSetCallback(dlg, "K_F2", (Icallback)item_nextmark_action_cb);
  IupSetCallback(dlg, "K_sF2", (Icallback)item_previousmark_action_cb);
  IupSetCallback(dlg, "K_F3", (Icallback)find_next_action_cb);
  IupSetCallback(dlg, "K_cF3", (Icallback)selection_find_next_action_cb);
  IupSetCallback(dlg, "K_cV", (Icallback)item_paste_action_cb);  /* replace system processing */
  IupSetCallback(dlg, "K_c+", (Icallback)item_zoomin_action_cb);
  IupSetCallback(dlg, "K_c-", (Icallback)item_zoomout_action_cb);
  IupSetCallback(dlg, "K_c/", (Icallback)item_restorezoom_action_cb);
  IupSetCallback(dlg, "K_cU", (Icallback)item_case_action_cb);
  /* Ctrl+C, Ctrl+X, Ctrl+A, Del, already implemented inside IupText */

  /* parent for pre-defined dialogs in closed functions (IupMessage and IupAlarm) */
  IupSetAttributeHandle(NULL, "PARENTDIALOG", dlg);

  /* Initialize variables from the configuration file */

  IupConfigRecentInit(config, recent_menu, config_recent_cb, 10);

  font = IupConfigGetVariableStr(config, "MainWindow", "Font");
  if (font)
    IupSetStrAttribute(multitext, "FONT", font);

  IupSetAttribute(multitext, "WORDWRAPVISUALFLAGS", "MARGIN");
  /* line numbers */
  IupSetAttributeId(multitext, "MARKERFGCOLOR", 0, "0 0 255");
  IupSetAttributeId(multitext, "MARKERBGCOLOR", 0, "0 0 255");
  IupSetAttributeId(multitext, "MARKERALPHA", 0, "80");
  IupSetAttributeId(multitext, "MARKERSYMBOL", 0, "CIRCLE");
  /* bookmarks */
  IupSetIntId(multitext, "MARGINMASK", 1, 0x000005);
  IupSetAttributeId(multitext, "MARKERFGCOLOR", 1, "255 0 0");
  IupSetAttributeId(multitext, "MARKERBGCOLOR", 1, "255 0 0");
  IupSetAttributeId(multitext, "MARKERALPHA", 1, "80");
  IupSetAttributeId(multitext, "MARKERSYMBOL", 1, "CIRCLE");

  if (!IupConfigGetVariableIntDef(config, "MainWindow", "Toolbar", 1))
  {
    IupSetAttribute(item_toolbar, "VALUE", "OFF");

    IupSetAttribute(toolbar_hb, "FLOATING", "YES");
    IupSetAttribute(toolbar_hb, "VISIBLE", "NO");
  }

  if (!IupConfigGetVariableIntDef(config, "MainWindow", "Statusbar", 1))
  {
    IupSetAttribute(item_statusbar, "VALUE", "OFF");

    IupSetAttribute(lbl_statusbar, "FLOATING", "YES");
    IupSetAttribute(lbl_statusbar, "VISIBLE", "NO");
  }

  IupSetAttribute(dlg, "CONFIG", (char*)config);

  return dlg;
}

int main(int argc, char **argv)
{
  Ihandle *main_dialog;
  Ihandle *config;

  IupOpen(&argc, &argv);
  IupImageLibOpen();

  IupScintillaOpen();

  config = IupConfig();
  IupSetAttribute(config, "APP_NAME", "scintilla_notepad");
  IupConfigLoad(config);

  main_dialog = create_main_dialog(config);

  /* show the dialog at the last position, with the last size */
  IupConfigDialogShow(config, main_dialog, "MainWindow");

  /* initialize the current file */
  new_file(main_dialog);

  /* open a file from the command line (allow file association in Windows) */
  if (argc > 1 && argv[1])
  {
    const char* filename = argv[1];
    open_file(main_dialog, filename);
  }

  IupMainLoop();

  IupClose();
  return EXIT_SUCCESS;
}
