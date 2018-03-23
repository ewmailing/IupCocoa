/** \file
 * \brief Draw Functions
 *
 * See Copyright Notice in "iup.h"
 */

#include <windows.h>
#include <uxtheme.h>

#if (_MSC_VER >= 1700)  /* Visual C++ 11.0 ( Visual Studio 2012) */
#include <vssym32.h>
#else
#include <tmschema.h>
#endif


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "iup.h"

#include "iup_attrib.h"
#include "iup_class.h"
#include "iup_str.h"
#include "iup_object.h"
#include "iup_image.h"
#include "iup_drvdraw.h"

#include "iupwin_drv.h"
#include "iupwin_info.h"
#include "iupwin_draw.h"
#include "iupwin_str.h"


#ifndef TABP_AEROWIZARDBODY
#define TABP_AEROWIZARDBODY  11  /* Not defined for MingW and Cygwin */
#endif


/******************************************************************************
                             Themes
*******************************************************************************/


typedef HTHEME  (STDAPICALLTYPE *_winThemeOpenData)(HWND hwnd, LPCWSTR pszClassList);
typedef HRESULT (STDAPICALLTYPE *_winThemeCloseData)(HTHEME hTheme);
typedef HRESULT (STDAPICALLTYPE *_winThemeDrawBackground)(HTHEME hTheme, HDC hDC, int iPartId, int iStateId, const RECT *pRect, const RECT *pClipRect);
typedef HRESULT (STDAPICALLTYPE *_winThemeGetColor)(HTHEME hTheme, int iPartId, int iStateId, int iPropId, COLORREF *pColor);

static _winThemeOpenData winThemeOpenData = NULL;
static _winThemeCloseData winThemeCloseData = NULL;
static _winThemeDrawBackground winThemeDrawBackground = NULL;
static _winThemeGetColor winThemeGetColor = NULL;

typedef BOOL (CALLBACK* _winAlphaBlendFunc)( HDC hdcDest, 
                             int xoriginDest, int yoriginDest, 
                             int wDest, int hDest, HDC hdcSrc, 
                             int xoriginSrc, int yoriginSrc, 
                             int wSrc, int hSrc, 
                             BLENDFUNCTION ftn);
static _winAlphaBlendFunc winAlphaBlend = NULL;

static int winDrawThemeEnabled(void)
{
  return winThemeOpenData? 1: 0;
}

void iupwinDrawInit(void)
{
  if (!winAlphaBlend)
  {
    HINSTANCE lib = LoadLibrary(TEXT("Msimg32"));
    if (lib)
      winAlphaBlend = (_winAlphaBlendFunc)GetProcAddress(lib, "AlphaBlend");
  }

  if (!winThemeOpenData && iupwin_comctl32ver6)
  {
    HMODULE hinstDll = LoadLibrary(TEXT("uxtheme.dll"));
    if (hinstDll)
    {
      winThemeOpenData = (_winThemeOpenData)GetProcAddress(hinstDll, "OpenThemeData");
      winThemeCloseData = (_winThemeCloseData)GetProcAddress(hinstDll, "CloseThemeData");
      winThemeDrawBackground = (_winThemeDrawBackground)GetProcAddress(hinstDll, "DrawThemeBackground");
      winThemeGetColor = (_winThemeGetColor)GetProcAddress(hinstDll, "GetThemeColor");
    }
  }
}

static int winDrawGetThemeStateId(int itemState)
{
  if (itemState & ODS_DISABLED)
    return PBS_DISABLED; 
  else if (itemState & ODS_SELECTED)
    return PBS_PRESSED;  
  else if (itemState & ODS_HOTLIGHT)
    return PBS_HOT;      
  else if (itemState & ODS_DEFAULT)
    return PBS_DEFAULTED;
  else
    return PBS_NORMAL;       
}

