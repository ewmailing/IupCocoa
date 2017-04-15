
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "iupPlot.h"



const double kMajorTickXInitialFac = 2.0;
const double kMajorTickYInitialFac = 3.0;
const double kRangeVerySmall = (double)1.0e-3;


static int iPlotCountDigit(int inNum)
{
  int theCount = 0;
  while (inNum != 0)
  {
    inNum = inNum / 10;
    theCount++;
  }
  if (theCount == 0) theCount = 1;
  return theCount;
}

static int iPlotGetPrecisionNumChar(bool inAutoSpacing, const char* inFormatString, double inMin, double inMax)
{
  int thePrecision = 0;

  if (inAutoSpacing)
  {
    int theMinPrecision = iupPlotCalcPrecision(inMin);
    int theMaxPrecision = iupPlotCalcPrecision(inMax);
    if (theMinPrecision > theMaxPrecision)
      thePrecision = theMinPrecision;
    else
      thePrecision = theMaxPrecision;
  }
  else
  {
    while (*inFormatString)
    {
      if (*inFormatString == '.')
        break;
      inFormatString++;
    }

    if (*inFormatString == '.')
    {
      inFormatString++;
      iupStrToInt(inFormatString, &thePrecision);
    }
  }

  int theMin = iupPlotRound(inMin);
  int theMax = iupPlotRound(inMax);
  int theNumDigitMin = iPlotCountDigit(theMin);
  int theNumDigitMax = iPlotCountDigit(theMax);
  if (theNumDigitMin > theNumDigitMax)
    thePrecision += theNumDigitMin;
  else
    thePrecision += theNumDigitMax;

  thePrecision += 3;  // sign, decimal symbol, exp 

  return thePrecision;
}

void iupPlotAxis::GetTickNumberSize(cdCanvas* canvas, int *outWitdh, int *outHeight) const
{
  int theTickFontWidth, theTickFontHeight;
  SetFont(canvas, mTick.mFontStyle, mTick.mFontSize);
  cdCanvasGetTextSize(canvas, "1234567890.", &theTickFontWidth, &theTickFontHeight);
  theTickFontWidth /= 11;
  if (outHeight) *outHeight = theTickFontHeight;
  if (outWitdh)  *outWitdh = theTickFontWidth * iPlotGetPrecisionNumChar(mTick.mAutoSpacing, mTick.mFormatString, mMin, mMax);
}

void iupPlot::CalculateTitlePos()
{
  // it does not depend on theMargin
  if (mTitle.mAutoPos)
  {
    mTitle.mPosX = mViewport.mWidth / 2;
    mTitle.mPosY = 5;  // add small spacing
  }
}

bool iupPlot::CheckInsideTitle(cdCanvas* canvas, int x, int y)
{
  // it does not depend on theMargin
  if (mTitle.GetText())
  {
    SetTitleFont(canvas);

    cdCanvasTextAlignment(canvas, CD_NORTH);

    int xmin, xmax, ymin, ymax;
    cdCanvasGetTextBox(canvas, mTitle.mPosX, cdCanvasInvertYAxis(canvas, mTitle.mPosY), mTitle.GetText(), &xmin, &xmax, &ymin, &ymax);

    if (x >= xmin && x <= xmax && 
        y >= ymin && y <= ymax)
      return true;
  }

  return false;
}

bool iupPlot::CheckInsideLegend(cdCanvas* canvas, int x, int y)
{
  if (mLegend.mShow)
  {
    int theY = cdCanvasInvertYAxis(canvas, mLegend.mPos.mY);

    if (x >= mLegend.mPos.mX && x < mLegend.mPos.mX + mLegend.mPos.mWidth &&
        y >= theY            && y < theY + mLegend.mPos.mHeight)
      return true;
  }

  return false;
}

