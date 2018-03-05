/*
 * IupPlot Test
 * Description : Create all built-in plots.
 *               It is organised as two side-by-side panels:
 *                 - left panel for current plot control
 *                 - right panel containg tabbed plots
 *      Remark : depend on libs IUP, CD, IUP_PLOT
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


#include "iup.h"
#include "iupcontrols.h"
#include "iup_plot.h"
#include "iupim.h"

#include <cd.h>
#include <cdiup.h>
#include <cdpdf.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifdef PLOT_TEST

#define MAXPLOT 10  /* room for examples */


static Ihandle *plot[MAXPLOT] = { NULL }; /* Plot controls */
static Ihandle *dial1, *dial2,          /* dials for zooming */
*tgg1, *tgg2,            /* autoscale on|off toggles */
*tgg3, *tgg4,            /* grid show|hide toggles */
*tgg5,                   /* legend show|hide toggle */
*tabs;                   /* tabbed control */

static int delete_cb(Ihandle* ih, int index, int sample_index, double x, double y)
{
  printf("DELETE_CB(%d, %d, %g, %g)\n", index, sample_index, x, y);
  return IUP_DEFAULT;
}

static int select_cb(Ihandle* ih, int index, int sample_index, double x, double y, int select)
{
  printf("SELECT_CB(%d, %d, %g, %g, %d)\n", index, sample_index, x, y, select);
  return IUP_DEFAULT;
}

static int edit_cb(Ihandle* ih, int index, int sample_index, double x, double y, double *new_x, double *new_y)
{
  printf("EDIT_CB(%d, %d, %g, %g, %g, %g)\n", index, sample_index, x, y, *new_x, *new_y);
  return IUP_DEFAULT;
}

static int postdraw_cb(Ihandle* ih, cdCanvas* cnv)
{
  double ix, iy;

  IupPlotTransform(ih, 0.003f, 0.02f, &ix, &iy);
  cdCanvasFont(cnv, NULL, CD_BOLD, 10);
  cdCanvasTextAlignment(cnv, CD_SOUTH);
  cdfCanvasText(cnv, ix, iy, "My Inline Legend");
  printf("POSTDRAW_CB()\n");

  return IUP_DEFAULT;
}

static int predraw_cb(Ihandle* ih, cdCanvas* cnv)
{
  printf("PREDRAW_CB()\n");
  return IUP_DEFAULT;
}

static int clicksample_cb(Ihandle* ih, int ds, int sample, double rx, double ry, int button)
{
  printf("CLICKSAMPLE_CB(ds=%d, sample=%d, rx=%.4f, ry=%.4f, button=%d)\n", ds, sample, rx, ry, button);
  return IUP_DEFAULT;
}

static int clicksegment_cb(Ihandle* ih, int ds, int sample1, double rx1, double ry1, int sample2, double rx2, double ry2, int button)
{
  printf("CLICKSEGMENT_CB(ds=%d, sample1=%d, rx1=%.4f, ry1=%.4f, sample2=%d, rx12=%.4f, ry2=%.4f, button=%d)\n", ds, sample1, rx1, ry1, sample2, rx2, ry2, button);
  return IUP_DEFAULT;
}