static int winDrawThemeButtonBorder(HWND hWnd, HDC hDC, RECT *rect, UINT itemState)
{
  int iStateId;
  HTHEME hTheme;

  if (!winDrawThemeEnabled()) 
    return 0; 

  hTheme = winThemeOpenData(hWnd, L"BUTTON");
  if (!hTheme) 
    return 0;

  iStateId = winDrawGetThemeStateId(itemState);

  winThemeDrawBackground(hTheme, hDC, BP_PUSHBUTTON, iStateId, rect, NULL);

  winThemeCloseData(hTheme);
  return 1;
}

static int winDrawTheme3StateButton(HWND hWnd, HDC hDC, RECT *rect)
{
  HTHEME hTheme;

  if (!winDrawThemeEnabled()) 
    return 0; 

  hTheme = winThemeOpenData(hWnd, L"BUTTON");
  if (!hTheme) 
    return 0;

  winThemeDrawBackground(hTheme, hDC, BP_CHECKBOX, CBS_MIXEDNORMAL, rect, NULL);

  winThemeCloseData(hTheme);
  return 1;
}

void iupwinDrawThemeFrameBorder(HWND hWnd, HDC hDC, RECT *rect, UINT itemState)
{
  int iStateId = GBS_NORMAL;
  HTHEME hTheme;

  if (!winDrawThemeEnabled()) 
    return; 

  hTheme = winThemeOpenData(hWnd, L"BUTTON");
  if (!hTheme) 
    return;

  if (itemState & ODS_DISABLED)
    iStateId = GBS_DISABLED;

  winThemeDrawBackground(hTheme, hDC, BP_GROUPBOX, iStateId, rect, NULL);

  winThemeCloseData(hTheme);
}

int iupwinDrawGetThemeTabsBgColor(HWND hWnd, COLORREF *color)
{
  HTHEME hTheme;
  HRESULT ret;

  if (!winDrawThemeEnabled()) 
    return 0; 

  hTheme = winThemeOpenData(hWnd, L"TAB");
  if (!hTheme) 
    return 0;

  if (iupwinIsVistaOrNew())
    ret = winThemeGetColor(hTheme, TABP_AEROWIZARDBODY, TIS_NORMAL, TMT_FILLCOLORHINT, color);
  else
    ret = winThemeGetColor(hTheme, TABP_BODY, TIS_NORMAL, TMT_FILLCOLORHINT, color);

  winThemeCloseData(hTheme);
  return (ret == S_OK)? 1: 0;
}

int iupwinDrawGetThemeButtonBgColor(HWND hWnd, COLORREF *color)
{
  HTHEME hTheme;
  HRESULT ret;

  if (!winDrawThemeEnabled()) 
    return 0; 

  hTheme = winThemeOpenData(hWnd, L"BUTTON");
  if (!hTheme) 
    return 0;

  ret = winThemeGetColor(hTheme, BP_PUSHBUTTON, PBS_NORMAL, TMT_FILLCOLORHINT, color);

  winThemeCloseData(hTheme);
  return (ret == S_OK)? 1: 0;
}

int iupwinDrawGetThemeFrameFgColor(HWND hWnd, COLORREF *color)
{
  HTHEME hTheme;
  HRESULT ret;

  if (!winDrawThemeEnabled()) 
    return 0; 

  hTheme = winThemeOpenData(hWnd, L"BUTTON");
  if (!hTheme) 
    return 0;

  ret = winThemeGetColor(hTheme, BP_GROUPBOX, GBS_NORMAL, TMT_TEXTCOLOR, color);

  winThemeCloseData(hTheme);
  return (ret == S_OK)? 1: 0;
}

void iupwinDrawRemoveTheme(HWND hwnd)
{
  typedef HRESULT (STDAPICALLTYPE *winSetWindowTheme)(HWND hwnd, LPCWSTR pszSubAppName, LPCWSTR pszSubIdList);
  static winSetWindowTheme mySetWindowTheme = NULL;
  if (!mySetWindowTheme)
  {
    HMODULE hinstDll = LoadLibrary(TEXT("uxtheme.dll"));
    if (hinstDll)
      mySetWindowTheme = (winSetWindowTheme)GetProcAddress(hinstDll, "SetWindowTheme");
  }

  if (mySetWindowTheme)
    mySetWindowTheme(hwnd, L"", L"");
}


