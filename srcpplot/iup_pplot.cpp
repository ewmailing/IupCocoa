/*
 * IupPPlot component
 *
 * Description : A component, derived from PPlot and IUP canvas
 *      Remark : Depend on libs IUP, CD, IUPCD
 */


#ifdef _MSC_VER
#pragma warning(disable: 4100)
#pragma warning(disable: 4512)
#endif

//#define USE_OPENGL 1
// Not fully working, needs improvements in CD_GL

#ifdef USE_OPENGL
#ifdef WIN32
#include <windows.h>
#endif

#if defined (__APPLE__) || defined (OSX)
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "iup.h"
#include "iupcbs.h"
#include "iup_pplot.h"
#include "iupkey.h"
#ifdef USE_OPENGL
#include "iupgl.h"
#endif

#include <cd.h>
#ifdef USE_OPENGL
#include <cdgl.h>
#else
#include <cdiup.h>
#include <cddbuf.h>
#include <cdirgb.h>
#endif

#include "iup_class.h"
#include "iup_register.h"
#include "iup_object.h"
#include "iup_attrib.h"
#include "iup_str.h"
#include "iup_drv.h"
#include "iup_stdcontrols.h"
#include "iup_assert.h"

#include "iupPPlot.h"
#include "iupPPlotInteraction.h"
#include "iuppplot.hpp"


#ifndef M_E
#define M_E 2.71828182846
#endif

#define MAX_PLOTS 20

struct _IcontrolData
{
  iupCanvas canvas;  /* from IupCanvas (must reserve it) */

  PPainterIup* plots[MAX_PLOTS];
  int plots_count;
  int plots_numcol;

  PPainterIup* plt;  /* Points to the current plot */
  int plt_index;     /* current plot index */

  int sync_view;

  cdCanvas* cd_canvas;      /* double buffer drawing surface */
};


static int iPPlotGetCDFontStyle(const char* value);

static void iPPlotFlush(Ihandle* ih)
{
  cdCanvasFlush(ih->data->cd_canvas);
#ifdef USE_OPENGL
  IupGLSwapBuffers(ih);
#endif
}

static void iPPlotSetPlotCurrent(Ihandle* ih, int p)
{
  ih->data->plt_index = p;
  ih->data->plt = ih->data->plots[ih->data->plt_index];
}

static int iPPlotRedraw_CB(Ihandle* ih)
{
  if (!ih->data->cd_canvas)
    return IUP_DEFAULT;

  int old_current = ih->data->plt_index;
  for(int p=0; p<ih->data->plots_count; p++)
  {
    iPPlotSetPlotCurrent(ih, p);
    ih->data->plt->Paint(0, 0);  /* full redraw only if nothing changed */
  }
  iPPlotSetPlotCurrent(ih, old_current);

  // Do the flush once
  iPPlotFlush(ih);

  return IUP_DEFAULT;
}

static void iPPlotUpdateSizes(Ihandle* ih)
{
  int w, h;

  if (!ih->data->cd_canvas)
    return;

  cdCanvasActivate(ih->data->cd_canvas);
  cdCanvasGetSize(ih->data->cd_canvas, &w, &h, NULL, NULL);

  int numcol = ih->data->plots_numcol;
  if (numcol > ih->data->plots_count) numcol = ih->data->plots_count;
  int numlin = ih->data->plots_count / numcol;
  int pw = w / numcol;
  int ph = h / numlin;

  for(int p=0; p<ih->data->plots_count; p++)
  {
    int lin = p / numcol;
    int col = p % numcol;
    int px = col * pw;
    int py = lin * ph;

    ih->data->plots[p]->SetSize(px, py, pw, ph);
  }
}

static int iPPlotResize_CB(Ihandle* ih)
{
  iPPlotUpdateSizes(ih);
  return IUP_DEFAULT;
}

static int iPPlotFindPlot(Ihandle* ih, int x, int y)
{
  int w, h;

  cdCanvasActivate(ih->data->cd_canvas);
  cdCanvasGetSize(ih->data->cd_canvas, &w, &h, NULL, NULL);

  int numcol = ih->data->plots_numcol;
  if (numcol > ih->data->plots_count) numcol = ih->data->plots_count;
  int numlin = ih->data->plots_count / numcol;
  int pw = w / numcol;
  int ph = h / numlin;

  int lin = y / ph;
  int col = x / pw;

  int index = lin * numcol + col;
  if (index < ih->data->plots_count)
    return index;

  return -1;
}

static int iPPlotMouseButton_CB(Ihandle* ih, int btn, int stat, int x, int y, char* r)
{
  int index = iPPlotFindPlot(ih, x, y);
  if (index<0)
    return IUP_DEFAULT;

  iPPlotSetPlotCurrent(ih, index);

  IFniiffs cb = (IFniiffs)IupGetCallback(ih, "PLOTBUTTON_CB");
  if (cb) 
  {
    ih->data->plt->UpdateViewport();

    float rx = ih->data->plt->_plot.mXTrafo->TransformBack((float)(x - ih->data->plt->_x));
    float ry = ih->data->plt->_plot.mYTrafo->TransformBack((float)(y - ih->data->plt->_y));
    if (cb(ih, btn, stat, rx, ry, r) == IUP_IGNORE)
      return IUP_DEFAULT;
  }

  ih->data->plt->MouseButton(btn, stat, x - ih->data->plt->_x, y - ih->data->plt->_y, r);

  return IUP_DEFAULT;
}

static int iPPlotMouseMove_CB(Ihandle* ih, int x, int y)
{
  int index = iPPlotFindPlot(ih, x, y);
  if (index<0)
    return IUP_DEFAULT;

  iPPlotSetPlotCurrent(ih, index);

  IFnff cb = (IFnff)IupGetCallback(ih, "PLOTMOTION_CB");
  if (cb) 
  {
    ih->data->plt->UpdateViewport();

    float rx = ih->data->plt->_plot.mXTrafo->TransformBack((float)(x - ih->data->plt->_x));
    float ry = ih->data->plt->_plot.mYTrafo->TransformBack((float)(y - ih->data->plt->_y));
    if (cb(ih, rx, ry) == IUP_IGNORE)
      return IUP_DEFAULT;
  }

  ih->data->plt->MouseMove(x - ih->data->plt->_x, y - ih->data->plt->_y);

  return IUP_DEFAULT;
}

static int iPPlotWheel_CB(Ihandle *ih, float delta, int x, int y, char *status)
{
  int index = iPPlotFindPlot(ih, x, y);
  if (index<0)
    return IUP_DEFAULT;

  iPPlotSetPlotCurrent(ih, index);
  ih->data->plt->MouseWheel(delta, status);
  return IUP_DEFAULT;
}

static int iPPlotKeyPress_CB(Ihandle* ih, int c, int press)
{
  int old_current = ih->data->plt_index;
  for (int p = 0; p<ih->data->plots_count; p++)
  {
    iPPlotSetPlotCurrent(ih, p);
    ih->data->plt->KeyPress(c, press);
  }
  iPPlotSetPlotCurrent(ih, old_current);
  return IUP_DEFAULT;
} 

void IupPPlotBegin(Ihandle* ih, int strXdata)
{
  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return;

  if (ih->iclass->nativetype != IUP_TYPECANVAS || 
      !IupClassMatch(ih, "pplot"))
    return;

  PlotDataBase* inXData = (PlotDataBase*)iupAttribGet(ih, "_IUP_PPLOT_XDATA");
  PlotDataBase* inYData = (PlotDataBase*)iupAttribGet(ih, "_IUP_PPLOT_YDATA");

  if (inXData) delete inXData;
  if (inYData) delete inYData;

  if (strXdata)
    inXData = (PlotDataBase*)(new StringPlotData());
  else
    inXData = (PlotDataBase*)(new PlotData());

  inYData = (PlotDataBase*)new PlotData();

  iupAttribSet(ih, "_IUP_PPLOT_XDATA",    (char*)inXData);
  iupAttribSet(ih, "_IUP_PPLOT_YDATA",    (char*)inYData);
}

void IupPPlotAdd(Ihandle* ih, float x, float y)
{
  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return;

  if (ih->iclass->nativetype != IUP_TYPECANVAS || 
      !IupClassMatch(ih, "pplot"))
    return;

  PlotData* inXData = (PlotData*)iupAttribGet(ih, "_IUP_PPLOT_XDATA");
  PlotData* inYData = (PlotData*)iupAttribGet(ih, "_IUP_PPLOT_YDATA");

  if (!inYData || !inXData || inXData->IsString())
    return;

  inXData->push_back(x);
  inYData->push_back(y);
}

void IupPPlotAddStr(Ihandle* ih, const char* x, float y)
{
  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return;

  if (ih->iclass->nativetype != IUP_TYPECANVAS || 
      !IupClassMatch(ih, "pplot"))
    return;

  StringPlotData *inXData = (StringPlotData*)iupAttribGet(ih, "_IUP_PPLOT_XDATA");
  PlotData *inYData = (PlotData*)iupAttribGet(ih, "_IUP_PPLOT_YDATA");

  if (!inYData || !inXData || !inXData->IsString())
    return;

  inXData->AddItem(x);
  inYData->push_back(y);
}

void IupPPlotInsertStr(Ihandle* ih, int inIndex, int inSampleIndex, const char* inX, float inY)
{
  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return;

  if (ih->iclass->nativetype != IUP_TYPECANVAS || 
      !IupClassMatch(ih, "pplot"))
    return;

  PlotDataBase* theXDataBase = ih->data->plt->_plot.mPlotDataContainer.GetXData(inIndex);
  PlotDataBase* theYDataBase = ih->data->plt->_plot.mPlotDataContainer.GetYData(inIndex);
  StringPlotData *theXData = (StringPlotData*)theXDataBase;
  PlotData *theYData = (PlotData*)theYDataBase;

  if (!theYData || !theXData || !theXData->IsString())
    return;

  theXData->InsertItem(inSampleIndex, inX);
  theYData->insert(theYData->begin()+inSampleIndex, inY);
}

void IupPPlotInsert(Ihandle* ih, int inIndex, int inSampleIndex, float inX, float inY)
{
  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return;

  if (ih->iclass->nativetype != IUP_TYPECANVAS || 
      !IupClassMatch(ih, "pplot"))
    return;

  PlotDataBase* theXDataBase = ih->data->plt->_plot.mPlotDataContainer.GetXData(inIndex);
  PlotDataBase* theYDataBase = ih->data->plt->_plot.mPlotDataContainer.GetYData(inIndex);
  PlotData* theXData = (PlotData*)theXDataBase;
  PlotData* theYData = (PlotData*)theYDataBase;

  if (!theYData || !theXData || theXData->IsString())
    return;

  theXData->insert(theXData->begin()+inSampleIndex, inX);
  theYData->insert(theYData->begin()+inSampleIndex, inY);
}

void IupPPlotGetSample(Ihandle* ih, int inIndex, int inSampleIndex, float *x, float *y)
{
  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return;

  if (ih->iclass->nativetype != IUP_TYPECANVAS ||
    !IupClassMatch(ih, "pplot"))
    return;

  PlotDataBase* theXDataBase = ih->data->plt->_plot.mPlotDataContainer.GetXData(inIndex);
  PlotDataBase* theYDataBase = ih->data->plt->_plot.mPlotDataContainer.GetYData(inIndex);
  PlotData* theXData = (PlotData*)theXDataBase;
  PlotData* theYData = (PlotData*)theYDataBase;

  if (!theYData || !theXData || theXData->IsString())
    return;

  if (x) *x = theXData->GetValue(inSampleIndex);
  if (y) *y = theYData->GetValue(inSampleIndex);
}

void IupPPlotGetSampleStr(Ihandle* ih, int inIndex, int inSampleIndex, const char* *x, float *y)
{
  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return;

  if (ih->iclass->nativetype != IUP_TYPECANVAS ||
    !IupClassMatch(ih, "pplot"))
    return;

  PlotDataBase* theXDataBase = ih->data->plt->_plot.mPlotDataContainer.GetXData(inIndex);
  PlotDataBase* theYDataBase = ih->data->plt->_plot.mPlotDataContainer.GetYData(inIndex);
  StringPlotData* theXData = (StringPlotData*)theXDataBase;
  PlotData* theYData = (PlotData*)theYDataBase;

  if (!theYData || !theXData || !theXData->IsString())
    return;

  if (x) *x = (*(theXData->GetStringData()))[inSampleIndex].c_str();
  if (y) *y = theYData->GetValue(inSampleIndex);
}

void IupPPlotAddPoints(Ihandle* ih, int inIndex, float *x, float *y, int count)
{
  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return;

  if (ih->iclass->nativetype != IUP_TYPECANVAS || 
      !IupClassMatch(ih, "pplot"))
    return;

  PlotDataBase* theXDataBase = ih->data->plt->_plot.mPlotDataContainer.GetXData(inIndex);
  PlotDataBase* theYDataBase = ih->data->plt->_plot.mPlotDataContainer.GetYData(inIndex);
  PlotData* inXData = (PlotData*)theXDataBase;
  PlotData* inYData = (PlotData*)theYDataBase;

  if (!inYData || !inXData || inXData->IsString())
    return;

  for (int i=0; i<count; i++)
  {
    inXData->push_back(x[i]);
    inYData->push_back(y[i]);
  }
}

void IupPPlotAddStrPoints(Ihandle* ih, int inIndex, const char** x, float* y, int count)
{
  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return;

  if (ih->iclass->nativetype != IUP_TYPECANVAS || 
      !IupClassMatch(ih, "pplot"))
    return;

  PlotDataBase* theXDataBase = ih->data->plt->_plot.mPlotDataContainer.GetXData(inIndex);
  PlotDataBase* theYDataBase = ih->data->plt->_plot.mPlotDataContainer.GetYData(inIndex);
  StringPlotData *inXData = (StringPlotData*)theXDataBase;
  PlotData   *inYData = (PlotData*)theYDataBase;

  if (!inYData || !inXData || !inXData->IsString())
    return;

  for (int i=0; i<count; i++)
  {
    inXData->AddItem(x[i]);
    inYData->push_back(y[i]);
  }
}

void IupPPlotInsertStrPoints(Ihandle* ih, int inIndex, int inSampleIndex, const char** inX, float* inY, int count)
{
  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return;

  if (ih->iclass->nativetype != IUP_TYPECANVAS || 
      !IupClassMatch(ih, "pplot"))
    return;

  PlotDataBase* theXDataBase = ih->data->plt->_plot.mPlotDataContainer.GetXData(inIndex);
  PlotDataBase* theYDataBase = ih->data->plt->_plot.mPlotDataContainer.GetYData(inIndex);
  StringPlotData *theXData = (StringPlotData*)theXDataBase;
  PlotData   *theYData = (PlotData*)theYDataBase;

  if (!theYData || !theXData || !theXData->IsString())
    return;

  for (int i=0; i<count; i++)
  {
    theXData->InsertItem(inSampleIndex+i, inX[i]);
    theYData->insert(theYData->begin()+(inSampleIndex+i), inY[i]);
  }
}

void IupPPlotInsertPoints(Ihandle* ih, int inIndex, int inSampleIndex, float *inX, float *inY, int count)
{
  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return;

  if (ih->iclass->nativetype != IUP_TYPECANVAS || 
      !IupClassMatch(ih, "pplot"))
    return;

  PlotDataBase* theXDataBase = ih->data->plt->_plot.mPlotDataContainer.GetXData(inIndex);
  PlotDataBase* theYDataBase = ih->data->plt->_plot.mPlotDataContainer.GetYData(inIndex);
  PlotData* theXData = (PlotData*)theXDataBase;
  PlotData* theYData = (PlotData*)theYDataBase;

  if (!theYData || !theXData || theXData->IsString())
    return;

  for (int i=0; i<count; i++)
  {
    theXData->insert(theXData->begin()+(inSampleIndex+i), inX[i]);
    theYData->insert(theYData->begin()+(inSampleIndex+i), inY[i]);
  }
}

