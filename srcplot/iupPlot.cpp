
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "iupPlot.h"


inline static void iPlotCheckMinMax(double &inoutMin, double &inoutMax)
{
  if (inoutMin > inoutMax) 
  { 
    double theTmp = inoutMin;
    inoutMin = inoutMax;
    inoutMax = theTmp;
  }
}

double iupPlotTrafoLinear::Transform(double inValue) const 
{
  return inValue * mSlope + mOffset;
}

double iupPlotTrafoLinear::TransformBack(double inValue) const 
{
  if (mSlope != 0)
    return (inValue - mOffset) / mSlope;
  else
    return 0;
}

bool iupPlotTrafoLinear::Calculate(int inBegin, int inEnd, const iupPlotAxis& inAxis)
{
  double theDataRange = inAxis.mMax - inAxis.mMin;
  if (theDataRange < kFloatSmall)
    return false;

  double theMin = inAxis.mMin;
  if (inAxis.mDiscrete)
  {
    theDataRange++;
    theMin -= 0.5;
  }

  double theTargetRange = inEnd - inBegin;
  double theScale = theTargetRange / theDataRange;

  if (!inAxis.mReverse)
    mOffset = inBegin - theMin * theScale;
  else
    mOffset = inEnd + theMin * theScale;

  mSlope = theScale;
  if (inAxis.mReverse)
    mSlope *= -1;

  return true;
}


/************************************************************************************************/


double iupPlotTrafoLog::Transform(double inValue) const
{
  if (inValue<kLogMinClipValue) inValue = kLogMinClipValue;
  return iupPlotLog(inValue, mBase)*mSlope + mOffset;
}

double iupPlotTrafoLog::TransformBack(double inValue) const 
{
  if (mSlope != 0)
    return iupPlotExp((inValue - mOffset) / mSlope, mBase);
  else
    return 0;
}

bool iupPlotTrafoLog::Calculate(int inBegin, int inEnd, const iupPlotAxis& inAxis)
{
  double theBase = inAxis.mLogBase;
  double theDataRange = iupPlotLog(inAxis.mMax, theBase) - iupPlotLog(inAxis.mMin, theBase);
  if (theDataRange < kFloatSmall)
    return false;

  double theTargetRange = inEnd - inBegin;
  double theScale = theTargetRange / theDataRange;

  if (!inAxis.mReverse)
    mOffset = inBegin - iupPlotLog(inAxis.mMin, theBase) * theScale;
  else
    mOffset = inEnd + iupPlotLog(inAxis.mMin, theBase) * theScale;

  mBase = theBase;

  mSlope = theScale;
  if (inAxis.mReverse)
    mSlope *= -1;

  return true;
}


/************************************************************************************************/


bool iupPlotDataReal::CalculateRange(double &outMin, double &outMax) const
{
  int theCount = iupArrayCount(mArray);
  if (theCount > 0)
  {
    double* theData = (double*)iupArrayGetData(mArray);
    outMax = outMin = theData[0];
    for (int i = 1; i < theCount; i++)
    {
      if (theData[i] > outMax)
        outMax = theData[i];
      if (theData[i] < outMin)
        outMin = theData[i];
    }
    return true;
  }

  return false;
}

iupPlotDataString::~iupPlotDataString()
{
  for (int i = 0; i < mCount; i++) 
    free(mData[i]);
}

bool iupPlotDataString::CalculateRange(double &outMin, double &outMax) const
{
  if (mCount > 0)
  {
    outMin = 0;
    outMax = mCount - 1;
    return true;
  }

  return false;
}

bool iupPlotDataBool::CalculateRange(double &outMin, double &outMax) const
{
  if (mCount > 0)
  {
    outMin = 0;
    outMax = mCount - 1;
    return true;
  }

  return false;
}


/************************************************************************************************/


iupPlotDataSet::iupPlotDataSet(bool strXdata)
  :mColor(CD_BLACK), mLineStyle(CD_CONTINUOUS), mLineWidth(1), mMarkStyle(CD_X), mMarkSize(7),
   mMode(IUP_PLOT_LINE), mName(NULL), mHasSelected(false)
{
  if (strXdata)
    mDataX = (iupPlotDataBase*)(new iupPlotDataString());
  else
    mDataX = (iupPlotDataBase*)(new iupPlotDataReal());

  mDataY = (iupPlotDataBase*)new iupPlotDataReal();

  mSelection = new iupPlotDataBool();
  mSegment = NULL;
}