/******************************************************************************
                             Utilities
*******************************************************************************/


void iupwinDrawText(HDC hDC, const char* text, int x, int y, int width, int height, HFONT hFont, COLORREF fgcolor, int style)
{
  COLORREF oldcolor;
  RECT rect;
  HFONT hOldFont = (HFONT)SelectObject(hDC, hFont);

  rect.left = x;
  rect.top = y;
  rect.right = x+width;
  rect.bottom = y+height;

  SetTextAlign(hDC, TA_TOP|TA_LEFT);
  SetBkMode(hDC, TRANSPARENT);
  oldcolor = SetTextColor(hDC, fgcolor);

  DrawText(hDC, iupwinStrToSystem(text), -1, &rect, style|DT_NOCLIP);

  SelectObject(hDC, hOldFont);
  SetTextColor(hDC, oldcolor);
  SetBkMode(hDC, OPAQUE);
}

void iupwinDrawBitmap(HDC hDC, HBITMAP hBitmap, HBITMAP hMask, int x, int y, int width, int height, int bpp)
{
  HDC hMemDC = CreateCompatibleDC(hDC);
  HBITMAP oldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);

  if (bpp == 32 && winAlphaBlend)
  {
    BLENDFUNCTION blendfunc;
    blendfunc.BlendOp = AC_SRC_OVER;
    blendfunc.BlendFlags = 0;
    blendfunc.SourceConstantAlpha = 0xFF;
    blendfunc.AlphaFormat = AC_SRC_ALPHA;

    winAlphaBlend(hDC, x, y, width, height, 
                  hMemDC, 0, 0, width, height, 
                  blendfunc);
  }
  else if (bpp == 8 && hMask)
    MaskBlt(hDC, x, y, width, height, 
            hMemDC, 0, 0, 
            hMask, 0, 0, MAKEROP4(SRCCOPY, 0xAA0000));
  else
    BitBlt(hDC, x, y, width, height, 
           hMemDC, 0, 0, 
           SRCCOPY);

  SelectObject(hMemDC, oldBitmap);
  DeleteDC(hMemDC);
}

static int winDrawGetStateId(int itemState)
{
  if (itemState & ODS_DISABLED)
    return DFCS_INACTIVE; 
  else if (itemState & ODS_SELECTED)
    return DFCS_PUSHED;  
  else if (itemState & ODS_HOTLIGHT)
    return DFCS_HOT;      
  else
    return 0;   
}

void iupwinDrawButtonBorder(HWND hWnd, HDC hDC, RECT *rect, UINT itemState)
{
  if (!winDrawThemeButtonBorder(hWnd, hDC, rect, itemState))
  {
    DrawFrameControl(hDC, rect, DFC_BUTTON, DFCS_BUTTONPUSH | winDrawGetStateId(itemState));
    if (itemState & ODS_DEFAULT)  /* if a button has the focus, must draw the default button frame */
      FrameRect(hDC, rect, (HBRUSH)GetStockObject(BLACK_BRUSH));
  }
}

void iupwinDraw3StateButton(HWND hWnd, HDC hDC, RECT *rect)
{
  if (!winDrawTheme3StateButton(hWnd, hDC, rect))
  {
    /* only used from TreeView where the 3state images are 3px smaller than icon size */
    rect->left += 2;
    rect->top += 2;
    rect->right -= 1;
    rect->bottom -= 1;
    DrawFrameControl(hDC, rect, DFC_BUTTON, DFCS_BUTTON3STATE | DFCS_CHECKED | DFCS_FLAT);
  }
}

void iupwinDrawParentBackground(Ihandle* ih, HDC hDC, RECT* rect)
{
  char* color_str = iupBaseNativeParentGetBgColorAttrib(ih);
  long color = iupDrawStrToColor(color_str, 0);
  SetDCBrushColor(hDC, RGB(iupDrawRed(color),iupDrawGreen(color),iupDrawBlue(color)));
  FillRect(hDC, rect, (HBRUSH)GetStockObject(DC_BRUSH));
}

