/*
 * IupPlot element
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "iupPlot.h"

#include "iup_plot.h"
#include "iupkey.h"
#include "iupgl.h"

#include <cdgl.h>
#include <cdiup.h>
#include <cdprint.h>
#include <cdsvg.h>
#include <cdemf.h>
#include <cdwmf.h>
#include <cdcgm.h>
#include <cdclipbd.h>
#include <cdps.h>

#include "iup_class.h"
#include "iup_register.h"
#include "iup_object.h"
#include "iup_attrib.h"
#include "iup_str.h"
#include "iup_drv.h"
#include "iup_drvfont.h"
#include "iup_stdcontrols.h"
#include "iup_assert.h"
#include "iup_predialogs.h"
#include "iup_linefile.h"

#include "iup_plot_ctrl.h"


static int iPlotSelectFile(Ihandle* parent, char* filename, const char* title, const char* extfilter)
{
  Ihandle* filedlg = IupFileDlg();

  IupSetStrAttribute(filedlg, "DIALOGTYPE", "SAVE");
  IupSetStrAttribute(filedlg, "EXTFILTER", extfilter);
  IupSetStrAttribute(filedlg, "TITLE", title);
  IupSetStrAttribute(filedlg, "FILE", filename);
  IupSetAttributeHandle(filedlg, "PARENTDIALOG", parent);

  IupPopup(filedlg, IUP_CENTERPARENT, IUP_CENTERPARENT);

  if (IupGetInt(filedlg, "STATUS") != -1)
  {
    char* value = IupGetAttribute(filedlg, "VALUE");
    strcpy(filename, value);

    IupDestroy(filedlg);
    return 1;
  }

  IupDestroy(filedlg);
  return 0;
}

static int iPlotCopyAsMetafile_CB(Ihandle* self)
{
  Ihandle* ih = (Ihandle*)IupGetAttribute(self, "PLOT");
  char StrData[100];
  int w, h;
  IupGetIntInt(ih, "DRAWSIZE", &w, &h);
  sprintf(StrData, "%dx%d", w, h);
  cdCanvas* cd_canvas = cdCreateCanvas(CD_CLIPBOARD, StrData);
  IupPlotPaintTo(ih, cd_canvas);
  cdKillCanvas(cd_canvas);
  return IUP_DEFAULT;
}

static int iPlotCopyAsImage_CB(Ihandle* self)
{
  Ihandle* ih = (Ihandle*)IupGetAttribute(self, "PLOT");
  char StrData[100];
  int w, h;
  IupGetIntInt(ih, "DRAWSIZE", &w, &h);
  sprintf(StrData, "%dx%d -b", w, h);
  cdCanvas* cd_canvas = cdCreateCanvas(CD_CLIPBOARD, StrData);
  IupPlotPaintTo(ih, cd_canvas);
  cdKillCanvas(cd_canvas);
  return IUP_DEFAULT;
}

static int iPlotExportEPS_CB(Ihandle* self)
{
  Ihandle* ih = (Ihandle*)IupGetAttribute(self, "PLOT");
  char filename[10240] = "*.eps";
  if (iPlotSelectFile(IupGetDialog(ih), filename, "_@IUP_EXPORT", "Encapsulated Postscript (EPS)|*.eps|All Files|*.*|"))
  {
    char StrData[10240];
    int dpi = IupGetInt(NULL, "SCREENDPI");
    sprintf(StrData, "%s -e -s%d", filename, dpi);
    cdCanvas* cd_canvas = cdCreateCanvas(CD_PS, StrData);
    if (cd_canvas)
    {
      IupPlotPaintTo(ih, cd_canvas);
      cdKillCanvas(cd_canvas);
    }
    else
      iupShowError(IupGetDialog(ih), "IUP_ERRORFILEOPEN");
  }
  return IUP_DEFAULT;
}

static int iPlotExportSVG_CB(Ihandle* self)
{
  Ihandle* ih = (Ihandle*)IupGetAttribute(self, "PLOT");
  char filename[10240] = "*.svg";
  if (iPlotSelectFile(IupGetDialog(ih), filename, "_@IUP_EXPORT", "Scalable Vector Graphics (SVG)|*.svg|All Files|*.*|"))
  {
    char StrData[10240];
    int w, h;
    IupGetIntInt(ih, "DRAWSIZE", &w, &h);
    double res = (double)IupGetInt(NULL, "SCREENDPI") / 25.4;
    double w_mm = ((double)w) / res;
    double h_mm = ((double)h) / res;
    sprintf(StrData, "%s %gx%g %g", filename, w_mm, h_mm, res);
    cdCanvas* cd_canvas = cdCreateCanvas(CD_SVG, StrData);
    if (cd_canvas)
    {
      IupPlotPaintTo(ih, cd_canvas);
      cdKillCanvas(cd_canvas);
    }
    else
      iupShowError(IupGetDialog(ih), "IUP_ERRORFILEOPEN");
  }
  return IUP_DEFAULT;
}

static int iPlotExportCGM_CB(Ihandle* self)
{
  Ihandle* ih = (Ihandle*)IupGetAttribute(self, "PLOT");
  char filename[10240] = "*.cgm";
  if (iPlotSelectFile(IupGetDialog(ih), filename, "_@IUP_EXPORT", "Computer Graphics Metafile (CGM)|*.cgm|All Files|*.*|"))
  {
    char StrData[10240];
    int w, h;
    IupGetIntInt(ih, "DRAWSIZE", &w, &h);
    double res = (double)IupGetInt(NULL, "SCREENDPI") / 25.4;
    double w_mm = ((double)w) / res;
    double h_mm = ((double)h) / res;
    sprintf(StrData, "%s %gx%g %g", filename, w_mm, h_mm, res);
    cdCanvas* cd_canvas = cdCreateCanvas(CD_CGM, StrData);
    if (cd_canvas)
    {
      IupPlotPaintTo(ih, cd_canvas);
      cdKillCanvas(cd_canvas);
    }
    else
      iupShowError(IupGetDialog(ih), "IUP_ERRORFILEOPEN");
  }
  return IUP_DEFAULT;
}

#ifdef WIN32
static int iPlotExportEMF_CB(Ihandle* self)
{
  Ihandle* ih = (Ihandle*)IupGetAttribute(self, "PLOT");
  char filename[10240] = "*.emf";
  if (iPlotSelectFile(IupGetDialog(ih), filename, "_@IUP_EXPORT", "Windows Enhanced Metafile (EMF)|*.emf|All Files|*.*|"))
  {
    char StrData[10240];
    int w, h;
    IupGetIntInt(ih, "DRAWSIZE", &w, &h);
    sprintf(StrData, "%s %dx%d", filename, w, h);
    cdCanvas* cd_canvas = cdCreateCanvas(CD_EMF, StrData);
    if (cd_canvas)
    {
      IupPlotPaintTo(ih, cd_canvas);
      cdKillCanvas(cd_canvas);
    }
    else
      iupShowError(IupGetDialog(ih), "IUP_ERRORFILEOPEN");
  }
  return IUP_DEFAULT;
}

static int iPlotExportWMF_CB(Ihandle* self)
{
  Ihandle* ih = (Ihandle*)IupGetAttribute(self, "PLOT");
  char filename[10240] = "*.wmf";
  if (iPlotSelectFile(IupGetDialog(ih), filename, "_@IUP_EXPORT", "Windows Metafile (WMF)|*.wmf|All Files|*.*|"))
  {
    char StrData[10240];
    int w, h;
    IupGetIntInt(ih, "DRAWSIZE", &w, &h);
    sprintf(StrData, "%s %dx%d", filename, w, h);
    cdCanvas* cd_canvas = cdCreateCanvas(CD_WMF, StrData);
    if (cd_canvas)
    {
      IupPlotPaintTo(ih, cd_canvas);
      cdKillCanvas(cd_canvas);
    }
    else
      iupShowError(IupGetDialog(ih), "IUP_ERRORFILEOPEN");
  }
  return IUP_DEFAULT;
}
#endif

static int iPlotPrint_CB(Ihandle* self)
{
  Ihandle* ih = (Ihandle*)IupGetAttribute(self, "PLOT");
  cdCanvas* cd_canvas = cdCreateCanvas(CD_PRINTER, (void*)"Plot -d");
  IupPlotPaintTo(ih, cd_canvas);
  cdKillCanvas(cd_canvas);
  return IUP_DEFAULT;
}

static int iPlotKeyPress_CB(Ihandle* ih, int c, int press);
static void iPlotRedrawInteract(Ihandle *ih);

static int iPlotZoomIn_CB(Ihandle* self)
{
  Ihandle* ih = (Ihandle*)IupGetAttribute(self, "PLOT");
  return iPlotKeyPress_CB(ih, K_plus, 1);
}

static int iPlotZoomOut_CB(Ihandle* self)
{
  Ihandle* ih = (Ihandle*)IupGetAttribute(self, "PLOT");
  return iPlotKeyPress_CB(ih, K_minus, 1);
}

static int iPlotZoomReset_CB(Ihandle* self)
{
  Ihandle* ih = (Ihandle*)IupGetAttribute(self, "PLOT");
  return iPlotKeyPress_CB(ih, K_period, 1);
}

static int iPlotShowLegend_CB(Ihandle* self)
{
  Ihandle* ih = (Ihandle*)IupGetAttribute(self, "PLOT");
  if (ih->data->current_plot->mLegend.mShow)
    ih->data->current_plot->mLegend.mShow = false;
  else
    ih->data->current_plot->mLegend.mShow = true;

  ih->data->current_plot->mRedraw = true;
  iPlotRedrawInteract(ih);
  return IUP_DEFAULT;
}

static int iPlotShowGrid_CB(Ihandle* self)
{
  Ihandle* ih = (Ihandle*)IupGetAttribute(self, "PLOT");
  if (ih->data->current_plot->mGrid.mShowX || ih->data->current_plot->mGrid.mShowY)
  {
    ih->data->current_plot->mGrid.mShowY = false;
    ih->data->current_plot->mGrid.mShowX = false;
  }
  else
  {
    ih->data->current_plot->mGrid.mShowX = true;
    ih->data->current_plot->mGrid.mShowY = true;
  }

  ih->data->current_plot->mRedraw = true;
  iPlotRedrawInteract(ih);
  return IUP_DEFAULT;
}

static int iPlotGetListIndex(const char** list, const char* value)
{
  int i = 0;
  while (list[i])
  {
    if (iupStrEqualNoCase(list[i], value))
      return i;

    i++;
  }
  return 0;
}

struct iPlotAttribParam
{
  const char* name;
  Icallback check;

  const char* label;
  const char* type;
  const char* extra;
  const char* tip;

  const char** list;
};

static const char* iplot_linestyle_list[] = { "CONTINUOUS", "DASHED", "DOTTED", "DASH_DOT", "DASH_DOT_DOT", NULL };
static const char* iplot_fontstyle_list[] = { "", "BOLD", "ITALIC", "BOLDITALIC", NULL };
static const char* iplot_legendpos_list[] = { "TOPRIGHT", "TOPLEFT", "BOTTOMRIGHT", "BOTTOMLEFT", "BOTTOMCENTER", "XY", NULL };
static const char* iplot_grid_list[] = { "NO", "YES", "HORIZONTAL", "VERTICAL", NULL };
static const char* iplot_scale_list[] = { "LIN", "LOG10", "LOG2", "LOGN", NULL };

static const char* iplot_linestyle_extra = { "|_@IUP_CONTINUOUS|_@IUP_DASHED|_@IUP_DOTTED|_@IUP_DASH_DOT|_@IUP_DASH_DOT_DOT|" };
static const char* iplot_fontstyle_extra = { "|_@IUP_PLAIN|_@IUP_BOLD|_@IUP_ITALIC|_@IUP_BOLDITALIC|" };
static const char* iplot_legendpos_extra = { "|_@IUP_TOPRIGHT|_@IUP_TOPLEFT|_@IUP_BOTTOMRIGHT|_@IUP_BOTTOMLEFT|_@IUP_BOTTOMCENTER|_@IUP_XY|" };
static const char* iplot_grid_extra = { "|_@IUP_NO|_@IUP_YES|_@IUP_HORIZONTAL|_@IUP_VERTICAL|" };
static const char* iplot_scale_extra = { "|_@IUP_LINEAR|_@IUP_LOG10|_@IUP_LOG2|_@IUP_LOGN|" };

static int iPlotCheckBool(Ihandle* param)
{
  return !iupAttribGetBoolean(param, "VALUE");
}

static int iPlotCheckBool2(Ihandle* param)
{
  iupAttribSetInt(param, "CHILDCOUNT", 2);
  return !iupAttribGetBoolean(param, "VALUE");
}

static int iPlotCheckLegendXY(Ihandle* param)
{
  int index = iupAttribGetInt(param, "VALUE");
  return iupStrEqualNoCase(iplot_legendpos_list[index], "XY");
}

static iPlotAttribParam iplot_background_attribs[] = {
  { "", NULL, "_@IUP_MARGIN", "t", NULL, NULL, NULL },
  { "MARGINLEFTAUTO", iPlotCheckBool, "_@IUP_LEFT", "b", "[ ,Auto]", "", NULL },
  { "MARGINLEFT",   NULL, "\t_@IUP_VALUE", "i", "", "", NULL },
  { "MARGINRIGHTAUTO",  iPlotCheckBool, "_@IUP_RIGHT", "b", "", "", NULL },
  { "MARGINRIGHT",  NULL, "\t_@IUP_VALUE", "i", "", "", NULL },
  { "MARGINTOPAUTO",    iPlotCheckBool, "_@IUP_TOP", "b", "[ ,Auto]", "", NULL },
  { "MARGINTOP",    NULL, "\t_@IUP_VALUE", "i", "", "", NULL },
  { "MARGINBOTTOMAUTO", iPlotCheckBool, "_@IUP_BOTTOM", "b", "[ ,Auto]", "", NULL },
  { "MARGINBOTTOM", NULL, "\t_@IUP_VALUE", "i", "", "", NULL },
  { "", NULL, "", "t", NULL, NULL, NULL },
  { "BACKCOLOR", NULL, "_@IUP_COLOR", "c", "", "", NULL },
  { NULL, NULL, NULL, NULL, NULL, NULL, NULL }
};

static iPlotAttribParam iplot_title_attribs[] = {
  { "TITLE", NULL, "_@IUP_TEXT", "s", "", "", NULL },
  { "TITLECOLOR", NULL, "_@IUP_COLOR", "c", "", "", NULL },
  { "TITLEFONTSTYLE", NULL, "_@IUP_FONTSTYLE", "l", iplot_fontstyle_extra, "", iplot_fontstyle_list },
  { "TITLEFONTSIZE", NULL, "_@IUP_FONTSIZE", "i", "[1,,]", "", NULL },
  { "TITLEPOSAUTO", iPlotCheckBool, "_@IUP_POSITION", "b", "[ ,Auto]", "", NULL },
  { "TITLEPOSXY", NULL, "\t_@IUP_POSXY", "s", "/d+[,]/d+", "{(pixels)}", NULL },
  { NULL, NULL, NULL, NULL, NULL, NULL, NULL }
};

static iPlotAttribParam iplot_legend_attribs[] = {
  { "LEGEND", NULL, "_@IUP_SHOW", "b", "", "", NULL },
  { "LEGENDFONTSTYLE", NULL, "_@IUP_FONTSTYLE", "l", iplot_fontstyle_extra, "", iplot_fontstyle_list },
  { "LEGENDFONTSIZE", NULL, "_@IUP_FONTSIZE", "i", "[1,,]", "", NULL },
  { "LEGENDPOS", iPlotCheckLegendXY, "_@IUP_POSITION", "l", iplot_legendpos_extra, "", iplot_legendpos_list },
  { "LEGENDPOSXY", NULL, "\t_@IUP_POSXY", "s", "/d+[,]/d+", "{(pixels)}", NULL },
  { NULL, NULL, NULL, NULL, NULL, NULL, NULL }
};

static iPlotAttribParam iplot_legendbox_attribs[] = {
  { "LEGENDBOX", NULL, "_@IUP_SHOW", "b", "", "", NULL },
  { "LEGENDBOXCOLOR", NULL, "_@IUP_COLOR", "c", "", "", NULL },
  { "LEGENDBOXBACKCOLOR", NULL, "_@IUP_COLOR", "c", "", "", NULL },
  { "LEGENDBOXLINESTYLE", NULL, "_@IUP_LINESTYLE", "l", iplot_linestyle_extra, "", iplot_linestyle_list },
  { "LEGENDBOXLINEWIDTH", NULL, "_@IUP_LINEWIDTH", "i", "[1,,]", "", NULL },
  { NULL, NULL, NULL, NULL, NULL, NULL, NULL }
};

static iPlotAttribParam iplot_grid_attribs[] = {
  { "GRID", NULL, "_@IUP_SHOW", "l", iplot_grid_extra, "", iplot_grid_list },
  { "GRIDCOLOR", NULL, "_@IUP_COLOR", "c", "", "", NULL },
  { "GRIDLINESTYLE", NULL, "_@IUP_LINESTYLE", "l", iplot_linestyle_extra, "", iplot_linestyle_list },
  { "GRIDLINEWIDTH", NULL, "_@IUP_LINEWIDTH", "i", "[1,,]", "", NULL },
  { NULL, NULL, NULL, NULL, NULL, NULL, NULL }
};

static iPlotAttribParam iplot_gridminor_attribs[] = {
  { "GRIDMINOR", NULL, "_@IUP_SHOW", "l", iplot_grid_extra, "", iplot_grid_list },
  { "GRIDMINORCOLOR", NULL, "_@IUP_COLOR", "c", "", "", NULL },
  { "GRIDMINORLINESTYLE", NULL, "_@IUP_LINESTYLE", "l", iplot_linestyle_extra, "", iplot_linestyle_list },
  { "GRIDMINORLINEWIDTH", NULL, "_@IUP_LINEWIDTH", "i", "[1,,]", "", NULL },
  { NULL, NULL, NULL, NULL, NULL, NULL, NULL }
};

static iPlotAttribParam iplot_box_attribs[] = {
  { "BOX", NULL, "_@IUP_SHOW", "b", "", "", NULL },
  { "BOXCOLOR", NULL, "_@IUP_COLOR", "c", "", "", NULL },
  { "BOXLINESTYLE", NULL, "_@IUP_LINESTYLE", "l", iplot_linestyle_extra, "", iplot_linestyle_list },
  { "BOXLINEWIDTH", NULL, "_@IUP_LINEWIDTH", "i", "[1,,]", "", NULL },
  { NULL, NULL, NULL, NULL, NULL, NULL, NULL }
};

static iPlotAttribParam iplot_axisX_attribs[] = {
  { "AXS_X", NULL, "_@IUP_SHOW", "b", "", "", NULL },
  { "AXS_XARROW", NULL, "_@IUP_SHOWARROW", "b", "", "", NULL },
  { "AXS_XCOLOR", NULL, "_@IUP_COLOR", "c", "", "", NULL },
  { "AXS_XLINEWIDTH", NULL, "_@IUP_LINEWIDTH", "i", "[1,,]", "", NULL },
  { "", NULL, "", "t", NULL, NULL, NULL },
  { "AXS_XAUTOMIN", iPlotCheckBool, "_@IUP_MIN", "b", "[ ,Auto]", "", NULL },
  { "AXS_XMIN", NULL, "\t_@IUP_VALUE", "R", "", "", NULL },
  { "AXS_XAUTOMAX", iPlotCheckBool, "_@IUP_MAX", "b", "[ ,Auto]", "", NULL },
  { "AXS_XMAX", NULL, "\t_@IUP_VALUE", "R", "", "", NULL },
  { "AXS_XSCALE", NULL, "_@IUP_SCALE", "l", iplot_scale_extra, "", iplot_scale_list },
  { "AXS_XREVERSE", NULL, "_@IUP_REVERSE", "b", "", "", NULL },
  { "AXS_XCROSSORIGIN", NULL, "_@IUP_CROSSORIGIN", "b", "", "", NULL },
  { NULL, NULL, NULL, NULL, NULL, NULL, NULL }
};

static iPlotAttribParam iplot_axisY_attribs[] = {
  { "AXS_Y", NULL, "_@IUP_SHOW", "b", "", "", NULL },
  { "AXS_YARROW", NULL, "_@IUP_SHOWARROW", "b", "", "", NULL },
  { "AXS_YCOLOR", NULL, "_@IUP_COLOR", "c", "", "", NULL },
  { "AXS_YLINEWIDTH", NULL, "_@IUP_LINEWIDTH", "i", "[1,,]", "", NULL },
  { "", NULL, "", "t", NULL, NULL, NULL },
  { "AXS_YAUTOMIN", iPlotCheckBool, "_@IUP_MIN", "b", "[ ,Auto]", "", NULL },
  { "AXS_YMIN", NULL, "\t_@IUP_VALUE", "R", "", "", NULL },
  { "AXS_YAUTOMAX", iPlotCheckBool, "_@IUP_MAX", "b", "[ ,Auto]", "", NULL },
  { "AXS_YMAX", NULL, "\t_@IUP_VALUE", "R", "", "", NULL },
  { "AXS_YSCALE", NULL, "_@IUP_SCALE", "l", iplot_scale_extra, "", iplot_scale_list },
  { "AXS_YREVERSE", NULL, "_@IUP_REVERSE", "b", "", "", NULL },
  { "AXS_YCROSSORIGIN", NULL, "_@IUP_CROSSORIGIN", "b", "", "", NULL },
  { NULL, NULL, NULL, NULL, NULL, NULL, NULL }
};

static iPlotAttribParam iplot_axisXlabel_attribs[] = {
  { "AXS_XLABEL", NULL, "_@IUP_TEXT", "s", "", "", NULL },
  { "AXS_XLABELCENTERED", NULL, "_@IUP_CENTERED", "b", "", "", NULL },
  { "AXS_XFONTSTYLE", NULL, "_@IUP_FONTSTYLE", "l", iplot_fontstyle_extra, "", iplot_fontstyle_list },
  { "AXS_XFONTSIZE", NULL, "_@IUP_FONTSIZE", "i", "[1,,]", "", NULL },
  { NULL, NULL, NULL, NULL, NULL, NULL, NULL }
};

static iPlotAttribParam iplot_axisYlabel_attribs[] = {
  { "AXS_YLABEL", NULL, "_@IUP_TEXT", "s", "", "", NULL },
  { "AXS_YLABELCENTERED", NULL, "_@IUP_CENTERED", "b", "", "", NULL },
  { "AXS_YFONTSTYLE", NULL, "_@IUP_FONTSTYLE", "l", iplot_fontstyle_extra, "", iplot_fontstyle_list },
  { "AXS_YFONTSIZE", NULL, "_@IUP_FONTSIZE", "i", "[1,,]", "", NULL },
  { NULL, NULL, NULL, NULL, NULL, NULL, NULL }
};

static iPlotAttribParam iplot_axisXticks_attribs[] = {
  { "AXS_XTICK", NULL, "_@IUP_SHOW", "b", "", "", NULL },
  { "AXS_XTICKAUTO", iPlotCheckBool2, "_@IUP_SPACING", "b", "[ ,Auto]", "", NULL },
  { "AXS_XTICKMAJORSPAN",     NULL, "\t_@IUP_MAJORSPAN", "R", "", "", NULL },
  { "AXS_XTICKMINORDIVISION", NULL, "\t_@IUP_MINORDIVISION", "i", "[1,,]", "", NULL },
  { "AXS_XTICKSIZEAUTO", iPlotCheckBool2, "_@IUP_SIZE", "b", "[ ,Auto]", "", NULL },
  { "AXS_XTICKMAJORSIZE", NULL, "\t_@IUP_MAJOR", "i", "[1,,]", "", NULL },
  { "AXS_XTICKMINORSIZE", NULL, "\t_@IUP_MINOR", "i", "[1,,]", "", NULL },
  { NULL, NULL, NULL, NULL, NULL, NULL, NULL }
};

static iPlotAttribParam iplot_axisYticks_attribs[] = {
  { "AXS_YTICK", NULL, "_@IUP_SHOW", "b", "", "", NULL },
  { "AXS_YTICKAUTO", iPlotCheckBool2, "_@IUP_SPACING", "b", "[ ,Auto]", "", NULL },
  { "AXS_YTICKMAJORSPAN",     NULL, "\t_@IUP_MAJORSPAN", "R", "", "", NULL },
  { "AXS_YTICKMINORDIVISION", NULL, "\t_@IUP_MINORDIVISION", "i", "[1,,]", "", NULL },
  { "AXS_YTICKSIZEAUTO", iPlotCheckBool2, "_@IUP_SIZE", "b", "[ ,Auto]", "", NULL },
  { "AXS_YTICKMAJORSIZE", NULL, "\t_@IUP_MAJOR", "i", "[1,,]", "", NULL },
  { "AXS_YTICKMINORSIZE", NULL, "\t_@IUP_MINOR", "i", "[1,,]", "", NULL },
  { NULL, NULL, NULL, NULL, NULL, NULL, NULL }
};

static iPlotAttribParam iplot_axisXticksnumber_attribs[] = {
  { "AXS_XTICKNUMBER", NULL, "_@IUP_SHOW", "b", "", "", NULL },
  { "AXS_XTICKROTATENUMBER", iPlotCheckBool, "_@IUP_ROTATE", "b", "", "", NULL },
  { "AXS_XTICKROTATENUMBERANGLE", NULL, "\t_@IUP_ANGLE", "A", "", "", NULL },
  { "AXS_XTICKFORMATAUTO", iPlotCheckBool, "_@IUP_FORMAT", "b", "[ ,Auto]", "", NULL },
  { "AXS_XTICKFORMATPRECISION",   NULL, "\t_@IUP_DECIMALS", "i", "[0,,]", "", NULL },
  { "AXS_XTICKFONTSTYLE", NULL, "_@IUP_FONTSTYLE", "l", iplot_fontstyle_extra, "", iplot_fontstyle_list },
  { "AXS_XTICKFONTSIZE", NULL, "_@IUP_FONTSIZE", "i", "[1,,]", "", NULL },
  { NULL, NULL, NULL, NULL, NULL, NULL, NULL }
};

static iPlotAttribParam iplot_axisYticksnumber_attribs[] = {
  { "AXS_YTICKNUMBER", NULL, "_@IUP_SHOW", "b", "", "", NULL },
  { "AXS_YTICKROTATENUMBER", iPlotCheckBool, "_@IUP_ROTATE", "b", "", "", NULL },
  { "AXS_YTICKROTATENUMBERANGLE", NULL, "\t_@IUP_ANGLE", "A", "", "", NULL },
  { "AXS_YTICKFORMATAUTO", iPlotCheckBool, "_@IUP_FORMAT", "b", "[ ,Auto]", "", NULL },
  { "AXS_YTICKFORMATPRECISION",   NULL, "\t_@IUP_DECIMALS", "i", "[0,,]", "", NULL },
  { "AXS_YTICKFONTSTYLE", NULL, "_@IUP_FONTSTYLE", "l", iplot_fontstyle_extra, "", iplot_fontstyle_list },
  { "AXS_YTICKFONTSIZE", NULL, "_@IUP_FONTSIZE", "i", "[1,,]", "", NULL },
  { NULL, NULL, NULL, NULL, NULL, NULL, NULL }
};

static void iPlotSetParamDouble(Ihandle* control, const char* name, double num)
{
  char value[80];
  char format[30];
  int prec = IupGetInt(NULL, "DEFAULTPRECISION");
  sprintf(format, "%%.%df", prec);
  iupStrPrintfDoubleLocale(value, format, num, IupGetGlobal("DEFAULTDECIMALSYMBOL"));

  IupStoreAttribute(control, name, value);
}

static void iPlotSetParamValue(Ihandle* param, const char* value)
{
  Ihandle* control = (Ihandle*)IupGetAttribute(param, "CONTROL");
  Ihandle* auxcontrol = (Ihandle*)IupGetAttribute(param, "AUXCONTROL");

  if (value && iupStrEqualNoCase(IupGetAttribute(param, "TYPE"), "LIST"))
  {
    const char** list = (const char**)IupGetAttribute(param, "PLOT_ATTRIBLIST");
    int index = iPlotGetListIndex(list, value);

    IupSetInt(param, "VALUE", index);
    if (control) IupSetInt(control, "VALUE", index + 1);
    // No Aux here
  }
  else
  {
    IupSetStrAttribute(param, "VALUE", value);
    if (control)
    {
      if (iupStrEqualNoCase(iupAttribGet(param, "TYPE"), "REAL"))
      {
        double num = IupGetDouble(param, "VALUE");
        iPlotSetParamDouble(control, "VALUE", num);
      }
      else
        IupSetStrAttribute(control, "VALUE", value);
    }
    if (auxcontrol) IupSetStrAttribute(auxcontrol, "VALUE", value);
  }
}

static const char* iPlotGetParamValue(Ihandle* param)
{
  char* value = IupGetAttribute(param, "VALUE");
  if (!value || value[0] == 0)
    return NULL;  /* reset to default */
  else
  {
    if (iupStrEqualNoCase(IupGetAttribute(param, "TYPE"), "LIST"))
    {
      const char** list = (const char**)IupGetAttribute(param, "PLOT_ATTRIBLIST");
      int index;
      iupStrToInt(value, &index);
      return list[index];
    }

    return value;
  }
}