void iupPlot::CalculateMargins(cdCanvas* canvas)
{
  if (mBack.mMarginAuto.mTop)
  {
    mBack.mMargin.mTop = 0;

    if (mTitle.GetText() && mTitle.mAutoPos)
    {
      SetTitleFont(canvas);

      int theTextHeight;
      cdCanvasGetTextSize(canvas, mTitle.GetText(), NULL, &theTextHeight);
      mBack.mMargin.mTop += theTextHeight + 5 + theTextHeight/2;
    }
    else
    {
      if (mAxisY.mShow)
      {
        if (mAxisY.mShowArrow && !mAxisY.mReverse)
          mBack.mMargin.mTop += mAxisY.mTick.mMinorSize + 2 + 5;  // size of the arrow

        if (mAxisY.mTick.mShowNumber)
        {
          if (mAxisY.mTick.mRotateNumber)
          {
            int theYTickNumberHeight;
            mAxisY.GetTickNumberSize(canvas, NULL, &theYTickNumberHeight);
            mBack.mMargin.mTop += theYTickNumberHeight / 2;
          }
          else
          {
            int theYTickNumberWidth;
            mAxisY.GetTickNumberSize(canvas, &theYTickNumberWidth, NULL);
            mBack.mMargin.mTop += theYTickNumberWidth / 2;
          }
        }
      }
    }
  }

  if (mBack.mMarginAuto.mBottom)
  {
    mBack.mMargin.mBottom = 0;

    if (mAxisX.mShow && !mAxisX.mCrossOrigin)
    {
      mBack.mMargin.mBottom += mAxisX.mTick.mMajorSize;

      if (mAxisX.mTick.mShowNumber)
      {
        if (mAxisX.mTick.mRotateNumber)
        {
          int theXTickNumberWidth;
          mAxisX.GetTickNumberSize(canvas, &theXTickNumberWidth, NULL);
          mBack.mMargin.mBottom += theXTickNumberWidth;
        }
        else
        {
          int theXTickNumberHeight;
          mAxisX.GetTickNumberSize(canvas, NULL, &theXTickNumberHeight);
          mBack.mMargin.mBottom += theXTickNumberHeight;
        }
      }

      if (mAxisX.GetLabel())
      {
        int theXFontHeight;
        SetFont(canvas, mAxisX.mFontStyle, mAxisX.mFontSize);
        cdCanvasGetFontDim(canvas, NULL, &theXFontHeight, NULL, NULL);

        mBack.mMargin.mBottom += theXFontHeight + theXFontHeight / 10;
      }
    }

    if (mAxisY.mShow)
    {
      if (mAxisY.mTick.mShowNumber)
      {
        if (mAxisY.mTick.mRotateNumber)
        {
          int theYTickNumberHeight;
          mAxisY.GetTickNumberSize(canvas, NULL, &theYTickNumberHeight);
          mBack.mMargin.mBottom += theYTickNumberHeight/2;
        }
        else
        {
          int theYTickNumberWidth;
          mAxisY.GetTickNumberSize(canvas, &theYTickNumberWidth, NULL);
          mBack.mMargin.mBottom += theYTickNumberWidth/2;
        }
      }
    }
  }

  if (mBack.mMarginAuto.mRight)
  {
    mBack.mMargin.mRight = 0;

    if (mAxisX.mShow)
    {
      if (mAxisX.mShowArrow && !mAxisX.mReverse)
        mBack.mMargin.mRight += mAxisX.mTick.mMinorSize + 2 + 5;  // size of the arrow

      if (mAxisX.mTick.mShowNumber)
      {
        if (mAxisX.mTick.mRotateNumber)
        {
          int theXTickNumberHeight;
          mAxisX.GetTickNumberSize(canvas, NULL, &theXTickNumberHeight);
          mBack.mMargin.mRight += theXTickNumberHeight/2;
        }
        else
        {
          int theXTickNumberWidth;
          mAxisX.GetTickNumberSize(canvas, &theXTickNumberWidth, NULL);
          mBack.mMargin.mRight += theXTickNumberWidth/2;
        }
      }
    }
  }

  if (mBack.mMarginAuto.mLeft)
  {
    mBack.mMargin.mLeft = 0;

    if (mAxisY.mShow && !mAxisY.mCrossOrigin)
    {
      mBack.mMargin.mLeft += mAxisY.mTick.mMajorSize;

      if (mAxisY.mTick.mShowNumber)
      {
        if (mAxisY.mTick.mRotateNumber)
        {
          int theYTickNumberHeight;
          mAxisY.GetTickNumberSize(canvas, NULL, &theYTickNumberHeight);
          mBack.mMargin.mLeft += theYTickNumberHeight;
        }
        else
        {
          int theYTickNumberWidth;
          mAxisY.GetTickNumberSize(canvas, &theYTickNumberWidth, NULL);
          mBack.mMargin.mLeft += theYTickNumberWidth;
        }
      }

      if (mAxisY.GetLabel())
      {
        int theYFontHeight;
        SetFont(canvas, mAxisY.mFontStyle, mAxisY.mFontSize);
        cdCanvasGetFontDim(canvas, NULL, &theYFontHeight, NULL, NULL);

        mBack.mMargin.mLeft += theYFontHeight + theYFontHeight / 10;
      }
    }

    if (mAxisX.mShow)
    {
      if (mAxisX.mShowArrow && mAxisX.mReverse)
        mBack.mMargin.mLeft += mAxisX.mTick.mMinorSize + 2 + 5;  // size of the arrow

      if (mAxisX.mTick.mShowNumber)
      {
        if (mAxisX.mTick.mRotateNumber)
        {
          int theXTickNumberHeight;
          mAxisX.GetTickNumberSize(canvas, NULL, &theXTickNumberHeight);
          mBack.mMargin.mLeft += theXTickNumberHeight / 2;
        }
        else
        {
          int theXTickNumberWidth;
          mAxisX.GetTickNumberSize(canvas, &theXTickNumberWidth, NULL);
          mBack.mMargin.mLeft += theXTickNumberWidth / 2;
        }
      }
    }
  }
}