HDC iupwinDrawCreateBitmapDC(iupwinBitmapDC *bmpDC, HDC hDC, int x, int y, int w, int h)
{
  bmpDC->x = x;
  bmpDC->y = y;
  bmpDC->w = w;
  bmpDC->h = h;
  bmpDC->hDC = hDC;

  bmpDC->hBitmap = CreateCompatibleBitmap(bmpDC->hDC, w, h);
  bmpDC->hBitmapDC = CreateCompatibleDC(bmpDC->hDC);
  bmpDC->hOldBitmap = SelectObject(bmpDC->hBitmapDC, bmpDC->hBitmap);
  return bmpDC->hBitmapDC;
}

void iupwinDrawDestroyBitmapDC(iupwinBitmapDC *bmpDC)
{
  BitBlt(bmpDC->hDC, bmpDC->x, bmpDC->y, bmpDC->w, bmpDC->h, bmpDC->hBitmapDC, 0, 0, SRCCOPY);
  SelectObject(bmpDC->hBitmapDC, bmpDC->hOldBitmap);
  DeleteObject(bmpDC->hBitmap);
  DeleteDC(bmpDC->hBitmapDC);
}

int iupwinCustomDrawToDrawItem(Ihandle* ih, NMHDR* msg_info, int *result, IFdrawItem drawitem_cb)
{
  NMCUSTOMDRAW *customdraw = (NMCUSTOMDRAW*)msg_info;

  if (customdraw->dwDrawStage == CDDS_PREERASE)
  {
    DRAWITEMSTRUCT drawitem;
    drawitem.itemState = 0;

    /* combination of the following flags. */
    if (customdraw->uItemState & CDIS_DISABLED)
      drawitem.itemState |= ODS_DISABLED;
    if (customdraw->uItemState & CDIS_SELECTED)
      drawitem.itemState |= ODS_SELECTED;
    if (customdraw->uItemState & CDIS_HOT)
      drawitem.itemState |= ODS_HOTLIGHT;
    if (customdraw->uItemState & CDIS_DEFAULT)
      drawitem.itemState |= ODS_DEFAULT;

    if (customdraw->uItemState & CDIS_FOCUS)
      drawitem.itemState |= ODS_FOCUS;

    if (!(customdraw->uItemState & CDIS_SHOWKEYBOARDCUES))
      drawitem.itemState |= ODS_NOFOCUSRECT | ODS_NOACCEL;

    drawitem.hDC = customdraw->hdc;
    drawitem.rcItem = customdraw->rc;

    drawitem_cb(ih, (void*)&drawitem);

    *result = CDRF_SKIPDEFAULT;
    return 1;
  }
  else
    return 0;
}


/******************************************************************************
                             Simple Draw
*******************************************************************************/


struct _IdrawCanvas{
  Ihandle* ih;
  int w, h;

  HWND hWnd;
  int release_dc;
  HBITMAP hBitmap, hOldBitmap;
  HDC hBitmapDC, hDC;
};

IdrawCanvas* iupdrvDrawCreateCanvas(Ihandle* ih)
{
  IdrawCanvas* dc = calloc(1, sizeof(IdrawCanvas));
  RECT rect;

  dc->ih = ih;

  dc->hWnd = (HWND)IupGetAttribute(ih, "HWND");

  /* valid only inside the ACTION callback of an IupCanvas */
  dc->hDC = (HDC)IupGetAttribute(ih, "HDC_WMPAINT");
  if (!dc->hDC)
  {
    dc->hDC = GetDC(dc->hWnd);
    dc->release_dc = 1;
  }

  GetClientRect(dc->hWnd, &rect);
  dc->w = rect.right - rect.left;
  dc->h = rect.bottom - rect.top;

  dc->hBitmap = CreateCompatibleBitmap(dc->hDC, dc->w, dc->h);
  dc->hBitmapDC = CreateCompatibleDC(dc->hDC);
  dc->hOldBitmap = SelectObject(dc->hBitmapDC, dc->hBitmap);

  SetBkMode(dc->hBitmapDC, TRANSPARENT);
  SetTextAlign(dc->hBitmapDC, TA_TOP|TA_LEFT);

  return dc;
}