static void InitPlots(void)
{
  int theI;
  double x, y, theFac, Mult = 1, count;

  /************************************************************************/
  /* PLOT 0 - MakeExamplePlot1 */
  IupSetAttribute(plot[0], "TITLE", "AutoScale");
  IupSetAttribute(plot[0], "FONT", "Helvetica, 10");
  //  IupSetAttribute(plot[0], "MARGINTOP", "40");
  //  IupSetAttribute(plot[0], "MARGINLEFT", "40");
  //  IupSetAttribute(plot[0], "MARGINBOTTOM", "50");
  //  IupSetAttribute(plot[0], "TITLEFONTSIZE", "16");
  IupSetAttribute(plot[0], "LEGENDSHOW", "YES");
  IupSetAttribute(plot[0], "AXS_XLABEL", "gnu (Foo)");
  IupSetAttribute(plot[0], "AXS_YLABEL", "Space (m^3)");
  //  IupSetAttribute(plot[0], "AXS_YFONTSIZE", "8");
  //  IupSetAttribute(plot[0], "AXS_YTICKFONTSIZE", "8");
  //  IupSetAttribute(plot[0], "AXS_XFONTSIZE", "10");
  //  IupSetAttribute(plot[0], "AXS_YFONTSIZE", "10");
  IupSetAttribute(plot[0], "AXS_XCROSSORIGIN", "Yes");
  IupSetAttribute(plot[0], "AXS_YCROSSORIGIN", "Yes");
//  IupSetAttribute(plot[0], "GRAPHICSMODE", "OPENGL");
  //  IupSetAttribute(plot[0], "ACTIVE", "No");

  //  IupSetAttribute(plot[0], "AXS_AUTOSCALEEQUAL", "Yes");
  //  Mult = 80;
//  IupSetAttribute(plot[0], "VIEWPORTSQUARE", "Yes");
  IupSetAttribute(plot[0], "HIGHLIGHTMODE", "CURVE");

  if (0)
  {
    Ihandle* image = IupLoadImage("../test/corsega.tif");
    if (image)
    {
      IupSetAttributeHandle(plot[0], "BACKIMAGE", image);
      IupSetAttribute(plot[0], "BACKIMAGE_XMIN", "-100");
      IupSetAttribute(plot[0], "BACKIMAGE_XMAX", "150");
      IupSetAttribute(plot[0], "BACKIMAGE_YMIN", "-2");
      IupSetAttribute(plot[0], "BACKIMAGE_YMAX", "2");
    }
  }

  theFac = 1.0 / (100 * 100 * 100);
  IupPlotBegin(plot[0], 0);
  for (theI = -100; theI <= 100; theI++)
  {
    x = (theI + 50);
    y = Mult*theFac*theI*theI*theI;
    IupPlotAdd(plot[0], x, y);
  }
  IupPlotEnd(plot[0]);
  IupSetAttribute(plot[0], "DS_LINEWIDTH", "3");
  IupSetAttribute(plot[0], "DS_LEGEND", "Line");
  IupSetAttribute(plot[0], "DS_MODE", "LINE");

  theFac = 2.0 / 100;
  IupPlotBegin(plot[0], 0);
  for (theI = -100; theI < 0; theI++)
  {
    x = theI;
    y = -Mult * theFac*theI;
    IupPlotAdd(plot[0], x, y);
  }

  {
    int index = IupPlotEnd(plot[0]); /* add an empty plot */
    double px[210], py[210];
    int count = 0;
    for (theI = 0; theI <= 100; theI++)
    {
      x = theI;
      y = -Mult * theFac*theI;
      px[theI] = x;
      py[theI] = y;
      count++;
    }
    //IupPlotAddSamples(plot[0], index, px, py, count);
    IupPlotInsertSamples(plot[0], index, 100, px, py, count);
  }

  IupSetAttribute(plot[0], "DS_LEGEND", "Curve 1");

  IupPlotBegin(plot[0], 0);
  for (theI = -100; theI <= 100; theI++)
  {
    x = (0.01*theI*theI - 30);
    y = 0.01 * Mult * theI;
    IupPlotAdd(plot[0], x, y);
  }
  IupPlotEnd(plot[0]);
  IupSetAttribute(plot[0], "DS_LEGEND", "Curve 2");
  IupSetAttribute(plot[0], "DS_ORDEREDX", "No");

  /************************************************************************/
  /* PLOT 1 */
  IupSetAttribute(plot[1], "TITLE", "No Autoscale+No CrossOrigin");
  IupSetAttribute(plot[1], "FONT", "Helvetica, 10");
  //  IupSetAttribute(plot[1], "TITLEFONTSIZE", "16");
  //  IupSetAttribute(plot[1], "MARGINTOP", "40");
  //IupSetAttribute(plot[1], "MARGINLEFT", "65");
  //IupSetAttribute(plot[1], "MARGINBOTTOM", "60");
  IupSetAttribute(plot[1], "BGCOLOR", "0 192 192");
  IupSetAttribute(plot[1], "AXS_XLABEL", "Tg (X)");
  IupSetAttribute(plot[1], "AXS_YLABEL", "Tg (Y)");
  IupSetAttribute(plot[1], "AXS_XAUTOMIN", "NO");
  IupSetAttribute(plot[1], "AXS_XAUTOMAX", "NO");
  IupSetAttribute(plot[1], "AXS_YAUTOMIN", "NO");
  IupSetAttribute(plot[1], "AXS_YAUTOMAX", "NO");
  IupSetAttribute(plot[1], "AXS_XMIN", "10");
  IupSetAttribute(plot[1], "AXS_XMAX", "60");
  IupSetAttribute(plot[1], "AXS_YMIN", "-0.5");
  IupSetAttribute(plot[1], "AXS_YMAX", "0.5");
  IupSetAttribute(plot[1], "AXS_XFONTSTYLE", "ITALIC");
  IupSetAttribute(plot[1], "AXS_YFONTSTYLE", "BOLD");
  IupSetAttribute(plot[1], "AXS_XREVERSE", "YES");
  IupSetAttribute(plot[1], "GRIDCOLOR", "128 255 128");
  IupSetAttribute(plot[1], "GRIDLINESTYLE", "DOTTED");
  IupSetAttribute(plot[1], "GRID", "YES");
  IupSetAttribute(plot[1], "LEGENDSHOW", "YES");
  IupSetAttribute(plot[1], "AXS_XLABELCENTERED", "Yes");
  IupSetAttribute(plot[1], "AXS_YLABELCENTERED", "Yes");
  IupSetAttribute(plot[1], "GRAPHICSMODE", "IMAGERGB");
  IupSetAttribute(plot[1], "HIGHLIGHTMODE", "CURVE");

  theFac = 1.0 / (100 * 100 * 100);
  IupPlotBegin(plot[1], 0);
  for (theI = 0; theI <= 100; theI++)
  {
    x = (theI);
    y = theFac*theI*theI*theI;
    IupPlotAdd(plot[1], x, y);
  }
  IupPlotEnd(plot[1]);
  IupSetAttribute(plot[1], "DS_ORDEREDX", "No");

  theFac = 2.0 / 100;
  IupPlotBegin(plot[1], 0);
  for (theI = 0; theI <= 100; theI++)
  {
    x = (theI);
    y = -theFac*theI;
    IupPlotAdd(plot[1], x, y);
  }
  IupPlotEnd(plot[1]);
  IupSetAttribute(plot[1], "DS_ORDEREDX", "No");

  /************************************************************************/
  /* PLOT 2 */
  IupSetAttribute(plot[2], "TITLE", "Log Scale");
  //  IupSetAttribute(plot[2], "TITLEFONTSIZE", "16");
  //IupSetAttribute(plot[2], "MARGINTOP", "40");
  //IupSetAttribute(plot[2], "MARGINLEFT", "70");
  //IupSetAttribute(plot[2], "MARGINBOTTOM", "60");
  IupSetAttribute(plot[2], "GRID", "YES");
  IupSetAttribute(plot[2], "AXS_XSCALE", "LOG10");
  IupSetAttribute(plot[2], "AXS_YSCALE", "LOG2");
  IupSetAttribute(plot[2], "AXS_XLABEL", "Tg (X)");
  IupSetAttribute(plot[2], "AXS_YLABEL", "Tg (Y)");
  IupSetAttribute(plot[2], "AXS_XFONTSTYLE", "BOLD");
  IupSetAttribute(plot[2], "AXS_YFONTSTYLE", "BOLD");
  IupSetAttribute(plot[2], "HIGHLIGHTMODE", "CURVE");

  theFac = 100.0 / (100 * 100 * 100);
  IupPlotBegin(plot[2], 0);
  for (theI = 0; theI <= 100; theI++)
  {
    x = (0.0001 + theI*0.001);
    y = (0.01 + theFac*theI*theI*theI);
    IupPlotAdd(plot[2], x, y);
  }
  IupPlotEnd(plot[2]);
  IupSetAttribute(plot[2], "DS_COLOR", "100 100 200");
  IupSetAttribute(plot[2], "DS_LINESTYLE", "DOTTED");

  /************************************************************************/
  /* PLOT 3 */
  IupSetAttribute(plot[3], "TITLE", "Bar Mode");
  //  IupSetAttribute(plot[3], "TITLEFONTSIZE", "16");
  //IupSetAttribute(plot[3], "MARGINTOP", "40");
  //IupSetAttribute(plot[3], "MARGINLEFT", "30");
  //IupSetAttribute(plot[3], "MARGINBOTTOM", "30");
  IupSetAttribute(plot[3], "HIGHLIGHTMODE", "SAMPLE");

  {
    const char * kLables[12] = { "jan", "feb", "mar", "apr", "may", "jun", "jul", "aug", "sep", "oct", "nov", "dec" };
    const double kData[12] = { 10, -20, 30, 40, 50, 60, 70, 80, 90, 0, 10, 20 };
    IupPlotBegin(plot[3], 1);
    for (theI = 0; theI < 12; theI++)
      IupPlotAddStr(plot[3], kLables[theI], kData[theI]);
  }
  IupPlotEnd(plot[3]);
  IupSetAttribute(plot[3], "DS_COLOR", "180 180 250");
  IupSetAttribute(plot[3], "DS_BAROUTLINE", "Yes");
  IupSetAttribute(plot[3], "DS_BAROUTLINECOLOR", "70 70 160");
  IupSetAttribute(plot[3], "DS_BARSPACING", "0");
  IupSetAttribute(plot[3], "DS_MODE", "BAR");
//  IupSetAttribute(plot[3], "DS_BARMULTICOLOR", "Yes");

  /************************************************************************/
  /* PLOT 4 */
  IupSetAttribute(plot[4], "TITLE", "Marks Mode");
  //  IupSetAttribute(plot[4], "TITLEFONTSIZE", "16");
  //  IupSetAttribute(plot[4], "MARGINTOP", "40");
  //IupSetAttribute(plot[4], "MARGINLEFT", "45");
  //IupSetAttribute(plot[4], "MARGINBOTTOM", "40");
  IupSetAttribute(plot[4], "AXS_XAUTOMIN", "NO");
  IupSetAttribute(plot[4], "AXS_XAUTOMAX", "NO");
  IupSetAttribute(plot[4], "AXS_YAUTOMIN", "NO");
  IupSetAttribute(plot[4], "AXS_YAUTOMAX", "NO");
  IupSetAttribute(plot[4], "AXS_XMIN", "0");
  IupSetAttribute(plot[4], "AXS_XMAX", "0.011");
  IupSetAttribute(plot[4], "AXS_YMIN", "0");
  IupSetAttribute(plot[4], "AXS_YMAX", "0.22");
  IupSetAttribute(plot[4], "AXS_XTICKFORMAT", "%1.3f");
  IupSetAttribute(plot[4], "LEGENDSHOW", "YES");
  IupSetAttribute(plot[4], "LEGENDPOS", "BOTTOMRIGHT");

  theFac = 100.0 / (100 * 100 * 100);
  IupPlotBegin(plot[4], 0);
  for (theI = 0; theI <= 10; theI++)
  {
    x = (0.0001 + theI*0.001);
    y = (0.01 + theFac*theI*theI);
    IupPlotAdd(plot[4], x, y);
  }
  IupPlotEnd(plot[4]);
  IupSetAttribute(plot[4], "DS_MODE", "MARKLINE");
  IupSetCallback(plot[4], "CLICKSEGMENT_CB", (Icallback)clicksegment_cb);
  IupSetCallback(plot[4], "CLICKSAMPLE_CB", (Icallback)clicksample_cb);

  IupPlotBegin(plot[4], 0);
  for (theI = 0; theI <= 10; theI++)
  {
    x = (0.0001 + theI*0.001);
    y = (0.2 - theFac*theI*theI);
    IupPlotAdd(plot[4], x, y);
  }
  IupPlotEnd(plot[4]);
  IupSetAttribute(plot[4], "DS_MODE", "MARK");
  IupSetAttribute(plot[4], "DS_MARKSTYLE", "HOLLOW_CIRCLE");
  IupSetAttribute(plot[4], "HIGHLIGHTMODE", "BOTH");

  /************************************************************************/
  /* PLOT 5 */
  IupSetAttribute(plot[5], "TITLE", "Data Selection and Editing");
  //IupSetAttribute(plot[5], "TITLEFONTSIZE", "16");
  //IupSetAttribute(plot[5], "MARGINTOP", "40");

#if 0
  IupPlotLoadData(plot[5], "../test/plot.dat", 0);
#else
  theFac = 100.0 / (100 * 100 * 100);
  IupPlotBegin(plot[5], 0);
  for (theI = -10; theI <= 10; theI++) {
    x = (0.001*theI);
    y = (0.01 + theFac*theI*theI*theI);
    IupPlotAdd(plot[5], x, y);
  }
  IupPlotEnd(plot[5]);

  IupSetAttribute(plot[5], "AXS_XCROSSORIGIN", "Yes");
  IupSetAttribute(plot[5], "AXS_YCROSSORIGIN", "Yes");
#endif

  IupSetAttribute(plot[5], "DS_COLOR", "100 100 200");
  IupSetCallback(plot[5], "DELETE_CB", (Icallback)delete_cb);
  IupSetCallback(plot[5], "SELECT_CB", (Icallback)select_cb);
  IupSetCallback(plot[5], "POSTDRAW_CB", (Icallback)postdraw_cb);
  IupSetCallback(plot[5], "PREDRAW_CB", (Icallback)predraw_cb);

  /************************************************************************/
  /* PLOT 6 */
  IupPlotBegin(plot[6], 0);
  for (theI = 0; theI <= 360; theI = theI + 10)
  {
    x = theI;
    y = sin(x*M_PI / 180.);
    IupPlotAdd(plot[6], x, y);
  }
  IupPlotEnd(plot[6]);
  IupSetAttribute(plot[6], "TITLE", "Step curve");
  IupSetAttribute(plot[6], "DS_LINEWIDTH", "3");
  IupSetAttribute(plot[6], "DS_LEGEND", "Line");
  IupSetAttribute(plot[6], "DS_MODE", "STEP");
  IupSetAttribute(plot[6], "HIGHLIGHTMODE", "SAMPLE");
  IupSetAttribute(plot[6], "DATASETCLIPPING", "NONE");

  /************************************************************************/
  /* PLOT 7 */
  IupSetAttribute(plot[7], "TITLE", "Stem Mode");
  //  IupSetAttribute(plot[4], "TITLEFONTSIZE", "16");
  //  IupSetAttribute(plot[4], "MARGINTOP", "40");
  //IupSetAttribute(plot[4], "MARGINLEFT", "45");
  //IupSetAttribute(plot[4], "MARGINBOTTOM", "40");
  IupSetAttribute(plot[7], "AXS_XAUTOMIN", "NO");
  IupSetAttribute(plot[7], "AXS_XAUTOMAX", "NO");
  IupSetAttribute(plot[7], "AXS_YAUTOMIN", "NO");
  IupSetAttribute(plot[7], "AXS_YAUTOMAX", "NO");
  IupSetAttribute(plot[7], "AXS_XMIN", "0");
  IupSetAttribute(plot[7], "AXS_XMAX", "0.011");
  IupSetAttribute(plot[7], "AXS_YMIN", "0");
  IupSetAttribute(plot[7], "AXS_YMAX", "0.22");
  IupSetAttribute(plot[7], "AXS_XTICKFORMAT", "%1.3f");
  IupSetAttribute(plot[7], "LEGENDSHOW", "YES");
  IupSetAttribute(plot[7], "LEGENDPOS", "BOTTOMRIGHT");

  IupPlotBegin(plot[7], 0);
  for (theI = 0; theI <= 10; theI++) {
    x = (0.0001 + theI*0.001);
    y = (0.2 - theFac*theI*theI);
    IupPlotAdd(plot[7], x, y);
  }
  IupPlotEnd(plot[7]);
  IupSetAttribute(plot[7], "DS_MODE", "MARKSTEM");
  IupSetAttribute(plot[7], "DS_MARKSTYLE", "HOLLOW_CIRCLE");
  IupSetAttribute(plot[7], "HIGHLIGHTMODE", "SAMPLE");

  /************************************************************************/
  /* PLOT 8 */
  IupSetAttribute(plot[8], "TITLE", "Horizontal Bar Mode");
  //  IupSetAttribute(plot[3], "TITLEFONTSIZE", "16");
  //IupSetAttribute(plot[3], "MARGINTOP", "40");
  //IupSetAttribute(plot[3], "MARGINLEFT", "30");
  //IupSetAttribute(plot[3], "MARGINBOTTOM", "30");

  {
    const double kLables[12] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };
    const double kData[12] = { 10, 20, 30, 40, 50, 60, 70, 80, 90, 0, 10, 20 };
    IupPlotBegin(plot[8], 0);
    for (theI = 0; theI < 12; theI++)
      IupPlotAdd(plot[8], kData[theI], kLables[theI]);
  }
  IupPlotEnd(plot[8]);
  IupSetAttribute(plot[8], "AXS_YAUTOMIN", "NO");
  IupSetAttribute(plot[8], "AXS_YMIN", "0");
  IupSetAttribute(plot[8], "DS_COLOR", "100 100 200");
  IupSetAttribute(plot[8], "DS_MODE", "HORIZONTALBAR");
  IupSetAttribute(plot[8], "DS_BAROUTLINE", "Yes");
  IupSetAttribute(plot[8], "DS_BAROUTLINECOLOR", "70 70 160");
  IupSetAttribute(plot[8], "HIGHLIGHTMODE", "SAMPLE");

  /************************************************************************/
  /* PLOT 9 */
  IupSetAttribute(plot[9], "PLOT_COUNT", "4");
  IupSetAttribute(plot[9], "PLOT_NUMCOL", "2");

  IupSetAttribute(plot[9], "PLOT_CURRENT", "0");
  IupSetAttribute(plot[9], "TITLE", "Multi Bar Mode");
  //  IupSetAttribute(plot[3], "TITLEFONTSIZE", "16");
  //IupSetAttribute(plot[3], "MARGINTOP", "40");
  //IupSetAttribute(plot[3], "MARGINLEFT", "30");
  //IupSetAttribute(plot[3], "MARGINBOTTOM", "30");

  {
    const char * kLables[12] = { "jan", "feb", "mar", "apr", "may", "jun", "jul", "aug", "sep", "oct", "nov", "dec" };
    const double kData[12] = { 10, 20, 30, 40, 50, 60, 70, 80, 90, 0, 10, 20 };
    IupPlotBegin(plot[9], 1);
    for (theI = 0; theI < 12; theI++)
      IupPlotAddStr(plot[9], kLables[theI], kData[theI]);
  }
  IupPlotEnd(plot[9]);
  IupSetAttribute(plot[9], "DS_MODE", "MULTIBAR");
  IupSetAttribute(plot[9], "DS_BAROUTLINE", "Yes");
  IupSetAttribute(plot[9], "DS_BAROUTLINECOLOR", "70 70 160");
  IupSetAttribute(plot[9], "HIGHLIGHTMODE", "SAMPLE");

  {
    const char * kLables[12] = { "jan", "feb", "mar", "apr", "may", "jun", "jul", "aug", "sep", "oct", "nov", "dec" };
    const double kData[12] = { 5, 10, 15, 20, 25, 30, 35, 40, 45, 0, 5, 10 };
    IupPlotBegin(plot[9], 1);
    for (theI = 0; theI < 12; theI++)
      IupPlotAddStr(plot[9], kLables[theI], kData[theI]);
  }
  IupPlotEnd(plot[9]);
  IupSetAttribute(plot[9], "DS_MODE", "MULTIBAR");
  IupSetAttribute(plot[9], "DS_BAROUTLINE", "Yes");
  IupSetAttribute(plot[9], "DS_BAROUTLINECOLOR", "70 70 160");
  IupSetAttribute(plot[9], "HIGHLIGHTMODE", "SAMPLE");
