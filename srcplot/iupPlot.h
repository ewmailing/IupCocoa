/* Single plot class.
   Multiple plots can be used in the same IupPlot element.

   This is a re-write of the PPlot library,
   directly using CD for drawing and IUP callback for interaction.
*/

#include "iup.h"
#include "iupcbs.h"

#include "iup_array.h"
#include "iup_str.h"

#include <cd.h>

#ifndef __IUPPLOT_H__
#define __IUPPLOT_H__


enum iupPlotMode { IUP_PLOT_LINE, IUP_PLOT_MARK, IUP_PLOT_MARKLINE, IUP_PLOT_AREA, IUP_PLOT_BAR };
enum iupPlotLegendPosition { IUP_PLOT_TOPRIGHT, IUP_PLOT_TOPLEFT, IUP_PLOT_BOTTOMRIGHT, IUP_PLOT_BOTTOMLEFT, IUP_PLOT_BOTTOMCENTER, IUP_PLOT_XY };

const double kFloatSmall = 1e-20;
const double kLogMinClipValue = 1e-10;  // pragmatism to avoid problems with small values in log plot

int iupPlotCalcPrecision(double inValue);

inline int iupPlotRound(double inFloat)
{
  return (int)(inFloat>0 ? inFloat + 0.5 : inFloat - 0.5);
}

inline double iupPlotLog(double inFloat, double inBase)
{
  const double kLogMin = 1e-10;  // min argument for log10 function
  if (inFloat<kLogMin) inFloat = kLogMin;
  return log10(inFloat) / log10(inBase);
}

inline double iupPlotExp(double inFloat, double inBase)
{
  const double kExpMax = 1e10;  // max argument for pow10 function
  if (inFloat>kExpMax) inFloat = kExpMax;
  return pow(inBase, inFloat);
}

class iupPlotRect 
{
public:
  iupPlotRect() :mX(0), mY(0), mWidth(0), mHeight(0) {}
  iupPlotRect(int inX, int inY, int inWidth, int inHeight)
    : mX(inX), mY(inY), mWidth(inWidth), mHeight(inHeight) {}

  int mX;
  int mY;
  int mWidth;
  int mHeight;
};

class iupPlotMargin 
{
public:
  iupPlotMargin() :mLeft(0), mRight(0), mTop(0), mBottom(0) {}
  iupPlotMargin(int inLeft, int inRight, int inTop, int inBottom)
    : mLeft(inLeft), mRight(inRight), mTop(inTop), mBottom(inBottom) {}

  int mLeft;
  int mRight;
  int mTop;
  int mBottom;
};

class iupPlotAxis;

class iupPlotTrafoBase
{
public:
  virtual ~iupPlotTrafoBase() {}
  virtual double Transform(double inValue) const = 0;
  virtual double TransformBack(double inValue) const = 0;
  virtual bool Calculate(int inBegin, int inEnd, const iupPlotAxis& inAxis) = 0;
};

class iupPlotTrafoLinear : public iupPlotTrafoBase
{
public:
  iupPlotTrafoLinear() :mOffset(0), mSlope(0) {}
  double Transform(double inValue) const;
  double TransformBack(double inValue) const;

  bool Calculate(int inBegin, int inEnd, const iupPlotAxis& inAxis);

  double mOffset;
  double mSlope;
};

class iupPlotTrafoLog : public iupPlotTrafoBase 
{
public:
  iupPlotTrafoLog() :mOffset(0), mSlope(0), mBase(10) {}
  double Transform(double inValue) const;
  double TransformBack(double inValue) const;

  bool Calculate(int inBegin, int inEnd, const iupPlotAxis& inAxis);

  double mOffset;
  double mSlope;
  double mBase;
};

class iupPlotDataBase
{
public:
  iupPlotDataBase(int inSize) : mCount(0), mIsString(false) { mArray = iupArrayCreate(20, inSize); }
  virtual ~iupPlotDataBase() { iupArrayDestroy(mArray); }