static void iPlotPropertiesCheckParam(Ihandle* parambox, Ihandle* param, int param_index)
{
  Icallback check = IupGetCallback(param, "PLOT_ATTRIBCHECK");
  if (check)
  {
    int active = check(param);
    int count = iupAttribGetInt(param, "CHILDCOUNT");
    if (count == 0) count = 1;
    while (count)
    {
      param = (Ihandle*)IupGetAttributeId(parambox, "PARAM", param_index + count);

      Ihandle* control = (Ihandle*)IupGetAttribute(param, "CONTROL");
      IupSetInt(IupGetParent(control), "ACTIVE", active);

      count--;
    }
  }
}

static void iPlotPropertiesInit(Ihandle* parambox)
{
  Ihandle* ih = (Ihandle*)IupGetAttribute(parambox, "PLOT");

  int i, count = IupGetInt(parambox, "PARAMCOUNT");
  for (i = 0; i < count; i++)
  {
    Ihandle* param = (Ihandle*)IupGetAttributeId(parambox, "PARAM", i);
    char* name = IupGetAttribute(param, "PLOT_ATTRIB");

    // From Plot
    char* value = IupGetAttribute(ih, name);
    // To Param
    iPlotSetParamValue(param, value);

    iPlotPropertiesCheckParam(parambox, param, i);
  }

  IupSetAttribute(parambox, "PLOT_CHANGED", NULL);
  IupSetAttribute(ih, "REDRAW", NULL);
}