//  IupSetAttribute(plot[9], "GRAPHICSMODE", "OPENGL");

  {
    const char * kLables[12] = { "jan", "feb", "mar", "apr", "may", "jun", "jul", "aug", "sep", "oct", "nov", "dec" };
    const double kData[12] = { 15, 20, 15, 25, 45, 10, 5, 0, 45, 90, 115, 110 };
    IupPlotBegin(plot[9], 1);
    for (theI = 0; theI < 12; theI++)
      IupPlotAddStr(plot[9], kLables[theI], kData[theI]);
  }
  IupPlotEnd(plot[9]);
  IupSetAttribute(plot[9], "DS_MODE", "MULTIBAR");
  IupSetAttribute(plot[9], "DS_BAROUTLINE", "Yes");
  IupSetAttribute(plot[9], "DS_BAROUTLINECOLOR", "70 70 160");
  IupSetAttribute(plot[9], "HIGHLIGHTMODE", "SAMPLE");

  /************************************************************************/
  /* PLOT 10 */
  IupSetAttribute(plot[9], "PLOT_CURRENT", "1");
  IupPlotBegin(plot[9], 0);
  for (theI = 0; theI <= 360; theI = theI + 10) {
    x = theI;
    y = sin(x*M_PI / 180.);
    IupPlotAdd(plot[9], x, y);
  }
  IupPlotEnd(plot[9]);
  count = 0;
  for (theI = 0; theI <= 360; theI = theI + 10) {
    IupPlotSetSampleExtra(plot[9], 0, count++, 0.1 * (double)rand() / (double)RAND_MAX);
  }
  IupSetAttribute(plot[9], "TITLE", "Error Bar");
  //  IupSetAttribute(plot[9], "DS_LINEWIDTH", "3");
  IupSetAttribute(plot[9], "DS_LEGEND", "Error Bar");
  IupSetAttribute(plot[9], "DS_MODE", "ERRORBAR");
  IupSetAttribute(plot[9], "HIGHLIGHTMODE", "BOTH");
  IupSetAttribute(plot[9], "DATASETCLIPPING", "AREAOFFSET");

  /************************************************************************/
  /* PLOT 11 */
  IupSetAttribute(plot[9], "PLOT_CURRENT", "2");
  IupSetAttribute(plot[9], "TITLE", "Pie");

  {
    const char * kLables[12] = { "jan", "feb", "mar", "apr", "may", "jun", "jul", "aug", "sep", "oct", "nov", "dec" };
    const double kData[12] = { 10, 20, 30, 40, 50, 60, 70, 80, 90, 0, 10, 20 };
    IupPlotBegin(plot[9], 1);
    for (theI = 0; theI < 12; theI++)
      IupPlotAddStr(plot[9], kLables[theI], kData[theI]);
  }
  IupPlotEnd(plot[9]);
  //IupSetAttribute(plot[9], "DS_PIESTARTANGLE", "90");
  IupSetAttribute(plot[9], "DS_PIECONTOUR", "YES");
  IupSetAttribute(plot[9], "HIGHLIGHTMODE", "SAMPLE");
  IupSetAttribute(plot[9], "DS_PIESLICELABEL", "X");
  //IupSetAttribute(plot[9], "DS_PIESLICELABEL", "PERCENT");
  IupSetAttribute(plot[9], "AXS_YFONTSIZE", "10"); // label font
  if (1)
  {
    IupSetAttribute(plot[9], "AXS_YCOLOR", "255 255 255"); // label color
    IupSetAttribute(plot[9], "DS_PIEHOLE", "0.3"); // factor of radius
    IupSetAttribute(plot[9], "DS_COLOR", "255 255 255"); // contour color
    IupSetAttribute(plot[9], "DS_LINEWIDTH", "3");
  }
  else
  {
    IupSetAttribute(plot[9], "DS_PIERADIUS", "0.8");
    IupSetAttribute(plot[9], "DS_PIESLICELABELPOS", "-1");
    IupSetAttribute(plot[9], "AXS_YCOLOR", "0 0 0"); // label color
    IupSetAttribute(plot[9], "DS_LINEWIDTH", "1"); // contour line width
    IupSetAttribute(plot[9], "DS_COLOR", "0 0 0"); // contour color
  }

  IupSetAttribute(plot[9], "DS_MODE", "PIE");