iupPlotDataSet::~iupPlotDataSet()
{
  SetName(NULL);

  delete mDataX;
  delete mDataY;
  delete mSelection;
  if (mSegment)
    delete mSegment;
}

bool iupPlotDataSet::FindSample(double inX, double inY, double tolX, double tolY,
                                int &outSampleIndex, double &outX, double &outY) const
{
  int theCount = mDataX->GetCount();
  for (int i = 0; i < theCount; i++)
  {
    outX = mDataX->GetSample(i);
    outY = mDataY->GetSample(i);

    if (fabs(outX - inX) < tolX &&
        fabs(outY - inY) < tolY)
    {
      outSampleIndex = i;
      return true;
    }
  }

  return false;
}

bool iupPlotDataSet::SelectSamples(double inMinX, double inMaxX, double inMinY, double inMaxY, const iupPlotSampleNotify* inNotify)
{
  bool theChanged = false;
  mHasSelected = false;

  int theCount = mDataX->GetCount();
  for (int i = 0; i < theCount; i++)
  {
    double theX = mDataX->GetSample(i);
    double theY = mDataY->GetSample(i);
    bool theSelected = mSelection->GetSampleBool(i);

    if (theX >= inMinX && theX <= inMaxX &&
        theY >= inMinY && theY <= inMaxY)
    {
      mHasSelected = true;

      if (!theSelected)
      {
        if (inNotify)
        {
          int ret = inNotify->cb(inNotify->ih, inNotify->ds, i, theX, theY, (int)theSelected);
          if (ret == IUP_IGNORE)
            continue;
        }

        theChanged = true;
        mSelection->SetSampleBool(i, true);
      }
    }
    else
    {
      if (theSelected)
      {
        if (inNotify)
        {
          int ret = inNotify->cb(inNotify->ih, inNotify->ds, i, theX, theY, (int)theSelected);
          if (ret == IUP_IGNORE)
            continue;
        }

        theChanged = true;
        mSelection->SetSampleBool(i, false);
      }
    }
  }

  return theChanged;
}

bool iupPlotDataSet::ClearSelection(const iupPlotSampleNotify* inNotify)
{
  bool theChanged = false;

  if (!mHasSelected)
    return theChanged;

  mHasSelected = false;

  int theCount = mDataX->GetCount();
  for (int i = 0; i < theCount; i++)
  {
    bool theSelected = mSelection->GetSampleBool(i);
    if (theSelected)
    {
      if (inNotify)
      {
        double theX = mDataX->GetSample(i);
        double theY = mDataY->GetSample(i);
        int ret = inNotify->cb(inNotify->ih, inNotify->ds, i, theX, theY, (int)theSelected);
        if (ret == IUP_IGNORE)
          continue;
      }

      theChanged = true;
      mSelection->SetSampleBool(i, false);
    }
  }

  return theChanged;
}

bool iupPlotDataSet::DeleteSelectedSamples(const iupPlotSampleNotify* inNotify)
{
  bool theChanged = false;

  if (!mHasSelected)
    return theChanged;

  mHasSelected = false;

  int theCount = mDataX->GetCount();
  for (int i = theCount-1; i >= 0; i--)
  {
    bool theSelected = mSelection->GetSampleBool(i);
    if (theSelected)
    {
      if (inNotify)
      {
        double theX = mDataX->GetSample(i);
        double theY = mDataY->GetSample(i);
        int ret = inNotify->cb(inNotify->ih, inNotify->ds, i, theX, theY, (int)theSelected);
        if (ret == IUP_IGNORE)
          continue;
      }

      theChanged = true;
      RemoveSample(i);
    }
  }

  return theChanged;
}

int iupPlotDataSet::GetCount()
{
  return mDataX->GetCount();
}

void iupPlotDataSet::AddSample(double inX, double inY)
{
  iupPlotDataReal *theXData = (iupPlotDataReal*)mDataX;
  iupPlotDataReal *theYData = (iupPlotDataReal*)mDataY;

  if (theXData->IsString())
    return;

  theXData->AddSample(inX);
  theYData->AddSample(inY);
  mSelection->AddSample(false);
  if (mSegment)
    mSegment->AddSample(false);
}