  bool IsString() const { return mIsString; }
  int GetCount() const { return mCount; }

  virtual bool CalculateRange(double &outMin, double &outMax) const = 0;
  virtual double GetSample(int inSampleIndex) const = 0;

  void RemoveSample(int inSampleIndex) { 
    if (inSampleIndex < 0) inSampleIndex = 0; if (inSampleIndex > mCount) inSampleIndex = mCount; 
    iupArrayRemove(mArray, inSampleIndex, 1); mCount--; }

protected:
  int mCount;
  Iarray* mArray;
  bool mIsString;
};

class iupPlotDataReal : public iupPlotDataBase
{
public:
  iupPlotDataReal() :iupPlotDataBase(sizeof(double)) { mData = (double*)iupArrayGetData(mArray); }

  double GetSample(int inSampleIndex) const { return mData[inSampleIndex]; }
  void SetSample(int inSampleIndex, double inReal) const { mData[inSampleIndex] = inReal; }

  void AddSample(double inReal) { mData = (double*)iupArrayInc(mArray); mData[mCount] = inReal; mCount++; }
  void InsertSample(int inSampleIndex, double inReal) {
    if (inSampleIndex < 0) inSampleIndex = 0; if (inSampleIndex > mCount) inSampleIndex = mCount;
    mData = (double*)iupArrayInsert(mArray, inSampleIndex, 1); mData[inSampleIndex] = inReal; mCount++; }

  bool CalculateRange(double &outMin, double &outMax) const;

protected:
  double* mData;
};

class iupPlotDataString : public iupPlotDataBase
{
public:
  iupPlotDataString() :iupPlotDataBase(sizeof(char*)) { mIsString = true; mData = (char**)iupArrayGetData(mArray); }
  ~iupPlotDataString(); 

  double GetSample(int inSampleIndex) const { return inSampleIndex; }

  const char* GetSampleString(int inSampleIndex) const { return mData[inSampleIndex]; }
  void SetSampleString(int inSampleIndex, const char *inString) const { free(mData[inSampleIndex]); mData[inSampleIndex] = iupStrDup(inString); }

  void AddSample(const char *inString) { mData = (char**)iupArrayInc(mArray); mData[mCount] = iupStrDup(inString); mCount++; }
  void InsertSample(int inSampleIndex, const char *inString) { 
    if (inSampleIndex < 0) inSampleIndex = 0; if (inSampleIndex > mCount) inSampleIndex = mCount; 
    mData = (char**)iupArrayInsert(mArray, inSampleIndex, 1); mData[inSampleIndex] = iupStrDup(inString); mCount++; }

  bool CalculateRange(double &outMin, double &outMax) const;

protected:
  char** mData;
};

class iupPlotDataBool : public iupPlotDataBase
{
public:
  iupPlotDataBool() :iupPlotDataBase(sizeof(bool)) { mData = (bool*)iupArrayGetData(mArray); }

  double GetSample(int inSampleIndex) const { return (int)mData[inSampleIndex]; }

  bool GetSampleBool(int inSampleIndex) const { return mData[inSampleIndex]; }
  void SetSampleBool(int inSampleIndex, bool inBool) { mData[inSampleIndex] = inBool; }

  void AddSample(bool inBool) { mData = (bool*)iupArrayInc(mArray); mData[mCount] = inBool; mCount++; }
  void InsertSample(int inSampleIndex, bool inBool) { 
    if (inSampleIndex < 0) inSampleIndex = 0; if (inSampleIndex > mCount) inSampleIndex = mCount; 
    mData = (bool*)iupArrayInsert(mArray, inSampleIndex, 1); mData[inSampleIndex] = inBool; mCount++; }

  bool CalculateRange(double &outMin, double &outMax) const;

protected:
  bool* mData;
};

struct iupPlotSampleNotify
{
  Ihandle* ih;
  int ds;
  IFniiddi cb;
};

