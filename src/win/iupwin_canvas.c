/** \file
 * \brief Canvas Control
 *
 * See Copyright Notice in "iup.h"
 */

#include <windows.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <limits.h>

#include "iup.h"
#include "iupcbs.h"

#include "iup_object.h"
#include "iup_childtree.h"
#include "iup_attrib.h"
#include "iup_dialog.h"
#include "iup_str.h"
#include "iup_drv.h"
#include "iup_drvfont.h"
#include "iup_canvas.h"
#include "iup_key.h"

#include "iupwin_drv.h"
#include "iupwin_handle.h"
#include "iupwin_brush.h"
#include "iupwin_info.h"
#include "iupwin_str.h"


/*  The system hides and disables a standard scroll bar 
    when equal minimum and maximum values are specified. 
    The system also hides and disables a standard scroll bar 
    if you specify a page size that includes the entire scroll range of the scroll bar. 
    THIS IS NOT WORKING: we have to call ShowScrollBar and SetScrollPos manually. */
static void winCanvasSetScrollInfo(HWND hWnd, int imin, int imax, int ipage, int flag, int autohide)
{
  SCROLLINFO scrollinfo;
  scrollinfo.cbSize = sizeof(SCROLLINFO);
  scrollinfo.fMask = SIF_RANGE | SIF_PAGE;
  if (!autohide)
    scrollinfo.fMask |= SIF_DISABLENOSCROLL;
  scrollinfo.nPage = ipage;
  scrollinfo.nMax = imax;
  scrollinfo.nMin = imin;
  SetScrollInfo(hWnd, flag, &scrollinfo, TRUE);
}

static int winCanvasSetBgColorAttrib(Ihandle *ih, const char *value)
{
  (void)value;
  iupdrvPostRedraw(ih);
  return 1;
}
              
static int winCanvasSetDXAttrib(Ihandle* ih, const char *value)
{
  if (ih->data->sb & IUP_SB_HORIZ)
  {
    double posx, xmin, xmax;
    float dx;
    int iposx, ipagex;

    if (!iupStrToFloatDef(value, &dx, 0.1f))
      return 1;

    xmin = iupAttribGetFloat(ih, "XMIN");
    xmax = iupAttribGetFloat(ih, "XMAX");
    posx = ih->data->posx;

    iupCanvasCalcScrollIntPos(xmin, xmax, dx, posx, 
                              IUP_SB_MIN, IUP_SB_MAX, &ipagex, &iposx);

    winCanvasSetScrollInfo(ih->handle, IUP_SB_MIN, IUP_SB_MAX, ipagex, SB_HORZ, iupAttribGetBoolean(ih, "XAUTOHIDE"));

    if (dx >= (xmax-xmin))
    {
      if (iupAttribGetBoolean(ih, "XAUTOHIDE"))
      {
        iupAttribSet(ih, "XHIDDEN", "YES");
        ShowScrollBar(ih->handle, SB_HORZ, FALSE);
        SetScrollPos(ih->handle, SB_HORZ, IUP_SB_MIN, FALSE);
      }
      else
      {
        EnableScrollBar(ih->handle, SB_HORZ, ESB_DISABLE_BOTH);
        SetScrollPos(ih->handle, SB_HORZ, IUP_SB_MIN, TRUE);
      }

      ih->data->posx = (float)xmin;
    }
    else
    {
      if (iupAttribGetBoolean(ih, "XAUTOHIDE"))
      {
        iupAttribSet(ih, "XHIDDEN", "NO");
        ShowScrollBar(ih->handle, SB_HORZ, TRUE);
      }
      else
        EnableScrollBar(ih->handle, SB_HORZ, ESB_ENABLE_BOTH);

      /* also update position because it could have being changed */
      iupCanvasCalcScrollRealPos(xmin, xmax, &posx, 
                                 IUP_SB_MIN, IUP_SB_MAX, ipagex, &iposx);
      SetScrollPos(ih->handle, SB_HORZ, iposx, TRUE);
      ih->data->posx = (float)posx;
    }
  }
  return 1;
}

