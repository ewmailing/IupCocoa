/*
* IupScintilla component
*
* Description : A source code editing component, 
* derived from Scintilla (http://www.scintilla.org/)
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "Scintilla.h"
#include "SciLexer.h"

#include <gtk/gtk.h>
#include <ScintillaWidget.h>

#include "iup.h"
#include "iup_scintilla.h"
#include "iupcbs.h"
#include "iup_key.h"

#include "iup_class.h"
#include "iup_object.h"
#include "iup_attrib.h"
#include "iup_str.h"
#include "iup_drv.h"
#include "iup_drvfont.h"
#include "iup_register.h"
#include "iup_layout.h"
#include "iup_assert.h"
#include "iup_array.h"

#include "iupgtk_drv.h"

#include "iupsci.h"


void iupdrvScintillaRefreshCaret(Ihandle* ih)
{
  (void)ih;
}

int iupdrvScintillaGetBorder(void)
{
  int border_size = 2 * 5;
  return border_size;
}

void iupdrvScintillaOpen(void)
{
}

sptr_t IupScintillaSendMessage(Ihandle* ih, unsigned int iMessage, uptr_t wParam, sptr_t lParam)
{
  return scintilla_send_message(SCINTILLA(ih->handle), iMessage, wParam, lParam);
}

static void gtkScintillaNotify(GtkWidget *w, gint wp, gpointer lp, Ihandle *ih)
{
  SCNotification *pMsg =(SCNotification*)lp;

  iupScintillaNotify(ih, pMsg);

  (void)w;
  (void)wp;
}

static gboolean gtkScintillaKeyReleaseEvent(GtkWidget *widget, GdkEventKey *evt, Ihandle *ih)
{
  iupScintillaCallCaretCb(ih);
  (void)widget;
  (void)evt;
  return FALSE;
}

static gboolean gtkScintillaButtonEvent(GtkWidget *widget, GdkEventButton *evt, Ihandle *ih)
{
  iupScintillaCallCaretCb(ih);
  return iupgtkButtonEvent(widget, evt, ih);
}

static void gtkScintillaSetupFormat(struct Sci_RangeToFormat *frPrint, GtkPrintContext *context, Ihandle* ih)
{
  int margin_left, margin_top, margin_right, margin_bottom, margin_units,
      page_width, page_height;
  double dpi;
  cairo_t *cr = gtk_print_context_get_cairo_context(context);

  frPrint->hdc = cr;
  frPrint->hdcTarget = cr;

  page_width = (int)gtk_print_context_get_width(context);
  page_height = (int)gtk_print_context_get_height(context);
  dpi = gtk_print_context_get_dpi_y(context);

  margin_units = iupSciGetPrintMarginUnits(ih);
  margin_left = iupSciGetPrintMargin(ih, "PRINTMARGINLEFT", margin_units, dpi);
  margin_top = iupSciGetPrintMargin(ih, "PRINTMARGINTOP", margin_units, dpi);
  margin_right = iupSciGetPrintMargin(ih, "PRINTMARGINRIGHT", margin_units, dpi);
  margin_bottom = iupSciGetPrintMargin(ih, "PRINTMARGINBOTTOM", margin_units, dpi);

  frPrint->rc.left = margin_left;
  frPrint->rc.top = margin_top;
  frPrint->rc.right = page_width - margin_right;
  frPrint->rc.bottom = page_height - margin_bottom;
  frPrint->rcPage.left = 0;
  frPrint->rcPage.top = 0;
  frPrint->rcPage.right = page_width;
  frPrint->rcPage.bottom = page_height;
}

static void gtkScintillaBeginPrint(GtkPrintOperation *operation, GtkPrintContext *context, Ihandle* ih)
{
  int lengthPrinted, lengthDoc, p, lengthDocMax;
  struct Sci_RangeToFormat frPrint;
  Iarray* page_starts_array = (Iarray*)iupAttribGet(ih, "_IUP_PAGE_ARRAY");
  int* page_starts;
  int startPos = 0, endPos = 0;

  IupGetIntInt(ih, "SELECTIONPOS", &startPos, &endPos);

  gtkScintillaSetupFormat(&frPrint, context, ih);

  lengthDoc = (int)IupScintillaSendMessage(ih, SCI_GETLENGTH, 0, 0);
  lengthPrinted = 0;
  lengthDocMax = lengthDoc;

#if GTK_CHECK_VERSION(2, 18, 0)
  if (startPos != endPos && gtk_print_operation_get_has_selection(operation))
  {
    if (startPos > endPos)
    {
      lengthPrinted = endPos;
      lengthDoc = startPos;
    }
    else
    {
      lengthPrinted = startPos;
      lengthDoc = endPos;
    }

    if (lengthPrinted < 0)
      lengthPrinted = 0;
    if (lengthDoc > lengthDocMax)
      lengthDoc = lengthDocMax;
  }
#endif

  /* Pre-process each page */
  p = 0;  /* page_nr starts at 0 */
  while (lengthPrinted < lengthDoc)
  {
    page_starts = (int*)iupArrayInc(page_starts_array);
    page_starts[p] = lengthPrinted;

    frPrint.chrg.cpMin = lengthPrinted;
    frPrint.chrg.cpMax = lengthDoc;
    lengthPrinted = (int)IupScintillaSendMessage(ih, SCI_FORMATRANGE, 0, (sptr_t)&frPrint);
    p++;
  }

  page_starts = (int*)iupArrayInc(page_starts_array);
  page_starts[p] = lengthPrinted;

  gtk_print_operation_set_n_pages(operation, p);
}

