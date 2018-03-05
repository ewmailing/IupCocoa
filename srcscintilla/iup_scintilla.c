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

#include "iupsci.h"


/***** AUXILIARY FUNCTIONS *****/

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
  *pos = (int)IupScintillaSendMessage(ih, SCI_POSITIONFROMLINE, lin, 0);
    
  if(*pos != -1)
  {
    /* length of the line not including any end of line characters */
    int line_length = (int)IupScintillaSendMessage(ih, SCI_GETLINEENDPOSITION, lin, 0) -
                      (int)IupScintillaSendMessage(ih, SCI_POSITIONFROMLINE, lin, 0);

    if (col <= line_length)
      *pos += col;
    else
      *pos += line_length;
  }
  else
  {
    /* "lin" is greater than the lines in the document */
    lin = (int)IupScintillaSendMessage(ih, SCI_GETLINECOUNT, 0, 0);
    *pos = (int)IupScintillaSendMessage(ih, SCI_POSITIONFROMLINE, lin, 0);
  }
}

void iupScintillaConvertPosToLinCol(Ihandle* ih, int pos, int *lin, int *col)
{
  *lin = (int)IupScintillaSendMessage(ih, SCI_LINEFROMPOSITION, pos, 0);
  *col = (int)IupScintillaSendMessage(ih, SCI_GETCOLUMN, pos, 0);
}

static int iScintillaConvertXYToPos(Ihandle* ih, int x, int y)
{
  return (int)IupScintillaSendMessage(ih, SCI_POSITIONFROMPOINTCLOSE, x, y);
}


/***** AUXILIARY ATTRIBUTES *****/

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

void iupScintillaNotify(Ihandle *ih, SCNotification* pMsg)
{
  int lin = (int)IupScintillaSendMessage(ih, SCI_LINEFROMPOSITION, pMsg->position, 0);
  int col = (int)IupScintillaSendMessage(ih, SCI_GETCOLUMN, pMsg->position, 0);

  switch(pMsg->nmhdr.code)
  {
  case SCN_DWELLSTART:
  case SCN_DWELLEND:
    {
      IFniiii cb = (IFniiii)IupGetCallback(ih, "DWELL_CB");
      if (cb)
        cb(ih, pMsg->nmhdr.code == SCN_DWELLSTART ? 1 : 0, pMsg->position, pMsg->x, pMsg->y);

      break;
    }
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
        int points = (int)IupScintillaSendMessage(ih, SCI_GETZOOM, 0, 0);
        cb(ih, points);
      }

      break;
    }
  case SCN_MODIFIED:
    if (ih->data->ignore_change)
    {
      iupdrvScintillaRefreshCaret(ih);
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

        iupdrvScintillaRefreshCaret(ih);
      }

      if (pMsg->modificationType&SC_MOD_INSERTTEXT ||
          pMsg->modificationType&SC_MOD_DELETETEXT)
      {
        IFn value_cb = (IFn)IupGetCallback(ih, "VALUECHANGED_CB");
        if (value_cb)
          value_cb(ih);

        iupdrvScintillaRefreshCaret(ih);

        if (pMsg->linesAdded != 0)
        {
          IFnii cb = (IFnii)IupGetCallback(ih, "LINESCHANGED_CB");
          if (cb)
            cb(ih, lin, pMsg->linesAdded);
        }
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
  case SCN_UPDATEUI:
    {
      if (pMsg->updated & SC_UPDATE_CONTENT)
      {
        IFn cb = (IFn)IupGetCallback(ih, "UPDATECONTENT_CB");
        if (cb)
          cb(ih);
      }
      if (pMsg->updated & SC_UPDATE_SELECTION)
      {
        IFn cb = (IFn)IupGetCallback(ih, "UPDATESELECTION_CB");
        if (cb)
          cb(ih);
      }
      if (pMsg->updated & SC_UPDATE_V_SCROLL)
      {
        IFn cb = (IFn)IupGetCallback(ih, "UPDATEVSCROLL_CB");
        if (cb)
          cb(ih);
      }
      if (pMsg->updated & SC_UPDATE_H_SCROLL)
      {
        IFn cb = (IFn)IupGetCallback(ih, "UPDATEHSCROLL_CB");
        if (cb)
          cb(ih);
      }

      break;
    }
  }
}