static int winCanvasSetDYAttrib(Ihandle* ih, const char *value)
{
  if (ih->data->sb & IUP_SB_VERT)
  {
    double posy, ymin, ymax;
    float dy;
    int iposy, ipagey;

    if (!iupStrToFloatDef(value, &dy, 0.1f))
      return 1;

    ymin = iupAttribGetFloat(ih, "YMIN");
    ymax = iupAttribGetFloat(ih, "YMAX");
    posy = ih->data->posy;

    iupCanvasCalcScrollIntPos(ymin, ymax, dy, posy, 
                              IUP_SB_MIN, IUP_SB_MAX, &ipagey, &iposy);

    winCanvasSetScrollInfo(ih->handle, IUP_SB_MIN, IUP_SB_MAX, ipagey, SB_VERT, iupAttribGetBoolean(ih, "YAUTOHIDE"));

    if (dy >= (ymax-ymin))
    {
      if (iupAttribGetBoolean(ih, "YAUTOHIDE"))
      {
        iupAttribSet(ih, "YHIDDEN", "YES");
        ShowScrollBar(ih->handle, SB_VERT, FALSE);
        SetScrollPos(ih->handle, SB_VERT, IUP_SB_MIN, FALSE);
      }
      else
      {
        EnableScrollBar(ih->handle, SB_VERT, ESB_DISABLE_BOTH);
        SetScrollPos(ih->handle, SB_VERT, IUP_SB_MIN, TRUE);
      }

      ih->data->posy = (float)ymin;
      return 1;
    }
    else
    {
      if (iupAttribGetBoolean(ih, "YAUTOHIDE"))
      {
        iupAttribSet(ih, "YHIDDEN", "NO");
        ShowScrollBar(ih->handle, SB_VERT, TRUE);
      }
      else
        EnableScrollBar(ih->handle, SB_VERT, ESB_ENABLE_BOTH);

      /* also update position because it could have being changed */
      iupCanvasCalcScrollRealPos(ymin, ymax, &posy,
                                 IUP_SB_MIN, IUP_SB_MAX, ipagey, &iposy);
      SetScrollPos(ih->handle, SB_VERT, iposy, TRUE);
      ih->data->posy = (float)posy;
    }
  }
  return 1;
}

static int winCanvasSetPosXAttrib(Ihandle *ih, const char *value)
{
  if (ih->data->sb & IUP_SB_HORIZ)
  {
    double xmin, xmax, dx;
    float posx;
    int iposx, ipagex;

    if (!iupStrToFloat(value, &posx))
      return 1;

    xmin = iupAttribGetFloat(ih, "XMIN");
    xmax = iupAttribGetFloat(ih, "XMAX");
    dx = iupAttribGetFloat(ih, "DX");

    if (posx < xmin) posx = (float)xmin;
    if (posx >(xmax - dx)) posx = (float)(xmax - dx);
    ih->data->posx = posx;

    iupCanvasCalcScrollIntPos(xmin, xmax, dx, posx,
                              IUP_SB_MIN, IUP_SB_MAX, &ipagex, &iposx);

    SetScrollPos(ih->handle, SB_HORZ, iposx, TRUE);
  }
  return 1;
}

static int winCanvasSetPosYAttrib(Ihandle *ih, const char *value)
{
  if (ih->data->sb & IUP_SB_VERT)
  {
    double ymin, ymax, dy;
    float posy;
    int iposy, ipagey;

    if (!iupStrToFloat(value, &posy))
      return 1;

    ymin = iupAttribGetFloat(ih, "YMIN");
    ymax = iupAttribGetFloat(ih, "YMAX");
    dy = iupAttribGetFloat(ih, "DY");

    if (posy < ymin) posy = (float)ymin;
    if (posy > (ymax - dy)) posy = (float)(ymax - dy);
    ih->data->posy = posy;

    iupCanvasCalcScrollIntPos(ymin, ymax, dy, posy, 
                              IUP_SB_MIN, IUP_SB_MAX, &ipagey, &iposy);

    SetScrollPos(ih->handle, SB_VERT, iposy, TRUE);
  }
  return 1;
}

