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

#include "Scintilla.h"
#include "SciLexer.h"

#include <windows.h>

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

#include "iupwin_drv.h"
#include "iupwin_str.h"

#include "iupsci.h"


#define WM_IUPCARET WM_APP+1   /* Custom IUP message */

void iupdrvScintillaRefreshCaret(Ihandle* ih)
{
  PostMessage(ih->handle, WM_IUPCARET, 0, 0L);
}

int iupdrvScintillaGetBorder(void)
{
  int border_size = 2 * 3;
  return border_size;
}

void iupdrvScintillaOpen(void)
{
  Scintilla_RegisterClasses(IupGetGlobal("HINSTANCE"));
}

sptr_t IupScintillaSendMessage(Ihandle* ih, unsigned int iMessage, uptr_t wParam, sptr_t lParam)
{
  return SendMessage(ih->handle, iMessage, wParam, lParam);
}

static int winScintillaWmNotify(Ihandle* ih, NMHDR* msg_info, int *result)
{
  SCNotification *pMsg = (SCNotification*)msg_info;

  iupScintillaNotify(ih, pMsg);

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
      if (iupObjectCheck(ih))
        iupAttribSet(ih, "_IUPWIN_IGNORE_CHAR", "1");
      *result = 0;
      return 1;
    }
    else
    {
      if (iupObjectCheck(ih))
        iupAttribSet(ih, "_IUPWIN_IGNORE_CHAR", NULL);
    }
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
      return 0;  /* already processed at the beginning of this function */
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
      iupScintillaCallCaretCb(ih);
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

int iupdrvScintillaPrintAttrib(Ihandle* ih, const char* value)
{
  PRINTDLG pdlg;
  DOCINFO di;
  int startPos = 0, endPos = 0;
  HDC hdc;
  struct Sci_RangeToFormat frPrint;
  int margin_left, margin_top, margin_right, margin_bottom, margin_units,
      page_width, page_height;
  int p, printPage;
  double dpi;
  LONG lengthDoc, lengthDocMax, lengthPrinted;

  ZeroMemory(&pdlg, sizeof(PRINTDLG));
  pdlg.lStructSize = sizeof(PRINTDLG);
  pdlg.hwndOwner = IupGetDialog(ih)->handle;
  pdlg.hInstance = (HINSTANCE)IupGetGlobal("HINSTANCE");
  pdlg.Flags = PD_USEDEVMODECOPIES | PD_ALLPAGES | PD_RETURNDC;
  pdlg.nCopies = 1;
  pdlg.nFromPage = 1;
  pdlg.nToPage = 1;
  pdlg.nMinPage = 1;
  pdlg.nMaxPage = 0xffffU; /* We do not know how many pages in the */
                           /* document until the printer is selected and the paper size is known. */

  /* See if a range has been selected */
  IupGetIntInt(ih, "SELECTIONPOS", &startPos, &endPos);

  if (startPos == endPos)
    pdlg.Flags |= PD_NOSELECTION;
  else
    pdlg.Flags |= PD_SELECTION;

  if (!iupAttribGetBoolean(ih, "PRINTDIALOG")) 
  {
    /* Don't display dialog box, just use the default printer and options */
    pdlg.Flags |= PD_RETURNDEFAULT;
  }

  if (!PrintDlg(&pdlg)) 
  {
    if (pdlg.hDevMode)
      GlobalFree(pdlg.hDevMode);
    if (pdlg.hDevNames)
      GlobalFree(pdlg.hDevNames);
    return 0;
  }

  hdc = pdlg.hDC;

  page_width = GetDeviceCaps(hdc, HORZRES);    /* physically printable pixels */
  page_height = GetDeviceCaps(hdc, VERTRES);
  dpi = (double)GetDeviceCaps(hdc, LOGPIXELSY);

  di.cbSize = sizeof(DOCINFO);
  di.lpszDocName = iupwinStrToSystem(value);
  di.lpszOutput = 0;
  di.lpszDatatype = 0;
  di.fwType = 0;

  if (StartDoc(hdc, &di) < 0) 
  {
    DeleteDC(hdc);
    if (pdlg.hDevMode)
      GlobalFree(pdlg.hDevMode);
    if (pdlg.hDevNames)
      GlobalFree(pdlg.hDevNames);
    return 0;
  }

  lengthDoc = (LONG)IupScintillaSendMessage(ih, SCI_GETLENGTH, 0, 0);
  lengthDocMax = lengthDoc;
  lengthPrinted = 0;

  /* Requested to print selection */
  if (pdlg.Flags & PD_SELECTION) 
  {
    if (startPos > endPos) 
    {
      lengthPrinted = endPos;
      lengthDoc = startPos;
    }
    else 
    {
      lengthPrinted = startPos;
      lengthDoc = endPos;
    }

    if (lengthPrinted < 0)
      lengthPrinted = 0;
    if (lengthDoc > lengthDocMax)
      lengthDoc = lengthDocMax;
  }

  margin_units = iupSciGetPrintMarginUnits(ih);
  margin_left = iupSciGetPrintMargin(ih, "PRINTMARGINLEFT", margin_units, dpi);
  margin_top = iupSciGetPrintMargin(ih, "PRINTMARGINTOP", margin_units, dpi);
  margin_right = iupSciGetPrintMargin(ih, "PRINTMARGINRIGHT", margin_units, dpi);
  margin_bottom = iupSciGetPrintMargin(ih, "PRINTMARGINBOTTOM", margin_units, dpi);

  frPrint.hdc = hdc;
  frPrint.hdcTarget = hdc;
  frPrint.rc.left = margin_left;
  frPrint.rc.top = margin_top;
  frPrint.rc.right = page_width - margin_right;
  frPrint.rc.bottom = page_height - margin_bottom;
  frPrint.rcPage.left = 0;
  frPrint.rcPage.top = 0;
  frPrint.rcPage.right = page_width;
  frPrint.rcPage.bottom = page_height;

  /* Print each page */
  p = 1;  /* starts at 1 to match page selection */
  while (lengthPrinted < lengthDoc) 
  {
    printPage = (!(pdlg.Flags & PD_PAGENUMS) ||
                      ((p >= pdlg.nFromPage) && (p <= pdlg.nToPage)));

    if (printPage)
      StartPage(hdc);

    frPrint.chrg.cpMin = lengthPrinted;
    frPrint.chrg.cpMax = lengthDoc;
    lengthPrinted = (LONG)IupScintillaSendMessage(ih, SCI_FORMATRANGE, printPage, (sptr_t)&frPrint);
    p++;

    if (printPage)
      EndPage(hdc);

    if ((pdlg.Flags & PD_PAGENUMS) && (p > pdlg.nToPage))
      break;
  }

  IupScintillaSendMessage(ih, SCI_FORMATRANGE, FALSE, 0);

  EndDoc(hdc);
  DeleteDC(hdc);

  if (pdlg.hDevMode)
    GlobalFree(pdlg.hDevMode);
  if (pdlg.hDevNames)
    GlobalFree(pdlg.hDevNames);

  return 0;
}


/*****************************************************************************/

int idrvScintillaMap(Ihandle* ih)
{
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

  return IUP_NOERROR;
}

void iupdrvScintillaReleaseMethod(Iclass* ic)
{
  (void)ic;
  Scintilla_ReleaseResources();
}
