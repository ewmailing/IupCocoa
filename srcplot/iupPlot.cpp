
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

inline static bool iPlotCheckInsideBox(double x, double y, double boxMinX, double boxMaxX, double boxMinY, double boxMaxY)
{
  if (x > boxMaxX || x < boxMinX || y > boxMaxY || y < boxMinY)
    return false;

  return true;
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
  if (inValue < kLogMinClipValue) inValue = kLogMinClipValue;
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
: mColor(CD_BLACK), mLineStyle(CD_CONTINUOUS), mLineWidth(1), mAreaTransparency(255), mMarkStyle(CD_X), mMarkSize(7),
  mMultibarIndex(-1), mMultibarCount(0), mBarOutlineColor(0), mBarShowOutline(false), mBarSpacingPercent(10),
  mPieStartAngle(0), mPieRadius(0.95), mPieContour(false), mPieHole(0), mPieSliceLabelPos(0.95),
  mHighlightedSample(-1), mHighlightedCurve(false), mBarMulticolor(false), mOrderedX(false),
  mPieSliceLabel(IUP_PLOT_NONE), mMode(IUP_PLOT_LINE), mName(NULL), mHasSelected(false)
{
  if (strXdata)
    mDataX = (iupPlotDataBase*)(new iupPlotDataString());
  else
    mDataX = (iupPlotDataBase*)(new iupPlotDataReal());

  mDataY = (iupPlotDataBase*)new iupPlotDataReal();

  mSelection = new iupPlotDataBool();
  mSegment = NULL;
  mExtra = NULL;
}

iupPlotDataSet::~iupPlotDataSet()
{
  SetName(NULL);

  delete mDataX;
  delete mDataY;
  delete mSelection;
  if (mSegment)
    delete mSegment;
  if (mExtra)
    delete mExtra;
}

bool iupPlotDataSet::FindSample(iupPlotTrafoBase *inTrafoX, iupPlotTrafoBase *inTrafoY, double inScreenX, double inScreenY, double inScreenTolerance,
                                int &outSampleIndex, double &outX, double &outY) const
{
  switch (mMode)
  {
  case IUP_PLOT_MULTIBAR:
    return this->FindMultipleBarSample(inTrafoX, inTrafoY, inScreenX, inScreenY, outSampleIndex, outX, outY);
  case IUP_PLOT_BAR:
    return this->FindBarSample(inTrafoX, inTrafoY, inScreenX, inScreenY, outSampleIndex, outX, outY);
  case IUP_PLOT_HORIZONTALBAR:
    return this->FindHorizontalBarSample(inTrafoX, inTrafoY, inScreenX, inScreenY, outSampleIndex, outX, outY);
  case IUP_PLOT_PIE:
    return this->FindPieSample(inTrafoX, inTrafoY, inScreenX, inScreenY, outSampleIndex, outX, outY);
  default:
    return this->FindPointSample(inTrafoX, inTrafoY, inScreenX, inScreenY, inScreenTolerance, outSampleIndex, outX, outY);
  }
}

bool iupPlotDataSet::FindPointSample(iupPlotTrafoBase *inTrafoX, iupPlotTrafoBase *inTrafoY, double inScreenX, double inScreenY, double inScreenTolerance,
                                     int &outSampleIndex, double &outX, double &outY) const
{
  double theX, theY, theScreenX, theScreenY;
  double thePrevScreenX = 0;

  int theCount = mDataX->GetCount();

  for (int i = 0; i < theCount; i++)
  {
    theX = mDataX->GetSample(i);
    theY = mDataY->GetSample(i);
    theScreenX = inTrafoX->Transform(theX);
    theScreenY = inTrafoY->Transform(theY);

    // optimization when X values are ordered
    if (mOrderedX && i > 0 && (inScreenX < thePrevScreenX - inScreenTolerance || inScreenX > theScreenX + inScreenTolerance))
    {
      if (inScreenX < thePrevScreenX - inScreenTolerance)
        break;

      thePrevScreenX = theScreenX;
      continue;
    }

    if (fabs(theScreenX - inScreenX) < inScreenTolerance &&
        fabs(theScreenY - inScreenY) < inScreenTolerance)
    {
      outX = theX;
      outY = theY;
      outSampleIndex = i;
      return true;
    }

    thePrevScreenX = theScreenX;
  }

  return false;
}

bool iupPlotDataSet::FindMultipleBarSample(iupPlotTrafoBase *inTrafoX, iupPlotTrafoBase *inTrafoY, double inScreenX, double inScreenY,
                                           int &outSampleIndex, double &outX, double &outY) const
{
  double theX, theY, theScreenX, theScreenY;

  int theCount = mDataX->GetCount();

  double theScreenY0 = inTrafoY->Transform(0);

  double theMinX = mDataX->GetSample(0);
  double theScreenMinX = inTrafoX->Transform(theMinX);
  double theMaxX = mDataX->GetSample(theCount - 1);
  double theScreenMaxX = inTrafoX->Transform(theMaxX);

  double theTotalBarWidth = (theScreenMaxX - theScreenMinX) / (theCount - 1);
  theTotalBarWidth *= 1 - (double)mBarSpacingPercent / 100.0;
  double theBarWidth = theTotalBarWidth / mMultibarCount;

  for (int i = 0; i < theCount; i++)
  {
    theX = mDataX->GetSample(i);
    theY = mDataY->GetSample(i);
    theScreenX = inTrafoX->Transform(theX);
    theScreenY = inTrafoY->Transform(theY);

    double theBarX = (theScreenX - theTotalBarWidth / 2) + (mMultibarIndex*theBarWidth);
    double theBarHeight = theScreenY - theScreenY0;

    if (iPlotCheckInsideBox(inScreenX, inScreenY, theBarX, theBarX + theBarWidth, theScreenY0, theScreenY0 + theBarHeight))
    {
      outX = theX;
      outY = theY;
      outSampleIndex = i;
      return true;
    }
  }

  return false;
}

bool iupPlotDataSet::FindBarSample(iupPlotTrafoBase *inTrafoX, iupPlotTrafoBase *inTrafoY, double inScreenX, double inScreenY,
                                   int &outSampleIndex, double &outX, double &outY) const
{
  double theX, theY, theScreenX, theScreenY;

  int theCount = mDataX->GetCount();

  double theScreenY0 = inTrafoY->Transform(0);

  double theMinX = mDataX->GetSample(0);
  double theScreenMinX = inTrafoX->Transform(theMinX);
  double theMaxX = mDataX->GetSample(theCount - 1);
  double theScreenMaxX = inTrafoX->Transform(theMaxX);

  double theBarWidth = (theScreenMaxX - theScreenMinX) / (theCount - 1);
  theBarWidth *= 1 - (double)mBarSpacingPercent / 100.0;

  for (int i = 0; i < theCount; i++)
  {
    theX = mDataX->GetSample(i);
    theY = mDataY->GetSample(i);
    theScreenX = inTrafoX->Transform(theX);
    theScreenY = inTrafoY->Transform(theY);

    double theBarX = theScreenX - theBarWidth / 2;
    double theBarHeight = theScreenY - theScreenY0;

    if (iPlotCheckInsideBox(inScreenX, inScreenY, theBarX, theBarX + theBarWidth, theScreenY0, theScreenY0 + theBarHeight))
    {
      outX = theX;
      outY = theY;
      outSampleIndex = i;
      return true;
    }
  }

  return false;
}

bool iupPlotDataSet::FindHorizontalBarSample(iupPlotTrafoBase *inTrafoX, iupPlotTrafoBase *inTrafoY, double inScreenX, double inScreenY,
                                             int &outSampleIndex, double &outX, double &outY) const
{
  double theX, theY, theScreenX, theScreenY;

  int theCount = mDataX->GetCount();

  double theScreenX0 = inTrafoX->Transform(0);

  double theMinY = mDataY->GetSample(0);
  double theScreenMinY = inTrafoY->Transform(theMinY);
  double theMaxY = mDataY->GetSample(theCount - 1);
  double theScreenMaxY = inTrafoY->Transform(theMaxY);

  double theBarHeight = (theScreenMaxY - theScreenMinY) / (theCount - 1);
  theBarHeight *= 1 - (double)mBarSpacingPercent / 100.0;

  for (int i = 0; i < theCount; i++)
  {
    theX = mDataX->GetSample(i);
    theY = mDataY->GetSample(i);
    theScreenX = inTrafoX->Transform(theX);
    theScreenY = inTrafoY->Transform(theY);

    double theBarY = theScreenY - theBarHeight / 2;
    double theBarWidth = theScreenX - theScreenX0;

    if (iPlotCheckInsideBox(inScreenX, inScreenY, theScreenX0, theScreenX0 + theBarWidth, theBarY, theBarY + theBarHeight))
    {
      outX = theX;
      outY = theY;
      outSampleIndex = i;
      return true;
    }
  }

  return false;
}

bool iupPlotDataSet::FindPieSample(iupPlotTrafoBase *inTrafoX, iupPlotTrafoBase *inTrafoY, double inScreenX, double inScreenY, int &outSampleIndex, double &outX, double &outY) const
{
  double theX, theY;

  int theCount = mDataX->GetCount();
  double sum = 0;

  for (int i = 0; i < theCount; i++)
  {
    double theY = mDataY->GetSample(i);

    if (theY <= 0)
      continue;

    sum += theY;
  }

  double inX = inTrafoX->TransformBack(inScreenX);
  double inY = inTrafoY->TransformBack(inScreenY);

  double inRadius = sqrt(inX*inX + inY*inY);

  double holeRadius = mPieHole * mPieRadius;

  double inAngle = atan2(inY, inX);

  inAngle = CD_RAD2DEG*inAngle;

  if (inAngle < 0)
    inAngle += 360.;

  if (inRadius < holeRadius || inRadius > mPieRadius)
    return false;

  double startAngle = mPieStartAngle;

  for (int i = 0; i < theCount; i++)
  {
    theX = mDataX->GetSample(i);
    theY = mDataY->GetSample(i);

    if (theY <= 0)
      continue;

    double angle = (theY*360.) / sum;

    if (inAngle > startAngle  &&
        inAngle < startAngle + angle)
    {
      outX = theX;
      outY = theY;
      outSampleIndex = i;
      return true;
    }

    startAngle += angle;
  }

  return false;
}

static bool iPlotCheckInsideBoxTolerance(double x1, double y1, double x2, double y2, double inX, double inY, double inTolerance)
{
  if (x1 > x2)
  {
    double tmp = x1;
    x1 = x2;
    x2 = tmp;
  }

  if (y1 > y2)
  {
    double tmp = y1;
    y1 = y2;
    y2 = tmp;
  }

  x1 -= inTolerance;
  x2 += inTolerance;

  y1 -= inTolerance;
  y2 += inTolerance;

  if (inX < x1 || inX > x2)
    return false;

  if (inY < y1 || inY > y2)
    return false;

  return true;
}

bool iupPlotDataSet::FindSegment(iupPlotTrafoBase *mTrafoX, iupPlotTrafoBase *mTrafoY, double inScreenX, double inScreenY, double inScreenTolerance,
                                 int &outSampleIndex1, int &outSampleIndex2, double &outX1, double &outY1, double &outX2, double &outY2) const
{
  if (!mTrafoX || !mTrafoY)
    return false;

  double lowestDist = 0;
  int found_Id = -1;
  double found_x1 = 0, found_y1 = 0, found_x2 = 0, found_y2 = 0;
  bool found = false;

  double theX1 = mDataX->GetSample(0);
  double theY1 = mDataY->GetSample(0);
  double theScreenX1 = mTrafoX->Transform(theX1);
  double theScreenY1 = mTrafoY->Transform(theY1);

  int theCount = mDataX->GetCount();
  for (int i = 0; i < theCount - 1; i++)
  {
    double theX2 = mDataX->GetSample(i + 1);
    double theY2 = mDataY->GetSample(i + 1);
    double theScreenX2 = mTrafoX->Transform(theX2);
    double theScreenY2 = mTrafoY->Transform(theY2);

    // optimization when X values are ordered
    if (mOrderedX && (inScreenX < theScreenX1 || inScreenX > theScreenX2))
    {
      if (inScreenX < theScreenX1)
        break;

      theX1 = theX2;
      theY1 = theY2;
      theScreenX1 = theScreenX2;
      theScreenY1 = theScreenY2;
      continue;
    }

    // inX,inY must be inside box theScreenX1,theScreenY1 - theScreenX2,theScreenY2
    if (!iPlotCheckInsideBoxTolerance(theScreenX1, theScreenY1, theScreenX2, theScreenY2, inScreenX, inScreenY, inScreenTolerance))
    {
      theX1 = theX2;
      theY1 = theY2;
      theScreenX1 = theScreenX2;
      theScreenY1 = theScreenY2;
      continue;
    }

    double v1x = theScreenX2 - theScreenX1;
    double v1y = theScreenY2 - theScreenY1;

    double v1 = v1x*v1x + v1y*v1y;

    double v2x = inScreenX - theScreenX1;
    double v2y = inScreenY - theScreenY1;

    double prod = v1x*v2x + v1y*v2y;

    if (v1 == 0.)
    {
      theX1 = theX2;
      theY1 = theY2;
      theScreenX1 = theScreenX2;
      theScreenY1 = theScreenY2;
      continue;
    }

    double p1 = prod / v1;

    if (p1<0. || p1>1.)
    {
      theX1 = theX2;
      theY1 = theY2;
      theScreenX1 = theScreenX2;
      theScreenY1 = theScreenY2;
      continue;
    }

    double px = theScreenX1 + (theScreenX2 - theScreenX1)*p1;
    double py = theScreenY1 + (theScreenY2 - theScreenY1)*p1;

    double d = sqrt((inScreenX - px)*(inScreenX - px) + (inScreenY - py)*(inScreenY - py));

    if (!found || fabs(d) < lowestDist)
    {
      lowestDist = fabs(d);
      found_Id = i;
      found_x1 = theX1;
      found_x2 = theX2;
      found_y1 = theY1;
      found_y2 = theY2;
      found = true;
    }

    theX1 = theX2;
    theY1 = theY2;
    theScreenX1 = theScreenX2;
    theScreenY1 = theScreenY2;
  }

  if (found && lowestDist < inScreenTolerance)
  {
    outSampleIndex1 = found_Id;
    outSampleIndex2 = found_Id + 1;
    outX1 = found_x1;
    outY1 = found_y1;
    outX2 = found_x2;
    outY2 = found_y2;
    return true;
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
        if (inNotify->cb)
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
        if (inNotify->cb)
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
      if (inNotify->cb)
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
  for (int i = theCount - 1; i >= 0; i--)
  {
    bool theSelected = mSelection->GetSampleBool(i);
    if (theSelected)
    {
      if (inNotify->cb)
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
  if (mExtra)
    mExtra->AddSample(0);
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
  if (mExtra)
    mExtra->InsertSample(inSampleIndex, 0);
}

void iupPlotDataSet::InitSegment()
{
  mSegment = new iupPlotDataBool();

  int theCount = mDataX->GetCount();
  for (int i = 0; i < theCount; i++)
    mSegment->AddSample(false);
}

void iupPlotDataSet::InitExtra()
{
  mExtra = new iupPlotDataReal();

  int theCount = mDataX->GetCount();
  for (int i = 0; i < theCount; i++)
    mExtra->AddSample(0);
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
  if (mExtra)
    mExtra->AddSample(0);
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
  if (mExtra)
    mExtra->InsertSample(inSampleIndex, 0);
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
  if (mSegment)
    mSegment->AddSample(false);
  if (mExtra)
    mExtra->AddSample(0);
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
  if (mSegment)
    mSegment->InsertSample(inSampleIndex, false);
  if (mExtra)
    mExtra->InsertSample(inSampleIndex, 0);
}

void iupPlotDataSet::RemoveSample(int inSampleIndex)
{
  mDataX->RemoveSample(inSampleIndex);
  mDataY->RemoveSample(inSampleIndex);
  mSelection->RemoveSample(inSampleIndex);
  if (mSegment)
    mSegment->RemoveSample(inSampleIndex);
  if (mExtra)
    mExtra->RemoveSample(inSampleIndex);
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

double iupPlotDataSet::GetSampleExtra(int inSampleIndex)
{
  int theCount = mDataX->GetCount();
  if (inSampleIndex < 0 || inSampleIndex >= theCount || !mExtra)
    return 0;

  return mExtra->GetSample(inSampleIndex);
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

void iupPlotDataSet::SetSampleExtra(int inSampleIndex, double inExtra)
{
  int theCount = mDataX->GetCount();
  if (inSampleIndex < 0 || inSampleIndex >= theCount)
    return;

  if (!mExtra)
    InitExtra();

  mExtra->SetSample(inSampleIndex, inExtra);
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
    // Save NoZoom state values
    mNoZoomMin = mMin;
    mNoZoomMax = mMax;
    mNoZoomAutoScaleMin = mAutoScaleMin;
    mNoZoomAutoScaleMax = mAutoScaleMax;

    mHasZoom = true;

    // disable AutoScale
    mAutoScaleMin = false;
    mAutoScaleMax = false;
  }
}

bool iupPlotAxis::ResetZoom()
{
  if (mHasZoom)
  {
    mHasZoom = false;

    // Restore NoZoom state values
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
  :ih(_ih), mCurrentDataSet(-1), mRedraw(true), mDataSetListCount(0), mCrossHairH(false), mCrossHairV(false),
   mGrid(true), mGridMinor(false), mViewportSquare(false), mScaleEqual(false), mHighlightMode(IUP_PLOT_HIGHLIGHT_NONE),
   mDefaultFontSize(inDefaultFontSize), mDefaultFontStyle(inDefaultFontStyle), mScreenTolerance(5),
   mAxisX(inDefaultFontStyle, inDefaultFontSize), mAxisY(inDefaultFontStyle, inDefaultFontSize),
   mCrossHairX(0), mCrossHairY(0), mShowSelectionBand(false), mDataSetListMax(20)
{
  mDataSetList = (iupPlotDataSet**)malloc(sizeof(iupPlotDataSet*)* mDataSetListMax); /* use malloc because we will use realloc */
  memset(mDataSetList, 0, sizeof(iupPlotDataSet*)* mDataSetListMax);
}

iupPlot::~iupPlot()
{
  RemoveAllDataSets();
  free(mDataSetList);  /* use free because we used malloc */
}

void iupPlot::SetViewport(int x, int y, int w, int h)
{
  mViewportBack.mX = x;
  mViewportBack.mY = y;
  mViewportBack.mWidth = w;
  mViewportBack.mHeight = h;

  if (mViewportSquare && w != h)
  {
    /* take the smallest length */
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

void iupPlot::UpdateMultibarCount()
{
  int i, count = 0, index = 0;

  for (i = 0; i < mDataSetListCount; i++)
  {
    if (mDataSetList[i]->mMode == IUP_PLOT_MULTIBAR)
      count++;
  }

  for (i = 0; i < mDataSetListCount; i++)
  {
    if (mDataSetList[i]->mMode == IUP_PLOT_MULTIBAR)
    {
      mDataSetList[i]->mMultibarCount = count;
      mDataSetList[i]->mMultibarIndex = index;
      index++;
    }
    else
    {
      mDataSetList[i]->mMultibarCount = 0;
      mDataSetList[i]->mMultibarIndex = 0;
    }
  }
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

    for (i = 0; i < mDataSetListCount; i++)
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
  } while (def_color < 12);

  return theColor;
}

void iupPlot::AddDataSet(iupPlotDataSet* inDataSet)
{
  if (mDataSetListCount >= mDataSetListMax)
  {
    int old_max = mDataSetListMax;
    mDataSetListMax += 20;
    mDataSetList = (iupPlotDataSet**)realloc(mDataSetList, sizeof(iupPlotDataSet*)* mDataSetListMax);
    memset(mDataSetList + old_max, 0, sizeof(iupPlotDataSet*)* (mDataSetListMax - old_max));
  }

  if (mDataSetListCount < mDataSetListMax)
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

  for (int i = inIndex; i < mDataSetListCount; i++)
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

void iupPlot::ClearHighlight()
{
  for (int ds = 0; ds < mDataSetListCount; ds++)
  {
    iupPlotDataSet* dataset = mDataSetList[ds];
    dataset->mHighlightedCurve = false;
    dataset->mHighlightedSample = -1;
  }
}

bool iupPlot::FindDataSetSample(double inScreenX, double inScreenY, int &outIndex, const char* &outName, int &outSampleIndex, double &outX, double &outY, const char* &outStrX) const
{
  if (!mAxisX.mTrafo || !mAxisY.mTrafo)
    return false;

  /* search for datasets in the inverse order they are drawn */
  for (int ds = mDataSetListCount - 1; ds >= 0; ds--)
  {
    iupPlotDataSet* dataset = mDataSetList[ds];

    if (dataset->FindSample(mAxisX.mTrafo, mAxisY.mTrafo, inScreenX, inScreenY, mScreenTolerance, outSampleIndex, outX, outY))
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

bool iupPlot::FindDataSetSegment(double inScreenX, double inScreenY, int &outIndex, const char* &outName, int &outSampleIndex1, double &outX1, double &outY1, int &outSampleIndex2, double &outX2, double &outY2) const
{
  if (!mAxisX.mTrafo || !mAxisY.mTrafo)
    return false;

  /* search for datasets in the inverse order they are drawn */
  for (int ds = mDataSetListCount - 1; ds >= 0; ds--)
  {
    iupPlotDataSet* dataset = mDataSetList[ds];

    // only for modes that have lines connecting the samples.
    if (dataset->mMode != IUP_PLOT_LINE &&
        dataset->mMode != IUP_PLOT_MARKLINE &&
        dataset->mMode != IUP_PLOT_AREA &&
        dataset->mMode != IUP_PLOT_ERRORBAR)
        continue;

    if (dataset->FindSegment(mAxisX.mTrafo, mAxisY.mTrafo, inScreenX, inScreenY, mScreenTolerance, outSampleIndex1, outSampleIndex2, outX1, outY1, outX2, outY2))
    {
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
    iupPlotSampleNotify theNotify = { ih, ds, select_cb };
    if (dataset->SelectSamples(inMinX, inMaxX, inMinY, inMaxY, &theNotify))
      theChanged = true;
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
    iupPlotSampleNotify theNotify = { ih, ds, select_cb };
    if (dataset->ClearSelection(&theNotify))
      theChanged = true;
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

  IFniiddi delete_cb = (IFniiddi)IupGetCallback(ih, "DELETE_CB");
  if (delete_cb)
  {
    Icallback cb = IupGetCallback(ih, "DELETEBEGIN_CB");
    if (cb && cb(ih) == IUP_IGNORE)
      return;
  }

  for (int ds = 0; ds < mDataSetListCount; ds++)
  {
    iupPlotDataSet* dataset = mDataSetList[ds];
    iupPlotSampleNotify theNotify = { ih, ds, delete_cb };
    if (dataset->DeleteSelectedSamples(&theNotify))
      theChanged = true;
  }

  if (delete_cb)
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

iupPlotDataSet* iupPlot::HasPie()
{
  for (int ds = 0; ds < mDataSetListCount; ds++)
  {
    iupPlotDataSet* dataset = mDataSetList[ds];
    if (dataset->mMode == IUP_PLOT_PIE)
      return dataset;
  }
  return NULL;
}

bool iupPlot::Render(cdCanvas* canvas)
{
  if (!mRedraw)
    return true;

  // draw entire plot viewport
  DrawBackground(canvas);

  // Shift the drawing area to the plot viewport
  cdCanvasOrigin(canvas, mViewport.mX, mViewport.mY);

  // There are no additional transformations set in the CD canvas,
  // all transformations are done here.

  cdCanvasClip(canvas, CD_CLIPAREA);

  // Draw axis and grid restricted only by the viewport
  cdCanvasClipArea(canvas, 0, mViewport.mWidth - 1, 0, mViewport.mHeight - 1);

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

  iupPlotRect theDataSetArea;
  theDataSetArea.mX = mBack.mMargin.mLeft;
  theDataSetArea.mY = mBack.mMargin.mBottom;
  theDataSetArea.mWidth = mViewport.mWidth - mBack.mMargin.mLeft - mBack.mMargin.mRight;
  theDataSetArea.mHeight = mViewport.mHeight - mBack.mMargin.mTop - mBack.mMargin.mBottom;

  if (!CalculateTickSpacing(theDataSetArea, canvas))
    return false;

  if (!CalculateXTransformation(theDataSetArea))
    return false;

  if (!CalculateYTransformation(theDataSetArea))
    return false;

  IFnC pre_cb = (IFnC)IupGetCallback(ih, "PREDRAW_CB");
  if (pre_cb)
    pre_cb(ih, canvas);

  if (mBack.GetImage())
    DrawBackgroundImage(canvas);

  if (!mGrid.DrawX(mAxisX.mTickIter, mAxisX.mTrafo, theDataSetArea, canvas))
    return false;

  if (mGrid.mShowX)
    mGridMinor.DrawX(mAxisX.mTickIter, mAxisX.mTrafo, theDataSetArea, canvas);

  if (!mGrid.DrawY(mAxisY.mTickIter, mAxisY.mTrafo, theDataSetArea, canvas))
    return false;

  if (mGrid.mShowY)
    mGridMinor.DrawY(mAxisY.mTickIter, mAxisY.mTrafo, theDataSetArea, canvas);

  if (!mAxisX.DrawX(theDataSetArea, canvas, mAxisY))
    return false;

  if (!mAxisY.DrawY(theDataSetArea, canvas, mAxisX))
    return false;

  if (mBox.mShow)
    mBox.Draw(theDataSetArea, canvas);

  // draw the datasets, legend, crosshair and selection restricted to the dataset area
  cdCanvasClipArea(canvas, theDataSetArea.mX, theDataSetArea.mX + theDataSetArea.mWidth - 1, theDataSetArea.mY, theDataSetArea.mY + theDataSetArea.mHeight - 1);

  IFniiddi drawsample_cb = (IFniiddi)IupGetCallback(ih, "DRAWSAMPLE_CB");

  iupPlotDataSet* pie_dataset = HasPie();

  for (int ds = 0; ds < mDataSetListCount; ds++)
  {
    iupPlotDataSet* dataset = mDataSetList[ds];
    iupPlotSampleNotify theNotify = { ih, ds, drawsample_cb };

    if (pie_dataset)
    {
      if (dataset != pie_dataset)
        continue;
      else
        dataset->DrawDataPie(mAxisX.mTrafo, mAxisY.mTrafo, canvas, &theNotify, mAxisY, mBack.mColor);
    }

    dataset->DrawData(mAxisX.mTrafo, mAxisY.mTrafo, canvas, &theNotify);
  }

  if (mCrossHairH)
    DrawCrossHairH(theDataSetArea, canvas);
  else if (mCrossHairV)
    DrawCrossHairV(theDataSetArea, canvas);

  if (mShowSelectionBand)
  {
    if (mSelectionBand.mX < theDataSetArea.mX)
    {
      mSelectionBand.mWidth = mSelectionBand.mX + mSelectionBand.mWidth - theDataSetArea.mX;
      mSelectionBand.mX = theDataSetArea.mX;
    }
    if (mSelectionBand.mY < theDataSetArea.mY)
    {
      mSelectionBand.mHeight = mSelectionBand.mY + mSelectionBand.mHeight - theDataSetArea.mY;
      mSelectionBand.mY = theDataSetArea.mY;
    }
    if (mSelectionBand.mX + mSelectionBand.mWidth > theDataSetArea.mX + theDataSetArea.mWidth)
      mSelectionBand.mWidth = theDataSetArea.mX + theDataSetArea.mWidth - mSelectionBand.mX;
    if (mSelectionBand.mY + mSelectionBand.mHeight > theDataSetArea.mY + theDataSetArea.mHeight)
      mSelectionBand.mHeight = theDataSetArea.mY + theDataSetArea.mHeight - mSelectionBand.mY;

    mBox.Draw(mSelectionBand, canvas);
  }

  IFnC post_cb = (IFnC)IupGetCallback(ih, "POSTDRAW_CB");
  if (post_cb)
    post_cb(ih, canvas);

  if (pie_dataset)
    DrawSampleColorLegend(pie_dataset, theDataSetArea, canvas, mLegend.mPos);
  else if (!DrawLegend(theDataSetArea, canvas, mLegend.mPos))
    return false;

  // Draw title restricted only by the viewport
  cdCanvasClipArea(canvas, 0, mViewport.mWidth - 1, 0, mViewport.mHeight - 1);

  DrawTitle(canvas);

  if (!IupGetInt(ih, "ACTIVE"))
    DrawInactive(canvas);

  mRedraw = false;
  return true;
}

