/** \file
 * \brief Scintilla control: Markers
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


#include <Scintilla.h>

#include "iup.h"

#include "iup_object.h"
#include "iup_attrib.h"
#include "iup_str.h"
#include "iup_image.h"

#include "iupsci.h"

/***** MARKERS *****
SCI_MARKERDEFINE(int markerNumber, int markerSymbols)
--SCI_MARKERDEFINEPIXMAP(int markerNumber, const char *xpm)
SCI_RGBAIMAGESETWIDTH(int width)
SCI_RGBAIMAGESETHEIGHT(int height)
SCI_RGBAIMAGESETSCALE(int scalePercent)
SCI_MARKERDEFINERGBAIMAGE(int markerNumber, const char *pixels)
SCI_MARKERSYMBOLDEFINED(int markerNumber) 
SCI_MARKERSETFORE(int markerNumber, int colour)
SCI_MARKERSETBACK(int markerNumber, int colour)
SCI_MARKERSETBACKSELECTED(int markerNumber, int colour)
SCI_MARKERENABLEHIGHLIGHT(int enabled)
SCI_MARKERSETALPHA(int markerNumber, int alpha)
SCI_MARKERADD(int line, int markerNumber)
--SCI_MARKERADDSET(int line, int markerMask)
SCI_MARKERDELETE(int line, int markerNumber)
--SCI_MARKERDELETEALL(int markerNumber)
--SCI_MARKERGET(int line)
--SCI_MARKERNEXT(int lineStart, int markerMask)
--SCI_MARKERPREVIOUS(int lineStart, int markerMask)
--SCI_MARKERLINEFROMHANDLE(int handle)
--SCI_MARKERDELETEHANDLE(int handle)
*/

static int iScintillaSetMarkerSymbolAttribId(Ihandle* ih, int markerNumber, const char* value)
{
  int markerSymbol;

  /* Setting marker symbols */
  if (iupStrEqualNoCase(value, "CIRCLE"))
    markerSymbol = SC_MARK_CIRCLE;
  else if (iupStrEqualNoCase(value, "ROUNDRECT"))
    markerSymbol = SC_MARK_ROUNDRECT;
  else if (iupStrEqualNoCase(value, "ARROW"))
    markerSymbol = SC_MARK_ARROW;
  else if (iupStrEqualNoCase(value, "SMALLRECT"))
    markerSymbol = SC_MARK_SMALLRECT;
  else if (iupStrEqualNoCase(value, "SHORTARROW"))
    markerSymbol = SC_MARK_SHORTARROW;
  else if (iupStrEqualNoCase(value, "EMPTY"))    /* invisible */
    markerSymbol = SC_MARK_EMPTY;
  else if (iupStrEqualNoCase(value, "ARROWDOWN"))
    markerSymbol = SC_MARK_ARROWDOWN;
  else if (iupStrEqualNoCase(value, "MINUS"))
    markerSymbol = SC_MARK_MINUS;
  else if (iupStrEqualNoCase(value, "PLUS"))
    markerSymbol = SC_MARK_PLUS;
  else if (iupStrEqualNoCase(value, "VLINE"))
    markerSymbol = SC_MARK_VLINE;
  else if (iupStrEqualNoCase(value, "LCORNER"))
    markerSymbol = SC_MARK_LCORNER;
  else if (iupStrEqualNoCase(value, "TCORNER"))
    markerSymbol = SC_MARK_TCORNER;
  else if (iupStrEqualNoCase(value, "BOXPLUS"))
    markerSymbol = SC_MARK_BOXPLUS;
  else if (iupStrEqualNoCase(value, "BOXPLUSCONNECTED"))
    markerSymbol = SC_MARK_BOXPLUSCONNECTED;
  else if (iupStrEqualNoCase(value, "BOXMINUS"))
    markerSymbol = SC_MARK_BOXMINUS;
  else if (iupStrEqualNoCase(value, "BOXMINUSCONNECTED"))
    markerSymbol = SC_MARK_BOXMINUSCONNECTED;
  else if (iupStrEqualNoCase(value, "LCORNERCURVE"))
    markerSymbol = SC_MARK_LCORNERCURVE;
  else if (iupStrEqualNoCase(value, "TCORNERCURVE"))
    markerSymbol = SC_MARK_TCORNERCURVE;
  else if (iupStrEqualNoCase(value, "CIRCLEPLUS"))
    markerSymbol = SC_MARK_CIRCLEPLUS;
  else if (iupStrEqualNoCase(value, "CIRCLEPLUSCONNECTED"))
    markerSymbol = SC_MARK_CIRCLEPLUSCONNECTED;
  else if (iupStrEqualNoCase(value, "CIRCLEMINUS"))
    markerSymbol = SC_MARK_CIRCLEMINUS;
  else if (iupStrEqualNoCase(value, "CIRCLEMINUSCONNECTED"))
    markerSymbol = SC_MARK_CIRCLEMINUSCONNECTED;
  else if (iupStrEqualNoCase(value, "BACKGROUND"))   /* line background color */
    markerSymbol = SC_MARK_BACKGROUND;
  else if (iupStrEqualNoCase(value, "DOTDOTDOT"))
    markerSymbol = SC_MARK_DOTDOTDOT;
  else if (iupStrEqualNoCase(value, "ARROWS"))
    markerSymbol = SC_MARK_ARROWS;
/*  else if (iupStrEqualNoCase(value, "PIXMAP"))
    markerSymbol = SC_MARK_PIXMAP; */
  else if (iupStrEqualNoCase(value, "FULLRECT"))   /* margin background color */
    markerSymbol = SC_MARK_FULLRECT;
  else if (iupStrEqualNoCase(value, "LEFTRECT"))
    markerSymbol = SC_MARK_LEFTRECT;
/*  else if (iupStrEqualNoCase(value, "AVAILABLE"))
    markerSymbol = SC_MARK_AVAILABLE; */
  else if (iupStrEqualNoCase(value, "UNDERLINE"))  /* underline across the line. */
    markerSymbol = SC_MARK_UNDERLINE;
  else if (iupStrEqualNoCase(value, "RGBAIMAGE"))
    markerSymbol = SC_MARK_RGBAIMAGE;
  else if (iupStrEqualPartial(value, "CHARACTER"))
  {
    int c = (int)value[9];
    markerSymbol = SC_MARK_CHARACTER+c;
  }
  else
    return 0;

  IupScintillaSendMessage(ih, SCI_MARKERDEFINE, markerNumber, markerSymbol);
  return 0;
}