//  IupSetAttribute(plot[9], "GRAPHICSMODE", "OPENGL");
  //IupSetAttribute(plot[9], "GRAPHICSMODE", "NATIVE");
  //IupSetAttribute(plot[9], "GRAPHICSMODE", "IMAGERGB");

  IupSetAttribute(plot[9], "VIEWPORTSQUARE", "Yes");


  /************************************************************************/
  /* PLOT 12 */
  IupSetAttribute(plot[9], "PLOT_CURRENT", "3");
  IupSetAttribute(plot[9], "TITLE", "�rea");
  IupSetAttribute(plot[9], "FONT", "Helvetica, 10");
  IupSetAttribute(plot[9], "LEGENDSHOW", "YES");
  IupSetAttribute(plot[9], "AXS_XLABEL", "Year");
  IupSetAttribute(plot[9], "AXS_YLABEL", "Company Performance");
  IupSetAttribute(plot[9], "AXS_XMIN", "2013");
  IupSetAttribute(plot[9], "AXS_XMAX", "2016");
  IupSetAttribute(plot[9], "AXS_YMIN", "0");
  IupSetAttribute(plot[9], "AXS_YMAX", "1200");
  IupSetAttribute(plot[9], "AXS_XAUTOMIN", "NO");
  IupSetAttribute(plot[9], "AXS_XAUTOMAX", "NO");
  IupSetAttribute(plot[9], "AXS_YAUTOMIN", "NO");
  IupSetAttribute(plot[9], "AXS_YAUTOMAX", "NO");

  {
    const double kData[4] = { 1000, 1170, 660, 1030 };
    const double kLables[4] = { 2013, 2014, 2015, 2016 };
    IupPlotBegin(plot[9], 0);
    for (theI = 0; theI < 4; theI++)
      IupPlotAdd(plot[9], kLables[theI], kData[theI]);
  }
  IupPlotEnd(plot[9]);
  IupSetAttribute(plot[9], "DS_MODE", "AREA");
  IupSetAttribute(plot[9], "DS_AREATRANSPARENCY", "64");
  IupSetAttribute(plot[9], "DS_LINEWIDTH", "3");
  IupSetAttribute(plot[9], "HIGHLIGHTMODE", "BOTH");

  {
    const double kData[4] = { 400, 460, 1120, 540 };
    const double kLables[4] = { 2013, 2014, 2015, 2016 };
    IupPlotBegin(plot[9], 0);
    for (theI = 0; theI < 4; theI++)
      IupPlotAdd(plot[9], kLables[theI], kData[theI]);
  }
  IupPlotEnd(plot[9]);
  IupSetAttribute(plot[9], "DS_MODE", "AREA");
  IupSetAttribute(plot[9], "DS_AREATRANSPARENCY", "64");
  IupSetAttribute(plot[9], "DS_LINEWIDTH", "3");
  IupSetAttribute(plot[9], "HIGHLIGHTMODE", "BOTH");
}