static void winCanvasGetScrollInfo(HWND hWnd, int *ipos, int *ipage, int flag, int track)
{
  SCROLLINFO scrollinfo;
  scrollinfo.cbSize = sizeof(SCROLLINFO);
  if (track)
    scrollinfo.fMask = SIF_PAGE | SIF_TRACKPOS;
  else
    scrollinfo.fMask = SIF_PAGE | SIF_POS;
  GetScrollInfo(hWnd, flag, &scrollinfo);
  *ipage = scrollinfo.nPage;
  if (track)
    *ipos = scrollinfo.nTrackPos;
  else
    *ipos = scrollinfo.nPos;
}

static void winCanvasProcessHorScroll(Ihandle* ih, WORD winop)
{
  IFniff cb;
  double xmin, xmax, posx, linex;
  int ipagex, iposx, ilinex, op;

  /* unused */
  if (winop == SB_TOP ||
      winop == SB_BOTTOM ||
      winop == SB_ENDSCROLL)
    return;

  xmax = iupAttribGetFloat(ih,"XMAX");
  xmin = iupAttribGetFloat(ih,"XMIN");

  winCanvasGetScrollInfo(ih->handle, &iposx, &ipagex, SB_HORZ, winop==SB_THUMBTRACK||winop==SB_THUMBPOSITION? 1: 0);

  if (!iupAttribGet(ih,"LINEX"))
  {
    ilinex = ipagex/10;
    if (!ilinex)
      ilinex = 1;
  }
  else
  {
    /* line and page conversions are the same */
    linex = iupAttribGetFloat(ih,"LINEX");
    iupCanvasCalcScrollIntPos(xmin, xmax, linex, 0, 
                              IUP_SB_MIN, IUP_SB_MAX, &ilinex,  NULL);
  }

  switch (winop)
  {
  case SB_LINEDOWN:
    iposx = iposx + ilinex;
    op = IUP_SBRIGHT;
    break;
  case SB_LINEUP:
    iposx = iposx - ilinex;
    op = IUP_SBLEFT;
    break;
  case SB_PAGEDOWN:
    iposx = iposx + ipagex;
    op = IUP_SBPGRIGHT;
    break;
  case SB_PAGEUP:
    iposx = iposx - ipagex;
    op = IUP_SBPGLEFT;
    break;
  case SB_THUMBTRACK:
    op = IUP_SBDRAGH;
    break;
  case SB_THUMBPOSITION:
    op = IUP_SBPOSH;
    break;
  default:
    return;
  }

  iupCanvasCalcScrollRealPos(xmin, xmax, &posx, 
                             IUP_SB_MIN, IUP_SB_MAX, ipagex, &iposx);
  SetScrollPos(ih->handle, SB_HORZ, iposx, TRUE);
  ih->data->posx = (float)posx;

  cb = (IFniff)IupGetCallback(ih,"SCROLL_CB");
  if (cb)
    cb(ih,op,(float)posx,ih->data->posy);
  else
  {
    IFnff cb = (IFnff)IupGetCallback(ih,"ACTION");
    if (cb)
      cb (ih, (float)posx, ih->data->posy);
  }
}

