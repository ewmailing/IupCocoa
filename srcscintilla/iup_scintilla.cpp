/*
* IupScintilla component
*
* Description : A source code editing component, 
* derived from Scintilla (http://www.scintilla.org/)
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


#include <Scintilla.h>
#include <SciLexer.h>

#ifdef GTK
#include <gtk/gtk.h>
#include <ScintillaWidget.h>
#else
#include <windows.h>
#endif

#include "iup.h"
#include "iup_scintilla.h"
#include "iupcbs.h"
#include "iup_key.h"

#include "iup_class.h"
#include "iup_object.h"
#include "iup_attrib.h"
#include "iup_str.h"
#include "iup_drv.h"
#include "iup_drvfont.h"
#include "iup_register.h"
#include "iup_layout.h"
#include "iup_assert.h"

#ifdef GTK
#include "iupgtk_drv.h"
#else
#include "iupwin_drv.h"
#endif

#include "iupsci.h"

#ifndef GTK
#define WM_IUPCARET WM_APP+1   /* Custom IUP message */
#endif

sptr_t IupScintillaSendMessage(Ihandle* ih, unsigned int iMessage, uptr_t wParam, sptr_t lParam)
{
#ifdef GTK
  return scintilla_send_message(SCINTILLA(ih->handle), iMessage, wParam, lParam);
#else
  return SendMessage(ih->handle, iMessage, wParam, lParam);
#endif
}


/***** AUXILIARY ATTRIBUTES *****/

long iupScintillaEncodeColor(unsigned char r, unsigned char g, unsigned char b)
{
  return (((unsigned long)r) <<  0) |
         (((unsigned long)g) <<  8) |
         (((unsigned long)b) << 16);
}

void iupScintillaDecodeColor(long color, unsigned char *r, unsigned char *g, unsigned char *b)
{
  *r = (unsigned char)(((color) >>  0) & 0xFF);
  *g = (unsigned char)(((color) >>  8) & 0xFF);
  *b = (unsigned char)(((color) >> 16) & 0xFF);
}

void iupScintillaConvertLinColToPos(Ihandle* ih, int lin, int col, int *pos)
{
  *pos = IupScintillaSendMessage(ih, SCI_POSITIONFROMLINE, lin, 0);
    
  if(*pos != -1)
  {
    /* length of the line not including any end of line characters */
    int line_length = IupScintillaSendMessage(ih, SCI_GETLINEENDPOSITION, lin, 0) - 
                      IupScintillaSendMessage(ih, SCI_POSITIONFROMLINE, lin, 0);

    if (col <= line_length)
      *pos += col;
    else
      *pos += line_length;
  }
  else
  {
    /* "lin" is greater than the lines in the document */
    lin = IupScintillaSendMessage(ih, SCI_GETLINECOUNT, 0, 0);
    *pos = IupScintillaSendMessage(ih, SCI_POSITIONFROMLINE, lin, 0);
  }
}

void iupScintillaConvertPosToLinCol(Ihandle* ih, int pos, int *lin, int *col)
{
  *lin = IupScintillaSendMessage(ih, SCI_LINEFROMPOSITION, pos, 0);
  *col = IupScintillaSendMessage(ih, SCI_GETCOLUMN, pos, 0);
}

static int iScintillaConvertXYToPos(Ihandle* ih, int x, int y)
{
  return IupScintillaSendMessage(ih, SCI_POSITIONFROMPOINT, x, y);
}


/***** GENERAL FUNCTIONS *****/

static int iScintillaSetUsePopupAttrib(Ihandle* ih, const char* value)
{
  IupScintillaSendMessage(ih, SCI_USEPOPUP, iupStrBoolean(value), 0);
  return 1;  /* there is no get */
}


/***** NOTIFICATIONS *****/

static void iScintillaKeySetStatus(int state, char* status, int doubleclick)
{
  if (state & SCMOD_SHIFT)
    iupKEY_SETSHIFT(status);

  if (state & SCMOD_CTRL)
    iupKEY_SETCONTROL(status); 

  iupKEY_SETBUTTON1(status);

  if (state & SCMOD_ALT)
    iupKEY_SETALT(status);

  if (state & SCMOD_META) /* Apple/Win */
    iupKEY_SETSYS(status);

  if (doubleclick)
    iupKEY_SETDOUBLE(status);
}