static char* iScintillaGetMarkerSymbolAttribId(Ihandle* ih, int markerNumber)
{
  char* markerSymbolStr[32] = {
    "CIRCLE", "ROUNDRECT", "ARROW", "SMALLRECT", "SHORTARROW", "EMPTY", "ARROWDOWN", "MINUS", 
    "PLUS", "VLINE", "LCORNER", "TCORNER", "BOXPLUS", "BOXPLUSCONNECTED", "BOXMINUS", 
    "BOXMINUSCONNECTED", "LCORNERCURVE", "TCORNERCURVE", "CIRCLEPLUS", "CIRCLEPLUSCONNECTED", 
    "CIRCLEMINUS", "CIRCLEMINUSCONNECTED", "BACKGROUND", "DOTDOTDOT", "ARROWS", "PIXMAP", 
    "FULLRECT", "LEFTRECT", "AVAILABLE", "UNDERLINE", "RGBAIMAGE", "CHARACTER"};

  int markerSymbol = (int)IupScintillaSendMessage(ih, SCI_MARKERSYMBOLDEFINED, markerNumber, 0);
    
  if (markerSymbol>SC_MARK_CHARACTER)
  {
    char c = (char)(markerSymbol-SC_MARK_CHARACTER);
    return iupStrReturnStrStr(markerSymbolStr[31], "", c);
  }

  if (markerSymbol<0 || markerSymbol>31)
    return NULL;

  return markerSymbolStr[markerSymbol];
}

static int iScintillaSetMarkerDefineAttrib(Ihandle* ih, const char* value)
{
  char strNumb[25];
  char strSymb[25];
  int markerNumber;

  iupStrToStrStr(value, strNumb, strSymb, '=');

  /* Setting marker numbers */
  if (iupStrEqualNoCase(strNumb, "FOLDEREND"))
    markerNumber = SC_MARKNUM_FOLDEREND;
  else if (iupStrEqualNoCase(strNumb, "FOLDEROPENMID"))
    markerNumber = SC_MARKNUM_FOLDEROPENMID;
  else if (iupStrEqualNoCase(strNumb, "FOLDERMIDTAIL"))
    markerNumber = SC_MARKNUM_FOLDERMIDTAIL;
  else if (iupStrEqualNoCase(strNumb, "FOLDERTAIL"))
    markerNumber = SC_MARKNUM_FOLDERTAIL;
  else if (iupStrEqualNoCase(strNumb, "FOLDERSUB"))
    markerNumber = SC_MARKNUM_FOLDERSUB;
  else if (iupStrEqualNoCase(strNumb, "FOLDER"))
    markerNumber = SC_MARKNUM_FOLDER;
  else if (iupStrEqualNoCase(strNumb, "FOLDEROPEN"))
    markerNumber = SC_MARKNUM_FOLDEROPEN;
  else
  {
    if (!iupStrToInt(strNumb, &markerNumber))
      return 0;

    if (markerNumber<0 || markerNumber>31)
      return 0;
  }

  iScintillaSetMarkerSymbolAttribId(ih, markerNumber, strSymb);
  return 0;
}