void iupPlotDataSet::InsertSample(int inSampleIndex, double inX, double inY)
{
  iupPlotDataReal *theXData = (iupPlotDataReal*)mDataX;
  iupPlotDataReal *theYData = (iupPlotDataReal*)mDataY;

  if (theXData->IsString())
    return;

  theXData->InsertSample(inSampleIndex, inX);
  theYData->InsertSample(inSampleIndex, inY);
  mSelection->InsertSample(inSampleIndex, false);
  if (mSegment)
    mSegment->InsertSample(inSampleIndex, false);
}

void iupPlotDataSet::InitSegment()
{
  mSegment = new iupPlotDataBool();

  int theCount = mDataX->GetCount();
  for (int i = 0; i < theCount; i++)
    mSegment->AddSample(false);
}

void iupPlotDataSet::AddSampleSegment(double inX, double inY, bool inSegment)
{
  iupPlotDataReal *theXData = (iupPlotDataReal*)mDataX;
  iupPlotDataReal *theYData = (iupPlotDataReal*)mDataY;

  if (theXData->IsString())
    return;

  if (!mSegment)
    InitSegment();

  theXData->AddSample(inX);
  theYData->AddSample(inY);
  mSelection->AddSample(false);
  mSegment->AddSample(inSegment);
}

void iupPlotDataSet::InsertSampleSegment(int inSampleIndex, double inX, double inY, bool inSegment)
{
  iupPlotDataReal *theXData = (iupPlotDataReal*)mDataX;
  iupPlotDataReal *theYData = (iupPlotDataReal*)mDataY;

  if (theXData->IsString())
    return;

  if (!mSegment)
    InitSegment();

  theXData->InsertSample(inSampleIndex, inX);
  theYData->InsertSample(inSampleIndex, inY);
  mSelection->InsertSample(inSampleIndex, false);
  mSegment->InsertSample(inSampleIndex, inSegment);
}

void iupPlotDataSet::AddSample(const char* inX, double inY)
{
  iupPlotDataString *theXData = (iupPlotDataString*)mDataX;
  iupPlotDataReal *theYData = (iupPlotDataReal*)mDataY;

  if (!theXData->IsString())
    return;

  theXData->AddSample(inX);
  theYData->AddSample(inY);
  mSelection->AddSample(false);
}

void iupPlotDataSet::InsertSample(int inSampleIndex, const char* inX, double inY)
{
  iupPlotDataString *theXData = (iupPlotDataString*)mDataX;
  iupPlotDataReal *theYData = (iupPlotDataReal*)mDataY;

  if (!theXData->IsString())
    return;

  theXData->InsertSample(inSampleIndex, inX);
  theYData->InsertSample(inSampleIndex, inY);
  mSelection->InsertSample(inSampleIndex, false);
}

void iupPlotDataSet::RemoveSample(int inSampleIndex)
{
  mDataX->RemoveSample(inSampleIndex);
  mDataY->RemoveSample(inSampleIndex);
  mSelection->RemoveSample(inSampleIndex);
  if (mSegment)
    mSegment->RemoveSample(inSampleIndex);
}

void iupPlotDataSet::GetSample(int inSampleIndex, double *inX, double *inY)
{
  iupPlotDataReal *theXData = (iupPlotDataReal*)mDataX;
  iupPlotDataReal *theYData = (iupPlotDataReal*)mDataY;

  if (theXData->IsString())
    return;

  int theCount = theXData->GetCount();
  if (inSampleIndex < 0 || inSampleIndex >= theCount)
    return;

  if (inX) *inX = theXData->GetSample(inSampleIndex);
  if (inY) *inY = theYData->GetSample(inSampleIndex);
}

void iupPlotDataSet::GetSample(int inSampleIndex, const char* *inX, double *inY)
{
  iupPlotDataString *theXData = (iupPlotDataString*)mDataX;
  iupPlotDataReal *theYData = (iupPlotDataReal*)mDataY;

  if (!theXData->IsString())
    return;

  int theCount = theXData->GetCount();
  if (inSampleIndex < 0 || inSampleIndex >= theCount)
    return;

  if (inX) *inX = theXData->GetSampleString(inSampleIndex);
  if (inY) *inY = theYData->GetSample(inSampleIndex);
}

bool iupPlotDataSet::GetSampleSelection(int inSampleIndex)
{
  int theCount = mDataX->GetCount();
  if (inSampleIndex < 0 || inSampleIndex >= theCount)
    return false;

  return mSelection->GetSampleBool(inSampleIndex);
}