bool iupPlot::CalculateXRange(double &outXMin, double &outXMax)
{
  bool theFirst = true;
  outXMin = 0;
  outXMax = 0;

  for (int ds = 0; ds < mDataSetListCount; ds++)
  {
    const iupPlotDataBase *theXData = mDataSetList[ds]->GetDataX();

    if (theXData->GetCount() == 0)
      continue;

    double theXMin;
    double theXMax;
    if (mDataSetList[ds]->mMode == IUP_PLOT_PIE)
    {
      theXMin = -1;
      theXMax = 1;
    }
    else if (!theXData->CalculateRange(theXMin, theXMax))
      return false;
    
    if (theFirst) 
    {
      outXMin = theXMin;
      outXMax = theXMax;
      theFirst = false;
    }
    if (theXMax>outXMax)
      outXMax = theXMax;
    if (theXMin<outXMin)
      outXMin = theXMin;
  }

  if (outXMin == 0 && outXMax == 0)
    return false;

  return true;
}

bool iupPlot::CalculateYRange(double &outYMin, double &outYMax)
{
  bool theFirst = true;
  outYMin = 0;
  outYMax = 0;

  for (int ds = 0; ds < mDataSetListCount; ds++)
  {
    const iupPlotDataBase *theYData = mDataSetList[ds]->GetDataY();

    double theYMin;
    double theYMax;
    if (mDataSetList[ds]->mMode == IUP_PLOT_PIE)
    {
      theYMin = -1;
      theYMax = 1;
    }
    else if (!theYData->CalculateRange(theYMin, theYMax))
      return false;
    
    if (theFirst) 
    {
      outYMin = theYMin;
      outYMax = theYMax;
      theFirst = false;
    }
    if (theYMin<outYMin) 
      outYMin = theYMin;
    if (theYMax>outYMax)
      outYMax = theYMax;
  }

  // YRange can be 0

  return true;
}

bool iupPlot::CalculateAxisRange() 
{
  if (mAxisX.mAutoScaleMin || mAxisX.mAutoScaleMax) 
  {
    double theXMin;
    double theXMax;

    CalculateXRange(theXMin, theXMax);

    if (mAxisX.mAutoScaleMin)
    {
      mAxisX.mMin = theXMin;
      if (mAxisX.mLogScale && (theXMin < kLogMinClipValue)) 
        mAxisX.mMin = kLogMinClipValue;
    }

    if (mAxisX.mAutoScaleMax) 
      mAxisX.mMax = theXMax;

    if (!mAxisX.mTickIter->AdjustRange(mAxisX.mMin, mAxisX.mMax)) 
      return false;
  }

  if (mAxisY.mAutoScaleMin || mAxisY.mAutoScaleMax) 
  {
    double theYMin;
    double theYMax;

    CalculateYRange(theYMin, theYMax);

    if (mAxisY.mAutoScaleMin) 
    {
      mAxisY.mMin = theYMin;
      if (mAxisY.mLogScale && (theYMin < kLogMinClipValue)) 
        mAxisY.mMin = kLogMinClipValue;
    }
    if (mAxisY.mAutoScaleMax)
      mAxisY.mMax = theYMax;

    if (!mAxisY.mTickIter->AdjustRange(mAxisY.mMin, mAxisY.mMax))
      return false;
  }

  if (mScaleEqual)
  {
    if (mAxisY.HasZoom() || mAxisX.HasZoom())
    {
      if (mAxisY.mMax - mAxisY.mMin != mAxisX.mMax - mAxisX.mMin)
      {
        double theLength;

        if (mAxisY.mMax - mAxisY.mMin > mAxisX.mMax - mAxisX.mMin)
        {
          theLength = mAxisY.mMax - mAxisY.mMin;
          mAxisX.mMax = mAxisX.mMin + theLength;
        }
        else
        {
          theLength = mAxisX.mMax - mAxisX.mMin;
          mAxisY.mMax = mAxisY.mMin + theLength;
        }
      }
    }
    else
    {
      double theMin = mAxisY.mMin;
      if (mAxisX.mMin < theMin)
        theMin = mAxisX.mMin;

      double theMax = mAxisY.mMax;
      if (mAxisX.mMax > theMax)
        theMax = mAxisX.mMax;

      mAxisX.mMin = theMin;
      mAxisY.mMin = theMin;
      mAxisX.mMax = theMax;
      mAxisY.mMax = theMax;
    }
  }

  return true;
}

