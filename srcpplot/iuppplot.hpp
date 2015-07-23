/*
 * IupPPlot component - private include file
 *
 * Description : A component, derived from PPlot and IUP canvas
 *      Remark : Depend on libs IUP, CD, IUPCD
 */


#ifndef __IUP_PPLOT_HPP
#define __IUP_PPLOT_HPP

/*
  PPainterIup class definition
*/

class PostPainterCallbackIup: public PDrawer
{
public:
  PostPainterCallbackIup(PPlot &inPPlot, Ihandle* inHandle);
  virtual bool Draw (Painter &inPainter);
protected:
  Ihandle *_ih;   // IUP handle
};

class PrePainterCallbackIup: public PDrawer
{
public:
  PrePainterCallbackIup(PPlot &inPPlot, Ihandle* inHandle);
  virtual bool Draw (Painter &inPainter);
protected:
  Ihandle *_ih;   // IUP handle
};

class PDeleteInteractionIup: public PDeleteInteraction
{
public:
  PDeleteInteractionIup(PPlot &inPPlot, Ihandle* inHandle)
    :PDeleteInteraction(inPPlot), _ih(inHandle) {}

protected:
  Ihandle *_ih;   // IUP handle
  bool DeleteNotify(int inIndex, int inSampleIndex, PlotDataBase *inXData, PlotDataBase *inYData);
};

class PSelectionInteractionIup: public PSelectionInteraction
{
public:
  PSelectionInteractionIup(PPlot &inPPlot, Ihandle* inHandle)
    :PSelectionInteraction(inPPlot), _ih(inHandle) {}

protected:
  Ihandle *_ih;   // IUP handle
  bool SelectNotify(int inIndex, int inSampleIndex, PlotDataBase *inXData, PlotDataBase *inYData, bool inSelect);
};

class PEditInteractionIup: public PEditInteraction
{
public:
  PEditInteractionIup (PPlot &inPPlot, Ihandle* inHandle)
    :PEditInteraction(inPPlot), _ih(inHandle) {}

  virtual bool Impl_HandleKeyEvent (const PKeyEvent &inEvent);
  virtual bool Impl_Calculate (Painter &inPainter, PPlot& inPPlot);

protected:
  Ihandle *_ih;   // IUP handle
  void HandleCursorKey (const PlotDataSelection *inPlotDataSelection, PlotDataBase *inXData, PlotDataBase *inYData, int inIndex);
  bool EditNotify(int inIndex, int inSampleIndex, float inX, float inY, float *inNewX, float *inNewY);
};

class InteractionContainerIup: public InteractionContainer
{
public:
  InteractionContainerIup(PPlot &inPPlot, Ihandle* inHandle);

  PZoomInteraction mZoomInteraction;
  PSelectionInteractionIup mSelectionInteraction;
  PEditInteractionIup mEditInteraction;
  PDeleteInteractionIup mDeleteInteraction;
  PCrosshairInteraction mCrosshairInteraction;

  PostPainterCallbackIup mPostPainterCallback;
  PrePainterCallbackIup mPrePainterCallback;
};

class PPainterIup: public Painter
{
public:
  PPainterIup(Ihandle *ih);
  virtual ~PPainterIup();

  PPlot _plot;               /* plot data holder */
  int _redraw;               /* must update the double buffer before flush */
  int _currentDataSetIndex;  /* dataset index used for DS_ attributes in SetAttribute and GetAttribute */
  int _x, _y, _width, _height;

  // Called from C functions
  void Paint(int force, int flush);   // paint the stuff
  void SetSize(int x, int y, int w, int h); // called when resizing
  void UpdateViewport();
  void MouseButton(int btn, int stat, int x, int y, char *r); // mouse event
  void MouseMove(int x, int y); // mouse event
  void MouseWheel(float delta, char *r); // mouse event
  void KeyPress(int c, int press); // keyboard event

protected:
  InteractionContainerIup *_InteractionContainer;
  Ihandle *_ih;   // IUP handle
  short int _mouseDown;
  short int _mouse_ALT;
  short int _mouse_SHIFT;
  short int _mouse_CTRL;

  PPainterIup() { };

  void Pan(float delta, bool vertical);
  void FillArrow(int inX1, int inY1, int inX2, int inY2, int inX3, int inY3);
  void DrawLine(float inX1, float inY1, float inX2, float inY2);
  void FillRect(int inX, int inY, int inW, int inH);
  void SetClipRect(int inX, int inY, int inW, int inH);
  long GetWidth() const { return _width; }
  long GetHeight() const { return _height; }
  void SetLineColor(int inR, int inG, int inB);
  void SetFillColor(int inR, int inG, int inB);
  long CalculateTextDrawSize(const char *inString);
  long GetFontHeight() const;
  void DrawText(int inX, int inY, short align, const char *inString);
  void DrawRotatedText(int inX, int inY, float inDegrees,
                        short align, const char *inString);
  void SetStyle(const PStyle &inStyle);
  void BeginArea();
  void AddVertex(float inX, float inY);
  void EndArea();

}; // PPainterIup


#endif /* #ifndef __IUP_PPLOT_HPP */