class iupPlotDataSet
{
public:
  iupPlotDataSet(bool strXdata);
  ~iupPlotDataSet();

  void SetName(const char* inName) { if (mName) free(mName); mName = iupStrDup(inName); }
  const char* GetName() {return mName; }

  bool FindSample(double inX, double inY, double tolX, double tolY,
                  int &outSampleIndex, double &outX, double &outY) const;

  void DrawData(const iupPlotTrafoBase *inTrafoX, const iupPlotTrafoBase *inTrafoY, cdCanvas* canvas, const iupPlotSampleNotify* inNotify) const;

  int GetCount();
  void AddSample(double inX, double inY);
  void InsertSample(int inSampleIndex, double inX, double inY);
  void AddSampleSegment(double inX, double inY, bool inSegment);
  void InsertSampleSegment(int inSampleIndex, double inX, double inY, bool inSegment);
  void AddSample(const char* inX, double inY);
  void InsertSample(int inSampleIndex, const char* inX, double inY);
  void RemoveSample(int inSampleIndex);
  void GetSample(int inSampleIndex, double *inX, double *inY);
  void GetSample(int inSampleIndex, const char* *inX, double *inY);
  bool GetSampleSelection(int inSampleIndex);
  void SetSample(int inSampleIndex, double inX, double inY);
  void SetSample(int inSampleIndex, const char* inX, double inY);
  void SetSampleSelection(int inSampleIndex, bool inSelected);

  const iupPlotDataBase* GetDataX() const { return mDataX; }
  const iupPlotDataBase* GetDataY() const { return mDataY; }
  const iupPlotDataBool* GetSelection() const { return mSelection; }
  const iupPlotDataBool* GetSegment() const { return mSegment; }

  bool SelectSamples(double inMinX, double inMaxX, double inMinY, double inMaxY, const iupPlotSampleNotify* inNotify);
  bool ClearSelection(const iupPlotSampleNotify* inNotify);
  bool DeleteSelectedSamples(const iupPlotSampleNotify* inNotify);

  long mColor;
  iupPlotMode mMode;
  int mLineStyle;
  int mLineWidth;
  int mMarkStyle;
  int mMarkSize;
  void* mUserData;

protected:
  char* mName;

  iupPlotDataBase* mDataX;
  iupPlotDataBase* mDataY;
  iupPlotDataBool* mSelection;
  iupPlotDataBool* mSegment;
  bool mHasSelected;

  void InitSegment();

  void DrawDataLine(const iupPlotTrafoBase *inTrafoX, const iupPlotTrafoBase *inTrafoY, cdCanvas* canvas, const iupPlotSampleNotify* inNotify) const;
  void DrawDataMark(const iupPlotTrafoBase *inTrafoX, const iupPlotTrafoBase *inTrafoY, cdCanvas* canvas, const iupPlotSampleNotify* inNotify) const;
  void DrawDataMarkLine(const iupPlotTrafoBase *inTrafoX, const iupPlotTrafoBase *inTrafoY, cdCanvas* canvas, const iupPlotSampleNotify* inNotify) const;
  void DrawDataArea(const iupPlotTrafoBase *inTrafoX, const iupPlotTrafoBase *inTrafoY, cdCanvas* canvas, const iupPlotSampleNotify* inNotify) const;
  void DrawDataBar(const iupPlotTrafoBase *inTrafoX, const iupPlotTrafoBase *inTrafoY, cdCanvas* canvas, const iupPlotSampleNotify* inNotify) const;
  void DrawSelection(const iupPlotTrafoBase *inTrafoX, const iupPlotTrafoBase *inTrafoY, cdCanvas* canvas, const iupPlotSampleNotify* inNotify) const;
};

class iupPlotTick;

class iupPlotTickIterBase 
{
public:
  iupPlotTickIterBase() :mAxis(NULL){}
  virtual ~iupPlotTickIterBase() {}