static void winCanvasProcessVerScroll(Ihandle* ih, WORD winop)
{
  IFniff cb;
  double ymin, ymax, posy, liney;
  int ipagey, iposy, iliney, op;

  /* unused */
  if (winop == SB_TOP ||
      winop == SB_BOTTOM ||
      winop == SB_ENDSCROLL)
    return;

  ymax = iupAttribGetFloat(ih,"YMAX");
  ymin = iupAttribGetFloat(ih,"YMIN");

  winCanvasGetScrollInfo(ih->handle, &iposy, &ipagey, SB_VERT, winop==SB_THUMBTRACK||winop==SB_THUMBPOSITION? 1: 0);

  if (!iupAttribGet(ih, "LINEY"))
  {
    iliney = ipagey/10;
    if (!iliney)
      iliney = 1;
  }
  else
  {
    /* line and page conversions are the same */
    liney = iupAttribGetFloat(ih,"LINEY");
    iupCanvasCalcScrollIntPos(ymin, ymax, liney, 0, 
                              IUP_SB_MIN, IUP_SB_MAX, &iliney,  NULL);
  }

  switch (winop)
  {
  case SB_LINEDOWN:
    iposy = iposy + iliney;
    op = IUP_SBDN;
    break;
  case SB_LINEUP:
    iposy = iposy - iliney;
    op = IUP_SBUP;
    break;
  case SB_PAGEDOWN:
    iposy = iposy + ipagey;
    op = IUP_SBPGDN;
    break;
  case SB_PAGEUP:
    iposy = iposy - ipagey;
    op = IUP_SBPGUP;
    break;
  case SB_THUMBTRACK:
    op = IUP_SBDRAGV;
    break;
  case SB_THUMBPOSITION:
    op = IUP_SBPOSV;
    break;
  default:
    return;
  }

  iupCanvasCalcScrollRealPos(ymin, ymax, &posy, 
                             IUP_SB_MIN, IUP_SB_MAX, ipagey, &iposy);
  SetScrollPos(ih->handle, SB_VERT, iposy, TRUE);
  ih->data->posy = (float)posy;

  cb = (IFniff)IupGetCallback(ih,"SCROLL_CB");
  if (cb)
    cb(ih, op, ih->data->posx,(float)posy);
  else
  {
    IFnff cb = (IFnff)IupGetCallback(ih,"ACTION");
    if (cb)
      cb (ih, ih->data->posx, (float)posy);
  }
}

static char* winCanvasGetDrawSizeAttrib(Ihandle* ih)
{
  RECT rect;
  GetClientRect(ih->handle, &rect);
  return iupStrReturnIntInt((int)(rect.right-rect.left), (int)(rect.bottom-rect.top), 'x');
}