int IupPPlotEnd(Ihandle* ih)
{
  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return -1;

  if (ih->iclass->nativetype != IUP_TYPECANVAS || 
      !IupClassMatch(ih, "pplot"))
    return -1;

  PlotDataBase* inXData = (PlotDataBase*)iupAttribGet(ih, "_IUP_PPLOT_XDATA");
  PlotDataBase* inYData = (PlotDataBase*)iupAttribGet(ih, "_IUP_PPLOT_YDATA");
  if (!inYData || !inXData)
    return -1;

  /* add to plot */
  ih->data->plt->_currentDataSetIndex = ih->data->plt->_plot.mPlotDataContainer.AddXYPlot(inXData, inYData);

  LegendData* legend = ih->data->plt->_plot.mPlotDataContainer.GetLegendData(ih->data->plt->_currentDataSetIndex);
  legend->mStyle.mFontStyle = iPPlotGetCDFontStyle(IupGetAttribute(ih, "LEGENDFONTSTYLE"));
  legend->mStyle.mFontSize  = IupGetInt(ih, "LEGENDFONTSIZE");

  iupAttribSet(ih, "_IUP_PPLOT_XDATA", NULL);
  iupAttribSet(ih, "_IUP_PPLOT_YDATA", NULL);

  ih->data->plt->_redraw = 1;
  return ih->data->plt->_currentDataSetIndex;
}

void IupPPlotTransform(Ihandle* ih, float x, float y, int *ix, int *iy)
{
  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return;

  if (ih->iclass->nativetype != IUP_TYPECANVAS || 
      !IupClassMatch(ih, "pplot"))
    return;

  if (ix) *ix = ih->data->plt->_plot.Round(ih->data->plt->_plot.mXTrafo->Transform(x));
  if (iy) *iy = ih->data->plt->_plot.Round(ih->data->plt->_plot.mYTrafo->Transform(y));
}

void IupPPlotTransformTo(Ihandle* ih, int x, int y, float *rx, float *ry)
{
  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return;

  if (ih->iclass->nativetype != IUP_TYPECANVAS || 
      !IupClassMatch(ih, "pplot"))
    return;

  if (rx) *rx = ih->data->plt->_plot.mXTrafo->TransformBack((float)x);
  if (ry) *ry = ih->data->plt->_plot.mYTrafo->TransformBack((float)y);
}

void IupPPlotPaintTo(Ihandle* ih, void* _cnv)
{
  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return;

  if (ih->iclass->nativetype != IUP_TYPECANVAS || 
      !IupClassMatch(ih, "pplot"))
    return;

  if (!_cnv)
    return;

  cdCanvas *old_cddbuffer  = ih->data->cd_canvas;
  ih->data->cd_canvas = (cdCanvas*)_cnv;

  iPPlotUpdateSizes(ih);

  int old_current = ih->data->plt_index;
  for (int p = 0; p<ih->data->plots_count; p++)
  {
    iPPlotSetPlotCurrent(ih, p);
    ih->data->plots[p]->Paint(1, 0); /* no flush here */
  }
  iPPlotSetPlotCurrent(ih, old_current);

  ih->data->cd_canvas = old_cddbuffer;

  iPPlotUpdateSizes(ih);
}

/* --------------------------------------------------------------------
                      class implementation
   -------------------------------------------------------------------- */

PostPainterCallbackIup::PostPainterCallbackIup (PPlot &inPPlot, Ihandle* inHandle):
  _ih(inHandle)
{
  inPPlot.mPostDrawerList.push_back (this);
}

bool PostPainterCallbackIup::Draw(Painter &inPainter)
{
  IFnC cb = (IFnC)IupGetCallback(_ih, "POSTDRAW_CB");
  if (cb)
    cb(_ih, _ih->data->cd_canvas);

  return true;
}

PrePainterCallbackIup::PrePainterCallbackIup (PPlot &inPPlot, Ihandle* inHandle):
  _ih(inHandle)
{
  inPPlot.mPreDrawerList.push_back (this);
}

bool PrePainterCallbackIup::Draw(Painter &inPainter)
{
  IFnC cb = (IFnC)IupGetCallback(_ih, "PREDRAW_CB");
  if (cb)
    cb(_ih, _ih->data->cd_canvas);

  return true;
}

bool PDeleteInteractionIup::DeleteNotify(int inIndex, int inSampleIndex, PlotDataBase* inXData, PlotDataBase* inYData)
{
  IFniiff cb = (IFniiff)IupGetCallback(_ih, "DELETE_CB");
  if (cb)
  {
    if (inIndex == -1)
    {
      Icallback cbb = IupGetCallback(_ih, "DELETEBEGIN_CB");
      if (cbb && cbb(_ih) == IUP_IGNORE)
        return false;
    }
    else if (inIndex == -2)
    {
      Icallback cbb = IupGetCallback(_ih, "DELETEEND_CB");
      if (cbb)
        cbb(_ih);
    }
    else
    {
      float theX = inXData->GetValue(inSampleIndex);
      float theY = inYData->GetValue(inSampleIndex);
      int ret = cb(_ih, inIndex, inSampleIndex, theX, theY);
      if (ret == IUP_IGNORE)
        return false;
    }
  }

  return true;
}

bool PSelectionInteractionIup::SelectNotify(int inIndex, int inSampleIndex, PlotDataBase* inXData, PlotDataBase* inYData, bool inSelect)
{
  IFniiffi cb = (IFniiffi)IupGetCallback(_ih, "SELECT_CB");
  if (cb)
  {
    if (inIndex == -1)
    {
      Icallback cbb = IupGetCallback(_ih, "SELECTBEGIN_CB");
      if (cbb && cbb(_ih) == IUP_IGNORE)
        return false;
    }
    else if (inIndex == -2)
    {
      Icallback cbb = IupGetCallback(_ih, "SELECTEND_CB");
      if (cbb)
        cbb(_ih);
    }
    else
    {
      float theX = inXData->GetValue(inSampleIndex);
      float theY = inYData->GetValue(inSampleIndex);
      int ret = cb(_ih, inIndex, inSampleIndex, theX, theY, (int)inSelect);
      if (ret == IUP_IGNORE)
        return false;
    }
  }

  return true;
}

bool PEditInteractionIup::Impl_HandleKeyEvent (const PKeyEvent &inEvent)
{
  if (inEvent.IsArrowDown () || inEvent.IsArrowUp () ||
      inEvent.IsArrowLeft () || inEvent.IsArrowRight ())
    return true;

  return false;
};

bool PEditInteractionIup::Impl_Calculate (Painter &inPainter, PPlot& inPPlot)
{
  PlotDataContainer &theContainer = inPPlot.mPlotDataContainer;
  long thePlotCount = theContainer.GetPlotCount();

  if (!EditNotify(-1, 0, 0, 0, NULL, NULL))
    return false;

  for (long theI=0;theI<thePlotCount;theI++)
  {
    PlotDataBase* theXData = theContainer.GetXData (theI);
    PlotDataBase* theYData = theContainer.GetYData (theI);
    PlotDataSelection *thePlotDataSelection = theContainer.GetPlotDataSelection (theI);

    if (mKeyEvent.IsArrowDown () || mKeyEvent.IsArrowUp () ||
        mKeyEvent.IsArrowLeft () || mKeyEvent.IsArrowRight ())
      HandleCursorKey (thePlotDataSelection, theXData, theYData, theI);
  }

  EditNotify(-2, 0, 0, 0, NULL, NULL);

  return true;
}

void PEditInteractionIup::HandleCursorKey (const PlotDataSelection *inPlotDataSelection, PlotDataBase* inXData, PlotDataBase* inYData, int inIndex)
{
  float theXDelta = 0; /* pixels */
  if (mKeyEvent.IsArrowLeft () || mKeyEvent.IsArrowRight ())
  {
    theXDelta = 1;

    if (mKeyEvent.IsArrowLeft ())
      theXDelta *= -1;

    if (mKeyEvent.IsOnlyControlKeyDown ())
      theXDelta *= 10;
  }

  float theYDelta = 0; /* pixels */
  if (mKeyEvent.IsArrowDown () || mKeyEvent.IsArrowUp ())
  {
    theYDelta = 1;

    if (mKeyEvent.IsArrowDown ())
      theYDelta *= -1;

    if (mKeyEvent.IsOnlyControlKeyDown ())
      theYDelta *= 10;
  }

  for (int theI=0;theI<inYData->GetSize ();theI++)
  {
    if (inPlotDataSelection->IsSelected (theI))
    {
      float theX = inXData->GetValue(theI);
      float newX = theX;

      if (theXDelta)
      {
        float theXPixels = mPPlot.mXTrafo->Transform(theX);
        theXPixels += theXDelta;
        newX = mPPlot.mXTrafo->TransformBack(theXPixels);
      }

      float theY = inYData->GetValue(theI);
      float newY = theY;
      if (theYDelta)
      {
        float theYPixels = mPPlot.mYTrafo->Transform(theY);
        theYPixels -= theYDelta;  /* in pixels Y is descending */
        newY = mPPlot.mYTrafo->TransformBack(theYPixels);
      }

      if (!EditNotify(inIndex, theI, theX, theY, &newX, &newY))
        return;

      if (inXData->IsString())
      {
        StringPlotData *theXData = (StringPlotData*)(inXData);
        PlotData* theYData = (PlotData*)(inYData);
        theXData->mRealPlotData[theI] = newX;
        (*theYData)[theI] = newY;
      }
      else
      {
        PlotData* theXData = (PlotData*)(inXData);
        PlotData* theYData = (PlotData*)(inYData);
        (*theXData)[theI] = newX;
        (*theYData)[theI] = newY;
      }
    }
  }
}

bool PEditInteractionIup::EditNotify(int inIndex, int inSampleIndex, float inX, float inY, float *inNewX, float *inNewY)
{
  IFniiffFF cb = (IFniiffFF)IupGetCallback(_ih, "EDIT_CB");
  if (cb)
  {
    if (inIndex == -1)
    {
      Icallback cbb = IupGetCallback(_ih, "EDITBEGIN_CB");
      if (cbb && cbb(_ih) == IUP_IGNORE)
        return false;
    }
    else if (inIndex == -2)
    {
      Icallback cbb = IupGetCallback(_ih, "EDITEND_CB");
      if (cbb)
        cbb(_ih);
    }
    else
    {
      int ret = cb(_ih, inIndex, inSampleIndex, inX, inY, inNewX, inNewY);
      if (ret == IUP_IGNORE)
        return false;
    }
  }

  return true;
}

InteractionContainerIup::InteractionContainerIup(PPlot &inPPlot, Ihandle* inHandle):
  mZoomInteraction (inPPlot),
  mSelectionInteraction (inPPlot, inHandle),
  mEditInteraction (inPPlot, inHandle),
  mDeleteInteraction (inPPlot, inHandle),
  mCrosshairInteraction (inPPlot),
  mPostPainterCallback(inPPlot, inHandle),
  mPrePainterCallback(inPPlot, inHandle)
{
  AddInteraction (mZoomInteraction);
  AddInteraction (mSelectionInteraction);
  AddInteraction (mEditInteraction);
  AddInteraction (mDeleteInteraction);
  AddInteraction (mCrosshairInteraction);
}

PPainterIup::PPainterIup(Ihandle *ih) : 
  Painter(),
  _ih(ih),
  _mouseDown(0),
  _currentDataSetIndex(-1),
  _redraw(1)
{
  _plot.mShowLegend = false; /* change default to hidden */
  _plot.mPlotBackground.mTransparent = false;  /* always draw the background */
  _plot.mMargins.mLeft = 15;
  _plot.mMargins.mBottom = 15;
  _plot.mMargins.mTop = 30;
  _plot.mMargins.mRight = 15;
  _plot.mXAxisSetup.mTickInfo.mTickDivision = 5;
  _plot.mYAxisSetup.mTickInfo.mTickDivision = 5;
  _plot.mXAxisSetup.mTickInfo.mMinorTickScreenSize = 5;
  _plot.mYAxisSetup.mTickInfo.mMinorTickScreenSize = 5;
  _plot.mXAxisSetup.mTickInfo.mMajorTickScreenSize = 8;
  _plot.mYAxisSetup.mTickInfo.mMajorTickScreenSize = 8;

  _InteractionContainer = new InteractionContainerIup(_plot, _ih);

} /* c-tor */


PPainterIup::~PPainterIup()
{
  delete _InteractionContainer;
}

class MarkDataDrawer: public LineDataDrawer
{
 public:
   MarkDataDrawer (bool inDrawLine, Ihandle* inHandle)
   {
     mDrawLine = inDrawLine;
     mDrawPoint = true;
     mMode = inDrawLine ? "MARKLINE" : "MARK";
     _ih = inHandle;
   };
   virtual bool DrawPoint (int inScreenX, int inScreenY, const PRect &inRect, Painter &inPainter) const;
   virtual DataDrawerBase* Clone() const;
protected:
  Ihandle *_ih;   // IUP handle
};

DataDrawerBase* MarkDataDrawer::Clone() const {
  return new MarkDataDrawer(*this);
}

bool MarkDataDrawer::DrawPoint (int inScreenX, int inScreenY, const PRect &inRect, Painter &inPainter) const
{
  cdCanvasMark(_ih->data->cd_canvas, inScreenX, cdCanvasInvertYAxis(_ih->data->cd_canvas, inScreenY));
  return true;
}

static void RemoveSample(PPlot& inPPlot, int inIndex, int inSampleIndex)
{
  PlotDataBase* theXDataBase = inPPlot.mPlotDataContainer.GetXData(inIndex);
  PlotDataBase* theYDataBase = inPPlot.mPlotDataContainer.GetYData(inIndex);

  if (theXDataBase->IsString())
  {
    StringPlotData *theXData = (StringPlotData *)theXDataBase;
    PlotData* theYData = (PlotData*)theYDataBase;
    theXData->mRealPlotData.erase(theXData->mRealPlotData.begin()+inSampleIndex);
    theXData->mStringPlotData.erase(theXData->mStringPlotData.begin()+inSampleIndex);
    theYData->erase(theYData->begin()+inSampleIndex);
  }
  else
  {
    PlotData* theXData = (PlotData*)theXDataBase;
    PlotData* theYData = (PlotData*)theYDataBase;
    theXData->erase(theXData->begin()+inSampleIndex);
    theYData->erase(theYData->begin()+inSampleIndex);
  }
}

/* --------------------------------------------------------------------
                      CD Gets - size and style
   -------------------------------------------------------------------- */

static int iPPlotGetCDFontStyle(const char* value)
{
  if (!value)
    return -1;
  if (iupStrEqualNoCase(value, "PLAIN"))
    return CD_PLAIN;
  if (iupStrEqualNoCase(value, "BOLD"))
    return CD_BOLD;
  if (iupStrEqualNoCase(value, "ITALIC"))
    return CD_ITALIC;
  if (iupStrEqualNoCase(value, "BOLDITALIC"))
    return CD_BOLD_ITALIC;
  return -1;
}

static char* iPPlotGetPlotFontSize(int size)
{
  if (size)
    return iupStrReturnInt(size);
  else
    return NULL;
}

static char* iPPlotGetPlotFontStyle(int style)
{
  if (style >= CD_PLAIN && style <= CD_BOLD_ITALIC)
  {
    const char* style_str[4] = {"PLAIN", "BOLD", "ITALIC", "BOLDITALIC"};
    return (char*)style_str[style];
  }
  else
    return NULL;
}

static char* iPPlotGetPlotPenStyle(int style)
{
  if (style >= CD_CONTINUOUS && style <= CD_DASH_DOT_DOT)
  {
    const char* style_str[5] = {"CONTINUOUS", "DASHED", "DOTTED", "DASH_DOT", "DASH_DOT_DOT"};
    return (char*)style_str[style];
  }
  else
    return NULL;
}