  virtual bool Init() = 0;
  virtual bool GetNextTick(double &outTick, bool &outIsMajorTick, char* outFormatString) = 0;

  virtual bool CalculateSpacing(double inParRange, double inDivGuess, iupPlotTick &outTickInfo) const = 0;
  virtual bool AdjustRange(double &, double &) const { return true; };

  void SetAxis(const iupPlotAxis *inAxis) { mAxis = inAxis; };

protected:
  const iupPlotAxis *mAxis;
};

class iupPlotTickIterLinear : public iupPlotTickIterBase 
{
public:
  iupPlotTickIterLinear() :mCurrentTick(0), mDelta(0){}

  virtual bool Init();
  virtual bool GetNextTick(double &outTick, bool &outIsMajorTick, char* outFormatString);

  bool CalculateSpacing(double inParRange, double inDivGuess, iupPlotTick &outTickInfo) const;

protected:
  double mCurrentTick;
  long mCount;
  double mDelta;
};

class iupPlotTickIterLog : public iupPlotTickIterBase 
{
public:
  iupPlotTickIterLog() :mCurrentTick(0), mDelta(0){}

  virtual bool Init();
  virtual bool GetNextTick(double &outTick, bool &outIsMajorTick, char* outFormatString);

  bool CalculateSpacing(double inParRange, double inDivGuess, iupPlotTick &outTickInfo) const;
  virtual bool AdjustRange(double &ioMin, double &ioMax) const;

  double RoundUp(double inFloat) const;
  double RoundDown(double inFloat) const;

protected:
  double mCurrentTick;
  long mCount;
  double mDelta;
};

class iupPlotTickIterNamed : public iupPlotTickIterLinear 
{
public:
  iupPlotTickIterNamed(){}

  void SetStringList(const iupPlotDataString* inStringData) { mStringData = inStringData; };

  virtual bool GetNextTick(double &outTick, bool &outIsMajorTick, char* outFormatString);
  bool CalculateSpacing(double inParRange, double inDivGuess, iupPlotTick &outTickInfo) const;

protected:
  const iupPlotDataString* mStringData;
};

class iupPlotTick 
{
public:
  iupPlotTick() 
    :mAutoSpacing(true), mAutoSize(true), mMinorDivision(1), mShowNumber(true),
    mMajorSpan(1), mMajorSize(1), mMinorSize(1), mShow(true), mRotateNumberAngle(90),
    mFontSize(0), mFontStyle(-1), mRotateNumber(false), mFormatAuto(true)
  { strcpy(mFormatString, "%.0f"); }

  bool mShow;

  bool mShowNumber;
  bool mRotateNumber;
  double mRotateNumberAngle;
  char mFormatString[30];
  bool mFormatAuto;
  int mFontSize;
  int mFontStyle;

  bool mAutoSpacing;
  int mMinorDivision;
  double mMajorSpan; // in plot units

  bool mAutoSize;
  int mMajorSize;
  int mMinorSize;
};

class iupPlotAxis
{
public:
  iupPlotAxis(int inDefaultFontStyle, int inDefaultFontSize)
    : mShow(true), mMin(0), mMax(0), mAutoScaleMin(true), mAutoScaleMax(true),
      mReverse(false), mLogScale(false), mCrossOrigin(false), mColor(CD_BLACK),
      mMaxDecades(-1), mLogBase(10), mLabelCentered(false), mHasZoom(false),
      mDiscrete(false), mLabel(NULL), mShowArrow(true), mLineWidth(1),
      mFontSize(0), mFontStyle(-1), mDefaultFontSize(inDefaultFontSize), 
      mTrafo(NULL), mTickIter(NULL), mDefaultFontStyle(inDefaultFontStyle),
      mAutoScaleEqual(false)
    { strcpy(mTipFormatString, "%.2f"); }
  ~iupPlotAxis() { SetLabel(NULL); }

