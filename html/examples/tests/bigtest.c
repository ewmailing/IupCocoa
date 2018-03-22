#if 0
/* To check for memory leaks */
#define VLD_MAX_DATA_DUMP 80
#include <vld.h>
#endif

/* When using valgrind in UNIX to check the GTK driver:
export G_DEBUG=gc-friendly
export G_SLICE=always-malloc
*/

#include <stdlib.h>
#include <stdio.h>
#include <locale.h>

#include "iup.h"
#include "iupcontrols.h"
#include "iupkey.h"

/* Must define BIG_TEST on the Project and include in the build all test files */
#ifdef BIG_TEST
void ButtonTest(void);
void CanvasScrollbarTest(void);
void CanvasTest(void);
void CanvasCDDBufferTest(void);
void CanvasCDSimpleTest(void);
void DialogTest(void);
void FlatButtonTest(void);
void FrameTest(void);
void FlatFrameTest(void);
void TabsTest(void);
void SysInfoTest(void);
void GetColorTest(void);
#ifdef USE_OPENGL
void GLCanvasTest(void);
void GLCanvasCubeTest(void);
#endif
void HboxTest(void);
void IdleTest(void);
void LabelTest(void);
void FlatLabelTest(void);
void ListTest(void);
void MatrixTest(void);
void MatrixCbModeTest(void);
void MatrixCbsTest(void);
void MdiTest(void);
void VboxTest(void);
void GridBoxTest(void);
void MenuTest(void);
void ColorBrowserTest(void);
void ColorbarTest(void);
void CellsNumberingTest(void);
void DialTest(void);
void GaugeTest(void);
void CellsDegradeTest(void);
void CellsCheckboardTest(void);
void ValTest(void);
void TreeTest(void);
void TrayTest(void);
void ToggleTest(void);
void TimerTest(void);
void TextSpinTest(void);
void TextTest(void);
void SpinTest(void);
void SampleTest(void);
void FlatSampleTest(void);
void ProgressbarTest(void);
void ProgressDlgTest(void);
void PreDialogsTest(void);
void PlotTest(void);
#ifdef MGLPLOT_TEST
void MglPlotTest(void);
#endif
void GetParamTest(void);
void ClassInfo(void);
void ZboxTest(void);
void ScanfTest(void);
void SboxTest(void);
void ScrollBoxTest(void);
void FlatScrollBoxTest(void);
void SplitTest(void);
void ClipboardTest(void);
void CharacTest(void);
void LinkTest(void);
void ExpanderTest(void);
void CalendarTest(void);
void ScintillaTest(void);
void DetachBoxTest(void);
void CboxTest(void);
void WebBrowserTest(void);
void MglLabelTest(void);
void MatrixListTest(void);
void MglPlotModesTest(void);
void FlatTabsTest(void);


void HelpTest(void)
{
  IupHelp("http://www.tecgraf.puc-rio.br/iup");
  //IupHelp("..\\html\\index.html");
}

int IupExecuteWait(const char *filename, const char* parameters);

void ExecuteTest(void)
{
  IupExecute("zip", "-D test.zip *");
//  IupExecute("MSPaint", "");
}

void ExecuteWaitTest(void)
{
  IupExecuteWait("zip", "-D test.zip *");
//  IupExecuteWait("MSPaint", "");
}

typedef struct _TestItems{
  char* title;
  void (*func)(void);
}TestItems;