static int iPPlotGetCDPenStyle(const char* value)
{
  if (!value || iupStrEqualNoCase(value, "CONTINUOUS"))
    return CD_CONTINUOUS;
  else if (iupStrEqualNoCase(value, "DASHED"))
    return CD_DASHED;
  else if (iupStrEqualNoCase(value, "DOTTED"))
    return CD_DOTTED;
  else if (iupStrEqualNoCase(value, "DASH_DOT"))
    return CD_DASH_DOT;
  else if (iupStrEqualNoCase(value, "DASH_DOT_DOT"))
    return CD_DASH_DOT_DOT;
  else
    return CD_CONTINUOUS;
}
 
static char* iPPlotGetPlotMarkStyle(int style)
{
  if (style >= CD_PLUS && style <= CD_HOLLOW_DIAMOND)
  {
    const char* style_str[9] = {"PLUS", "STAR", "CIRCLE", "X", "BOX", "DIAMOND", "HOLLOW_CIRCLE", "HOLLOW_BOX", "HOLLOW_DIAMOND"};
    return (char*)style_str[style];
  }
  else
    return NULL;
}

static int iPPlotGetCDMarkStyle(const char* value)
{
  if (!value || iupStrEqualNoCase(value, "PLUS"))
    return CD_PLUS;
  else if (iupStrEqualNoCase(value, "STAR"))
    return CD_STAR;
  else if (iupStrEqualNoCase(value, "CIRCLE"))
    return CD_CIRCLE;
  else if (iupStrEqualNoCase(value, "X"))
    return CD_X;
  else if (iupStrEqualNoCase(value, "BOX"))
    return CD_BOX;
  else if (iupStrEqualNoCase(value, "DIAMOND"))
    return CD_DIAMOND;
  else if (iupStrEqualNoCase(value, "HOLLOW_CIRCLE"))
    return CD_HOLLOW_CIRCLE;
  else if (iupStrEqualNoCase(value, "HOLLOW_BOX"))
    return CD_HOLLOW_BOX;
  else if (iupStrEqualNoCase(value, "HOLLOW_DIAMOND"))
    return CD_HOLLOW_DIAMOND;
  else
    return CD_PLUS;
}

/*****************************************************************************/
/***** SET AND GET ATTRIBUTES ************************************************/
/*****************************************************************************/

static int iPPlotSetRedrawAttrib(Ihandle* ih, const char* value)
{
  int flush = 1, 
    current = 0;

  if (!ih->data->cd_canvas)
    return IUP_DEFAULT;

  if (iupStrEqualNoCase(value, "NOFLUSH"))
    flush = 0;
  else if (iupStrEqualNoCase(value, "CURRENT"))
  {
    flush = 0;
    current = 1;
  }

  if (current)
  {
    ih->data->plt->Paint(1, 0);
  }
  else
  {
    int old_current = ih->data->plt_index;
    for (int p = 0; p < ih->data->plots_count; p++)
    {
      iPPlotSetPlotCurrent(ih, p);
      ih->data->plt->Paint(1, 0);
    }
    iPPlotSetPlotCurrent(ih, old_current);
  }

  if (ih->data->plots_count == 1 &&
      ih->data->plt->_plot.mPlotDataContainer.GetPlotCount() == 0)
    cdCanvasClear(ih->data->cd_canvas);

  // Do the flush once
  if (flush)
    iPPlotFlush(ih);

  (void)value;  /* not used */
  return 0;
}

static char* iPPlotGetCountAttrib(Ihandle* ih)
{
  return iupStrReturnInt(ih->data->plt->_plot.mPlotDataContainer.GetPlotCount());
}

static int iPPlotSetLegendShowAttrib(Ihandle* ih, const char* value)
{
  if (iupStrBoolean(value))
    ih->data->plt->_plot.mShowLegend = true;
  else 
    ih->data->plt->_plot.mShowLegend = false;

  ih->data->plt->_redraw = 1;
  return 0;
}

static char* iPPlotGetLegendShowAttrib(Ihandle* ih)
{
  return iupStrReturnBoolean (ih->data->plt->_plot.mShowLegend); 
}

static int iPPlotSetLegendPosAttrib(Ihandle* ih, const char* value)
{
  if (iupStrEqualNoCase(value, "TOPLEFT"))
    ih->data->plt->_plot.mLegendPos = PPLOT_TOPLEFT;
  else if (iupStrEqualNoCase(value, "BOTTOMLEFT"))
    ih->data->plt->_plot.mLegendPos = PPLOT_BOTTOMLEFT;
  else if (iupStrEqualNoCase(value, "BOTTOMRIGHT"))
    ih->data->plt->_plot.mLegendPos = PPLOT_BOTTOMRIGHT;
  else if (iupStrEqualNoCase(value, "BOTTOMCENTER"))
    ih->data->plt->_plot.mLegendPos = PPLOT_BOTTOMCENTER;
  else
    ih->data->plt->_plot.mLegendPos = PPLOT_TOPRIGHT;

  ih->data->plt->_redraw = 1;
  return 0;
}

static char* iPPlotGetLegendPosAttrib(Ihandle* ih)
{
  const char* legendpos_str[4] = {"TOPLEFT", "TOPRIGHT", "BOTTOMLEFT", "BOTTOMRIGHT"};
  return (char*)legendpos_str[ih->data->plt->_plot.mLegendPos];
}

static int iPPlotSetBGColorAttrib(Ihandle* ih, const char* value)
{
  unsigned char rr, gg, bb;
  if (iupStrToRGB(value, &rr, &gg, &bb))
  {
    ih->data->plt->_plot.mPlotBackground.mPlotRegionBackColor = PColor(rr, gg, bb);
    ih->data->plt->_redraw = 1;
  }
  return 0;
}

static char* iPPlotGetBGColorAttrib(Ihandle* ih)
{
  return iupStrReturnRGB(
          ih->data->plt->_plot.mPlotBackground.mPlotRegionBackColor.mR,
          ih->data->plt->_plot.mPlotBackground.mPlotRegionBackColor.mG,
          ih->data->plt->_plot.mPlotBackground.mPlotRegionBackColor.mB);
}

static int iPPlotSetFGColorAttrib(Ihandle* ih, const char* value)
{
  unsigned char rr, gg, bb;
  if (iupStrToRGB(value, &rr, &gg, &bb))
  {
    ih->data->plt->_plot.mPlotBackground.mTitleColor = PColor(rr, gg, bb);
    ih->data->plt->_redraw = 1;
  }
  return 0;
}

static char* iPPlotGetFGColorAttrib(Ihandle* ih)
{
  return iupStrReturnRGB(
          ih->data->plt->_plot.mPlotBackground.mTitleColor.mR,
          ih->data->plt->_plot.mPlotBackground.mTitleColor.mG,
          ih->data->plt->_plot.mPlotBackground.mTitleColor.mB);
}

static int iPPlotSetTitleAttrib(Ihandle* ih, const char* value)
{
  if (value && value[0] != 0)
    ih->data->plt->_plot.mPlotBackground.mTitle = value;
  else
    ih->data->plt->_plot.mPlotBackground.mTitle.resize(0);

  ih->data->plt->_redraw = 1;
  return 0;
}

static char* iPPlotGetTitleAttrib(Ihandle* ih)
{
  return iupStrReturnStr(ih->data->plt->_plot.mPlotBackground.mTitle.c_str());
}

static int iPPlotSetTitleFontSizeAttrib(Ihandle* ih, const char* value)
{
  int ii;
  if (iupStrToInt(value, &ii))
  {
    ih->data->plt->_plot.mPlotBackground.mStyle.mFontSize = ii;
    ih->data->plt->_redraw = 1;
  }
  return 0;
}

static char* iPPlotGetTitleFontSizeAttrib(Ihandle* ih)
{
  return iPPlotGetPlotFontSize(ih->data->plt->_plot.mPlotBackground.mStyle.mFontSize);
}

static int iPPlotSetTitleFontStyleAttrib(Ihandle* ih, const char* value)
{
  int style = iPPlotGetCDFontStyle(value);
  if (style != -1)
  {
    ih->data->plt->_plot.mPlotBackground.mStyle.mFontStyle = style;
    ih->data->plt->_redraw = 1;
  }
  return 0;
}

static int iPPlotSetLegendFontSizeAttrib(Ihandle* ih, const char* value)
{
  int ii, xx;
  if (!iupStrToInt(value, &xx))
    return 0;

  for (ii = 0; ii < ih->data->plt->_plot.mPlotDataContainer.GetPlotCount(); ii++)
  {
    LegendData* legend = ih->data->plt->_plot.mPlotDataContainer.GetLegendData(ii);
    legend->mStyle.mFontSize = xx;
  }

  ih->data->plt->_redraw = 1;
  return 1;
}

static int iPPlotSetLegendFontStyleAttrib(Ihandle* ih, const char* value)
{
  int ii;
  int style = iPPlotGetCDFontStyle(value);
  if (style == -1)
    return 0;

  for (ii = 0; ii < ih->data->plt->_plot.mPlotDataContainer.GetPlotCount(); ii++)
  {
    LegendData* legend = ih->data->plt->_plot.mPlotDataContainer.GetLegendData(ii);
    legend->mStyle.mFontStyle = style;
  }

  ih->data->plt->_redraw = 1;
  return 1;
}

static int iPPlotSetMarginLeftAttrib(Ihandle* ih, const char* value)
{
  int ii;
  if (iupStrToInt(value, &ii))
  {
    ih->data->plt->_plot.mMargins.mLeft = ii;
    ih->data->plt->_redraw = 1;
  }
  return 0;
}

static int iPPlotSetMarginRightAttrib(Ihandle* ih, const char* value)
{
  int ii;
  if (iupStrToInt(value, &ii))
  {
    ih->data->plt->_plot.mMargins.mRight = ii;
    ih->data->plt->_redraw = 1;
  }
  return 0;
}

static int iPPlotSetMarginTopAttrib(Ihandle* ih, const char* value)
{
  int ii;
  if (iupStrToInt(value, &ii))
  {
    ih->data->plt->_plot.mMargins.mTop = ii;
    ih->data->plt->_redraw = 1;
  }
  return 0;
}

static int iPPlotSetMarginBottomAttrib(Ihandle* ih, const char* value)
{
  int ii;
  if (iupStrToInt(value, &ii))
  {
    ih->data->plt->_plot.mMargins.mBottom = ii;
    ih->data->plt->_redraw = 1;
  }
  return 0;
}

static char* iPPlotGetMarginLeftAttrib(Ihandle* ih)
{
  return iupStrReturnInt(ih->data->plt->_plot.mMargins.mLeft);
}

static char* iPPlotGetMarginRightAttrib(Ihandle* ih)
{
  return iupStrReturnInt(ih->data->plt->_plot.mMargins.mRight);
}

static char* iPPlotGetMarginTopAttrib(Ihandle* ih)
{
  return iupStrReturnInt(ih->data->plt->_plot.mMargins.mTop);
}

static char* iPPlotGetMarginBottomAttrib(Ihandle* ih)
{
  return iupStrReturnInt(ih->data->plt->_plot.mMargins.mBottom);
}

static int iPPlotSetGridColorAttrib(Ihandle* ih, const char* value)
{
  unsigned char rr, gg, bb;
  if (iupStrToRGB(value, &rr, &gg, &bb))
  {
    ih->data->plt->_plot.mGridInfo.mGridColor = PColor(rr, gg, bb);
    ih->data->plt->_redraw = 1;
  }
  return 0;
}

static char* iPPlotGetGridColorAttrib(Ihandle* ih)
{
  return iupStrReturnRGB(
          ih->data->plt->_plot.mGridInfo.mGridColor.mR,
          ih->data->plt->_plot.mGridInfo.mGridColor.mG,
          ih->data->plt->_plot.mGridInfo.mGridColor.mB);
}

static int iPPlotSetGridLineStyleAttrib(Ihandle* ih, const char* value)
{
  ih->data->plt->_plot.mGridInfo.mStyle.mPenStyle = iPPlotGetCDPenStyle(value);
  ih->data->plt->_redraw = 1;
  return 0;
}

static char* iPPlotGetGridLineStyleAttrib(Ihandle* ih)
{
  return iPPlotGetPlotPenStyle(ih->data->plt->_plot.mGridInfo.mStyle.mPenStyle);
}

static int iPPlotSetGridAttrib(Ihandle* ih, const char* value)
{
  if (iupStrEqualNoCase(value, "VERTICAL"))  /* vertical grid - X axis  */
  {
    ih->data->plt->_plot.mGridInfo.mXGridOn = true;
    ih->data->plt->_plot.mGridInfo.mYGridOn = false;
  }
  else if (iupStrEqualNoCase(value, "HORIZONTAL")) /* horizontal grid - Y axis */
  {
    ih->data->plt->_plot.mGridInfo.mYGridOn = true;
    ih->data->plt->_plot.mGridInfo.mXGridOn = false;
  }
  else if (iupStrEqualNoCase(value, "YES"))
  {
    ih->data->plt->_plot.mGridInfo.mXGridOn = true;
    ih->data->plt->_plot.mGridInfo.mYGridOn = true;
  }
  else
  {
    ih->data->plt->_plot.mGridInfo.mYGridOn = false;
    ih->data->plt->_plot.mGridInfo.mXGridOn = false;
  }

  ih->data->plt->_redraw = 1;
  return 0;
}

static char* iPPlotGetGridAttrib(Ihandle* ih)
{
  if (ih->data->plt->_plot.mGridInfo.mXGridOn && ih->data->plt->_plot.mGridInfo.mYGridOn)
    return "YES";
  else if (ih->data->plt->_plot.mGridInfo.mYGridOn)
    return "HORIZONTAL";
  else if (ih->data->plt->_plot.mGridInfo.mXGridOn)
    return "VERTICAL";
  else
    return "NO";
}

static int iPPlotSetCurrentAttrib(Ihandle* ih, const char* value)
{
  int ii;
  if (iupStrToInt(value, &ii))
  {
    int imax = ih->data->plt->_plot.mPlotDataContainer.GetPlotCount();
    ih->data->plt->_currentDataSetIndex = ( (ii>=0) && (ii<imax) ? ii : -1);
    ih->data->plt->_redraw = 1;
  }
  else
  {
    ii = ih->data->plt->_plot.mPlotDataContainer.GetPlotIndexByName(value);
    if (ii != -1)
    {
      int imax = ih->data->plt->_plot.mPlotDataContainer.GetPlotCount();
      ih->data->plt->_currentDataSetIndex = ( (ii>=0) && (ii<imax) ? ii : -1);
      ih->data->plt->_redraw = 1;
    }
  }
  return 0;
}

static char* iPPlotGetCurrentAttrib(Ihandle* ih)
{
  return iupStrReturnInt(ih->data->plt->_currentDataSetIndex);
}

static int iPPlotSetPlotCurrentAttrib(Ihandle* ih, const char* value)
{
  int i;
  if (iupStrToInt(value, &i))
  {
    if (i>=0 && i<ih->data->plots_count)
      iPPlotSetPlotCurrent(ih, i);
  }
  else
  {
    for (i=0; i<ih->data->plots_count; i++)
    {
      const char* title = ih->data->plots[i]->_plot.mPlotBackground.mTitle.c_str();
      if (iupStrEqual(title, value))
      {
        iPPlotSetPlotCurrent(ih, i);
        return 0;
      }
    }
  }
  return 0;
}

static char* iPPlotGetPlotCurrentAttrib(Ihandle* ih)
{
  return iupStrReturnInt(ih->data->plt_index);
}

static char* iPPlotGetPlotCountAttrib(Ihandle* ih)
{
  return iupStrReturnInt(ih->data->plots_count);
}