static void iScintillaNotify(Ihandle *ih, struct Scintilla::SCNotification* pMsg)
{
  int lin = IupScintillaSendMessage(ih, SCI_LINEFROMPOSITION, pMsg->position, 0);
  int col = IupScintillaSendMessage(ih, SCI_GETCOLUMN, pMsg->position, 0);

  switch(pMsg->nmhdr.code)
  {
  case SCN_SAVEPOINTREACHED:
  case SCN_SAVEPOINTLEFT:
    {
      IFni cb = (IFni)IupGetCallback(ih, "SAVEPOINT_CB");
      if (cb)
        cb(ih, pMsg->nmhdr.code==SCN_SAVEPOINTREACHED? 1: 0);

      break;
    }
  case SCN_MARGINCLICK:
    {
      IFniis cb = (IFniis)IupGetCallback(ih, "MARGINCLICK_CB");
      if (cb)
      {
        char status[IUPKEY_STATUS_SIZE] = IUPKEY_STATUS_INIT;
        iScintillaKeySetStatus(pMsg->modifiers, status, 0);
        cb(ih, pMsg->margin, lin, status);
      }

      break;
    }
  case SCN_HOTSPOTDOUBLECLICK:
  case SCN_HOTSPOTCLICK:
    {
      IFniiis cb = (IFniiis)IupGetCallback(ih, "HOTSPOTCLICK_CB");
      if (cb)
      {
        char status[IUPKEY_STATUS_SIZE] = IUPKEY_STATUS_INIT;
        iScintillaKeySetStatus(pMsg->modifiers, status, pMsg->nmhdr.code==SCN_HOTSPOTDOUBLECLICK? 1: 0);
        cb(ih, pMsg->position, lin, col, status);
      }

      break;
    }
  case SCN_ZOOM:
    {
      IFni cb = (IFni)IupGetCallback(ih, "ZOOM_CB");
      if (cb)
      {
        int points = IupScintillaSendMessage(ih, SCI_GETZOOM, 0, 0);
        cb(ih, points);
      }

      break;
    }
  case SCN_MODIFIED:
    if (ih->data->ignore_change)
    {
#ifndef GTK
      PostMessage(ih->handle, WM_IUPCARET, 0, 0L);
#endif
      break;
    }

    if (pMsg->modificationType&SC_PERFORMED_USER ||
        pMsg->modificationType&SC_PERFORMED_UNDO || 
        pMsg->modificationType&SC_PERFORMED_REDO)
    {
      if (pMsg->modificationType&SC_MOD_BEFOREINSERT ||
          pMsg->modificationType&SC_MOD_BEFOREDELETE)
      {
        IFniiis cb = (IFniiis)IupGetCallback(ih, "ACTION");
        if (cb)
        {
          int insert = 1;
          if (pMsg->modificationType&SC_MOD_BEFOREDELETE)
            insert = 0;

          cb(ih, insert, pMsg->position, pMsg->length, (char*)pMsg->text);
        }
#ifndef GTK
        PostMessage(ih->handle, WM_IUPCARET, 0, 0L);
#endif
      }

      if (pMsg->modificationType&SC_MOD_INSERTTEXT ||
          pMsg->modificationType&SC_MOD_DELETETEXT)
      {
        IFn value_cb = (IFn)IupGetCallback(ih, "VALUECHANGED_CB");
        if (value_cb)
          value_cb(ih);
#ifndef GTK
        PostMessage(ih->handle, WM_IUPCARET, 0, 0L);
#endif
      }
    }

    break;
  case SCN_AUTOCSELECTION:
    {
      IFnis cb = (IFnis)IupGetCallback(ih, "AUTOCSELECTION_CB");
      if (cb)
        cb(ih, pMsg->position, (char*)pMsg->text);

      break;
    }
  case SCN_AUTOCCANCELLED:
    {
      IFn cb = (IFn)IupGetCallback(ih, "AUTOCCANCELLED_CB");
      if (cb)
        cb(ih);

      break;
    }
  case SCN_AUTOCCHARDELETED:
    {
      IFn cb = (IFn)IupGetCallback(ih, "AUTOCCHARDELETED_CB");
      if (cb)
        cb(ih);

      break;
    }
  }
}