static void iPlotPropertiesResetChanges(Ihandle* parambox)
{
  Ihandle* ih = (Ihandle*)IupGetAttribute(parambox, "PLOT");

  int i, count = IupGetInt(parambox, "PARAMCOUNT");
  for (i = count-1; i >= 0; i--) // backwards to avoid dependencies
  { 
    Ihandle* param = (Ihandle*)IupGetAttributeId(parambox, "PARAM", i);
    char* name = IupGetAttribute(param, "PLOT_ATTRIB");

    // From Original Value
    const char* value = IupGetAttribute(param, "RESET_VALUE");
    // To Param
    iPlotSetParamValue(param, value);

    // From Param
    value = iPlotGetParamValue(param);
    // To Plot
    IupSetStrAttribute(ih, name, value);

    iPlotPropertiesCheckParam(parambox, param, i);
  }

  IupSetAttribute(parambox, "PLOT_CHANGED", NULL);
  IupSetAttribute(ih, "REDRAW", NULL);
}

static void iPlotPropertiesApplyChanges(Ihandle* parambox)
{
  Ihandle* ih = (Ihandle*)IupGetAttribute(parambox, "PLOT");

  int i, count = IupGetInt(parambox, "PARAMCOUNT");
  for (i = count - 1; i >= 0; i--) // backwards to avoid dependencies
  {
    Ihandle* param = (Ihandle*)IupGetAttributeId(parambox, "PARAM", i);
    char* name = IupGetAttribute(param, "PLOT_ATTRIB");

    // From Param
    const char* value = iPlotGetParamValue(param);
    // To Plot
    IupSetAttribute(ih, name, value);

    iPlotPropertiesCheckParam(parambox, param, i);
  }

  IupSetAttribute(parambox, "PLOT_CHANGED", NULL);
  IupSetAttribute(ih, "REDRAW", NULL);
}

static void iPlotPropertiesCheckChanges(Ihandle* parambox)
{
  if (IupGetInt(parambox, "PLOT_CHANGED"))
  {
    Ihandle* dlg = IupMessageDlg();

    IupSetAttributeHandle(dlg, "PARENTDIALOG", IupGetDialog(parambox));

    IupSetAttribute(dlg, "DIALOGTYPE", "WARNING");
    IupSetAttribute(dlg, "BUTTONS", "YESNO");

    IupSetStrAttribute(dlg, "TITLE", "_@IUP_WARNING");
    IupSetStrAttribute(dlg, "VALUE", "_@IUP_CHANGESNOTAPPLIEDAPPLY");

    IupPopup(dlg, IUP_CURRENT, IUP_CURRENT);

    int ret = IupGetInt(dlg, "BUTTONRESPONSE");
    IupDestroy(dlg);

    if (ret == 1)
      iPlotPropertiesApplyChanges(parambox);
    else
      IupSetAttribute(parambox, "PLOT_CHANGED", NULL);
  }
}

static int iPlotPropertiesTreeSelection_CB(Ihandle *ih_tree, int id, int status)
{
  if (status == 0)
  {
    Ihandle* zbox = IupGetBrother(ih_tree);
    Ihandle* parambox = (Ihandle*)IupGetAttribute(zbox, "VALUE_HANDLE");
    iPlotPropertiesCheckChanges(parambox);
  }
  if (status == 1)
  {
    Ihandle* zbox = IupGetBrother(ih_tree);
    IupSetInt(zbox, "VALUEPOS", id);
    Ihandle* parambox = (Ihandle*)IupGetAttribute(zbox, "VALUE_HANDLE");
    iPlotPropertiesInit(parambox);
  }
  return IUP_DEFAULT;
}

static int iPlotPropertiesParam_CB(Ihandle* parambox, int param_index, void*)
{
  if (param_index == IUP_GETPARAM_BUTTON1)
  {
    iPlotPropertiesApplyChanges(parambox);
    return 0;
  }
  if (param_index == IUP_GETPARAM_BUTTON2)
  {
    iPlotPropertiesResetChanges(parambox);
    return 0;
  }
  if (param_index == IUP_GETPARAM_BUTTON3)
  {
    iPlotPropertiesCheckChanges(parambox);

    IupExitLoop();
    return 0;
  }

  Ihandle* param = (Ihandle*)IupGetAttributeId(parambox, "PARAM", param_index);
  iPlotPropertiesCheckParam(parambox, param, param_index);

  IupSetAttribute(parambox, "PLOT_CHANGED", "1");
  return 1;
}

static int iPlotPropertiesClose_CB(Ihandle* dlg)
{
  Ihandle* zbox = IupGetChild(dlg, 1);
  Ihandle* parambox = (Ihandle*)IupGetAttribute(zbox, "VALUE_HANDLE");
  iPlotPropertiesCheckChanges(parambox);
  return IUP_CLOSE;
}

static void iPlotPropertiesAddParamBox(Ihandle* ih, Ihandle* parent, iPlotAttribParam* attribs)
{
  Ihandle* params[50];
  int count = 0;
  char format[10240];

  while (attribs[count].name)
  {
    sprintf(format, "%s%%%s%s%s\n", attribs[count].label, attribs[count].type, attribs[count].extra, attribs[count].tip);
    params[count] = IupParamf(format);

    if (attribs[count].name[0] != 0)
    {
      IupSetStrAttribute(params[count], "PLOT_ATTRIB", attribs[count].name);
      IupSetAttribute(params[count], "PLOT_ATTRIBLIST", (char*)(attribs[count].list));
      IupSetCallback(params[count], "PLOT_ATTRIBCHECK", attribs[count].check);

      // From Plot
      char* value = IupGetAttribute(ih, attribs[count].name);
      // To Param
      iPlotSetParamValue(params[count], value);
      IupSetStrAttribute(params[count], "RESET_VALUE", value);
    }

    count++;
  }

  params[count] = IupParamf("%u[,,_@IUP_CLOSE]");
  count++;

  Ihandle* parambox = IupParamBox(parent, params, count);
  IupSetCallback(parambox, "PARAM_CB", (Icallback)iPlotPropertiesParam_CB);

  count = IupGetInt(parambox, "PARAMCOUNT");
  for (int i = 0; i < count; i++)
  {
    Ihandle* param = (Ihandle*)IupGetAttributeId(parambox, "PARAM", i);
    iPlotPropertiesCheckParam(parambox, param, i);
  }
}

static int iPlotProperties_CB(Ihandle* ih_item)
{
  Ihandle* ih = (Ihandle*)IupGetAttribute(ih_item, "PLOT");
  Ihandle* parent = IupGetDialog(ih);

  Ihandle* tree = IupTree();
  IupSetAttribute(tree, "ADDROOT", "NO");
  IupSetCallback(tree, "SELECTION_CB", (Icallback)iPlotPropertiesTreeSelection_CB);
  IupSetAttribute(tree, "EXPAND", "VERTICAL");
  IupSetAttribute(tree, "SIZE", "100x140");
  IupSetAttribute(tree, "IMAGELEAF", "IMGPAPER");

  Ihandle* zbox = IupZbox(NULL);
  iPlotPropertiesAddParamBox(ih, zbox, iplot_background_attribs);    /* 0 */
  iPlotPropertiesAddParamBox(ih, zbox, iplot_title_attribs);         /* 1 */
  iPlotPropertiesAddParamBox(ih, zbox, iplot_legend_attribs);        /* 2 */
  iPlotPropertiesAddParamBox(ih, zbox, iplot_legendbox_attribs);     /* 3 */
  iPlotPropertiesAddParamBox(ih, zbox, iplot_box_attribs);           /* 4 */
  iPlotPropertiesAddParamBox(ih, zbox, iplot_grid_attribs);          /* 5 */
  iPlotPropertiesAddParamBox(ih, zbox, iplot_gridminor_attribs);     /* 6 */
  iPlotPropertiesAddParamBox(ih, zbox, iplot_axisX_attribs);         /* 7 */
  iPlotPropertiesAddParamBox(ih, zbox, iplot_axisXlabel_attribs);    /* 8 */
  iPlotPropertiesAddParamBox(ih, zbox, iplot_axisXticks_attribs);    /* 9 */
  iPlotPropertiesAddParamBox(ih, zbox, iplot_axisXticksnumber_attribs);  /* 10 */
  iPlotPropertiesAddParamBox(ih, zbox, iplot_axisY_attribs);             /* 11 */
  iPlotPropertiesAddParamBox(ih, zbox, iplot_axisYlabel_attribs);        /* 12 */
  iPlotPropertiesAddParamBox(ih, zbox, iplot_axisYticks_attribs);        /* 13 */
  iPlotPropertiesAddParamBox(ih, zbox, iplot_axisYticksnumber_attribs);  /* 14 */

  IupSetAttribute(zbox, "PLOT", (char*)ih);

  Ihandle* dlg = IupDialog(IupHbox(tree, zbox, NULL));
  IupSetAttributeHandle(dlg, "PARENTDIALOG", parent);
  IupSetStrAttribute(dlg, "TITLE", "_@IUP_PROPERTIESDLG");
  IupSetCallback(dlg, "K_ESC", iPlotPropertiesClose_CB);
  IupSetCallback(dlg, "CLOSE_CB", (Icallback)iPlotPropertiesClose_CB);
  IupSetAttribute(dlg, "MINBOX", "NO");
  IupSetAttribute(dlg, "MAXBOX", "NO");

  if (IupGetAttribute(parent, "ICON"))
    IupSetStrAttribute(dlg, "ICON", IupGetAttribute(parent, "ICON"));
  else
    IupSetStrAttribute(dlg, "ICON", IupGetGlobal("ICON"));

  IupMap(dlg);

  IupSetStrAttribute(tree, "ADDLEAF-1", "_@IUP_BACKGROUND");  /* 0 */
  IupSetStrAttribute(tree, "ADDLEAF0", "_@IUP_TITLE");        /* 1 */
  IupSetStrAttribute(tree, "ADDBRANCH1", "_@IUP_LEGEND");     /* 2 */
    IupSetStrAttribute(tree, "ADDLEAF2", "_@IUP_LEGENDBOX");  /* 3 */
  IupSetStrAttribute(tree, "INSERTLEAF2", "_@IUP_BOX");       /* 4 */
  IupSetStrAttribute(tree, "ADDBRANCH4", "_@IUP_GRID");       /* 5 */
    IupSetStrAttribute(tree, "ADDLEAF5", "_@IUP_GRIDMINOR");  /* 6 */
  IupSetStrAttribute(tree, "INSERTBRANCH5", "_@IUP_XAXIS");   /* 7 */
    IupSetStrAttribute(tree, "ADDLEAF7", "_@IUP_AXISLABEL");  /* 8 */
    IupSetStrAttribute(tree, "ADDLEAF8", "_@IUP_AXISTICKS");  /* 9 */
    IupSetStrAttribute(tree, "ADDLEAF9", "_@IUP_AXISTICKSNUMBER");   /* 10 */
  IupSetStrAttribute(tree, "INSERTBRANCH7", "_@IUP_YAXIS");          /* 11 */
    IupSetStrAttribute(tree, "ADDLEAF11", "_@IUP_AXISLABEL");        /* 12 */
    IupSetStrAttribute(tree, "ADDLEAF12", "_@IUP_AXISTICKS");        /* 13 */
    IupSetStrAttribute(tree, "ADDLEAF13", "_@IUP_AXISTICKSNUMBER");  /* 14 */

  IupPopup(dlg, IUP_CENTERPARENT, IUP_CENTERPARENT);

  IupDestroy(dlg);

  return IUP_DEFAULT;
}