void iupPlotDataSet::SetSample(int inSampleIndex, double inX, double inY)
{
  iupPlotDataReal *theXData = (iupPlotDataReal*)mDataX;
  iupPlotDataReal *theYData = (iupPlotDataReal*)mDataY;

  if (theXData->IsString())
    return;

  int theCount = theXData->GetCount();
  if (inSampleIndex < 0 || inSampleIndex >= theCount)
    return;

  theXData->SetSample(inSampleIndex, inX);
  theYData->SetSample(inSampleIndex, inY);
}

void iupPlotDataSet::SetSample(int inSampleIndex, const char* inX, double inY)
{
  iupPlotDataString *theXData = (iupPlotDataString*)mDataX;
  iupPlotDataReal *theYData = (iupPlotDataReal*)mDataY;

  if (!theXData->IsString())
    return;

  int theCount = theXData->GetCount();
  if (inSampleIndex < 0 || inSampleIndex >= theCount)
    return;

  theXData->SetSampleString(inSampleIndex, inX);
  theYData->SetSample(inSampleIndex, inY);
}

void iupPlotDataSet::SetSampleSelection(int inSampleIndex, bool inSelected)
{
  int theCount = mDataX->GetCount();
  if (inSampleIndex < 0 || inSampleIndex >= theCount)
    return;

  mSelection->SetSampleBool(inSampleIndex, inSelected);
}


/************************************************************************************************/


void iupPlotAxis::Init()
{
  if (mLogScale)
  {
    mTickIter = &mLogTickIter;
    mTrafo = &mLogTrafo;
  }
  else
  {
    mTickIter = &mLinTickIter;
    mTrafo = &mLinTrafo;
  }

  mTickIter->SetAxis(this);
}

void iupPlotAxis::SetNamedTickIter(const iupPlotDataString *inStringData)
{
  mTickIter = &mNamedTickIter;
  mTickIter->SetAxis(this);
  mNamedTickIter.SetStringList(inStringData);
}

void iupPlotAxis::CheckZoomOutLimit(double inRange)
{
  if (mMin < mNoZoomMin)
  {
    mMin = mNoZoomMin;
    mMax = mMin + inRange;
    if (mMax > mNoZoomMax)
      mMax = mNoZoomMax;
  }

  if (mMax > mNoZoomMax)
  {
    mMax = mNoZoomMax;
    mMin = mMax - inRange;
    if (mMin < mNoZoomMin)
      mMin = mNoZoomMin;
  }
}

void iupPlotAxis::InitZoom()
{
  if (!mHasZoom)
  {
    mHasZoom = true;
    mAutoScaleMin = false;
    mAutoScaleMax = false;

    mNoZoomMin = mMin;
    mNoZoomMax = mMax;
    mNoZoomAutoScaleMin = mAutoScaleMin;
    mNoZoomAutoScaleMax = mAutoScaleMax;
  }
}

bool iupPlotAxis::ResetZoom()
{
  if (mHasZoom)
  {
    mHasZoom = false;

    mMin = mNoZoomMin;
    mMax = mNoZoomMax;
    mAutoScaleMin = mNoZoomAutoScaleMin;
    mAutoScaleMax = mNoZoomAutoScaleMax;
    return true;
  }

  return false;
}

bool iupPlotAxis::ZoomOut(double inCenter)
{
  if (inCenter < mMin || inCenter > mMax)
    return false;

  if (!mHasZoom)
    return false;

  double theRange = mMax - mMin;
  double theNewRange = theRange * 1.1; // 10%
  double theFactor = (inCenter - mMin) / theRange;
  double theOffset = (theNewRange - theRange);

  mMin -= theOffset*theFactor;
  mMax += theOffset*(1.0 - theFactor);

  CheckZoomOutLimit(theNewRange);

  if (mMin == mNoZoomMin && mMax == mNoZoomMax)
    ResetZoom();

  return true;
}

bool iupPlotAxis::ZoomIn(double inCenter)
{
  if (inCenter < mMin || inCenter > mMax)
    return false;

  InitZoom();

  double theRange = mMax - mMin;
  double theNewRange = theRange * 0.9; // 10%
  double theFactor = (inCenter - mMin) / theRange;
  double theOffset = (theRange - theNewRange);

  mMin += theOffset*theFactor;
  mMax -= theOffset*(1.0 - theFactor);

  // Check limits only in ZoomOut and Pan
  return true;
}