static void iScintillaCallCaretCb(Ihandle* ih)
{
  int pos;

  IFniii cb = (IFniii)IupGetCallback(ih, "CARET_CB");
  if (!cb)
    return;

  pos = IupScintillaSendMessage(ih, SCI_GETCURRENTPOS, 0, 0);

  if (pos != ih->data->last_caret_pos)
  {
    int col, lin;
    iupScintillaConvertPosToLinCol(ih, pos, &lin, &col);

    ih->data->last_caret_pos = pos;

    cb(ih, lin, col, pos);
  }
}

#ifdef GTK
static void gtkScintillaNotify(GtkWidget *w, gint wp, gpointer lp, Ihandle *ih)
{
  struct Scintilla::SCNotification *pMsg =(struct Scintilla::SCNotification *)lp;

  iScintillaNotify(ih, pMsg);

  (void)w;
  (void)wp;
}

static gboolean gtkScintillaKeyReleaseEvent(GtkWidget *widget, GdkEventKey *evt, Ihandle *ih)
{
  iScintillaCallCaretCb(ih);
  (void)widget;
  (void)evt;
  return FALSE;
}

static gboolean gtkScintillaButtonEvent(GtkWidget *widget, GdkEventButton *evt, Ihandle *ih)
{
  iScintillaCallCaretCb(ih);
  return iupgtkButtonEvent(widget, evt, ih);
}

#else

static int winScintillaWmNotify(Ihandle* ih, NMHDR* msg_info, int *result)
{
  struct Scintilla::SCNotification *pMsg = (struct Scintilla::SCNotification*)msg_info;

  iScintillaNotify(ih, pMsg);

  (void)result;
  return 0; /* result not used */
}

static int winScintillaMsgProc(Ihandle* ih, UINT msg, WPARAM wp, LPARAM lp, LRESULT *result)
{
  if (msg==WM_KEYDOWN) /* process K_ANY before text callbacks */
  {
    int ret = iupwinBaseMsgProc(ih, msg, wp, lp, result);
    if (ret) 
    {
      iupAttribSet(ih, "_IUPWIN_IGNORE_CHAR", "1");
      *result = 0;
      return 1;
    }
    else
      iupAttribSet(ih, "_IUPWIN_IGNORE_CHAR", NULL);
  }

  switch (msg)
  {
  case WM_CHAR:
    {
      /* even aborting WM_KEYDOWN, a WM_CHAR will be sent, so ignore it also */
      /* if a dialog was shown, the loop will be processed, so ignore out of focus WM_CHAR messages */
      if (GetFocus() != ih->handle || iupAttribGet(ih, "_IUPWIN_IGNORE_CHAR"))
      {
        iupAttribSet(ih, "_IUPWIN_IGNORE_CHAR", NULL);
        *result = 0;
        return 1;
      }

      break;
    }
  case WM_KEYDOWN:
    {
      PostMessage(ih->handle, WM_IUPCARET, 0, 0L);
      return 0;  /* already processed at the begining of this function */
    }
  case WM_KEYUP:
    {
      PostMessage(ih->handle, WM_IUPCARET, 0, 0L);
      break;
    }
  case WM_LBUTTONDBLCLK:
  case WM_MBUTTONDBLCLK:
  case WM_RBUTTONDBLCLK:
  case WM_LBUTTONDOWN:
  case WM_MBUTTONDOWN:
  case WM_RBUTTONDOWN:
    {
      if (iupwinButtonDown(ih, msg, wp, lp)==-1)
      {
        *result = 0;
        return 1;
      }
      PostMessage(ih->handle, WM_IUPCARET, 0, 0L);
      break;
    }
  case WM_MBUTTONUP:
  case WM_RBUTTONUP:
  case WM_LBUTTONUP:
    {
      if (iupwinButtonUp(ih, msg, wp, lp)==-1)
      {
        *result = 0;
        return 1;
      }
      PostMessage(ih->handle, WM_IUPCARET, 0, 0L);
      break;
    }
  case WM_IUPCARET:
    {
      iScintillaCallCaretCb(ih);
      break;
    }
  case WM_MOUSEMOVE:
    {
      iupwinMouseMove(ih, msg, wp, lp);
      break;
    }
  }

  return iupwinBaseMsgProc(ih, msg, wp, lp, result);
}
#endif