static int iPlotDatasetProperties_CB(Ihandle* ih_item)
{
  Ihandle* ih = (Ihandle*)IupGetAttribute(ih_item, "PLOT");
  char* ds_name = IupGetAttribute(ih_item, "DS_NAME");
  char name[100];
  strcpy(name, ds_name);

  IupSetStrAttribute(ih, "CURRENT", ds_name);

  const char* ds_color = IupGetAttribute(ih, "DS_COLOR");
  char color[30];
  strcpy(color, ds_color);

  const char* ds_mode = IupGetAttribute(ih, "DS_MODE");
  const char* mode_list[] = { "LINE", "MARK", "MARKLINE", "BAR", "AREA", NULL };
  int mode = iPlotGetListIndex(mode_list, ds_mode);

  const char* ds_linestyle = IupGetAttribute(ih, "DS_LINESTYLE");
  int linestyle = iPlotGetListIndex(iplot_linestyle_list, ds_linestyle);

  int linewidth = IupGetInt(ih, "DS_LINEWIDTH");

  const char* ds_markstyle = IupGetAttribute(ih, "DS_MARKSTYLE");
  const char* markstyle_list[] = { "PLUS", "STAR", "CIRCLE", "X", "BOX", "DIAMOND", "HOLLOW_CIRCLE", "HOLLOW_BOX", "HOLLOW_DIAMOND", NULL };
  int markstyle = iPlotGetListIndex(markstyle_list, ds_markstyle);

  int marksize = IupGetInt(ih, "DS_MARKSIZE");

  char format[1024] =
    "_@IUP_NAME%s\n"
    "_@IUP_COLOR%c\n"
    "_@IUP_MODE%l|_@IUP_LINES|_@IUP_MARKS|_@IUP_MARKSLINES|_@IUP_BARS|_@IUP_AREA|\n"
    "_@IUP_LINESTYLE%l|_@IUP_CONTINUOUS|_@IUP_DASHED|_@IUP_DOTTED|_@IUP_DASH_DOT|_@IUP_DASH_DOT_DOT|\n"
    "_@IUP_LINEWIDTH%i[1,,]\n"
    "_@IUP_MARKSTYLE%l|_@IUP_PLUS|_@IUP_STAR|_@IUP_CIRCLE|_@IUP_X|_@IUP_BOX|_@IUP_DIAMOND|_@IUP_HOLLOW_CIRCLE|_@IUP_HOLLOW_BOX|_@IUP_HOLLOW_DIAMOND|\n"
    "_@IUP_MARKSIZE%i[1,,]\n";

  if (!IupGetParam("_@IUP_DATASETPROPERTIESDLG", NULL, NULL, format,
    name, color, &mode, &linestyle, &linewidth, &markstyle, &marksize, NULL))
    return IUP_DEFAULT;

  IupSetStrAttribute(ih, "DS_NAME", name);
  IupSetStrAttribute(ih, "DS_COLOR", color);

  ds_mode = mode_list[mode];
  IupSetStrAttribute(ih, "DS_MODE", ds_mode);

  ds_linestyle = iplot_linestyle_list[linestyle];
  IupSetStrAttribute(ih, "DS_LINESTYLE", ds_linestyle);

  IupSetInt(ih, "DS_LINEWIDTH", linewidth);

  ds_markstyle = markstyle_list[markstyle];
  IupSetStrAttribute(ih, "DS_MARKSTYLE", ds_markstyle);

  IupSetInt(ih, "DS_MARKSIZE", marksize);

  IupSetAttribute(ih, "REDRAW", NULL);

  return IUP_DEFAULT;
}

static Ihandle* iPlotCreateMenuContext(Ihandle* ih, int x, int y)
{
  Ihandle* menu = IupMenu(
    IupSetCallbacks(IupItem("_@IUP_ZOOMINAC", NULL), "ACTION", iPlotZoomIn_CB, NULL),
    IupSetCallbacks(IupItem("_@IUP_ZOOMOUTAC", NULL), "ACTION", iPlotZoomOut_CB, NULL),
    IupSetCallbacks(IupItem("_@IUP_RESETZOOMAC", NULL), "ACTION", iPlotZoomReset_CB, NULL),
    IupSeparator(),
    IupSetCallbacks(IupItem("_@IUP_SHOWHIDELEGEND", NULL), "ACTION", iPlotShowLegend_CB, NULL),
    IupSetCallbacks(IupItem("_@IUP_SHOWHIDEGRID", NULL), "ACTION", iPlotShowGrid_CB, NULL),
    IupSeparator(),
    IupSubmenu("_@IUP_COPY",
      IupMenu(
        IupSetCallbacks(IupItem("Metafile", NULL), "ACTION", iPlotCopyAsMetafile_CB, NULL),
        IupSetCallbacks(IupItem("Bitmap", NULL), "ACTION", iPlotCopyAsImage_CB, NULL),
        NULL)),
    IupSubmenu("_@IUP_EXPORT",
      IupMenu(
        IupSetCallbacks(IupItem("SVG...", NULL), "ACTION", iPlotExportSVG_CB, NULL),
        IupSetCallbacks(IupItem("EPS...", NULL), "ACTION", iPlotExportEPS_CB, NULL),
        IupSetCallbacks(IupItem("CGM...", NULL), "ACTION", iPlotExportCGM_CB, NULL),
#ifdef WIN32
        IupSetCallbacks(IupItem("EMF...", NULL), "ACTION", iPlotExportEMF_CB, NULL),
        IupSetCallbacks(IupItem("WMF...", NULL), "ACTION", iPlotExportWMF_CB, NULL),
#endif
        NULL)),
    IupSeparator(),
    IupSetCallbacks(IupItem("_@IUP_PRINTDLG", NULL), "ACTION", iPlotPrint_CB, NULL),
    NULL);

  if (IupGetInt(ih, "MENUITEMPROPERTIES"))
  {
    Ihandle* item;
    IupAppend(menu, IupSeparator());
    IupAppend(menu, IupSetCallbacks(item = IupItem("_@IUP_DATASETPROPERTIESDLG", NULL), "ACTION", iPlotDatasetProperties_CB, NULL));
    IupAppend(menu, IupSetCallbacks(IupItem("_@IUP_PROPERTIESDLG", NULL), "ACTION", iPlotProperties_CB, NULL));

    int ds, sample;
    double rx, ry;
    const char* ds_name;
    const char* strX;
    if (ih->data->current_plot->FindDataSetSample((double)x, (double)y, ds, ds_name, sample, rx, ry, strX))
    {
      IupSetStrAttribute(item, "DS_NAME", ds_name);
      IupSetAttribute(item, "ACTIVE", "YES");
    }
    else
      IupSetAttribute(item, "ACTIVE", "NO");
  }

  IupSetAttribute(menu, "PLOT", (char*)ih);

  return menu;
}

void iupPlotShowMenuContext(Ihandle* ih, int screen_x, int screen_y, int x, int y)
{
  Ihandle* menu = iPlotCreateMenuContext(ih, x, y);
  IFnnii menucontext_cb;

  menucontext_cb = (IFnnii)IupGetCallback(ih, "MENUCONTEXT_CB");
  if (menucontext_cb)
    menucontext_cb(ih, menu, x, y);

  IupPopup(menu, screen_x, screen_y);

  menucontext_cb = (IFnnii)IupGetCallback(ih, "MENUCONTEXTCLOSE_CB");
  if (menucontext_cb) 
    menucontext_cb(ih, menu, x, y);

  IupDestroy(menu);
}

void iupPlotSetPlotCurrent(Ihandle* ih, int p)
{
  ih->data->current_plot_index = p;
  ih->data->current_plot = ih->data->plot_list[ih->data->current_plot_index];
}

void iupPlotRedraw(Ihandle* ih, int flush, int only_current, int reset_redraw)
{
  cdCanvasActivate(ih->data->cd_canvas);

  if (ih->data->clear)
  {
    only_current = 0;

    long bgcolor;
    iupStrToColor(iupAttribGetStr(ih, "BGCOLOR"), &bgcolor);
    cdCanvasBackground(ih->data->cd_canvas, bgcolor);
    cdCanvasClear(ih->data->cd_canvas);
  }

  if (only_current)
  {
    if (reset_redraw)
      ih->data->current_plot->mRedraw = true;
    ih->data->current_plot->Render(ih->data->cd_canvas);
  }
  else
  {
    int old_current = ih->data->current_plot_index;
    for (int p = 0; p < ih->data->plot_list_count; p++)
    {
      // Set current because of the draw callbacks
      iupPlotSetPlotCurrent(ih, p);

      if (reset_redraw)
        ih->data->current_plot->mRedraw = true;
      ih->data->current_plot->Render(ih->data->cd_canvas);
    }
    iupPlotSetPlotCurrent(ih, old_current);
  }

  // Do the flush once
  if (flush)
  {
    cdCanvasFlush(ih->data->cd_canvas);

    if (ih->data->graphics_mode == IUP_PLOT_OPENGL)
      IupGLSwapBuffers(ih);
  }
}

static int iPlotAction_CB(Ihandle* ih)
{
  // in the redraw callback
  int flush = 1,  // always flush
    only_current = 0,  // redraw all plots
    reset_redraw = 0; // render only if necessary

  if (ih->data->graphics_mode == IUP_PLOT_OPENGL)
  {
    IupGLMakeCurrent(ih);

    // in OpenGL mode must:
    flush = 1;  // always flush
    only_current = 0;  // redraw all plots
    reset_redraw = 1;  // always render
  }

  iupPlotRedraw(ih, flush, only_current, reset_redraw);

  return IUP_DEFAULT;
}

void iupPlotUpdateViewports(Ihandle* ih)
{
  int w, h;

  cdCanvasActivate(ih->data->cd_canvas);
  cdCanvasGetSize(ih->data->cd_canvas, &w, &h, NULL, NULL);

  int numcol = ih->data->numcol;
  if (numcol > ih->data->plot_list_count) numcol = ih->data->plot_list_count;
  int numlin = ih->data->plot_list_count / numcol;
  int pw = w / numcol;
  int ph = h / numlin;

  for(int p=0; p<ih->data->plot_list_count; p++)
  {
    int lin = p / numcol;
    int col = p % numcol;
    int px = col * pw;
    int py = lin * ph;

    ih->data->plot_list[p]->SetViewport(px, py, pw, ph);
  }
}

static int iPlotResize_CB(Ihandle* ih, int width, int height)
{
  if (ih->data->graphics_mode == IUP_PLOT_OPENGL)
  {
    IupGLMakeCurrent(ih);

    double res = IupGetDouble(NULL, "SCREENDPI") / 25.4;
    cdCanvasSetfAttribute(ih->data->cd_canvas, "SIZE", "%dx%d %g", width, height, res);
  }

  iupPlotUpdateViewports(ih);
  return IUP_DEFAULT;
}

static void iPlotRedrawInteract(Ihandle *ih)
{
  // when interacting
  int flush = 0, // flush if necessary
    only_current = 1, // draw all plots if sync
    reset_redraw = 0;  // render only if necessary

  if (ih->data->graphics_mode == IUP_PLOT_OPENGL)
  {
    // in OpenGL mode must:
    flush = 1;  // always flush
    only_current = 0;  // redraw all plots
    reset_redraw = 1;  // always render
  }
  else
  {
    if (ih->data->current_plot->mRedraw)
      flush = 1;

    if (ih->data->sync_view)
      only_current = 0;
  }

  iupPlotRedraw(ih, flush, only_current, reset_redraw);
}

void iupPlotResetZoom(Ihandle *ih, int redraw)
{
  ih->data->current_plot->ResetZoom();

  if (ih->data->sync_view)
  {
    for (int p = 0; p<ih->data->plot_list_count; p++)
    {
      if (ih->data->plot_list[p] != ih->data->current_plot)
        ih->data->plot_list[p]->ResetZoom();
    }
  }

  if (redraw)
    iPlotRedrawInteract(ih);
}

static void iPlotPanStart(Ihandle *ih)
{
  ih->data->current_plot->PanStart();

  if (ih->data->sync_view)
  {
    for (int p = 0; p<ih->data->plot_list_count; p++)
    {
      if (ih->data->plot_list[p] != ih->data->current_plot)
        ih->data->plot_list[p]->PanStart();
    }
  }
}

static void iPlotPan(Ihandle *ih, int x1, int y1, int x2, int y2)
{
  double rx1, ry1, rx2, ry2;
  ih->data->current_plot->TransformBack(x1, y1, rx1, ry1);
  ih->data->current_plot->TransformBack(x2, y2, rx2, ry2);

  double offsetX = rx2 - rx1;
  double offsetY = ry2 - ry1;

  ih->data->current_plot->Pan(offsetX, offsetY);

  if (ih->data->sync_view)
  {
    for (int p = 0; p<ih->data->plot_list_count; p++)
    {
      if (ih->data->plot_list[p] != ih->data->current_plot)
      {
        ih->data->plot_list[p]->TransformBack(x1, y1, rx1, ry1);
        ih->data->plot_list[p]->TransformBack(x2, y2, rx2, ry2);

        offsetX = rx2 - rx1;
        offsetY = ry2 - ry1;

        ih->data->plot_list[p]->Pan(offsetX, offsetY);
      }
    }
  }

  iPlotRedrawInteract(ih);
}

static void iPlotZoom(Ihandle *ih, int x, int y, float delta)
{
  double rx, ry;
  ih->data->current_plot->TransformBack(x, y, rx, ry);

  if (delta > 0)
    ih->data->current_plot->ZoomIn(rx, ry);
  else
    ih->data->current_plot->ZoomOut(rx, ry);

  if (ih->data->sync_view)
  {
    for (int p = 0; p<ih->data->plot_list_count; p++)
    {
      if (ih->data->plot_list[p] != ih->data->current_plot)
      {
        ih->data->plot_list[p]->TransformBack(x, y, rx, ry);

        if (delta > 0)
          ih->data->plot_list[p]->ZoomIn(rx, ry);
        else
          ih->data->plot_list[p]->ZoomOut(rx, ry);
      }
    }
  }

  iPlotRedrawInteract(ih);
}