static int iPPlotSetPlotCountAttrib(Ihandle* ih, const char* value)
{
  int count;
  if (iupStrToInt(value, &count))
  {
    if (count>0 && count<MAX_PLOTS)
    {
      if (count != ih->data->plots_count)
      {
        if (count < ih->data->plots_count)
        {
          // Remove at the end
          if (ih->data->plt_index >= count)
            iPPlotSetPlotCurrent(ih, count-1);

          for (int i=count; i<ih->data->plots_count; i++)
          {
            delete ih->data->plots[i];
            ih->data->plots[i] = NULL;
          }
        }
        else
        {
          // Add at the end
          for (int i=ih->data->plots_count; i<count; i++)
            ih->data->plots[i] = new PPainterIup(ih);
        }
      }

      ih->data->plots_count = count;

      iPPlotUpdateSizes(ih);
    }
  }
  return 0;
}

static void iPPlotPlotInsert(Ihandle* ih, int p)
{
  for (int i=ih->data->plots_count; i>p; i--)
    ih->data->plots[i] = ih->data->plots[i-1];

  ih->data->plots[p] = new PPainterIup(ih);

  ih->data->plots_count++;

  iPPlotUpdateSizes(ih);
}

static int iPPlotSetPlotInsertAttrib(Ihandle* ih, const char* value)
{
  if (!value)
  {
    if (ih->data->plots_count<MAX_PLOTS)
    {
      // Insert at the end (append)
      iPPlotPlotInsert(ih, ih->data->plots_count);
      iPPlotSetPlotCurrent(ih, ih->data->plots_count-1);
    }
  }
  else
  {
    // Insert before reference
    int i;
    if (iupStrToInt(value, &i))
    {
      if (i>=0 && i<ih->data->plots_count)
      {
        iPPlotPlotInsert(ih, i);
        iPPlotSetPlotCurrent(ih, ih->data->plots_count-1);
      }
    }
    else
    {
      for (i=0; i<ih->data->plots_count; i++)
      {
        const char* title = ih->data->plots[i]->_plot.mPlotBackground.mTitle.c_str();
        if (iupStrEqual(title, value))
        {
          iPPlotPlotInsert(ih, i);
          iPPlotSetPlotCurrent(ih, ih->data->plots_count-1);
          return 0;
        }
      }
    }
  }
  return 0;
}

static char* iPPlotGetPlotNumColAttrib(Ihandle* ih)
{
  return iupStrReturnInt(ih->data->plots_numcol);
}

static int iPPlotSetPlotNumColAttrib(Ihandle* ih, const char* value)
{
  int plots_numcol;
  if (iupStrToInt(value, &plots_numcol))
  {
    if (plots_numcol > 0)
    {
      ih->data->plots_numcol = plots_numcol;

      iPPlotUpdateSizes(ih);
    }
  }
  return 0;
}

static void iPPlotPlotRemove(Ihandle* ih, int p)
{
  if (ih->data->plt_index == ih->data->plots_count-1)
    ih->data->plt_index--;

  delete ih->data->plots[p];

  for (int i=p; i<ih->data->plots_count; i++)
    ih->data->plots[i] = ih->data->plots[i+1];

  ih->data->plots[ih->data->plots_count-1] = NULL;

  ih->data->plots_count--;

  iPPlotSetPlotCurrent(ih, ih->data->plt_index);

  iPPlotUpdateSizes(ih);
}

static int iPPlotSetPlotRemoveAttrib(Ihandle* ih, const char* value)
{
  if (ih->data->plots_count == 1)
    return 0;

  if (!value)
  {
    iPPlotPlotRemove(ih, ih->data->plt_index);
    return 0;
  }

  int i;
  if (iupStrToInt(value, &i))
  {
    if (i>=0 && i<ih->data->plots_count)
      iPPlotPlotRemove(ih, i);
  }
  else
  {
    for (i=0; i<ih->data->plots_count; i++)
    {
      const char* title = ih->data->plots[i]->_plot.mPlotBackground.mTitle.c_str();
      if (iupStrEqual(title, value))
      {
        iPPlotPlotRemove(ih, i);
        return 0;
      }
    }
  }
  return 0;
}

static int iPPlotSetSyncViewAttrib(Ihandle* ih, const char* value)
{
  ih->data->sync_view = iupStrBoolean(value);
  return 0;
}

static char* iPPlotGetSyncViewAttrib(Ihandle* ih)
{
  return iupStrReturnBoolean(ih->data->sync_view);
}
                                    
static char* iPPlotGetCanvasAttrib(Ihandle* ih)
{
  return (char*)(ih->data->cd_canvas);
}

static void iPPlotCheckCurrentDataSet(Ihandle* ih)
{
  int count = ih->data->plt->_plot.mPlotDataContainer.GetPlotCount();
  if (ih->data->plt->_currentDataSetIndex == count)
    ih->data->plt->_currentDataSetIndex--;
}

static int iPPlotSetRemoveAttrib(Ihandle* ih, const char* value)
{
  if (!value)
  {
    ih->data->plt->_plot.mPlotDataContainer.RemoveElement(ih->data->plt->_currentDataSetIndex);
    ih->data->plt->_redraw = 1;
    iPPlotCheckCurrentDataSet(ih);
    return 0;
  }

  int ii;
  if (iupStrToInt(value, &ii))
  {
    ih->data->plt->_plot.mPlotDataContainer.RemoveElement(ii);
    ih->data->plt->_redraw = 1;
    iPPlotCheckCurrentDataSet(ih);
  }
  else
  {
    ii = ih->data->plt->_plot.mPlotDataContainer.GetPlotIndexByName(value);
    if (ii != -1)
    {
      ih->data->plt->_plot.mPlotDataContainer.RemoveElement(ii);
      ih->data->plt->_redraw = 1;
      iPPlotCheckCurrentDataSet(ih);
    }
  }
  return 0;
}

static int iPPlotSetClearAttrib(Ihandle* ih, const char* value)
{
  ih->data->plt->_plot.mPlotDataContainer.ClearData();
  ih->data->plt->_redraw = 1;
  return 0;
}

/* =============================== */
/* current plot dataset attributes */
/* =============================== */

static int iPPlotSetDSLineStyleAttrib(Ihandle* ih, const char* value)
{
  if (ih->data->plt->_currentDataSetIndex <  0 ||
      ih->data->plt->_currentDataSetIndex >= ih->data->plt->_plot.mPlotDataContainer.GetPlotCount())
    return 0;
  
  DataDrawerBase* drawer = ih->data->plt->_plot.mPlotDataContainer.GetDataDrawer(ih->data->plt->_currentDataSetIndex);
  drawer->mStyle.mPenStyle = iPPlotGetCDPenStyle(value);

  ih->data->plt->_redraw = 1;
  return 0;
}

static char* iPPlotGetDSLineStyleAttrib(Ihandle* ih)
{
  if (ih->data->plt->_currentDataSetIndex < 0 ||
      ih->data->plt->_currentDataSetIndex >= ih->data->plt->_plot.mPlotDataContainer.GetPlotCount())
    return NULL;

  DataDrawerBase* drawer = ih->data->plt->_plot.mPlotDataContainer.GetDataDrawer(ih->data->plt->_currentDataSetIndex);

  return iPPlotGetPlotPenStyle(drawer->mStyle.mPenStyle);
}

static int iPPlotSetDSLineWidthAttrib(Ihandle* ih, const char* value)
{
  int ii;

  if (ih->data->plt->_currentDataSetIndex <  0 ||
      ih->data->plt->_currentDataSetIndex >= ih->data->plt->_plot.mPlotDataContainer.GetPlotCount())
    return 0;

  if (iupStrToInt(value, &ii))
  {
    DataDrawerBase* drawer = ih->data->plt->_plot.mPlotDataContainer.GetDataDrawer(ih->data->plt->_currentDataSetIndex);
    drawer->mStyle.mPenWidth = ii;
    ih->data->plt->_redraw = 1;
  }
  return 0;
}

static char* iPPlotGetDSLineWidthAttrib(Ihandle* ih)
{
  if (ih->data->plt->_currentDataSetIndex < 0 ||
      ih->data->plt->_currentDataSetIndex >= ih->data->plt->_plot.mPlotDataContainer.GetPlotCount())
    return NULL;

  DataDrawerBase* drawer = ih->data->plt->_plot.mPlotDataContainer.GetDataDrawer(ih->data->plt->_currentDataSetIndex);
  return iupStrReturnInt(drawer->mStyle.mPenWidth);
}

static int iPPlotSetDSMarkStyleAttrib(Ihandle* ih, const char* value)
{
  if (ih->data->plt->_currentDataSetIndex <  0 ||
      ih->data->plt->_currentDataSetIndex >= ih->data->plt->_plot.mPlotDataContainer.GetPlotCount())
    return 0;
  
  DataDrawerBase* drawer = ih->data->plt->_plot.mPlotDataContainer.GetDataDrawer(ih->data->plt->_currentDataSetIndex);
  drawer->mStyle.mMarkStyle = iPPlotGetCDMarkStyle(value);
  ih->data->plt->_redraw = 1;
  return 0;
}

static char* iPPlotGetDSMarkStyleAttrib(Ihandle* ih)
{
  if (ih->data->plt->_currentDataSetIndex < 0 ||
      ih->data->plt->_currentDataSetIndex >= ih->data->plt->_plot.mPlotDataContainer.GetPlotCount())
    return NULL;

  DataDrawerBase* drawer = ih->data->plt->_plot.mPlotDataContainer.GetDataDrawer(ih->data->plt->_currentDataSetIndex);

  return iPPlotGetPlotMarkStyle(drawer->mStyle.mMarkStyle);
}

static int iPPlotSetDSMarkSizeAttrib(Ihandle* ih, const char* value)
{
  int ii;

  if (ih->data->plt->_currentDataSetIndex <  0 ||
      ih->data->plt->_currentDataSetIndex >= ih->data->plt->_plot.mPlotDataContainer.GetPlotCount())
    return 0;
  
  if (iupStrToInt(value, &ii))
  {
    DataDrawerBase* drawer = ih->data->plt->_plot.mPlotDataContainer.GetDataDrawer(ih->data->plt->_currentDataSetIndex);
    drawer->mStyle.mMarkSize = ii;
    ih->data->plt->_redraw = 1;
  }
  return 0;
}

static char* iPPlotGetDSMarkSizeAttrib(Ihandle* ih)
{
  if (ih->data->plt->_currentDataSetIndex < 0 ||
      ih->data->plt->_currentDataSetIndex >= ih->data->plt->_plot.mPlotDataContainer.GetPlotCount())
    return NULL;

  DataDrawerBase* drawer = ih->data->plt->_plot.mPlotDataContainer.GetDataDrawer(ih->data->plt->_currentDataSetIndex);
  return iupStrReturnInt(drawer->mStyle.mMarkSize);
}

static int iPPlotSetDSLegendAttrib(Ihandle* ih, const char* value)
{
  if (ih->data->plt->_currentDataSetIndex <  0 ||
      ih->data->plt->_currentDataSetIndex >= ih->data->plt->_plot.mPlotDataContainer.GetPlotCount())
    return 0;
  
  LegendData* legend = ih->data->plt->_plot.mPlotDataContainer.GetLegendData(ih->data->plt->_currentDataSetIndex);
    
  if (value)
    legend->mName = value;
  else
    legend->mName.resize(0);

  ih->data->plt->_redraw = 1;
  return 0;
}

static char* iPPlotGetDSLegendAttrib(Ihandle* ih)
{
  if (ih->data->plt->_currentDataSetIndex < 0 ||
      ih->data->plt->_currentDataSetIndex >= ih->data->plt->_plot.mPlotDataContainer.GetPlotCount())
    return NULL;

  LegendData* legend = ih->data->plt->_plot.mPlotDataContainer.GetLegendData(ih->data->plt->_currentDataSetIndex);
  return iupStrReturnStr(legend->mName.c_str());
}

static int iPPlotSetDSColorAttrib(Ihandle* ih, const char* value)
{
  unsigned char rr, gg, bb;

  if (ih->data->plt->_currentDataSetIndex <  0 ||
      ih->data->plt->_currentDataSetIndex >= ih->data->plt->_plot.mPlotDataContainer.GetPlotCount())
    return 0;

  if (iupStrToRGB(value, &rr, &gg, &bb))
  {
    LegendData* legend = ih->data->plt->_plot.mPlotDataContainer.GetLegendData(ih->data->plt->_currentDataSetIndex);
    legend->mColor = PColor(rr, gg, bb);
    ih->data->plt->_redraw = 1;
  }
  return 0;
}

static char* iPPlotGetDSColorAttrib(Ihandle* ih)
{
  if (ih->data->plt->_currentDataSetIndex < 0 ||
      ih->data->plt->_currentDataSetIndex >= ih->data->plt->_plot.mPlotDataContainer.GetPlotCount())
    return NULL;

  LegendData* legend = ih->data->plt->_plot.mPlotDataContainer.GetLegendData(ih->data->plt->_currentDataSetIndex);
  return iupStrReturnRGB(legend->mColor.mR, legend->mColor.mG, legend->mColor.mB);
}

static int iPPlotSetDSShowValuesAttrib(Ihandle* ih, const char* value)
{
  if (ih->data->plt->_currentDataSetIndex <  0 ||
      ih->data->plt->_currentDataSetIndex >= ih->data->plt->_plot.mPlotDataContainer.GetPlotCount())
    return 0;
 
  DataDrawerBase* drawer = ih->data->plt->_plot.mPlotDataContainer.GetDataDrawer(ih->data->plt->_currentDataSetIndex);
    
  if (iupStrBoolean(value))
    drawer->mShowValues = true;
  else 
    drawer->mShowValues = false;

  ih->data->plt->_redraw = 1;
  return 0;
}

static char* iPPlotGetDSShowValuesAttrib(Ihandle* ih)
{
  if (ih->data->plt->_currentDataSetIndex < 0 ||
      ih->data->plt->_currentDataSetIndex >= ih->data->plt->_plot.mPlotDataContainer.GetPlotCount())
    return NULL;

  DataDrawerBase* drawer = ih->data->plt->_plot.mPlotDataContainer.GetDataDrawer(ih->data->plt->_currentDataSetIndex);
  return iupStrReturnBoolean (drawer->mShowValues); 
}

static int iPPlotSetDSModeAttrib(Ihandle* ih, const char* value)
{
  if (ih->data->plt->_currentDataSetIndex <  0 ||
      ih->data->plt->_currentDataSetIndex >= ih->data->plt->_plot.mPlotDataContainer.GetPlotCount())
    return 0;
  
  DataDrawerBase *theDataDrawer = NULL;
  ih->data->plt->_plot.mXAxisSetup.mDiscrete = false;
    
  if(iupStrEqualNoCase(value, "BAR"))
  {
    theDataDrawer = new BarDataDrawer();
    ih->data->plt->_plot.mXAxisSetup.mDiscrete = true;
  }
  else if (iupStrEqualNoCase(value, "AREA"))
    theDataDrawer = new AreaDataDrawer();
  else if (iupStrEqualNoCase(value, "MARK"))
    theDataDrawer = new MarkDataDrawer(0, ih);
  else if(iupStrEqualNoCase(value, "MARKLINE"))
    theDataDrawer = new MarkDataDrawer(1, ih);
  else  /* LINE */
    theDataDrawer = new LineDataDrawer();

  ih->data->plt->_plot.mPlotDataContainer.SetDataDrawer(ih->data->plt->_currentDataSetIndex, theDataDrawer);

  ih->data->plt->_redraw = 1;
  return 0;
}

static char* iPPlotGetDSModeAttrib(Ihandle* ih)
{
  if (ih->data->plt->_currentDataSetIndex < 0 ||
      ih->data->plt->_currentDataSetIndex >= ih->data->plt->_plot.mPlotDataContainer.GetPlotCount())
    return NULL;

  DataDrawerBase* drawer = ih->data->plt->_plot.mPlotDataContainer.GetDataDrawer(ih->data->plt->_currentDataSetIndex);

  return (char*)drawer->mMode;
}