void iupdrvDrawKillCanvas(IdrawCanvas* dc)
{
  SelectObject(dc->hBitmapDC, dc->hOldBitmap);
  DeleteObject(dc->hBitmap);
  DeleteDC(dc->hBitmapDC);
  if (dc->release_dc)
    DeleteDC(dc->hDC);

  free(dc);
}

void iupdrvDrawUpdateSize(IdrawCanvas* dc)
{
  int w, h;
  RECT rect;

  GetClientRect(dc->hWnd, &rect);
  w = rect.right - rect.left;
  h = rect.bottom - rect.top;

  if (w != dc->w || h != dc->h)
  {
    dc->w = w;
    dc->h = h;

    SelectObject(dc->hBitmapDC, dc->hOldBitmap);
    DeleteObject(dc->hBitmap);
    DeleteDC(dc->hBitmapDC);

    dc->hBitmap = CreateCompatibleBitmap(dc->hDC, dc->w, dc->h);
    dc->hBitmapDC = CreateCompatibleDC(dc->hDC);
    dc->hOldBitmap = SelectObject(dc->hBitmapDC, dc->hBitmap);

    SetBkMode(dc->hBitmapDC, TRANSPARENT);
    SetTextAlign(dc->hBitmapDC, TA_TOP|TA_LEFT);
  }
}

void iupdrvDrawFlush(IdrawCanvas* dc)
{
  BitBlt(dc->hDC, 0, 0, dc->w, dc->h, dc->hBitmapDC, 0, 0, SRCCOPY);
}

void iupdrvDrawGetSize(IdrawCanvas* dc, int *w, int *h)
{
  if (w) *w = dc->w;
  if (h) *h = dc->h;
}

static int iDrawGetLineStyle(int style)
{
  if (style == IUP_DRAW_STROKE_DASH)
    return PS_DASH;
  else if (style == IUP_DRAW_STROKE_DOT)
    return PS_DOT;
  else
    return PS_SOLID;
}

void iupdrvDrawRectangle(IdrawCanvas* dc, int x1, int y1, int x2, int y2, long color, int style, int line_width)
{
  SetDCBrushColor(dc->hBitmapDC, RGB(iupDrawRed(color),iupDrawGreen(color),iupDrawBlue(color)));

  if (style == IUP_DRAW_FILL)
  {
    RECT rect;
    iupDrawCheckSwapCoord(x1, x2);
    iupDrawCheckSwapCoord(y1, y2);
    SetRect(&rect, x1, y1, x2 + 1, y2 + 1);
    FillRect(dc->hBitmapDC, &rect, (HBRUSH)GetStockObject(DC_BRUSH));
  }
  else if (style == IUP_DRAW_STROKE && line_width == 1)
  {
    RECT rect;
    iupDrawCheckSwapCoord(x1, x2);
    iupDrawCheckSwapCoord(y1, y2);
    SetRect(&rect, x1, y1, x2 + 1, y2 + 1);
    FrameRect(dc->hBitmapDC, &rect, (HBRUSH)GetStockObject(DC_BRUSH));
  }
  else
  {
    POINT line_poly[5];
    HPEN hPen = CreatePen(iDrawGetLineStyle(style), line_width, RGB(iupDrawRed(color),iupDrawGreen(color),iupDrawBlue(color)));
    HPEN hPenOld = SelectObject(dc->hBitmapDC, hPen);
    line_poly[0].x = x1;
    line_poly[0].y = y1;
    line_poly[1].x = x1;
    line_poly[1].y = y2;
    line_poly[2].x = x2;
    line_poly[2].y = y2;
    line_poly[3].x = x2;
    line_poly[3].y = y1;
    line_poly[4].x = x1;
    line_poly[4].y = y1;
    Polyline(dc->hBitmapDC, line_poly, 5);
    SelectObject(dc->hBitmapDC, hPenOld);
    DeleteObject(hPen);
  }
}