static void iPlotZoomTo(Ihandle *ih, int x1, int y1, int x2, int y2)
{
  double rx1, ry1, rx2, ry2;
  ih->data->current_plot->TransformBack(x1, y1, rx1, ry1);
  ih->data->current_plot->TransformBack(x2, y2, rx2, ry2);

  ih->data->current_plot->ZoomTo(rx1, rx2, ry1, ry2);

  if (ih->data->sync_view)
  {
    for (int p = 0; p<ih->data->plot_list_count; p++)
    {
      if (ih->data->plot_list[p] != ih->data->current_plot)
      {
        ih->data->plot_list[p]->TransformBack(x1, y1, rx1, ry1);
        ih->data->plot_list[p]->TransformBack(x2, y2, rx2, ry2);

        ih->data->plot_list[p]->ZoomTo(rx1, rx2, ry1, ry2);
      }
    }
  }

  iPlotRedrawInteract(ih);
}

static void iPlotScroll(Ihandle *ih, float delta, bool full_page, bool vertical)
{
  ih->data->current_plot->Scroll(delta, full_page, vertical);

  if (ih->data->sync_view)
  {
    for (int p = 0; p<ih->data->plot_list_count; p++)
    {
      if (ih->data->plot_list[p] != ih->data->current_plot)
        ih->data->plot_list[p]->Scroll(delta, full_page, vertical);
    }
  }

  iPlotRedrawInteract(ih);
}

static void iPlotScrollTo(Ihandle *ih, int x, int y)
{
  double rx, ry;
  ih->data->current_plot->TransformBack(x, y, rx, ry);

  ih->data->current_plot->ScrollTo(rx, ry);

  if (ih->data->sync_view)
  {
    for (int p = 0; p<ih->data->plot_list_count; p++)
    {
      if (ih->data->plot_list[p] != ih->data->current_plot)
      {
        ih->data->plot_list[p]->TransformBack(x, y, rx, ry);
        ih->data->plot_list[p]->ScrollTo(rx, ry);
      }
    }
  }

  iPlotRedrawInteract(ih);
}

static int iPlotFindPlot(Ihandle* ih, int x, int &y)
{
  int w, h;

  if (ih->data->graphics_mode == IUP_PLOT_OPENGL)
    IupGLMakeCurrent(ih);

  cdCanvasActivate(ih->data->cd_canvas);
  cdCanvasGetSize(ih->data->cd_canvas, &w, &h, NULL, NULL);

  y = cdCanvasInvertYAxis(ih->data->cd_canvas, y);

  int numcol = ih->data->numcol;
  if (numcol > ih->data->plot_list_count) numcol = ih->data->plot_list_count;
  int numlin = ih->data->plot_list_count / numcol;
  int pw = w / numcol;
  int ph = h / numlin;

  int lin = y / ph;
  int col = x / pw;

  int index = lin * numcol + col;
  if (index < ih->data->plot_list_count)
    return index;

  return -1;
}

static int iPlotButton_CB(Ihandle* ih, int button, int press, int x, int y, char* status)
{
  int screen_x = x, screen_y = y;
  int index = iPlotFindPlot(ih, x, y);
  if (index < 0)
  {
    iPlotRedrawInteract(ih);
    return IUP_DEFAULT;
  }

  iupPlotSetPlotCurrent(ih, index);

  if (ih->data->current_plot->mDataSetListCount == 0)
    return IUP_DEFAULT;

  x -= ih->data->current_plot->mViewport.mX;
  y -= ih->data->current_plot->mViewport.mY;

  IFniidds cb = (IFniidds)IupGetCallback(ih, "PLOTBUTTON_CB");
  if (cb) 
  {
    cdCanvasOrigin(ih->data->cd_canvas, ih->data->current_plot->mViewport.mX, ih->data->current_plot->mViewport.mY);

    double rx, ry;
    ih->data->current_plot->TransformBack(x, y, rx, ry);
    if (cb(ih, button, press, rx, ry, status) == IUP_IGNORE)
      return IUP_DEFAULT;
  }

  ih->data->last_pos_moving = 0;

  if (press)
  {
    ih->data->last_click_x = x;
    ih->data->last_click_y = y;
    ih->data->last_click_plot = index;

    if (button == IUP_BUTTON1)
    {
      if (!iup_iscontrol(status))
      {
        if (iup_isdouble(status))
          iupPlotResetZoom(ih, 1);
        else
        {
          if (!ih->data->current_plot->mTitle.mAutoPos && 
              ih->data->current_plot->CheckInsideTitle(ih->data->cd_canvas, x, y))
          {
            ih->data->last_pos_x = ih->data->current_plot->mTitle.mPosX;
            ih->data->last_pos_y = ih->data->current_plot->mTitle.mPosY;
            ih->data->last_pos_moving = 1;
          }
          else if (ih->data->current_plot->mLegend.mPosition == IUP_PLOT_XY &&
                   ih->data->current_plot->CheckInsideLegend(ih->data->cd_canvas, x, y))
          {
            ih->data->last_pos_x = ih->data->current_plot->mLegend.mPos.mX;
            ih->data->last_pos_y = ih->data->current_plot->mLegend.mPos.mY;
            ih->data->last_pos_moving = 2;
          }
          else
            iPlotPanStart(ih);
        }
      }
    }
    else if (button == IUP_BUTTON2)
    {
      if (!iup_iscontrol(status))
        iPlotScrollTo(ih, x, y);
    }
    else if (button == IUP_BUTTON3 && !iup_iscontrol(status) && !iup_isshift(status) && IupGetInt(ih, "MENUCONTEXT"))
    {
      int sx, sy;
      IupGetIntInt(ih, "SCREENPOSITION", &sx, &sy);

      screen_x += sx;
      screen_y += sy;

      iupPlotShowMenuContext(ih, screen_x, screen_y, x, y);
    }
  }
  else
  {
    if (iup_iscontrol(status))
    {
      if (ih->data->last_click_x == x && ih->data->last_click_y == y && ih->data->last_click_plot == index)
      {
        float delta = 0;
        if (button == IUP_BUTTON1)
          delta = 1.0;
        else if (button == IUP_BUTTON3)
          delta = -1.0;

        if (delta)
          iPlotZoom(ih, x, y, delta);
      }
      else if (button == IUP_BUTTON1 && ih->data->last_click_x != x && ih->data->last_click_y != y && ih->data->last_click_plot == index)
      {
        iPlotZoomTo(ih, ih->data->last_click_x, ih->data->last_click_y, x, y);
      }
    }
    else if (button == IUP_BUTTON1 && iup_isshift(status))
    {
      double rx1, ry1, rx2, ry2;
      ih->data->current_plot->TransformBack(ih->data->last_click_x, ih->data->last_click_y, rx1, ry1);
      ih->data->current_plot->TransformBack(x, y, rx2, ry2);

      ih->data->current_plot->SelectDataSetSamples(rx1, rx2, ry1, ry2);

      iPlotRedrawInteract(ih);
    }

    if (!iup_iscontrol(status) && ih->data->last_click_x == x && ih->data->last_click_y == y && ih->data->last_click_plot == index)
    {
      IFniiddi clicksample_cb = (IFniiddi)IupGetCallback(ih, "CLICKSAMPLE_CB");
      if (clicksample_cb)
      {
        int ds, sample;
        double rx, ry;
        const char* ds_name;
        const char* strX;
        if (ih->data->current_plot->FindDataSetSample((double)x, (double)y, ds, ds_name, sample, rx, ry, strX))
          clicksample_cb(ih, ds, sample, rx, ry, button);
      }
    }
  }

  return IUP_DEFAULT;
}

static int iPlotMotion_CB(Ihandle* ih, int x, int y, char *status)
{
  if (iupStrEqualNoCase(IupGetAttribute(ih, "CURSOR"), "HAND"))
    IupSetAttribute(ih, "CURSOR", NULL);

  int index = iPlotFindPlot(ih, x, y);
  if (index<0)
    return IUP_DEFAULT;

  iupPlotSetPlotCurrent(ih, index);

  if (ih->data->current_plot->mDataSetListCount == 0)
    return IUP_DEFAULT;

  x -= ih->data->current_plot->mViewport.mX;
  y -= ih->data->current_plot->mViewport.mY;

  IFndds cb = (IFndds)IupGetCallback(ih, "PLOTMOTION_CB");
  if (cb) 
  {
    cdCanvasOrigin(ih->data->cd_canvas, ih->data->current_plot->mViewport.mX, ih->data->current_plot->mViewport.mY);

    double rx, ry;
    ih->data->current_plot->TransformBack(x, y, rx, ry);
    if (cb(ih, rx, ry, status) == IUP_IGNORE)
      return IUP_DEFAULT;
  }

  if (!ih->data->current_plot->mTitle.mAutoPos &&
      ih->data->current_plot->CheckInsideTitle(ih->data->cd_canvas, x, y))
    IupSetAttribute(ih, "CURSOR", "HAND");
  else if (ih->data->current_plot->mLegend.mPosition == IUP_PLOT_XY &&
           ih->data->current_plot->CheckInsideLegend(ih->data->cd_canvas, x, y))
    IupSetAttribute(ih, "CURSOR", "HAND");

  if (iup_isbutton1(status) && ih->data->last_click_plot == index)
  {
    if (iup_iscontrol(status) || iup_isshift(status))
    {
      ih->data->current_plot->mRedraw = true;
      ih->data->current_plot->mShowSelectionBand = true;
      ih->data->current_plot->mSelectionBand.mX = ih->data->last_click_x < x? ih->data->last_click_x: x;
      ih->data->current_plot->mSelectionBand.mY = ih->data->last_click_y < y? ih->data->last_click_y: y;
      ih->data->current_plot->mSelectionBand.mWidth = abs(ih->data->last_click_x - x) + 1;
      ih->data->current_plot->mSelectionBand.mHeight = abs(ih->data->last_click_y - y) + 1;

      iPlotRedrawInteract(ih);

      ih->data->current_plot->mShowSelectionBand = false;
      return IUP_DEFAULT;
    }
    else
    {
      if (ih->data->last_click_x != x || ih->data->last_click_y != y)
      {
        if (!ih->data->current_plot->mTitle.mAutoPos && ih->data->last_pos_moving == 1)
        {
          ih->data->current_plot->mTitle.mPosX = ih->data->last_pos_x + (x - ih->data->last_click_x);
          ih->data->current_plot->mTitle.mPosY = ih->data->last_pos_y - (y - ih->data->last_click_y);
          ih->data->current_plot->mRedraw = true;

          iPlotRedrawInteract(ih);
          return IUP_DEFAULT;
        }

        if (ih->data->current_plot->mLegend.mPosition == IUP_PLOT_XY && ih->data->last_pos_moving == 2)
        {
          ih->data->current_plot->mLegend.mPos.mX = ih->data->last_pos_x + (x - ih->data->last_click_x);
          ih->data->current_plot->mLegend.mPos.mY = ih->data->last_pos_y - (y - ih->data->last_click_y);
          ih->data->current_plot->mRedraw = true;

          iPlotRedrawInteract(ih);
          return IUP_DEFAULT;
        }

        iPlotPan(ih, ih->data->last_click_x, ih->data->last_click_y, x, y);
        return IUP_DEFAULT;
      }
    }
  }

  int ds, sample;
  double rx, ry;
  const char* ds_name;
  const char* strX;
  if (ih->data->current_plot->FindDataSetSample((double)x, (double)y, ds, ds_name, sample, rx, ry, strX))
  {
    if (ih->data->last_tip_ds != ds && ih->data->last_tip_sample != sample)
    {
      char* tipformat = iupAttribGetStr(ih, "TIPFORMAT");

      char str_Y[100];
      iupStrPrintfDoubleLocale(str_Y, ih->data->current_plot->mAxisY.mTipFormatString, ry, IupGetGlobal("DEFAULTDECIMALSYMBOL"));

      if (strX)
      {
        IupSetfAttribute(ih, "TIP", tipformat, ds_name, strX, str_Y);
      }
      else
      {
        char str_X[100];
        iupStrPrintfDoubleLocale(str_X, ih->data->current_plot->mAxisX.mTipFormatString, rx, IupGetGlobal("DEFAULTDECIMALSYMBOL"));

        IupSetfAttribute(ih, "TIP", tipformat, ds_name, str_X, str_Y);
      }

      IupSetAttribute(ih, "TIPVISIBLE", "Yes");

      ih->data->last_tip_ds = ds;
      ih->data->last_tip_sample = sample;
    }
  }
  else
  {
    if (ih->data->last_tip_ds != -1 && ih->data->last_tip_sample != -1)
    {
      ih->data->last_tip_ds = -1;
      ih->data->last_tip_sample = -1;
      IupSetAttribute(ih, "TIP", NULL);
      IupSetAttribute(ih, "TIPVISIBLE", "Yes");
    }
  }

  if (ih->data->show_cross_hair)
  {
    ih->data->current_plot->mRedraw = true;
    if (ih->data->show_cross_hair == IUP_PLOT_CROSSHORIZ)
    {
      ih->data->current_plot->mCrossHairH = true;
      ih->data->current_plot->mCrossHairX = x;
    }
    else
    {
      ih->data->current_plot->mCrossHairV = true;
      ih->data->current_plot->mCrossHairY = y;
    }

    iPlotRedrawInteract(ih);
  }

  return IUP_DEFAULT;
}

static int iPlotWheel_CB(Ihandle *ih, float delta, int x, int y, char* status)
{
  int index = iPlotFindPlot(ih, x, y);
  if (index<0)
    return IUP_DEFAULT;

  iupPlotSetPlotCurrent(ih, index);

  if (ih->data->current_plot->mDataSetListCount == 0)
    return IUP_DEFAULT;

  x -= ih->data->current_plot->mViewport.mX;
  y -= ih->data->current_plot->mViewport.mY;

  if (iup_iscontrol(status))
    iPlotZoom(ih, x, y, delta);
  else 
  {
    bool vertical = true;
    if (iup_isshift(status))
      vertical = false;

    iPlotScroll(ih, delta, false, vertical);
  }

  return IUP_DEFAULT;
}