static int winCanvasMsgProc(Ihandle* ih, UINT msg, WPARAM wp, LPARAM lp, LRESULT *result)
{
  switch (msg)
  {
  case WM_ERASEBKGND:
    /* only paint background if ACTION is not defined */
    if (!IupGetCallback(ih, "ACTION")) 
    {
      RECT rect;
      HDC hdc = (HDC)wp;
      COLORREF color;
      iupwinGetColorRef(ih, "BGCOLOR", &color);
      GetClientRect(ih->handle, &rect); 
      FillRect(hdc, &rect, iupwinBrushGet(color)); 
    }
    else
      InvalidateRect(ih->handle,NULL,FALSE);  /* This will invalidate all area. 
                                                 Necessary in XP, or overlapping windows will have the effect of partial redrawing. */

    /* always return non zero value */
    *result = 1;
    return 1; 
  case WM_PAINT:
    {
      IFnff cb = (IFnff)IupGetCallback(ih, "ACTION");
      if (cb && !(ih->data->inside_resize))
      {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(ih->handle, &ps);
        iupAttribSet(ih, "HDC_WMPAINT", (char*)hdc);
        iupAttribSetStrf(ih, "CLIPRECT", "%d %d %d %d", ps.rcPaint.left, ps.rcPaint.top, ps.rcPaint.right-ps.rcPaint.left, ps.rcPaint.bottom-ps.rcPaint.top);

        cb(ih, ih->data->posx, ih->data->posy);

        iupAttribSet(ih, "CLIPRECT", NULL);
        iupAttribSet(ih, "HDC_WMPAINT", NULL);
        EndPaint(ih->handle, &ps);
      }
      break;
    }
  case WM_SIZE:
    {
      IFnii cb = (IFnii)IupGetCallback(ih, "RESIZE_CB");
      if (cb && !(ih->data->inside_resize))
      {
        /* w=LOWORD (lp), h=HIWORD(lp) can not be used because an invalid size 
           at the first time of WM_SIZE with scrollbars. */
        int w, h;
        RECT rect;
        GetClientRect(ih->handle, &rect);
        w = rect.right-rect.left;
        h = rect.bottom-rect.top;

        ih->data->inside_resize = 1;  /* avoid recursion */
        cb(ih, w, h);
        ih->data->inside_resize = 0;
      }

      if (!iupAttribGetBoolean(ih, "MDICLIENT"))
      {
        /* If a MDI client, let the DefMDIChildProc do its work. */
        *result = 0;
        return 1;
      }
      break;
    }
  case WM_GETDLGCODE:
    /* avoid beeps when keys are pressed */
    *result = DLGC_WANTCHARS|DLGC_WANTARROWS;
    return 1;
  case WM_MOUSEWHEEL:
    {
      IFnfiis cb = (IFnfiis)IupGetCallback(ih, "WHEEL_CB");
      short delta = (short)HIWORD(wp);
      if (cb)
      {
        char status[IUPKEY_STATUS_SIZE] = IUPKEY_STATUS_INIT;
        POINT p; 
        p.x = GET_X_LPARAM(lp);   
        p.y = GET_Y_LPARAM(lp);   

        ScreenToClient(ih->handle, &p);

        iupwinButtonKeySetStatus(LOWORD(wp), status, 0);
        
        cb(ih, (float)delta/120.0f, p.x, p.y, status);
      }
      else
      {
        if (ih->data->sb & IUP_SB_VERT)
        {
          int i, winop = delta>0? SB_LINEUP: SB_LINEDOWN;
          delta = (short)abs(delta/120);
          for (i=0; i < delta; i++)
            SendMessage(ih->handle, WM_VSCROLL, MAKELONG(winop, 0), 0);
        }
      }

      *result = 0;
      return 1;
    }
  case WM_XBUTTONDBLCLK:
  case WM_LBUTTONDBLCLK:
  case WM_MBUTTONDBLCLK:
  case WM_RBUTTONDBLCLK:
  case WM_XBUTTONDOWN:
  case WM_LBUTTONDOWN:
  case WM_MBUTTONDOWN:
  case WM_RBUTTONDOWN:
    {
      /* Force focus on canvas click */
      if (iupAttribGetBoolean(ih, "CANFOCUS"))
        SetFocus(ih->handle);

      SetCapture(ih->handle);

      if (iupwinButtonDown(ih, msg, wp, lp))
      {
        /* refresh the cursor, it could have been changed in BUTTON_CB */
        iupwinRefreshCursor(ih);
      }

      if (msg==WM_XBUTTONDOWN || msg==WM_XBUTTONDBLCLK)
        *result = 1;
      else
        *result = 0;
      return 1;
    }
  case WM_MOUSEMOVE:
    {
      if (iupwinMouseMove(ih, msg, wp, lp))
      {
        /* refresh the cursor, it could have been changed in MOTION_CB */
        iupwinRefreshCursor(ih);
      }

      break; /* let iupwinBaseMsgProc process enter/leavewin */
    }
  case WM_XBUTTONUP:
  case WM_LBUTTONUP:
  case WM_MBUTTONUP:
  case WM_RBUTTONUP:
    {
      ReleaseCapture();

      if (iupwinButtonUp(ih, msg, wp, lp))
      {
        /* refresh the cursor, it could have been changed in BUTTON_CB */
        iupwinRefreshCursor(ih);
      }

      *result = 0;
      if (msg==WM_XBUTTONUP)
        *result = 1;
      return 1;
    }
  case WM_KILLFOCUS:
    {
      if (GetCapture() == ih->handle)
        ReleaseCapture();
      break;
    }
  case WM_INITMENU:
    /* abort capture if a menu is opened */
    ReleaseCapture();
    break;
  case WM_VSCROLL:
    /* only process the scrollbar if not a MDI client AND a standard scrollbar */
    if (!iupAttribGetBoolean(ih, "MDICLIENT") && lp == 0)
    {
      winCanvasProcessVerScroll(ih, LOWORD(wp));
      *result = 0;
      return 1;
    }
  case WM_HSCROLL:
    /* only process the scrollbar if not a MDI client AND a standard scrollbar */
    if (!iupAttribGetBoolean(ih, "MDICLIENT") && lp == 0)
    {
      winCanvasProcessHorScroll(ih, LOWORD(wp));
      *result = 0;
      return 1;
    }
  case WM_SETFOCUS:
    if (!iupAttribGetBoolean(ih, "CANFOCUS"))
    {
      HWND previous = (HWND)wp;
      if (previous && previous != ih->handle)
        SetFocus(previous);
    }
    break;
  case WM_NCHITTEST:
    {
      if (iupAttribGetBoolean(ih, "HTTRANSPARENT"))
      {
        *result = HTTRANSPARENT;
        return 1;
      }

      break;
    }
  }

  /* can be a container */
  if (ih->iclass->childtype != IUP_CHILDNONE || ih->firstchild)
    return iupwinBaseContainerMsgProc(ih, msg, wp, lp, result);
  else
    return iupwinBaseMsgProc(ih, msg, wp, lp, result);
}