static int tabs_get_index(void)
{
  Ihandle *curr_tab = IupGetHandle(IupGetAttribute(tabs, "VALUE"));
  char *ss = IupGetAttribute(curr_tab, "TABTITLE");
  ss += 5; /* Skip "Plot " */
  return atoi(ss);
}

/* Some processing required by current tab change: the controls at left
   will be updated according to current plot props */
static int tabs_tabchange_cb(Ihandle* self, Ihandle* new_tab)
{
  int ii = 0;

  char *ss = IupGetAttribute(new_tab, "TABTITLE");
  ss += 5; /* Skip "Plot " */
  ii = atoi(ss);

  /* autoscaling X axis */
  if (IupGetInt(plot[ii], "AXS_XAUTOMIN") && IupGetInt(plot[ii], "AXS_XAUTOMAX")) {
    IupSetAttribute(tgg2, "VALUE", "ON");
    IupSetAttribute(dial2, "ACTIVE", "NO");
  }
  else {
    IupSetAttribute(tgg2, "VALUE", "OFF");
    IupSetAttribute(dial2, "ACTIVE", "YES");
  }
  /* autoscaling Y axis */
  if (IupGetInt(plot[ii], "AXS_YAUTOMIN") && IupGetInt(plot[ii], "AXS_YAUTOMAX")) {
    IupSetAttribute(tgg1, "VALUE", "ON");
    IupSetAttribute(dial1, "ACTIVE", "NO");
  }
  else {
    IupSetAttribute(tgg1, "VALUE", "OFF");
    IupSetAttribute(dial1, "ACTIVE", "YES");
  }

  /* grid */
  if (IupGetInt(plot[ii], "GRID"))
  {
    IupSetAttribute(tgg3, "VALUE", "ON");
    IupSetAttribute(tgg4, "VALUE", "ON");
  }
  else
  {
    /* X axis */
    if (*IupGetAttribute(plot[ii], "GRID") == 'V')
      IupSetAttribute(tgg3, "VALUE", "ON");
    else
      IupSetAttribute(tgg3, "VALUE", "OFF");
    /* Y axis */
    if (*IupGetAttribute(plot[ii], "GRID") == 'H')
      IupSetAttribute(tgg4, "VALUE", "ON");
    else
      IupSetAttribute(tgg4, "VALUE", "OFF");
  }

  /* legend */
  if (IupGetInt(plot[ii], "LEGENDSHOW"))
    IupSetAttribute(tgg5, "VALUE", "ON");
  else
    IupSetAttribute(tgg5, "VALUE", "OFF");

  return IUP_DEFAULT;
}