static int iScintillaSetMarkerDefineRGBAImageId(Ihandle* ih, int markerNumber, const char* value)
{
  Ihandle* ih_image = iupImageGetHandle(value);
  if (ih_image)
  {
    int bpp = IupGetInt(ih_image, "BPP");
    if (bpp == 32)
    {
      unsigned char* imgdata = (unsigned char*)IupGetAttribute(ih, "WID");
      if (imgdata)
      {
        int w = IupGetInt(ih, "WIDTH");
        int h = IupGetInt(ih, "HEIGHT");
        IupScintillaSendMessage(ih, SCI_RGBAIMAGESETWIDTH,  w, 0);
        IupScintillaSendMessage(ih, SCI_RGBAIMAGESETHEIGHT, h, 0);
        IupScintillaSendMessage(ih, SCI_MARKERDEFINERGBAIMAGE, markerNumber, (sptr_t)imgdata);
      }
    }
  }

  return 0;
}

static int iScintillaSetRGBAImageSetScale(Ihandle* ih, const char* value)
{
  int scale;
  iupStrToInt(value, &scale);
  
  if(scale <= 0) scale = 1;
  
  IupScintillaSendMessage(ih, SCI_RGBAIMAGESETSCALE, scale, 0);
  
  return 0;
}

static int iScintillaSetMarkerFgColorAttribId(Ihandle* ih, int markerNumber, const char* value)
{
  unsigned char r, g, b;

  if (!iupStrToRGB(value, &r, &g, &b))
    return 0;

  IupScintillaSendMessage(ih, SCI_MARKERSETFORE, markerNumber, iupScintillaEncodeColor(r, g, b));

  return 0;
}

static int iScintillaSetMarkerBgColorAttribId(Ihandle* ih, int markerNumber, const char* value)
{
  unsigned char r, g, b;

  if (!iupStrToRGB(value, &r, &g, &b))
    return 0;

  IupScintillaSendMessage(ih, SCI_MARKERSETBACK, markerNumber, iupScintillaEncodeColor(r, g, b));

  return 0;
}

static int iScintillaSetMarkerBgColorSelectedAttribId(Ihandle* ih, int markerNumber, const char* value)
{
  unsigned char r, g, b;

  if (!iupStrToRGB(value, &r, &g, &b))
    return 0;

  IupScintillaSendMessage(ih, SCI_MARKERSETBACKSELECTED, markerNumber, iupScintillaEncodeColor(r, g, b));

  return 0;
}

static int iScintillaSetMarkerAlphaAttribId(Ihandle* ih, int markerNumber, const char* value)
{
  int alpha;
  iupStrToInt(value, &alpha);

  if (alpha < 0 || alpha > 255)
    return 0;

  IupScintillaSendMessage(ih, SCI_MARKERSETALPHA, markerNumber, alpha);

  return 0;
}

static int iScintillaSetMarkerEnableHighlightAttrib(Ihandle *ih, const char *value)
{
  if (iupStrBoolean(value))
    IupScintillaSendMessage(ih, SCI_MARKERENABLEHIGHLIGHT, 1, 0);
  else
    IupScintillaSendMessage(ih, SCI_MARKERENABLEHIGHLIGHT, 0, 0);
  return 0;
}

static int iScintillaSetMarkerAddAttribId(Ihandle* ih, int line, const char* value)
{
  int markerNumber, markerID;
  iupStrToInt(value, &markerNumber);

  markerID = (int)IupScintillaSendMessage(ih, SCI_MARKERADD, line, markerNumber);
  iupAttribSetInt(ih, "LASTMARKERADDHANDLE", markerID);

  return 0;
}