bool iupPlotAxis::ZoomTo(double inMin, double inMax)
{
  InitZoom();

  iPlotCheckMinMax(inMin, inMax);

  if (inMin > mNoZoomMax || inMax < mNoZoomMin)
    return false;

  if (inMin < mNoZoomMin) inMin = mNoZoomMin;
  if (inMax > mNoZoomMax) inMax = mNoZoomMax;

  mMin = inMin;
  mMax = inMax;

  if (mMin == mNoZoomMin && mMax == mNoZoomMax)
    ResetZoom();

  return true;
}

bool iupPlotAxis::Pan(double inOffset)
{
  if (!mHasZoom)
    return false;

  double theRange = mMax - mMin;

  mMin = mPanMin - inOffset;
  mMax = mMin + theRange;

  CheckZoomOutLimit(theRange);
  return true;
}

bool iupPlotAxis::Scroll(double inDelta, bool inFullPage)
{
  if (!mHasZoom)
    return false;

  double theRange = mMax - mMin;

  double thePage;
  if (inFullPage)
    thePage = theRange;
  else
    thePage = theRange / 10.0;

  double theOffset = thePage * inDelta;

  mMin += theOffset;
  mMax += theOffset;

  CheckZoomOutLimit(theRange);
  return true;
}

bool iupPlotAxis::ScrollTo(double inMin)
{
  if (inMin < mNoZoomMin || inMin > mNoZoomMax)
    return false;

  if (!mHasZoom)
    return false;

  double theRange = mMax - mMin;

  mMin = inMin;
  mMax = mMin + theRange;

  CheckZoomOutLimit(theRange);
  return true;
}

void iupPlotAxis::SetFont(cdCanvas* canvas, int inFontStyle, int inFontSize) const
{
  if (inFontStyle == -1) inFontStyle = mDefaultFontStyle;
  if (inFontSize == 0) inFontSize = mDefaultFontSize;
  cdCanvasFont(canvas, NULL, inFontStyle, inFontSize);
}


/************************************************************************************************/


iupPlot::iupPlot(Ihandle* _ih, int inDefaultFontStyle, int inDefaultFontSize)
  :ih(_ih), mCurrentDataSet(-1), mRedraw(true), mDataSetListCount(0), mCrossHairH(false),
  mGrid(true), mGridMinor(false), mViewportSquare(false),
  mDefaultFontSize(inDefaultFontSize), mDefaultFontStyle(inDefaultFontStyle), 
  mAxisX(inDefaultFontStyle, inDefaultFontSize), mAxisY(inDefaultFontStyle, inDefaultFontSize)
{
}

iupPlot::~iupPlot()
{
  RemoveAllDataSets();
}

void iupPlot::SetViewport(int x, int y, int w, int h)
{
  if (mViewportSquare && w != h)
  {
    if (w > h)
    {
      mViewport.mX = x + (w - h) / 2;
      mViewport.mY = y;
      mViewport.mWidth = h;
      mViewport.mHeight = h;
    }
    else
    {
      mViewport.mX = x;
      mViewport.mY = y + (h - w) / 2;
      mViewport.mWidth = w;
      mViewport.mHeight = w;
    }
  }
  else
  {
    mViewport.mX = x;
    mViewport.mY = y;
    mViewport.mWidth = w;
    mViewport.mHeight = h;
  }

  mRedraw = true;
}

void iupPlot::SetFont(cdCanvas* canvas, int inFontStyle, int inFontSize) const
{
  if (inFontStyle == -1) inFontStyle = mDefaultFontStyle;
  if (inFontSize == 0) inFontSize = mDefaultFontSize;
  cdCanvasFont(canvas, NULL, inFontStyle, inFontSize);
}

static long iPlotGetDefaultColor(int index)
{
  switch (index)
  {
  case 0: return cdEncodeColor(255, 0, 0);
  case 1: return cdEncodeColor(0, 255, 0);
  case 2: return cdEncodeColor(0, 0, 255);
  case 3: return cdEncodeColor(0, 255, 255);
  case 4: return cdEncodeColor(255, 0, 255);
  case 5: return cdEncodeColor(255, 255, 0);
  case 6: return cdEncodeColor(128, 0, 0);
  case 7: return cdEncodeColor(0, 128, 0);
  case 8: return cdEncodeColor(0, 0, 128);
  case 9: return cdEncodeColor(0, 128, 128);
  case 10: return cdEncodeColor(128, 0, 128);
  case 11: return cdEncodeColor(128, 128, 0);
  default: return cdEncodeColor(0, 0, 0);  // the last must be always black
  }
}