void iupdrvDrawLine(IdrawCanvas* dc, int x1, int y1, int x2, int y2, long color, int style, int line_width)
{
  POINT line_poly[2];
  HPEN hPen = CreatePen(iDrawGetLineStyle(style), line_width, RGB(iupDrawRed(color),iupDrawGreen(color),iupDrawBlue(color)));
  HPEN hPenOld = SelectObject(dc->hBitmapDC, hPen);

  line_poly[0].x = x1;
  line_poly[0].y = y1;
  line_poly[1].x = x2;
  line_poly[1].y = y2;
  Polyline(dc->hBitmapDC, line_poly, 2);
  SetPixelV(dc->hBitmapDC, x2, y2, RGB(iupDrawRed(color),iupDrawGreen(color),iupDrawBlue(color)));

  SelectObject(dc->hBitmapDC, hPenOld);
  DeleteObject(hPen);
}

#define IUP_DEG2RAD  0.01745329252  /* degrees to radians (rad = DEG2RAD * deg) */

static int winDrawCalcArc(int c1, int c2, double a, int horiz)
{
  double proj;
  int pos;
  if (horiz)
    proj = cos(IUP_DEG2RAD * a);
  else
    proj = - sin(IUP_DEG2RAD * a);
  pos = (c2 + c1) / 2 + iupROUND((c2 - c1 + 1) * proj / 2.0);
  return pos;
}

void iupdrvDrawArc(IdrawCanvas* dc, int x1, int y1, int x2, int y2, double a1, double a2, long color, int style, int line_width)
{
  int XStartArc, XEndArc, YStartArc, YEndArc;

  iupDrawCheckSwapCoord(x1, x2);
  iupDrawCheckSwapCoord(y1, y2);

  XStartArc = winDrawCalcArc(x1, x2, a1, 1);
  XEndArc = winDrawCalcArc(x1, x2, a2, 1);
  YStartArc = winDrawCalcArc(y1, y2, a1, 0);
  YEndArc = winDrawCalcArc(y1, y2, a2, 0);

  if (style==IUP_DRAW_FILL)
  {
    HBRUSH hBrush = CreateSolidBrush(RGB(iupDrawRed(color),iupDrawGreen(color),iupDrawBlue(color)));
    HPEN hBrushOld = SelectObject(dc->hBitmapDC, hBrush); 
    BeginPath(dc->hBitmapDC); 
    Pie(dc->hBitmapDC, x1, y1, x2 + 1, y2 + 1, XStartArc, YStartArc, XEndArc, YEndArc);
    EndPath(dc->hBitmapDC);
    FillPath(dc->hBitmapDC);
    SelectObject(dc->hBitmapDC, hBrushOld);
    DeleteObject(hBrush);
  }
  else
  {
    HPEN hPen = CreatePen(iDrawGetLineStyle(style), line_width, RGB(iupDrawRed(color),iupDrawGreen(color),iupDrawBlue(color)));
    HPEN hPenOld = SelectObject(dc->hBitmapDC, hPen);
    Arc(dc->hBitmapDC, x1, y1, x2 + 1, y2 + 1, XStartArc, YStartArc, XEndArc, YEndArc);
    SelectObject(dc->hBitmapDC, hPenOld);
    DeleteObject(hPen);
  }
}