/*****************************************************************************/

static int iScintillaMapMethod(Ihandle* ih)
{
#ifdef GTK
  ih->handle = scintilla_new();
  if (!ih->handle)
    return IUP_ERROR;

  gtk_widget_show(ih->handle);

  /* add to the parent, all GTK controls must call this. */
  iupgtkAddToParent(ih);

  if (!iupAttribGetBoolean(ih, "CANFOCUS"))
    iupgtkSetCanFocus(ih->handle, 0);

  g_signal_connect(G_OBJECT(ih->handle), "enter-notify-event", G_CALLBACK(iupgtkEnterLeaveEvent), ih);
  g_signal_connect(G_OBJECT(ih->handle), "leave-notify-event", G_CALLBACK(iupgtkEnterLeaveEvent), ih);
  g_signal_connect(G_OBJECT(ih->handle), "focus-in-event",     G_CALLBACK(iupgtkFocusInOutEvent), ih);
  g_signal_connect(G_OBJECT(ih->handle), "focus-out-event",    G_CALLBACK(iupgtkFocusInOutEvent), ih);
  g_signal_connect(G_OBJECT(ih->handle), "key-press-event",    G_CALLBACK(iupgtkKeyPressEvent), ih);
  g_signal_connect(G_OBJECT(ih->handle), "show-help",          G_CALLBACK(iupgtkShowHelp), ih);

  g_signal_connect_after(G_OBJECT(ih->handle), "key-release-event", G_CALLBACK(gtkScintillaKeyReleaseEvent), ih);
  g_signal_connect(G_OBJECT(ih->handle), "button-press-event", G_CALLBACK(gtkScintillaButtonEvent), ih);  /* if connected "after" then it is ignored */
  g_signal_connect(G_OBJECT(ih->handle), "button-release-event", G_CALLBACK(gtkScintillaButtonEvent), ih);
  g_signal_connect(G_OBJECT(ih->handle), "motion-notify-event", G_CALLBACK(iupgtkMotionNotifyEvent), ih);

  g_signal_connect(G_OBJECT(ih->handle), "sci-notify", G_CALLBACK(gtkScintillaNotify), ih);

  gtk_widget_realize(ih->handle);
#else
  DWORD dwStyle = WS_CHILD | WS_CLIPSIBLINGS;
  DWORD dwExStyle = 0;

  if (!ih->parent)
    return IUP_ERROR;

  if (iupAttribGetBoolean(ih, "CANFOCUS"))
    dwStyle |= WS_TABSTOP;

  if (iupAttribGetBoolean(ih, "BORDER"))
    dwExStyle |= WS_EX_CLIENTEDGE;
  
  if (!iupwinCreateWindow(ih, TEXT("Scintilla"), dwExStyle, dwStyle, NULL))
    return IUP_ERROR;

  /* Process Scintilla Notifications */
  IupSetCallback(ih, "_IUPWIN_NOTIFY_CB", (Icallback)winScintillaWmNotify);

  /* Process BUTTON_CB, MOTION_CB and CARET_CB */
  IupSetCallback(ih, "_IUPWIN_CTRLMSGPROC_CB", (Icallback)winScintillaMsgProc);
#endif

  /* configure for DROP of files */
  if (IupGetCallback(ih, "DROPFILES_CB"))
    iupAttribSet(ih, "DROPFILESTARGET", "YES");

  /* add scrollbar */
  if (ih->data->sb & IUP_SB_HORIZ)
    IupScintillaSendMessage(ih, SCI_SETHSCROLLBAR, 1, 0);
  else
    IupScintillaSendMessage(ih, SCI_SETHSCROLLBAR, 0, 0);

  if (ih->data->sb & IUP_SB_VERT)
    IupScintillaSendMessage(ih, SCI_SETVSCROLLBAR, 1, 0);
  else
    IupScintillaSendMessage(ih, SCI_SETVSCROLLBAR, 0, 0);

  IupSetCallback(ih, "_IUP_XY2POS_CB", (Icallback)iScintillaConvertXYToPos);
  IupSetCallback(ih, "_IUP_POS2LINCOL_CB", (Icallback)iupScintillaConvertPosToLinCol);
  IupSetCallback(ih, "_IUP_LINCOL2POS_CB", (Icallback)iupScintillaConvertLinColToPos);

  IupScintillaSendMessage(ih, SCI_SETPASTECONVERTENDINGS, 1, 0);
  IupScintillaSendMessage(ih, SCI_SETEOLMODE, SC_EOL_LF, 0);

  if(IupGetInt(NULL, "UTF8MODE"))
    IupScintillaSendMessage(ih, SCI_SETCODEPAGE, SC_CP_UTF8, 0);
  else
    IupScintillaSendMessage(ih, SCI_SETCODEPAGE, 0, 0);

  return IUP_NOERROR;
}