long iupPlot::GetNextDataSetColor()
{
  int def_color = 0, i = 0;
  long theColor;

  do
  {
    theColor = iPlotGetDefaultColor(def_color);

    for (i = 0; i<mDataSetListCount; i++)
    {
      // already used, get another
      long theDataSetColor = cdEncodeAlpha(mDataSetList[i]->mColor, 255);
      if (theDataSetColor == theColor)
          break;
    }

    // not found, use it
    if (i == mDataSetListCount)
      break;

    def_color++;
  } while (def_color<12);

  return theColor;
}

void iupPlot::AddDataSet(iupPlotDataSet* inDataSet)
{
  if (mDataSetListCount < IUP_PLOT_MAX_DS)
  {
    long theColor = GetNextDataSetColor();

    mCurrentDataSet = mDataSetListCount;
    mDataSetListCount++;

    char theLegend[30];
    sprintf(theLegend, "plot %d", mCurrentDataSet);

    mDataSetList[mCurrentDataSet] = inDataSet;
    
    inDataSet->SetName(theLegend);
    inDataSet->mColor = theColor;
  }
}

void iupPlot::RemoveDataSet(int inIndex)
{
  if (mCurrentDataSet == mDataSetListCount - 1)
    mCurrentDataSet--;

  delete mDataSetList[inIndex];

  for (int i = inIndex; i<mDataSetListCount; i++)
    mDataSetList[i] = mDataSetList[i + 1];

  mDataSetList[mDataSetListCount - 1] = NULL;

  mDataSetListCount--;
}

int iupPlot::FindDataSet(const char* inName)
{
  for (int ds = 0; ds < mDataSetListCount; ds++)
  {
    if (iupStrEqualNoCase(mDataSetList[ds]->GetName(), inName))
      return ds;
  }
  return -1;
}

void iupPlot::RemoveAllDataSets()
{
  for (int ds = 0; ds < mDataSetListCount; ds++)
  {
    delete mDataSetList[ds];
  }
  mDataSetListCount = 0;
}

bool iupPlot::FindDataSetSample(double inX, double inY, int &outIndex, const char* &outName, int &outSampleIndex, double &outX, double &outY, const char* &outStrX) const
{
  if (!mAxisX.mTrafo || !mAxisY.mTrafo)
    return false;

  double theX = mAxisX.mTrafo->TransformBack(inX);
  double theY = mAxisY.mTrafo->TransformBack(inY);
  double tolX = (fabs(mAxisX.mMax - mAxisX.mMin) / mViewport.mWidth) * 5.0;  // 5 pixels tolerance
  double tolY = (fabs(mAxisY.mMax - mAxisY.mMin) / mViewport.mHeight) * 5.0;

  for (int ds = 0; ds < mDataSetListCount; ds++)
  {
    iupPlotDataSet* dataset = mDataSetList[ds];

    if (dataset->FindSample(theX, theY, tolX, tolY, outSampleIndex, outX, outY))
    {
      const iupPlotDataBase *theXData = dataset->GetDataX();
      if (theXData->IsString())
      {
        const iupPlotDataString *theStringXData = (const iupPlotDataString *)(theXData);
        outStrX = theStringXData->GetSampleString(outSampleIndex);
      }
      else
        outStrX = NULL;

      outIndex = ds;
      outName = dataset->GetName();
      return true;
    }
  }
  return false;
}

void iupPlot::SelectDataSetSamples(double inMinX, double inMaxX, double inMinY, double inMaxY)
{
  bool theChanged = false;

  iPlotCheckMinMax(inMinX, inMaxX);
  iPlotCheckMinMax(inMinY, inMaxY);

  IFniiddi select_cb = (IFniiddi)IupGetCallback(ih, "SELECT_CB");
  if (select_cb)
  {
    Icallback cb = IupGetCallback(ih, "SELECTBEGIN_CB");
    if (cb && cb(ih) == IUP_IGNORE)
      return;
  }

  for (int ds = 0; ds < mDataSetListCount; ds++)
  {
    iupPlotDataSet* dataset = mDataSetList[ds];

    if (select_cb)
    {
      iupPlotSampleNotify inNotify = { ih, ds, select_cb };
      if (dataset->SelectSamples(inMinX, inMaxX, inMinY, inMaxY, &inNotify))
        theChanged = true;
    }
    else
    {
      if (dataset->SelectSamples(inMinX, inMaxX, inMinY, inMaxY, NULL))
        theChanged = true;
    }
  }

  if (select_cb)
  {
    Icallback cb = IupGetCallback(ih, "SELECTEND_CB");
    if (cb)
      return;
  }

  if (theChanged)
    mRedraw = true;
}