static int iScintillaSetMarkerDeleteAttribId(Ihandle* ih, int line, const char* value)
{
  int markerNumber;
  iupStrToInt(value, &markerNumber);

  IupScintillaSendMessage(ih, SCI_MARKERDELETE, line, markerNumber);

  return 0;
}

static char* iScintillaGetMarkerGetAttribId(Ihandle* ih, int line)
{
  int markers = (int)IupScintillaSendMessage(ih, SCI_MARKERGET, line, 0);
  return iupStrReturnInt(markers);
}

static int iScintillaSetMarkerDeleteAllAttrib(Ihandle* ih, const char* value)
{
  int markerNumber;
  iupStrToInt(value, &markerNumber);

  IupScintillaSendMessage(ih, SCI_MARKERDELETEALL, markerNumber, 0);

  return 0;
}

static int iScintillaSetMarkerNextAttribId(Ihandle* ih, int lineStart, const char* value)
{
  int markerMask, last_marker_found;
  iupStrToInt(value, &markerMask);

  last_marker_found = (int)IupScintillaSendMessage(ih, SCI_MARKERNEXT, lineStart, markerMask);
  iupAttribSetInt(ih, "LASTMARKERFOUND", last_marker_found);

  return 0;
}

static int iScintillaSetMarkerPreviousAttribId(Ihandle* ih, int lineStart, const char* value)
{
  int markerMask, last_marker_found;
  iupStrToInt(value, &markerMask);

  last_marker_found = (int)IupScintillaSendMessage(ih, SCI_MARKERPREVIOUS, lineStart, markerMask);
  iupAttribSetInt(ih, "LASTMARKERFOUND", last_marker_found);

  return 0;
}

static char* iScintillaGetMarkerLineFromHandleAttribId(Ihandle* ih, int markerHandle)
{
  int line = (int)IupScintillaSendMessage(ih, SCI_MARKERLINEFROMHANDLE, markerHandle, 0);
  return iupStrReturnInt(line);
}

static int iScintillaSetMarkerDeleteHandleAttrib(Ihandle* ih, const char* value)
{
  int markerHandle;
  iupStrToInt(value, &markerHandle);

  IupScintillaSendMessage(ih, SCI_MARKERDELETEHANDLE, markerHandle, 0);

  return 0;
}

void iupScintillaRegisterMarker(Iclass* ic)
{
  iupClassRegisterAttribute(ic,   "MARKERDEFINE", NULL, iScintillaSetMarkerDefineAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "MARKERSYMBOL", iScintillaGetMarkerSymbolAttribId, iScintillaSetMarkerSymbolAttribId, IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "MARKERFGCOLOR", NULL, iScintillaSetMarkerFgColorAttribId, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "MARKERBGCOLOR", NULL, iScintillaSetMarkerBgColorAttribId, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "MARKERBGCOLORSEL", NULL, iScintillaSetMarkerBgColorSelectedAttribId, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "MARKERALPHA", NULL, iScintillaSetMarkerAlphaAttribId, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "MARKERRGBAIMAGE", NULL, iScintillaSetMarkerDefineRGBAImageId, IUPAF_IHANDLENAME|IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic,   "MARKERRGBAIMAGESCALE", NULL, iScintillaSetRGBAImageSetScale, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic,   "MARKERHIGHLIGHT", NULL, iScintillaSetMarkerEnableHighlightAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "MARKERADD", NULL, iScintillaSetMarkerAddAttribId, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "MARKERDELETE", NULL, iScintillaSetMarkerDeleteAttribId, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "MARKERGET", iScintillaGetMarkerGetAttribId, NULL, IUPAF_READONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic,   "MARKERDELETEALL", NULL, iScintillaSetMarkerDeleteAllAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "MARKERNEXT", NULL, iScintillaSetMarkerNextAttribId, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "MARKERPREVIOUS", NULL, iScintillaSetMarkerPreviousAttribId, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "MARKERLINEFROMHANDLE", iScintillaGetMarkerLineFromHandleAttribId, NULL, IUPAF_READONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic,   "MARKERDELETEHANDLE", NULL, iScintillaSetMarkerDeleteHandleAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic,   "LASTMARKERADDHANDLE", NULL, NULL, NULL, NULL, IUPAF_READONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic,   "LASTMARKERFOUND", NULL, NULL, NULL, NULL, IUPAF_READONLY|IUPAF_NO_INHERIT);
}