/* show/hide V grid */
static int tgg3_cb(Ihandle *self, int v)
{
  int ii = tabs_get_index();

  if (v)
  {
    if (IupGetInt(tgg4, "VALUE"))
      IupSetAttribute(plot[ii], "GRID", "YES");
    else
      IupSetAttribute(plot[ii], "GRID", "VERTICAL");
  }
  else
  {
    if (!IupGetInt(tgg4, "VALUE"))
      IupSetAttribute(plot[ii], "GRID", "NO");
    else
      IupSetAttribute(plot[ii], "GRID", "HORIZONTAL");
  }

  IupSetAttribute(plot[ii], "REDRAW", NULL);

  return IUP_DEFAULT;
}


/* show/hide H grid */
static int tgg4_cb(Ihandle *self, int v)
{
  int ii = tabs_get_index();

  if (v)
  {
    if (IupGetInt(tgg3, "VALUE"))
      IupSetAttribute(plot[ii], "GRID", "YES");
    else
      IupSetAttribute(plot[ii], "GRID", "HORIZONTAL");
  }
  else
  {
    if (!IupGetInt(tgg3, "VALUE"))
      IupSetAttribute(plot[ii], "GRID", "NO");
    else
      IupSetAttribute(plot[ii], "GRID", "VERTICAL");
  }

  IupSetAttribute(plot[ii], "REDRAW", NULL);

  return IUP_DEFAULT;
}


/* show/hide legend */
static int tgg5_cb(Ihandle *self, int v)
{
  int ii = tabs_get_index();

  if (v)
    IupSetAttribute(plot[ii], "LEGENDSHOW", "YES");
  else
    IupSetAttribute(plot[ii], "LEGENDSHOW", "NO");

  IupSetAttribute(plot[ii], "REDRAW", NULL);

  return IUP_DEFAULT;
}