static void gtkScintillaDrawPage(GtkPrintOperation *operation, GtkPrintContext *context, gint page_nr, Ihandle* ih)
{
  int lengthDoc;
  struct Sci_RangeToFormat frPrint;
  Iarray* page_starts_array = (Iarray*)iupAttribGet(ih, "_IUP_PAGE_ARRAY");
  int* page_starts = (int*)iupArrayGetData(page_starts_array);

  gtkScintillaSetupFormat(&frPrint, context, ih);

  lengthDoc = (int)IupScintillaSendMessage(ih, SCI_GETLENGTH, 0, 0);
  frPrint.chrg.cpMin = page_starts[page_nr];
  frPrint.chrg.cpMax = page_starts[page_nr + 1];
  if (frPrint.chrg.cpMax < lengthDoc)
    frPrint.chrg.cpMax--;

  IupScintillaSendMessage(ih, SCI_FORMATRANGE, 1, (sptr_t)&frPrint);
  (void)operation;
}

int iupdrvScintillaPrintAttrib(Ihandle* ih, const char* value)
{
  GtkPrintOperation *printOp = gtk_print_operation_new();
  Iarray* page_starts_array = iupArrayCreate(10, sizeof(int));
  int startPos = 0, endPos = 0;

  iupAttribSet(ih, "_IUP_PAGE_ARRAY", (char*)page_starts_array);
  g_signal_connect(printOp, "begin_print", G_CALLBACK(gtkScintillaBeginPrint), ih);
  g_signal_connect(printOp, "draw_page", G_CALLBACK(gtkScintillaDrawPage), ih);

  gtk_print_operation_set_job_name(printOp, iupgtkStrConvertToSystem(value));

  /* See if a range has been selected */
  IupGetIntInt(ih, "SELECTIONPOS", &startPos, &endPos);

#if GTK_CHECK_VERSION(2, 18, 0)
  if (startPos == endPos)
    gtk_print_operation_set_support_selection(printOp, FALSE);
  else
  {
    gtk_print_operation_set_support_selection(printOp, TRUE);
    gtk_print_operation_set_has_selection(printOp, TRUE);
  }
#endif

  if (!iupAttribGetBoolean(ih, "PRINTDIALOG"))
    gtk_print_operation_run(printOp, GTK_PRINT_OPERATION_ACTION_PRINT, GTK_WINDOW(IupGetDialog(ih)->handle), NULL);
  else
    gtk_print_operation_run(printOp, GTK_PRINT_OPERATION_ACTION_PRINT_DIALOG, GTK_WINDOW(IupGetDialog(ih)->handle), NULL);

  iupAttribSet(ih, "_IUP_PAGE_ARRAY", NULL);
  iupArrayDestroy(page_starts_array);
  g_object_unref(printOp);
  return 0;
}


/*****************************************************************************/


int idrvScintillaMap(Ihandle* ih)
{
  ih->handle = scintilla_new();
  if (!ih->handle)
    return IUP_ERROR;

  gtk_widget_show(ih->handle);

  /* add to the parent, all GTK controls must call this. */
  iupgtkAddToParent(ih);

  if (!iupAttribGetBoolean(ih, "CANFOCUS"))
    iupgtkSetCanFocus(ih->handle, 0);

  g_signal_connect(G_OBJECT(ih->handle), "enter-notify-event", G_CALLBACK(iupgtkEnterLeaveEvent), ih);
  g_signal_connect(G_OBJECT(ih->handle), "leave-notify-event", G_CALLBACK(iupgtkEnterLeaveEvent), ih);
  g_signal_connect(G_OBJECT(ih->handle), "focus-in-event",     G_CALLBACK(iupgtkFocusInOutEvent), ih);
  g_signal_connect(G_OBJECT(ih->handle), "focus-out-event",    G_CALLBACK(iupgtkFocusInOutEvent), ih);
  g_signal_connect(G_OBJECT(ih->handle), "key-press-event",    G_CALLBACK(iupgtkKeyPressEvent), ih);
  g_signal_connect(G_OBJECT(ih->handle), "show-help",          G_CALLBACK(iupgtkShowHelp), ih);

  g_signal_connect_after(G_OBJECT(ih->handle), "key-release-event", G_CALLBACK(gtkScintillaKeyReleaseEvent), ih);
  g_signal_connect(G_OBJECT(ih->handle), "button-press-event", G_CALLBACK(gtkScintillaButtonEvent), ih);  /* if connected "after" then it is ignored */
  g_signal_connect(G_OBJECT(ih->handle), "button-release-event", G_CALLBACK(gtkScintillaButtonEvent), ih);
  g_signal_connect(G_OBJECT(ih->handle), "motion-notify-event", G_CALLBACK(iupgtkMotionNotifyEvent), ih);

  g_signal_connect(G_OBJECT(ih->handle), "sci-notify", G_CALLBACK(gtkScintillaNotify), ih);

  gtk_widget_realize(ih->handle);

  return IUP_NOERROR;
}

void iupdrvScintillaReleaseMethod(Iclass* ic)
{
  (void)ic;
}