static void iScintillaComputeNaturalSizeMethod(Ihandle* ih, int *w, int *h, int *children_expand)
{
  int natural_w = 0, 
      natural_h = 0,
      visiblecolumns = iupAttribGetInt(ih, "VISIBLECOLUMNS"),
      visiblelines = iupAttribGetInt(ih, "VISIBLELINES");
  (void)children_expand; /* unset if not a container */

  iupdrvFontGetCharSize(ih, NULL, &natural_h);  /* one line height */
  natural_w = iupdrvFontGetStringWidth(ih, "WWWWWWWWWW");
  natural_w = (visiblecolumns*natural_w)/10;
  natural_h = visiblelines*natural_h;

  /* compute the borders space */
  if (iupAttribGetBoolean(ih, "BORDER"))
  {
#ifdef GTK
    int border_size = 2*5;
#else
    int border_size = 2*3;
#endif
    natural_w += border_size;
    natural_h += border_size;
  }

  /* compute scrollbar */
  if (ih->data->sb != IUP_SB_NONE)
  {
    int sb_size = iupdrvGetScrollbarSize();
    if (ih->data->sb & IUP_SB_HORIZ)
      natural_h += sb_size;  /* sb horizontal affects vertical size */
    if (ih->data->sb & IUP_SB_VERT)
      natural_w += sb_size;  /* sb vertical affects horizontal size */
  }
  
  *w = natural_w;
  *h = natural_h;
}

static int iScintillaCreateMethod(Ihandle* ih, void **params)
{
  (void)params;
  ih->data = iupALLOCCTRLDATA();
  ih->data->sb = IUP_SB_HORIZ | IUP_SB_VERT;
  ih->data->append_newline = 1;
  iupAttribSet(ih, "_IUP_MULTILINE_TEXT", "1");

  /* unused for now */
  ih->data->useBraceHLIndicator = 1;
  ih->data->useBraceBLIndicator = 1;
  return IUP_NOERROR;
}

static void iScintillaReleaseMethod(Iclass* ic)
{
  (void)ic;
#ifndef GTK
  Scintilla_ReleaseResources();
#endif
}