static int iPPlotSetDSEditAttrib(Ihandle* ih, const char* value)
{
  if (ih->data->plt->_currentDataSetIndex <  0 ||
      ih->data->plt->_currentDataSetIndex >= ih->data->plt->_plot.mPlotDataContainer.GetPlotCount())
    return 0;
  
  PlotDataSelection* dataselect = ih->data->plt->_plot.mPlotDataContainer.GetPlotDataSelection(ih->data->plt->_currentDataSetIndex);
    
  if (iupStrBoolean(value))
    dataselect->resize(ih->data->plt->_plot.mPlotDataContainer.GetConstYData(ih->data->plt->_currentDataSetIndex)->GetSize());
  else
    dataselect->clear();

  ih->data->plt->_redraw = 1;
  return 0;
}

static char* iPPlotGetDSEditAttrib(Ihandle* ih)
{
  if (ih->data->plt->_currentDataSetIndex < 0 ||
      ih->data->plt->_currentDataSetIndex >= ih->data->plt->_plot.mPlotDataContainer.GetPlotCount())
    return NULL;

  PlotDataSelection* dataselect = ih->data->plt->_plot.mPlotDataContainer.GetPlotDataSelection(ih->data->plt->_currentDataSetIndex);
  return iupStrReturnBoolean(!dataselect->empty());
}

static int iPPlotSetDSRemoveAttrib(Ihandle* ih, const char* value)
{
  int ii;

  if (ih->data->plt->_currentDataSetIndex <  0 ||
      ih->data->plt->_currentDataSetIndex >= ih->data->plt->_plot.mPlotDataContainer.GetPlotCount())
    return 0;
  
  if (iupStrToInt(value, &ii))
  {
    RemoveSample(ih->data->plt->_plot, ih->data->plt->_currentDataSetIndex, ii);
    ih->data->plt->_redraw = 1;
  }
  return 0;
}

static char* iPPlotGetDSCountAttrib(Ihandle* ih)
{
  if (ih->data->plt->_currentDataSetIndex < 0 ||
      ih->data->plt->_currentDataSetIndex >= ih->data->plt->_plot.mPlotDataContainer.GetPlotCount())
    return NULL;

  {
    int count = ih->data->plt->_plot.mPlotDataContainer.GetCount(ih->data->plt->_currentDataSetIndex);
    return iupStrReturnInt(count);
  }
}

/* ========== */
/* axis props */
/* ========== */

static int iPPlotSetAxisXLabelAttrib(Ihandle* ih, const char* value)
{
  AxisSetup* axis = &ih->data->plt->_plot.mXAxisSetup;

  if (value)
    axis->mLabel = value;
  else
    axis->mLabel = "";

  ih->data->plt->_redraw = 1;
  return 0;
}

static int iPPlotSetAxisYLabelAttrib(Ihandle* ih, const char* value)
{
  AxisSetup* axis = &ih->data->plt->_plot.mYAxisSetup;

  if (value)
    axis->mLabel = value;
  else
    axis->mLabel = "";

  ih->data->plt->_redraw = 1;
  return 0;
}

static char* iPPlotGetAxisXLabelAttrib(Ihandle* ih)
{
  AxisSetup* axis = &ih->data->plt->_plot.mXAxisSetup;
  return iupStrReturnStr(axis->mLabel.c_str());
}

static char* iPPlotGetAxisYLabelAttrib(Ihandle* ih)
{
  AxisSetup* axis = &ih->data->plt->_plot.mYAxisSetup;
  return iupStrReturnStr(axis->mLabel.c_str());
}

static int iPPlotSetAxisXLabelCenteredAttrib(Ihandle* ih, const char* value)
{
  AxisSetup* axis = &ih->data->plt->_plot.mXAxisSetup;

  if (iupStrBoolean(value))
    axis->mLabelCentered = true;
  else 
   axis->mLabelCentered = false;

  ih->data->plt->_redraw = 1;
  return 0;
}

static int iPPlotSetAxisYLabelCenteredAttrib(Ihandle* ih, const char* value)
{
  AxisSetup* axis = &ih->data->plt->_plot.mYAxisSetup;

  if (iupStrBoolean(value))
    axis->mLabelCentered = true;
  else 
   axis->mLabelCentered = false;

  ih->data->plt->_redraw = 1;
  return 0;
}

static char* iPPlotGetAxisXLabelCenteredAttrib(Ihandle* ih)
{
  AxisSetup* axis = &ih->data->plt->_plot.mXAxisSetup;

  return iupStrReturnBoolean (axis->mLabelCentered); 
}

static char* iPPlotGetAxisYLabelCenteredAttrib(Ihandle* ih)
{
  AxisSetup* axis = &ih->data->plt->_plot.mYAxisSetup;

  return iupStrReturnBoolean (axis->mLabelCentered); 
}

static int iPPlotSetAxisXColorAttrib(Ihandle* ih, const char* value)
{
  unsigned char rr, gg, bb;
  if (iupStrToRGB(value, &rr, &gg, &bb))
  {
    AxisSetup* axis = &ih->data->plt->_plot.mXAxisSetup;
    axis->mColor = PColor(rr, gg, bb);
    ih->data->plt->_redraw = 1;
  }
  return 0;
}

static int iPPlotSetAxisYColorAttrib(Ihandle* ih, const char* value)
{
  unsigned char rr, gg, bb;
  if (iupStrToRGB(value, &rr, &gg, &bb))
  {
    AxisSetup* axis = &ih->data->plt->_plot.mYAxisSetup;
    axis->mColor = PColor(rr, gg, bb);
    ih->data->plt->_redraw = 1;
  }
  return 0;
}

static char* iPPlotGetAxisXColorAttrib(Ihandle* ih)
{
  AxisSetup* axis = &ih->data->plt->_plot.mXAxisSetup;
  return iupStrReturnRGB(
          axis->mColor.mR,
          axis->mColor.mG,
          axis->mColor.mB);
}

static char* iPPlotGetAxisYColorAttrib(Ihandle* ih)
{
  AxisSetup* axis = &ih->data->plt->_plot.mYAxisSetup;
  return iupStrReturnRGB(
          axis->mColor.mR,
          axis->mColor.mG,
          axis->mColor.mB);
}

static int iPPlotSetAxisXAutoMinAttrib(Ihandle* ih, const char* value)
{
  AxisSetup* axis = &ih->data->plt->_plot.mXAxisSetup;

  if (iupStrBoolean(value))
    axis->mAutoScaleMin = true;
  else 
    axis->mAutoScaleMin = false;

  ih->data->plt->_redraw = 1;
  return 0;
}

static int iPPlotSetAxisYAutoMinAttrib(Ihandle* ih, const char* value)
{
  AxisSetup* axis = &ih->data->plt->_plot.mYAxisSetup;

  if (iupStrBoolean(value))
    axis->mAutoScaleMin = true;
  else 
    axis->mAutoScaleMin = false;

  ih->data->plt->_redraw = 1;
  return 0;
}

static char* iPPlotGetAxisXAutoMinAttrib(Ihandle* ih)
{
  AxisSetup* axis = &ih->data->plt->_plot.mXAxisSetup;

  return iupStrReturnBoolean (axis->mAutoScaleMin); 
}

static char* iPPlotGetAxisYAutoMinAttrib(Ihandle* ih)
{
  AxisSetup* axis = &ih->data->plt->_plot.mYAxisSetup;

  return iupStrReturnBoolean (axis->mAutoScaleMin); 
}

static int iPPlotSetAxisXAutoMaxAttrib(Ihandle* ih, const char* value)
{
  AxisSetup* axis = &ih->data->plt->_plot.mXAxisSetup;

  if (iupStrBoolean(value))
    axis->mAutoScaleMax = true;
  else 
    axis->mAutoScaleMax = false;

  ih->data->plt->_redraw = 1;
  return 0;
}

static int iPPlotSetAxisYAutoMaxAttrib(Ihandle* ih, const char* value)
{
  AxisSetup* axis = &ih->data->plt->_plot.mYAxisSetup;

  if (iupStrBoolean(value))
    axis->mAutoScaleMax = true;
  else 
    axis->mAutoScaleMax = false;

  ih->data->plt->_redraw = 1;
  return 0;
}

static char* iPPlotGetAxisXAutoMaxAttrib(Ihandle* ih)
{
  AxisSetup* axis = &ih->data->plt->_plot.mXAxisSetup;

  return iupStrReturnBoolean (axis->mAutoScaleMax); 
}

static char* iPPlotGetAxisYAutoMaxAttrib(Ihandle* ih)
{
  AxisSetup* axis = &ih->data->plt->_plot.mYAxisSetup;

  return iupStrReturnBoolean (axis->mAutoScaleMax); 
}

static int iPPlotSetAxisXMinAttrib(Ihandle* ih, const char* value)
{
  float xx;
  if (iupStrToFloat(value, &xx))
  {
    AxisSetup* axis = &ih->data->plt->_plot.mXAxisSetup;
    axis->mMin = xx;
    ih->data->plt->_redraw = 1;
  }
  return 0;
}

static int iPPlotSetAxisYMinAttrib(Ihandle* ih, const char* value)
{
  float xx;
  if (iupStrToFloat(value, &xx))
  {
    AxisSetup* axis = &ih->data->plt->_plot.mYAxisSetup;
    axis->mMin = xx;
    ih->data->plt->_redraw = 1;
  }
  return 0;
}

static char* iPPlotGetAxisXMinAttrib(Ihandle* ih)
{
  AxisSetup* axis = &ih->data->plt->_plot.mXAxisSetup;
  return iupStrReturnFloat(axis->mMin);
}

static char* iPPlotGetAxisYMinAttrib(Ihandle* ih)
{
  AxisSetup* axis = &ih->data->plt->_plot.mYAxisSetup;
  return iupStrReturnFloat(axis->mMin);
}

static int iPPlotSetAxisXMaxAttrib(Ihandle* ih, const char* value)
{
  float xx;
  if (iupStrToFloat(value, &xx))
  {
    AxisSetup* axis = &ih->data->plt->_plot.mXAxisSetup;
    axis->mMax = xx;
    ih->data->plt->_redraw = 1;
  }
  return 0;
}

static int iPPlotSetAxisYMaxAttrib(Ihandle* ih, const char* value)
{
  float xx;
  if (iupStrToFloat(value, &xx))
  {
    AxisSetup* axis = &ih->data->plt->_plot.mYAxisSetup;
    axis->mMax = xx;
    ih->data->plt->_redraw = 1;
  }
  return 0;
}

static char* iPPlotGetAxisXMaxAttrib(Ihandle* ih)
{
  AxisSetup* axis = &ih->data->plt->_plot.mXAxisSetup;
  return iupStrReturnFloat(axis->mMax);
}

static char* iPPlotGetAxisYMaxAttrib(Ihandle* ih)
{
  AxisSetup* axis = &ih->data->plt->_plot.mYAxisSetup;
  return iupStrReturnFloat(axis->mMax);
}

static int iPPlotSetAxisXReverseAttrib(Ihandle* ih, const char* value)
{
  AxisSetup* axis = &ih->data->plt->_plot.mXAxisSetup;

  if (iupStrBoolean(value))
    axis->mAscending = false; /* inverted for X */
  else 
    axis->mAscending = true;

  ih->data->plt->_redraw = 1;
  return 0;
}

static int iPPlotSetAxisYReverseAttrib(Ihandle* ih, const char* value)
{
  AxisSetup* axis = &ih->data->plt->_plot.mYAxisSetup;

  if (iupStrBoolean(value))
    axis->mAscending = true; /* NOT inverted for Y  */
  else 
    axis->mAscending = false;

  ih->data->plt->_redraw = 1;
  return 0;
}

static char* iPPlotGetAxisXReverseAttrib(Ihandle* ih)
{
  AxisSetup* axis = &ih->data->plt->_plot.mXAxisSetup;

  return iupStrReturnBoolean(!axis->mAscending);  /* inverted for X */
}

static char* iPPlotGetAxisYReverseAttrib(Ihandle* ih)
{
  AxisSetup* axis = &ih->data->plt->_plot.mYAxisSetup;

  return iupStrReturnBoolean(axis->mAscending);  /* NOT inverted for Y */
}

static int iPPlotSetAxisXCrossOriginAttrib(Ihandle* ih, const char* value)
{
  AxisSetup* axis = &ih->data->plt->_plot.mXAxisSetup;

  if (iupStrBoolean(value))
    axis->mCrossOrigin = true;
  else 
    axis->mCrossOrigin = false;

  ih->data->plt->_redraw = 1;
  return 0;
}

static int iPPlotSetAxisYCrossOriginAttrib(Ihandle* ih, const char* value)
{
  AxisSetup* axis = &ih->data->plt->_plot.mYAxisSetup;

  if (iupStrBoolean(value))
    axis->mCrossOrigin = true;
  else 
    axis->mCrossOrigin = false;

  ih->data->plt->_redraw = 1;
  return 0;
}

static char* iPPlotGetAxisXCrossOriginAttrib(Ihandle* ih)
{
  AxisSetup* axis = &ih->data->plt->_plot.mXAxisSetup;

  return iupStrReturnBoolean (axis->mCrossOrigin); 
}

static char* iPPlotGetAxisYCrossOriginAttrib(Ihandle* ih)
{
  AxisSetup* axis = &ih->data->plt->_plot.mYAxisSetup;

  return iupStrReturnBoolean (axis->mCrossOrigin); 
}

static int iPPlotSetAxisXScaleAttrib(Ihandle* ih, const char* value)
{
  AxisSetup* axis = &ih->data->plt->_plot.mXAxisSetup;

  if(iupStrEqualNoCase(value, "LIN"))
  {
    axis->mLogScale = false;
  }
  else if(iupStrEqualNoCase(value, "LOG10"))
  {
    axis->mLogScale = true;
    axis->mLogBase  = 10.0;
  }
  else if(iupStrEqualNoCase(value, "LOG2"))
  {
    axis->mLogScale = true;
    axis->mLogBase  = 2.0;
  }
  else
  {
    axis->mLogScale = true;
    axis->mLogBase  = (float)M_E;
  }

  ih->data->plt->_redraw = 1;
  return 0;
}

static int iPPlotSetAxisYScaleAttrib(Ihandle* ih, const char* value)
{
  AxisSetup* axis = &ih->data->plt->_plot.mYAxisSetup;

  if(iupStrEqualNoCase(value, "LIN"))
  {
    axis->mLogScale = false;
  }
  else if(iupStrEqualNoCase(value, "LOG10"))
  {
    axis->mLogScale = true;
    axis->mLogBase  = 10.0;
  }
  else if(iupStrEqualNoCase(value, "LOG2"))
  {
    axis->mLogScale = true;
    axis->mLogBase  = 2.0;
  }
  else
  {
    axis->mLogScale = true;
    axis->mLogBase  = (float)M_E;
  }

  ih->data->plt->_redraw = 1;
  return 0;
}

static char* iPPlotGetAxisXScaleAttrib(Ihandle* ih)
{
  AxisSetup* axis = &ih->data->plt->_plot.mXAxisSetup;

  if (axis->mLogScale)
  {
    if (axis->mLogBase == 10.0)
      return "LOG10";
    else if (axis->mLogBase == 2.0)
      return "LOG2";
    else
      return "LOGN";
  }
  else
    return "LIN";
}

static char* iPPlotGetAxisYScaleAttrib(Ihandle* ih)
{
  AxisSetup* axis = &ih->data->plt->_plot.mYAxisSetup;

  if (axis->mLogScale)
  {
    if (axis->mLogBase == 10.0)
      return "LOG10";
    else if (axis->mLogBase == 2.0)
      return "LOG2";
    else
      return "LOGN";
  }
  else
    return "LIN";
}