bool iupPlot::CheckRange(const iupPlotAxis &inAxis)
{
  if (inAxis.mLogScale) 
  {
    if (inAxis.mMin < kLogMinClipValue)
      return false;
  }
  return true;
}


bool iupPlot::CalculateXTransformation(const iupPlotRect &inRect)
{
  return mAxisX.mTrafo->Calculate(inRect.mX, inRect.mX + inRect.mWidth, mAxisX);
}

bool iupPlot::CalculateYTransformation(const iupPlotRect &inRect)
{
  return mAxisY.mTrafo->Calculate(inRect.mY, inRect.mY + inRect.mHeight, mAxisY);
}

void iupPlot::CalculateTickSize(cdCanvas* canvas, iupPlotTick &ioTick)
{
  if (ioTick.mAutoSize)
  {
    int theFontHeight;
    SetFont(canvas, ioTick.mFontStyle, ioTick.mFontSize);
    cdCanvasGetFontDim(canvas, NULL, &theFontHeight, NULL, NULL);

    ioTick.mMajorSize = theFontHeight / 2;
    ioTick.mMinorSize = theFontHeight / 4;
  }
}

bool iupPlot::CalculateTickSpacing(const iupPlotRect &inRect, cdCanvas* canvas)
{
  double theXRange = mAxisX.mMax - mAxisX.mMin;
  double theYRange = mAxisY.mMax - mAxisY.mMin;

  if (theXRange <= 0 || theYRange < 0)
    return false;

  if ((mAxisY.mMax != 0 && fabs(theYRange / mAxisY.mMax) < kRangeVerySmall) || theYRange == 0)
  {
    double delta = 0.1;
    if (mAxisY.mMax != 0)
      delta *= fabs(mAxisY.mMax);

    mAxisY.mMax += delta;
    mAxisY.mMin -= delta;
    theYRange = mAxisY.mMax - mAxisY.mMin;
  }

  if (mAxisX.mTick.mAutoSpacing)
  {
    int theXFontHeight;
    SetFont(canvas, mAxisX.mTick.mFontStyle, mAxisX.mTick.mFontSize);
    cdCanvasGetFontDim(canvas, NULL, &theXFontHeight, NULL, NULL);

    int theTextWidth;
    cdCanvasGetTextSize(canvas, "12345", &theTextWidth, NULL);

    double theDivGuess = inRect.mWidth / (kMajorTickXInitialFac*theTextWidth);
    if (!mAxisX.mTickIter->CalculateSpacing(theXRange, theDivGuess, mAxisX.mTick))
      return false;
  }

  if (mAxisY.mTick.mAutoSpacing)
  {
    int theYFontHeight;
    SetFont(canvas, mAxisY.mTick.mFontStyle, mAxisY.mTick.mFontSize);
    cdCanvasGetFontDim(canvas, NULL, &theYFontHeight, NULL, NULL);

    double theDivGuess = inRect.mHeight / (kMajorTickYInitialFac*theYFontHeight);
    if (!mAxisY.mTickIter->CalculateSpacing(theYRange, theDivGuess, mAxisY.mTick))
      return false;
  }

  return true;
}