void iupPlot::ClearDataSetSelection()
{
  bool theChanged = false;

  IFniiddi select_cb = (IFniiddi)IupGetCallback(ih, "SELECT_CB");
  if (select_cb)
  {
    Icallback cb = IupGetCallback(ih, "SELECTBEGIN_CB");
    if (cb && cb(ih) == IUP_IGNORE)
      return;
  }

  for (int ds = 0; ds < mDataSetListCount; ds++)
  {
    iupPlotDataSet* dataset = mDataSetList[ds];

    if (select_cb)
    {
      iupPlotSampleNotify inNotify = { ih, ds, select_cb };
      if (dataset->ClearSelection(&inNotify))
        theChanged = true;
    }
    else
    {
      if (dataset->ClearSelection(NULL))
        theChanged = true;
    }
  }

  if (select_cb)
  {
    Icallback cb = IupGetCallback(ih, "SELECTEND_CB");
    if (cb)
      return;
  }

  if (theChanged)
    mRedraw = true;
}

void iupPlot::DeleteSelectedDataSetSamples()
{
  bool theChanged = false;

  IFniiddi select_cb = (IFniiddi)IupGetCallback(ih, "DELETE_CB");
  if (select_cb)
  {
    Icallback cb = IupGetCallback(ih, "DELETEBEGIN_CB");
    if (cb && cb(ih) == IUP_IGNORE)
      return;
  }

  for (int ds = 0; ds < mDataSetListCount; ds++)
  {
    iupPlotDataSet* dataset = mDataSetList[ds];

    if (select_cb)
    {
      iupPlotSampleNotify inNotify = { ih, ds, select_cb };
      if (dataset->DeleteSelectedSamples(&inNotify))
        theChanged = true;
    }
    else
    {
      if (dataset->DeleteSelectedSamples(NULL))
        theChanged = true;
    }
  }

  if (select_cb)
  {
    Icallback cb = IupGetCallback(ih, "DELETEEND_CB");
    if (cb)
      return;
  }

  if (theChanged)
    mRedraw = true;
}

void iupPlot::ConfigureAxis()
{
  mAxisX.Init();
  mAxisY.Init();

  if (mAxisX.mLogScale)
    mAxisY.mCrossOrigin = false;  // change at the other axis
  else
  {
    const iupPlotDataBase *theXData = mDataSetList[0]->GetDataX();
    if (theXData->IsString())
    {
      const iupPlotDataString *theStringXData = (const iupPlotDataString *)(theXData);
      mAxisX.SetNamedTickIter(theStringXData);
    }
  }

  if (mAxisY.mLogScale)
    mAxisX.mCrossOrigin = false;   // change at the other axis
}