static TestItems test_list[] = {
  {"Button", ButtonTest},
  { "FlatButton", FlatButtonTest },
  { "Calendar", CalendarTest },
  {"Canvas", CanvasTest},
  {"CanvasCDSimpleTest", CanvasCDSimpleTest},
  {"CanvasCDDBuffer", CanvasCDDBufferTest},
  {"CanvasScrollbar", CanvasScrollbarTest},
  { "Cbox", CboxTest },
  { "CellsCheckboard", CellsCheckboardTest },
  {"CellsDegrade", CellsDegradeTest},
  {"CellsNumbering", CellsNumberingTest},
  {"CharacTest", CharacTest},
  {"Classes Info", ClassInfo},
  {"Clipboard", ClipboardTest},
  {"ColorBrowser", ColorBrowserTest},
  {"Colorbar", ColorbarTest},
  { "DetachBox", DetachBoxTest },
  { "Dial", DialTest },
  {"Dialog", DialogTest},
  { "Execute", ExecuteTest },
  { "ExecuteWait", ExecuteWaitTest },
  { "Expander", ExpanderTest },
  { "Frame", FrameTest },
  { "FlatFrame", FlatFrameTest },
#ifdef USE_OPENGL
  {"GLCanvas", GLCanvasTest},
  {"GLCanvasCube", GLCanvasCubeTest},
#endif
  {"Gauge", GaugeTest},
  {"GetColor", GetColorTest},
  {"GetParam", GetParamTest},
  {"GridBox", GridBoxTest},
  {"Help", HelpTest},
  {"Hbox", HboxTest},
  {"Idle", IdleTest},
  {"Label", LabelTest},
  { "FlatLabel", FlatLabelTest },
  { "Link", LinkTest },
  {"List", ListTest},
  {"Matrix", MatrixTest},
  {"MatrixCbMode", MatrixCbModeTest},
  {"MatrixCbs", MatrixCbsTest},
  { "MatrixList", MatrixListTest },
  { "Mdi", MdiTest },
  {"Menu", MenuTest},
#ifdef MGLPLOT_TEST
  {"MglPlot", MglPlotTest},
  { "MglPlotModes", MglPlotModesTest },
  { "MglLabel", MglLabelTest },
#endif
  { "Plot", PlotTest },
  {"PreDialogs", PreDialogsTest},
  {"Progressbar", ProgressbarTest},
  {"ProgressDlg", ProgressDlgTest},
  {"Sample", SampleTest},
  { "FlatSample", FlatSampleTest },
  { "Sbox", SboxTest },
  { "Scintilla", ScintillaTest },
  {"ScrollBox", ScrollBoxTest},
  { "FlatScrollBox", FlatScrollBoxTest },
  { "Split", SplitTest },
  {"Scanf", ScanfTest},
  {"Spin", SpinTest},
  {"SysInfo", SysInfoTest},
  {"Tabs", TabsTest},
  { "FlatTabs", FlatTabsTest },
  { "Text", TextTest },
  {"TextSpin", TextSpinTest},
  {"Timer", TimerTest},
  {"Toggle", ToggleTest},
  {"Tray", TrayTest},
  {"Tree", TreeTest},
  {"Val", ValTest},
  {"Vbox", VboxTest},
  {"Zbox", ZboxTest},
  { "WebBrowser", WebBrowserTest },
};

static int k_enter_cb(Ihandle*ih)
{
  int pos = IupGetInt(ih, "VALUE");
  if (pos > 0)
    test_list[pos-1].func();
  return IUP_DEFAULT;
}

static int dblclick_cb(Ihandle *ih, int item, char *text)
{
  (void)text;
  test_list[item-1].func();
  return IUP_DEFAULT;
}

static int close_cb(Ihandle *ih)
{
  (void)ih;
  return IUP_CLOSE;
}

int main(int argc, char* argv[])
{
  int i, count = sizeof(test_list)/sizeof(TestItems);
  char str[50];
  Ihandle *dlg, *list;

  IupOpen(&argc, &argv);
  IupControlsOpen();
  IupImageLibOpen();

  IupSetGlobal("GLOBALLAYOUTDLGKEY", "Yes");
  IupSetGlobal("GLOBALLAYOUTRESIZEKEY", "Yes");

//  IupSetGlobal("OVERLAYSCROLLBAR", "Yes");
//  IupSetGlobal("UTF8MODE", "Yes");
//  IupSetGlobal("LANGUAGE", "PORTUGUESE");
//  IupSetGlobal("DEFAULTDECIMALSYMBOL", ".");
  //  setlocale(LC_NUMERIC, "C");
//  printf("EXEFILENAME=%s\n", IupGetGlobal("EXEFILENAME"));
//  IupLog("DEBUG", "IUP version %s\n", IupVersion());
//  IupLog("INFO", "IUP version %s", IupVersion());

  dlg = IupDialog(IupVbox(list = IupList(NULL), NULL));
  IupSetAttribute(dlg, "MARGIN", "10x10");
  IupSetAttribute(dlg, "TITLE", "IupTests");
  IupSetCallback(dlg, "CLOSE_CB", close_cb);
//  IupSetAttribute(dlg, "ICON", "SIZE_ICON");
  IupSetHandle("BIGTEST", dlg);

  IupSetAttribute(list, "VISIBLELINES", "15");
  IupSetAttribute(list, "EXPAND", "YES");
  IupSetCallback(list, "DBLCLICK_CB", (Icallback)dblclick_cb);
  IupSetCallback(list, "K_CR", k_enter_cb);

  for (i=0; i<count; i++)
  {
    sprintf(str, "%d", i+1);
    IupSetAttribute(list, str, test_list[i].title);
  }

  IupShowXY(dlg, 100, IUP_CENTER);

  IupMainLoop();

  IupClose();

  return EXIT_SUCCESS;
}
#endif