static int winCanvasMapMethod(Ihandle* ih)
{
  CLIENTCREATESTRUCT clientstruct;
  void *clientdata = NULL;
  TCHAR *classname;
  DWORD dwStyle = WS_CHILD|WS_CLIPSIBLINGS, 
      dwExStyle = 0;

  if (!ih->parent)
    return IUP_ERROR;

  if (ih->iclass->is_interactive)
  {
    if (iupAttribGetBoolean(ih, "CANFOCUS"))
      dwStyle |= WS_TABSTOP;
  }
                           
  if (ih->iclass->childtype != IUP_CHILDNONE || ih->firstchild) /* can be a container */
    iupwinGetNativeParentStyle(ih, &dwExStyle, &dwStyle);

  if (iupAttribGetBoolean(ih, "BORDER"))
    dwStyle |= WS_BORDER;

  ih->data->sb = iupBaseGetScrollbar(ih);
  if (ih->data->sb & IUP_SB_HORIZ)
    dwStyle |= WS_HSCROLL;
  if (ih->data->sb & IUP_SB_VERT)
    dwStyle |= WS_VSCROLL;
                           
  if (iupAttribGetBoolean(ih, "MDICLIENT"))  
  {
    /* creating a MDI Client that will be inside the MDI Frame, 
       it will work as parent of all MDI children */
    Ihandle *winmenu = IupGetAttributeHandle(ih, "MDIMENU");

    classname = TEXT("mdiclient");
    dwStyle = WS_CHILD|WS_CLIPCHILDREN|WS_VSCROLL|WS_HSCROLL|MDIS_ALLCHILDSTYLES;
    dwExStyle = WS_EX_CLIENTEDGE;

    iupAttribSet(ih, "BORDER", "NO");

    iupAttribSet(IupGetDialog(ih), "MDICLIENT_HANDLE", (char*)ih);

    clientdata = &clientstruct;
    clientstruct.hWindowMenu = winmenu? winmenu->handle: NULL;

    /* The system increments the identifier 
       for each additional MDI child window the application creates, 
       and reassigns identifiers when the application 
       destroys a window to keep the range of identifiers contiguous. */
    clientstruct.idFirstChild = IUP_MDI_FIRSTCHILD;
  }
  else 
    classname = TEXT("IupCanvas");

  if (!iupwinCreateWindow(ih, classname, dwExStyle, dwStyle, clientdata))
    return IUP_ERROR;

  IupSetCallback(ih, "_IUPWIN_CTRLMSGPROC_CB", (Icallback)winCanvasMsgProc);

  /* configure for DROP of files */
  if (IupGetCallback(ih, "DROPFILES_CB"))
    iupAttribSet(ih, "DROPFILESTARGET", "YES");

  winCanvasSetDXAttrib(ih, NULL);
  winCanvasSetDYAttrib(ih, NULL);

  return IUP_NOERROR;
}

static void winCanvasMDICloseChildren(Ihandle* client)
{
  HWND hWndChild = (HWND)SendMessage(client->handle, WM_MDIGETACTIVE, 0, 0);

  /* As long as the MDI client has a child, close it */
  while (hWndChild)
  {
    Ihandle* child = iupwinHandleGet(hWndChild); 
    if (iupObjectCheck(child) && iupAttribGetBoolean(child, "MDICHILD"))
      IupDestroy(child);

    hWndChild = (HWND)SendMessage(client->handle, WM_MDIGETACTIVE, 0, 0);
  }
}