bool iupPlot::Render(cdCanvas* canvas)
{
  if (!mRedraw)
    return true;

  // Shift the drawing area to the plot viewport
  cdCanvasOrigin(canvas, mViewport.mX, mViewport.mY);

  // There are no additional transformations set in the CD canvas,
  // all transformations are done here.

  cdCanvasClip(canvas, CD_CLIPAREA);

  // Draw the background, axis and grid restricted only by the viewport
  cdCanvasClipArea(canvas, 0, mViewport.mWidth - 1, 0, mViewport.mHeight - 1);

  // draw entire plot viewport
  DrawBackground(canvas);

  if (!mDataSetListCount)
    return true;

  cdCanvasNativeFont(canvas, IupGetAttribute(ih, "FONT"));

  ConfigureAxis();

  if (!CalculateAxisRange())
    return false;

  if (!CheckRange(mAxisX))
    return false;

  if (!CheckRange(mAxisY))
    return false;

  CalculateTitlePos();

  // Must be before calculate margins
  CalculateTickSize(canvas, mAxisX.mTick);
  CalculateTickSize(canvas, mAxisY.mTick);

  CalculateMargins(canvas);

  iupPlotRect theDatasetArea;
  theDatasetArea.mX = mBack.mMargin.mLeft;
  theDatasetArea.mY = mBack.mMargin.mBottom;
  theDatasetArea.mWidth = mViewport.mWidth - mBack.mMargin.mLeft - mBack.mMargin.mRight;
  theDatasetArea.mHeight = mViewport.mHeight - mBack.mMargin.mTop - mBack.mMargin.mBottom;

  if (!CalculateTickSpacing(theDatasetArea, canvas))
    return false;

  if (!CalculateXTransformation(theDatasetArea))
    return false;

  if (!CalculateYTransformation(theDatasetArea))
    return false;

  IFnC pre_cb = (IFnC)IupGetCallback(ih, "PREDRAW_CB");
  if (pre_cb)
    pre_cb(ih, canvas);

  if (mBack.GetImage())
    DrawBackgroundImage(canvas);

  if (!mGrid.DrawX(mAxisX.mTickIter, mAxisX.mTrafo, theDatasetArea, canvas))
    return false;

  if (mGrid.mShowX)
    mGridMinor.DrawX(mAxisX.mTickIter, mAxisX.mTrafo, theDatasetArea, canvas);

  if (!mGrid.DrawY(mAxisY.mTickIter, mAxisY.mTrafo, theDatasetArea, canvas))
    return false;

  if (mGrid.mShowY)
    mGridMinor.DrawY(mAxisY.mTickIter, mAxisY.mTrafo, theDatasetArea, canvas);

  if (!mAxisX.DrawX(theDatasetArea, canvas, mAxisY))
    return false;

  if (!mAxisY.DrawY(theDatasetArea, canvas, mAxisX))
    return false;

  if (mBox.mShow)
    mBox.Draw(theDatasetArea, canvas);

  // draw the datasets, legend, crosshair and selection restricted to the dataset area
  cdCanvasClipArea(canvas, theDatasetArea.mX, theDatasetArea.mX + theDatasetArea.mWidth - 1, theDatasetArea.mY, theDatasetArea.mY + theDatasetArea.mHeight - 1);

  IFniiddi drawsample_cb = (IFniiddi)IupGetCallback(ih, "DRAWSAMPLE_CB");

  for (int ds = 0; ds < mDataSetListCount; ds++) 
  {
    iupPlotDataSet* dataset = mDataSetList[ds];

    if (drawsample_cb)
    {
      iupPlotSampleNotify inNotify = { ih, ds, drawsample_cb };
      dataset->DrawData(mAxisX.mTrafo, mAxisY.mTrafo, canvas, &inNotify);
    }
    else
      dataset->DrawData(mAxisX.mTrafo, mAxisY.mTrafo, canvas, NULL);
  }

  if (mCrossHairH)
    DrawCrossHairH(theDatasetArea, canvas);
  else if (mCrossHairV)
    DrawCrossHairV(theDatasetArea, canvas);

  if (mShowSelectionBand)
  {
    if (mSelectionBand.mX < theDatasetArea.mX) 
    { 
      mSelectionBand.mWidth = mSelectionBand.mX + mSelectionBand.mWidth - theDatasetArea.mX; 
      mSelectionBand.mX = theDatasetArea.mX; 
    }
    if (mSelectionBand.mY < theDatasetArea.mY) 
    {
      mSelectionBand.mHeight = mSelectionBand.mY + mSelectionBand.mHeight - theDatasetArea.mY; 
      mSelectionBand.mY = theDatasetArea.mY;
    }
    if (mSelectionBand.mX + mSelectionBand.mWidth > theDatasetArea.mX + theDatasetArea.mWidth)
      mSelectionBand.mWidth = theDatasetArea.mX + theDatasetArea.mWidth - mSelectionBand.mX;
    if (mSelectionBand.mY + mSelectionBand.mHeight > theDatasetArea.mY + theDatasetArea.mHeight)
      mSelectionBand.mHeight = theDatasetArea.mY + theDatasetArea.mHeight - mSelectionBand.mY;

    mBox.Draw(mSelectionBand, canvas);
  }

  IFnC post_cb = (IFnC)IupGetCallback(ih, "POSTDRAW_CB");
  if (post_cb)
    post_cb(ih, canvas);

  if (!DrawLegend(theDatasetArea, canvas, mLegend.mPos))
    return false;

  // Draw title restricted only by the viewport
  cdCanvasClipArea(canvas, 0, mViewport.mWidth - 1, 0, mViewport.mHeight - 1);

  DrawTitle(canvas);

  if (!IupGetInt(ih, "ACTIVE"))
    DrawInactive(canvas);

  mRedraw = false;
  return true;
}