void iupScintillaCallCaretCb(Ihandle* ih)
{
  int pos;

  IFniii cb = (IFniii)IupGetCallback(ih, "CARET_CB");
  if (!cb)
    return;

  pos = (int)IupScintillaSendMessage(ih, SCI_GETCURRENTPOS, 0, 0);

  if (pos != ih->data->last_caret_pos)
  {
    int col, lin;
    iupScintillaConvertPosToLinCol(ih, pos, &lin, &col);

    ih->data->last_caret_pos = pos;

    cb(ih, lin, col, pos);
  }
}

/*****************************************************************************/

static int iScintillaMapMethod(Ihandle* ih)
{
  if (idrvScintillaMap(ih) == IUP_ERROR)
    return IUP_ERROR;

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

  /* change default behavior */
  IupScintillaSendMessage(ih, SCI_SETPASTECONVERTENDINGS, 1, 0);
  IupScintillaSendMessage(ih, SCI_SETEOLMODE, SC_EOL_LF, 0);
  IupScintillaSendMessage(ih, SCI_SETWHITESPACESIZE, 3, 0);
  IupScintillaSendMessage(ih, SCI_SETMARGINWIDTHN, 1, 0);

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
    int border_size = iupdrvScintillaGetBorder();
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
  ic->Release = iupdrvScintillaReleaseMethod;
  ic->Create  = iScintillaCreateMethod;
  ic->Map     = iScintillaMapMethod;
  ic->UnMap = iupdrvBaseUnMapMethod;
  ic->ComputeNaturalSize = iScintillaComputeNaturalSizeMethod;
  ic->LayoutUpdate = iupdrvBaseLayoutUpdateMethod;

  /* Callbacks */
  
  iupClassRegisterCallback(ic, "DWELL_CB", "iiii");
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
  iupClassRegisterCallback(ic, "UPDATECONTENT_CB", "");
  iupClassRegisterCallback(ic, "UPDATESELECTION_CB", "");
  iupClassRegisterCallback(ic, "UPDATEVSCROLL_CB", "");
  iupClassRegisterCallback(ic, "UPDATEHSCROLL_CB", "");
  iupClassRegisterCallback(ic, "LINESCHANGED_CB", "ii");

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
  iupScintillaRegisterPrint(ic);           /* Printing */
  iupScintillaRegisterIndicators(ic);      /* Indicators */

  /* General */
  iupClassRegisterAttribute(ic, "VISIBLECOLUMNS", NULL, NULL, IUPAF_SAMEASSYSTEM, "30", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "VISIBLELINES",   NULL, NULL, IUPAF_SAMEASSYSTEM, "10", IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "BORDER", NULL, NULL, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "MULTILINE", NULL, NULL, IUPAF_SAMEASSYSTEM, "YES", IUPAF_READONLY|IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "USEPOPUP", NULL, iScintillaSetUsePopupAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_WRITEONLY|IUPAF_NO_INHERIT);

  return ic;
}

Iclass* iupScintillaDlgNewClass(void);

void IupScintillaOpen(void)
{
  if (!IupGetGlobal("_IUP_SCINTILLA_OPEN"))
  {
    iupRegisterClass(iupScintillaNewClass());
    iupRegisterClass(iupScintillaDlgNewClass());
    IupSetGlobal("_IUP_SCINTILLA_OPEN", "1");

    iupdrvScintillaOpen();
  }
}

Ihandle *IupScintilla(void)
{
  return IupCreate("scintilla");
}