  void SetLabel(const char* inLabel) { if (mLabel) free(mLabel); mLabel = iupStrDup(inLabel); }
  const char* GetLabel() const { return mLabel; }

  void Init();
  void SetNamedTickIter(const iupPlotDataString *inStringXData);
  void GetTickNumberSize(cdCanvas* canvas, int *outWitdh, int *outHeight) const;

  bool DrawX(const iupPlotRect &inRect, cdCanvas* canvas, const iupPlotAxis& inAxisY) const;
  bool DrawY(const iupPlotRect &inRect, cdCanvas* canvas, const iupPlotAxis& inAxisX) const;

  bool HasZoom() const { return mHasZoom; }
  bool ResetZoom();
  bool ZoomIn(double inCenter);
  bool ZoomOut(double inCenter);
  bool ZoomTo(double inMin, double inMax);
  void PanStart() { mPanMin = mMin; }
  bool Pan(double inOffset);
  bool Scroll(double inDelta, bool inFullPage);
  bool ScrollTo(double inMin);

  bool mShow;
  long mColor;
  double mMin;
  double mMax;
  bool mAutoScaleMin;
  bool mAutoScaleMax;
  bool mAutoScaleEqual;
  bool mReverse;
  bool mCrossOrigin;
  bool mShowArrow;
  char mTipFormatString[30];

  int mFontSize;
  int mFontStyle;
  bool mLabelCentered;
  int mDefaultFontSize;
  int mDefaultFontStyle;

  bool mLogScale;
  int mMaxDecades;// property for auto logscale
  double mLogBase;
  bool mDiscrete;

  int mLineWidth;

  iupPlotTick mTick;

  iupPlotTrafoBase *mTrafo;
  iupPlotTickIterBase *mTickIter;

protected:
  char* mLabel;

  void SetFont(cdCanvas* canvas, int inFontStyle, int inFontSize) const;

  bool DrawXTick(double inX, double inScreenY, bool inMajor, const char* inFormatString, cdCanvas* canvas) const;
  bool DrawYTick(double inY, double inScreenX, bool inMajor, const char* inFormatString, cdCanvas* canvas) const;

  iupPlotTrafoLinear mLinTrafo;
  iupPlotTrafoLog mLogTrafo;

  iupPlotTickIterLinear mLinTickIter;
  iupPlotTickIterLog mLogTickIter;
  iupPlotTickIterNamed mNamedTickIter;

  void InitZoom();
  void CheckZoomOutLimit(double inRange);

  bool mHasZoom;
  double mNoZoomMin;
  double mNoZoomMax;
  bool mNoZoomAutoScaleMin;
  bool mNoZoomAutoScaleMax;
  double mPanMin;
};

class iupPlotGrid 
{
public:
  iupPlotGrid(bool inMajor)
    : mShowX(false), mShowY(false), mColor(cdEncodeColor(200, 200, 200)), 
      mLineStyle(CD_CONTINUOUS), mLineWidth(1), mMajor(inMajor) {}

  bool DrawX(iupPlotTickIterBase* inTickIter, iupPlotTrafoBase* inTrafo, const iupPlotRect &inRect, cdCanvas* canvas) const;
  bool DrawY(iupPlotTickIterBase* inTickIter, iupPlotTrafoBase* inTrafo, const iupPlotRect &inRect, cdCanvas* canvas) const;

  bool mMajor;
  bool mShowX;
  bool mShowY;
  long mColor;
  int  mLineStyle;
  int  mLineWidth;
};

class iupPlotBox
{
public:
  iupPlotBox()
    : mShow(false), mColor(CD_BLACK),
      mLineStyle(CD_CONTINUOUS), mLineWidth(1) {}

  void Draw(const iupPlotRect &inRect, cdCanvas* canvas) const;

  bool mShow;
  long mColor;
  int  mLineStyle;
  int  mLineWidth;
};