static int iPPlotSetAxisXFontSizeAttrib(Ihandle* ih, const char* value)
{
  int ii;
  if (iupStrToInt(value, &ii))
  {
    AxisSetup* axis = &ih->data->plt->_plot.mXAxisSetup;
    axis->mStyle.mFontSize = ii;
    ih->data->plt->_redraw = 1;
  }
  return 0;
}

static int iPPlotSetAxisYFontSizeAttrib(Ihandle* ih, const char* value)
{
  int ii;
  if (iupStrToInt(value, &ii))
  {
    AxisSetup* axis = &ih->data->plt->_plot.mYAxisSetup;
    axis->mStyle.mFontSize = ii;
    ih->data->plt->_redraw = 1;
  }
  return 0;
}

static char* iPPlotGetAxisXFontSizeAttrib(Ihandle* ih)
{
  AxisSetup* axis = &ih->data->plt->_plot.mXAxisSetup;
  return iPPlotGetPlotFontSize(axis->mStyle.mFontSize);
}

static char* iPPlotGetAxisYFontSizeAttrib(Ihandle* ih)
{
  AxisSetup* axis = &ih->data->plt->_plot.mYAxisSetup;
  return iPPlotGetPlotFontSize(axis->mStyle.mFontSize);
}

static int iPPlotSetAxisXFontStyleAttrib(Ihandle* ih, const char* value)
{
  int style = iPPlotGetCDFontStyle(value);
  if (style != -1)
  {
    AxisSetup* axis = &ih->data->plt->_plot.mXAxisSetup;
    axis->mStyle.mFontStyle = style;
    ih->data->plt->_redraw = 1;
  }
  return 0;
}

static int iPPlotSetAxisYFontStyleAttrib(Ihandle* ih, const char* value)
{
  int style = iPPlotGetCDFontStyle(value);
  if (style != -1)
  {
    AxisSetup* axis = &ih->data->plt->_plot.mYAxisSetup;
    axis->mStyle.mFontStyle = style;
    ih->data->plt->_redraw = 1;
  }
  return 0;
}

static char* iPPlotGetAxisXFontStyleAttrib(Ihandle* ih)
{
  AxisSetup* axis = &ih->data->plt->_plot.mXAxisSetup;
  return iPPlotGetPlotFontStyle(axis->mStyle.mFontStyle);
}

static char* iPPlotGetAxisYFontStyleAttrib(Ihandle* ih)
{
  AxisSetup* axis = &ih->data->plt->_plot.mYAxisSetup;
  return iPPlotGetPlotFontStyle(axis->mStyle.mFontStyle);
}

static int iPPlotSetAxisXAutoTickSizeAttrib(Ihandle* ih, const char* value)
{
  AxisSetup* axis = &ih->data->plt->_plot.mXAxisSetup;

  if (iupStrBoolean(value))
    axis->mTickInfo.mAutoTickSize = true;
  else 
    axis->mTickInfo.mAutoTickSize = false;

  ih->data->plt->_redraw = 1;
  return 0;
}

static int iPPlotSetAxisYAutoTickSizeAttrib(Ihandle* ih, const char* value)
{
  AxisSetup* axis = &ih->data->plt->_plot.mYAxisSetup;

  if (iupStrBoolean(value))
    axis->mTickInfo.mAutoTickSize = true;
  else 
    axis->mTickInfo.mAutoTickSize = false;

  ih->data->plt->_redraw = 1;
  return 0;
}

static char* iPPlotGetAxisXAutoTickSizeAttrib(Ihandle* ih)
{
  AxisSetup* axis = &ih->data->plt->_plot.mXAxisSetup;

  return iupStrReturnBoolean (axis->mTickInfo.mAutoTickSize); 
}

static char* iPPlotGetAxisYAutoTickSizeAttrib(Ihandle* ih)
{
  AxisSetup* axis = &ih->data->plt->_plot.mYAxisSetup;

  return iupStrReturnBoolean (axis->mTickInfo.mAutoTickSize); 
}

static int iPPlotSetAxisXTickSizeAttrib(Ihandle* ih, const char* value)
{
  int ii;
  if (iupStrToInt(value, &ii))
  {
    AxisSetup* axis = &ih->data->plt->_plot.mXAxisSetup;
    axis->mTickInfo.mMinorTickScreenSize = ii;
    ih->data->plt->_redraw = 1;
  }
  return 0;
}

static int iPPlotSetAxisYTickSizeAttrib(Ihandle* ih, const char* value)
{
  int ii;
  if (iupStrToInt(value, &ii))
  {
    AxisSetup* axis = &ih->data->plt->_plot.mYAxisSetup;
    axis->mTickInfo.mMinorTickScreenSize = ii;
    ih->data->plt->_redraw = 1;
  }
  return 0;
}

static char* iPPlotGetAxisXTickSizeAttrib(Ihandle* ih)
{
  AxisSetup* axis = &ih->data->plt->_plot.mXAxisSetup;
  return iupStrReturnInt(axis->mTickInfo.mMinorTickScreenSize);
}

static char* iPPlotGetAxisYTickSizeAttrib(Ihandle* ih)
{
  AxisSetup* axis = &ih->data->plt->_plot.mYAxisSetup;
  return iupStrReturnInt(axis->mTickInfo.mMinorTickScreenSize);
}

static int iPPlotSetAxisXTickMajorSizeAttrib(Ihandle* ih, const char* value)
{
  int ii;
  if (iupStrToInt(value, &ii))
  {
    AxisSetup* axis = &ih->data->plt->_plot.mXAxisSetup;
    axis->mTickInfo.mMajorTickScreenSize = ii;
    ih->data->plt->_redraw = 1;
  }
  return 0;
}

static int iPPlotSetAxisYTickMajorSizeAttrib(Ihandle* ih, const char* value)
{
  int ii;
  if (iupStrToInt(value, &ii))
  {
    AxisSetup* axis = &ih->data->plt->_plot.mYAxisSetup;
    axis->mTickInfo.mMajorTickScreenSize = ii;
    ih->data->plt->_redraw = 1;
  }
  return 0;
}

static char* iPPlotGetAxisXTickMajorSizeAttrib(Ihandle* ih)
{
  AxisSetup* axis = &ih->data->plt->_plot.mXAxisSetup;
  return iupStrReturnInt(axis->mTickInfo.mMajorTickScreenSize);
}

static char* iPPlotGetAxisYTickMajorSizeAttrib(Ihandle* ih)
{
  AxisSetup* axis = &ih->data->plt->_plot.mYAxisSetup;
  return iupStrReturnInt(axis->mTickInfo.mMajorTickScreenSize);
}

static int iPPlotSetAxisXTickFontSizeAttrib(Ihandle* ih, const char* value)
{
  int ii;
  if (iupStrToInt(value, &ii))
  {
    AxisSetup* axis = &ih->data->plt->_plot.mXAxisSetup;
    axis->mTickInfo.mStyle.mFontSize = ii;
    ih->data->plt->_redraw = 1;
  }
  return 0;
}

static int iPPlotSetAxisYTickFontSizeAttrib(Ihandle* ih, const char* value)
{
  int ii;
  if (iupStrToInt(value, &ii))
  {
    AxisSetup* axis = &ih->data->plt->_plot.mYAxisSetup;
    axis->mTickInfo.mStyle.mFontSize = ii;
    ih->data->plt->_redraw = 1;
  }
  return 0;
}

static char* iPPlotGetAxisXTickFontSizeAttrib(Ihandle* ih)
{
  AxisSetup* axis = &ih->data->plt->_plot.mXAxisSetup;

  return iPPlotGetPlotFontSize(axis->mTickInfo.mStyle.mFontSize);
}

static char* iPPlotGetAxisYTickFontSizeAttrib(Ihandle* ih)
{
  AxisSetup* axis = &ih->data->plt->_plot.mYAxisSetup;

  return iPPlotGetPlotFontSize(axis->mTickInfo.mStyle.mFontSize);
}

static int iPPlotSetAxisXTickFontStyleAttrib(Ihandle* ih, const char* value)
{
  int style = iPPlotGetCDFontStyle(value);
  if (style != -1)
  {
    AxisSetup* axis = &ih->data->plt->_plot.mXAxisSetup;
    axis->mTickInfo.mStyle.mFontStyle = style;
    ih->data->plt->_redraw = 1;
  }
  return 0;
}

static int iPPlotSetAxisYTickFontStyleAttrib(Ihandle* ih, const char* value)
{
  int style = iPPlotGetCDFontStyle(value);
  if (style != -1)
  {
    AxisSetup* axis = &ih->data->plt->_plot.mYAxisSetup;
    axis->mTickInfo.mStyle.mFontStyle = style;
    ih->data->plt->_redraw = 1;
  }
  return 0;
}

static char* iPPlotGetAxisXTickFontStyleAttrib(Ihandle* ih)
{
  AxisSetup* axis = &ih->data->plt->_plot.mXAxisSetup;

  return iPPlotGetPlotFontSize(axis->mTickInfo.mStyle.mFontStyle);
}

static char* iPPlotGetAxisYTickFontStyleAttrib(Ihandle* ih)
{
  AxisSetup* axis = &ih->data->plt->_plot.mYAxisSetup;

  return iPPlotGetPlotFontSize(axis->mTickInfo.mStyle.mFontStyle);
}

static int iPPlotSetAxisXTickFormatAttrib(Ihandle* ih, const char* value)
{
  AxisSetup* axis = &ih->data->plt->_plot.mXAxisSetup;

  if (value && value[0]!=0)
    axis->mTickInfo.mFormatString = value;
  else
    axis->mTickInfo.mFormatString = "%.0f";

  ih->data->plt->_redraw = 1;
  return 0;
}

static int iPPlotSetAxisYTickFormatAttrib(Ihandle* ih, const char* value)
{
  AxisSetup* axis = &ih->data->plt->_plot.mYAxisSetup;

  if (value && value[0]!=0)
    axis->mTickInfo.mFormatString = value;
  else
    axis->mTickInfo.mFormatString = "%.0f";

  ih->data->plt->_redraw = 1;
  return 0;
}

static char* iPPlotGetAxisXTickFormatAttrib(Ihandle* ih)
{
  AxisSetup* axis = &ih->data->plt->_plot.mXAxisSetup;
  return iupStrReturnStr(axis->mTickInfo.mFormatString.c_str());
}

static char* iPPlotGetAxisYTickFormatAttrib(Ihandle* ih)
{
  AxisSetup* axis = &ih->data->plt->_plot.mYAxisSetup;
  return iupStrReturnStr(axis->mTickInfo.mFormatString.c_str());
}

static int iPPlotSetAxisXTickAttrib(Ihandle* ih, const char* value)
{
  AxisSetup* axis = &ih->data->plt->_plot.mXAxisSetup;

  if (iupStrBoolean(value))
    axis->mTickInfo.mTicksOn = true;
  else 
    axis->mTickInfo.mTicksOn = false;

  ih->data->plt->_redraw = 1;
  return 0;
}

static int iPPlotSetAxisYTickAttrib(Ihandle* ih, const char* value)
{
  AxisSetup* axis = &ih->data->plt->_plot.mYAxisSetup;

  if (iupStrBoolean(value))
    axis->mTickInfo.mTicksOn = true;
  else 
    axis->mTickInfo.mTicksOn = false;

  ih->data->plt->_redraw = 1;
  return 0;
}

static char* iPPlotGetAxisXTickAttrib(Ihandle* ih)
{
  AxisSetup* axis = &ih->data->plt->_plot.mXAxisSetup;

  return iupStrReturnBoolean (axis->mTickInfo.mTicksOn); 
}

static char* iPPlotGetAxisYTickAttrib(Ihandle* ih)
{
  AxisSetup* axis = &ih->data->plt->_plot.mYAxisSetup;

  return iupStrReturnBoolean (axis->mTickInfo.mTicksOn); 
}

static int iPPlotSetAxisXTickMajorSpanAttrib(Ihandle* ih, const char* value)
{
  float xx;
  if (iupStrToFloat(value, &xx))
  {
    AxisSetup* axis = &ih->data->plt->_plot.mXAxisSetup;
    axis->mTickInfo.mMajorTickSpan = xx;
    ih->data->plt->_redraw = 1;
  }
  return 0;
}

static int iPPlotSetAxisYTickMajorSpanAttrib(Ihandle* ih, const char* value)
{
  float xx;
  if (iupStrToFloat(value, &xx))
  {
    AxisSetup* axis = &ih->data->plt->_plot.mYAxisSetup;
    axis->mTickInfo.mMajorTickSpan = xx;
    ih->data->plt->_redraw = 1;
  }
  return 0;
}

static char* iPPlotGetAxisXTickMajorSpanAttrib(Ihandle* ih)
{
  AxisSetup* axis = &ih->data->plt->_plot.mXAxisSetup;
  return iupStrReturnFloat(axis->mTickInfo.mMajorTickSpan);
}

static char* iPPlotGetAxisYTickMajorSpanAttrib(Ihandle* ih)
{
  AxisSetup* axis = &ih->data->plt->_plot.mYAxisSetup;
  return iupStrReturnFloat(axis->mTickInfo.mMajorTickSpan);
}

static int iPPlotSetAxisXTickDivisionAttrib(Ihandle* ih, const char* value)
{
  int ii;
  if (iupStrToInt(value, &ii))
  {
    AxisSetup* axis = &ih->data->plt->_plot.mXAxisSetup;
    axis->mTickInfo.mTickDivision = ii;
    ih->data->plt->_redraw = 1;
  }
  return 0;
}

static int iPPlotSetAxisYTickDivisionAttrib(Ihandle* ih, const char* value)
{
  int ii;
  if (iupStrToInt(value, &ii))
  {
    AxisSetup* axis = &ih->data->plt->_plot.mYAxisSetup;
    axis->mTickInfo.mTickDivision = ii;
    ih->data->plt->_redraw = 1;
  }
  return 0;
}

static char* iPPlotGetAxisXTickDivisionAttrib(Ihandle* ih)
{
  AxisSetup* axis = &ih->data->plt->_plot.mXAxisSetup;
  return iupStrReturnInt(axis->mTickInfo.mTickDivision);
}

static char* iPPlotGetAxisYTickDivisionAttrib(Ihandle* ih)
{
  AxisSetup* axis = &ih->data->plt->_plot.mYAxisSetup;
  return iupStrReturnInt(axis->mTickInfo.mTickDivision);
}

static int iPPlotSetAxisXAutoTickAttrib(Ihandle* ih, const char* value)
{
  AxisSetup* axis = &ih->data->plt->_plot.mXAxisSetup;

  if (iupStrBoolean(value))
    axis->mTickInfo.mAutoTick = true;
  else 
    axis->mTickInfo.mAutoTick = false;

  ih->data->plt->_redraw = 1;
  return 0;
}

static int iPPlotSetAxisYAutoTickAttrib(Ihandle* ih, const char* value)
{
  AxisSetup* axis = &ih->data->plt->_plot.mYAxisSetup;

  if (iupStrBoolean(value))
    axis->mTickInfo.mAutoTick = true;
  else 
    axis->mTickInfo.mAutoTick = false;

  ih->data->plt->_redraw = 1;
  return 0;
}

static char* iPPlotGetAxisXAutoTickAttrib(Ihandle* ih)
{
  AxisSetup* axis = &ih->data->plt->_plot.mXAxisSetup;
  return iupStrReturnBoolean (axis->mTickInfo.mAutoTick); 
}

static char* iPPlotGetAxisYAutoTickAttrib(Ihandle* ih)
{
  AxisSetup* axis = &ih->data->plt->_plot.mYAxisSetup;

  return iupStrReturnBoolean (axis->mTickInfo.mAutoTick); 
}