static int iPlotKeyPress_CB(Ihandle* ih, int c, int press)
{
  if (!press)
    return IUP_DEFAULT;

  if (ih->data->graphics_mode == IUP_PLOT_OPENGL)
    IupGLMakeCurrent(ih);

  if (c == K_cH || c == K_cV)
  {
    int CH = IUP_PLOT_CROSSHORIZ;
    if (c == K_cV) CH = IUP_PLOT_CROSSVERT;

    if (ih->data->show_cross_hair == CH)
      ih->data->show_cross_hair = IUP_PLOT_CROSSNONE;
    else
      ih->data->show_cross_hair = CH;

    for (int p = 0; p < ih->data->plot_list_count; p++)
    {
      if (ih->data->plot_list[p]->mCrossHairH)
      {
        ih->data->plot_list[p]->mRedraw = true;
        ih->data->plot_list[p]->mCrossHairH = false;
      }
      if (ih->data->plot_list[p]->mCrossHairV)
      {
        ih->data->plot_list[p]->mRedraw = true;
        ih->data->plot_list[p]->mCrossHairV = false;
      }
    }

    if (ih->data->show_cross_hair != IUP_PLOT_CROSSNONE)  // was shown
      iPlotRedrawInteract(ih);

    return IUP_DEFAULT;
  }

  if (ih->data->current_plot->mDataSetListCount == 0)
    return IUP_DEFAULT;

  if (c == K_plus)
  {
    int x = ih->data->current_plot->mViewport.mX + ih->data->current_plot->mViewport.mWidth/2;
    int y = ih->data->current_plot->mViewport.mY + ih->data->current_plot->mViewport.mHeight/2;
    iPlotZoom(ih, x, y, 1);
  }
  else if (c == K_minus)
  {
    int x = ih->data->current_plot->mViewport.mX + ih->data->current_plot->mViewport.mWidth / 2;
    int y = ih->data->current_plot->mViewport.mY + ih->data->current_plot->mViewport.mHeight / 2;
    iPlotZoom(ih, x, y, -1);
  }
  else if (c == K_period)
  {
    iupPlotResetZoom(ih, 1);
  }
  else if (c == K_LEFT || c == K_RIGHT)
  {
    float delta = 1.0f;
    if (c == K_LEFT) delta = -1.0f;
    iPlotScroll(ih, delta, false, false);
  }
  else if (c == K_UP || c == K_DOWN || c == K_PGUP || c == K_PGDN)
  {
    float delta = 1.0f;
    if (c == K_DOWN || c == K_PGDN) delta = -1.0f;

    bool full_page = false;
    if (c == K_PGUP || c == K_PGDN) full_page = true;

    iPlotScroll(ih, delta, full_page, true);
  }
  else if (c == K_DEL)
  {
    if (!ih->data->read_only)
    {
      ih->data->current_plot->DeleteSelectedDataSetSamples();
      iPlotRedrawInteract(ih);
    }
  }
  else if (c == K_ESC)
  {
    ih->data->current_plot->ClearDataSetSelection();
    iPlotRedrawInteract(ih);
  }

  return IUP_DEFAULT;
} 


/************************************************************************************/


void IupPlotBegin(Ihandle* ih, int strXdata)
{
  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return;

  if (ih->iclass->nativetype != IUP_TYPECANVAS || 
      !IupClassMatch(ih, "plot"))
    return;

  iupPlotDataSet* theDataSet = (iupPlotDataSet*)iupAttribGet(ih, "_IUP_PLOT_DATASET");
  if (theDataSet) 
    delete theDataSet;

  theDataSet = new iupPlotDataSet(strXdata? true: false);
  iupAttribSet(ih, "_IUP_PLOT_DATASET", (char*)theDataSet);
}

void IupPlotAdd(Ihandle* ih, double x, double y)
{
  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return;

  if (ih->iclass->nativetype != IUP_TYPECANVAS || 
      !IupClassMatch(ih, "plot"))
    return;

  iupPlotDataSet* theDataSet = (iupPlotDataSet*)iupAttribGet(ih, "_IUP_PLOT_DATASET");
  theDataSet->AddSample(x, y);
}

void IupPlotAddStr(Ihandle* ih, const char* x, double y)
{
  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return;

  if (ih->iclass->nativetype != IUP_TYPECANVAS ||
      !IupClassMatch(ih, "plot"))
      return;

  iupPlotDataSet* theDataSet = (iupPlotDataSet*)iupAttribGet(ih, "_IUP_PLOT_DATASET");
  theDataSet->AddSample(x, y);
}

void IupPlotAddSegment(Ihandle* ih, double x, double y)
{
  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return;

  if (ih->iclass->nativetype != IUP_TYPECANVAS ||
      !IupClassMatch(ih, "plot"))
      return;

  iupPlotDataSet* theDataSet = (iupPlotDataSet*)iupAttribGet(ih, "_IUP_PLOT_DATASET");
  theDataSet->AddSampleSegment(x, y, true);
}

int IupPlotEnd(Ihandle* ih)
{
  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return -1;

  if (ih->iclass->nativetype != IUP_TYPECANVAS ||
      !IupClassMatch(ih, "plot"))
      return -1;

  iupPlotDataSet* theDataSet = (iupPlotDataSet*)iupAttribGet(ih, "_IUP_PLOT_DATASET");
  if (!theDataSet)
    return -1;

  ih->data->current_plot->AddDataSet(theDataSet);

  iupAttribSet(ih, "_IUP_PLOT_DATASET", NULL);

  ih->data->current_plot->mRedraw = true;
  return ih->data->current_plot->mCurrentDataSet;
}

void IupPlotInsert(Ihandle* ih, int inIndex, int inSampleIndex, double inX, double inY)
{
  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return;

  if (ih->iclass->nativetype != IUP_TYPECANVAS ||
      !IupClassMatch(ih, "plot"))
      return;

  if (inIndex < 0 || inIndex >= ih->data->current_plot->mDataSetListCount)
    return;

  iupPlotDataSet* theDataSet = ih->data->current_plot->mDataSetList[inIndex];
  theDataSet->InsertSample(inSampleIndex, inX, inY);
}

void IupPlotInsertStr(Ihandle* ih, int inIndex, int inSampleIndex, const char* inX, double inY)
{
  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return;

  if (ih->iclass->nativetype != IUP_TYPECANVAS || 
      !IupClassMatch(ih, "plot"))
    return;

  if (inIndex < 0 || inIndex >= ih->data->current_plot->mDataSetListCount)
    return;

  iupPlotDataSet* theDataSet = ih->data->current_plot->mDataSetList[inIndex];
  theDataSet->InsertSample(inSampleIndex, inX, inY);
}

void IupPlotInsertSegment(Ihandle* ih, int inIndex, int inSampleIndex, double inX, double inY)
{
  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return;

  if (ih->iclass->nativetype != IUP_TYPECANVAS ||
      !IupClassMatch(ih, "plot"))
      return;

  if (inIndex < 0 || inIndex >= ih->data->current_plot->mDataSetListCount)
    return;

  iupPlotDataSet* theDataSet = ih->data->current_plot->mDataSetList[inIndex];
  theDataSet->InsertSampleSegment(inSampleIndex, inX, inY, true);
}

void IupPlotAddSamples(Ihandle* ih, int inIndex, double *x, double *y, int count)
{
  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return;

  if (ih->iclass->nativetype != IUP_TYPECANVAS || 
      !IupClassMatch(ih, "plot"))
    return;

  if (inIndex < 0 || inIndex >= ih->data->current_plot->mDataSetListCount)
    return;

  iupPlotDataSet* theDataSet = ih->data->current_plot->mDataSetList[inIndex];
  for (int i=0; i<count; i++)
    theDataSet->AddSample(x[i], y[i]);
}

void IupPlotAddStrSamples(Ihandle* ih, int inIndex, const char** x, double* y, int count)
{
  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return;

  if (ih->iclass->nativetype != IUP_TYPECANVAS || 
      !IupClassMatch(ih, "plot"))
    return;

  if (inIndex < 0 || inIndex >= ih->data->current_plot->mDataSetListCount)
    return;

  iupPlotDataSet* theDataSet = ih->data->current_plot->mDataSetList[inIndex];
  for (int i = 0; i<count; i++)
    theDataSet->AddSample(x[i], y[i]);
}

void IupPlotInsertStrSamples(Ihandle* ih, int inIndex, int inSampleIndex, const char** inX, double* inY, int count)
{
  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return;

  if (ih->iclass->nativetype != IUP_TYPECANVAS || 
      !IupClassMatch(ih, "plot"))
    return;

  if (inIndex < 0 || inIndex >= ih->data->current_plot->mDataSetListCount)
    return;

  iupPlotDataSet* theDataSet = ih->data->current_plot->mDataSetList[inIndex];

  for (int i=0; i<count; i++)
    theDataSet->InsertSample(inSampleIndex + i, inX[i], inY[i]);
}

void IupPlotInsertSamples(Ihandle* ih, int inIndex, int inSampleIndex, double *inX, double *inY, int count)
{
  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return;

  if (ih->iclass->nativetype != IUP_TYPECANVAS || 
      !IupClassMatch(ih, "plot"))
    return;

  if (inIndex < 0 || inIndex >= ih->data->current_plot->mDataSetListCount)
    return;

  iupPlotDataSet* theDataSet = ih->data->current_plot->mDataSetList[inIndex];

  for (int i=0; i<count; i++)
    theDataSet->InsertSample(inSampleIndex + i, inX[i], inY[i]);
}

void IupPlotGetSample(Ihandle* ih, int inIndex, int inSampleIndex, double *x, double *y)
{
  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return;

  if (ih->iclass->nativetype != IUP_TYPECANVAS ||
      !IupClassMatch(ih, "plot"))
      return;

  if (inIndex < 0 || inIndex >= ih->data->current_plot->mDataSetListCount)
    return;

  iupPlotDataSet* theDataSet = ih->data->current_plot->mDataSetList[inIndex];
  theDataSet->GetSample(inSampleIndex, x, y);
}

void IupPlotGetSampleStr(Ihandle* ih, int inIndex, int inSampleIndex, const char* *x, double *y)
{
  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return;

  if (ih->iclass->nativetype != IUP_TYPECANVAS ||
      !IupClassMatch(ih, "plot"))
      return;

  if (inIndex < 0 || inIndex >= ih->data->current_plot->mDataSetListCount)
    return;

  iupPlotDataSet* theDataSet = ih->data->current_plot->mDataSetList[inIndex];
  theDataSet->GetSample(inSampleIndex, x, y);
}

int IupPlotGetSampleSelection(Ihandle* ih, int inIndex, int inSampleIndex)
{
  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return -1;

  if (ih->iclass->nativetype != IUP_TYPECANVAS ||
      !IupClassMatch(ih, "plot"))
      return -1;

  if (inIndex < 0 || inIndex >= ih->data->current_plot->mDataSetListCount)
    return -1;

  iupPlotDataSet* theDataSet = ih->data->current_plot->mDataSetList[inIndex];

  int theCount = theDataSet->GetCount();
  if (inSampleIndex < 0 || inSampleIndex >= theCount)
    return -1;

  return theDataSet->GetSampleSelection(inSampleIndex);
}

void IupPlotSetSample(Ihandle* ih, int inIndex, int inSampleIndex, double x, double y)
{
  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return;

  if (ih->iclass->nativetype != IUP_TYPECANVAS ||
      !IupClassMatch(ih, "plot"))
      return;

  if (inIndex < 0 || inIndex >= ih->data->current_plot->mDataSetListCount)
    return;

  iupPlotDataSet* theDataSet = ih->data->current_plot->mDataSetList[inIndex];
  theDataSet->SetSample(inSampleIndex, x, y);
}

void IupPlotSetSampleStr(Ihandle* ih, int inIndex, int inSampleIndex, const char* x, double y)
{
  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return;

  if (ih->iclass->nativetype != IUP_TYPECANVAS ||
      !IupClassMatch(ih, "plot"))
      return;

  if (inIndex < 0 || inIndex >= ih->data->current_plot->mDataSetListCount)
    return;

  iupPlotDataSet* theDataSet = ih->data->current_plot->mDataSetList[inIndex];
  theDataSet->SetSample(inSampleIndex, x, y);
}

void IupPlotSetSampleSelection(Ihandle* ih, int inIndex, int inSampleIndex, int inSelected)
{
  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return;

  if (ih->iclass->nativetype != IUP_TYPECANVAS ||
      !IupClassMatch(ih, "plot"))
      return;

  if (inIndex < 0 || inIndex >= ih->data->current_plot->mDataSetListCount)
    return;

  iupPlotDataSet* theDataSet = ih->data->current_plot->mDataSetList[inIndex];

  int theCount = theDataSet->GetCount();
  if (inSampleIndex < 0 || inSampleIndex >= theCount)
    return;

  return theDataSet->SetSampleSelection(inSampleIndex, inSelected ? true : false);
}

void IupPlotTransform(Ihandle* ih, double x, double y, double *cnv_x, double *cnv_y)
{
  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return;

  if (ih->iclass->nativetype != IUP_TYPECANVAS || 
      !IupClassMatch(ih, "plot"))
    return;

  if (cnv_x) *cnv_x = ih->data->current_plot->mAxisX.mTrafo->Transform(x);
  if (cnv_y) *cnv_y = ih->data->current_plot->mAxisY.mTrafo->Transform(y);
}

void IupPlotTransformTo(Ihandle* ih, double cnv_x, double cnv_y, double *x, double *y)
{
  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return;

  if (ih->iclass->nativetype != IUP_TYPECANVAS || 
      !IupClassMatch(ih, "plot"))
    return;

  if (x) *x = ih->data->current_plot->mAxisX.mTrafo->TransformBack(cnv_x);
  if (y) *y = ih->data->current_plot->mAxisY.mTrafo->TransformBack(cnv_y);
}