class iupPlotLegend
{
public:
  iupPlotLegend() 
    : mShow(false), mBoxShow(true), mFontSize(0), mFontStyle(-1), mPosition(IUP_PLOT_TOPRIGHT), 
      mBoxLineStyle(CD_CONTINUOUS), mBoxLineWidth(1), mBoxColor(CD_BLACK), mBoxBackColor(CD_WHITE)
  {}

  bool mShow;
  iupPlotLegendPosition mPosition;
  iupPlotRect mPos;
  int mFontSize;
  int mFontStyle;

  bool mBoxShow;
  long mBoxColor;
  long mBoxBackColor;
  int  mBoxLineStyle;
  int  mBoxLineWidth;
};

class iupPlotTitle
{
public:
  iupPlotTitle() 
    : mColor(CD_BLACK), mText(NULL), mFontSize(0), mFontStyle(-1), 
      mAutoPos(true), mPosX(0), mPosY(0) {}
  ~iupPlotTitle() { if (mText) free(mText); }

  void SetText(const char* inValue) { if (mText) free(mText); mText = iupStrDup(inValue); }
  const char* GetText() const { return mText; }

  long mColor;
  int mFontSize;
  int mFontStyle;
  bool mAutoPos;
  int mPosX, mPosY;

protected:
  char* mText;
};

class iupPlotBackground
{
public:
  iupPlotBackground()
    : mColor(CD_WHITE), mMarginAuto(1, 1, 1, 1), mImage(NULL) {}
  ~iupPlotBackground() { if (mImage) free(mImage); }

  void SetImage(const char* inValue) { if (mImage) free(mImage); mImage = iupStrDup(inValue); }
  const char* GetImage() const { return mImage; }

  iupPlotMargin mMargin, 
                mMarginAuto;
  long mColor;
  double mImageMinX, 
         mImageMaxX, 
         mImageMinY, 
         mImageMaxY;

protected:
  char* mImage;
};

#define IUP_PLOT_MAX_DS 20

class iupPlot
{
public:
  iupPlot(Ihandle* ih, int inDefaultFontStyle, int inDefaultFontSize);
  ~iupPlot();

  /*********************************/

  Ihandle* ih;
  bool mRedraw;
  iupPlotRect mViewport;
  bool mViewportSquare;
  int mDefaultFontSize;
  int mDefaultFontStyle;

  void SetViewport(int x, int y, int w, int h);
  bool Render(cdCanvas* canvas);
  void ConfigureAxis();
  void SetFont(cdCanvas* canvas, int inFontStyle, int inFontSize) const;

  /*********************************/

  void DrawTitle(cdCanvas* canvas) const;
    void SetTitleFont(cdCanvas* canvas) const;
  void DrawBackground(cdCanvas* canvas) const;
  void DrawBackgroundImage(cdCanvas* canvas) const;
  bool DrawLegend(const iupPlotRect &inRect, cdCanvas* canvas, iupPlotRect &ioPos) const;
  void DrawCrossHairH(const iupPlotRect &inRect, cdCanvas* canvas) const;
    void DrawCrossSamplesH(const iupPlotRect &inRect, const iupPlotDataBase *inXData, const iupPlotDataBase *inYData, cdCanvas* canvas) const;
  void DrawCrossHairV(const iupPlotRect &inRect, cdCanvas* canvas) const;
    void DrawCrossSamplesV(const iupPlotRect &inRect, const iupPlotDataBase *inXData, const iupPlotDataBase *inYData, cdCanvas* canvas) const;
  void DrawInactive(cdCanvas* canvas) const;

  /*********************************/