static Iclass* iupScintillaNewClass(void)
{
  Iclass* ic = iupClassNew(NULL);

  ic->name = (char*)"scintilla";
  ic->format = NULL; /* no parameters */
  ic->nativetype = IUP_TYPECONTROL;
  ic->childtype  = IUP_CHILDNONE;
  ic->is_interactive = 1;
  ic->has_attrib_id  = 1;   /* has attributes with IDs that must be parsed */

  /* Class functions */
  ic->New     = iupScintillaNewClass;
  ic->Release = iScintillaReleaseMethod;
  ic->Create  = iScintillaCreateMethod;
  ic->Map     = iScintillaMapMethod;
  ic->UnMap = iupdrvBaseUnMapMethod;
  ic->ComputeNaturalSize = iScintillaComputeNaturalSizeMethod;
  ic->LayoutUpdate = iupdrvBaseLayoutUpdateMethod;

  /* Callbacks */
  iupClassRegisterCallback(ic, "SAVEPOINT_CB", "i");
  iupClassRegisterCallback(ic, "MARGINCLICK_CB", "iis");
  iupClassRegisterCallback(ic, "HOTSPOTCLICK_CB", "iiis");
  iupClassRegisterCallback(ic, "BUTTON_CB", "iiiis");
  iupClassRegisterCallback(ic, "MOTION_CB", "iis");
  iupClassRegisterCallback(ic, "CARET_CB", "iii");
  iupClassRegisterCallback(ic, "VALUECHANGED_CB", "");
  iupClassRegisterCallback(ic, "ACTION", "iiis");
  iupClassRegisterCallback(ic, "ZOOM_CB", "i");
  iupClassRegisterCallback(ic, "AUTOCSELECTION_CB", "is");
  iupClassRegisterCallback(ic, "AUTOCCANCELLED_CB", "");
  iupClassRegisterCallback(ic, "AUTOCCHARDELETED_CB", "");

  /* Common Callbacks */
  iupBaseRegisterCommonCallbacks(ic);

  /* Common */
  iupBaseRegisterCommonAttrib(ic);

  /* Visual */
  iupBaseRegisterVisualAttrib(ic);

  /* Drag&Drop */
  iupdrvRegisterDragDropAttrib(ic);
  
  iupScintillaRegisterText(ic);            /* Text retrieval and modification */
  iupScintillaRegisterSelection(ic);       /* Selection and information */
  iupScintillaRegisterClipboard(ic);       /* Clipboard: Cut, Copy, Paste, Undo and Redo */
  iupScintillaRegisterOvertype(ic);        /* Overtype */
  iupScintillaRegisterTab(ic);             /* Tabs and Indentation Guides */
  iupScintillaRegisterWordWrap(ic);        /* Line wrapping */
  iupScintillaRegisterStyle(ic);           /* Style Definition Attributes */
  iupScintillaRegisterLexer(ic);           /* Lexer Attributes */
  iupScintillaRegisterFolding(ic);         /* Folding Attributes */
  iupScintillaRegisterMargin(ic);          /* Margin Attributes */
  iupScintillaRegisterMarker(ic);          /* Marker Attributes */
  iupScintillaRegisterWhiteSpace(ic);      /* White space Attributes */
  iupScintillaRegisterBraceLight(ic);      /* Brace highlighting Attributes */
  iupScintillaRegisterCursor(ic);          /* Cursor and Zooming Attributes */
  iupScintillaRegisterAnnotation(ic);      /* Annotation Attributes */
  iupScintillaRegisterScrolling(ic);       /* Scrolling and automatic scrolling */
  iupScintillaRegisterAutocompletion(ic);  /* Autocompletion */
  iupScintillaRegisterSearching(ic);       /* Search & Replace */

  /* General */
  iupClassRegisterAttribute(ic, "VISIBLECOLUMNS", NULL, NULL, IUPAF_SAMEASSYSTEM, "30", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "VISIBLELINES",   NULL, NULL, IUPAF_SAMEASSYSTEM, "10", IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "BORDER", NULL, NULL, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "MULTILINE", NULL, NULL, IUPAF_SAMEASSYSTEM, "YES", IUPAF_READONLY|IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "USEPOPUP", NULL, iScintillaSetUsePopupAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_WRITEONLY|IUPAF_NO_INHERIT);

  return ic;
}

void IupScintillaOpen(void)
{
  if (!IupGetGlobal("_IUP_SCINTILLA_OPEN"))
  {
    iupRegisterClass(iupScintillaNewClass());
    IupSetGlobal("_IUP_SCINTILLA_OPEN", "1");

#ifndef GTK
    Scintilla_RegisterClasses(IupGetGlobal("HINSTANCE"));
#endif
  }
}

Ihandle *IupScintilla(void)
{
  return IupCreate("scintilla");
}


/*****  TODO  (by-demand)
- Multiple Selection and Virtual Space
- Macro recording
- Printing
- Long lines
- Call tips
- Caret, selection, and hotspot styles
- Indicators
- Brace Highlighting with Indicators
  BRACEHLINDICATOR (non inheritable, write only): defines a specified indicator to highlight matching braces instead of changing their style (See Indicator Styles).
  BRACEBLINDICATOR (non inheritable, write only): defines a specified indicator to highlight non matching brace instead of changing its style (See Indicator Styles).
  USEBRACEHLINDICATOR (non inheritable): enable or disable the indicator to highlight matching braces. Can be YES or NO. Default: YES.
  USEBRACEBLINDICATOR (non inheritable): enable or disable the indicator to highlight non matching brace. Can be YES or NO. Default: YES.
*/