void iupdrvDrawPolygon(IdrawCanvas* dc, int* points, int count, long color, int style, int line_width)
{
  if (style==IUP_DRAW_FILL)
  {
    HBRUSH hBrush = CreateSolidBrush(RGB(iupDrawRed(color),iupDrawGreen(color),iupDrawBlue(color)));
    HPEN hBrushOld = SelectObject(dc->hBitmapDC, hBrush); 
    BeginPath(dc->hBitmapDC); 
    Polygon(dc->hBitmapDC, (POINT*)points, count);
    EndPath(dc->hBitmapDC);
    FillPath(dc->hBitmapDC);
    SelectObject(dc->hBitmapDC, hBrushOld);
    DeleteObject(hBrush);
  }
  else
  {
    HPEN hPen = CreatePen(iDrawGetLineStyle(style), line_width, RGB(iupDrawRed(color),iupDrawGreen(color),iupDrawBlue(color)));
    HPEN hPenOld = SelectObject(dc->hBitmapDC, hPen);
    Polyline(dc->hBitmapDC, (POINT*)points, count);
    SelectObject(dc->hBitmapDC, hPenOld);
    DeleteObject(hPen);
  }
}

void iupdrvDrawSetClipRect(IdrawCanvas* dc, int x1, int y1, int x2, int y2)
{
  HRGN clip_hrgn;

  iupDrawCheckSwapCoord(x1, x2);
  iupDrawCheckSwapCoord(y1, y2);

  clip_hrgn = CreateRectRgn(x1, y1, x2 + 1, y2 + 1);
  SelectClipRgn(dc->hBitmapDC, clip_hrgn);
  DeleteObject(clip_hrgn);
}

void iupdrvDrawResetClip(IdrawCanvas* dc)
{
  SelectClipRgn(dc->hBitmapDC, NULL);
}

void iupdrvDrawText(IdrawCanvas* dc, const char* text, int len, int x, int y, int w, int h, long color, const char* font, int align)
{
  RECT rect;
  TCHAR* wtext;
  UINT uFormat = DT_LEFT;

  HFONT hOldFont, hFont = (HFONT)iupwinGetHFont(font);
  SetTextColor(dc->hBitmapDC, RGB(iupDrawRed(color),iupDrawGreen(color),iupDrawBlue(color)));
  hOldFont = SelectObject(dc->hBitmapDC, hFont);

  rect.left = x;
  rect.right = x + w;
  rect.top = y;
  rect.bottom = y + h;

  wtext = iupwinStrToSystemLen(text, &len);

  if (align == IUP_ALIGN_ARIGHT)
    uFormat = DT_RIGHT;
  else if (align == IUP_ALIGN_ACENTER)
    uFormat = DT_CENTER;

  DrawText(dc->hBitmapDC, wtext, len, &rect, uFormat);

  SelectObject(dc->hBitmapDC, hOldFont);
}

void iupdrvDrawImage(IdrawCanvas* dc, const char* name, int make_inactive, int x, int y)
{
  int bpp, img_w, img_h;
  HBITMAP hMask = NULL;
  HBITMAP hBitmap = (HBITMAP)iupImageGetImage(name, dc->ih, make_inactive);
  if (!hBitmap)
    return;

  /* must use this info, since image can be a driver image loaded from resources */
  iupdrvImageGetInfo(hBitmap, &img_w, &img_h, &bpp);

  if (bpp == 8)
    hMask = iupdrvImageCreateMask(IupGetHandle(name));

  iupwinDrawBitmap(dc->hBitmapDC, hBitmap, hMask, x, y, img_w, img_h, bpp);

  if (hMask)
    DeleteObject(hMask);
}

void iupdrvDrawSelectRect(IdrawCanvas* dc, int x1, int y1, int x2, int y2)
{
  iupDrawCheckSwapCoord(x1, x2);
  iupDrawCheckSwapCoord(y1, y2);

  BitBlt(dc->hBitmapDC, x1, y1, x2 - x1 + 1, y2 - y1 + 1, dc->hBitmapDC, x1, y1, DSTINVERT);
}

void iupdrvDrawFocusRect(IdrawCanvas* dc, int x1, int y1, int x2, int y2)
{
  RECT rect;

  iupDrawCheckSwapCoord(x1, x2);
  iupDrawCheckSwapCoord(y1, y2);

  rect.left = x1;  
  rect.right = x2 + 1;
  rect.top = y1;
  rect.bottom = y2 + 1;

  DrawFocusRect(dc->hBitmapDC, &rect);
}