/* autoscale Y */
static int tgg1_cb(Ihandle *self, int v)
{
  int ii = tabs_get_index();

  if (v) {
    IupSetAttribute(dial1, "ACTIVE", "NO");
    IupSetAttribute(plot[ii], "AXS_YAUTOMIN", "YES");
    IupSetAttribute(plot[ii], "AXS_YAUTOMAX", "YES");
  }
  else {
    IupSetAttribute(dial1, "ACTIVE", "YES");
    IupSetAttribute(plot[ii], "AXS_YAUTOMIN", "NO");
    IupSetAttribute(plot[ii], "AXS_YAUTOMAX", "NO");
  }

  IupSetAttribute(plot[ii], "REDRAW", NULL);

  return IUP_DEFAULT;
}


/* autoscale X */
static int tgg2_cb(Ihandle *self, int v)
{
  int ii = tabs_get_index();

  if (v) {
    IupSetAttribute(dial2, "ACTIVE", "NO");
    IupSetAttribute(plot[ii], "AXS_XAUTOMIN", "YES");
    IupSetAttribute(plot[ii], "AXS_XAUTOMAX", "YES");
  }
  else {
    IupSetAttribute(dial2, "ACTIVE", "YES");
    IupSetAttribute(plot[ii], "AXS_XAUTOMIN", "NO");
    IupSetAttribute(plot[ii], "AXS_XAUTOMAX", "NO");
  }

  IupSetAttribute(plot[ii], "REDRAW", NULL);

  return IUP_DEFAULT;
}


/* Y zoom */
static int dial1_btndown_cb(Ihandle *self, double angle)
{
  int ii = tabs_get_index();

  IupStoreAttribute(plot[ii], "OLD_YMIN", IupGetAttribute(plot[ii], "AXS_YMIN"));
  IupStoreAttribute(plot[ii], "OLD_YMAX", IupGetAttribute(plot[ii], "AXS_YMAX"));

  return IUP_DEFAULT;
}

static int dial1_btnup_cb(Ihandle *self, double angle)
{
  int ii = tabs_get_index();
  double x1, x2, xm;
  char *ss;

  x1 = IupGetFloat(plot[ii], "OLD_YMIN");
  x2 = IupGetFloat(plot[ii], "OLD_YMAX");

  ss = IupGetAttribute(plot[ii], "AXS_YMODE");
  if (ss && ss[3] == '2') {
    /* LOG2:  one circle will zoom 2 times */
    xm = 4.0 * fabs(angle) / 3.141592;
    if (angle > 0.0) { x2 /= xm; x1 *= xm; }
    else { x2 *= xm; x1 /= xm; }
  }
  if (ss && ss[3] == '1') {
    /* LOG10:  one circle will zoom 10 times */
    xm = 10.0 * fabs(angle) / 3.141592;
    if (angle > 0.0) { x2 /= xm; x1 *= xm; }
    else { x2 *= xm; x1 /= xm; }
  }
  else {
    /* LIN: one circle will zoom 2 times */
    xm = (x1 + x2) / 2.0;
    x1 = xm - (xm - x1)*(1.0 - angle*1.0 / 3.141592);
    x2 = xm + (x2 - xm)*(1.0 - angle*1.0 / 3.141592);
  }

  if (x1 < x2)
  {
    IupSetfAttribute(plot[ii], "AXS_YMIN", "%g", x1);
    IupSetfAttribute(plot[ii], "AXS_YMAX", "%g", x2);
  }

  IupSetAttribute(plot[ii], "REDRAW", NULL);

  return IUP_DEFAULT;
}


/* X zoom */
static int dial2_btndown_cb(Ihandle *self, double angle)
{
  int ii = tabs_get_index();

  IupStoreAttribute(plot[ii], "OLD_XMIN", IupGetAttribute(plot[ii], "AXS_XMIN"));
  IupStoreAttribute(plot[ii], "OLD_XMAX", IupGetAttribute(plot[ii], "AXS_XMAX"));

  return IUP_DEFAULT;
}

static int dial2_btnup_cb(Ihandle *self, double angle)
{
  int ii = tabs_get_index();
  double x1, x2, xm;

  x1 = IupGetFloat(plot[ii], "OLD_XMIN");
  x2 = IupGetFloat(plot[ii], "OLD_XMAX");

  xm = (x1 + x2) / 2.0;

  x1 = xm - (xm - x1)*(1.0 - angle*1.0 / 3.141592); /* one circle will zoom 2 times */
  x2 = xm + (x2 - xm)*(1.0 - angle*1.0 / 3.141592);

  IupSetfAttribute(plot[ii], "AXS_XMIN", "%g", x1);
  IupSetfAttribute(plot[ii], "AXS_XMAX", "%g", x2);

  IupSetAttribute(plot[ii], "REDRAW", NULL);

  return IUP_DEFAULT;
}

static int bt1_cb(Ihandle *self)
{
  int ii = tabs_get_index();
  IupSetAttribute(plot[ii], "CLEAR", "Yes");
  //  IupSetAttribute(plot[ii], "REMOVE", "0");
  IupSetAttribute(plot[ii], "REDRAW", NULL);

  //  cdCanvas* cnv = cdCreateCanvas(CD_PDF, "pplot.pdf -o");
  //  IupPlotPaintTo(plot[ii], cnv);
  //  cdKillCanvas(cnv);
  return IUP_DEFAULT;
}