void IupPlotPaintTo(Ihandle* ih, cdCanvas* cnv)
{
  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return;

  if (ih->iclass->nativetype != IUP_TYPECANVAS || 
      !IupClassMatch(ih, "plot"))
    return;

  if (!cnv)
    return;

  cdCanvas *old_cd_canvas  = ih->data->cd_canvas;

  ih->data->cd_canvas = (cdCanvas*)cnv;
  iupPlotUpdateViewports(ih);

  // when drawing to an external canvas
  int flush = 0, // never flush
    only_current = 0,  // draw all plots
    reset_redraw = 1;  // always render

  iupPlotRedraw(ih, flush, only_current, reset_redraw);

  ih->data->cd_canvas = old_cd_canvas;
  iupPlotUpdateViewports(ih);
}


static const char* iPlotSkipValue(const char* line_buffer)
{
  // fix next separator
  char ch = *line_buffer;
  while (ch != ' ' && ch != '\t' && ch != ';' && ch != 0)
  {
    line_buffer++;
    ch = *line_buffer;
  }

  // skip separators
  while ((ch == ' ' || ch == '\t' || ch == ';') && ch != 0)
  {
    line_buffer++;
    ch = *line_buffer;
  }

  return line_buffer;
}

static int iPlotCountDataSets(const char* line_buffer)
{
  int ds_count = 0;

  while (*line_buffer != 0)
  {
    line_buffer = iPlotSkipValue(line_buffer);
    ds_count++;
  }

  return ds_count;
}

static int iPlotAddToDataSets(Ihandle* ih, const char* line_buffer, int ds_start, int ds_count)
{
  double x = 0;
  double value;

  for (int ds = 0; ds < ds_count; ds++)
  {
    int ret = sscanf(line_buffer, "%lf", &value);
    if (!ret)
      return 0;

    line_buffer = iPlotSkipValue(line_buffer);

    if (ds == 0)
      x = value;
    else
    {
      double y = value;

      iupPlotDataSet* theDataSet = ih->data->current_plot->mDataSetList[ds_start + ds-1];
      theDataSet->AddSample(x, y);
    }
  }

  return 1;
}

static int iPlotAddToDataSetsStrX(Ihandle* ih, const char* line_buffer, int ds_start, int ds_count)
{
  char x[100] = "";
  double value = 0;

  for (int ds = 0; ds < ds_count; ds++)
  {
    if (ds == 0)
    {
      int ret = sscanf(line_buffer, "%s", x);
      if (!ret)
        return 0;
    }
    else
    {
      int ret = sscanf(line_buffer, "%lf", &value);
      if (!ret)
        return 0;
    }

    line_buffer = iPlotSkipValue(line_buffer);

    if (ds != 0)
    {
      double y = value;

      iupPlotDataSet* theDataSet = ih->data->current_plot->mDataSetList[ds_start + ds - 1];
      theDataSet->AddSample(x, y);
    }
  }

  return 1;
}

static int iPlotLoadDataFile(Ihandle* ih, IlineFile* line_file, int strXdata)
{
  int first_line = 1;
  int ds_count = 0;
  int ds, ds_start = ih->data->current_plot->mDataSetListCount;

  do
  {
    int line_len = iupLineFileReadLine(line_file);
    if (line_len == -1)
      return 0;

    const char* line_buffer = iupLineFileGetBuffer(line_file);

    int i = 0;
    while (line_buffer[i] == ' ') /* ignore spaces at start */
      i++;

    if (line_buffer[i] == 0) /* skip empty line */
      continue;

    if (line_buffer[i] == '#') /* "#" signifies a comment line when used as the first non-space character on a line */
      continue;

    if (first_line)
    {
      ds_count = iPlotCountDataSets(line_buffer);
      if (ds_count < 2) // must have at least X and Y1, could have Y2, Y3, ...
        return 0;

      for (ds = 0; ds < ds_count - 1; ds++)
      {
        iupPlotDataSet* theDataSet = new iupPlotDataSet(strXdata? true: false);
        ih->data->current_plot->AddDataSet(theDataSet);
      }

      first_line = 0;
    }

    if (strXdata)
    {
      if (!iPlotAddToDataSetsStrX(ih, line_buffer + i, ds_start, ds_count))
        return 0;
    }
    else
    {
      if (!iPlotAddToDataSets(ih, line_buffer + i, ds_start, ds_count))
        return 0;
    }

  } while (!iupLineFileEOF(line_file));

  return 1;
}

int IupPlotLoadData(Ihandle* ih, const char* filename, int strXdata)
{
  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return 0;

  if (ih->iclass->nativetype != IUP_TYPECANVAS ||
      !IupClassMatch(ih, "plot"))
      return 0;

  if (!filename)
    return 0;

  IlineFile* line_file = iupLineFileOpen(filename);
  if (!line_file)
    return 0;

  int error = iPlotLoadDataFile(ih, line_file, strXdata);

  iupLineFileClose(line_file);

  return error;
}

int IupPlotFindSample(Ihandle* ih, double cnv_x, double cnv_y, int *ds_index, int *sample_index)
{
  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return 0;

  if (ih->iclass->nativetype != IUP_TYPECANVAS ||
      !IupClassMatch(ih, "plot"))
      return 0;

  int ds, sample;
  double rx, ry;
  const char* ds_name;
  const char* strX;
  if (ih->data->current_plot->FindDataSetSample(cnv_x, cnv_y, ds, ds_name, sample, rx, ry, strX))
  {
    if (ds_index) *ds_index = ds;
    if (sample_index) *sample_index = sample;
    return 1;
  }

  return 0;
}


/************************************************************************************/


static int iPlotMapMethod(Ihandle* ih)
{
  if (ih->data->graphics_mode == IUP_PLOT_OPENGL)
  {
    IupGLMakeCurrent(ih);
    double res = IupGetDouble(NULL, "SCREENDPI") / 25.4;
    ih->data->cd_canvas = cdCreateCanvasf(CD_GL, "10x10 %g", res);
  }
  else if (ih->data->graphics_mode == IUP_PLOT_IMAGERGB)
    ih->data->cd_canvas = cdCreateCanvas(CD_IUPDBUFFERRGB, ih);
  else if (ih->data->graphics_mode == IUP_PLOT_NATIVEPLUS)
  {
    int old_plus = cdUseContextPlus(1);
    ih->data->cd_canvas = cdCreateCanvas(CD_IUPDBUFFER, ih);
    cdUseContextPlus(old_plus);
  }
  else
    ih->data->cd_canvas = cdCreateCanvas(CD_IUPDBUFFER, ih);
  if (!ih->data->cd_canvas)
    return IUP_ERROR;

  for(int p=0; p<ih->data->plot_list_count; p++)
    ih->data->plot_list[p]->mRedraw = true;

  return IUP_NOERROR;
}

static void iPlotUnMapMethod(Ihandle* ih)
{
  if (ih->data->cd_canvas != NULL)
  {
    cdKillCanvas(ih->data->cd_canvas);
    ih->data->cd_canvas = NULL;
  }
}

static void iPlotDestroyMethod(Ihandle* ih)
{
  for(int p=0; p<ih->data->plot_list_count; p++)
    delete ih->data->plot_list[p];

  iupPlotDataSet* theDataSet = (iupPlotDataSet*)iupAttribGet(ih, "_IUP_PLOT_DATASET");
  if (theDataSet)
    delete theDataSet;
}

static int iPlotCreateMethod(Ihandle* ih, void **params)
{
  (void)params;

  /* free the data alocated by IupCanvas */
  free(ih->data);
  ih->data = iupALLOCCTRLDATA();

  ih->data->read_only = 1;
  ih->data->plot_list_count = 1;
  ih->data->numcol = 1;
  ih->data->last_tip_ds = -1;
  ih->data->last_tip_sample = -1;
  ih->data->last_click_plot = -1;
  ih->data->graphics_mode = IUP_PLOT_NATIVEPLUS;

  ih->data->plot_list[0] = new iupPlot(ih, 0, 0);   // font style/size will be initialized bt stardardfont initialization
  ih->data->current_plot = ih->data->plot_list[ih->data->current_plot_index];

  /* IupCanvas callbacks */
  IupSetCallback(ih, "ACTION",      (Icallback)iPlotAction_CB);
  IupSetCallback(ih, "RESIZE_CB",   (Icallback)iPlotResize_CB);
  IupSetCallback(ih, "BUTTON_CB",   (Icallback)iPlotButton_CB);
  IupSetCallback(ih, "MOTION_CB",   (Icallback)iPlotMotion_CB);
  IupSetCallback(ih, "WHEEL_CB", (Icallback)iPlotWheel_CB);
  IupSetCallback(ih, "KEYPRESS_CB", (Icallback)iPlotKeyPress_CB);

  return IUP_NOERROR;
}