  void CalculateTitlePos();
  bool CheckInsideTitle(cdCanvas* canvas, int x, int y);
  bool CheckInsideLegend(cdCanvas* canvas, int x, int y);
  void CalculateMargins(cdCanvas* canvas);
  bool CalculateAxisRange();
    bool CalculateXRange(double &outXMin, double &outXMax);
    bool CalculateYRange(double &outYMin, double &outYMax);
  bool CheckRange(const iupPlotAxis &inAxis);
  bool CalculateYTransformation(const iupPlotRect &inRect);
  bool CalculateXTransformation(const iupPlotRect &inRect);
    bool CalculateLinTransformation(int inBegin, int inEnd, const iupPlotAxis& inAxis, iupPlotTrafoLinear* outTrafo);
    bool CalculateLogTransformation(int inBegin, int inEnd, const iupPlotAxis& inAxis, iupPlotTrafoLog* outTrafo);
  bool CalculateTickSpacing(const iupPlotRect &inRect, cdCanvas* canvas);
  void CalculateTickSize(cdCanvas* canvas, iupPlotTick &ioTick);

  /*********************************/

  bool HasZoom() const { return mAxisX.HasZoom() || mAxisY.HasZoom(); }
  void ResetZoom() { if (mAxisX.ResetZoom()) mRedraw = true; if (mAxisY.ResetZoom()) mRedraw = true; }
  void ZoomIn(double inCenterX, double inCenterY) { if (mAxisX.ZoomIn(inCenterX)) mRedraw = true; if (mAxisY.ZoomIn(inCenterY)) mRedraw = true; }
  void ZoomOut(double inCenterX, double inCenterY) { if (mAxisX.ZoomOut(inCenterX)) mRedraw = true; if (mAxisY.ZoomOut(inCenterY)) mRedraw = true; }
  void ZoomTo(double inMinX, double inMaxX, double inMinY, double inMaxY) { if (mAxisX.ZoomTo(inMinX, inMaxX)) mRedraw = true; if (mAxisY.ZoomTo(inMinY, inMaxY)) mRedraw = true; }
  void PanStart() { mAxisX.PanStart(); mAxisY.PanStart(); }
  void Pan(double inOffsetX, double inOffsetY) { if (mAxisX.Pan(inOffsetX)) mRedraw = true; if (mAxisY.Pan(inOffsetY)) mRedraw = true; }
  void Scroll(double inDelta, bool inFullPage, bool inVertical) { if (inVertical) { if (mAxisY.Scroll(inDelta, inFullPage)) mRedraw = true; } else { if (mAxisX.Scroll(inDelta, inFullPage)) mRedraw = true; } }
  void ScrollTo(double inMinX, double inMinY) { if (mAxisX.ScrollTo(inMinX)) mRedraw = true; if (mAxisY.ScrollTo(inMinY)) mRedraw = true; }

  void TransformBack(int inX, int inY, double &outX, double &outY) { outX = mAxisX.mTrafo->TransformBack((double)inX);
                                                                     outY = mAxisY.mTrafo->TransformBack((double)inY); }

  /*********************************/

  bool mCrossHairH, mCrossHairV;
  int mCrossHairX, mCrossHairY;

  bool mShowSelectionBand;
  iupPlotRect mSelectionBand;

  /*********************************/

  iupPlotBackground mBack;
  iupPlotGrid mGrid;
  iupPlotGrid mGridMinor;
  iupPlotAxis mAxisX;
  iupPlotAxis mAxisY;
  iupPlotBox mBox;
  iupPlotLegend mLegend;
  iupPlotTitle mTitle;

  /***********************************/

  iupPlotDataSet* mDataSetList[IUP_PLOT_MAX_DS];
  int mDataSetListCount;
  int mCurrentDataSet;

  void AddDataSet(iupPlotDataSet* inDataSet);
  void RemoveDataSet(int inIndex);
  int FindDataSet(const char* inName);
  void RemoveAllDataSets();
  long GetNextDataSetColor();

  bool FindDataSetSample(double inX, double inY, int &outIndex, const char* &outName, int &outSampleIndex, double &outX, double &outY, const char* &outStrX) const;
  void SelectDataSetSamples(double inMinX, double inMaxX, double inMinY, double inMaxY);
  void DeleteSelectedDataSetSamples();
  void ClearDataSetSelection();
};

#endif