static void winCanvasUnMapMethod(Ihandle* ih)
{
  if (iupAttribGetBoolean(ih, "MDICLIENT")) 
  {
    /* hide the MDI client window to avoid multiple re-paints */
    ShowWindow(ih->handle, SW_HIDE);

    /* must destroy all MDI Children */
    winCanvasMDICloseChildren(ih);

    DestroyWindow(ih->handle);

    /* mdiclient class is not a IUP class, must return here */
    return;
  }

  iupwinTipsDestroy(ih);
  iupwinDestroyDragDrop(ih);

  /* remove the association before destroying */
  iupwinHandleRemove(ih->handle);

  /* remove from parent and destroys window */
  SetParent(ih->handle, NULL);
  DestroyWindow(ih->handle);
}

static void winCanvasRegisterClass(void)
{
  WNDCLASS wndclass;
  ZeroMemory(&wndclass, sizeof(WNDCLASS));
  
  wndclass.hInstance      = iupwin_hinstance;
  wndclass.lpszClassName  = TEXT("IupCanvas");
  wndclass.lpfnWndProc    = (WNDPROC)DefWindowProc;
  wndclass.hCursor        = LoadCursor(NULL, IDC_ARROW);
  wndclass.style          = CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW; /* using CS_OWNDC will minimize the work of Activate in the CD library */
  wndclass.hbrBackground  = NULL;  /* remove the background to optimize redraw */
   
  RegisterClass(&wndclass);
}

void iupdrvCanvasInitClass(Iclass* ic)
{
  if (!iupwinClassExist(TEXT("IupCanvas")))
    winCanvasRegisterClass();

  /* Driver Dependent Class functions */
  ic->Map = winCanvasMapMethod;
  ic->UnMap = winCanvasUnMapMethod;

  /* Driver Dependent Attribute functions */

  /* Visual */
  iupClassRegisterAttribute(ic, "BGCOLOR", NULL, winCanvasSetBgColorAttrib, "255 255 255", NULL, IUPAF_DEFAULT);   /* force new default value */

  /* IupCanvas only */
  iupClassRegisterAttribute(ic, "DRAWSIZE", winCanvasGetDrawSizeAttrib, NULL, NULL, NULL, IUPAF_READONLY|IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "DX", NULL, winCanvasSetDXAttrib, NULL, NULL, IUPAF_NO_INHERIT);  /* force new default value */
  iupClassRegisterAttribute(ic, "DY", NULL, winCanvasSetDYAttrib, NULL, NULL, IUPAF_NO_INHERIT);  /* force new default value */
  iupClassRegisterAttribute(ic, "POSX", iupCanvasGetPosXAttrib, winCanvasSetPosXAttrib, "0", NULL, IUPAF_NO_INHERIT);  /* force new default value */
  iupClassRegisterAttribute(ic, "POSY", iupCanvasGetPosYAttrib, winCanvasSetPosYAttrib, "0", NULL, IUPAF_NO_INHERIT);  /* force new default value */
  iupClassRegisterAttribute(ic, "XAUTOHIDE", NULL, NULL, "YES", NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "YAUTOHIDE", NULL, NULL, "YES", NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);

  /* IupCanvas Windows only */
  iupClassRegisterAttribute(ic, "HWND", iupBaseGetWidAttrib, NULL, NULL, NULL, IUPAF_NO_STRING|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "HDC_WMPAINT", NULL, NULL, NULL, NULL, IUPAF_NO_STRING|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "HTTRANSPARENT", NULL, NULL, NULL, NULL, IUPAF_NO_INHERIT);

  /* Not Supported */
  iupClassRegisterAttribute(ic, "BACKINGSTORE", NULL, NULL, "YES", NULL, IUPAF_NOT_SUPPORTED|IUPAF_NO_INHERIT);
}