static Iclass* iPlotNewClass(void)
{
  Iclass* ic = iupClassNew(iupRegisterFindClass("glcanvas"));

  ic->name = (char*)"plot";
  ic->format = NULL;  /* none */
  ic->nativetype = IUP_TYPECANVAS;
  ic->childtype = IUP_CHILDNONE;
  ic->is_interactive = 1;

  /* Class functions */
  ic->New = iPlotNewClass;
  ic->Create  = iPlotCreateMethod;
  ic->Destroy = iPlotDestroyMethod;
  ic->Map     = iPlotMapMethod;
  ic->UnMap   = iPlotUnMapMethod;

  /* IupPlot Callbacks */
  iupClassRegisterCallback(ic, "POSTDRAW_CB", "C");
  iupClassRegisterCallback(ic, "PREDRAW_CB", "C");
  iupClassRegisterCallback(ic, "CLICKSAMPLE_CB", "iiddi");
  iupClassRegisterCallback(ic, "DRAWSAMPLE_CB", "iiddi");
  iupClassRegisterCallback(ic, "PLOTMOTION_CB", "dds");
  iupClassRegisterCallback(ic, "PLOTBUTTON_CB", "iidds");
  iupClassRegisterCallback(ic, "DELETE_CB", "iidd");
  iupClassRegisterCallback(ic, "DELETEBEGIN_CB", "");
  iupClassRegisterCallback(ic, "DELETEEND_CB", "");
  iupClassRegisterCallback(ic, "SELECT_CB", "iiddi");
  iupClassRegisterCallback(ic, "SELECTBEGIN_CB", "");
  iupClassRegisterCallback(ic, "SELECTEND_CB", "");
  iupClassRegisterCallback(ic, "MENUCONTEXT_CB", "nii");
  iupClassRegisterCallback(ic, "MENUCONTEXTCLOSE_CB", "nii");

  iupPlotRegisterAttributes(ic);

  if (iupStrEqualNoCase(IupGetGlobal("LANGUAGE"), "ENGLISH"))
  {
    IupSetLanguageString("IUP_EXPORT", "Export");
    IupSetLanguageString("IUP_COPY", "Copy");
    IupSetLanguageString("IUP_PRINTDLG", "Print...");

    IupSetLanguageString("IUP_ZOOMINAC", "Zoom In\t+");
    IupSetLanguageString("IUP_ZOOMOUTAC", "Zoom Out\t-");
    IupSetLanguageString("IUP_RESETZOOMAC", "Reset Zoom\t.");
    IupSetLanguageString("IUP_SHOWHIDELEGEND", "Show/Hide Legend");
    IupSetLanguageString("IUP_SHOWHIDEGRID", "Show/Hide Grid");

    IupSetLanguageString("IUP_ERRORINVALIDFORMULA", "Invalid Formula.");

    IupSetLanguageString("IUP_PROPERTIESDLG", "Properties...");
    IupSetLanguageString("IUP_DATASETPROPERTIESDLG", "Dataset Properties...");
    
    IupSetLanguageString("IUP_NAME", "Name:");
    IupSetLanguageString("IUP_COLOR", "Color:");
    IupSetLanguageString("IUP_MODE", "Mode:");
    IupSetLanguageString("IUP_LINES", "Lines");
    IupSetLanguageString("IUP_MARKS", "Marks");
    IupSetLanguageString("IUP_MARKSLINES", "Marks & Lines");
    IupSetLanguageString("IUP_BARS", "Bars");
    IupSetLanguageString("IUP_AREA", "Area");
    IupSetLanguageString("IUP_LINESTYLE", "Line Style:");
    IupSetLanguageString("IUP_CONTINUOUS", "Continuous");
    IupSetLanguageString("IUP_DASHED", "Dashed");
    IupSetLanguageString("IUP_DOTTED", "Dotted");
    IupSetLanguageString("IUP_DASH_DOT", "Dash Dot");
    IupSetLanguageString("IUP_DASH_DOT_DOT", "Dash Dot Dot");
    IupSetLanguageString("IUP_LINEWIDTH", "Line Width:");
    IupSetLanguageString("IUP_MARKSTYLE", "Mark Style:");
    IupSetLanguageString("IUP_PLUS", "Plus");
    IupSetLanguageString("IUP_STAR", "Star");
    IupSetLanguageString("IUP_CIRCLE", "Circle");
    IupSetLanguageString("IUP_X", "X");
    IupSetLanguageString("IUP_BOX", "Box");
    IupSetLanguageString("IUP_DIAMOND", "Diamond");
    IupSetLanguageString("IUP_HOLLOW_CIRCLE", "Hollow Circle");
    IupSetLanguageString("IUP_HOLLOW_BOX", "Hollow Box");
    IupSetLanguageString("IUP_HOLLOW_DIAMOND", "Hollow Diamond");
    IupSetLanguageString("IUP_MARKSIZE", "Mark Size:");

    IupSetLanguageString("IUP_CLOSE", "Close");

    IupSetLanguageString("IUP_BACKGROUND", "Background");
    IupSetLanguageString("IUP_TITLE", "Title");
    IupSetLanguageString("IUP_LEGEND", "Legend");
    IupSetLanguageString("IUP_LEGENDBOX", "Legend Box");
    IupSetLanguageString("IUP_GRID", "Grid");
    IupSetLanguageString("IUP_GRIDMINOR", "Grid Minor");
    IupSetLanguageString("IUP_XAXIS", "X Axis");
    IupSetLanguageString("IUP_YAXIS", "Y Axis");
    IupSetLanguageString("IUP_AXISLABEL", "Axis Label");
    IupSetLanguageString("IUP_AXISTICKS", "Axis Ticks");
    IupSetLanguageString("IUP_AXISTICKSNUMBER", "Axis Ticks Number");

    IupSetLanguageString("IUP_TEXT", "Text:");
    IupSetLanguageString("IUP_SHOW", "Show:");
    IupSetLanguageString("IUP_HORIZONTAL", "Horizontal");
    IupSetLanguageString("IUP_VERTICAL", "Vertical");

    IupSetLanguageString("IUP_WARNING", "Warning!");
    IupSetLanguageString("IUP_CHANGESNOTAPPLIEDAPPLY", "Changes Not Applied. Apply?");

    IupSetLanguageString("IUP_MARGIN", "Margin");
    IupSetLanguageString("IUP_LEFT", "Left:");
    IupSetLanguageString("IUP_RIGHT", "Right:");
    IupSetLanguageString("IUP_TOP", "Top:");
    IupSetLanguageString("IUP_BOTTOM", "Bottom:");

    IupSetLanguageString("IUP_FONTSTYLE", "Font Style:");
    IupSetLanguageString("IUP_FONTSIZE", "Font Size:");
    IupSetLanguageString("IUP_PLAIN", "Plain");
    IupSetLanguageString("IUP_BOLD", "Bold");
    IupSetLanguageString("IUP_ITALIC", "Italic");
    IupSetLanguageString("IUP_BOLDITALIC", "Bold Italic");

    IupSetLanguageString("IUP_POSITION", "Position:");
    IupSetLanguageString("IUP_POSXY", "(x,y):");
    IupSetLanguageString("IUP_TOPRIGHT", "Top Right");
    IupSetLanguageString("IUP_TOPLEFT", "Top Left");
    IupSetLanguageString("IUP_BOTTOMRIGHT", "Bottom Right");
    IupSetLanguageString("IUP_BOTTOMLEFT", "Bottom Left");
    IupSetLanguageString("IUP_BOTTOMCENTER", "Bottom Center");
    IupSetLanguageString("IUP_XY", "(x,y)");

    IupSetLanguageString("IUP_SHOWARROW", "Show Arrow:");
    IupSetLanguageString("IUP_MIN", "Min:");
    IupSetLanguageString("IUP_MAX", "Max:");
    IupSetLanguageString("IUP_REVERSE", "Reverse:");
    IupSetLanguageString("IUP_CROSSORIGIN", "Cross Origin:");
    IupSetLanguageString("IUP_CENTERED", "Centered:");

    IupSetLanguageString("IUP_SCALE", "Scale:");
    IupSetLanguageString("IUP_LINEAR", "Linear");
    IupSetLanguageString("IUP_LOG10", "Logarithm (base 10)");
    IupSetLanguageString("IUP_LOG2", "Logarithm (base 2)");
    IupSetLanguageString("IUP_LOGN", "Logarithm (base e)");

    IupSetLanguageString("IUP_SPACING", "Spacing:");
    IupSetLanguageString("IUP_MAJORSPAN", "Major Span:");
    IupSetLanguageString("IUP_MINORDIVISION", "Minor Division:");
    IupSetLanguageString("IUP_SIZE", "Size:");
    IupSetLanguageString("IUP_MAJOR", "Major:");
    IupSetLanguageString("IUP_MINOR", "Minor:");

    IupSetLanguageString("IUP_ROTATE", "Rotate:");
    IupSetLanguageString("IUP_ANGLE", "Angle:");

    IupSetLanguageString("IUP_VALUE", "Value:");
    IupSetLanguageString("IUP_DECIMALS", "Decimals:");
    IupSetLanguageString("IUP_FORMAT", "Format:");
  }
  else if (iupStrEqualNoCase(IupGetGlobal("LANGUAGE"), "PORTUGUESE"))
  {
    IupSetLanguageString("IUP_EXPORT", "Exportar");
    IupSetLanguageString("IUP_COPY", "Copiar");
    IupSetLanguageString("IUP_PRINTDLG", "Imprimir...");

    IupSetLanguageString("IUP_ZOOMINAC", "Mais Zoom\t+");
    IupSetLanguageString("IUP_ZOOMOUTAC", "Menos Zoom\t-");
    IupSetLanguageString("IUP_RESETZOOMAC", "Reiniciar Zoom\t.");
    IupSetLanguageString("IUP_SHOWHIDELEGEND", "Mostra/Esconde Legenda");
    IupSetLanguageString("IUP_SHOWHIDEGRID", "Mostra/Esconde Grade");

    IupSetLanguageString("IUP_ERRORINVALIDFORMULA", "FÛrmula Inv·lida.");

    IupSetLanguageString("IUP_PROPERTIESDLG", "Properties...");
    IupSetLanguageString("IUP_DATASETPROPERTIESDLG", "Dataset Properties...");

    IupSetLanguageString("IUP_NAME", "Nome:");
    IupSetLanguageString("IUP_COLOR", "Cor:");
    IupSetLanguageString("IUP_MODE", "Modo:");
    IupSetLanguageString("IUP_LINES", "Linhas");
    IupSetLanguageString("IUP_MARKS", "Marcas");
    IupSetLanguageString("IUP_MARKSLINES", "Marcas e Linhas");
    IupSetLanguageString("IUP_BARS", "Barras");
    IupSetLanguageString("IUP_AREA", "¡rea");
    IupSetLanguageString("IUP_LINESTYLE", "Estilo de Linha:");
    IupSetLanguageString("IUP_CONTINUOUS", "ContÌnuo");
    IupSetLanguageString("IUP_DASHED", "Tracejada");
    IupSetLanguageString("IUP_DOTTED", "Pontilhado");
    IupSetLanguageString("IUP_DASH_DOT", "TraÁo Ponto");
    IupSetLanguageString("IUP_DASH_DOT_DOT", "TraÁo Ponto Ponto");
    IupSetLanguageString("IUP_LINEWIDTH", "Largura de Linha:");
    IupSetLanguageString("IUP_MARKSTYLE", "Estilo de Marca:");
    IupSetLanguageString("IUP_PLUS", "Mais");
    IupSetLanguageString("IUP_STAR", "Estrela");
    IupSetLanguageString("IUP_CIRCLE", "CÌrculo");
    IupSetLanguageString("IUP_X", "X");
    IupSetLanguageString("IUP_BOX", "Caixa");
    IupSetLanguageString("IUP_DIAMOND", "Diamante");
    IupSetLanguageString("IUP_HOLLOW_CIRCLE", "CÌrculo Oco");
    IupSetLanguageString("IUP_HOLLOW_BOX", "Caixa Oca");
    IupSetLanguageString("IUP_HOLLOW_DIAMOND", "Diamante Oco");
    IupSetLanguageString("IUP_MARKSIZE", "Tamanho de Marca:");

    IupSetLanguageString("IUP_CLOSE", "Fechar");

    IupSetLanguageString("IUP_BACKGROUND", "Fundo");
    IupSetLanguageString("IUP_TITLE", "TÌtulo");
    IupSetLanguageString("IUP_LEGEND", "Legenda");
    IupSetLanguageString("IUP_LEGENDBOX", "Caixa da Legenda");
    IupSetLanguageString("IUP_GRID", "Grade");
    IupSetLanguageString("IUP_GRIDMINOR", "Grade Secund·ria");
    IupSetLanguageString("IUP_XAXIS", "Eixo X");
    IupSetLanguageString("IUP_YAXIS", "Eixo Y");
    IupSetLanguageString("IUP_AXISLABEL", "Etiqueta do Eixo");
    IupSetLanguageString("IUP_AXISTICKS", "Marcas do Eixo");
    IupSetLanguageString("IUP_AXISTICKSNUMBER", "N˙meros do Eixo");

    IupSetLanguageString("IUP_TEXT", "Texto:");
    IupSetLanguageString("IUP_SHOW", "Mostrar:");
    IupSetLanguageString("IUP_HORIZONTAL", "Horizontal");
    IupSetLanguageString("IUP_VERTICAL", "Vertical");

    IupSetLanguageString("IUP_WARNING", "AtenÁ„o!");
    IupSetLanguageString("IUP_CHANGESNOTAPPLIEDAPPLY", "ModificaÁıes n„o aplicadas. Aplicar?");

    IupSetLanguageString("IUP_MARGIN", "Margem");
    IupSetLanguageString("IUP_LEFT", "Esquerda:");
    IupSetLanguageString("IUP_RIGHT", "Direita:");
    IupSetLanguageString("IUP_TOP", "Superior:");
    IupSetLanguageString("IUP_BOTTOM", "Inferior:");

    IupSetLanguageString("IUP_FONTSTYLE", "Estilo da Fonte:");
    IupSetLanguageString("IUP_FONTSIZE", "Tamanho da Fonte:");
    IupSetLanguageString("IUP_PLAIN", "Regular");
    IupSetLanguageString("IUP_BOLD", "Negrito");
    IupSetLanguageString("IUP_ITALIC", "It·lico");
    IupSetLanguageString("IUP_BOLDITALIC", "Negrito It·lico");

    IupSetLanguageString("IUP_POSITION", "PosiÁ„o:");
    IupSetLanguageString("IUP_POSXY", "(x,y):");
    IupSetLanguageString("IUP_TOPRIGHT", "Superior Direita");
    IupSetLanguageString("IUP_TOPLEFT", "Superior Esquerda");
    IupSetLanguageString("IUP_BOTTOMRIGHT", "Inferior Direita");
    IupSetLanguageString("IUP_BOTTOMLEFT", "Inferior Esquerda");
    IupSetLanguageString("IUP_BOTTOMCENTER", "Inferior Centro");
    IupSetLanguageString("IUP_XY", "(x,y)");

    IupSetLanguageString("IUP_SHOWARROW", "Mostrar Seta:");
    IupSetLanguageString("IUP_MIN", "Min:");
    IupSetLanguageString("IUP_MAX", "Max:");
    IupSetLanguageString("IUP_REVERSE", "Reverso:");
    IupSetLanguageString("IUP_CROSSORIGIN", "Origem Cruzada:");
    IupSetLanguageString("IUP_CENTERED", "Centrado:");

    IupSetLanguageString("IUP_SCALE", "Escala:");
    IupSetLanguageString("IUP_LINEAR", "Linear");
    IupSetLanguageString("IUP_LOG10", "Logaritmo (base 10)");
    IupSetLanguageString("IUP_LOG2", "Logaritmo (base 2)");
    IupSetLanguageString("IUP_LOGN", "Logaritmo (base e)");

    IupSetLanguageString("IUP_SPACING", "EspaÁamento:");
    IupSetLanguageString("IUP_MAJORSPAN", "Intervalo Principal:");
    IupSetLanguageString("IUP_MINORDIVISION", "Divisıes Secund·rias:");
    IupSetLanguageString("IUP_SIZE", "Tamanho:");
    IupSetLanguageString("IUP_MAJOR", "Principal:");
    IupSetLanguageString("IUP_MINOR", "Secund·rio:");

    IupSetLanguageString("IUP_ROTATE", "RotaÁ„o:");
    IupSetLanguageString("IUP_ANGLE", "¬ngulo:");

    IupSetLanguageString("IUP_VALUE", "Valor:");
    IupSetLanguageString("IUP_DECIMALS", "Decimais:");
    IupSetLanguageString("IUP_FORMAT", "Formato:");

    if (IupGetInt(NULL, "UTF8MODE"))
    {
      /* When seeing this file assuming ISO8859-1 encoding, above will appear correct.
         When seeing this file assuming UTF-8 encoding, bellow will appear correct. */

      IupSetLanguageString("IUP_ERRORINVALIDFORMULA", "F√≥rmula Inv√°lida.");
      IupSetLanguageString("IUP_AREA", "√Årea");
      IupSetLanguageString("IUP_CONTINUOUS", "Cont√≠nuo");
      IupSetLanguageString("IUP_DASH_DOT", "Tra√ßo Ponto");
      IupSetLanguageString("IUP_DASH_DOT_DOT", "Tra√ßo Ponto Ponto");
      IupSetLanguageString("IUP_CIRCLE", "C√≠rculo");
      IupSetLanguageString("IUP_HOLLOW_CIRCLE", "C√≠rculo Oco");
      IupSetLanguageString("IUP_TITLE", "T√≠tulo");
      IupSetLanguageString("IUP_GRIDMINOR", "Grade Secund√°ria");
      IupSetLanguageString("IUP_AXISTICKSNUMBER", "N√∫meros do Eixo");
      IupSetLanguageString("IUP_WARNING", "Aten√ß√£o!");
      IupSetLanguageString("IUP_CHANGESNOTAPPLIEDAPPLY", "Modifica√ß√µes n√£o aplicadas. Aplicar?");
      IupSetLanguageString("IUP_ITALIC", "It√°lico");
      IupSetLanguageString("IUP_BOLDITALIC", "Negrito It√°lico");
      IupSetLanguageString("IUP_POSITION", "Posi√ß√£o:");
      IupSetLanguageString("IUP_SPACING", "Espa√ßamento:");
      IupSetLanguageString("IUP_MINORDIVISION", "Divis√µes Secund√°rias:");
      IupSetLanguageString("IUP_MINOR", "Secund√°rio:");
      IupSetLanguageString("IUP_ROTATE", "Rota√ß√£o:");
      IupSetLanguageString("IUP_ANGLE", "√Çngulo:");
    }
  }

  return ic;
}

Ihandle* IupPlot(void)
{
  return IupCreate("plot");
}

void IupPlotOpen(void)
{
  IupGLCanvasOpen();
  cdInitContextPlus();

  if (!IupGetGlobal("_IUP_PLOT_OPEN"))
  {
    iupRegisterClass(iPlotNewClass());
    IupSetGlobal("_IUP_PLOT_OPEN", "1");
  }
}