void PlotTest(void)
{
  Ihandle *vboxr[MAXPLOT + 1];       /* tabs containing the plots */
  Ihandle *dlg, *vboxl, *hbox, *lbl1, *lbl2, *lbl3, *bt1,
    *boxinfo, *boxdial1, *boxdial2, *f1, *f2;
  int ii;

  IupPlotOpen();     /* init IupPlot library */

  /* create plots */
  for (ii = 0; ii < MAXPLOT; ii++)
  {
    plot[ii] = IupPlot();

    IupSetAttribute(plot[ii], "MENUITEMPROPERTIES", "Yes");
  }

  /* left panel: plot control
     Y zooming               */
  dial1 = IupDial("VERTICAL");
  lbl1 = IupLabel("+");
  lbl2 = IupLabel("-");
  boxinfo = IupVbox(lbl1, IupFill(), lbl2, NULL);
  boxdial1 = IupHbox(boxinfo, dial1, NULL);

  IupSetAttribute(boxdial1, "ALIGNMENT", "ACENTER");
  IupSetAttribute(boxinfo, "ALIGNMENT", "ACENTER");
  IupSetAttribute(boxinfo, "SIZE", "20x52");
  IupSetAttribute(boxinfo, "GAP", "2");
  IupSetAttribute(boxinfo, "MARGIN", "4");
  IupSetAttribute(boxinfo, "EXPAND", "YES");
  IupSetAttribute(lbl1, "EXPAND", "NO");
  IupSetAttribute(lbl2, "EXPAND", "NO");

  IupSetAttribute(dial1, "ACTIVE", "NO");
  IupSetAttribute(dial1, "SIZE", "20x52");
  IupSetCallback(dial1, "BUTTON_PRESS_CB", (Icallback)dial1_btndown_cb);
  IupSetCallback(dial1, "MOUSEMOVE_CB", (Icallback)dial1_btnup_cb);
  IupSetCallback(dial1, "BUTTON_RELEASE_CB", (Icallback)dial1_btnup_cb);

  tgg1 = IupToggle("Y Autoscale", NULL);
  IupSetCallback(tgg1, "ACTION", (Icallback)tgg1_cb);
  IupSetAttribute(tgg1, "VALUE", "ON");

  f1 = IupFrame(IupVbox(boxdial1, tgg1, NULL));
  IupSetAttribute(f1, "TITLE", "Y Min-Max");

  /* X zooming */
  dial2 = IupDial("HORIZONTAL");
  lbl1 = IupLabel("-");
  lbl2 = IupLabel("+");
  boxinfo = IupHbox(lbl1, IupFill(), lbl2, NULL);
  boxdial2 = IupVbox(dial2, boxinfo, NULL);

  IupSetAttribute(boxdial2, "ALIGNMENT", "ACENTER");
  IupSetAttribute(boxinfo, "ALIGNMENT", "ACENTER");
  IupSetAttribute(boxinfo, "SIZE", "64x16");
  IupSetAttribute(boxinfo, "GAP", "2");
  IupSetAttribute(boxinfo, "MARGIN", "4");
  IupSetAttribute(boxinfo, "EXPAND", "HORIZONTAL");

  IupSetAttribute(lbl1, "EXPAND", "NO");
  IupSetAttribute(lbl2, "EXPAND", "NO");

  IupSetAttribute(dial2, "ACTIVE", "NO");
  IupSetAttribute(dial2, "SIZE", "64x16");
  IupSetCallback(dial2, "BUTTON_PRESS_CB", (Icallback)dial2_btndown_cb);
  IupSetCallback(dial2, "MOUSEMOVE_CB", (Icallback)dial2_btnup_cb);
  IupSetCallback(dial2, "BUTTON_RELEASE_CB", (Icallback)dial2_btnup_cb);

  tgg2 = IupToggle("X Autoscale", NULL);
  IupSetCallback(tgg2, "ACTION", (Icallback)tgg2_cb);

  f2 = IupFrame(IupVbox(boxdial2, tgg2, NULL));
  IupSetAttribute(f2, "TITLE", "X Min-Max");

  lbl1 = IupLabel("");
  IupSetAttribute(lbl1, "SEPARATOR", "HORIZONTAL");

  tgg3 = IupToggle("Vertical Grid", NULL);
  IupSetCallback(tgg3, "ACTION", (Icallback)tgg3_cb);
  tgg4 = IupToggle("Horizontal Grid", NULL);
  IupSetCallback(tgg4, "ACTION", (Icallback)tgg4_cb);

  lbl2 = IupLabel("");
  IupSetAttribute(lbl2, "SEPARATOR", "HORIZONTAL");

  tgg5 = IupToggle("Legend", NULL);
  IupSetCallback(tgg5, "ACTION", (Icallback)tgg5_cb);

  lbl3 = IupLabel("");
  IupSetAttribute(lbl3, "SEPARATOR", "HORIZONTAL");

  bt1 = IupButton("Export PDF", NULL);
  IupSetCallback(bt1, "ACTION", (Icallback)bt1_cb);

  vboxl = IupVbox(f1, f2, lbl1, tgg3, tgg4, lbl2, tgg5, lbl3, bt1, NULL);
  IupSetAttribute(vboxl, "GAP", "4");
  IupSetAttribute(vboxl, "EXPAND", "NO");

  /* right panel: tabs with plots */
  for (ii = 0; ii < MAXPLOT; ii++) {
    vboxr[ii] = IupVbox(plot[ii], NULL); /* each plot a tab */
    IupSetfAttribute(vboxr[ii], "TABTITLE", "Plot %d", ii); /* name each tab */
    IupSetHandle(IupGetAttribute(vboxr[ii], "TABTITLE"), vboxr[ii]);
  }
  vboxr[MAXPLOT] = NULL; /* mark end of vector */

  //  tabs = IupZboxv(vboxr);
  //  IupSetAttribute(tabs, "VALUE", "Plot 3");
  tabs = IupTabsv(vboxr);
  IupSetCallback(tabs, "TABCHANGE_CB", (Icallback)tabs_tabchange_cb);

  hbox = IupHbox(vboxl, tabs, NULL);
  IupSetAttribute(hbox, "MARGIN", "4x4");
  IupSetAttribute(hbox, "GAP", "10");

  dlg = IupDialog(hbox);
  IupSetAttribute(dlg, "TITLE", "IupPlot Example");

  InitPlots(); /* It must be able to be done independent of dlg Mapping */

  tabs_tabchange_cb(tabs, vboxr[0]);

  IupSetAttribute(dlg, "SIZE", "300x");
  IupShowXY(dlg, IUP_CENTER, IUP_CENTER);
  IupSetAttribute(dlg, "SIZE", NULL);
}

#ifndef BIG_TEST
int main(int argc, char* argv[])
{
  IupOpen(&argc, &argv);
  IupControlsOpen();

  PlotTest();

  IupMainLoop();

  IupClose();

  return EXIT_SUCCESS;
}
#endif

#endif