void PPainterIup::MouseButton(int btn, int stat, int x, int y, char *r)
{
  PMouseEvent theEvent;
  int theModifierKeys = 0;

  theEvent.mX = x;
  theEvent.mY = y;
  
  if (btn == IUP_BUTTON1)
  {
    theEvent.mType = ( stat!=0 ? (PMouseEvent::kDown) : (PMouseEvent::kUp) );
    _mouseDown = ( stat!=0 ? 1 : 0 );
  }
  else 
    return;

  _mouse_ALT = 0;
  _mouse_SHIFT = 0;
  _mouse_CTRL = 0;

  if (iup_isalt(r))  /* signal Alt */
  {
    theModifierKeys = (theModifierKeys | PMouseEvent::kAlt);
    _mouse_ALT = 1;
  }
  if (iup_iscontrol(r))  /* signal Ctrl */
  {
    theModifierKeys = (theModifierKeys | PMouseEvent::kControl);
    _mouse_CTRL = 1;
  }
  if (iup_isshift(r))  /* signal Shift */
  {
    theModifierKeys = (theModifierKeys | PMouseEvent::kShift);
    _mouse_SHIFT = 1;
  }
  theEvent.SetModifierKeys (theModifierKeys);

  int old_size = _InteractionContainer->mZoomInteraction.GetZoomStackSize();

  if( _InteractionContainer->HandleMouseEvent(theEvent))
  {
    if (btn == IUP_BUTTON1 && !_mouseDown && _ih->data->sync_view)
    {
      this->Paint(1, 0);

      int size = _InteractionContainer->mZoomInteraction.GetZoomStackSize();
      if (size != old_size)
      {
        int old_current = _ih->data->plt_index;

        for (int p = 0; p<_ih->data->plots_count; p++)
        {
          if (_ih->data->plots[p] != this)
          {
            int off_x = _plot.mMargins.mLeft - _ih->data->plots[p]->_plot.mMargins.mLeft;
            int off_y = _plot.mMargins.mTop - _ih->data->plots[p]->_plot.mMargins.mTop;
            _ih->data->plots[p]->_InteractionContainer->mZoomInteraction.RepeatZoom(_InteractionContainer->mZoomInteraction, off_x, off_y);

            iPPlotSetPlotCurrent(_ih, p);
            _ih->data->plots[p]->Paint(1, 0);
          }
        }

        iPPlotSetPlotCurrent(_ih, old_current);
      }

      iPPlotFlush(_ih);
    }
    else
      this->Paint(1, 1);
  }
}

void PPainterIup::Pan(float delta, bool vertical)
{
  float page, offset, Range, OriginalRange;
  PAxisInfo OriginalAxis = _InteractionContainer->mZoomInteraction.mOriginalAxis;
  if (vertical)
  {
    Range = _plot.mYAxisSetup.mMax - _plot.mYAxisSetup.mMin;
    OriginalRange = OriginalAxis.mYAxisSetup.mMax - OriginalAxis.mYAxisSetup.mMin;
  }
  else
  {
    Range = _plot.mXAxisSetup.mMax - _plot.mXAxisSetup.mMin;
    OriginalRange = OriginalAxis.mXAxisSetup.mMax - OriginalAxis.mXAxisSetup.mMin;
  }

  page = fabs(OriginalRange - Range)/10;
  offset = page*delta;

  if (vertical)
  {
    _plot.mYAxisSetup.mMin -= offset;
    _plot.mYAxisSetup.mMax -= offset;

    if (_plot.mYAxisSetup.mMin < OriginalAxis.mYAxisSetup.mMin)
    {
      _plot.mYAxisSetup.mMin = OriginalAxis.mYAxisSetup.mMin;
      _plot.mYAxisSetup.mMax = _plot.mYAxisSetup.mMin + Range;
    }
    if (_plot.mYAxisSetup.mMax > OriginalAxis.mYAxisSetup.mMax)
    {
      _plot.mYAxisSetup.mMax = OriginalAxis.mYAxisSetup.mMax;
      _plot.mYAxisSetup.mMin = _plot.mYAxisSetup.mMax - Range;
    }
  }
  else
  {
    _plot.mXAxisSetup.mMin += offset;
    _plot.mXAxisSetup.mMax += offset;

    if (_plot.mXAxisSetup.mMin < OriginalAxis.mXAxisSetup.mMin)
    {
      _plot.mXAxisSetup.mMin = OriginalAxis.mXAxisSetup.mMin;
      _plot.mXAxisSetup.mMax = _plot.mXAxisSetup.mMin + Range;
    }
    if (_plot.mXAxisSetup.mMax > OriginalAxis.mXAxisSetup.mMax)
    {
      _plot.mXAxisSetup.mMax = OriginalAxis.mXAxisSetup.mMax;
      _plot.mXAxisSetup.mMin = _plot.mXAxisSetup.mMax - Range;
    }
  }
}

void PPainterIup::MouseWheel(float delta, char *status)
{
  int size = _InteractionContainer->mZoomInteraction.GetZoomStackSize();
  if (size > 0)
  {
    bool vertical = true;
    if (iup_isshift(status))
      vertical = false;
  
    Pan(delta, vertical);

    if (_ih->data->sync_view)
    {
      this->Paint(1, 0);

      int old_current = _ih->data->plt_index;

      for (int p = 0; p<_ih->data->plots_count; p++)
      {
        if (_ih->data->plots[p] != this)
        {
          _ih->data->plots[p]->Pan(delta, vertical);

          iPPlotSetPlotCurrent(_ih, p);
          _ih->data->plots[p]->Paint(1, 0);
        }
      }
      iPPlotSetPlotCurrent(_ih, old_current);

      iPPlotFlush(_ih);
    }
    else
      this->Paint(1, 1);
  }
}

void PPainterIup::MouseMove(int x, int y)
{
  PMouseEvent theEvent;
  int theModifierKeys = 0;

  if(!_mouseDown )  // Only process when mouse is down
    return;

  theEvent.mX = x;
  theEvent.mY = y;

  theEvent.mType = PMouseEvent::kMove;
  if(_mouse_ALT)  /* signal Alt */
  {
    theModifierKeys = (theModifierKeys | PMouseEvent::kAlt);
  }
  if(_mouse_SHIFT)  /* signal Shift */
  {
    theModifierKeys = (theModifierKeys | PMouseEvent::kControl);
  }
  if(_mouse_CTRL)  /* signal Ctrl */
  {
    theModifierKeys = (theModifierKeys | PMouseEvent::kShift);
  }
  theEvent.SetModifierKeys (theModifierKeys);

  if(_InteractionContainer->HandleMouseEvent(theEvent))
    this->Paint(1, 1);
}

void PPainterIup::KeyPress(int c, int press)
{
  int theModifierKeys = 0;
  int theRepeatCount = 0;
  PKeyEvent::EKey theKeyCode = PKeyEvent::kNone;
  char theChar = 0;

  if(!press) return;

  switch(c)
  {
    case K_cX:  /* CTRL + X */
      theModifierKeys = PMouseEvent::kControl;
      theKeyCode = PKeyEvent::kChar;
      theChar = 'x';
    break;
    case K_cY:  /* CTRL + Y */
      theModifierKeys = PMouseEvent::kControl;
      theKeyCode = PKeyEvent::kChar;
      theChar = 'y';
    break;
    case K_cR:  /* CTRL + R */
      theModifierKeys = PMouseEvent::kControl;
      theKeyCode = PKeyEvent::kChar;
      theChar = 'r';
    break;
    case K_cUP:  /* CTRL + Arrow */
      theModifierKeys = PMouseEvent::kControl;
    case K_UP:   /* Arrow */
      theKeyCode = PKeyEvent::kArrowUp;
    break;
    case K_cDOWN:  /* CTRL + Arrow */
      theModifierKeys = PMouseEvent::kControl;
    case K_DOWN:   /* Arrow */
      theKeyCode = PKeyEvent::kArrowDown;
    break;
    case K_cLEFT:  /* CTRL + Arrow */
      theModifierKeys = PMouseEvent::kControl;
    case K_LEFT:   /* Arrow */
      theKeyCode = PKeyEvent::kArrowLeft;
    break;
    case K_cRIGHT:  /* CTRL + Arrow */
      theModifierKeys = PMouseEvent::kControl;
    case K_RIGHT:   /* Arrow */
      theKeyCode = PKeyEvent::kArrowRight;
    break;
    case K_cDEL:  /* CTRL + Arrow */
      theModifierKeys = PMouseEvent::kControl;
    case K_DEL:   /* Arrow */
      theKeyCode = PKeyEvent::kDelete;
    break;
  }

  PKeyEvent theEvent (theKeyCode, theRepeatCount, theModifierKeys, theChar);

  if(_InteractionContainer->HandleKeyEvent(theEvent))
    this->Paint(1, 1);
}

void PPainterIup::UpdateViewport()
{
#ifdef USE_OPENGL
  IupGLMakeCurrent(_ih);

  glViewport(_x, _y, _width, _height);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(_x, _x + _width, y, _y + _height, -1, 1);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  char StrData[100];
  sprintf(StrData, "%dx%d", _width, _height);
  cdCanvasSetAttribute(_ih->data->cd_canvas, "SIZE", StrData);
#endif

  cdCanvasActivate(_ih->data->cd_canvas);
  cdCanvasOrigin(_ih->data->cd_canvas, _x, _y);
}

void PPainterIup::Paint(int force, int flush)
{
  if (!_ih->data->cd_canvas)
    return;

#ifdef USE_OPENGL
  if (!IupGLIsCurrent(_ih))
    force = 1;
#endif

  UpdateViewport();

  if (force || _redraw)
  {
    _plot.Draw(*this);
    _redraw = 0;
  }

  if (flush)
    iPPlotFlush(_ih);
}

void PPainterIup::SetSize(int x, int y, int w, int h)
{
  _x = x;
  _y = y;
  _width = w;
  _height = h;
  _redraw = 1;
}

void PPainterIup::FillArrow(int inX1, int inY1, int inX2, int inY2, int inX3, int inY3)
{
  if (!_ih->data->cd_canvas)
    return;

  cdCanvasBegin(_ih->data->cd_canvas, CD_FILL);
  cdCanvasVertex(_ih->data->cd_canvas, inX1, cdCanvasInvertYAxis(_ih->data->cd_canvas, inY1));
  cdCanvasVertex(_ih->data->cd_canvas, inX2, cdCanvasInvertYAxis(_ih->data->cd_canvas, inY2));
  cdCanvasVertex(_ih->data->cd_canvas, inX3, cdCanvasInvertYAxis(_ih->data->cd_canvas, inY3));
  cdCanvasEnd(_ih->data->cd_canvas);
}

void PPainterIup::BeginArea()
{
  if (!_ih->data->cd_canvas)
    return;

  cdCanvasBegin(_ih->data->cd_canvas, CD_FILL);
}

void PPainterIup::AddVertex(float inX, float inY)
{
  if (!_ih->data->cd_canvas)
    return;

  cdfCanvasVertex(_ih->data->cd_canvas, inX, cdfCanvasInvertYAxis(_ih->data->cd_canvas, inY));
}

void PPainterIup::EndArea()
{
  if (!_ih->data->cd_canvas)
    return;

  cdCanvasEnd(_ih->data->cd_canvas);
}

void PPainterIup::DrawLine(float inX1, float inY1, float inX2, float inY2)
{
  if (!_ih->data->cd_canvas)
    return;

  cdfCanvasLine(_ih->data->cd_canvas, inX1, cdfCanvasInvertYAxis(_ih->data->cd_canvas, inY1),
                                           inX2, cdfCanvasInvertYAxis(_ih->data->cd_canvas, inY2));
}

void PPainterIup::FillRect(int inX, int inY, int inW, int inH)
{
  if (!_ih->data->cd_canvas)
    return;

  cdCanvasBox(_ih->data->cd_canvas, inX, inX+inW, 
              cdCanvasInvertYAxis(_ih->data->cd_canvas, inY),
              cdCanvasInvertYAxis(_ih->data->cd_canvas, inY + inH - 1));
}

void PPainterIup::SetClipRect(int inX, int inY, int inW, int inH)
{
  if (!_ih->data->cd_canvas)
    return;

  cdCanvasClipArea(_ih->data->cd_canvas, inX, inX + inW - 1,
                   cdCanvasInvertYAxis(_ih->data->cd_canvas, inY),
                   cdCanvasInvertYAxis(_ih->data->cd_canvas, inY + inH - 1));
  cdCanvasClip(_ih->data->cd_canvas, CD_CLIPAREA);
}

void PPainterIup::SetLineColor(int inR, int inG, int inB)
{
  if (!_ih->data->cd_canvas)
    return;

  cdCanvasForeground(_ih->data->cd_canvas, cdEncodeColor((unsigned char)inR,
                                               (unsigned char)inG,
                                               (unsigned char)inB));
}

void PPainterIup::SetFillColor(int inR, int inG, int inB)
{
  if (!_ih->data->cd_canvas)
    return;

  cdCanvasForeground(_ih->data->cd_canvas, cdEncodeColor((unsigned char)inR,
                                               (unsigned char)inG,
                                               (unsigned char)inB));
}

long PPainterIup::CalculateTextDrawSize(const char *inString)
{
  int iw;

  if (!_ih->data->cd_canvas)
    return IUP_NOERROR;

  cdCanvasGetTextSize(_ih->data->cd_canvas, const_cast<char *>(inString), &iw, NULL);

  return iw;
}

long PPainterIup::GetFontHeight() const
{
  int ih;

  if (!_ih->data->cd_canvas)
    return IUP_NOERROR;

  cdCanvasGetFontDim(_ih->data->cd_canvas, NULL, &ih, NULL, NULL);

  return ih;
}

void PPainterIup::DrawText(int inX, int inY, short align, const char *inString)
{
  if (!_ih->data->cd_canvas)
    return;

  cdCanvasTextAlignment(_ih->data->cd_canvas, align);
  cdCanvasText(_ih->data->cd_canvas, inX, cdCanvasInvertYAxis(_ih->data->cd_canvas, inY), const_cast<char *>(inString));
}

void PPainterIup::DrawRotatedText(int inX, int inY, float inDegrees, short align, const char *inString)
{
  double aprev;

  if (!_ih->data->cd_canvas)
    return;

  cdCanvasTextAlignment(_ih->data->cd_canvas, align);
  aprev = cdCanvasTextOrientation(_ih->data->cd_canvas, -inDegrees);
  cdCanvasText(_ih->data->cd_canvas, inX, cdCanvasInvertYAxis(_ih->data->cd_canvas, inY), const_cast<char *>(inString));
  cdCanvasTextOrientation(_ih->data->cd_canvas, aprev);
}

void PPainterIup::SetStyle(const PStyle &inStyle)
{
  if (!_ih->data->cd_canvas)
    return;

  cdCanvasLineWidth(_ih->data->cd_canvas, inStyle.mPenWidth);
  cdCanvasLineStyle(_ih->data->cd_canvas, inStyle.mPenStyle);

  cdCanvasNativeFont(_ih->data->cd_canvas, IupGetAttribute(_ih, "FONT"));

  if (inStyle.mFontStyle != -1 || inStyle.mFontSize != 0)
    cdCanvasFont(_ih->data->cd_canvas, NULL, inStyle.mFontStyle, inStyle.mFontSize);

  cdCanvasMarkType(_ih->data->cd_canvas, inStyle.mMarkStyle);
  cdCanvasMarkSize(_ih->data->cd_canvas, inStyle.mMarkSize);
}

static int iPPlotMapMethod(Ihandle* ih)
{
#ifdef USE_OPENGL
  char StrData[100];
  int w, h;
  IupGetIntInt(ih, "DRAWSIZE", &w, &h);
  sprintf(StrData, "%dx%d", w, h);

  ih->data->cd_canvas = cdCreateCanvas(CD_GL, StrData);
  if (!ih->data->cd_canvas)
    return IUP_ERROR;
#else
  int old_gdi = 0;

  if (IupGetInt(ih, "USE_GDI+") || IupGetInt(ih, "USE_CONTEXTPLUS"))
    old_gdi = cdUseContextPlus(1);

  if (IupGetInt(ih, "USE_IMAGERGB"))
    ih->data->cd_canvas = cdCreateCanvas(CD_IUPDBUFFERRGB, ih);
  else
    ih->data->cd_canvas = cdCreateCanvas(CD_IUPDBUFFER, ih);

  if (IupGetInt(ih, "USE_GDI+") || IupGetInt(ih, "USE_CONTEXTPLUS"))
    cdUseContextPlus(old_gdi);

  if (!ih->data->cd_canvas)
    return IUP_ERROR;
#endif

  for(int p=0; p<ih->data->plots_count; p++)
    ih->data->plots[p]->_redraw = 1;

  return IUP_NOERROR;
}

static void iPPlotUnMapMethod(Ihandle* ih)
{
  if (ih->data->cd_canvas != NULL)
  {
    cdKillCanvas(ih->data->cd_canvas);
    ih->data->cd_canvas = NULL;
  }
}

static void iPPlotDestroyMethod(Ihandle* ih)
{
  for(int p=0; p<ih->data->plots_count; p++)
    delete ih->data->plots[p];
}

static int iPPlotCreateMethod(Ihandle* ih, void **params)
{
  (void)params;

  /* free the data alocated by IupCanvas */
  free(ih->data);
  ih->data = iupALLOCCTRLDATA();

  /* Initializing object with no cd canvases */
  ih->data->plots[0] = new PPainterIup(ih);
  ih->data->plt_index = 0;
  ih->data->plots_numcol = 1;
  ih->data->plots_count = 1;

  ih->data->plt = ih->data->plots[ih->data->plt_index];

  /* IupCanvas callbacks */
  IupSetCallback(ih, "ACTION",      (Icallback)iPPlotRedraw_CB);
  IupSetCallback(ih, "RESIZE_CB",   (Icallback)iPPlotResize_CB);
  IupSetCallback(ih, "BUTTON_CB",   (Icallback)iPPlotMouseButton_CB);
  IupSetCallback(ih, "MOTION_CB",   (Icallback)iPPlotMouseMove_CB);
  IupSetCallback(ih, "KEYPRESS_CB", (Icallback)iPPlotKeyPress_CB);
  IupSetCallback(ih, "WHEEL_CB",    (Icallback)iPPlotWheel_CB);

#ifdef USE_OPENGL
  IupSetAttribute(ih, "BUFFER", "DOUBLE");
#endif

  return IUP_NOERROR;
}

static Iclass* iPPlotNewClass(void)
{
#ifdef USE_OPENGL
  Iclass* ic = iupClassNew(iupRegisterFindClass("glcanvas"));
#else
  Iclass* ic = iupClassNew(iupRegisterFindClass("canvas"));
#endif

  ic->name = (char*)"pplot";
  ic->format = NULL;  /* none */
  ic->nativetype = IUP_TYPECANVAS;
  ic->childtype = IUP_CHILDNONE;
  ic->is_interactive = 1;

  /* Class functions */
  ic->New = iPPlotNewClass;
  ic->Create  = iPPlotCreateMethod;
  ic->Destroy = iPPlotDestroyMethod;
  ic->Map     = iPPlotMapMethod;
  ic->UnMap   = iPPlotUnMapMethod;

  /* IupPPlot Callbacks */
  iupClassRegisterCallback(ic, "POSTDRAW_CB", "C");
  iupClassRegisterCallback(ic, "PREDRAW_CB", "C");
  iupClassRegisterCallback(ic, "PLOTMOTION_CB", "ff");
  iupClassRegisterCallback(ic, "PLOTBUTTON_CB", "iiffs");
  iupClassRegisterCallback(ic, "DELETE_CB", "iiff");
  iupClassRegisterCallback(ic, "DELETEBEGIN_CB", "");
  iupClassRegisterCallback(ic, "DELETEEND_CB", "");
  iupClassRegisterCallback(ic, "SELECT_CB", "iiffi");
  iupClassRegisterCallback(ic, "SELECTBEGIN_CB", "");
  iupClassRegisterCallback(ic, "SELECTEND_CB", "");
  iupClassRegisterCallback(ic, "EDIT_CB", "iiffff");
  iupClassRegisterCallback(ic, "EDITBEGIN_CB", "");
  iupClassRegisterCallback(ic, "EDITEND_CB", "");

  /* Visual */
  iupClassRegisterAttribute(ic, "BGCOLOR", iPPlotGetBGColorAttrib, iPPlotSetBGColorAttrib, IUPAF_SAMEASSYSTEM, "255 255 255", IUPAF_NOT_MAPPED);   /* overwrite canvas implementation, set a system default to force a new default */
  iupClassRegisterAttribute(ic, "FGCOLOR", iPPlotGetFGColorAttrib, iPPlotSetFGColorAttrib, IUPAF_SAMEASSYSTEM, "0 0 0", IUPAF_NOT_MAPPED);

  /* IupPPlot only */

  iupClassRegisterAttribute(ic, "REDRAW", NULL, iPPlotSetRedrawAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SYNCVIEW", iPPlotGetSyncViewAttrib, iPPlotSetSyncViewAttrib, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "CANVAS", iPPlotGetCanvasAttrib, NULL, NULL, NULL, IUPAF_READONLY | IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "TITLE", iPPlotGetTitleAttrib, iPPlotSetTitleAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "TITLEFONTSIZE", iPPlotGetTitleFontSizeAttrib, iPPlotSetTitleFontSizeAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "TITLEFONTSTYLE", NULL, iPPlotSetTitleFontStyleAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "LEGENDSHOW", iPPlotGetLegendShowAttrib, iPPlotSetLegendShowAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "LEGENDPOS", iPPlotGetLegendPosAttrib, iPPlotSetLegendPosAttrib, IUPAF_SAMEASSYSTEM, "TOPRIGHT", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "LEGENDFONTSIZE", NULL, iPPlotSetLegendFontSizeAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "LEGENDFONTSTYLE", NULL, iPPlotSetLegendFontStyleAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "MARGINLEFT", iPPlotGetMarginLeftAttrib, iPPlotSetMarginLeftAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "MARGINRIGHT", iPPlotGetMarginRightAttrib, iPPlotSetMarginRightAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "MARGINTOP", iPPlotGetMarginTopAttrib, iPPlotSetMarginTopAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "MARGINBOTTOM", iPPlotGetMarginBottomAttrib, iPPlotSetMarginBottomAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "GRIDLINESTYLE", iPPlotGetGridLineStyleAttrib, iPPlotSetGridLineStyleAttrib, IUPAF_SAMEASSYSTEM, "CONTINUOUS", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "GRIDCOLOR", iPPlotGetGridColorAttrib, iPPlotSetGridColorAttrib, IUPAF_SAMEASSYSTEM, "200 200 200", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "GRID", iPPlotGetGridAttrib, iPPlotSetGridAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "DS_LINESTYLE", iPPlotGetDSLineStyleAttrib, iPPlotSetDSLineStyleAttrib, IUPAF_SAMEASSYSTEM, "CONTINUOUS", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "DS_LINEWIDTH", iPPlotGetDSLineWidthAttrib, iPPlotSetDSLineWidthAttrib, IUPAF_SAMEASSYSTEM, "1", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "DS_MARKSTYLE", iPPlotGetDSMarkStyleAttrib, iPPlotSetDSMarkStyleAttrib, IUPAF_SAMEASSYSTEM, "X", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "DS_MARKSIZE", iPPlotGetDSMarkSizeAttrib, iPPlotSetDSMarkSizeAttrib, IUPAF_SAMEASSYSTEM, "7", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "DS_LEGEND", iPPlotGetDSLegendAttrib, iPPlotSetDSLegendAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "DS_COLOR", iPPlotGetDSColorAttrib, iPPlotSetDSColorAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "DS_SHOWVALUES", iPPlotGetDSShowValuesAttrib, iPPlotSetDSShowValuesAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "DS_MODE", iPPlotGetDSModeAttrib, iPPlotSetDSModeAttrib, IUPAF_SAMEASSYSTEM, "LINE", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "DS_EDIT", iPPlotGetDSEditAttrib, iPPlotSetDSEditAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "DS_REMOVE", NULL, iPPlotSetDSRemoveAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "DS_COUNT", iPPlotGetDSCountAttrib, NULL, NULL, NULL, IUPAF_READONLY|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "AXS_XLABEL", iPPlotGetAxisXLabelAttrib, iPPlotSetAxisXLabelAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_YLABEL", iPPlotGetAxisYLabelAttrib, iPPlotSetAxisYLabelAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_XLABELCENTERED", iPPlotGetAxisXLabelCenteredAttrib, iPPlotSetAxisXLabelCenteredAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_YLABELCENTERED", iPPlotGetAxisYLabelCenteredAttrib, iPPlotSetAxisYLabelCenteredAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_XCOLOR", iPPlotGetAxisXColorAttrib, iPPlotSetAxisXColorAttrib, IUPAF_SAMEASSYSTEM, "0 0 0", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_YCOLOR", iPPlotGetAxisYColorAttrib, iPPlotSetAxisYColorAttrib, IUPAF_SAMEASSYSTEM, "0 0 0", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_XAUTOMIN", iPPlotGetAxisXAutoMinAttrib, iPPlotSetAxisXAutoMinAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_YAUTOMIN", iPPlotGetAxisYAutoMinAttrib, iPPlotSetAxisYAutoMinAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_XAUTOMAX", iPPlotGetAxisXAutoMaxAttrib, iPPlotSetAxisXAutoMaxAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_YAUTOMAX", iPPlotGetAxisYAutoMaxAttrib, iPPlotSetAxisYAutoMaxAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_XMIN", iPPlotGetAxisXMinAttrib, iPPlotSetAxisXMinAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_YMIN", iPPlotGetAxisYMinAttrib, iPPlotSetAxisYMinAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_XMAX", iPPlotGetAxisXMaxAttrib, iPPlotSetAxisXMaxAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_YMAX", iPPlotGetAxisYMaxAttrib, iPPlotSetAxisYMaxAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_XREVERSE", iPPlotGetAxisXReverseAttrib, iPPlotSetAxisXReverseAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_YREVERSE", iPPlotGetAxisYReverseAttrib, iPPlotSetAxisYReverseAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_XCROSSORIGIN", iPPlotGetAxisXCrossOriginAttrib, iPPlotSetAxisXCrossOriginAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_YCROSSORIGIN", iPPlotGetAxisYCrossOriginAttrib, iPPlotSetAxisYCrossOriginAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_XSCALE", iPPlotGetAxisXScaleAttrib, iPPlotSetAxisXScaleAttrib, IUPAF_SAMEASSYSTEM, "LIN", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_YSCALE", iPPlotGetAxisYScaleAttrib, iPPlotSetAxisYScaleAttrib, IUPAF_SAMEASSYSTEM, "LIN", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_XFONTSIZE", iPPlotGetAxisXFontSizeAttrib, iPPlotSetAxisXFontSizeAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_YFONTSIZE", iPPlotGetAxisYFontSizeAttrib, iPPlotSetAxisYFontSizeAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_XFONTSTYLE", iPPlotGetAxisXFontStyleAttrib, iPPlotSetAxisXFontStyleAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_YFONTSTYLE", iPPlotGetAxisYFontStyleAttrib, iPPlotSetAxisYFontStyleAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_XTICK", iPPlotGetAxisXTickAttrib, iPPlotSetAxisXTickAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_YTICK", iPPlotGetAxisYTickAttrib, iPPlotSetAxisYTickAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_XTICKSIZE", iPPlotGetAxisXTickSizeAttrib, iPPlotSetAxisXTickSizeAttrib, IUPAF_SAMEASSYSTEM, "5", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_YTICKSIZE", iPPlotGetAxisYTickSizeAttrib, iPPlotSetAxisYTickSizeAttrib, IUPAF_SAMEASSYSTEM, "5", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_XTICKFORMAT", iPPlotGetAxisXTickFormatAttrib, iPPlotSetAxisXTickFormatAttrib, IUPAF_SAMEASSYSTEM, "%.0f", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_YTICKFORMAT", iPPlotGetAxisYTickFormatAttrib, iPPlotSetAxisYTickFormatAttrib, IUPAF_SAMEASSYSTEM, "%.0f", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_XTICKFONTSIZE", iPPlotGetAxisXTickFontSizeAttrib, iPPlotSetAxisXTickFontSizeAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_YTICKFONTSIZE", iPPlotGetAxisYTickFontSizeAttrib, iPPlotSetAxisYTickFontSizeAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_XTICKFONTSTYLE", iPPlotGetAxisXTickFontStyleAttrib, iPPlotSetAxisXTickFontStyleAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_YTICKFONTSTYLE", iPPlotGetAxisYTickFontStyleAttrib, iPPlotSetAxisYTickFontStyleAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_XAUTOTICK", iPPlotGetAxisXAutoTickAttrib, iPPlotSetAxisXAutoTickAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_YAUTOTICK", iPPlotGetAxisYAutoTickAttrib, iPPlotSetAxisYAutoTickAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_XTICKMAJORSPAN", iPPlotGetAxisXTickMajorSpanAttrib, iPPlotSetAxisXTickMajorSpanAttrib, IUPAF_SAMEASSYSTEM, "1", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_YTICKMAJORSPAN", iPPlotGetAxisYTickMajorSpanAttrib, iPPlotSetAxisYTickMajorSpanAttrib, IUPAF_SAMEASSYSTEM, "1", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_XTICKDIVISION", iPPlotGetAxisXTickDivisionAttrib, iPPlotSetAxisXTickDivisionAttrib, IUPAF_SAMEASSYSTEM, "5", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_YTICKDIVISION", iPPlotGetAxisYTickDivisionAttrib, iPPlotSetAxisYTickDivisionAttrib, IUPAF_SAMEASSYSTEM, "5", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_XAUTOTICKSIZE", iPPlotGetAxisXAutoTickSizeAttrib, iPPlotSetAxisXAutoTickSizeAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_YAUTOTICKSIZE", iPPlotGetAxisYAutoTickSizeAttrib, iPPlotSetAxisYAutoTickSizeAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_XTICKMAJORSIZE", iPPlotGetAxisXTickMajorSizeAttrib, iPPlotSetAxisXTickMajorSizeAttrib, IUPAF_SAMEASSYSTEM, "8", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_YTICKMAJORSIZE", iPPlotGetAxisYTickMajorSizeAttrib, iPPlotSetAxisYTickMajorSizeAttrib, IUPAF_SAMEASSYSTEM, "8", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "REMOVE", NULL, iPPlotSetRemoveAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "CLEAR", NULL, iPPlotSetClearAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "COUNT", iPPlotGetCountAttrib, NULL, NULL, NULL, IUPAF_READONLY|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "CURRENT", iPPlotGetCurrentAttrib, iPPlotSetCurrentAttrib, IUPAF_SAMEASSYSTEM, "-1", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "PLOT_NUMCOL", iPPlotGetPlotNumColAttrib, iPPlotSetPlotNumColAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "PLOT_CURRENT", iPPlotGetPlotCurrentAttrib, iPPlotSetPlotCurrentAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "PLOT_COUNT", iPPlotGetPlotCountAttrib, iPPlotSetPlotCountAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "PLOT_REMOVE", NULL, iPPlotSetPlotRemoveAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "PLOT_INSERT", NULL, iPPlotSetPlotInsertAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);

  return ic;
}

Ihandle* IupPPlot(void)
{
  return IupCreate("pplot");
}

void IupPPlotOpen(void)
{
#ifdef USE_OPENGL
  IupGLCanvasOpen();
#endif

  if (!IupGetGlobal("_IUP_PPLOT_OPEN"))
  {
    iupRegisterClass(iPPlotNewClass());
    IupSetGlobal("_IUP_PPLOT_OPEN", "1");
  }
}
